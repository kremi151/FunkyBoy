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

#include "io_registers.h"
#include <exception/read_exception.h>
#include <util/stream_utils.h>

using namespace FunkyBoy;

#define FB_HW_IO_BYTES 128

io_registers::io_registers(const io_registers &registers)
    : sys_counter(registers.sys_counter)
    , hwIO(registers.hwIO)
    , inputsDPad(registers.inputsDPad)
    , inputsButtons(registers.inputsButtons)
    , inputsChanged(registers.inputsChanged)
    , ptrCounter(registers.ptrCounter)
{
    (*ptrCounter)++;
}

io_registers::io_registers()
    : sys_counter(new u16(0))
    , hwIO(new u8[FB_HW_IO_BYTES]{})
    , inputsDPad(new u8_fast(0b11111111u))
    , inputsButtons(new u8_fast(0b11111111u))
    , inputsChanged(new bool(false))
    , ptrCounter(new u16(1))
{
}

io_registers::~io_registers() {
    if (--(*ptrCounter) < 1) {
        delete sys_counter;
        delete[] hwIO;
        delete inputsDPad;
        delete inputsButtons;
        delete inputsChanged;
        delete ptrCounter;
    }
}

void io_registers::resetSysCounter() {
    *sys_counter = 0;
}

void io_registers::handleMemoryWrite(u8 offset, u8 value) {
    switch (offset) {
        case __FB_REG_OFFSET_DIV: {
            // Direct write to DIV ; reset to 0
            resetSysCounter();
            break;
        }
        case __FB_REG_OFFSET_P1: {
            // Only bits 4 and 5 are writable
            u8 currentP1 = *(hwIO + __FB_REG_OFFSET_P1) & 0b00001111u;
            value = (value & 0b00110000u) // Keep the two writable bits (Bits 6 and 7 always read '1' and are set to '1' by calculateP1Value)
                  | currentP1;            // Take the read-only bits from the current P1 value
            *(hwIO + __FB_REG_OFFSET_P1) = calculateP1Value(value);
            break;
        }
        case __FB_REG_OFFSET_STAT: {
            // Only bits 3-6 are writable, bit 7 reads always '1'
            value = (value & 0b01111000u) | 0b10000000u;
            value |= *(hwIO + __FB_REG_OFFSET_STAT) & 0b00000111u;
            *(hwIO + __FB_REG_OFFSET_STAT) = value;
            break;
        }
        default: {
            *(hwIO + offset) = value;
            break;
        }
    }
}

u8 io_registers::handleMemoryRead(u8 offset) {
    switch (offset) {
        case __FB_REG_OFFSET_DIV:
            return *sys_counter >> 8;
        default:
            return *(hwIO + offset);
    }
}

void io_registers::setInputState(Controller::JoypadKey key, bool pressed) {
    u8_fast nextInputsButtons = *inputsButtons;
    u8_fast nextInputsDPad = *inputsDPad;
    switch (key) {
        case Controller::JoypadKey::JOYPAD_A:
            if (pressed) {
                nextInputsButtons &= 0b11111110u;
            } else {
                nextInputsButtons |= 0b00000001u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_B:
            if (pressed) {
                nextInputsButtons &= 0b11111101u;
            } else {
                nextInputsButtons |= 0b00000010u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_SELECT:
            if (pressed) {
                nextInputsButtons &= 0b11111011u;
            } else {
                nextInputsButtons |= 0b00000100u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_START:
            if (pressed) {
                nextInputsButtons &= 0b11110111u;
            } else {
                nextInputsButtons |= 0b00001000u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_RIGHT:
            if (pressed) {
                nextInputsDPad &= 0b11111110u;
            } else {
                nextInputsDPad |= 0b00000001u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_LEFT:
            if (pressed) {
                nextInputsDPad &= 0b11111101u;
            } else {
                nextInputsDPad |= 0b00000010u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_UP:
            if (pressed) {
                nextInputsDPad &= 0b11111011u;
            } else {
                nextInputsDPad |= 0b00000100u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_DOWN:
            if (pressed) {
                nextInputsDPad &= 0b11110111u;
            } else {
                nextInputsDPad |= 0b00001000u;
            }
            break;
    }
    if (nextInputsButtons != *inputsButtons || nextInputsDPad != *inputsDPad) {
        *inputsButtons = nextInputsButtons;
        *inputsDPad = nextInputsDPad;
        *inputsChanged = true;
        updateJoypad();
    }
}

u8_fast io_registers::calculateP1Value(u8_fast inP1) {
    u8_fast val = inP1 | 0b11001111u;
    if ((inP1 & 0b00100000u) == 0) {
        // Select Button keys
        val &= *inputsButtons;
    }
    if ((inP1 & 0b00010000u) == 0) {
        // Select Direction keys
        val &= *inputsDPad;
    }
    return val;
}

void io_registers::serialize(std::ostream &ostream) const {
    ostream.write(reinterpret_cast<const char*>(hwIO), FB_HW_IO_BYTES);
    ostream.put(*inputsDPad & 0xffu);
    ostream.put(*inputsButtons & 0xffu);
    ostream.put(*inputsChanged);
    Util::Stream::write16Bits(*sys_counter, ostream);
}

void io_registers::deserialize(std::istream &istream) {
    istream.read(reinterpret_cast<char*>(hwIO), FB_HW_IO_BYTES);
    if (!istream) {
        throw Exception::ReadException("Stream is too short (HWIO)");
    }

    char buffer[3];
    istream.read(buffer, sizeof(buffer));
    if (!istream) {
        throw Exception::ReadException("Stream is too short (IO registers)");
    }
    *inputsDPad = buffer[0];
    *inputsButtons = buffer[1];
    *inputsChanged = buffer[2];
    *sys_counter = Util::Stream::read16Bits(istream);
}