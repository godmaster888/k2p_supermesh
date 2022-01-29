#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
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
	local failed=0
	while IFS= read -r -d '' profile_path
	do
		let total++
		local profile=$(basename "$profile_path")
		profile_path="$profile_path/$profile"

		if [ -z "$(find ./profiles/$profile/ -name "*config.default")" ]; then
			printf "%s is not a profile\n" "$profile"
			continue;
		fi

		local err=$(make PROFILE="$profile" show-targets 2>&1 >/dev/null | grep -Po "in TARGET_CFLAGS: \K([^\.]+)")
		if [ -n "$err" ]
		then
			local msg="$profile failed:"
			printf "%b%-45s%b \t%s\n" "$RED" "$msg" "$NC" "$err"
			failed=$((failed + 1))
		else
			printf "%b%s passed%b\n" "$GREEN" "$profile" "$NC"
		fi
	done < <(find ./profiles/ -mindepth 1 -maxdepth 1 -type d -print0)
	echo "==================================="
	printf "DONE! %d/%d profiles were failed" "$failed" "$total"
}

updating
cleaning

