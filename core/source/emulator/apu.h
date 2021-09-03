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

#ifdef FB_USE_SOUND

#include <functional>
#include <util/typedefs.h>
#include <emulator/io_registers.h>
#include <emulator/gb_type.h>
#include <controllers/controllers.h>

namespace FunkyBoy {

    FB_FORWARD_DECLARE Memory;

}

namespace FunkyBoy::Sound {

    typedef struct {
        bool channelEnabled;

        u16_fast lengthTimer; // 16 bits because channel 3 can go up to 256
        u16_fast freqTimer;

        bool dacEnabled;
    } BaseChannel;

    typedef struct BaseChannelType : BaseChannel {
        u8_fast periodTimer;
        u8_fast currentVolume;
    } EnvelopeChannel;

    typedef struct WaveChannelType {
        u8_fast wavePosition;
    } WaveChannel;

    typedef struct ToneChannelType : EnvelopeChannel, WaveChannel {
    } ToneChannel;

    typedef struct ChannelOneType : ToneChannel {
        bool sweepEnabled;
        u8_fast sweepTimer;
        u8_fast shadowFrequency;
    } ChannelOne;

    typedef ToneChannel ChannelTwo;

    typedef struct ChannelThreeType : BaseChannel, WaveChannel {
    } ChannelThree;

    typedef struct ChannelFourType : EnvelopeChannel {
        u16_fast lfsr;
    } ChannelFour;

    class APU {
    private:
        Controller::ControllersPtr controllers;

        io_registers ioRegisters;

        std::function<void(memory_address, u8_fast)> writeToMemory;

        const u16_fast frameSeqMod;
        u8_fast frameSeqStep;

        bool apuEnabled;

        ChannelOne channelOne{};
        ChannelTwo channelTwo{};
        ChannelThree channelThree{};
        ChannelFour channelFour{};

        void initChannels();

        void doSweepOnChannel1();

        static void doEnvelope(u8_fast nrx2, EnvelopeChannel &channel);
        static u16_fast calculateSweepFrequency(u8_fast shift, bool increase, ChannelOne &channel);
        static void doLength(u8_fast nrx4, BaseChannel &channel);

        static void tickChannel1Or2(ToneChannel &channel, u8_fast nrx3, u8_fast nrx4);
        void tickChannel3();
        void tickChannel4();


        void doTriggerEvent(int channelNbr);

        float getChannel1DACOut();
        float getChannel2DACOut();
        float getChannel3DACOut();
        float getChannel4DACOut();

        static inline u16_fast getChannelFrequency(u8_fast nrx3, u8_fast nrx4) {
            return ((nrx4 & 0b00000111) << 8) | nrx3;
        }

        inline u16_fast getChannel1Frequency() {
            return ((ioRegisters.getNR14() & 0b00000111) << 8) | ioRegisters.getNR13();
        }

        inline void setChannel1Frequency(u16_fast freq) {
            ioRegisters.getNR13() = freq & 0b11111111u;
            ioRegisters.getNR14() = (freq >> 8) & 0b111;
        }

        inline u16_fast getChannel2Frequency() {
            return ((ioRegisters.getNR24() & 0b00000111) << 8) | ioRegisters.getNR23();
        }

        inline u16_fast getChannel3Frequency() {
            return ((ioRegisters.getNR34() & 0b00000111) << 8) | ioRegisters.getNR33();
        }

    public:
        APU(GameBoyType gbType, const io_registers &ioRegisters, Controller::ControllersPtr controllers);

        void doTick();

        void handleWrite(memory_address addr, u8_fast value);

        friend Memory;
    };

}

#endif //FB_USE_SOUND

#endif //FB_CORE_APU_H
