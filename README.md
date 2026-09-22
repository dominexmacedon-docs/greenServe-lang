# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Makefile

```makefile
GREEN_SERVE_VERSION := v1.0.1

GREEN_SERVE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip

GREEN_SERVE_VSCODE_VERSION := v1.0.0
GREEN_SERVE_VSCODE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-vscode-extension-v1.0.0/greenServe-vscode-4c2320012566325b325b76d4aa25020205be0182.zip

INSTALL_DIR := /usr/local/bin
SHARE_DIR := /usr/local/share/greenServe

TMP_DIR := /tmp/greenServe-install

GREEN_SERVE_ZIP := $(TMP_DIR)/greenServe-linux-x86_64.zip
GREEN_SERVE_VSCODE_ZIP := $(TMP_DIR)/greenServe-vscode-extension.zip

EXTENSION_ID := dominexmacedon.greenserve-language
EXTENSION_INSTALL_FILE := $(SHARE_DIR)/greenServe-vscode-extension.vsix

.PHONY: install install-language install-vscode uninstall clean

install:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Error: installation requires root privileges."; \
		echo "Run: sudo make install"; \
		exit 1; \
	fi; \
	command -v curl >/dev/null 2>&1 || { \
		echo "Error: curl is required."; \
		exit 1; \
	}; \
	command -v unzip >/dev/null 2>&1 || { \
		echo "Error: unzip is required."; \
		exit 1; \
	}; \
	command -v code >/dev/null 2>&1 || { \
		echo "Error: VS Code command 'code' was not found."; \
		exit 1; \
	}; \
	echo ""; \
	echo "========================================"; \
	echo " Installing greenServe"; \
	echo "========================================"; \
	echo ""; \
	rm -rf "$(TMP_DIR)"; \
	mkdir -p "$(TMP_DIR)/greenServe"; \
	mkdir -p "$(TMP_DIR)/extracted"; \
	mkdir -p "$(SHARE_DIR)"; \
	\
	echo "Downloading greenServe $(GREEN_SERVE_VERSION)..."; \
	curl -fL "$(GREEN_SERVE_URL)" -o "$(GREEN_SERVE_ZIP)"; \
	\
	echo "Extracting greenServe..."; \
	unzip -o "$(GREEN_SERVE_ZIP)" -d "$(TMP_DIR)/greenServe" >/dev/null; \
	\
	BINARY=$$(find "$(TMP_DIR)/greenServe" -type f -name greenServe -print -quit); \
	if [ -z "$$BINARY" ]; then \
		echo "Error: greenServe binary was not found in the release archive."; \
		rm -rf "$(TMP_DIR)"; \
		exit 1; \
	fi; \
	\
	echo "Installing greenServe executable..."; \
	install -m 755 "$$BINARY" "$(INSTALL_DIR)/greenServe"; \
	\
	echo ""; \
	echo "greenServe language installed."; \
	echo "  Executable: $(INSTALL_DIR)/greenServe"; \
	echo ""; \
	\
	echo "==> Removing existing greenServe extension..."; \
	code --uninstall-extension "$(EXTENSION_ID)" >/dev/null 2>&1 || true; \
	\
	echo "==> Removing old greenServe extension directories..."; \
	rm -rf "$$HOME/.vscode-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	rm -rf "$$HOME/.local/share/code-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	\
	TEMP_DIR="$$(mktemp -d)"; \
	trap 'rm -rf "$$TEMP_DIR"' EXIT; \
	\
	echo "==> Downloading greenServe VS Code extension $(GREEN_SERVE_VSCODE_VERSION)..."; \
	curl -fL "$(GREEN_SERVE_VSCODE_URL)" -o "$$TEMP_DIR/greenServe.zip"; \
	\
	echo "==> Extracting extension ZIP..."; \
	unzip -q "$$TEMP_DIR/greenServe.zip" -d "$$TEMP_DIR/extracted"; \
	\
	echo "==> Finding VSIX..."; \
	VSIX_FILE="$$(find "$$TEMP_DIR/extracted" -type f -name "*.vsix" -print -quit)"; \
	if [ -z "$$VSIX_FILE" ]; then \
		echo "ERROR: No VSIX file found."; \
		exit 1; \
	fi; \
	\
	echo "VSIX:"; \
	echo "$$VSIX_FILE"; \
	\
	echo "==> Installing VSIX..."; \
	sudo mkdir -p "$(SHARE_DIR)"; \
	sudo cp "$$VSIX_FILE" "$(EXTENSION_INSTALL_FILE)"; \
	code --install-extension "$(EXTENSION_INSTALL_FILE)" --force; \
	\
	echo "==> Installed extension:"; \
	code --list-extensions --show-versions | grep -i "greenserve" || true; \
	\
	echo "==> Cleaning temporary files..."; \
	rm -rf "$$TEMP_DIR"; \
	trap - EXIT; \
	\
	echo ""; \
	echo "========================================"; \
	echo " Installation completed"; \
	echo "========================================"; \
	echo ""; \
	echo "greenServe:"; \
	echo "  Version: $(GREEN_SERVE_VERSION)"; \
	echo "  Executable: $(INSTALL_DIR)/greenServe"; \
	echo ""; \
	echo "VS Code extension:"; \
	echo "  Version: $(GREEN_SERVE_VSCODE_VERSION)"; \
	echo "  Extension ID: $(EXTENSION_ID)"; \
	echo "  Source: $(GREEN_SERVE_VSCODE_URL)"; \
	echo ""; \
	echo "Installed successfully."; \
	echo ""; \
	echo "Reload VS Code:"; \
	echo "  Ctrl+Shift+P"; \
	echo "  Developer: Reload Window"; \
	echo ""; \
	"$(INSTALL_DIR)/greenServe" --version

install-language:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Error: installation requires root privileges."; \
		echo "Run: sudo make install-language"; \
		exit 1; \
	fi; \
	command -v curl >/dev/null 2>&1 || { \
		echo "Error: curl is required."; \
		exit 1; \
	}; \
	command -v unzip >/dev/null 2>&1 || { \
		echo "Error: unzip is required."; \
		exit 1; \
	}; \
	rm -rf "$(TMP_DIR)"; \
	mkdir -p "$(TMP_DIR)/greenServe"; \
	mkdir -p "$(SHARE_DIR)"; \
	echo "Downloading greenServe $(GREEN_SERVE_VERSION)..."; \
	curl -fL "$(GREEN_SERVE_URL)" -o "$(GREEN_SERVE_ZIP)"; \
	echo "Extracting greenServe..."; \
	unzip -o "$(GREEN_SERVE_ZIP)" -d "$(TMP_DIR)/greenServe" >/dev/null; \
	BINARY=$$(find "$(TMP_DIR)/greenServe" -type f -name greenServe -print -quit); \
	if [ -z "$$BINARY" ]; then \
		echo "Error: greenServe binary was not found in the release archive."; \
		rm -rf "$(TMP_DIR)"; \
		exit 1; \
	fi; \
	install -m 755 "$$BINARY" "$(INSTALL_DIR)/greenServe"; \
	rm -rf "$(TMP_DIR)"; \
	echo "greenServe $(GREEN_SERVE_VERSION) installed."; \
	"$(INSTALL_DIR)/greenServe" --version

install-vscode:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Error: installation requires root privileges."; \
		echo "Run: sudo make install-vscode"; \
		exit 1; \
	fi; \
	command -v curl >/dev/null 2>&1 || { \
		echo "Error: curl is required."; \
		exit 1; \
	}; \
	command -v unzip >/dev/null 2>&1 || { \
		echo "Error: unzip is required."; \
		exit 1; \
	}; \
	command -v code >/dev/null 2>&1 || { \
		echo "Error: VS Code command 'code' was not found."; \
		exit 1; \
	}; \
	\
	EXTENSION_ID="$(EXTENSION_ID)"; \
	ZIP_URL="$(GREEN_SERVE_VSCODE_URL)"; \
	TEMP_DIR="$$(mktemp -d)"; \
	INSTALL_DIR="$(SHARE_DIR)"; \
	\
	trap 'rm -rf "$$TEMP_DIR"' EXIT; \
	\
	echo "==> Removing existing greenServe extension..."; \
	code --uninstall-extension "$$EXTENSION_ID" >/dev/null 2>&1 || true; \
	\
	echo "==> Removing old greenServe extension directories..."; \
	rm -rf "$$HOME/.vscode-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	rm -rf "$$HOME/.local/share/code-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	\
	echo "==> Downloading extension ZIP..."; \
	curl -fL "$$ZIP_URL" -o "$$TEMP_DIR/greenServe.zip"; \
	\
	echo "==> Extracting ZIP..."; \
	unzip -q "$$TEMP_DIR/greenServe.zip" -d "$$TEMP_DIR/extracted"; \
	\
	echo "==> Finding VSIX..."; \
	VSIX_FILE="$$(find "$$TEMP_DIR/extracted" -type f -name "*.vsix" -print -quit)"; \
	\
	if [ -z "$$VSIX_FILE" ]; then \
		echo "ERROR: No VSIX file found."; \
		exit 1; \
	fi; \
	\
	echo "VSIX:"; \
	echo "$$VSIX_FILE"; \
	\
	echo "==> Installing VSIX..."; \
	sudo mkdir -p "$$INSTALL_DIR"; \
	sudo cp "$$VSIX_FILE" "$$INSTALL_DIR/greenServe-vscode-extension.vsix"; \
	code --install-extension \
		"$$INSTALL_DIR/greenServe-vscode-extension.vsix" \
		--force; \
	\
	echo "==> Installed extension:"; \
	code --list-extensions --show-versions | grep -i "greenserve" || true; \
	\
	echo "==> Cleaning temporary files..."; \
	rm -rf "$$TEMP_DIR"; \
	trap - EXIT; \
	\
	echo ""; \
	echo "greenServe extension installed successfully."; \
	echo ""; \
	echo "Reload VS Code:"; \
	echo "Ctrl+Shift+P"; \
	echo "Developer: Reload Window"

uninstall:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Error: uninstallation requires root privileges."; \
		echo "Run: sudo make uninstall"; \
		exit 1; \
	fi; \
	echo "Removing greenServe..."; \
	rm -f "$(INSTALL_DIR)/greenServe"; \
	\
	if command -v code >/dev/null 2>&1; then \
		code --uninstall-extension "$(EXTENSION_ID)" >/dev/null 2>&1 || true; \
	fi; \
	\
	rm -rf "$$HOME/.vscode-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	rm -rf "$$HOME/.local/share/code-server/extensions/dominexmacedon.greenserve-language-"* 2>/dev/null || true; \
	rm -rf "$(SHARE_DIR)"; \
	rm -rf "$(TMP_DIR)"; \
	\
	echo "greenServe and its VS Code extension have been removed."

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
