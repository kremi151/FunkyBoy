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

#define FB_GB_DISPLAY_WIDTH 160
#define FB_GB_DISPLAY_HEIGHT 144

#define FB_TARGET_FPS 59.7154

#define FB_FORWARD_DECLARE class

// Buffer size which is guaranteed to be large enough to fit a save state in it
#define FB_SAVE_STATE_MAX_BUFFER_SIZE ((105 * 1024) + 200)

namespace FunkyBoy {

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef uint_fast8_t u8_fast;
    typedef uint_fast16_t u16_fast;
    typedef uint_fast32_t u32_fast;
    typedef uint_fast64_t u64_fast;

    typedef u16_fast memory_address;

    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef int ret_code;

}

#endif //FUNKYBOY_CORE_TYPEDEFS_H
