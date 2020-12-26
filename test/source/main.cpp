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
#include "perf_mode.h"

#include <cstring>
#include <cstdlib>

int runTests(bool runMooneye) {
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

int main(int argc, char **argv) {
    bool runMooneye = false;
    std::string perfRomPath;
    size_t perfCycles = 10240000;

    char **argv_end = argv + argc;
    for (char **argv_c = argv ; argv_c < argv_end ; argv_c++) {
        if (std::strcmp(*argv_c, "--mooneye") == 0) {
            runMooneye = true;
        } else if (std::strcmp(*argv_c, "--perf") == 0) {
            perfRomPath = *(++argv_c);
        } else if (std::strcmp(*argv_c, "--perf-cycles") == 0) {
            char *cycles_str = *(++argv_c);
            perfCycles = std::strtol(cycles_str, nullptr, 10);
        }
    }

    if (perfRomPath.empty()) {
        return runTests(runMooneye);
    } else {
        return FunkyBoyTests::Perf::runPerfMode(perfRomPath, perfCycles);
    }
}