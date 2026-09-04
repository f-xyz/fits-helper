#!/usr/bin/env bash
set -euo pipefail

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=OFF
cmake --build build -j4

echo "########################################"

./build/tests "$@"