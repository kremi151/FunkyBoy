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

#ifndef FB_CORE_CARTRIDGE_RTC_H
#define FB_CORE_CARTRIDGE_RTC_H

#include <util/typedefs.h>
#include <ctime>
#include <memory>
#include <iostream>

namespace FunkyBoy {

    class RTC {
    private:
        const size_t secondFactor;
        const size_t dayFactor;
        const size_t hourFactor;
        const size_t minuteFactor;
        const size_t overflowTime;
        time_t startTimestamp;
        time_t timestampOffset;
        time_t latchTimestamp;
        u16 haltedDays;
        u8 haltedHours, haltedMinutes, haltedSeconds;
        bool halted;

        time_t currentTimestamp() const;

        void startLatch();
        void endLatch();

        static size_t determineTimeConstant();
    public:
        RTC();

        u8 getSeconds();
        u8 getMinutes();
        u8 getHours();
        u16 getDays();

        u8 getDL();
        u8 getDH();

        void setSeconds(u8 val);
        void setMinutes(u8 val);
        void setHours(u8 val);

        void setDL(u8 val);
        void setDH(u8 val);

        void write(std::ostream &stream);
        void load(std::istream &stream);

#ifdef FB_TESTING
        inline bool isHalted() const {
            return halted;
        }
#endif

        static size_t approximativeTimeConstant(size_t iterations);
    };

    typedef std::shared_ptr<RTC> RTCPtr;

}

#endif //FB_CORE_CARTRIDGE_RTC_H
