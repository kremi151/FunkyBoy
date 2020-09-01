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
#include "blargg/blargg_tests.h"
#include "mooneye/mooneye_tests.h"

#include <cstring>

int main(int argc, char *argv[]) {
    bool runMooneye = false;
    for (int i = 1 ; i < argc ; i++) {
        if (std::strcmp(argv[i], "--mooneye") == 0) {
            runMooneye = true;
        }
    }

    acacia::Report report;

    report += __fbTests_runUnitTests();
    report += __fbTests_runBlarggTests();

    if (runMooneye) {
        report += __fbTests_runMooneyeTests();
    }

    std::ofstream reportFile("acacia-report.txt");
    acacia::generateAcaciaReport(report, reportFile);

    return report ? 0 : 1;
}