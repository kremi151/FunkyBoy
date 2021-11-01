# FunkyBoy - SDL2

![Build SDL platform](https://github.com/kremi151/FunkyBoy/workflows/Build%20SDL%20platform/badge.svg)

This is the implementation of FunkyBoy for Windows, Linux and macOS

## Platform support

| Platform            | Support            | Additional required libraries |
|---------------------|--------------------|-------------------------------|
| Windows             | :heavy_check_mark: | SDL2                          |
| macOS               | :heavy_check_mark: | SDL2                          |
| Ubuntu              | :heavy_check_mark: | SDL2, GTK3                     |
| Other Linux distros | :question:         | SDL2, GTK3                     |

## Controls

|Action|Key|
|------|---|
|A button|Q|
|B button|W|
|Start button|O|
|Select button|P|
|DPad Up|Up|
|DPad Down|Down|
|DPad Left|Left|
|DPad Right|Right|
|Toggle fullscreen|F|
|Create save state|H|
|Load save state|J|

## Command line arguments

Usage:
```shell
fb_sdl [options] [path to rom]
```

Options:

|Argument|Argument (short)|Description|
|---|---|---|
|--test|-t|Test whether the application can start correctly|
|--full-screen|-f|Launch emulator in full screen mode|
|--auto-resume|-a|Automatically saves the game state and resumes the next time when emulator is opened again using this flag|
|--help|-h|Print usage|

## Build on Ubuntu

1. Install SDL2, GTK3 and CMake:

```shell
sudo add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu `lsb_release -sc` main universe restricted multiverse"
sudo apt-get update -y -qq
sudo apt-get install libsdl2-dev libgtk-3-dev libx11-dev cmake
```
2. Compile:

```shell
mkdir -p cmake-build && cd cmake-build
cmake ..
make
```

## Build on macOS

1. Install [Homebrew](https://brew.sh/)

2. Install SDL2 library and CMake:

```shell
brew install sdl2 cmake
```

3. Compile:

```shell
mkdir -p cmake-build && cd cmake-build
cmake ..
make
```

Or if you want to create an installable app bundle, you can perform the following instead of just `make`:

```shell
make app_bundle
```

To work in Xcode, generate Xcode project files:

```shell
mkdir -p xcode && cd xcode
cmake -G Xcode ..
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