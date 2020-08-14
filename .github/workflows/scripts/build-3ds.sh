#!/bin/sh

tmp_dir=$(mktemp -d -t ci-XXXXXXXXXX)

echo "Upgrade to newer CMake"
sudo apt-get purge cmake
cd $tmp_dir
wget https://github.com/Kitware/CMake/releases/download/v3.18.1/cmake-3.18.1.tar.gz
tar -xzvf cmake-3.18.1.tar.gz
cd cmake-3.18.1
./bootstrap
make -j4
sudo make install

echo "Test CMake installation"
cd ~
cmake --version

echo "Build 3DS platform"
cd /github/workspace/platform-3ds
mkdir cmake-build-3ds
cd cmake-build-3ds
cmake ..
make
