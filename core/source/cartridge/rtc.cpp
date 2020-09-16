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

#include <algorithm>
#include "rtc.h"

using namespace FunkyBoy;

#define DAY_OVERFLOW 511

size_t RTC::determineTimeConstant() {
    time_t time1 = time(nullptr);
    tm timeBuffer{};
#ifdef OS_WINDOWS
    localtime_s(&timeBuffer, &time1);
#else
    localtime_r(&time1, &timeBuffer);
#endif
    timeBuffer.tm_sec++;
    time_t time2 = mktime(&timeBuffer);
    return std::max<size_t>(1, time2 - time1);
}

size_t RTC::approximativeTimeConstant(size_t iterations) {
    size_t time = 0;
    for (size_t i = 0 ; i < iterations ; i++) {
        time += determineTimeConstant();
    }
    return time / iterations;
}

RTC::RTC()
    : secondFactor(approximativeTimeConstant(5))
    , dayFactor(24 * 60 * 60 * secondFactor)
    , hourFactor(60 * 60 * secondFactor)
    , minuteFactor(60 * secondFactor)
    , overflowTime(DAY_OVERFLOW * dayFactor)
    , haltedDays(0)
    , haltedHours(0)
    , haltedMinutes(0)
    , haltedSeconds(0)
    , latchTimestamp(0)
    , startTimestamp(time(nullptr))
    , timestampOffset(0)
    , halted(false)
{
}

u8 RTC::getSeconds() {
    return currentTimestamp() / secondFactor;
}

void RTC::setSeconds(u8 val) {
    if (halted) {
        haltedSeconds = val;
    }
}

u8 RTC::getMinutes() {
    return currentTimestamp() / minuteFactor;
}

void RTC::setMinutes(u8 val) {
    if (halted) {
        haltedMinutes = val;
    }
}

u8 RTC::getHours() {
    return currentTimestamp() / hourFactor;
}

void RTC::setHours(u8 val) {
    if (halted) {
        haltedHours = val;
    }
}

u16 RTC::getDays() {
    return currentTimestamp() / dayFactor;
}

u8 RTC::getDL() {
    return getDays() & 0xffu;
}

void RTC::setDL(u8 val) {
    haltedDays = (haltedDays & 0xff00u) | (val & 0xffu);
}

u8 RTC::getDH() {
    size_t days = getDays();
    return ((days >> 8) & 0xffu) | (halted ? 0b01000000u : 0u) | ((days > DAY_OVERFLOW) ? 0b10000000u : 0u);
}

void RTC::setDH(u8 val) {
    bool requestHalt = val & 0b01000000u;
    if (requestHalt && !halted) {
        startLatch();
        haltedDays = getDays();
        haltedHours = getHours();
        haltedMinutes = getMinutes();
        haltedSeconds = getSeconds();
        endLatch();
    } else if (!requestHalt && halted) {
        timestampOffset = (haltedDays * dayFactor) + (haltedHours * hourFactor) + (haltedMinutes * minuteFactor) + (haltedSeconds * secondFactor);
        startTimestamp = time(nullptr);
    }
    halted = requestHalt;
    if (!(val & 0b10000000u)) {
        while (currentTimestamp() > overflowTime) {
            timestampOffset -= overflowTime;
        }
    }
    haltedDays = (haltedDays & 0x00ffu) | ((val & 0b1u) << 8);
}

void RTC::startLatch() {
    latchTimestamp = time(nullptr);
}

void RTC::endLatch() {
    latchTimestamp = 0;
}

time_t RTC::currentTimestamp() {
    if (latchTimestamp) {
        return (latchTimestamp - startTimestamp) + timestampOffset;
    } else {
        return (time(nullptr) - startTimestamp) + timestampOffset;
    }
}
