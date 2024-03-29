cmake_minimum_required(VERSION 3.13)
project(fb_sdl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake-common)

include(FunkyBoyVersion)
include(DetectOS)

if (NOT OS_Detected)
    message(FATAL_ERROR "Unsupported OS")
endif()

set(SOURCES
        source/main.cpp
        source/controllers/serial_sdl.cpp
        source/controllers/display_sdl.cpp
        source/controllers/audio_sdl.cpp
        source/ui/native_ui_windows.cpp
        source/ui/native_ui_gtk.cpp
        source/window/window.cpp
        )

set(HEADERS
        source/controllers/serial_sdl.h
        source/controllers/display_sdl.h
        source/controllers/audio_sdl.h
        source/ui/native_ui.h
        source/window/window.h
        )

set(DARWIN_SOURCES
        source/ui/native_ui_cocoa.mm
        )

set(EXTRA_EXEC_DEPS
        )

if (OS_MACOS)
        set(SOURCES ${SOURCES} ${DARWIN_SOURCES})
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

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

if (OS_WINDOWS)
    set(fb_win32_ICON ${CMAKE_SOURCE_DIR}/win32_appicon.rc)
    set(EXTRA_EXEC_DEPS ${EXTRA_EXEC_DEPS} ${fb_win32_ICON})
endif()

if (OS_MACOS)
    set(MACOSX_BUNDLE_ICON_FILE funkyboy.icns)
    set(fb_macos_ICON ${CMAKE_CURRENT_BINARY_DIR}/funkyboy.icns)
    set(EXTRA_EXEC_DEPS ${EXTRA_EXEC_DEPS} ${fb_macos_ICON})
    set_source_files_properties(${fb_macos_ICON} PROPERTIES
       MACOSX_PACKAGE_LOCATION "Resources")

    add_custom_command(
        OUTPUT ${fb_macos_ICON}
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/mac_os_create_icon.sh "${CMAKE_SOURCE_DIR}/../resources" "${CMAKE_CURRENT_BINARY_DIR}"
    )

    add_executable(fb_sdl MACOSX_BUNDLE ${EXTRA_EXEC_DEPS} ${SOURCES} ${HEADERS})

    set_target_properties(fb_sdl PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_GUI_IDENTIFIER lu.kremi151.funkyboy.sdl
        MACOSX_BUNDLE_BUNDLE_NAME ${FB_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${FB_VERSION_MAJOR}.${FB_VERSION_MINOR}.${FB_VERSION_PATCH}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${FB_VERSION}
    )

    set(fb_macos_PACKED_BINARY "${CMAKE_CURRENT_BINARY_DIR}/packed/fb_sdl.app/Contents/MacOS/fb_sdl")

    add_custom_command(
        OUTPUT ${fb_macos_PACKED_BINARY}
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/macos_pack_bundle.sh ./fb_sdl.app
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS fb_sdl
    )

    add_custom_target(app_bundle DEPENDS ${fb_macos_PACKED_BINARY})
else()
    add_executable(fb_sdl ${EXTRA_EXEC_DEPS} ${SOURCES} ${HEADERS})
endif()

if (OS_WINDOWS)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        # Don't set Win32 app type for debug because we want logging to remain possible
    else()
        set_property(TARGET fb_sdl PROPERTY WIN32_EXECUTABLE true)
        target_compile_definitions(fb_sdl PRIVATE FB_WIN32)
    endif()
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories(${FB_EXTERNAL_INCLUDE_DIRS})

if (OS_MACOS)
        set_source_files_properties(${DARWIN_SOURCES} PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

target_link_libraries(fb_sdl fb_core ${FB_EXTERNAL_LIBS})

fb_use_autosave(fb_core)
fb_use_sound(fb_core)

add_definitions(-DSDL_MAIN_HANDLED)
