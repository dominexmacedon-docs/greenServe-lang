#include "evaluator.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_BUFFER 16384
#define SERVER_MAX_BODY 1048576

static char *dup_text(const char *s) {
    size_t n = strlen(s ? s : "");
    char *p = malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s ? s : "", n + 1);
    return p;
}

static GSValue response_value(int status, const char *body, const char *content_type) {
    GSValue out = gs_object();
    gs_object_set(&out, "status", gs_number(status));
    gs_object_set(&out, "body", gs_string(body ? body : ""));
    gs_object_set(&out, "contentType", gs_string(content_type ? content_type : "text/plain; charset=utf-8"));
    return out;
}

static const char *mime_type(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "text/plain; charset=utf-8";
    if (!strcmp(dot, ".html") || !strcmp(dot, ".htm")) return "text/html; charset=utf-8";
    if (!strcmp(dot, ".css")) return "text/css; charset=utf-8";
    if (!strcmp(dot, ".js")) return "application/javascript; charset=utf-8";
    if (!strcmp(dot, ".json")) return "application/json; charset=utf-8";
    if (!strcmp(dot, ".txt")) return "text/plain; charset=utf-8";
    if (!strcmp(dot, ".svg")) return "image/svg+xml";
    if (!strcmp(dot, ".png")) return "image/png";
    if (!strcmp(dot, ".jpg") || !strcmp(dot, ".jpeg")) return "image/jpeg";
    if (!strcmp(dot, ".gif")) return "image/gif";
    if (!strcmp(dot, ".ico")) return "image/x-icon";
    return "application/octet-stream";
}

static char *read_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0 || size > SERVER_MAX_BODY) { fclose(f); return NULL; }
    rewind(f);
    char *data = malloc((size_t)size + 1);
    if (!data) { fclose(f); return NULL; }
    size_t got = fread(data, 1, (size_t)size, f);
    fclose(f);
    data[got] = '\0';
    if (out_len) *out_len = got;
    return data;
}

