/**
 * Copyright 2020 Michel Kremer (kremi151)
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FUNKYBOY_CORE_TYPEDEFS_H
#define FUNKYBOY_CORE_TYPEDEFS_H

#include <cstdint>

#define __fb_versionStr_indirect(major, minor, patch) "" #major "." #minor "." #patch
#define __fb_versionStr(major, minor, patch) __fb_versionStr_indirect(major, minor, patch)

#define FB_NAME "FunkyBoy"
#define FB_VERSION_MAJOR 0
#define FB_VERSION_MINOR 1
#define FB_VERSION_PATCH 0
#define FB_VERSION __fb_versionStr(FB_VERSION_MAJOR, FB_VERSION_MINOR, FB_VERSION_PATCH) "-Alpha"

#define FB_GB_DISPLAY_WIDTH 160
#define FB_GB_DISPLAY_HEIGHT 144

#define FB_CAST_8_TO_16_BIT(x) static_cast<u16*>(static_cast<void*>(x))

namespace FunkyBoy {

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef u32 memory_address;

    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef int ret_code;

}

#endif //FUNKYBOY_CORE_TYPEDEFS_H
