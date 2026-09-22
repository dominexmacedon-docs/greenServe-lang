# greenServe-lang

greenServe is the programming language specially built for server implementation.

## Install on Linux

The Linux release can be installed directly from the published `greenServe-v1.0.1` release. The Makefile below downloads the release, extracts it, installs the `greenServe` executable into a system-wide directory, and makes it available from any folder.

### Command

```completecommand
set -e

GREEN_SERVE_VERSION="v1.0.1"
GREEN_SERVE_EXTENSION_VERSION="v1.0.0"

GREEN_SERVE_URL="https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-v1.0.1/greenServe-linux-x86_64.zip"

GREEN_SERVE_EXTENSION_URL="https://github.com/dominexmacedon-docs/greenServe-lang/releases/download/greenServe-vscode-extension-v1.0.0/greenServe-vscode-5f0270f875024058c4d7ea97ec5f466aa097ebd0.zip"

GREEN_SERVE_COMMAND="greenServe"
GREEN_SERVE_EXTENSION_ID="dominexmacedon.greenserve-language"

INSTALL_DIR="/usr/local/bin"
SHARE_DIR="/usr/local/share/greenServe"
EXTENSION_INSTALL_DIR="$SHARE_DIR/vscode-extension"

TEMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TEMP_DIR"' EXIT

echo "========================================"
echo "Removing existing greenServe installation"
echo "========================================"

sudo rm -f "$INSTALL_DIR/$GREEN_SERVE_COMMAND"

echo "Removing existing VS Code extension..."

code --uninstall-extension "$GREEN_SERVE_EXTENSION_ID" || true

rm -rf "$HOME/.vscode-server/extensions/${GREEN_SERVE_EXTENSION_ID}-"*
rm -rf "$HOME/.local/share/code-server/extensions/${GREEN_SERVE_EXTENSION_ID}-"*

sudo rm -rf "$SHARE_DIR"

echo
echo "========================================"
echo "Checking required commands"
echo "========================================"

command -v curl >/dev/null
command -v unzip >/dev/null
command -v sudo >/dev/null
command -v code >/dev/null

echo "curl: OK"
echo "unzip: OK"
echo "sudo: OK"
echo "code: OK"

echo
echo "========================================"
echo "Installing greenServe $GREEN_SERVE_VERSION"
echo "========================================"

mkdir -p "$TEMP_DIR/greenServe"

echo "Downloading greenServe..."

curl -fL --show-error \
    "$GREEN_SERVE_URL" \
    -o "$TEMP_DIR/greenServe.zip"

echo "Extracting greenServe..."

unzip -q \
    "$TEMP_DIR/greenServe.zip" \
    -d "$TEMP_DIR/greenServe"

GREEN_SERVE_BINARY="$(find "$TEMP_DIR/greenServe" -type f -name "$GREEN_SERVE_COMMAND" -print -quit)"

if [ -z "$GREEN_SERVE_BINARY" ]; then
    echo "ERROR: greenServe binary was not found in the archive."
    exit 1
fi

sudo install -m 0755 \
    "$GREEN_SERVE_BINARY" \
    "$INSTALL_DIR/$GREEN_SERVE_COMMAND"

echo
echo "greenServe installation:"
"$INSTALL_DIR/$GREEN_SERVE_COMMAND" --version

echo
echo "========================================"
echo "Installing VS Code extension $GREEN_SERVE_EXTENSION_VERSION"
echo "========================================"

mkdir -p "$TEMP_DIR/extension"

echo "Downloading VS Code extension..."

curl -fL --show-error \
    "$GREEN_SERVE_EXTENSION_URL" \
    -o "$TEMP_DIR/greenServe-vscode.zip"

echo "Extracting extension release..."

unzip -q \
    "$TEMP_DIR/greenServe-vscode.zip" \
    -d "$TEMP_DIR/extension"

VSIX_FILE="$(find "$TEMP_DIR/extension" -type f -name "*.vsix" -print -quit)"

if [ -z "$VSIX_FILE" ]; then
    echo "ERROR: No .vsix file was found inside the extension ZIP."
    echo
    echo "Archive contents:"
    find "$TEMP_DIR/extension" -maxdepth 4 -type f -print
    exit 1
fi

echo "Found VSIX:"
echo "$VSIX_FILE"

sudo mkdir -p "$EXTENSION_INSTALL_DIR"

sudo cp \
    "$VSIX_FILE" \
    "$EXTENSION_INSTALL_DIR/greenServe-vscode-extension.vsix"

echo
echo "Installing VS Code extension..."

code --install-extension \
    "$EXTENSION_INSTALL_DIR/greenServe-vscode-extension.vsix" \
    --force

echo
echo "========================================"
echo "Verifying installation"
echo "========================================"

echo
echo "greenServe:"
"$INSTALL_DIR/$GREEN_SERVE_COMMAND" --version

echo
echo "VS Code extension:"

if code --list-extensions --show-versions | grep -i "^$GREEN_SERVE_EXTENSION_ID@" ; then
    echo
    echo "greenServe VS Code extension installed successfully."
else
    echo
    echo "ERROR: greenServe VS Code extension was not detected."
    exit 1
fi

echo
echo "Installed extension package:"
sudo ls -lh \
    "$EXTENSION_INSTALL_DIR/greenServe-vscode-extension.vsix"

echo
echo "========================================"
echo "Installation complete"
echo "========================================"

echo "greenServe: $GREEN_SERVE_VERSION"
echo "VS Code extension: $GREEN_SERVE_EXTENSION_VERSION"
echo "Extension ID: $GREEN_SERVE_EXTENSION_ID"
echo
echo "VS Code was not reloaded."
```

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

## Release

Current Linux release:

- Version: `greenServe-v1.0.1`
- Platform: Linux x86_64
- Archive: `greenServe-linux-x86_64.zip`
