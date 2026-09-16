# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Makefile

```makefile
GREEN_SERVE_VERSION := v1.0.1
GREEN_SERVE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip
INSTALL_DIR := /usr/local/bin
TMP_DIR := /tmp/greenServe-install
ZIP_FILE := $(TMP_DIR)/greenServe-linux-x86_64.zip

.PHONY: install uninstall clean

install:
	@set -e; \
	command -v curl >/dev/null 2>&1 || { echo "curl is required"; exit 1; }; \
	command -v unzip >/dev/null 2>&1 || { echo "unzip is required"; exit 1; }; \
	mkdir -p "$(TMP_DIR)"; \
	curl -L "$(GREEN_SERVE_URL)" -o "$(ZIP_FILE)"; \
	unzip -o "$(ZIP_FILE)" -d "$(TMP_DIR)/extracted"; \
	BINARY=$$(find "$(TMP_DIR)/extracted" -type f -name greenServe -print -quit); \
	if [ -z "$$BINARY" ]; then echo "greenServe binary was not found in the release archive"; exit 1; fi; \
	install -m 755 "$$BINARY" "$(INSTALL_DIR)/greenServe"; \
	rm -rf "$(TMP_DIR)"; \
	echo "greenServe installed to $(INSTALL_DIR)/greenServe"; \
	"$(INSTALL_DIR)/greenServe" --version

uninstall:
	rm -f "$(INSTALL_DIR)/greenServe"
	echo "greenServe removed from $(INSTALL_DIR)"

clean:
	rm -rf "$(TMP_DIR)"
```

### Install

Save the Makefile above as `Makefile`, then run:

```bash
sudo make install
```

The Makefile downloads:

```text
https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip
```

It extracts the release and installs the executable as:

```text
/usr/local/bin/greenServe
```

Because `/usr/local/bin` is normally included in the Linux system `PATH`, `greenServe` can then be run from any directory without specifying its installation path.

### Run greenServe

After installation:

```bash
greenServe filename.gsve
```

For example:

```bash
cd my-server

greenServe app.gsve
```

You do not need to copy `greenServe` into every project directory.

### Check installation

```bash
greenServe --version
```

To see where the command is installed:

```bash
which greenServe
```

Expected location:

```text
/usr/local/bin/greenServe
```

### Uninstall

Use the same Makefile:

```bash
sudo make uninstall
```

This removes the system-wide `greenServe` executable.

## Release

Current Linux release:

- Version: `greenServe-v1.0.1`
- Platform: Linux x86_64
- Archive: `greenServe-linux-x86_64.zip`

The installation process is intentionally kept inside the Makefile so users only need to run `sudo make install` to download, extract, install, and verify greenServe.
