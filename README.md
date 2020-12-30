# FunkyBoy

![Build core](https://github.com/kremi151/FunkyBoy/workflows/Build%20core/badge.svg)
![Build SDL platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20SDL%20platform/badge.svg)
![Build Libretro](https://github.com/kremi151/FunkyBoy/workflows/Build%20Libretro/badge.svg)
![Build 3DS platform](https://github.com/kremi151/FunkyBoy/workflows/Build%203DS%20platform/badge.svg)
![Build PSP platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20PSP%20platform/badge.svg)
![Test](https://github.com/kremi151/FunkyBoy/workflows/Test/badge.svg)

A GameBoy emulator written in C++.

## Features

FunkyBoy is currently at an early stage,
A lot of games are already playable, albeit with some graphical glitches and without sound.

|Feature name|Status|
|------------|------|
|Graphical output|:warning: Supported, but there are some graphical glitches|
|Button inputs|:heavy_check_mark: Supported|
|Sound|:x: Not started ([Issue #51](https://github.com/kremi151/FunkyBoy/issues/51))|
|MBC1|:heavy_check_mark: Supported|
|MBC1 Multicard|:x: Not started ([Issue #53](https://github.com/kremi151/FunkyBoy/issues/53))|
|MBC2|:heavy_check_mark: Supported|
|MBC3|:construction_worker: In progress ([Issue #37](https://github.com/kremi151/FunkyBoy/issues/37))|
|MBC5|:x: Not started ([Issue #52](https://github.com/kremi151/FunkyBoy/issues/52))|
|Save games|:question: Implemented, but to be tested|
|Save states|:x: Not started ([Issue #39](https://github.com/kremi151/FunkyBoy/issues/39))|

## Implementations

* [SDL](https://github.com/kremi151/FunkyBoy/tree/master/platform-sdl) (Linux, macOS & Windows)
* [Libretro (RetroArch)](https://github.com/kremi151/FunkyBoy/tree/master/platform-libretro)
* [Nintendo 3DS](https://github.com/kremi151/FunkyBoy/tree/master/platform-3ds)
* [PlayStation Portable](https://github.com/kremi151/FunkyBoy/tree/master/platform-psp)
* [Android](https://github.com/kremi151/FunkyBoyAndroid)
* [Tests](https://github.com/kremi151/FunkyBoy/tree/master/test)
