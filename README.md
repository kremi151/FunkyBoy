# FunkyBoy

![Build core](https://github.com/kremi151/FunkyBoy/workflows/Build%20core/badge.svg)
![Test](https://github.com/kremi151/FunkyBoy/workflows/Test/badge.svg)

A Game Boy emulator written in C++.

## Features

FunkyBoy is currently at advanced stage.
Most of the games are already playable, albeit with some minor graphical glitches and currently without sound.

|Feature|Core|SDL|Libretro|Android|3DS|PSP|
|-|-|-|-|-|-|-|
|Graphical output|:heavy_check_mark:*|:heavy_check_mark:*|:heavy_check_mark:*|:heavy_check_mark:*|:heavy_check_mark:*|:heavy_check_mark:*|
|Button inputs|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|Sound ([#51](https://github.com/kremi151/FunkyBoy/issues/51))|:heavy_check_mark:|:heavy_check_mark:\*\*\*|:heavy_check_mark:\*\*\*|:x:|:x:|:x:|
|MBC1|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|MBC1 Multicard ([#53](https://github.com/kremi151/FunkyBoy/issues/53))|:x:|:x:|:x:|:x:|:x:|:x:|
|MBC2|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|MBC3 / MBC30|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|MBC5|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|MBC6|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|
|MBC7|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|:no_entry_sign:|
|Real-time clock (RTC)|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:question:|:question:|
|Save games|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:x:|:x:|
|Save states ([#39](https://github.com/kremi151/FunkyBoy/issues/39))|:heavy_check_mark:|:eyes:|:eyes:|:x:**|:x:|:x:|

Legend:
- :heavy_check_mark:: Supported
- :eyes:: Experimental support, may be unstable and/or might be changed in the future
- :question:: In theory supported, but untested
- :x:: Not supported for now, but planned
- :no_entry_sign:: Not supported with no current plans to do so
- \*: Supported with some minor graphical glitches
- \*\*: Android uses save states for resuming the app state from the task manager, but there is currently no way to create save states manually
- \*\*\*: Sound may have some cracks here and there during playback

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

## References

See [References used during development](./docs/REFERENCES.md)
