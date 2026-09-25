#!/usr/bin/env bash
set -euo pipefail

########################################
# Building (Release) ###################
########################################

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_COVERAGE=OFF
CLICOLOR_FORCE=1 cmake --build build --parallel 4 \
  | tee >(sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,3})*)?[mGK]//g" > build.log)

########################################
# Running Unit Tests ###################
########################################

./build/tests "$@" # --gtest_filter="*"

########################################
# Installing ###########################
########################################

sudo cmake --install build
