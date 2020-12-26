# FunkyBoy - PlayStation Portable

![Build PSP platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20PSP%20platform/badge.svg)

This is the implementation of FunkyBoy for the PlayStation Portable.

This is more like a proof of concept of the portability of FunkyBoy. \
The emulation is currently horribly slow (slower than the 3DS implementation). \
At a later stage of development, I might consider actively working on improving the emulator's performance on the PSP.

## Build 

1. Install the [pspdev toolchain](https://github.com/pspdev/psptoolchain)

2. Compile

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```

The `CMakeLists.txt` file is configured to compile by default a `Release` binary for best performance during runtime.
Please do not select a manual `CMAKE_BUILD_TYPE` of type `Debug` as that one will slow down emulation even more!

## How to use

This is a very bare bone port of FunkyBoy, there is so far no GUI that allows you to pick ROMs. \
FunkyBoy looks for a ROM, relatively to the root of your Memory Stick, at `./funkyboy/game.gb`. \
If a ROM has been found at that location, it gets loaded at the startup of FunkyBoy.
