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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND
#include "blargg/blargg_tests.h", either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FB_TESTS_BLARGG_TESTS_H
#define FB_TESTS_BLARGG_TESTS_H

#include <acacia.h>

#include "rom_cpu_instrs_tests.h"

inline acacia::Report __fbTests_runBlarggTests() {
    acacia::Report report;

    report += __fbTests_runBlarggCpuInstrsRomTests();

    return report;
}

#endif //FB_TESTS_BLARGG_TESTS_H
