#!/bin/bash

ERROR=false
OLD_TAG=""
NEW_TAG="HEAD"
FOR_DM_COMMIT=false
GIT_FORMAT="%h %an: %s"

if test -t 1; then
	ncolors=$(tput colors)

	# can colors
	if test -n "$ncolors" && test "$ncolors" -ge 8; then
		GIT_FORMAT="%C(cyan)%h%C(green) %an:%C(reset) %s"
		CLR_RED='\033[0;31m'
		CLR_YELLOW='\033[0;33m'
		CLR_REVERSE='\e[7m'
		CLR_RST='\033[0m'
	fi
fi
SUMMARY_REPS=""

show_usage() {
	echo "Usage:"
	echo "$1 [options]"
	echo ""
	echo "options:"
	echo "	-d		format for datamodel commit"
	echo "	-o old tag	set old tag for search (default tag from dm_version.dm)"
	echo "	-n new tag	set new tag for search (default HEAD)"
}

while getopts "hdo:n:" opt
do
	case $opt in
		d) FOR_DM_COMMIT=true
			;;
		o)
			OLD_TAG="$OPTARG"
			;;
		n)
			NEW_TAG="$OPTARG"
			;;
		h)
			show_usage "$0"
			exit 0
			;;
		\? ) show_usage; exit 1;;
		:  ) show_usage; exit 1;;
	esac
done

pushd() {
	builtin pushd $1 >/dev/null 2>&1
}

popd() {
	builtin popd >/dev/null 2>&1
}

show_info() {
	local paths
	local path
	local repo
	local branch
	local clean=true
	local message=""

	local COMMIT_STATS
	local COMMIT_BEHIND
	local COMMIT_AHEAD
	local DIFF_UNSTAGED
	local DIFF_STAGED
	local COMMIT_DIFF="${OLD_TAG}...${NEW_TAG}" # TODO: commit diff between tags

	paths=($1)
	path=${paths[0]}

	pushd "$path" || exit 1

	repo=$(git config --get remote.origin.url | cut -d':' -f2)
	repo=${repo##*/}
	branch=$(git symbolic-ref --short -q HEAD)

	git fetch --all >/dev/null 2>&1

	COMMIT_STATS=$(git rev-list --left-right --count "$COMMIT_DIFF")
	COMMIT_BEHIND=$(echo "$COMMIT_STATS" | awk '{print $1}')
	COMMIT_AHEAD=$(echo "$COMMIT_STATS" | awk '{print $2}')
	DIFF_UNSTAGED=$(git --no-pager diff)
	DIFF_STAGED=$(git --no-pager diff --staged)

	if [ -n "$DIFF_STAGED" ] || [ -n "$DIFF_UNSTAGED" ]; then
		message="${CLR_RED}Your repo is not clean.${CLR_RST}"
		ERROR=true
		clean=false
	fi

	if [ "$COMMIT_BEHIND" -gt 0 ]; then
		message="${CLR_RED}Your branch has $COMMIT_BEHIND commits behind the upstream.${CLR_RST}"
		ERROR=true
		clean=false
	fi

	if [ "$COMMIT_AHEAD" -gt 0 ]; then
		message="${CLR_YELLOW}Your branch has $COMMIT_AHEAD commits ahead.${CLR_RST}"
		clean=false
	fi

	if ! $clean && $FOR_DM_COMMIT && [ -n "$SUMMARY_REPS" ]; then
		echo ""
	fi

	if ! $FOR_DM_COMMIT || ! $clean; then
		echo -e "${CLR_REVERSE}${repo}-${branch}${CLR_RST}"
	fi

	if ! $clean; then
		git --no-pager log --format="$GIT_FORMAT" "${OLD_TAG}..${NEW_TAG}"

		if ! $FOR_DM_COMMIT; then
			git status --short -uno

			echo -e "$message"
		fi

		SUMMARY_REPS="$SUMMARY_REPS$repo"
	fi

	popd || exit 1
}

SCRIPT_PATH=$(dirname "$0")
BASE_DIR=$(realpath "$SCRIPT_PATH/..")
REPOS="deuteron"
DEUTERON_DIR=$(realpath "$BASE_DIR"/output/build/deuteron-* | head -n 1)

if [ -z "$OLD_TAG" ]; then
	OLD_TAG="v"$(cat "$DEUTERON_DIR/dm/dm/dm_version.dm")
fi

show_info "$BASE_DIR"
show_info "$DEUTERON_DIR"

if $ERROR; then
	exit 1
else
	exit 0
fi
