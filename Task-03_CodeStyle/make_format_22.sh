#!/usr/bin/env bash

TARGET_DIR="$1"
CLANG_VER="22"

main(){
startup_security "$@"
apply_format
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
  Automatically formats C/C++ source code in-place using clang-format-$CLANG_VER.

ARGUMENTS:
  <target_dir>  Root directory containing source files and .clang-format
EOF
}

apply_format() {
    local cmd="clang-format-$CLANG_VER"

    echo "==== APPLYING CLANG-FORMAT $CLANG_VER VERSION ===="

    if ! command -v "$cmd" &> /dev/null; then
        echo "Error: $cmd is not installed."
        echo "=================================================="
        exit 1
    fi

    find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.h" \) -exec "$cmd" -i {} +
    local status=$?

    if [ $status -eq 0 ]; then
        echo "Status: SUCCESS"
        echo "All files have been formatted."
    else
        echo "Status: FAILED"
        echo "An error occurred during formatting."
    fi

    echo "=================================================="
    echo ""
}

main "$@"