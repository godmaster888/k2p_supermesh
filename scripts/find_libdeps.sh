#!/bin/bash

declare -a search_path
declare -a found_deps

find_dependencies() {
    local file="$1"
    local -a deps
    local -a deps_to_process

    deps=( $( readelf -d "$file" | grep "NEEDED" | \
        grep -o -E "\[[^]]*\]" | grep -o -E "[^][]*" ) )

    # program interpreter
    deps+=( $(basename $(readelf -p .interp "$file"  | \
        grep -o -E "\[\s{0,}{1,}[0-9]\].*" | grep -o -E "\S{1,}$")) )

    local add_this_dep=true

    # always assume we've found $file and add it to the $found_deps list
    # if it's not there
    for found_dep in "${found_deps[@]}"
    do
        if [ "$found_dep" = "$(basename $file)" ]
        then
            add_this_dep=false
            break
        fi
    done

    # if $add_this_dep is true, go ahead and add to the found_deps list
    if $add_this_dep
    then
        if [ "$(basename $file)" != "$(basename $base_file)" ]
        then
            found_deps+=("$(basename $file)")
        fi
    fi

    # for every dependency found by readelf (no path)
    for dep in "${deps[@]}"
    do
        local process_dep=true

        # if the basename of the file passed into the function is
        # this dep, skip processing altogether
        if [ "$dep" = "$(basename $file)" ]
        then
            break
        else
            # otherwise, if it's one of the 'found deps' don't process it
            for found_dep in "${found_deps[@]}"
            do
                if [ "$dep" = "$found_dep" ]
                then
                    process_dep=false
                    break
                fi
            done

            # it wasn't one of the 'found deps' so add 
            # it to the found_deps list
            if $process_dep
            then
                found_deps+=($dep)
            fi
        fi

        # if we are supposed to process this dep
        if $process_dep
        then
            local file_path=

            # check each search path for a file named $dep underneath it
            for dir in $search_path
            do
                file_path=$( find "$dir" -name "$dep" | head -n 1 )

                # if the $file_path is not empty, then we found
                # a copy of it, so break out of the loop
                if [ -n "$file_path" ]; then break; fi;
            done

            # if the $file_path is not empty, then we found a copy
            # of the file, place it the list of deps to be processed
            if [ -n "$file_path" ]
            then
                deps_to_process+=($file_path)
            fi
        fi
    done

    # now, go through all of our $deps_to_process (with path)
    # and run "find_dependencies" on them
    for dep_to_process in "${deps_to_process[@]}"
    do
        find_dependencies "$dep_to_process"
    done
}

argc=$#

if [ $argc -eq 0 ]
then
    printf '%s: prints dependencies of a (potentially) non-native elf executable, recursively\n'
    printf '\n'
    printf 'usage:\n'
    printf '\t%s <non-native elf executable> [ <path> ... ]\n' "$(basename $0)"
    printf '\twhere\n'
    printf '\t\t<non-native elf executable> is the name of a file to find the dependencies of.\n'
    printf '\t\t[ <path> ... ] is an optional list of directories under which to search for libraries.\n'
    printf '\n'
else
    base_file="$1"
    shift 1

    search_path="$@"
    find_dependencies $base_file

    printf '%s\n' "${found_deps[@]}"
fi

# ❤ copyheart, shelleybutterfly, 2014
# contact information:
# shelleybutterfly@mojoprocode.com
