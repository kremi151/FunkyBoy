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

#ifndef FB_CORE_UTIL_STRING_POLYFILLS_H
#define FB_CORE_UTIL_STRING_POLYFILLS_H

#ifdef __PSP__
#include <sstream>
#else
#include <string>
#endif

namespace FunkyBoy::Util {

#ifdef __PSP__
    template <class T>
    inline std::string toString(T val) {
        std::ostringstream os;
        os << val;
        return os.str();
    }
#else
    template <class T>
    inline std::string toString(T val) {
        return std::to_string(val);
    }
#endif

}

#endif //FB_CORE_UTIL_STRING_POLYFILLS_H
