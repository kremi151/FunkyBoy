# FunkyBoy - SDL2

![Build SDL platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20SDL%20platform/badge.svg)

This is the implementation of FunkyBoy for Windows, Linux and macOS

## Build on Ubuntu

1. Install SDL2 library and CMake:

```
sudo add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu `lsb_release -sc` main universe restricted multiverse"
sudo apt-get update -y -qq
sudo apt-get install libsdl2-dev cmake
```
2. Compile:

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

## Build on macOS

1. Install [Homebrew](https://brew.sh/)

2. Install SDL2 library and CMake:

```
brew install sdl2 cmake
```

3. Compile:

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

## Build on Windows

1. Download SDL2:
https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip
2. Extract the ZIP somewhere
3. Add a `SDL2_DIR` CMake variable which points to the folder contained in the extracted archive (the full path of the `SDL2-2.0.12` folder)
4. Add the the full path of `SDL2-2.0.12/lib/x64` to the `PATH`
5. Ensure that you're building for the correct architecture (just saying, I was building for `x86` without noticing...)

## Resources

https://gbdev.gg8.se/wiki/articles/Video_Display#VRAM_Tile_Data

http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf (Pages 22 to 27)