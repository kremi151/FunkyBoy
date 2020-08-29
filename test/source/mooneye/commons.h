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

#ifndef FB_TESTS_MOONEYE_COMMONS_H
#define FB_TESTS_MOONEYE_COMMONS_H

#include <util/fs.h>
#include "../util/rom_commons.h"

// Magic number sequences to check whether a test was successful or a failure
// See https://github.com/Gekkio/mooneye-gb/blob/master/tests/common/lib/quit.s

const char __fb_mooneye_success[7] = { 3, 5, 8, 13, 21, 34, 0 };
const char __fb_mooneye_failure[7] = { 66, 66, 66, 66, 66, 66, 0 };

inline void testUsingMooneyeROM(const FunkyBoy::fs::path &romPath, unsigned int expectedTicks) {
    testUsingROM(romPath, expectedTicks, __fb_mooneye_success, __fb_mooneye_failure);
}

#endif //FB_TESTS_MOONEYE_COMMONS_H
