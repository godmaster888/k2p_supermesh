#!/bin/bash

HASHES_FILE=$1

# Дописывает в выходной файл информацию по хешам.
#
# Параметры:
# file		выходной файл
hashes_file_generate()
{
	local FILE=$1

	if [ $# -ne 1 ]
	then
		return 1
	fi

	local REPOS=$(find output/build -maxdepth 2 -type d -name .git | cut -d / -f 3 | sort)

	# Дописываем в конец, так как билд-сервер генерирует заголовок файла.
	echo >> $FILE

	for rep in ${REPOS}
	do
		pushd output/build/$rep/ >/dev/null
		local HASH=$(tail -1 .git/logs/HEAD | cut -d ' ' -f 2)
		local COUNT=$(echo $rep | grep -o '-' | wc -l)

		if [ $COUNT -eq 0 ]
		then
			local REP_STR=$(echo $rep | tr a-z'-' A-Z'_')
		else
			local REP_STR=$(echo $rep | cut -d '-' -f1-$COUNT | tr a-z'-' A-Z'_')
		fi

		popd > /dev/null

		case "$REP_STR" in
			DEUTERON_APPS|DEUTERON_ARCH|DEUTERON_FRAMEWORK|DEUTERON_MODULES|DATAMODEL)
				continue
				;;
		esac

		echo "${REP_STR}_COMMIT=${HASH}" >> $FILE
	done

	echo >> $FILE

	return 0
}

if [ $# -ne 1 ]
then
	echo "Usage:"
	echo "$0 <hashes_file>"
	exit 1
fi

echo "Generating hashes file..."
hashes_file_generate "$HASHES_FILE"

if [ $? -ne 0 ]
then
	echo "Error occured."
	exit 1
fi

echo "Done!"
echo "File: $HASHES_FILE"

exit 0
