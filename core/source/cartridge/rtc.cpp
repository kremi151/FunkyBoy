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

#include "rtc.h"

#include <algorithm>
#include <util/testing.h>

using namespace FunkyBoy;

#define DAY_OVERFLOW 511
#define DAY_OVERFLOW_MOD (DAY_OVERFLOW + 1)

#ifdef FB_TESTING
#define get_time() FunkyBoy::Testing::time()
#else
#define get_time() time(nullptr)
#endif

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
    , overflowTime(DAY_OVERFLOW_MOD * dayFactor)
    , haltedDays(0)
    , haltedHours(0)
    , haltedMinutes(0)
    , haltedSeconds(0)
    , latchTimestamp(0)
    , startTimestamp(get_time())
    , timestampOffset(0)
    , halted(false)
{
}

u8 RTC::getSeconds() {
    if (halted) {
        return haltedSeconds;
    } else {
        return (currentTimestamp() / secondFactor) % 60;
    }
}

void RTC::setSeconds(u8 val) {
    if (halted) {
        haltedSeconds = val;
    }
}

u8 RTC::getMinutes() {
    if (halted) {
        return haltedMinutes;
    } else {
        return (currentTimestamp() / minuteFactor) % 60;
    }
}

void RTC::setMinutes(u8 val) {
    if (halted) {
        haltedMinutes = val;
    }
}

u8 RTC::getHours() {
    if (halted) {
        return haltedHours;
    } else {
        return (currentTimestamp() / hourFactor) % 24;
    }
}

void RTC::setHours(u8 val) {
    if (halted) {
        haltedHours = val;
    }
}

u16 RTC::getDays() {
    if (halted) {
        return haltedDays % DAY_OVERFLOW_MOD;
    } else {
        return (currentTimestamp() / dayFactor) % DAY_OVERFLOW_MOD;
    }
}

u8 RTC::getDL() {
    return getDays() & 0xffu;
}

void RTC::setDL(u8 val) {
    haltedDays = (haltedDays & 0xff00u) | (val & 0xffu);
}

u8 RTC::getDH() {
    size_t days = halted ? haltedDays : currentTimestamp() / dayFactor;
    return (((days % DAY_OVERFLOW_MOD) >> 8) & 0b1u) | (halted ? 0b01000000u : 0u) | ((days > DAY_OVERFLOW) ? 0b10000000u : 0u);
}

void RTC::setDH(u8 val) {
    bool requestHalt = val & 0b01000000u;
    if (requestHalt && !halted) {
        startLatch();
        time_t ts = currentTimestamp();
        haltedDays = (ts / dayFactor) % DAY_OVERFLOW_MOD;
        haltedHours = (ts / hourFactor) % 24;
        haltedMinutes = (ts / minuteFactor) % 60;
        haltedSeconds = (ts / secondFactor) % 60;
        endLatch();
    } else if (!requestHalt && halted) {
        timestampOffset = (haltedDays * dayFactor) + (haltedHours * hourFactor) + (haltedMinutes * minuteFactor) + (haltedSeconds * secondFactor);
        startTimestamp = get_time();
    }
    halted = requestHalt;
    if (!(val & 0b10000000u)) {
        while (currentTimestamp() > overflowTime) {
            timestampOffset -= overflowTime;
        }
    }
    haltedDays = (haltedDays & 0x00ffu) | ((val & 0b10111111u) << 8);
}

void RTC::startLatch() {
    latchTimestamp = get_time();
}

void RTC::endLatch() {
    latchTimestamp = 0;
}

time_t RTC::currentTimestamp() const {
    if (latchTimestamp) {
        return (latchTimestamp - startTimestamp) + timestampOffset;
    } else {
        return (get_time() - startTimestamp) + timestampOffset;
    }
}

void RTC::write(std::ostream &stream) {
    u8 buffer[48]{};
    startLatch();
    buffer[0] = buffer[20] = getSeconds();
    buffer[4] = buffer[24] = getMinutes();
    buffer[8] = buffer[28] = getHours();
    buffer[12] = buffer[32] = getDL();
    buffer[16] = buffer[36] = getDH();
    size_t latch = latchTimestamp / secondFactor;
    buffer[40] = latch & 0xff;
    buffer[41] = (latch >> 8) & 0xff;
    buffer[42] = (latch >> 16) & 0xff;
    buffer[43] = (latch >> 24) & 0xff;
    endLatch();
    stream.write(reinterpret_cast<char*>(buffer), 48);
}

void RTC::load(std::istream &stream) {
    u8 buffer[44]{};
    stream.read(reinterpret_cast<char*>(buffer), 44);
    u8 &dh = buffer[16];
    setDH(dh);
    u8 &dl = buffer[12];
    setDL(dl);
    u8 &seconds = buffer[0];
    setSeconds(seconds);
    u8 &minutes = buffer[4];
    setMinutes(minutes);
    u8 &hours = buffer[8];
    setHours(hours);

    u16_fast days = ((dh & 0b1u) << 8) | dl;
    timestampOffset = (days * dayFactor) + (hours * hourFactor) + (minutes * minuteFactor) + (seconds * secondFactor);
    startTimestamp = ((buffer[43] << 24) | (buffer[42] << 16) | (buffer[41] << 8) | buffer[40]) * secondFactor;
}