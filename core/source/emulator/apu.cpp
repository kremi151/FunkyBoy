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

#include "apu.h"

#define FB_INCREASE_BIT 0b00001000u

namespace FunkyBoy::APUInternal {

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

    template <int maxLength>
    inline void setLengthTimer(u8_fast nrx1, APUChannel &channel) {
        channel.lengthTimer = maxLength - (nrx1 & (maxLength - 1));
    }

}

using namespace FunkyBoy;

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
    for (auto &channel : channels) {
        channel = {0};
        channel.freqTimer = 1;
    }
}

void APU::doTick() {
    u8 &div = ioRegisters.getDIV();

    tickChannel1Or2(channels[0], ioRegisters.getNR13(), ioRegisters.getNR14());
    tickChannel1Or2(channels[1], ioRegisters.getNR23(), ioRegisters.getNR24());
    tickChannel3();
    tickChannel4();

    // Frame sequencer
    if ((lastDiv & frameSeqMask) && !(div & frameSeqMask)) {
        frameSeqStep = (frameSeqStep + 1) % 8;
        if (frameSeqStep % 2 == 0) {
            doLength(ioRegisters.getNR14(), channels[0]);
            doLength(ioRegisters.getNR24(), channels[1]);
            doLength(ioRegisters.getNR34(), channels[2]);
            doLength(ioRegisters.getNR44(), channels[3]);
        }
        if (frameSeqStep == 7) {
            doEnvelope(APUInternal::DutyWaveforms[ioRegisters.getNR11() >> 6], ioRegisters.getNR12(), channels[0]);
            doEnvelope(APUInternal::DutyWaveforms[ioRegisters.getNR21() >> 6], ioRegisters.getNR22(), channels[1]);
            doEnvelope(0, ioRegisters.getNR42(), channels[3]);
        }
        if (frameSeqStep == 2 || frameSeqStep == 6) {
            doSweep(channels[0]);
        }
    }

    // Post
    lastDiv = div;
}

// TODO: Read wave duties / wave RAM when sampling (use channel.wavePosition)

void APU::tickChannel1Or2(APUChannel &channel, u8_fast nrx3, u8_fast nrx4) {
    if (--channel.freqTimer > 0) {
        return;
    }
    channel.freqTimer = (2048 - (nrx3 | ((nrx4 & 0b111) << 8))) * 4;
    channel.wavePosition = (channel.wavePosition + 1) % 8;
}

// https://nightshade256.github.io/2021/03/27/gb-sound-emulation.html

void APU::tickChannel3() {
    APUChannel &channel = channels[2];
    if (--channel.freqTimer > 0) {
        return;
    }
    channel.freqTimer = (2048 - (ioRegisters.getNR33() | ((ioRegisters.getNR34() & 0b111) << 8))) * 4;
    channel.wavePosition = (channel.wavePosition + 1) % 32;

    // TODO: Handle volume shift
}

void APU::tickChannel4() {
    // TODO: Implement
}

void APU::doTriggerEvent(int channelNbr, u8_fast nrx4) {
    APUChannel &channel = channels[channelNbr];
    switch (channelNbr) {
        case 0: {
            const u8_fast nr10 = ioRegisters.getNR10();
            const u8_fast nr12 = ioRegisters.getNR12();

            // Envelope function
            channel.periodTimer = nr12 & 0b00000111u;
            channel.currentVolume = (nr12 & 0b11110000u) >> 4;

            // Sweep function
            channel.shadowFrequency = channel.currentFrequencyOut;
            const u8_fast sweepPeriod = (nr10 & 0b01110000) >> 4;
            const u8_fast sweepShift = nr10 & 0b00000111u;
            if (sweepPeriod == 0) {
                channel.sweepTimer = 8;
                channel.sweepEnabled = sweepShift != 0;
            } else {
                channel.sweepTimer = sweepPeriod;
                channel.sweepEnabled = true;
            }
            if (sweepShift != 0) {
                // Re-run overflow check
                calculateSweepFrequency(sweepShift, nr10 & FB_INCREASE_BIT, channel);
            }

            // Length function
            if (channel.lengthTimer == 0) {
                APUInternal::setLengthTimer<64>(ioRegisters.getNR11(), channel);
            }
            break;
        }
        case 1: {
            const u8_fast nr22 = ioRegisters.getNR22();

            // Envelope function
            channel.periodTimer = nr22 & 0b00000111u;
            channel.currentVolume = (nr22 & 0b11110000u) >> 4;

            // No sweep function

            // Length function
            if (channel.lengthTimer == 0) {
                APUInternal::setLengthTimer<64>(ioRegisters.getNR21(), channel);
            }
            break;
        }
        case 2: {
            // No envelope function

            // No sweep function

            // Length function
            if (channel.lengthTimer == 0) {
                APUInternal::setLengthTimer<256>(ioRegisters.getNR31(), channel);
            }
            break;
        }
        case 3: {
            const u8_fast nr42 = ioRegisters.getNR42();

            // No sweep function

            // Envelope function
            channel.periodTimer = nr42 & 0b00000111u;
            channel.currentVolume = (nr42 & 0b11110000u) >> 4;

            // Length function
            if (channel.lengthTimer == 0) {
                APUInternal::setLengthTimer<64>(ioRegisters.getNR41(), channel);
            }
            break;
        }
        default:
            fprintf(stderr, "Attempt to perform trigger event for unsupported channel: %d\n", channelNbr);
            break;
    }
}

void APU::doEnvelope(float waveDuty, u8_fast nrx2, APUChannel &channel) {
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
    channel.dacOut = (dacInput / 7.5) - 1.0; // TODO: Do anything with dacOutput
}

void APU::doSweep(APUChannel &channel) {
    if (!channel.channelEnabled) {
        return;
    }
    if (channel.sweepTimer > 0) {
        channel.sweepTimer--;
    }
    if (channel.sweepTimer != 0) {
        return;
    }
    const u8_fast nr10 = ioRegisters.getNR10();
    const u8_fast sweepPeriod = (nr10 & 0b01110000) >> 4;
    if (sweepPeriod > 0) {
        channel.sweepTimer = sweepPeriod;
    } else {
        channel.sweepTimer = 8;
    }
    if (!channel.sweepEnabled || sweepPeriod <= 0) {
        return;
    }
    const u8_fast shift = nr10 & 0b00000111u;
    u16_fast newFrequency = calculateSweepFrequency(shift, nr10 & FB_INCREASE_BIT, channel);
    if (newFrequency <= 2047 && shift > 0) {
        channel.currentFrequencyOut = newFrequency; // TODO: Do anything with frequency
        channel.shadowFrequency = newFrequency;

        // Re-run overflow check
        calculateSweepFrequency(shift, nr10 & FB_INCREASE_BIT, channel);
    }
}

u16_fast APU::calculateSweepFrequency(u8_fast shift, bool increase, APUChannel &channel) {
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

void APU::doLength(u8_fast nrx4, APUChannel &channel) {
    if (!channel.channelEnabled) {
        return;
    }
    if (nrx4 & 0b01000000u && --channel.lengthTimer == 0) {
        channel.channelEnabled = false;
    }
}