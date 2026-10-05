#!/usr/bin/env bash

set -euo pipefail

sudo DEBIAN_FRONTEND=noninteractive apt-get update
sudo DEBIAN_FRONTEND=noninteractive apt-get install --yes --no-install-recommends \
    build-essential \
    clang-format \
    clang-tidy \
    cmake \
    debhelper \
    devscripts \
    libgtest-dev \
    ninja-build \
    pipx

pipx install --force cmakelang
pipx install --force cmakelint

export PATH="$HOME/.local/bin:$PATH"
if [[ -n "${GITHUB_PATH:-}" ]]; then
    echo "$HOME/.local/bin" >>"$GITHUB_PATH"
fi
