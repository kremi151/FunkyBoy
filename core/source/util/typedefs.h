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

#define FB_CAST_8_TO_16_BIT(x) static_cast<u16*>(static_cast<void*>(x))

#ifndef MSVC
// Stupid Visual Studio compiler...
#define fb_inline
#else
#define fb_inline inline
#endif

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

}

#endif //FUNKYBOY_CORE_TYPEDEFS_H
