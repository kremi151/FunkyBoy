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

#include <acacia.h>

#include "unit_tests.h"
#include "rom_cpu_instrs_tests.h"
#include "mbc_tests.h"

int main() {
    acacia::Report report;

    report += __fbTests_runUnitTests();
    report += __fbTests_runMbcTests();
    report += __fbTests_runRomTests();

    std::ofstream reportFile("acacia-report.txt");
    acacia::generateAcaciaReport(report, reportFile);

    return report ? 0 : 1;
}