#!/bin/sh

echo "Upgrade to newer CMake"
new_cmake_dir="$HOME/tmp_cmake"
mkdir $new_cmake_dir
cd $new_cmake_dir
wget https://cmake.org/files/v3.18/cmake-3.18.1-Linux-x86_64.tar.gz
tar -xzf cmake-3.18.1-Linux-x86_64.tar.gz
export PATH="$new_cmake_dir/cmake-3.18.1-Linux-x86_64/bin:$PATH"

echo "Test CMake installation"
cd ~
cmake --version

echo "Build 3DS platform"
cd /github/workspace/platform-3ds
mkdir cmake-build
cd cmake-build
cmake ..
make
