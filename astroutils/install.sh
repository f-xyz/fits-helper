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
cmake --build build -j4

########################################
# Running Unit Tests ###################
########################################

./build/tests "$@" # --gtest_filter="*"

########################################
# Installing ###########################
########################################

sudo cmake --install build
