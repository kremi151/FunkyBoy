# - Try to find sfil
# You can set SFIL_ROOT to specify a certain directory to look in first.
# Once done this will define
#  SFIL_FOUND - System has sfil
#  SFIL_INCLUDE_DIRS - The sfil include directories
#  SFIL_LIBRARIES - The libraries needed to use sfil
# Unless we are unable to find JPEG, PNG or SF2D
# It also adds an imported target named `3ds::sfil`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${SFIL_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${SFIL_INCLUDE_DIRS})


if(NOT 3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

include(LibFindMacros)
include(try_add_imported_target)

# sfil requires jpeg, png and sf2d
libfind_package(SFIL JPEG)
libfind_package(SFIL PNG)
libfind_package(SFIL SF2D)

set(_SFIL_SEARCHES)

# Search SFIL_ROOT first if it is set.
if(SFIL_ROOT)
  set(_SFIL_SEARCH_ROOT
    PATHS ${SFIL_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _SFIL_SEARCHES _SFIL_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM} etc.
set(_SFIL_SEARCH_NORMAL
  PATHS / /libsfil /sfillib /libctru /ctrulib
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _SFIL_SEARCHES _SFIL_SEARCH_NORMAL)

foreach(search ${_SFIL_SEARCHES})
  find_path(SFIL_INCLUDE_DIR NAMES sfil.h
    ${${search}}
    PATH_SUFFIXES include libsfil/include)
  find_library(SFIL_LIBRARY NAMES sfil libsfil.a
    ${${search}}
    PATH_SUFFIXES lib libsfil/lib)
endforeach()

set(SFIL_PROCESS_INCLUDES SFIL_INCLUDE_DIR)
set(SFIL_PROCESS_LIBS SFIL_LIBRARY)

libfind_process(SFIL)

try_add_imported_target(SFIL 3ds::jpeg 3ds::png 3ds::sf2d)
