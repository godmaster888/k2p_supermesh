#!/bin/bash

SCRIPT_PATH=$(dirname "$0")
BASE_DIR=$(realpath "$SCRIPT_PATH/..")

CLR_RED='\033[0;31m'
CLR_YELLOW='\033[0;33m'
CLR_REVERSE='\e[7m'
CLR_RST='\033[0m'

REPOS=". output/build/deuteron-*"

pushd() {
	builtin pushd "$1" >/dev/null 2>&1
}

popd() {
	builtin popd >/dev/null 2>&1
}

repo_status() {
	git -c color.status=always status --short -uno
}

repo_branch_status() {
	git -c color.status=always status --short -b | head -n 1
}

repo_check_head() {
	local COMMIT_STATS
	local COMMIT_BEHIND
	local COMMIT_AHEAD

	COMMIT_STATS=$(git rev-list --left-right --count '@{u}...HEAD' 2>/dev/null)

	if [ -z "$COMMIT_STATS" ]; then
		echo -n ""
		return
	fi

	COMMIT_BEHIND=$(echo "$COMMIT_STATS" | awk '{print $1}')
	COMMIT_AHEAD=$(echo "$COMMIT_STATS" | awk '{print $2}')

	if [ "$COMMIT_BEHIND" -gt 0 ]; then
		echo -n "-"
	elif [ "$COMMIT_AHEAD" -gt 0 ]; then
		echo -n "+"
	else
		echo -n ""
	fi
}

repo_tag_check() {
	version=$1
	if [ -z "$(git branch --contains "$(git rev-parse "$version" 2>/dev/null)" 2>/dev/null)" ]; then
		echo "| tag $version is$CLR_YELLOW NOT FOUND.$CLR_RST"
	else
		echo ""
	fi
}

check () {
	pushd "$BASE_DIR"
	local dm_version=$(readlink -f ./output/build/deuteron-*/dm/dm/dm_version.dm 2>/dev/null | head -1)

	if [ ! -f "$dm_version" ]; then
		echo ""
		return 0;
	fi
	local STAT_LOG
	local VERSION
	local action

	STAT_LOG=""
	VERSION="v$(cat $dm_version)"
	action=$1

	for rep in $REPOS; do
		pushd "$rep"

		# git fetch >/dev/null 2>&1
		local CURRENT_STAT
		local BRANCH_STAT
		local REPO_CHECK
		local REPO
		local REPO

		CURRENT_STAT=$(repo_status)
		BRANCH_STAT=$(repo_branch_status)
		REPO_CHECK=$(repo_tag_check "$VERSION")
		REPO=$(git config --get remote.origin.url | cut -d':' -f2)
		REPO=${REPO##*/}

		if [ -n "$CURRENT_STAT" ] || [ -n "$REPO_CHECK" ] || [ -n "$(repo_check_head)" ] ; then
			if [ "$action" = "status" ]; then
				STAT_LOG="$STAT_LOG${CLR_REVERSE}$REPO${CLR_RST}: $BRANCH_STAT $REPO_CHECK\n$CURRENT_STAT\n"
			else
				STAT_LOG="+\n"

				if [ -n "$(repo_tag_check "$VERSION")" ]; then
					STAT_LOG="-\n"
					break;
				fi

				local sign
				sign="$(repo_check_head)"

				if [ -z "$sign" ]; then
					continue;
				fi

				if [ "$sign" = "-" ]; then
					STAT_LOG="-\n"
					break;
				fi
			fi
		fi

		popd
	done

	echo -ne "$STAT_LOG"

	popd
}

case $1 in
	"status")
		check status
		;;
	"sign")
		check sign
		;;
	*)
		echo "syntax: $0 status"
esac
