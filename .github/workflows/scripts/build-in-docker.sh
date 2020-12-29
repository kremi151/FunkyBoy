#!/bin/sh

# Arguments:
# $1 : source path relative to the workspace root

FB_SRC_DIR=/github/workspace/$1

echo "Upgrade to newer CMake"
new_cmake_dir="$HOME/tmp_cmake"
mkdir $new_cmake_dir
cd $new_cmake_dir
wget -q https://cmake.org/files/v3.18/cmake-3.18.1-Linux-x86_64.tar.gz
tar -xzf cmake-3.18.1-Linux-x86_64.tar.gz
export PATH="$new_cmake_dir/cmake-3.18.1-Linux-x86_64/bin:$PATH"

echo "Test CMake installation"
cd ~
cmake --version

echo "Build FunkyBoy at $FB_SRC_DIR"
cd $FB_SRC_DIR
mkdir cmake-build
cd cmake-build
cmake ..
make
