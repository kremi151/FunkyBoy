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

#include "rom_tests.h"
#include "unit_tests.h"

int main() {
    auto unitTestsReport = __fbTests_runUnitTests();
    auto romTestsReport = __fbTests_runRomTests();

    acacia::Report totalReport;
    totalReport += unitTestsReport;
    totalReport += romTestsReport;

    std::ofstream reportFile("test-report.xml");
    acacia::generateJUnitReport(totalReport, reportFile);

    return totalReport ? 0 : 1;
}