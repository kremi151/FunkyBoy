set(FB_NAME FunkyBoy)
set(FB_VERSION_MAJOR 0)
set(FB_VERSION_MINOR 1)
set(FB_VERSION_PATCH 0)
set(FB_VERSION ${FB_VERSION_MAJOR}.${FB_VERSION_MINOR}.${FB_VERSION_PATCH}-Alpha)

message(STATUS "Using ${FB_NAME} version ${FB_VERSION}")

add_compile_definitions(
    FB_NAME="${FB_NAME}"
    FB_VERSION_MAJOR=${FB_VERSION_MAJOR}
    FB_VERSION_MINOR=${FB_VERSION_MINOR}
    FB_VERSION_PATCH=${FB_VERSION_PATCH}
    FB_VERSION="${FB_VERSION}"
    )