static char *render_text(const char *source, GSValue data) {
    if (!source) return dup_text("");
    if (data.type != GS_OBJECT) return dup_text(source);

    size_t capacity = strlen(source) + 1;
    char *out = malloc(capacity);
    if (!out) return NULL;
    out[0] = '\0';

    const char *p = source;
    while (*p) {
        const char *open = strstr(p, "{{");
        if (!open) {
            size_t need = strlen(out) + strlen(p) + 1;
            if (need > capacity) {
                capacity = need * 2;
                out = realloc(out, capacity);
                if (!out) return NULL;
            }
            strcat(out, p);
            break;
        }

        size_t prefix = (size_t)(open - p);
        size_t need = strlen(out) + prefix + 1;
        if (need > capacity) {
            capacity = need * 2;
            out = realloc(out, capacity);
            if (!out) return NULL;
        }
        strncat(out, p, prefix);

        const char *close = strstr(open + 2, "}}");
        if (!close) {
            strcat(out, open);
            break;
        }

        size_t key_len = (size_t)(close - (open + 2));
        char *key = malloc(key_len + 1);
        if (!key) { free(out); return NULL; }
        memcpy(key, open + 2, key_len);
        key[key_len] = '\0';

        char *start = key;
        while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') start++;
        char *end = start + strlen(start);
        while (end > start && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\n' || end[-1] == '\r')) end--;
        *end = '\0';

        GSValue value = gs_object_get(data, start);
        const char *replacement = gs_to_cstring(value);
        need = strlen(out) + strlen(replacement) + 1;
        if (need > capacity) {
            capacity = need * 2;
            out = realloc(out, capacity);
            if (!out) { free(key); return NULL; }
        }
        strcat(out, replacement);
        free(key);
        p = close + 2;
    }
    return out;
}

static int parse_url(const char *url, char **host, char **port, char **path) {
    if (!url || !*url) return 0;
    const char *start = url;
    if (!strncmp(start, "http://", 7)) start += 7;
    else if (!strncmp(start, "https://", 8)) return 0;
    else return 0;

    const char *slash = strchr(start, '/');
    const char *authority_end = slash ? slash : start + strlen(start);
    const char *colon = NULL;
    for (const char *p = start; p < authority_end; p++) {
        if (*p == ':') colon = p;
    }

    size_t host_len = (size_t)((colon ? colon : authority_end) - start);
    if (!host_len) return 0;
    *host = malloc(host_len + 1);
    if (!*host) return 0;
    memcpy(*host, start, host_len);
    (*host)[host_len] = '\0';

    if (colon) {
        size_t port_len = (size_t)(authority_end - colon - 1);
        *port = malloc(port_len + 1);
        if (!*port) { free(*host); *host = NULL; return 0; }
        memcpy(*port, colon + 1, port_len);
        (*port)[port_len] = '\0';
    } else {
        *port = dup_text("80");
    }

    *path = dup_text(slash ? slash : "/");
    if (!*path) { free(*host); free(*port); *host = NULL; *port = NULL; return 0; }
    return 1;
}

static GSValue http_request(const char *method, const char *url, const char *body, const char *content_type) {
    char *host = NULL;
    char *port = NULL;
    char *path = NULL;
    if (!parse_url(url, &host, &port, &path)) {
        return response_value(0, "Only plain http:// URLs are supported by libserver", "text/plain; charset=utf-8");
    }

    struct addrinfo hints;
    struct addrinfo *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host, port, &hints, &result);
    if (rc != 0) {
        free(host); free(port); free(path);
        return response_value(0, gai_strerror(rc), "text/plain; charset=utf-8");
    }

    int fd = -1;
    for (struct addrinfo *it = result; it; it = it->ai_next) {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, it->ai_addr, it->ai_addrlen) == 0) break;
        close(fd);
        fd = -1;
    }
    freeaddrinfo(result);

    if (fd < 0) {
        free(host); free(port); free(path);
        return response_value(0, "Could not connect to HTTP server", "text/plain; charset=utf-8");
    }

    size_t body_len = body ? strlen(body) : 0;
    size_t request_size = strlen(method) + strlen(path) + strlen(host) + body_len + 512;
    char *request = malloc(request_size);
    if (!request) {
        close(fd); free(host); free(port); free(path);
        return response_value(0, "Out of memory", "text/plain; charset=utf-8");
    }

    int written = snprintf(request, request_size,
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "User-Agent: greenServe-libserver/1.0\r\n"
        "%s%s"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        method, path, host,
        body_len ? "Content-Type: " : "",
        body_len ? (content_type ? content_type : "application/x-www-form-urlencoded") : "",
        body_len, body ? body : "");

    if (written < 0 || (size_t)written >= request_size) {
        free(request); close(fd); free(host); free(port); free(path);
        return response_value(0, "HTTP request was too large", "text/plain; charset=utf-8");
    }

    size_t sent = 0;
    while (sent < (size_t)written) {
        ssize_t n = send(fd, request + sent, (size_t)written - sent, 0);
        if (n <= 0) break;
        sent += (size_t)n;
    }
    free(request);
    free(host); free(port); free(path);

    if (sent == 0) {
        close(fd);
        return response_value(0, "Could not send HTTP request", "text/plain; charset=utf-8");
    }

    size_t capacity = SERVER_BUFFER;
    size_t used = 0;
    char *raw = malloc(capacity);
    if (!raw) { close(fd); return response_value(0, "Out of memory", "text/plain; charset=utf-8"); }

    for (;;) {
        if (used + 4096 + 1 > capacity) {
            if (capacity >= SERVER_MAX_BODY + SERVER_BUFFER) break;
            capacity *= 2;
            if (capacity > SERVER_MAX_BODY + SERVER_BUFFER) capacity = SERVER_MAX_BODY + SERVER_BUFFER;
            raw = realloc(raw, capacity);
            if (!raw) { close(fd); return response_value(0, "Out of memory", "text/plain; charset=utf-8"); }
        }
        ssize_t n = recv(fd, raw + used, capacity - used - 1, 0);
        if (n <= 0) break;
        used += (size_t)n;
        if (used >= SERVER_MAX_BODY) break;
    }
    close(fd);
    raw[used] = '\0';

    int status = 0;
    sscanf(raw, "HTTP/%*s %d", &status);
    char *separator = strstr(raw, "\r\n\r\n");
    const char *response_body = separator ? separator + 4 : raw;

    GSValue out = response_value(status, response_body, "text/plain; charset=utf-8");
    free(raw);
    return out;
}

static GSValue native_http_get(GSValue *args, int argc) {
    if (argc < 1) return response_value(0, "http_get requires a URL", "text/plain; charset=utf-8");
    return http_request("GET", gs_to_cstring(args[0]), NULL, NULL);
}

