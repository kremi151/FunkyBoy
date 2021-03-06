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
        uint64_t value = istream.get() & 0xff;
        value |= uint64_t(istream.get() & 0xff) << 8;
        value |= uint64_t(istream.get() & 0xff) << 16;
        value |= uint64_t(istream.get() & 0xff) << 24;
        value |= uint64_t(istream.get() & 0xff) << 32;
        value |= uint64_t(istream.get() & 0xff) << 40;
        value |= uint64_t(istream.get() & 0xff) << 48;
        value |= uint64_t(istream.get() & 0xff) << 56;
        return value;
    }

    inline void write32Bits(uint32_t val, std::ostream &ostream) {
        ostream.put(val & 0xff);
        ostream.put((val >> 8) & 0xff);
        ostream.put((val >> 16) & 0xff);
        ostream.put((val >> 24) & 0xff);
    }

    inline uint32_t read32Bits(std::istream &istream) {
        uint32_t value = istream.get() & 0xff;
        value |= (istream.get() & 0xff) << 8;
        value |= (istream.get() & 0xff) << 16;
        value |= (istream.get() & 0xff) << 24;
        return value;
    }

    inline void write16Bits(uint16_t val, std::ostream &ostream) {
        ostream.put(val & 0xff);
        ostream.put((val >> 8) & 0xff);
    }

    inline uint16_t read16Bits(std::istream &istream) {
        uint16_t value = istream.get() & 0xff;
        value |= (istream.get() & 0xff) << 8;
        return value;
    }

}

#endif //FB_CORE_UTIL_STREAM_UTILS_H
