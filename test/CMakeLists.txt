cmake_minimum_required(VERSION 3.13)
project(fb_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

set(ACACIA_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/acacia")

add_subdirectory("${ACACIA_SOURCE_DIR}")
include_directories("${ACACIA_SOURCE_DIR}/source")

set_acacia_test_sources(
        source/blargg/rom_cpu_instrs_tests.cpp
        source/unit_tests/unit_tests.cpp
        source/unit_tests/rtc.cpp
        source/unit_tests/save_states.cpp
        source/mooneye/rom_mooneye_mbc1.cpp
        source/mooneye/rom_mooneye_mbc2.cpp
        source/mooneye/rom_mooneye_mbc5.cpp
        source/mooneye/rom_acceptance.cpp
)

set(SOURCES
        source/controllers/serial_test.cpp
        source/util/rom_commons.cpp
        source/util/mock_time.cpp
        ${ACACIA_TEST_SOURCES}
        source/perf_mode.cpp
        source/main.cpp
        )

set(HEADERS
        source/controllers/serial_test.h
        source/util/rom_commons.h
        source/util/mock_time.h
        ${ACACIA_TEST_HEADERS}
        source/mooneye/commons.h
        source/blargg/commons.h
        source/perf_mode.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFB_TESTING")

add_executable(fb_tests ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source" ${ACACIA_TEST_INCLUDE_DIRS})

target_link_libraries(fb_tests fb_core acacia)
