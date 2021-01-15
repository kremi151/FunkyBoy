cmake_minimum_required(VERSION 3.13)
project(fb_core CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(FunkyBoyVersion)
include(DetectOS)

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
        source/controllers/display_void.cpp
        source/util/romsizes.cpp
        source/memory/memory.cpp
        source/cartridge/mbc1.cpp
        source/cartridge/mbc2.cpp
        source/cartridge/mbc3.cpp
        source/cartridge/mbc5.cpp
        source/cartridge/mbc_none.cpp
        source/cartridge/rtc.cpp
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
        source/util/frame_executor.cpp
        source/exception/state_exception.cpp
        source/exception/read_exception.cpp
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
        source/controllers/display.h
        source/controllers/display_void.h
        source/util/typedefs.h
        source/cartridge/header.h
        source/util/romsizes.h
        source/util/ramsizes.h
        source/util/fs.h
        source/cartridge/status.h
        source/memory/memory.h
        source/util/debug.h
        source/util/testing.h
        source/cartridge/mbc.h
        source/cartridge/mbc1.h
        source/cartridge/mbc2.h
        source/cartridge/mbc3.h
        source/cartridge/mbc5.h
        source/cartridge/mbc_none.h
        source/cartridge/rtc.h
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
        source/util/frame_executor.h
        source/palette/dmg_palette.h
        source/exception/state_exception.h
        source/exception/read_exception.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

find_package(Filesystem REQUIRED)

add_library(fb_core STATIC ${SOURCES} ${HEADERS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

# Check whether we can include thread
check_include_file_cxx("thread" HAVE_STD_THREAD)
target_compile_definitions(fb_core PUBLIC -DHAS_STD_THREAD=$<BOOL:${HAVE_STD_THREAD}>)

# Check whether we can use std::this_thread
string(CONFIGURE [[
        #include <thread>

        int main() {
            std::this_thread::get_id();
            return 0;
        }
]] code @ONLY)
check_cxx_source_compiles("${code}" CAN_COMPILE_STD_THIS_THREAD)
target_compile_definitions(fb_core PUBLIC -DHAS_STD_THIS_THREAD=$<BOOL:${CAN_COMPILE_STD_THIS_THREAD}>)

# Check whether we can use usleep from unistd.h
string(CONFIGURE [[
        #include <unistd.h>

        int main() {
            usleep(0);
            return 0;
        }
]] code @ONLY)
check_cxx_source_compiles("${code}" CAN_COMPILE_UNISTD_USLEEP)
target_compile_definitions(fb_core PUBLIC -DHAS_UNISTD_USLEEP=$<BOOL:${CAN_COMPILE_UNISTD_USLEEP}>)

# Macros
macro(fb_use_autosave target)
    target_compile_definitions(${target} PUBLIC -DFB_USE_AUTOSAVE)
endmacro()

target_link_libraries(fb_core CXX::Filesystem)
