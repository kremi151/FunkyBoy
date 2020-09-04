cmake_minimum_required(VERSION 3.13)
project(fb_sdl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(DetectOS)

set(SOURCES
        source/main.cpp
        source/controllers/serial_sdl.cpp
        source/controllers/joypad_sdl.cpp
        source/controllers/display_sdl.cpp
        source/ui/file_picker_windows.cpp
        source/ui/file_picker_qt.cpp
        source/window/window.cpp
        )

set(HEADERS
        source/controllers/serial_sdl.h
        source/controllers/joypad_sdl.h
        source/controllers/display_sdl.h
        source/ui/native_ui.h
        source/window/window.h
        )

set(DARWIN_SOURCES
        source/ui/file_picker_cocoa.mm
        )

if (OS_MACOS)
        set(SOURCES ${SOURCES} ${DARWIN_SOURCES})
endif()

find_package(SDL2 REQUIRED)

if (OS_LINUX)
    if (NOT QT_QMAKE_EXECUTABLE)
        # As we use QT only on Linux, we manually set QT_QMAKE_EXECUTABLE by executing "which qmake".
        # The FindQt5 script relies on setting some environment variables first to find the executable.
        execute_process(COMMAND which qmake OUTPUT_VARIABLE WHICH_QMAKE_OUT)
        string(REGEX REPLACE "\n$" "" WHICH_QMAKE_OUT "${WHICH_QMAKE_OUT}") # Trim output
        if (NOT "${WHICH_QMAKE_OUT}" STREQUAL "")
            SET(QT_QMAKE_EXECUTABLE ${WHICH_QMAKE_OUT} CACHE PATH "Qt qmake program." FORCE)
        endif()
    endif()
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif()

if (SDL2_FOUND)
    set(FB_EXTERNAL_LIBS ${FB_EXTERNAL_LIBS} ${SDL2_LIBRARIES})
    set(FB_EXTERNAL_INCLUDE_DIRS ${FB_EXTERNAL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2 was not found")
endif()

if (QT5_FOUND)
    set(FB_EXTERNAL_LIBS ${FB_EXTERNAL_LIBS} ${QT_LIBRARIES})
    set(FB_EXTERNAL_INCLUDE_DIRS ${FB_EXTERNAL_INCLUDE_DIRS} ${QT_INCLUDES})
elseif(OS_LINUX)
    message(FATAL_ERROR "Qt5 was not found")
endif()

add_executable(fb_sdl ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories(${FB_EXTERNAL_INCLUDE_DIRS})

if (OS_MACOS)
        set_source_files_properties(${DARWIN_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

target_link_libraries(fb_sdl fb_core ${FB_EXTERNAL_LIBS})
add_definitions(-DSDL_MAIN_HANDLED)
