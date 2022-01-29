#!/bin/bash

if [ ! -f .sdk_root ];
then
	echo "Not in sdk root TODO"
	exit 1
fi

TEMPDIR=$(mktemp -d)
MYPWD=$(pwd)
RESULT_PATCHES=""
IP=""
PORT="13666"

while [ $# -gt 0 ];
do
	case $1 in
		-i)
			IP=$2
			shift 2
			continue
			;;
		-p)
			PORT=$2
			shift 2
			continue
			;;
	esac

	path=$(realpath --relative-to=$MYPWD $1)
	patches=1

	if echo $2 | egrep -q '^-?[0-9]+$' ; then
		patches=$2
		shift
	fi

	temppath=$TEMPDIR/$path

	mkdir -pv $temppath

	cd $path

	retpatch=$(git format-patch -$patches -o $temppath)

	RESULT_PATCHES="$RESULT_PATCHES $(realpath --relative-to=$TEMPDIR $retpatch)"

	cd - >/dev/null

	shift
done

if [ -z "$IP" ];
then
	echo "IP is null!"
	exit 1
fi

cd $TEMPDIR
sha256sum $RESULT_PATCHES > control.sha256sum

RESULT_PATCHES="$RESULT_PATCHES control.sha256sum"

tar -cf - $RESULT_PATCHES | nc -q1 $IP $PORT
