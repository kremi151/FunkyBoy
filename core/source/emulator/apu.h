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

#include <util/typedefs.h>
#include <emulator/io_registers.h>
#include <emulator/gb_type.h>

namespace FunkyBoy::Sound {

    typedef struct {
        bool channelEnabled;

        u8_fast lengthTimer;
        u16_fast freqTimer;

        u8_fast currentFrequencyOut;
        float dacOut; // = amplitude
    } BaseChannel;

    typedef struct BaseChannelType : BaseChannel {
        u8_fast periodTimer;
        u8_fast currentVolume;
    } EnvelopeChannel;

    typedef struct WaveChannelType {
        u8_fast wavePosition;
    } WaveChannel;

    typedef struct ToneChannelType : EnvelopeChannel, WaveChannel {
        bool sweepEnabled;
        u8_fast shadowFrequency;
    } ToneChannel;

    typedef struct ChannelOneType : ToneChannel {
        u8_fast sweepTimer;
    } ChannelOne;

    typedef ToneChannel ChannelTwo;

    typedef struct ChannelThreeType : BaseChannel, WaveChannel {
    } ChannelThree;

    typedef struct ChannelFourType : EnvelopeChannel {
        u16_fast lfsr;
    } ChannelFour;

    class APU {
    private:
        io_registers ioRegisters;

        u8_fast frameSeqMask;
        u8_fast frameSeqStep;

        u8_fast lastDiv;

        ChannelOne channelOne{};
        ChannelTwo channelTwo{};
        ChannelThree channelThree{};
        ChannelFour channelFour{};

        void initChannels();

        void doSweepOnChannel1();

        static void doEnvelope(float waveDuty, u8_fast nrx2, EnvelopeChannel &channel);
        static u16_fast calculateSweepFrequency(u8_fast shift, bool increase, ChannelOne &channel);
        static void doLength(u8_fast nrx4, BaseChannel &channel);

        static void tickChannel1Or2(ToneChannel &channel, u8_fast nrx3, u8_fast nrx4);
        void tickChannel3();
        void tickChannel4();

        // TODO: Call this from memory while intercepting writes to bit 7 of NRx4
        void doTriggerEvent(int channelNbr, u8_fast nrx4);

    public:
        APU(GameBoyType gbType, const io_registers &ioRegisters);

        void doTick();

        void handleWrite(memory_address addr, u8_fast value);
    };

}

#endif //FB_USE_SOUND

#endif //FB_CORE_APU_H