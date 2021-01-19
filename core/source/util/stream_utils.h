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

    inline void write64BitIgnoreEndianness(uint64_t val, std::ostream &ostream) {
        ostream.write(reinterpret_cast<char *>(&val), sizeof(val));
    }

    inline uint64_t read64BitIgnoreEndianness(std::istream &istream) {
        uint64_t buf;
        istream.read(reinterpret_cast<char *>(&buf), sizeof(buf));
        return buf;
    }

    inline void write32BitIgnoreEndianness(uint32_t val, std::ostream &ostream) {
        ostream.write(reinterpret_cast<char *>(&val), sizeof(val));
    }

    inline uint32_t read32BitIgnoreEndianness(std::istream &istream) {
        uint32_t buf;
        istream.read(reinterpret_cast<char *>(&buf), sizeof(buf));
        return buf;
    }

    inline void write16BitIgnoreEndianness(uint16_t val, std::ostream &ostream) {
        ostream.write(reinterpret_cast<char *>(&val), sizeof(val));
    }

    inline uint16_t read16BitIgnoreEndianness(std::istream &istream) {
        uint16_t buf;
        istream.read(reinterpret_cast<char *>(&buf), sizeof(buf));
        return buf;
    }

}

#endif //FB_CORE_UTIL_STREAM_UTILS_H
