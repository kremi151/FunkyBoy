cmake_minimum_required(VERSION 3.15)
project(core)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        library.cpp
        source/emulator/emulator.cpp
        source/emulator/cpu.cpp
        source/util/romsizes.cpp
        source/cartridge/cartridge.cpp
        source/memory/memory.cpp
        )

set(HEADERS
        library.h
        source/emulator/emulator.h
        source/emulator/cpu.h
        source/util/typedefs.h
        source/cartridge/header.h
        source/util/romsizes.h
        source/cartridge/cartridge.h
        source/memory/memory.h
        )

#add_library(core SHARED ${SOURCES} ${HEADERS})
add_executable(core ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")