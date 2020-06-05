cmake_minimum_required(VERSION 3.15)
project(fb_tests)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        source/main.cpp
        )

set(HEADERS
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFB_DEBUG")

add_executable(fb_tests ${SOURCES} ${HEADERS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" fb_core_build)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/acacia")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../core/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/acacia/source")

target_link_libraries(fb_tests fb_core acacia)