static GSValue native_http_post(GSValue *args, int argc) {
    if (argc < 1) return response_value(0, "http_post requires a URL", "text/plain; charset=utf-8");
    const char *body = argc > 1 ? gs_to_cstring(args[1]) : "";
    const char *type = argc > 2 ? gs_to_cstring(args[2]) : "application/json";
    return http_request("POST", gs_to_cstring(args[0]), body, type);
}

static GSValue native_render(GSValue *args, int argc) {
    if (argc < 1) return response_value(404, "Template path is required", "text/plain; charset=utf-8");
    size_t len = 0;
    char *source = read_file(gs_to_cstring(args[0]), &len);
    if (!source) return response_value(404, "Template file not found", "text/plain; charset=utf-8");
    GSValue data = argc > 1 ? args[1] : gs_object();
    char *html = render_text(source, data);
    free(source);
    if (!html) return response_value(500, "Could not render template", "text/plain; charset=utf-8");
    GSValue out = response_value(200, html, "text/html; charset=utf-8");
    free(html);
    return out;
}

static GSValue native_read_file(GSValue *args, int argc) {
    if (argc < 1) return gs_string("");
    char *data = read_file(gs_to_cstring(args[0]), NULL);
    if (!data) return gs_string("");
    GSValue out = gs_string(data);
    free(data);
    return out;
}

static void send_http(int fd, int status, const char *content_type, const char *body) {
    const char *reason = "OK";
    if (status == 404) reason = "Not Found";
    else if (status == 405) reason = "Method Not Allowed";
    else if (status >= 500) reason = "Internal Server Error";

    size_t len = strlen(body ? body : "");
    char header[512];
    int n = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "Server: greenServe/1.0\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n",
        status, reason, content_type ? content_type : "text/plain; charset=utf-8", len);
    if (n > 0) send(fd, header, (size_t)n, 0);
    if (len) send(fd, body, len, 0);
}

static void handle_client(int fd, const char *root) {
    char request[SERVER_BUFFER];
    ssize_t n = recv(fd, request, sizeof(request) - 1, 0);
    if (n <= 0) return;
    request[n] = '\0';

    char method[16];
    char path[4096];
    if (sscanf(request, "%15s %4095s", method, path) != 2) {
        send_http(fd, 400, "text/plain; charset=utf-8", "Bad Request");
        return;
    }

    if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
        send_http(fd, 405, "text/plain; charset=utf-8", "Method Not Allowed");
        return;
    }

    char *query = strchr(path, '?');
    if (query) *query = '\0';
    if (!strcmp(path, "/")) strcpy(path, "/index.html");
    if (strstr(path, "..")) {
        send_http(fd, 404, "text/plain; charset=utf-8", "Not Found");
        return;
    }

    char full[8192];
    snprintf(full, sizeof(full), "%s%s", root, path);
    size_t len = 0;
    char *data = read_file(full, &len);
    if (!data) {
        send_http(fd, 404, "text/plain; charset=utf-8", "Not Found");
        return;
    }

    const char *type = mime_type(full);
    send_http(fd, 200, type, strcmp(method, "HEAD") == 0 ? "" : data);
    free(data);
}

static GSValue native_serve(GSValue *args, int argc) {
    int port = argc > 0 ? (int)gs_to_number(args[0]) : 8080;
    const char *root = argc > 1 ? gs_to_cstring(args[1]) : ".";
    if (port < 1 || port > 65535) return response_value(0, "Invalid port", "text/plain; charset=utf-8");

    signal(SIGPIPE, SIG_IGN);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return response_value(0, strerror(errno), "text/plain; charset=utf-8");

    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 || listen(server_fd, 32) < 0) {
        const char *message = strerror(errno);
        close(server_fd);
        return response_value(0, message, "text/plain; charset=utf-8");
    }

    printf("greenServe HTTP server listening on http://127.0.0.1:%d\n", port);
    fflush(stdout);

    for (;;) {
        int client = accept(server_fd, NULL, NULL);
        if (client < 0) {
            if (errno == EINTR) continue;
            break;
        }
        handle_client(client, root);
        close(client);
    }

    close(server_fd);
    return gs_null();
}

static const GSNativeFunction server_functions[] = {
    {"http_get", native_http_get},
    {"http_post", native_http_post},
    {"render", native_render},
    {"read_file", native_read_file},
    {"serve", native_serve}
};

static GSModule server_module = {
    "server",
    server_functions,
    sizeof(server_functions) / sizeof(server_functions[0])
};

GSModule *green_module_init(void) {
    return &server_module;
}
