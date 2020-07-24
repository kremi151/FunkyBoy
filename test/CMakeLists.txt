cmake_minimum_required(VERSION 3.13)
project(fb_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

set(SOURCES
        source/controllers/serial_test.cpp
        source/commons.cpp
        source/rom_cpu_instrs_tests.cpp
        source/unit_tests.cpp
        source/main.cpp
        )

set(HEADERS
        source/controllers/serial_test.h
        source/commons.h
        source/rom_cpu_instrs_tests.h
        source/unit_tests.h
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFB_TESTING")

find_package(Filesystem REQUIRED)

add_executable(fb_tests ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/acacia")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/acacia/source")

target_link_libraries(fb_tests fb_core acacia CXX::Filesystem)
