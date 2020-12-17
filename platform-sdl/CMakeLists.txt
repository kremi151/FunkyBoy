cmake_minimum_required(VERSION 3.13)
project(fb_sdl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(DetectOS)

set(SOURCES
        source/main.cpp
        source/controllers/serial_sdl.cpp
        source/controllers/display_sdl.cpp
        source/ui/native_ui_windows.cpp
        source/ui/native_ui_gtk.cpp
        source/window/window.cpp
        )

set(HEADERS
        source/controllers/serial_sdl.h
        source/controllers/display_sdl.h
        source/ui/native_ui.h
        source/window/window.h
        )

set(DARWIN_SOURCES
        source/ui/native_ui_cocoa.mm
        )

if (OS_MACOS)
        set(SOURCES ${SOURCES} ${DARWIN_SOURCES})
endif()

find_package(SDL2 REQUIRED)

if (OS_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)
    find_package(X11 REQUIRED)
endif()

if (SDL2_FOUND)
    set(FB_EXTERNAL_LIBS ${FB_EXTERNAL_LIBS} ${SDL2_LIBRARIES})
    set(FB_EXTERNAL_INCLUDE_DIRS ${FB_EXTERNAL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2 was not found")
endif()

if (GTK_FOUND)
    set(FB_EXTERNAL_LIBS ${FB_EXTERNAL_LIBS} ${GTK_LIBRARIES})
    set(FB_EXTERNAL_INCLUDE_DIRS ${FB_EXTERNAL_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS})

    add_compile_definitions(FB_USE_GTK)
elseif(OS_LINUX)
    message(FATAL_ERROR "GTK was not found")
endif()

if (X11_FOUND)
    set(FB_EXTERNAL_LIBS ${FB_EXTERNAL_LIBS} ${X11_LIBRARIES})
    set(FB_EXTERNAL_INCLUDE_DIRS ${FB_EXTERNAL_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
elseif(OS_LINUX)
    message(FATAL_ERROR "X11 was not found")
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
