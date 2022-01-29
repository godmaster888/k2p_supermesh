#!/bin/bash

if [ ! -f .sdk_root ];
then
	echo "Not in sdk root TODO"
	exit 1
fi

TEMPDIR=$(mktemp -d)
MYPWD=$(pwd)

PORT="13666"
APPLY=false

while [ $# -gt 0 ];
do
	case $1 in
		-a|--apply)
			APPLY=true
			;;

		-p|--port)
			PORT=$2
			shift
			;;
	esac

	shift
done

cd $TEMPDIR

nc -lp $PORT | tar x

echo "Patches saved in $TEMPDIR"

if ! sha256sum -c control.sha256sum;
then
	echo "Hash mismatch!"
	exit 1;
fi

if ! $APPLY;
then
	exit 0
fi

LIST_DIRS=""

for line in $(find $TEMPDIR -type f -iname "*.patch"); do
	LIST_DIRS="$LIST_DIRS $(dirname $line)"
done

LIST_DIRS=$(echo $LIST_DIRS | xargs -n1 | sort -u | xargs)

for dir in $LIST_DIRS; do
	cd $MYPWD/$(realpath --relative-to=$TEMPDIR $dir)
	git am -s $dir/*.patch
	cd - >/dev/null
done
