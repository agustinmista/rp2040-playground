#!/bin/bash

set -e

echo "*** Initializing submodules"
git submodule update --init

echo "*** Installing system tools"
sudo apt-get update
sudo apt-get install -y \
  autoconf \
  automake \
  build-essential \
  cmake \
  gcc-arm-none-eabi \
  libftdi-dev \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib \
  libtool \
  libusb-1.0-0-dev \
  pkg-config \
  screen \
  texinfo \
  udev \
  usbutils

echo "*** Building openocd from source"
pushd tools/openocd
./bootstrap
./configure
make -j8
popd

echo "*** Installing udev rules"
sudo cp tools/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload