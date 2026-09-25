#!/usr/bin/env bash
set -euo pipefail
shopt -s globstar

########################################
# Building (Debug) #####################
########################################

# Removing previous GCDA files,
# needed only for llvm-cov-18
rm -f -- build/**/*.gcda

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=ON
CLICOLOR_FORCE=1 cmake --build build --parallel 4 \
  | tee >(sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,3})*)?[mGK]//g" > build.log)

########################################
# Running Unit Tests ###################
########################################

./build/tests "$@" # --gtest_filter="*"

########################################
# Coverage #############################
########################################

rm -rf coverage
mkdir -p coverage
cd coverage

# Set --gcov-executable to "/usr/bin/gcov-14" if using g++
gcovr \
  --gcov-executable "llvm-cov-18 gcov" \
  --gcov-object-directory ../build \
  -r .. \
  -f ../src/ \
  -f ../include/ \
  -e '.*build/.*' \
  -e '.*tests/.*' \
  --exclude-throw-branches \
  --exclude-unreachable-branches \
  --exclude-noncode-lines \
  --html-details \
  -o coverage.html