#!/bin/bash

set -e

function tput() {
	command tput $@ 2>/dev/null || true
}

STRIP_CMD="${STRIP:-strip}"
DEPMOD_CMD="${DEPMOD:-/sbin/depmod}"
KERNEL_DIR="$1"
TARGET_DIR="$2"
KERNEL_VERSION="$3"

LIB_MODULES="$TARGET_DIR/lib/modules"
MODULES_DIR="$LIB_MODULES/$KERNEL_VERSION/"
INSTALL_MOD_PATH="$MODULES_DIR/kernel"

DEPRECATED_MODULES_DIR="$LIB_MODULES"

mkdir -p $INSTALL_MOD_PATH
find "$DEPRECATED_MODULES_DIR" -maxdepth 1 -type f \( -name '*.ko' -or -name 'modules*' \) -exec mv {} "$INSTALL_MOD_PATH/" \;

# remove symlinks
rm -f  $DEPRECATED_MODULES_DIR/build
rm -f  $DEPRECATED_MODULES_DIR/source
rm -rf $DEPRECATED_MODULES_DIR/kernel

$DEPMOD_CMD -b $TARGET_DIR $KERNEL_VERSION

find "$MODULES_DIR" -type f -name '*.ko' -exec $STRIP_CMD {} \;

# remove symlinks
rm -rf $MODULES_DIR/build
rm -rf $MODULES_DIR/source

# remove binary files
rm -f $MODULES_DIR/*.bin

# sanity check
KERNEL_RELEASE_FILE="$KERNEL_DIR/include/config/kernel.release"
KERNEL_RELEASE=$(cat "$KERNEL_RELEASE_FILE")

if [ "$KERNEL_RELEASE" != "$KERNEL_VERSION" ]; then
	tput bold
	tput setaf 1
	echo ""
	echo "kernel version missmatch detected"
	echo ""
	echo "$KERNEL_RELEASE_FILE: '$KERNEL_RELEASE'"
	echo "make kernelrelease: '$KERNEL_VERSION'"
	echo "please fix the kernel"
	tput sgr0
	exit 1
fi
