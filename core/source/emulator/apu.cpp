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

#ifdef FB_USE_SOUND

#include "apu.h"

#define FB_INCREASE_BIT 0b00001000u
#define FB_TRIGGER_BIT 0b10000000u

namespace FunkyBoy::Sound {

    const FunkyBoy::u8 DutyWaveforms[4] = {
            0b00000001,
            0b10000001,
            0b10000111,
            0b01111110
    };

    const float DutyRatios[4] = {
            12.5,
            25.0,
            50.0,
            75.0,
    };

    const FunkyBoy::u8 Divisors[8] = {
            0,
            16,
            32,
            48,
            64,
            80,
            96,
            112,
    };

    const FunkyBoy::u8 ChannelThreeShifts[4] = {
            4,
            0,
            1,
            2,
    };

    template <int maxLength>
    inline void setLengthTimer(u8_fast nrx1, BaseChannel &channel) {
        channel.lengthTimer = maxLength - (nrx1 & (maxLength - 1));
    }

}

using namespace FunkyBoy::Sound;

APU::APU(GameBoyType gbType, const io_registers &ioRegisters)
    : ioRegisters(ioRegisters)
    , lastDiv(0)
    , frameSeqStep(7)
{
    if (gbType == GameBoyDMG) {
        frameSeqMask = 0b00100000;
    } else {
        frameSeqMask = 0b01000000;
    }
    initChannels();
}

void APU::initChannels() {
    channelOne.freqTimer = 1;
    channelTwo.freqTimer = 1;
    channelThree.freqTimer = 1;
    channelFour.freqTimer = 1;
}

void APU::doTick() {
    u8 &div = ioRegisters.getDIV();

    tickChannel1Or2(channelOne, ioRegisters.getNR13(), ioRegisters.getNR14());
    tickChannel1Or2(channelTwo, ioRegisters.getNR23(), ioRegisters.getNR24());
    tickChannel3();
    tickChannel4();

    // Frame sequencer
    if ((lastDiv & frameSeqMask) && !(div & frameSeqMask)) {
        frameSeqStep = (frameSeqStep + 1) % 8;
        if (frameSeqStep % 2 == 0) {
            doLength(ioRegisters.getNR14(), channelOne);
            doLength(ioRegisters.getNR24(), channelTwo);
            doLength(ioRegisters.getNR34(), channelThree);
            doLength(ioRegisters.getNR44(), channelFour);
        }
        if (frameSeqStep == 7) {
            doEnvelope(DutyWaveforms[ioRegisters.getNR11() >> 6], ioRegisters.getNR12(), channelOne);
            doEnvelope(DutyWaveforms[ioRegisters.getNR21() >> 6], ioRegisters.getNR22(), channelTwo);
            doEnvelope(0 /* TODO */, ioRegisters.getNR42(), channelFour);
        }
        if (frameSeqStep == 2 || frameSeqStep == 6) {
            doSweepOnChannel1();
        }
    }

    // Post
    lastDiv = div;
}

// TODO: Read wave duties / wave RAM when sampling (use channel.wavePosition)

void APU::tickChannel1Or2(ToneChannel &channel, u8_fast nrx3, u8_fast nrx4) {
    if (--channel.freqTimer > 0) {
        return;
    }
    channel.freqTimer = (2048 - (nrx3 | ((nrx4 & 0b111) << 8))) * 4;
    channel.wavePosition = (channel.wavePosition + 1) % 8;
}

void APU::tickChannel3() {
    if (--channelThree.freqTimer > 0) {
        return;
    }
    channelThree.freqTimer = (2048 - (ioRegisters.getNR33() | ((ioRegisters.getNR34() & 0b111) << 8))) * 4;
    channelThree.wavePosition = (channelThree.wavePosition + 1) % 32;

    u8_fast sample = ioRegisters.getWaveRAM()[channelThree.wavePosition / 2];
    sample = sample >> ((((channelThree.wavePosition & 1) != 0) ? 4 : 0)) & 0b00001111;

    // Set DAC output here because channel 3 doesn't have an envelope function
    channelThree.dacOut = ((sample >> ChannelThreeShifts[(ioRegisters.getNR32() & 0b01100000) >> 5]) / 7.5) - 1.0;
}

void APU::tickChannel4() {
    if (--channelFour.freqTimer > 0) {
        return;
    }
    const u8_fast nr43 = ioRegisters.getNR43();
    const u8_fast shift = (nr43 & 0b11110000) >> 4;
    channelFour.freqTimer = Divisors[nr43 & 0b00000111] << shift;

    const u16_fast xorResult = (channelFour.lfsr % 0b01) ^ ((channelFour.lfsr & 0b10) >> 1);
    channelFour.lfsr = (channelFour.lfsr >> 1) | (xorResult << 14);
    if (nr43 & 0b00001000) {
        channelFour.lfsr &= ~(1 << 6);
        channelFour.lfsr |= xorResult << 6;
    }

    // TODO: Amplitude = ~LFSR & 0x01;
}

