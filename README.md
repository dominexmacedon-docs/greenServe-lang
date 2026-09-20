# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Makefile

```makefile
GREEN_SERVE_VERSION := v1.0.1

GREEN_SERVE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip

GREEN_SERVE_VSCODE_VERSION := v1.0.0
GREEN_SERVE_VSCODE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-vscode-extension-v1.0.0/greenServe-vscode-f6ebc673df0d4993459813708aa5f911411e3766.zip

INSTALL_DIR := /usr/local/bin
SHARE_DIR := /usr/local/share/greenServe

VS_CODE_EXTENSION_DIR := /usr/share/code/extensions
CODE_SERVER_EXTENSION_DIR := /usr/share/code-server/extensions

TMP_DIR := /tmp/greenServe-install

GREEN_SERVE_ZIP := $(TMP_DIR)/greenServe-linux-x86_64.zip
GREEN_SERVE_VSCODE_ZIP := $(TMP_DIR)/greenServe-vscode-extension.zip

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
	echo ""; \
	echo "========================================"; \
	echo " Installing greenServe"; \
	echo "========================================"; \
	echo ""; \
	rm -rf "$(TMP_DIR)"; \
	mkdir -p "$(TMP_DIR)/greenServe"; \
	mkdir -p "$(TMP_DIR)/vscode"; \
	mkdir -p "$(INSTALL_DIR)"; \
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
	echo "Downloading greenServe VS Code extension $(GREEN_SERVE_VSCODE_VERSION)..."; \
	curl -fL "$(GREEN_SERVE_VSCODE_URL)" -o "$(GREEN_SERVE_VSCODE_ZIP)"; \
	\
	echo "Extracting VS Code extension..."; \
	unzip -o "$(GREEN_SERVE_VSCODE_ZIP)" -d "$(TMP_DIR)/vscode" >/dev/null; \
	\
	VSIX=$$(find "$(TMP_DIR)/vscode" -type f -name '*.vsix' -print -quit); \
	if [ -n "$$VSIX" ]; then \
		echo "Found VSIX package: $$VSIX"; \
		\
		command -v code >/dev/null 2>&1 || { \
			echo "Error: VS Code command 'code' was not found."; \
			echo "The VSIX was downloaded, but automatic installation could not continue."; \
			rm -rf "$(TMP_DIR)"; \
			exit 1; \
		}; \
		\
		echo "Installing VS Code extension..."; \
		code --install-extension "$$VSIX" --force; \
	else \
		echo "No .vsix file found in the extension archive."; \
		echo "Checking for an already-extracted VS Code extension..."; \
		\
		EXT_DIR=$$(find "$(TMP_DIR)/vscode" -mindepth 1 -maxdepth 2 -type f -name package.json -printf '%h\n' -quit); \
		if [ -z "$$EXT_DIR" ]; then \
			echo "Error: VS Code extension package was not found."; \
			rm -rf "$(TMP_DIR)"; \
			exit 1; \
		fi; \
		\
		echo "Installing extracted VS Code extension..."; \
		mkdir -p "$(VS_CODE_EXTENSION_DIR)"; \
		EXT_NAME=$$(basename "$$EXT_DIR"); \
		rm -rf "$(VS_CODE_EXTENSION_DIR)/$$EXT_NAME"; \
		cp -a "$$EXT_DIR" "$(VS_CODE_EXTENSION_DIR)/$$EXT_NAME"; \
	fi; \
	\
	rm -rf "$(TMP_DIR)"; \
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
	echo "  Source: $(GREEN_SERVE_VSCODE_URL)"; \
	echo ""; \
	echo "Installed successfully."; \
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
	rm -rf "$(TMP_DIR)"; \
	mkdir -p "$(TMP_DIR)/vscode"; \
	echo "Downloading greenServe VS Code extension $(GREEN_SERVE_VSCODE_VERSION)..."; \
	curl -fL "$(GREEN_SERVE_VSCODE_URL)" -o "$(GREEN_SERVE_VSCODE_ZIP)"; \
	echo "Extracting VS Code extension archive..."; \
	unzip -o "$(GREEN_SERVE_VSCODE_ZIP)" -d "$(TMP_DIR)/vscode" >/dev/null; \
	VSIX=$$(find "$(TMP_DIR)/vscode" -type f -name '*.vsix' -print -quit); \
	if [ -n "$$VSIX" ]; then \
		if command -v code >/dev/null 2>&1; then \
			echo "Installing VS Code extension with code..."; \
			code --install-extension "$$VSIX" --force; \
		else \
			echo "Error: VS Code command 'code' was not found."; \
			rm -rf "$(TMP_DIR)"; \
			exit 1; \
		fi; \
	else \
		EXT_DIR=$$(find "$(TMP_DIR)/vscode" -mindepth 1 -maxdepth 2 -type f -name package.json -printf '%h\n' -quit); \
		if [ -z "$$EXT_DIR" ]; then \
			echo "Error: VS Code extension package was not found."; \
			rm -rf "$(TMP_DIR)"; \
			exit 1; \
		fi; \
		mkdir -p "$(VS_CODE_EXTENSION_DIR)"; \
		EXT_NAME=$$(basename "$$EXT_DIR"); \
		rm -rf "$(VS_CODE_EXTENSION_DIR)/$$EXT_NAME"; \
		cp -a "$$EXT_DIR" "$(VS_CODE_EXTENSION_DIR)/$$EXT_NAME"; \
	fi; \
	rm -rf "$(TMP_DIR)"; \
	echo "greenServe VS Code extension installed."

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
		code --uninstall-extension greenserve-language >/dev/null 2>&1 || true; \
	fi; \
	\
	rm -rf "$(VS_CODE_EXTENSION_DIR)/greenserve-language"*; \
	rm -rf "$(CODE_SERVER_EXTENSION_DIR)/greenserve-language"*; \
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
