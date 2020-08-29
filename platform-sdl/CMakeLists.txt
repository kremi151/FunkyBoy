cmake_minimum_required(VERSION 3.13)
project(fb_sdl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

set(SOURCES
        source/main.cpp
        source/controllers/serial_sdl.cpp
        source/controllers/joypad_sdl.cpp
        source/controllers/display_sdl.cpp
        source/ui/file_picker_windows.cpp
        source/window/window.cpp
        )

set(HEADERS
        source/controllers/serial_sdl.h
        source/controllers/joypad_sdl.h
        source/controllers/display_sdl.h
        source/ui/file_picker.h
        source/window/window.h
        )

set(DARWIN_SOURCES
        source/ui/file_picker_cocoa.mm
        )

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(SOURCES ${SOURCES} ${DARWIN_SOURCES})
endif()

find_package(SDL2 REQUIRED)

IF (NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 was not found")
ENDIF()

add_executable(fb_sdl ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories(${SDL2_INCLUDE_DIRS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_source_files_properties(${DARWIN_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

target_link_libraries(fb_sdl fb_core ${SDL2_LIBRARIES})
add_definitions(-DSDL_MAIN_HANDLED)
