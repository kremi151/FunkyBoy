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

#ifndef FB_TESTS_MOONEYE_TESTS_H
#define FB_TESTS_MOONEYE_TESTS_H

#include <acacia.h>

#include "rom_mooneye_mbc1.h"
#include "rom_acceptance.h"

inline acacia::Report __fbTests_runMooneyeTests() {
    acacia::Report report;

    report += __fbTests_runMooneyeMBC1RomTests();
    report += __fbTests_runMooneyeAcceptanceRomTests();

    return report;
}

#endif //FB_TESTS_MOONEYE_TESTS_H