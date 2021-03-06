# FunkyBoy

![Build core](https://github.com/kremi151/FunkyBoy/workflows/Build%20core/badge.svg)
![Test](https://github.com/kremi151/FunkyBoy/workflows/Test/badge.svg)

A Game Boy emulator written in C++.

## Features

FunkyBoy is currently at advanced stage.
Most of the games are already playable, albeit with some minor graphical glitches and currently without sound.

|Feature name|Status|
|------------|------|
|Graphical output|:heavy_check_mark: Supported with some minor graphical glitches|
|Button inputs|:heavy_check_mark: Supported|
|Sound|:x: Not started ([Issue #51](https://github.com/kremi151/FunkyBoy/issues/51))|
|MBC1|:heavy_check_mark: Supported|
|MBC1 Multicard|:x: Not started ([Issue #53](https://github.com/kremi151/FunkyBoy/issues/53))|
|MBC2|:heavy_check_mark: Supported|
|MBC3 / MBC30|:heavy_check_mark: Supported|
|MBC5|:heavy_check_mark: Supported|
|MBC6|:x: Not supported|
|MBC7|:x: Not supported|
|Real-time clock (RTC) as part of MBC3[0]|:heavy_check_mark: Supported|
|Save games|:heavy_check_mark: Supported in SDL, libretro and Android implementations|
|Save states|:eyes: Experimental support in SDL and libretro ([Issue #39](https://github.com/kremi151/FunkyBoy/issues/39))|

## Compatible games
See [Compatible games](./docs/COMPATIBLE_GAMES.md)

## Implementations

FunkyBoy is developed with keeping portability in mind.
The [core](https://github.com/kremi151/FunkyBoy/tree/master/core) is developed as an isolated module which can be easily integrated in a platform specific implementation.

The implementations of FunkyBoy are classified as primary or secondary implementations.
The main focus lies on the primary implementations to get them working properly.
Secondary implementations provide basic functionality, but may experience slow emulation.
Those performance issues will be tackled at a later stage of development.

|Platform|Classification|CI status|
|--------|--------------|---------|
|[SDL](https://github.com/kremi151/FunkyBoy/tree/master/platform-sdl) (Linux, macOS & Windows)|Primary|![Build SDL platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20SDL%20platform/badge.svg)|
|[Libretro (RetroArch)](https://github.com/kremi151/FunkyBoy/tree/master/platform-libretro)|Primary|![Build Libretro](https://github.com/kremi151/FunkyBoy/workflows/Build%20Libretro/badge.svg)|
|[Android](https://github.com/kremi151/FunkyBoyAndroid)|Primary|![CI](https://github.com/kremi151/FunkyBoyAndroid/workflows/CI/badge.svg)|
|[Nintendo 3DS](https://github.com/kremi151/FunkyBoy/tree/master/platform-3ds)|Secondary|![Build 3DS platform](https://github.com/kremi151/FunkyBoy/workflows/Build%203DS%20platform/badge.svg)|
|[PlayStation Portable](https://github.com/kremi151/FunkyBoy/tree/master/platform-psp)|Secondary|![Build PSP platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20PSP%20platform/badge.svg)|
|[Tests](https://github.com/kremi151/FunkyBoy/tree/master/test)| |![Test](https://github.com/kremi151/FunkyBoy/workflows/Test/badge.svg)|
