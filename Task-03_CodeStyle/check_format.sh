#!/usr/bin/env bash

TARGET_DIR="$1"

main(){

startup_security "$@"

check_format

}


startup_security(){
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
show_help
exit 0
fi

if [ $# -gt 1 ]; then
echo "Invalid number of arguments"
exit 1
fi

if [[ "$#" -eq 0 ]]; then
show_help
exit 0
fi

if [[ ! -d "$TARGET_DIR" ]]; then
echo "Target directory does not exist"
exit 2
fi

if [[ ! -f "$TARGET_DIR/.clang-format" ]]; then
echo ".clang-format file does not exist"
exit 3
fi

}

show_help(){
cat << EOF
USAGE:
  ${0##*/} <target_dir>
  ${0##*/} -h | --help

DESCRIPTION:
  Checks C/C++ source code formatting against .clang-format
  using two versions of clang-format 17 and 22.

ARGUMENTS:
  <target_dir>  Root directory containing source files and .clang-format

EXAMPLES:
  ${0##*/} ./dummy
EOF
}

check_version() {
    local ver="$1"
    local cmd="clang-format-$ver"

    echo "==== CLANG-FORMAT  $ver VERSION ===="

    if ! command -v "$cmd" &> /dev/null; then
        echo "$cmd is not installed."
        echo "================================="
        echo ""
        return 1
    fi

    local output
    output=$(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.h" \) -exec "$cmd" --dry-run --Werror {} + 2>&1)
    local status=$?

    if [ $status -eq 0 ]; then
        echo "Status: PASSED"
        echo "All checked files comply with the formatting rules."
    else
        local issue_count
        issue_count=$(echo "$output" | grep -c "error:")

        local failed_files
        failed_files=$(echo "$output" | grep "error:"| grep -Eo '^[^:]+\.(c|h)' | sort -u)

        echo "Status: FAILED"
        echo "Total formatting issues detected: $issue_count"
        echo ""
        echo "Files with violations:"
        if [ -n "$failed_files" ]; then
            echo "$failed_files" | sed 's/^/  - /'
        else
            echo "Unable to parse file list"
        fi
        echo ""
    fi

    echo "================================="
    echo ""
}

check_format() {
    check_version 17
    check_version 22
}

main "$@"