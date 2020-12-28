cmake_minimum_required(VERSION 3.13)
project(fb_core CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(FunkyBoyVersion)

if (MSVC)
    message("We're running in Visual Studio!")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(MSVC)

set(SOURCES
        source/emulator/emulator.cpp
        source/emulator/io_registers.cpp
        source/emulator/cpu.cpp
        source/emulator/ppu.cpp
        source/controllers/controllers.cpp
        source/controllers/serial_null.cpp
        source/controllers/joypad_void.cpp
        source/controllers/display_void.cpp
        source/util/romsizes.cpp
        source/cartridge/cartridge.cpp
        source/memory/memory.cpp
        source/cartridge/mbc1.cpp
        source/cartridge/mbc2.cpp
        source/cartridge/mbc_none.cpp
        source/memory/ppu_memory.cpp
        source/operands/instruction_context.cpp
        source/operands/alu.cpp
        source/operands/loads.cpp
        source/operands/misc.cpp
        source/operands/prefix.cpp
        source/operands/reads.cpp
        source/operands/writes.cpp
        source/operands/jumps.cpp
        source/operands/rot_shifts.cpp
        source/operands/conditions.cpp
        source/operands/debug.cpp
        source/operands/registry.cpp
        source/operands/table_unprefixed_instructions.cpp
        source/util/registers.cpp
        source/util/flags.cpp
        source/exception/state_exception.cpp
        )

set(HEADERS
        source/emulator/emulator.h
        source/emulator/gb_type.h
        source/emulator/io_registers.h
        source/emulator/cpu.h
        source/emulator/ppu.h
        source/controllers/controllers.h
        source/controllers/serial.h
        source/controllers/serial_null.h
        source/controllers/joypad.h
        source/controllers/joypad_void.h
        source/controllers/display.h
        source/controllers/display_void.h
        source/util/typedefs.h
        source/cartridge/header.h
        source/util/romsizes.h
        source/util/ramsizes.h
        source/util/fs.h
        source/cartridge/cartridge.h
        source/memory/memory.h
        source/util/debug.h
        source/util/testing.h
        source/cartridge/mbc.h
        source/cartridge/mbc1.h
        source/cartridge/mbc2.h
        source/cartridge/mbc_none.h
        source/memory/ppu_memory.h
        source/operands/instruction_context.h
        source/operands/alu.h
        source/operands/loads.h
        source/operands/instructions.h
        source/operands/misc.h
        source/operands/prefix.h
        source/operands/reads.h
        source/operands/writes.h
        source/operands/jumps.h
        source/operands/rot_shifts.h
        source/operands/conditions.h
        source/operands/debug.h
        source/operands/registry.h
        source/operands/tables.h
        source/util/endianness.h
        source/util/registers.h
        source/util/flags.h
        source/util/gpumode.h
        source/util/return_codes.h
        source/util/string_polyfills.h
        source/palette/dmg_palette.h
        source/exception/state_exception.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

find_package(Filesystem REQUIRED)

add_library(fb_core STATIC ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

target_link_libraries(fb_core CXX::Filesystem)
