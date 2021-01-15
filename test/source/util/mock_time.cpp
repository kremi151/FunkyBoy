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

#include "mock_time.h"

#include <util/testing.h>
#include <cartridge/rtc.h>

namespace FunkyBoy::Testing {
    static bool mockTime = false;
    static time_t secondFactor;
    static time_t mockedSeconds = 0;
}

using namespace FunkyBoy;

void Testing::useMockTime(bool mock) {
    mockTime = mock;
    if (mock) {
        secondFactor = RTC::approximativeTimeConstant(5);
    }
}

void Testing::setMockSeconds(time_t seconds) {
    mockedSeconds = seconds;
}

size_t Testing::time() {
    if (mockTime) {
        return mockedSeconds * secondFactor;
    } else {
        return std::time(nullptr);
    }
}