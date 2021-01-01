cmake_minimum_required(VERSION 3.13)
project(fb_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

set(SOURCES
        source/controllers/serial_test.cpp
        source/util/rom_commons.cpp
        source/blargg/rom_cpu_instrs_tests.cpp
        source/unit_tests.cpp
        source/mooneye/rom_mooneye_mbc1.cpp
        source/mooneye/rom_mooneye_mbc2.cpp
        source/mooneye/rom_mooneye_mbc5.cpp
        source/mooneye/rom_acceptance.cpp
        source/perf_mode.cpp
        source/main.cpp
        )

set(HEADERS
        source/controllers/serial_test.h
        source/util/rom_commons.h
        source/util/membuf.h
        source/blargg/rom_cpu_instrs_tests.h
        source/unit_tests.h
        source/mooneye/rom_mooneye_mbc1.h
        source/mooneye/rom_mooneye_mbc2.h
        source/mooneye/rom_mooneye_mbc5.h
        source/mooneye/commons.h
        source/blargg/commons.h
        source/blargg/blargg_tests.h
        source/mooneye/mooneye_tests.h
        source/mooneye/rom_acceptance.h
        source/perf_mode.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFB_TESTING")

add_executable(fb_tests ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/acacia")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/acacia/source")

target_link_libraries(fb_tests fb_core acacia)
