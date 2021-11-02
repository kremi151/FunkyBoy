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

#include <utility>

#define FB_INCREASE_BIT 0b00001000u
#define FB_TRIGGER_BIT 0b10000000u
#define FB_LENGTH_DATA_BITS 0b00111111

#define FB_FRAME_SEQ_MOD_DMG 0b0010000000000000
#define FB_FRAME_SEQ_MOD_CGB 0b0100000000000000

#define FB_CPU_CLOCK 4194304
#define FB_SAMPLE_RATE 48000

// Increasing this will lead to more samples being collected in a shorter time frame.
// Samples will also be collected more frequently.
// Increase this if you notice gaps in the audio output.
#define FB_SAMPLE_FACTOR 4

#define FB_SAMPLE_CLOCKS (FB_CPU_CLOCK / FB_SAMPLE_RATE / FB_SAMPLE_FACTOR)

//#define FB_SAMPLE_BASE_VALUE 0.75f
#define FB_SAMPLE_BASE_VALUE 0.0f

namespace FunkyBoy::Sound {

    const FunkyBoy::u8 DutyWaveforms[4][8] = {
            {0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 1},
            {0, 1, 1, 1, 1, 1, 1, 0}
    };

    const FunkyBoy::u8 Divisors[8] = {
            8,
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

}

using namespace FunkyBoy::Sound;

APU::APU(GameBoyType gbType, const io_registers &ioRegisters)
    : ioRegisters(ioRegisters)
    , frameSeqMod(gbType == GameBoyDMG ? FB_FRAME_SEQ_MOD_DMG : FB_FRAME_SEQ_MOD_CGB)
    , frameSeqStep(7)
    , apuEnabled(false)
{
    initChannels();
#ifdef FB_DEBUG
    fprintf(stdout, "Queue audio every %d ticks\n", FB_SAMPLE_CLOCKS);
#endif
}

void APU::onControllersUpdated(const FunkyBoy::Controller::Controllers &controllers) {
    audioController = controllers.getAudio();
}

void APU::initChannels() {
    channelOne.freqTimer = 1;
    channelTwo.freqTimer = 1;
    channelThree.freqTimer = 1;
    channelFour.freqTimer = 1;
}

void APU::doTick() {
    if (!apuEnabled) {
        return;
    }
    u16_fast sysCounter = ioRegisters.getSysCounter();

    tickChannel1Or2(channelOne, ioRegisters.getNR13(), ioRegisters.getNR14());
    tickChannel1Or2(channelTwo, ioRegisters.getNR23(), ioRegisters.getNR24());
    tickChannel3();
    tickChannel4();

    // Frame sequencer
    if (sysCounter % frameSeqMod == 0) {
        frameSeqStep = (frameSeqStep + 1) % 8;
        if (frameSeqStep % 2 == 0) {
            doLength(ioRegisters.getNR14(), channelOne);
            doLength(ioRegisters.getNR24(), channelTwo);
            doLength(ioRegisters.getNR34(), channelThree);
            doLength(ioRegisters.getNR44(), channelFour);
        }
        if (frameSeqStep == 7) {
            doEnvelope(ioRegisters.getNR12(), channelOne);
            doEnvelope(ioRegisters.getNR22(), channelTwo);
            doEnvelope(ioRegisters.getNR42(), channelFour);
        }
        if (frameSeqStep == 2 || frameSeqStep == 6) {
            doSweepOnChannel1();
        }
    }

    if (sysCounter % FB_SAMPLE_CLOCKS == 0) {
        u8_fast nr50 = ioRegisters.getNR50();
        u8_fast nr51 = ioRegisters.getNR51();
        float leftVolume = ((nr50 & 0b01110000u) >> 4) / 7.0f;
        float rightVolume = (nr50 & 0b00000111u) / 7.0f;

        audioController->pushSample(
                FB_SAMPLE_BASE_VALUE + leftVolume * (
                        ((nr51 & 0b10000000) ? getChannel4DACOut() : 0.0f)
                        + ((nr51 & 0b01000000) ? getChannel3DACOut() : 0.0f)
                        + ((nr51 & 0b00100000) ? getChannel2DACOut() : 0.0f)
                        + ((nr51 & 0b00010000) ? getChannel1DACOut() : 0.0f)
                ) / 4.0f,
                FB_SAMPLE_BASE_VALUE + rightVolume * (
                        ((nr51 & 0b00001000) ? getChannel4DACOut() : 0.0f)
                        + ((nr51 & 0b00000100) ? getChannel3DACOut() : 0.0f)
                        + ((nr51 & 0b00000010) ? getChannel2DACOut() : 0.0f)
                        + ((nr51 & 0b00000001) ? getChannel1DACOut() : 0.0f)
                ) / 4.0f
        );
    }

}

// TODO: Implement NR52 (master switch)

void APU::tickChannel1Or2(ToneChannel &channel, u8_fast nrx3, u8_fast nrx4) {
    if (--channel.freqTimer > 0) {
        return;
    }
    channel.freqTimer = ((2048 - getChannelFrequency(nrx3, nrx4)) * 4) / FB_SAMPLE_FACTOR;
    channel.wavePosition = (channel.wavePosition + 1) % 8;
}

void APU::tickChannel3() {
    if (--channelThree.freqTimer > 0) {
        return;
    }
    channelThree.freqTimer = ((2048 - getChannel3Frequency()) * 2) / FB_SAMPLE_FACTOR;
    channelThree.wavePosition = (channelThree.wavePosition + 1) % 32;
}

void APU::tickChannel4() {
    if (--channelFour.freqTimer > 0) {
        return;
    }
    const u8_fast nr43 = ioRegisters.getNR43();
    const u8_fast shift = (nr43 & 0b11110000) >> 4;
    channelFour.freqTimer = (Divisors[nr43 & 0b00000111] << shift) / FB_SAMPLE_FACTOR;

    const u16_fast xorResult = (channelFour.lfsr & 0b01) ^ ((channelFour.lfsr & 0b10) >> 1);
    channelFour.lfsr = (channelFour.lfsr >> 1) | (xorResult << 14);
    if (nr43 & 0b00001000) {
        channelFour.lfsr &= ~(1 << 6);
        channelFour.lfsr |= xorResult << 6;
    }
}

void APU::doTriggerEvent(int channelNbr) {
    switch (channelNbr) {
        case 0: {
            if (channelOne.dacEnabled) {
                channelOne.channelEnabled = true;

                const u8_fast nr10 = ioRegisters.getNR10();
                const u8_fast nr12 = ioRegisters.getNR12();

                // Envelope function
                channelOne.periodTimer = nr12 & 0b00000111u;
                channelOne.currentVolume = (nr12 & 0b11110000u) >> 4;

                // Sweep function
                channelOne.shadowFrequency = getChannel1Frequency();
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
            }

            // Length function
            if (channelOne.lengthTimer == 0) {
                channelOne.lengthTimer = 64;
            }
            break;
        }
        case 1: {
            if (channelTwo.dacEnabled) {
                channelTwo.channelEnabled = true;

                const u8_fast nr22 = ioRegisters.getNR22();

                // Envelope function
                channelTwo.periodTimer = nr22 & 0b00000111u;
                channelTwo.currentVolume = (nr22 & 0b11110000u) >> 4;
            }

            // No sweep function

            // Length function
            if (channelTwo.lengthTimer == 0) {
                channelTwo.lengthTimer = 64;
            }
            break;
        }
        case 2: {
            if (channelThree.dacEnabled) {
                channelThree.channelEnabled = true;
            }

            // No envelope function

            // No sweep function

            // Length function
            if (channelThree.lengthTimer == 0) {
                channelThree.lengthTimer = 256;
            }
            break;
        }
        case 3: {
            if (channelFour.dacEnabled) {
                channelFour.channelEnabled = true;
            }

            const u8_fast nr42 = ioRegisters.getNR42();

            channelFour.lfsr = ~0;

            // No sweep function

            // Envelope function
            channelFour.periodTimer = nr42 & 0b00000111u;
            channelFour.currentVolume = (nr42 & 0b11110000u) >> 4;

            // Length function
            if (channelFour.lengthTimer == 0) {
                channelFour.lengthTimer = 64;
            }
            break;
        }
        default:
            fprintf(stderr, "Attempt to perform trigger event for unsupported channel: %d\n", channelNbr);
            break;
    }
}

void APU::doEnvelope(u8_fast nrx2, EnvelopeChannel &channel) {
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
}

void APU::doSweepOnChannel1() {
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
        setChannel1Frequency(newFrequency);
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
    if (nrx4 & 0b01000000u && channel.lengthTimer > 0) {
        if (--channel.lengthTimer == 0) {
            channel.channelEnabled = false;
        }
    }
}

float APU::getChannel1DACOut() {
    if (channelOne.channelEnabled && channelOne.dacEnabled) {
        float dacIn = DutyWaveforms[(ioRegisters.getNR11() >> 6) & 0b11][channelOne.wavePosition];
        return (dacIn * channelOne.currentVolume / 7.5f) - 1.0f;
    }
    return 0.0f;
}

float APU::getChannel2DACOut() {
    if (channelTwo.channelEnabled && channelTwo.dacEnabled) {
        float dacIn = DutyWaveforms[(ioRegisters.getNR21() >> 6) & 0b11][channelTwo.wavePosition];
        return (dacIn * channelTwo.currentVolume / 7.5f) - 1.0f;
    }
    return 0.0f;
}

float APU::getChannel3DACOut() {
    if (channelThree.dacEnabled) {
        u8_fast sample = ioRegisters.getWaveRAM()[channelThree.wavePosition / 2];
        sample = (sample >> (((channelThree.wavePosition & 1u) != 0) ? 4u : 0u)) & 0b00001111u;

        float dacIn = sample >> ChannelThreeShifts[(ioRegisters.getNR32() & 0b01100000) >> 5];
        return (dacIn / 7.5f) - 1.0f;
    }
    return 0.0f;
}

float APU::getChannel4DACOut() {
    if (channelFour.channelEnabled && channelFour.dacEnabled) {
        float dacIn = ~channelFour.lfsr & 0b01;
        return (dacIn * channelFour.currentVolume / 7.5f) - 1.0f;
    }
    return 0.0f;
}

void APU::handleWrite(memory_address addr, u8_fast value) {
    // TODO: Sync channel states here
    // TODO: Check for missing channel state updates here

    switch (addr) {
        case FB_REG_NR11:
            channelOne.lengthTimer = 64 - (value & FB_LENGTH_DATA_BITS);
            break;
        case FB_REG_NR12:
            channelOne.dacEnabled = (value & 0b11111000) != 0;
            if (!channelOne.dacEnabled) {
                channelOne.channelEnabled = false;
            }
            break;
        case FB_REG_NR14:
            if ((value & FB_TRIGGER_BIT) && channelOne.dacEnabled) {
                doTriggerEvent(0);
            }
            break;
        case FB_REG_NR21:
            channelTwo.lengthTimer = 64 - (value & FB_LENGTH_DATA_BITS);
            break;
        case FB_REG_NR22:
            channelTwo.dacEnabled = (value & 0b11111000) != 0;
            if (!channelTwo.dacEnabled) {
                channelTwo.channelEnabled = false;
            }
            break;
        case FB_REG_NR24:
            if ((value & FB_TRIGGER_BIT) && channelTwo.dacEnabled) {
                doTriggerEvent(1);
            }
            break;
        case FB_REG_NR30:
            channelThree.dacEnabled = (value & 0b10000000) != 0;
            if (!channelThree.dacEnabled) {
                channelThree.channelEnabled = false;
            }
            break;
        case FB_REG_NR31:
            channelThree.lengthTimer = 256 - value;
            break;
        case FB_REG_NR34:
            if ((value & FB_TRIGGER_BIT) && channelThree.dacEnabled) {
                doTriggerEvent(2);
            }
            break;
        case FB_REG_NR41:
            channelFour.lengthTimer = 64 - (value & FB_LENGTH_DATA_BITS);
            break;
        case FB_REG_NR42:
            channelFour.dacEnabled = (value & 0b11111000) != 0;
            if (!channelFour.dacEnabled) {
                channelFour.channelEnabled = false;
            }
            break;
        case FB_REG_NR44:
            if ((value & FB_TRIGGER_BIT) && channelFour.dacEnabled) {
                doTriggerEvent(3);
            }
            break;
        case FB_REG_NR52: {
            // TODO: Implement read of sound ON flags
            bool enabled = value & 0b10000000u;
            if (!enabled && apuEnabled) {
                for (memory_address addr = FB_REG_NR10 ; addr <= FB_REG_NR51 ; addr++) {
                    writeToMemory(addr, 0x00);
                }

                apuEnabled = false;
            } else if (enabled && !apuEnabled) {
                frameSeqStep = 0;

                channelOne.wavePosition = 0;
                channelTwo.wavePosition = 0;
                channelThree.wavePosition = 0;

                apuEnabled = true;
            }
            break;
        }
        default:
            break;
    }
}

void APU::serialize(std::ostream &ostream) const {
    channelOne.serialize(ostream);
    channelTwo.serialize(ostream);
    channelThree.serialize(ostream);
    channelFour.serialize(ostream);

    ostream.put(frameSeqStep);
    ostream.put(apuEnabled);
}

void APU::deserialize(std::istream &istream) {
    channelOne.deserialize(istream);
    channelTwo.deserialize(istream);
    channelThree.deserialize(istream);
    channelFour.deserialize(istream);

    frameSeqStep = istream.get();
    apuEnabled = istream.get();
}

#endif
