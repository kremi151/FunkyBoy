# FunkyBoy - Nintendo 3DS

![Build 3DS platform](https://github.com/kremi151/FunkyBoy/workflows/Build%203DS%20platform/badge.svg)

This is the implementation of FunkyBoy for the Nintendo 3DS.

This is more like a proof of concept of the porability of FunkyBoy. \
The emulation is currently extremely slow (about 7-8 FPS), mainly due to the fact that the code is not optimized enough for the 3DS' limited resources. \
At a later stage of development, I might consider actively working on improving the emulator's performance on the 3DS.

## Build 

1. Install devkitARM

Follow the [official Tutorial](https://devkitpro.org/wiki/Getting_Started)

2. Compile

```
mkdir -p cmake-build && cd cmake-build
cmake .. && make
```
