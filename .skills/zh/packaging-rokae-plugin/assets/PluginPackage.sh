#!/bin/bash

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CURRENT_NAME="$(basename "$SCRIPT_DIR")"

CLIENT_DIR="$SCRIPT_DIR/client"
CONTROLLER_DIR="$SCRIPT_DIR/controller"
TEMP_DIR="/tmp/zip_work_$$"
CLIENT_ZIP="$TEMP_DIR/client.zip"
CONTROLLER_ZIP="$TEMP_DIR/controller.zip"
FINAL_ZIP="$SCRIPT_DIR/$CURRENT_NAME.zip"

echo ""
echo "========================================"
echo "  Zip Tool"
echo "========================================"
echo ""
echo "Script: $SCRIPT_DIR"
echo "Output: $FINAL_ZIP"
echo ""

# Check if folders exist and have content
echo "[1/5] Checking folders..."

CLIENT_EXISTS=0
CONTROLLER_EXISTS=0

# Check client folder
if [ -d "$CLIENT_DIR" ] && [ "$(ls -A "$CLIENT_DIR" 2>/dev/null)" ]; then
    CLIENT_EXISTS=1
fi

# Check controller folder
if [ -d "$CONTROLLER_DIR" ] && [ "$(ls -A "$CONTROLLER_DIR" 2>/dev/null)" ]; then
    CONTROLLER_EXISTS=1
fi

# If both are empty, exit
if [ $CLIENT_EXISTS -eq 0 ] && [ $CONTROLLER_EXISTS -eq 0 ]; then
    echo "[Error] Both folders are empty or missing"
    read -n 1 -s -r -p "Press any key to exit..."
    echo ""
    exit 1
fi

echo "[2/5] Initializing..."
# Cleanup
rm -rf "$TEMP_DIR" 2>/dev/null
rm -f "$FINAL_ZIP" 2>/dev/null
mkdir -p "$TEMP_DIR"

# Build zip list
ZIP_LIST=""

# Compress client
if [ $CLIENT_EXISTS -eq 1 ]; then
    echo "[3/5] Compressing client..."
    cd "$CLIENT_DIR" || exit 1
    zip -r "$CLIENT_ZIP" . -x "*.zip" >/dev/null 2>&1
    cd "$SCRIPT_DIR" || exit 1
    ZIP_LIST="$CLIENT_ZIP"
fi

# Compress controller
if [ $CONTROLLER_EXISTS -eq 1 ]; then
    echo "[4/5] Compressing controller..."
    cd "$CONTROLLER_DIR" || exit 1
    zip -r "$CONTROLLER_ZIP" . -x "*.zip" >/dev/null 2>&1
    cd "$SCRIPT_DIR" || exit 1
    if [ -z "$ZIP_LIST" ]; then
        ZIP_LIST="$CONTROLLER_ZIP"
    else
        ZIP_LIST="$ZIP_LIST $CONTROLLER_ZIP"
    fi
fi

echo "[5/5] Creating final archive..."
cd "$TEMP_DIR" || exit 1
zip -j "$FINAL_ZIP" $ZIP_LIST >/dev/null 2>&1
cd "$SCRIPT_DIR" || exit 1

echo "[6/5] Cleaning up..."
rm -rf "$TEMP_DIR" 2>/dev/null

echo ""
echo "[OK] Done: $FINAL_ZIP"
echo ""
read -n 1 -s -r -p "Press any key to exit..."
echo ""
exit 0
