# - Try to find sf2d
# You can set SF2D_ROOT to specify a certain directory to look in first.
# Once done this will define
#  SF2D_FOUND - System has sf2d
#  SF2D_INCLUDE_DIRS - The sf2d include directories
#  SF2D_LIBRARIES - The libraries needed to use sf2d
# Unless we are unable to find CITRO3D
# It also adds an imported target named `3ds::sf2d`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${SF2D_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${SF2D_INCLUDE_DIRS})

if(NOT 3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

include(LibFindMacros)
include(try_add_imported_target)

# sf2d requires citro3d
libfind_package(SF2D CITRO3D)

set(_SF2D_SEARCHES)

# Search SF2D_ROOT first if it is set.
if(SF2D_ROOT)
  set(_SF2D_SEARCH_ROOT
    PATHS ${SF2D_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _SF2D_SEARCHES _SF2D_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM} etc.
set(_SF2D_SEARCH_NORMAL
  PATHS / /libsf2d /sf2dlib /libctru /ctrulib
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _SF2D_SEARCHES _SF2D_SEARCH_NORMAL)

foreach(search ${_SF2D_SEARCHES})
  find_path(SF2D_INCLUDE_DIR NAMES sf2d.h
    ${${search}}
    PATH_SUFFIXES include libsf2d/include)
  find_library(SF2D_LIBRARY NAMES sf2d libsf2d.a
    ${${search}}
    PATH_SUFFIXES lib libsf2d/lib)
endforeach()

set(SF2D_PROCESS_INCLUDES SF2D_INCLUDE_DIR)
set(SF2D_PROCESS_LIBS SF2D_LIBRARY)

libfind_process(SF2D)

try_add_imported_target(SF2D 3ds::citro3d)
