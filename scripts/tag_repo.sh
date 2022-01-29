#!/bin/bash

CLR_YELLOW='\033[0;33m'
CLR_REVERSE='\e[7m'
CLR_RST='\033[0m'
SUMMARY=""
DRY_RUN=""

while getopts "n" opt
do
	case $opt in
		n) DRY_RUN="--dry-run"
			shift
			;;
		\? ) echo "Unknown option: -$OPTARG" >&2; exit 1;;
		:  ) echo "Missing option argument for -$OPTARG" >&2; exit 1;;
	esac
done

pushd() {
	builtin pushd $1 >/dev/null 2>&1
}

popd() {
	builtin popd >/dev/null 2>&1
}

GIT_LOG() {
	git --no-pager log --format="%C(cyan)%h%C(green) %an:%C(reset) %s %C(magenta)%d%C(reset)" $1
}

show_info() {
	pushd $1
	local REPO=$(git config --get remote.origin.url | cut -d':' -f2)
	local REPO=${REPO##*/}
	local BRANCH=$(git symbolic-ref --short -q HEAD)
	echo -e "${CLR_REVERSE}${REPO}-${BRANCH}${CLR_RST}"

	local LOG=$(GIT_LOG FETCH_HEAD..HEAD)

	if [ -z "$LOG" ]; then
		GIT_LOG -1
	else
		GIT_LOG FETCH_HEAD..HEAD
	fi

	popd
}

push() {
	local path="$1"
	local tag="$2"
	pushd $path
	git push $DRY_RUN
	git push origin "$tag" $DRY_RUN
	popd
}

clean_up() {
	local path="$1"
	local tag="$2"
	pushd $path
	git tag -d "$tag"
	popd
}

clean_up_all() {
	if [ -n "$DRY_RUN" ]; then
		echo "Clean up"

		clean_up . "$TAG"
		clean_up "$DEUTERON_DIR" "$TAG"

		pushd $DEUTERON_DIR
		git reset --hard HEAD~1 # Удаляем коммит с инкрементом версии датамодели
		popd
		exit 0
	fi
}

SCRIPT_PATH=$(dirname "$0")
BASE_DIR=$(realpath $SCRIPT_PATH/..)
DEUTERON_DIR=$(realpath $BASE_DIR/output/build/deuteron-* | head -n 1)

VERSION=${1##v}
TAG="v$VERSION"

VERSION_MAJOR=$(echo $VERSION | cut -d'.' -f 1)
VERSION_MINOR=$(echo $VERSION | cut -d'.' -f 2)
VERSION_BUILD=$(echo $VERSION | cut -d'.' -f 3 | grep -o -E '[0-9]+' | head -1)
VERSION_STR="$VERSION_MAJOR.$VERSION_MINOR.$VERSION_BUILD"

shift

echo -e "${CLR_YELLOW}VERSION:${CLR_RST} $VERSION_STR"
echo
echo "I'll put the tags on the repository:"

$BASE_DIR/scripts/tag_info.sh || exit 1
SUMMARY=$($BASE_DIR/scripts/tag_info.sh -d)

printf '%*s\n' "${COLUMNS:-$(tput cols)}" '' | tr ' ' -
read -p "Tag repos? " reply

if [[ ! $reply =~ ^[Yy]$ ]]; then
	exit 0;
fi

pushd $DEUTERON_DIR
echo -n "Update datamodel version file... "
echo $VERSION_STR > dm/dm/dm_version.dm && echo "OK" || echo "FAIL"


echo "Lookup for next-version fixups..."
VERSION_FIXUP_DIR="modules/config_fixup/version_fixups"
NEXT_FIXUP="$VERSION_FIXUP_DIR/version_next.c"

test -f "$NEXT_FIXUP" && git mv "$NEXT_FIXUP" "${VERSION_FIXUP_DIR}/version_${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_BUILD}.c"


echo "Commit new version to datamodel"
SUMMARY=$(echo -e "$SUMMARY")
git add dm/dm/dm_version.dm && git commit -sm "VERSION: $VERSION_STR" -m "" -m "$SUMMARY"
ret=$?

if [ $ret -ne 0 ]; then
	echo "Failed to commit new datamodel version"
	exit 1
fi
popd

# тегируем SDK
pushd $BASE_DIR
git tag -a "$TAG" -m "Version $TAG"
echo "SDK"

pushd $DEUTERON_DIR
echo "deuteron"
git tag -a "$TAG" -m "Version $TAG"
popd

echo "Done tagging."

show_info .
show_info $DEUTERON_DIR

trap clean_up_all SIGINT
DRY_RUN="cleanup" # HACK
read -p "Push to upstream? " reply

if [[ $reply =~ ^[Yy]$ ]]; then
	DRY_RUN="" # HACK
	push . "$TAG"
	push "$DEUTERON_DIR" "$TAG"
fi

clean_up_all

popd
