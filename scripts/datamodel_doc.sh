#!/bin/bash

SCRIPT_PATH=$(dirname "$0")
BASE_DIR=$(realpath $SCRIPT_PATH/..)

DMSDM=$(realpath $BASE_DIR/output/host/usr/sbin/dmsdm)
DM_JSON=$(realpath $BASE_DIR/output/build/datamodel-*/buildroot-build/reference_dm.json | head -n 1)

if ! [ -f "$DM_JSON" ]; then
	DM_JSON=$(realpath $BASE_DIR/output/build/datamodel-*/buildroot-build/dm.json | head -n 1)
fi

if ! [ -f "$DM_JSON" ]; then
	echo "Compiling datamodel..."
	make datamodel-recompile
fi

OUT=$1

if [ -z "$OUT" ]; then
	OUT=$BASE_DIR/output/host/usr/share/dm.html
fi

cmd="$DMSDM doc $DM_JSON $OUT"

echo "Compiling datamodel documentation..."
echo $cmd
$cmd
