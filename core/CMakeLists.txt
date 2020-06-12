cmake_minimum_required(VERSION 3.13)
project(fb_core CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

if (MSVC)
    message("We're running in Visual Studio!")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(MSVC)

set(SOURCES
#        library.cpp
        source/emulator/emulator.cpp
        source/emulator/cpu.cpp
        source/util/romsizes.cpp
        source/cartridge/cartridge.cpp
        source/memory/memory.cpp
        source/memory/mbc1.cpp
        source/memory/mbc_none.cpp
        )

set(HEADERS
#        library.h
        source/emulator/emulator.h
        source/emulator/cpu.h
        source/util/typedefs.h
        source/cartridge/header.h
        source/util/romsizes.h
        source/cartridge/cartridge.h
        source/memory/memory.h
        source/util/debug.h
        source/util/testing.h
        source/memory/mbc.h
        source/memory/mbc1.h
        source/memory/mbc_none.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

add_library(fb_core STATIC ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")