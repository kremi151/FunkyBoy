execute_process(COMMAND psp-config --psp-prefix OUTPUT_VARIABLE PSP_PREFIX)
string(REGEX REPLACE "\n$" "" PSP_PREFIX "${PSP_PREFIX}")

set(PSP_MODULE_PATH ${PSP_PREFIX}/share/cmake)

message(STATUS "PSP Prefix: ${PSP_PREFIX}")
message(STATUS "PSP Module path: ${PSP_MODULE_PATH}")

set(CMAKE_TOOLCHAIN_FILE ${PSP_MODULE_PATH}/PSP.cmake)

cmake_minimum_required(VERSION 3.13)
project(fb_psp CXX)

include(FunkyBoyVersion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PSP_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

if(NOT CMAKE_BUILD_TYPE)
    message("Setting CMAKE_BUILD_TYPE to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Release is recommended for best performance)" FORCE)
elseif(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(WARNING "Using CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, but Release is recommended for best performance during runtime")
endif()

set(SOURCES
        source/main.cpp
        source/callback.cpp
        source/user_input.cpp
        source/controllers/display_psp.cpp
        source/controllers/joypad_psp.cpp
        )

set(HEADERS
        source/callback.h
        source/user_input.h
        source/controllers/display_psp.h
        source/controllers/joypad_psp.h
        )

add_executable(fb_psp ${SOURCES} ${HEADERS})

# Include FunkyBoy core
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

set_target_properties(fb_psp PROPERTIES LINKER_LANGUAGE CXX)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

target_link_libraries(fb_psp -lstdc++ ${PSP_LIBRARIES} fb_core)

create_pbp_file(TARGET fb_psp TITLE ${FB_NAME})
