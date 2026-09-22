# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Makefile

```makefile
GREEN_SERVE_VERSION := v1.0.1

GREEN_SERVE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip

GREEN_SERVE_VSCODE_VERSION := v1.0.0

GREEN_SERVE_VSCODE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-vscode-extension-v1.0.0/greenServe-vscode-5f0270f875024058c4d7ea97ec5f466aa097ebd0.zip

INSTALL_DIR := /usr/local/bin
SHARE_DIR := /usr/local/share/greenServe

EXTENSION_ID := dominexmacedon.greenserve-language
EXTENSION_INSTALL_FILE := $(SHARE_DIR)/greenServe-vscode-extension.vsix

TMP_DIR := /tmp/greenServe-install

GREEN_SERVE_ZIP := $(TMP_DIR)/greenServe-linux-x86_64.zip

.PHONY: install install-language install-vscode uninstall clean

install:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Please run: sudo make install"; \
		exit 1; \
	fi; \
	command -v curl >/dev/null 2>&1 || { \
		echo "ERROR: curl is required."; \
		exit 1; \
	}; \
	command -v unzip >/dev/null 2>&1 || { \
		echo "ERROR: unzip is required."; \
		exit 1; \
	}; \
	command -v code >/dev/null 2>&1 || { \
		echo "ERROR: VS Code 'code' command was not found."; \
		exit 1; \
	}; \
	$(MAKE) install-language; \
	$(MAKE) install-vscode

install-language:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Please run: sudo make install-language"; \
		exit 1; \
	fi; \
	echo "==> Installing greenServe $(GREEN_SERVE_VERSION)..."; \
	mkdir -p "$(TMP_DIR)"; \
	rm -f "$(GREEN_SERVE_ZIP)"; \
	echo "==> Downloading greenServe..."; \
	curl -fL "$(GREEN_SERVE_URL)" -o "$(GREEN_SERVE_ZIP)"; \
	echo "==> Extracting greenServe..."; \
	rm -rf "$(TMP_DIR)/greenServe-extracted"; \
	mkdir -p "$(TMP_DIR)/greenServe-extracted"; \
	unzip -q "$(GREEN_SERVE_ZIP)" -d "$(TMP_DIR)/greenServe-extracted"; \
	GREEN_SERVE_BINARY="$$(find "$(TMP_DIR)/greenServe-extracted" -type f -name "greenServe" -print -quit)"; \
	if [ -z "$$GREEN_SERVE_BINARY" ]; then \
		echo "ERROR: greenServe binary was not found."; \
		exit 1; \
	fi; \
	echo "==> Installing binary to $(INSTALL_DIR)..."; \
	mkdir -p "$(INSTALL_DIR)"; \
	install -m 0755 "$$GREEN_SERVE_BINARY" "$(INSTALL_DIR)/greenServe"; \
	echo "==> Verifying installation..."; \
	"$(INSTALL_DIR)/greenServe" --version; \
	echo; \
	echo "greenServe language installed successfully."

install-vscode:
	@set -e; \
	command -v curl >/dev/null 2>&1 || { \
		echo "ERROR: curl is required."; \
		exit 1; \
	}; \
	command -v unzip >/dev/null 2>&1 || { \
		echo "ERROR: unzip is required."; \
		exit 1; \
	}; \
	command -v code >/dev/null 2>&1 || { \
		echo "ERROR: VS Code 'code' command was not found."; \
		exit 1; \
	}; \
	EXTENSION_TEMP_DIR="$$(mktemp -d)"; \
	trap 'rm -rf "$$EXTENSION_TEMP_DIR"' EXIT; \
	echo "==> Removing existing greenServe extension..."; \
	code --uninstall-extension "$(EXTENSION_ID)" >/dev/null 2>&1 || true; \
	echo "==> Removing old greenServe extension directories..."; \
	rm -rf "$$HOME/.vscode-server/extensions/$(EXTENSION_ID)-"* 2>/dev/null || true; \
	rm -rf "$$HOME/.local/share/code-server/extensions/$(EXTENSION_ID)-"* 2>/dev/null || true; \
	echo "==> Downloading greenServe VS Code extension..."; \
	curl -fL "$(GREEN_SERVE_VSCODE_URL)" \
		-o "$$EXTENSION_TEMP_DIR/greenServe-extension.zip"; \
	echo "==> Extracting extension ZIP..."; \
	mkdir -p "$$EXTENSION_TEMP_DIR/extracted"; \
	unzip -q "$$EXTENSION_TEMP_DIR/greenServe-extension.zip" \
		-d "$$EXTENSION_TEMP_DIR/extracted"; \
	echo "==> Finding VSIX package..."; \
	VSIX_FILE="$$(find "$$EXTENSION_TEMP_DIR/extracted" \
		-type f -name "*.vsix" -print -quit)"; \
	if [ -z "$$VSIX_FILE" ]; then \
		echo "ERROR: No VSIX file found in the extension ZIP."; \
		exit 1; \
	fi; \
	echo "VSIX package:"; \
	echo "$$VSIX_FILE"; \
	echo "==> Installing VSIX package..."; \
	sudo mkdir -p "$(SHARE_DIR)"; \
	sudo cp "$$VSIX_FILE" "$(EXTENSION_INSTALL_FILE)"; \
	code --install-extension "$(EXTENSION_INSTALL_FILE)" --force; \
	echo "==> Installed greenServe extensions:"; \
	code --list-extensions --show-versions | \
		grep -i "$(EXTENSION_ID)" || true; \
	echo; \
	echo "greenServe VS Code extension installed successfully."; \
	echo; \
	echo "Reload VS Code to apply the changes:"; \
	echo "1. Press Ctrl+Shift+P"; \
	echo "2. Select Developer: Reload Window"

uninstall:
	@set -e; \
	echo "==> Removing greenServe VS Code extension..."; \
	if command -v code >/dev/null 2>&1; then \
		code --uninstall-extension "$(EXTENSION_ID)" || true; \
	else \
		echo "VS Code 'code' command was not found. Skipping extension uninstall."; \
	fi; \
	echo "==> Removing extension directories..."; \
	rm -rf "$$HOME/.vscode-server/extensions/$(EXTENSION_ID)-"* 2>/dev/null || true; \
	rm -rf "$$HOME/.local/share/code-server/extensions/$(EXTENSION_ID)-"* 2>/dev/null || true; \
	echo "==> Removing installed VSIX package..."; \
	if [ "$$(id -u)" -eq 0 ]; then \
		rm -f "$(EXTENSION_INSTALL_FILE)"; \
	else \
		sudo rm -f "$(EXTENSION_INSTALL_FILE)" 2>/dev/null || true; \
	fi; \
	echo "==> Removing greenServe binary..."; \
	if [ "$$(id -u)" -eq 0 ]; then \
		rm -f "$(INSTALL_DIR)/greenServe"; \
	else \
		sudo rm -f "$(INSTALL_DIR)/greenServe" 2>/dev/null || true; \
	fi; \
	echo; \
	echo "greenServe has been uninstalled."

clean:
	@echo "==> Cleaning temporary files..."
	@rm -rf "$(TMP_DIR)"
	@echo "Temporary files removed."
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
