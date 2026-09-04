#!/usr/bin/env bash
set -euo pipefail

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=ON
cmake --build build -j4

echo "########################################"

./build/tests "$@"

rm -rf coverage
mkdir -p coverage
cd coverage
gcovr \
  --gcov-executable /usr/bin/gcov-14 \
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
