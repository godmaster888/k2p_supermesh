#!/bin/sh

print_help() {
	echo "Help:"
	echo ""
	echo "create <NUMBER_COMMITS or REVISION or all> <NUMBER_TICKET> [TAGS_IN_FILE] [OUTPUT_DIR_FOR_PATCH]"
	echo "apply <files for apply patch>"
	echo "apply-hash <files for apply patch>"
	echo "apply-abort for abort incomplete apply"
	echo "check-patch <patch>"
	echo "check-file <file>"
	exit 1;
}

do_create() {
	if [ $# -lt 2 ]; then
		print_help;
	fi

	REPO_REVISION="$1"
	REPO_TICKET="$2"
	REPO_TICKET_POSTFIX="_"
	SUBJECT_PREFIX="--subject-prefix=#${REPO_TICKET}"
	REPO_NAME=$(git config --get remote.origin.url | cut -d':' -f2 | sed "s|.*/||g")
	REPO_URL=$(git config user.email | cut -d@ -f1)
	REPO_BRANCH=$(git symbolic-ref HEAD | sed 's#^refs/heads/##')
	REPO_BRANCH_NAME=$(echo "$REPO_BRANCH" | sed 's#/#_#g')
	REPO_HASH=$(git rev-parse origin/"${REPO_BRANCH}")
	REPO_OUT_DIR="."
	TAGS=""

	if [ "$REPO_TICKET" = "null" ]; then
		REPO_TICKET=""
		REPO_TICKET_POSTFIX=""
		SUBJECT_PREFIX="-k"
	elif ! echo "$REPO_TICKET" | egrep -q '^-?[0-9]+$' ; then
		SUBJECT_PREFIX="-k"
	fi

	REPO_TICKET=$(echo "$REPO_TICKET" | sed 's/^[#_]//')

	if [ "$REPO_REVISION" = "all" ]; then
		REPO_REVISION="origin/${REPO_BRANCH}"
	elif echo "$REPO_REVISION"  | egrep -q '^-?[0-9]+$' ; then
		REPO_REVISION="-n$REPO_REVISION"
	fi

	shift 2
	while [ $# -gt 0 ]; do
		if [ -d "$1" ]; then
			REPO_OUT_DIR="$1"
		else
			if [ -z "$TAGS" ]; then
				TAGS="$(echo "$1" | sed "s/^[[:blank:]]\+//" | sed "s/[[:blank:]]\+$//" | sed "s/ /_/g")"
			else
				TAGS="${TAGS}_$(echo "$1" | sed "s/^[[:blank:]]\+//" | sed "s/[[:blank:]]\+$//" | sed "s/ /_/g")"
			fi
		fi

		shift
	done


	if [ -n "$3" ]; then
		REPO_OUT_DIR="$3"
	fi

	OUT_FILE_BASE=${REPO_OUT_DIR}/${REPO_TICKET}${REPO_TICKET_POSTFIX}${REPO_URL}_${REPO_NAME}_${REPO_BRANCH_NAME}


	if [ -n "$TAGS" ]; then
		OUT_FILE_BASE="${OUT_FILE_BASE}_${TAGS}"
	fi

	OUT_FILE_NAME=${OUT_FILE_BASE}.patch
	VERSION=1

	while [ -f "$OUT_FILE_NAME" ]; do
		VERSION=$((VERSION+1))
		OUT_FILE_NAME=${OUT_FILE_BASE}_v${VERSION}.patch
	done

	git format-patch "$REPO_REVISION" \
	                    $SUBJECT_PREFIX \
	                    --add-header="Patch-from-hash: $REPO_HASH" \
	                    --add-header="Patch-repo: $REPO_NAME" \
	                    --add-header="Patch-branch: $REPO_BRANCH" \
	                    --add-header="Patch-version: $VERSION" \
	                    --suffix="_${REPO_NAME}_${REPO_URL}_${REPO_TICKET}.patch" \
	                    --no-numbered -s --stdout > "$OUT_FILE_NAME"

	echo "Created patch in $OUT_FILE_NAME"
}

do_apply() {
	git am -k --committer-date-is-author-date "$@"
}

do_apply_abort() {
	git am --abort
}

print_error() {
	case $1 in
		1)
			echo "Error checkout to main branch."
			;;
		2)
			echo "Merge error. Please resolve of conflicts and run git rebase."
			;;
		3)
			echo "Error rebasing, please resolve conflicts."
			;;
		4)
			echo "Error apply patch"
			;;
	esac
}

do_apply_from_hash() {
	GLOBAL_STATUS=0
	REPO_HASH=$(grep "Patch-from-hash" "$1" | cut -d' ' -f2)
	REPO_BRANCH=$(git symbolic-ref --short HEAD)

	if [ -z "$REPO_HASH" ]; then
		echo "Hash not found!"
		exit 1
	fi

	git checkout "$REPO_HASH" -b "apply_$REPO_HASH"

	do_apply "$@" && \
	GLOBAL_STATUS=-1 \
	git checkout "$REPO_BRANCH" && \
	GLOBAL_STATUS=1 && \
	git merge "apply_$REPO_HASH" && \
	GLOBAL_STATUS=2 && \
	git rebase && \
	GLOBAL_STATUS=3 && \
	git branch -D "apply_$REPO_HASH" || \
	print_error $GLOBAL_STATUS
}

do_checkpatch() {
	CURRENT_DIR=$(dirname "$0")
	SCRIPT="$CURRENT_DIR/checkpatch.pl"
	IGNORE_TYPES="C99_COMMENTS" #Ignore types splited space symbol
	ARGS=""
	ACTION=$1
	shift

	if [ -n "$IGNORE_TYPES" ];
	then
		ARGS="$ARGS --ignore $IGNORE_TYPES"
	fi

	if [ "$ACTION" -eq 1 ];
	then
		ARGS="$ARGS $*"
		# checkpatch
	else
		ARGS="$ARGS --file $*"
		# checkfile
	fi

	$SCRIPT $ARGS
}

main() {
	ACTION="$1"
	shift

	case "$ACTION" in
		create)
			do_create "$@"
			;;
		apply)
			do_apply "$@"
			;;
		apply-hash)
			do_apply_from_hash "$@"
			;;
		apply-abort)
			do_apply_abort
			;;
		check-patch)
			do_checkpatch 1 "$@"
			;;
		check-file)
			do_checkpatch 0 "$@"
			;;
		*)
			print_help
			;;
	esac
}

main "$@"
