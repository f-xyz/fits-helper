# FITS Helper

## Usage
```sh

###############################
# Sorting images by sharpness #
###############################

./fits-helper.sh analyze \
  -s better \ # Select images better than
  -p 0.95 \ # 95% percentile
  -f ~/images/M101/*

./fits-helper.sh move \
  -s better \ # Select images better than
  -p 0.95 \ # 5% percentile
  -f ~/images/M101/*
  -d _good \ # into "_good" destination directory

./fits-helper.sh move \
  -s worse \ # Select images worse than
  -p 0.05 \ # 5% percentile
  -f ~/images/M101/*
  -d _bad \ # into "_bad" destination directory

################################
# Breaking down by sub-folders #
################################

./fits-helper.sh chop -s 10 -f images/ngc3675/* --dark images/calibration/dark.fit --flat images/calibration/flat.fit --bias images/calibration/bias.fit

./fits-helper.sh unchop --dir images/ngc3675
```

## Prerequisites

```sh
sudo apt update
sudo apt install build-essential cmake gdb mold
sudo apt install libopencv-dev # OpenCV
sudo apt install libcfitsio-dev # CFITSIO
```

## Building

```sh
./build.sh
```