# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Makefile

```makefile
GREEN_SERVE_VERSION := v1.0.1

GREEN_SERVE_URL := https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip
GREEN_SERVE_ICON_URL := https://raw.githubusercontent.com/dominexmacedon-docs/greenServe-lang/main/greenServe.ico

INSTALL_DIR := /usr/local/bin
SHARE_DIR := /usr/local/share/greenServe
ICON_DIR := /usr/local/share/icons/hicolor/256x256/apps

ICON_FILE := $(SHARE_DIR)/greenServe.ico
DESKTOP_FILE := /usr/share/applications/greenServe.desktop
MIME_FILE := /usr/share/mime/packages/greenserve.xml

TMP_DIR := /tmp/greenServe-install
ZIP_FILE := $(TMP_DIR)/greenServe-linux-x86_64.zip

.PHONY: install uninstall clean

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
	echo "Installing greenServe $(GREEN_SERVE_VERSION)..."; \
	rm -rf "$(TMP_DIR)"; \
	mkdir -p "$(TMP_DIR)/extracted"; \
	mkdir -p "$(SHARE_DIR)"; \
	mkdir -p "$(ICON_DIR)"; \
	echo "Downloading greenServe..."; \
	curl -fL "$(GREEN_SERVE_URL)" -o "$(ZIP_FILE)"; \
	echo "Extracting greenServe..."; \
	unzip -o "$(ZIP_FILE)" -d "$(TMP_DIR)/extracted" >/dev/null; \
	BINARY=$$(find "$(TMP_DIR)/extracted" -type f -name greenServe -print -quit); \
	if [ -z "$$BINARY" ]; then \
		echo "Error: greenServe binary was not found in the release archive."; \
		rm -rf "$(TMP_DIR)"; \
		exit 1; \
	fi; \
	echo "Installing executable..."; \
	install -m 755 "$$BINARY" "$(INSTALL_DIR)/greenServe"; \
	echo "Downloading greenServe.ico..."; \
	curl -fL "$(GREEN_SERVE_ICON_URL)" -o "$(ICON_FILE)"; \
	chmod 644 "$(ICON_FILE)"; \
	echo "Creating greenServe MIME type..."; \
	cat > "$(MIME_FILE)" <<'EOF' \
<?xml version="1.0" encoding="UTF-8"?> \
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info"> \
  <mime-type type="text/x-gsve"> \
    <comment>greenServe source file</comment> \
    <glob pattern="*.gsve"/> \
  </mime-type> \
</mime-info> \
EOF \
	echo "Creating desktop file association..."; \
	cat > "$(DESKTOP_FILE)" <<EOF \
[Desktop Entry] \
Name=greenServe \
GenericName=greenServe Programming Language \
Comment=greenServe programming language interpreter \
Exec=$(INSTALL_DIR)/greenServe %f \
Icon=$(ICON_FILE) \
Terminal=true \
Type=Application \
Categories=Development;Programming; \
MimeType=text/x-gsve; \
NoDisplay=false \
StartupNotify=false \
EOF \
	echo "Updating MIME database..."; \
	if command -v update-mime-database >/dev/null 2>&1; then \
		update-mime-database /usr/share/mime >/dev/null 2>&1 || true; \
	fi; \
	echo "Updating desktop database..."; \
	if command -v update-desktop-database >/dev/null 2>&1; then \
		update-desktop-database /usr/share/applications >/dev/null 2>&1 || true; \
	fi; \
	echo "Refreshing icon cache..."; \
	if command -v gtk-update-icon-cache >/dev/null 2>&1; then \
		gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor >/dev/null 2>&1 || true; \
	fi; \
	rm -rf "$(TMP_DIR)"; \
	echo ""; \
	echo "greenServe $(GREEN_SERVE_VERSION) installed successfully."; \
	echo ""; \
	echo "Executable:"; \
	echo "  $(INSTALL_DIR)/greenServe"; \
	echo ""; \
	echo "Icon:"; \
	echo "  $(ICON_FILE)"; \
	echo ""; \
	echo "File extension:"; \
	echo "  *.gsve"; \
	echo ""; \
	echo "MIME type:"; \
	echo "  text/x-gsve"; \
	echo ""; \
	echo "Desktop association:"; \
	echo "  $(DESKTOP_FILE)"; \
	echo ""; \
	echo "Version:"; \
	"$(INSTALL_DIR)/greenServe" --version

uninstall:
	@set -e; \
	if [ "$$(id -u)" -ne 0 ]; then \
		echo "Error: uninstallation requires root privileges."; \
		echo "Run: sudo make uninstall"; \
		exit 1; \
	fi; \
	echo "Removing greenServe..."; \
	rm -f "$(INSTALL_DIR)/greenServe"; \
	rm -f "$(ICON_FILE)"; \
	rm -f "$(DESKTOP_FILE)"; \
	rm -f "$(MIME_FILE)"; \
	rmdir "$(SHARE_DIR)" 2>/dev/null || true; \
	if command -v update-mime-database >/dev/null 2>&1; then \
		update-mime-database /usr/share/mime >/dev/null 2>&1 || true; \
	fi; \
	if command -v update-desktop-database >/dev/null 2>&1; then \
		update-desktop-database /usr/share/applications >/dev/null 2>&1 || true; \
	fi; \
	if command -v gtk-update-icon-cache >/dev/null 2>&1; then \
		gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor >/dev/null 2>&1 || true; \
	fi; \
	echo "greenServe has been completely removed."

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
