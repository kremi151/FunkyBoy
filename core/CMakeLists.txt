cmake_minimum_required(VERSION 3.15)
project(fb_core)

set(CMAKE_CXX_STANDARD 17)

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
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

add_library(fb_core STATIC ${SOURCES} ${HEADERS})
#add_executable(fb_core ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")