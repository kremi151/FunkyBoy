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

#ifndef FB_CORE_APU_H
#define FB_CORE_APU_H

#include <util/typedefs.h>
#include <emulator/io_registers.h>
#include <emulator/gb_type.h>

namespace FunkyBoy {

    typedef struct {
        u8_fast periodTimer;
        u8_fast currentVolume;

        bool channelEnabled;
        u8_fast lengthTimer;

        // Channel 1 exclusive
        bool sweepEnabled;
        u8_fast shadowFrequency;
        u8_fast sweepTimer;

        // General output
        u8_fast currentFrequencyOut;
        float dacOut;

        u16_fast freqTimer;
        u8_fast waveDutyPos;
    } APUChannel;

    class APU {
    private:
        io_registers ioRegisters;

        u8_fast frameSeqMask;
        u8_fast frameSeqStep;

        u8_fast lastDiv;

        APUChannel channels[4]{};

        void initChannels();

        void doSweep(APUChannel &channel);

        static void doEnvelope(float waveDuty, u8_fast nrx2, APUChannel &channel);
        static u16_fast calculateSweepFrequency(u8_fast shift, bool increase, APUChannel &channel);
        static void doLength(u8_fast nrx4, APUChannel &channel);

        void tickChannel1Or2(APUChannel &channel);
        void tickChannel3();
        void tickChannel4();

    public:
        APU(GameBoyType gbType, const io_registers &ioRegisters);

        void doTick();

        // TODO: Call this from memory while intercepting writes to bit 7 of NRx4
        void doTriggerEvent(int channelNbr, u8_fast nrx4);
    };

}

#endif //FB_CORE_APU_H
