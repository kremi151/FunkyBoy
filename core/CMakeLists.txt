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
        source/emulator/io_registers.cpp
        source/emulator/cpu.cpp
        source/util/romsizes.cpp
        source/cartridge/cartridge.cpp
        source/memory/memory.cpp
        source/memory/mbc1.cpp
        source/memory/mbc_none.cpp
        source/instructions/instruction_context.cpp
        source/instructions/alu.cpp
        source/instructions/loads.cpp
        source/instructions/misc.cpp
        source/instructions/prefix.cpp
        source/instructions/reads.cpp
        source/instructions/writes.cpp
        source/instructions/jumps.cpp
        source/instructions/rot_shifts.cpp
        source/instructions/conditions.cpp
        source/instructions/debug.cpp
        source/util/endianness.cpp
        source/util/registers.cpp
        source/util/flags.cpp
        )

set(HEADERS
#        library.h
        source/emulator/emulator.h
        source/emulator/gb_type.h
        source/emulator/io_registers.h
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
        source/instructions/instruction_context.h
        source/instructions/alu.h
        source/instructions/loads.h
        source/instructions/instructions.h
        source/instructions/misc.h
        source/instructions/prefix.h
        source/instructions/reads.h
        source/instructions/writes.h
        source/instructions/jumps.h
        source/instructions/rot_shifts.h
        source/instructions/conditions.h
        source/instructions/debug.h
        source/util/endianness.h
        source/util/registers.h
        source/util/flags.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

add_library(fb_core STATIC ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")