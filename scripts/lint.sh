#!/usr/bin/env bash

set -euo pipefail

repository_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repository_root"

check_cpp_format() {
    find ctprintf \
        -path ctprintf/build -prune -o \
        -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 |
        xargs --null --no-run-if-empty \
            clang-format \
            --dry-run \
            --Werror \
            --style=file:.clang-format
}

check_cmake_format() {
    find ctprintf \
        -path ctprintf/build -prune -o \
        -type f -name 'CMakeLists.txt' -print0 |
        xargs --null --no-run-if-empty \
            cmake-format \
            --check \
            --config-file .cmake-format.json
}

check_cmake_lint() {
    find ctprintf \
        -path ctprintf/build -prune -o \
        -type f -name 'CMakeLists.txt' -print0 |
        xargs --null --no-run-if-empty \
            cmakelint --linelength=100
}

usage() {
    echo "Usage: $0 [all|cpp-format|cmake-format|cmake-lint]" >&2
}

case "${1:-all}" in
    all)
        check_cpp_format
        check_cmake_format
        check_cmake_lint
        ;;
    cpp-format)
        check_cpp_format
        ;;
    cmake-format)
        check_cmake_format
        ;;
    cmake-lint)
        check_cmake_lint
        ;;
    *)
        usage
        exit 2
        ;;
esac
