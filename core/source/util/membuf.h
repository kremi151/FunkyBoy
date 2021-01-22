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

#ifndef FB_CORE_UTIL_MEMBUF_H
#define FB_CORE_UTIL_MEMBUF_H

#include <iostream>

namespace FunkyBoy::Util {

    class membuf: public std::streambuf {
    public:
        membuf(char *buffer, size_t size, bool read) {
            if (read) {
                setg(buffer, buffer, buffer + size);
            } else {
                setp(buffer, buffer + size);
            }
        }

        std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override {
            if (way == std::ios_base::cur) {
                gbump(off);
            } else if (way == std::ios_base::end) {
                setg(eback(), egptr() + off, egptr());
            } else if (way == std::ios_base::beg) {
                setg(eback(), eback() + off, egptr());
            }
            return gptr() - eback();
        }

        pos_type seekpos(pos_type sp, std::ios_base::openmode which) override {
            return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
        }
    };

}

#endif //FB_CORE_UTIL_MEMBUF_H