void APU::doTriggerEvent(int channelNbr, u8_fast nrx4) {
    switch (channelNbr) {
        case 0: {
            const u8_fast nr10 = ioRegisters.getNR10();
            const u8_fast nr12 = ioRegisters.getNR12();

            // Envelope function
            channelOne.periodTimer = nr12 & 0b00000111u;
            channelOne.currentVolume = (nr12 & 0b11110000u) >> 4;

            // Sweep function
            channelOne.shadowFrequency = channelOne.currentFrequencyOut;
            const u8_fast sweepPeriod = (nr10 & 0b01110000) >> 4;
            const u8_fast sweepShift = nr10 & 0b00000111u;
            if (sweepPeriod == 0) {
                channelOne.sweepTimer = 8;
                channelOne.sweepEnabled = sweepShift != 0;
            } else {
                channelOne.sweepTimer = sweepPeriod;
                channelOne.sweepEnabled = true;
            }
            if (sweepShift != 0) {
                // Re-run overflow check
                calculateSweepFrequency(sweepShift, nr10 & FB_INCREASE_BIT, channelOne);
            }

            // Length function
            if (channelOne.lengthTimer == 0) {
                setLengthTimer<64>(ioRegisters.getNR11(), channelOne);
            }
            break;
        }
        case 1: {
            const u8_fast nr22 = ioRegisters.getNR22();

            // Envelope function
            channelTwo.periodTimer = nr22 & 0b00000111u;
            channelTwo.currentVolume = (nr22 & 0b11110000u) >> 4;

            // No sweep function

            // Length function
            if (channelTwo.lengthTimer == 0) {
                setLengthTimer<64>(ioRegisters.getNR21(), channelTwo);
            }
            break;
        }
        case 2: {
            // No envelope function

            // No sweep function

            // Length function
            if (channelThree.lengthTimer == 0) {
                setLengthTimer<256>(ioRegisters.getNR31(), channelThree);
            }
            break;
        }
        case 3: {
            const u8_fast nr42 = ioRegisters.getNR42();

            channelFour.lfsr = ~0;

            // No sweep function

            // Envelope function
            channelFour.periodTimer = nr42 & 0b00000111u;
            channelFour.currentVolume = (nr42 & 0b11110000u) >> 4;

            // Length function
            if (channelFour.lengthTimer == 0) {
                setLengthTimer<64>(ioRegisters.getNR41(), channelFour);
            }
            break;
        }
        default:
            fprintf(stderr, "Attempt to perform trigger event for unsupported channel: %d\n", channelNbr);
            break;
    }
}

void APU::doEnvelope(float waveDuty, u8_fast nrx2, EnvelopeChannel &channel) {
    if (!channel.channelEnabled) {
        return;
    }

    if (channel.periodTimer == 0) {
        return;
    }
    if (channel.periodTimer > 0) {
        channel.periodTimer--;
    }
    if (channel.periodTimer == 0) {
        channel.periodTimer = nrx2 & 0b00000011;

        bool upwards = nrx2 & FB_INCREASE_BIT;
        if ((channel.currentVolume < 0xF && upwards) || ((channel.currentVolume > 0x0 && !upwards))) {
            if (upwards) {
                channel.currentVolume++;
            } else {
                channel.currentVolume--;
            }
        }
    }

    float dacInput = waveDuty * channel.currentVolume;
    channel.dacOut = (dacInput / 7.5) - 1.0;
}

void APU::doSweepOnChannel1() {
    if (!channelOne.channelEnabled) {
        return;
    }
    if (channelOne.sweepTimer > 0) {
        channelOne.sweepTimer--;
    }
    if (channelOne.sweepTimer != 0) {
        return;
    }
    const u8_fast nr10 = ioRegisters.getNR10();
    const u8_fast sweepPeriod = (nr10 & 0b01110000) >> 4;
    if (sweepPeriod > 0) {
        channelOne.sweepTimer = sweepPeriod;
    } else {
        channelOne.sweepTimer = 8;
    }
    if (!channelOne.sweepEnabled || sweepPeriod <= 0) {
        return;
    }
    const u8_fast shift = nr10 & 0b00000111u;
    u16_fast newFrequency = calculateSweepFrequency(shift, nr10 & FB_INCREASE_BIT, channelOne);
    if (newFrequency <= 2047 && shift > 0) {
        channelOne.currentFrequencyOut = newFrequency; // TODO: Do anything with frequency
        channelOne.shadowFrequency = newFrequency;

        // Re-run overflow check
        calculateSweepFrequency(shift, nr10 & FB_INCREASE_BIT, channelOne);
    }
}

FunkyBoy::u16_fast APU::calculateSweepFrequency(u8_fast shift, bool increase, ChannelOne &channel) {
    u16_fast newFrequency = channel.shadowFrequency >> shift;
    if (increase) {
        newFrequency = channel.shadowFrequency + newFrequency;
    } else {
        newFrequency = channel.shadowFrequency - newFrequency;
    }
    if (newFrequency > 2047) {
        channel.sweepEnabled = false;
    }
    return newFrequency;
}

void APU::doLength(u8_fast nrx4, BaseChannel &channel) {
    if (!channel.channelEnabled) {
        return;
    }
    if (nrx4 & 0b01000000u && --channel.lengthTimer == 0) {
        channel.channelEnabled = false;
    }
}

void APU::handleWrite(memory_address addr, u8_fast value) {
    // TODO: Sync channel states here

    switch (addr) {
        case FB_REG_NR14:
            if (value & FB_TRIGGER_BIT) {
                doTriggerEvent(0, ioRegisters.getNR14());
            }
            break;
        case FB_REG_NR24:
            if (value & FB_TRIGGER_BIT) {
                doTriggerEvent(1, ioRegisters.getNR24());
            }
            break;
        case FB_REG_NR34:
            if (value & FB_TRIGGER_BIT) {
                doTriggerEvent(2, ioRegisters.getNR34());
            }
            break;
        case FB_REG_NR44:
            if (value & FB_TRIGGER_BIT) {
                doTriggerEvent(3, ioRegisters.getNR44());
            }
            break;
        default:
            break;
    }
}

#endif
