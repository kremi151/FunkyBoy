set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/DevkitArm3DS.cmake)

cmake_minimum_required(VERSION 3.13)
project(fb_3ds CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

if(NOT CMAKE_BUILD_TYPE)
    message("Setting CMAKE_BUILD_TYPE to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Release is recommended for best performance)" FORCE)
elseif(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(WARNING "Using CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, but Release is recommended for best performance during runtime")
endif()

set(SOURCES
        source/main.cpp
        source/controllers/display_3ds.cpp
        source/controllers/joypad_3ds.cpp
        )

set(HEADERS
        source/controllers/display_3ds.h
        source/controllers/joypad_3ds.h
        )

# Use the devkitArm suggested flags. This is not mandatory.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DKA_SUGGESTED_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DKA_SUGGESTED_CXX_FLAGS} -fexceptions")

# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.

find_package(CTRULIB REQUIRED) # Look for ctrulib

# Include FunkyBoy core
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

# Create the elf file
add_executable(fb_3ds ${SOURCES} ${HEADERS})

# Link ctrulib, this will also setup the include directories !
target_link_libraries(fb_3ds 3ds::ctrulib)
target_link_libraries(fb_core 3ds::ctrulib)

set_target_properties(fb_3ds PROPERTIES LINKER_LANGUAGE CXX)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

target_link_libraries(fb_3ds fb_core)

add_3dsx_target(fb_3ds) # Generate the .3dsx and .smdh from the hello_world target
