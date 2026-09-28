#!/bin/bash
ARCH=$(uname -m)

if [ "$ARCH" = "x86_64" ]; then
    TARGET="app_host"
    CC="gcc"
    echo "Host: $ARCH" 
    echo "Target binary: $TARGET"
elif [ "$ARCH" = "aarch64" ]; then
    TARGET="app_target"
    CC="gcc"
    echo "Target: $ARCH"
    echo "Target binary: $TARGET"
else
    TARGET="app_unknown"
    CC="gcc"
    echo "Unknown arch: $ARCH"
    echo "Target binary: $TARGET"
fi

$CC -Wall -Wextra -O2 main.c -o "$TARGET"

