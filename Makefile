CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -D_GNU_SOURCE
LDFLAGS ?= -rdynamic
LDLIBS ?= -ldl -lm

TARGET := greenServe
OBJS := main.o lexer.o parser.o evaluator.o
MODULE_DIR ?= /usr/local/lib/greenServe/modules

.PHONY: all clean install uninstall module install-module module-install dirs

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

main.o: main.c lexer.h parser.h evaluator.h
lexer.o: lexer.c lexer.h
parser.o: parser.c parser.h lexer.h
evaluator.o: evaluator.c evaluator.h parser.h lexer.h

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	install -d /usr/local/bin
	install -m 0755 $(TARGET) /usr/local/bin/$(TARGET)
	install -d $(MODULE_DIR)
	@echo "greenServe installed to /usr/local/bin/$(TARGET)"
	@echo "C modules install to $(MODULE_DIR)"

uninstall:
	rm -f /usr/local/bin/$(TARGET)

module:
	@test -n "$(MODULE)" || (echo "Usage: make module MODULE=libserver.c"; exit 2)
	@mkdir -p build/modules
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. -fPIC -shared -o build/modules/$$(basename $(MODULE) .c).so $(MODULE)
	@echo "Built build/modules/$$(basename $(MODULE) .c).so"

install-module: module
	install -d $(MODULE_DIR)
	install -m 0755 build/modules/$$(basename $(MODULE) .c).so $(MODULE_DIR)/
	@echo "Installed module to $(MODULE_DIR)/$$(basename $(MODULE) .c).so"

module-install: install-module

dirs:
	install -d /usr/local/bin $(MODULE_DIR)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf build
