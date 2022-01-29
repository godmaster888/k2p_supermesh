#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

LAST_PROFILE=$(cat .last_profile 2> /dev/null)

cleaning() {
	printf "\n===================================\n"
	if [ -n "$LAST_PROFILE" ]; then
		echo "$LAST_PROFILE" > .last_profile
	else
		rm .last_profile 2> /dev/null
		LAST_PROFILE='not set'
	fi
	printf "%bYour current PROFILE is %s%b\n" "$BLUE" "$LAST_PROFILE" "$NC"
	exit 0
}

trap cleaning SIGINT

updating() {
	local updated=0
	while IFS= read -r -d '' profile_path
	do
		let total++
		local profile=$(basename "$profile_path")
		profile_path="$profile_path/$profile"

		if [ ! -f "$profile_path" ]; then
			printf "%b%s is not a profile%b\n" "$YELLOW" "$profile" "$NC"
			continue;
		fi

		md5before=($(md5sum "$profile_path"))
		if ! make PROFILE="$profile" olddefconfig >/dev/null 2>&1
		then
			printf "%b%s failed%b\n" "$RED" "$profile" "$NC"
			continue
		fi
		md5after=($(md5sum "$profile_path"))
		if [ "${md5before[0]}" == "${md5after[0]}" ]; then
			printf "%s was skipped\n" "$profile"
			continue;
		fi

		let updated++
		printf "%b%s was updated%b\n" "$GREEN" "$profile" "$NC"
	done < <(find ./profiles/ -mindepth 1 -maxdepth 1 -type d -print0)
	echo "==================================="
	printf "DONE! %d/%d profiles were updated" "$updated" "$total"
}

updating
cleaning

