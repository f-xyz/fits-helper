# FITS Helper

## Usage
```sh
./fits-helper.sh analyze \
  -s better \ # Select images better than
  -p 0.95 \ # 95% percentile
  -f /home/andr/ASILive/Data/2026/3.Summer2/M101/*

./fits-helper.sh move \
  -s worse \ # Select images worse than
  -p 0.05 \ # 5% percentile
  -f /home/andr/ASILive/Data/2026/3.Summer2/M101/*
  -d _bad \ # into "_bad" destination directory

./fits-helper.sh move \
  -s worse \ # Select images worse than
  -p 0.05 \ # 5% percentile
  -f /home/andr/ASILive/Data/2026/3.Summer2/M101/*
  -d _bad \ # into "_bad" destination directory
```

## Prerequisites

```sh
sudo apt update
sudo apt install build-essential cmake gdb
sudo apt install libopencv-dev # OpenCV
sudo apt install libcfitsio-dev # CFITSIO
```

## Building

```sh
./build.sh
```