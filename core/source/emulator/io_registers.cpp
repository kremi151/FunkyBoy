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

using namespace FunkyBoy;

io_registers::io_registers(const io_registers &registers)
    : sys_counter_lsb(registers.sys_counter_lsb)
    , sys_counter_msb(registers.sys_counter_msb)
    , hwIO(registers.hwIO)
    , controllers(registers.controllers)
    , inputsDPad(registers.inputsDPad)
    , inputsButtons(registers.inputsButtons)
    , ptrCounter(registers.ptrCounter)
{
    (*ptrCounter)++;
}

io_registers::io_registers(Controller::ControllersPtr controllers)
    : sys_counter_lsb(new u8(0))
    , sys_counter_msb(new u8(0))
    , hwIO(new u8[128]{})
    , controllers(std::move(controllers))
    , inputsDPad(new u8_fast(0b11111111u))
    , inputsButtons(new u8_fast(0b11111111u))
    , ptrCounter(new u16(1))
{
}

io_registers::~io_registers() {
    if (--(*ptrCounter) < 1) {
        delete sys_counter_lsb;
        delete sys_counter_msb;
        delete[] hwIO;
        delete inputsDPad;
        delete inputsButtons;
        delete ptrCounter;
    }
}

void io_registers::resetSysCounter() {
    *sys_counter_lsb = 0;
    *sys_counter_msb = 0;
}

void io_registers::setSysCounter(FunkyBoy::u16 counter) {
    *sys_counter_lsb = counter & 0xffu;
    *sys_counter_msb = (counter >> 8) & 0xffu;
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
            value = 0b11000000u           // Bits 6 and 7 always read '1'
                  | (value & 0b00110000u) // Keep the two writable bits
                  | currentP1;            // Take the read-only bits from the current P1 value
            *(hwIO + __FB_REG_OFFSET_P1) = value;
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
            return *sys_counter_msb;
        default:
            return *(hwIO + offset);
    }
}

void io_registers::setInputState(Controller::JoypadKey key, bool pressed) {
    switch (key) {
        case Controller::JoypadKey::JOYPAD_A:
            if (pressed) {
                *inputsButtons &= 0b11111110u;
            } else {
                *inputsButtons |= 0b00000001u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_B:
            if (pressed) {
                *inputsButtons &= 0b11111101u;
            } else {
                *inputsButtons |= 0b00000010u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_SELECT:
            if (pressed) {
                *inputsButtons &= 0b11111011u;
            } else {
                *inputsButtons |= 0b00000100u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_START:
            if (pressed) {
                *inputsButtons &= 0b11110111u;
            } else {
                *inputsButtons |= 0b00001000u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_RIGHT:
            if (pressed) {
                *inputsDPad &= 0b11111110u;
            } else {
                *inputsDPad |= 0b00000001u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_LEFT:
            if (pressed) {
                *inputsDPad &= 0b11111101u;
            } else {
                *inputsDPad |= 0b00000010u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_UP:
            if (pressed) {
                *inputsDPad &= 0b11111011u;
            } else {
                *inputsDPad |= 0b00000100u;
            }
            break;
        case Controller::JoypadKey::JOYPAD_DOWN:
            if (pressed) {
                *inputsDPad &= 0b11110111u;
            } else {
                *inputsDPad |= 0b00001000u;
            }
            break;
    }
}

u8_fast io_registers::updateJoypad() {
    u8 &p1 = *(hwIO + __FB_REG_OFFSET_P1);
    u8_fast originalValue = p1;
    u8_fast val = originalValue | 0b11001111u;
    if ((originalValue & 0b00100000u) == 0) {
        // Select Button keys
        val &= *inputsButtons;
    }
    if ((originalValue & 0b00010000u) == 0) {
        // Select Direction keys
        val &= *inputsDPad;
    }
    p1 = val;
    return val;
}
