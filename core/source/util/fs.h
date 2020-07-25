//
// Created by Michel Kremer on 25.07.2020.
//

#ifndef FB_CORE_INCLUDE_HELPER_FS_H
#define FB_CORE_INCLUDE_HELPER_FS_H

#if STD_FS_IS_EXPERIMENTAL

#include <experimental/filesystem>

namespace FunkyBoy {
    namespace fs = std::experimental::filesystem;
}

#else

#include <filesystem>

namespace FunkyBoy {
    namespace fs = std::filesystem;
}

#endif

#endif //FB_CORE_INCLUDE_HELPER_FS_H
