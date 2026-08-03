# FITS Culler

## Usage
```sh
./culler.sh \
  -s worse \ # Select images worse than
  -p 0.05 \ # 5% percentile
  -o move \ # and move
  -d _bad \ # into "_bad" destination directory
  -f /home/andr/ASILive/Data/2026/3.Summer2/M101/*
  
./culler.sh \
  -s better \ # Select images better than
  -p 0.95 \ # 95% percentile
  -o view \ # and just report, don't move
  -d _best \ # destination directory name is mandatory anyway
  -f /home/andr/ASILive/Data/2026/3.Summer2/M101/*
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