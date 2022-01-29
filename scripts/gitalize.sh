#!/bin/bash

SDK_DIR="$PWD"

PKG_NAME="$1"

if [ -z "$PKG_NAME" ]; then
	echo "Package name is not specified, abort"
	exit 1
fi

REPO_DIR=$(find output/build/ -mindepth 1 -maxdepth 1 -type d -name "$PKG_NAME*" | head -n1)
if [ -n "$REPO_DIR" ] && [ -d "$REPO_DIR" ]; then
	echo "$REPO_DIR already exists, abort"
	exit 1
fi

make "$PKG_NAME"-extract
REPO_DIR=$(find output/build/ -mindepth 1 -maxdepth 1 -type d -name "$PKG_NAME*" | head -n1)
if [ -z "$REPO_DIR" ] || ! cd "$REPO_DIR"; then
	echo "cannot cd to $REPO_DIR, abort"
	exit 1
fi

git init
echo -e "[alias]\\n\\tpatch = \"!sh $SDK_DIR/scripts/patch.sh\"" >> .git/config
git add .
git commit -m "initial commit"

for PATCH in "$SDK_DIR/package/$PKG_NAME/"*.patch; do
	git am -3k "$PATCH"
done

touch .stamp_patched
