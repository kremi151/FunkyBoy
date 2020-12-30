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

time_t RTC::currentTimestamp() const {
    if (latchTimestamp) {
        return (latchTimestamp - startTimestamp) + timestampOffset;
    } else {
        return (time(nullptr) - startTimestamp) + timestampOffset;
    }
}

void RTC::write(std::ostream &stream) {
    u8 buffer[48]{};
    startLatch();
    buffer[0] = buffer[20] = getSeconds();
    buffer[4] = buffer[24] = getMinutes();
    buffer[8] = buffer[28] = getHours();
    u16_fast days = getDays();
    buffer[12] = buffer[32] = days & 0xffu;
    buffer[16] = buffer[36] = (days >> 8) & 0xffu;
    size_t latch = latchTimestamp / secondFactor;
    buffer[40] = latch & 0xff;
    buffer[41] = (latch >> 8) & 0xff;
    buffer[42] = (latch >> 16) & 0xff;
    buffer[43] = (latch >> 24) & 0xff;
    endLatch();
    stream.write(static_cast<char*>(static_cast<void*>(buffer)), 48);
}

void RTC::load(std::istream &stream) {
    u8 buffer[44]{};
    stream.read(static_cast<char*>(static_cast<void*>(buffer)), 44);
    u8 &seconds = buffer[0];
    u8 &minutes = buffer[4];
    u8 &hours = buffer[8];
    u8 &daysLsb = buffer[12];
    u8 &daysMsb = buffer[16];
    u16_fast days = (daysMsb << 8) | daysLsb;
    timestampOffset = (days * dayFactor) + (hours * hourFactor) + (minutes * minuteFactor) + (seconds * secondFactor);
    startTimestamp = ((buffer[43] << 24) | (buffer[42] << 16) | (buffer[41] << 8) | buffer[40]) * secondFactor;
}