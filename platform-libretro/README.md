# FunkyBoy - Libretro (RetroArch)

![Build Libretro](https://github.com/kremi151/FunkyBoy/workflows/Build%20Libretro/badge.svg)

This is the implementation of FunkyBoy for RetroArch.

## Build preparations

To build this Libretro core yourself, you first have to download the [libretro.h](https://github.com/libretro/RetroArch/blob/master/libretro-common/include/libretro.h) header manually and put it into `include/libretro.h`

## Build on Ubuntu

1. Install CMake:

```
sudo add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu `lsb_release -sc` main universe restricted multiverse"
sudo apt-get update -y -qq
sudo apt-get install cmake
```
2. Compile:

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

## Build on macOS

1. Install [Homebrew](https://brew.sh/)

2. Install CMake:

```
brew install cmake
```

3. Compile:

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

## Build on Windows

1. Install CMake

2. Compile:

```
mkdir cmake-build
cd cmake-build
cmake ..
make
```
