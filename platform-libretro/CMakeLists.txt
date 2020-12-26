cmake_minimum_required(VERSION 3.13)
project(fb_libretro CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(FunkyBoyVersion)

if(NOT CMAKE_BUILD_TYPE)
    message("Setting CMAKE_BUILD_TYPE to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Release is recommended for best performance)" FORCE)
elseif(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(WARNING "Using CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, but Release is recommended for best performance during runtime")
endif()

set(SOURCES
        source/main.cpp
        source/display_libretro.cpp
        source/joypad_libretro.cpp
        )

set(HEADERS
        include/libretro.h
        source/display_libretro.h
        source/joypad_libretro.h
        )

add_library(fb_libretro SHARED ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

# Write fb_libretro.info in build directory
configure_file(source/fb_libretro.info.in ${CMAKE_BINARY_DIR}/fb_libretro.info @ONLY)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")

set_target_properties(fb_libretro PROPERTIES PREFIX "") # no "lib" prefix for built binary on Linux
set_property(TARGET fb_core PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(fb_libretro fb_core)
