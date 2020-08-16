# FunkyBoy - Nintendo 3DS

![Build 3DS platform](https://github.com/kremi151/FunkyBoy/workflows/Build%203DS%20platform/badge.svg)

This is the implementation of FunkyBoy for the Nintendo 3DS.

This is more like a proof of concept of the portability of FunkyBoy. \
The emulation is currently quite slow (about ~30 instead of 60 FPS). \
At a later stage of development, I might consider actively working on improving the emulator's performance on the 3DS.

## Build 

1. Install devkitARM

Follow the [official Tutorial](https://devkitpro.org/wiki/Getting_Started)

2. Compile

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

The `CMakeLists.txt` file is configured to compile by default a `Release` binary for best performance during runtime.
Please do not select a manual `CMAKE_BUILD_TYPE` of type `Debug` as that one will slow down emulation even more!

## How to use

This is a very bare bone port of FunkyBoy, there is so far no GUI that allows you to pick ROMs. \
FunkyBoy looks for a ROM, relatively to the folder where the `.3dsx` binary lives, at `./funkyboy3DS/game.gb`. \
If a ROM has been found at that location, it gets loaded at the startup of FunkyBoy.
