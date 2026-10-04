#!/usr/bin/env bash

set -euo pipefail

package_directory="${PKGDIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
output_directory="$package_directory/build/debian"

cd "$package_directory"
source_name="$(dpkg-parsechangelog --show-field Source)"
binary_package="$(awk '$1 == "Package:" { print $2; exit }' debian/control)"
mkdir --parents "$output_directory"

dpkg-buildpackage \
	--unsigned-source \
	--unsigned-changes \
	--post-clean \
	--build=binary

shopt -s nullglob
mv -- \
	"$package_directory"/../"$binary_package"_*.deb \
	"$package_directory"/../"$source_name"_*.build{,info} \
	"$package_directory"/../"$source_name"_*.changes \
	"$output_directory"
