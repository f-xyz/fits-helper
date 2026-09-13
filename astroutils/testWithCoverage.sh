#!/usr/bin/env bash
set -euo pipefail

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=ON
cmake --build build -j4

echo "----------------------------------------"

./build/tests "$@"

rm -fv build/**/*.gcda # for llvm-cov-18 only
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
