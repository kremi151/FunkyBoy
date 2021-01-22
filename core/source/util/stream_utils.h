/**
 * Copyright 2021 Michel Kremer (kremi151)
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

#ifndef FB_CORE_UTIL_STREAM_UTILS_H
#define FB_CORE_UTIL_STREAM_UTILS_H

#include <iostream>

namespace FunkyBoy::Util::Stream {

    inline void write64Bits(uint64_t val, std::ostream &ostream) {
        ostream.put(val & 0xff);
        ostream.put((val >> 8) & 0xff);
        ostream.put((val >> 16) & 0xff);
        ostream.put((val >> 24) & 0xff);
        ostream.put((val >> 32) & 0xff);
        ostream.put((val >> 40) & 0xff);
        ostream.put((val >> 48) & 0xff);
        ostream.put((val >> 56) & 0xff);
    }

    inline uint64_t read64Bits(std::istream &istream) {
        return uint64_t(istream.get() & 0xff)
               | (uint64_t(istream.get() & 0xff) << 8)
               | (uint64_t(istream.get() & 0xff) << 16)
               | (uint64_t(istream.get() & 0xff) << 24)
               | (uint64_t(istream.get() & 0xff) << 32)
               | (uint64_t(istream.get() & 0xff) << 40)
               | (uint64_t(istream.get() & 0xff) << 48)
               | (uint64_t(istream.get() & 0xff) << 56);
    }

    inline void write32Bits(uint32_t val, std::ostream &ostream) {
        ostream.put(val & 0xff);
        ostream.put((val >> 8) & 0xff);
        ostream.put((val >> 16) & 0xff);
        ostream.put((val >> 24) & 0xff);
    }

    inline uint32_t read32Bits(std::istream &istream) {
        return (istream.get() & 0xff)
                | ((istream.get() & 0xff) << 8)
                | ((istream.get() & 0xff) << 16)
                | ((istream.get() & 0xff) << 24);
    }

    inline void write16Bits(uint16_t val, std::ostream &ostream) {
        ostream.put(val & 0xff);
        ostream.put((val >> 8) & 0xff);
    }

    inline uint16_t read16Bits(std::istream &istream) {
        return (istream.get() & 0xff) | ((istream.get() & 0xff) << 8);
    }

}

#endif //FB_CORE_UTIL_STREAM_UTILS_H
