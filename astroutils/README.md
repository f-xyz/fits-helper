# Astro Utils Library

Astro Utils is a C++23 utility library for astronomy and scientific applications. It provides helpers for filesystem access, command execution, logging, CLI presentation, asynchronous tasks, image processing, and FITS file reading (tested with ZWO cameras!).

## Prerequisites

```sh
sudo apt update
sudo apt install build-essential cmake gdb mold
sudo apt install libopencv-dev # OpenCV
sudo apt install libcfitsio-dev # CFITSIO
```

## Building and Running Unit Tests

```sh
# Running all tests
./test.sh --gtest_filter="*"
# With coverage
./testWithCoverage.sh
```