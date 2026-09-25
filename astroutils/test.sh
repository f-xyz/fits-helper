#!/usr/bin/env bash
set -euo pipefail

########################################
# Building (Debug) #####################
########################################

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=OFF
CLICOLOR_FORCE=1 cmake --build build --parallel \
  | tee >(sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,3})*)?[mGK]//g" > build.log)

########################################
# Running Unit Tests ###################
########################################

./build/tests "$@" # --gtest_filter="*"