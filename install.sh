#!/bin/bash

set -euo pipefail
BIN="xre"

cd engine;

# LINUX
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if ! command -v sudo >/dev/null 2>&1; then
        echo "Error: 'sudo' not found"
        exit 1
    fi
    echo "Building program..."
    if ! make "$BIN"; then
        echo "Error: failed to build program"
        exit 1
    fi
    echo "Creating symlink..."
    if ! sudo ln -sf "$(pwd)/$BIN" /usr/bin/"$BIN"; then
        echo "Error: failed to create symlink"
        exit 1
    fi

# MAC OS
elif [[ "$OSTYPE" == "darwin"* ]]; then
    if ! command -v sudo >/dev/null 2>&1; then
        echo "Error: 'sudo' not found"
        exit 1
    fi
    echo "Building program..."
    if ! make "$BIN"; then
        echo "Error: failed to build program"
        exit 1
    fi
    echo "Creating symlink..."
    if ! sudo ln -sf "$(pwd)/$BIN" /usr/local/bin/"$BIN"; then
        echo "Error: failed to create symlink"
        exit 1
    fi
else
    echo "Error: unsupported operating system"
    exit 1
fi

echo "u can use xre now"