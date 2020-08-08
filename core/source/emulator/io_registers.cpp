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

#define FB_REG_OFFSET_P1 (FB_REG_P1 - 0xFF00)
#define FB_REG_OFFSET_DIV (FB_REG_DIV - 0xFF00)
#define FB_REG_OFFSET_STAT (FB_REG_STAT - 0xFF00)
#define FB_REG_OFFSET_LY (FB_REG_LY - 0xFF00)

io_registers::io_registers(Controller::ControllersPtr controllers)
    : sys_counter_lsb(0)
    , sys_counter_msb(0)
    , hwIO(new u8[128]{})
    , controllers(std::move(controllers))
{
}

io_registers::~io_registers() {
    delete[] hwIO;
}

void io_registers::resetSysCounter() {
    sys_counter_lsb = 0;
    sys_counter_msb = 0;
}

void io_registers::setSysCounter(FunkyBoy::u16 counter) {
    sys_counter_lsb = counter & 0xffu;
    sys_counter_msb = (counter >> 8) & 0xffu;
}

fb_inline u16 io_registers::getSysCounter() {
    return (sys_counter_msb << 8) | sys_counter_lsb;
}

void io_registers::handleMemoryWrite(u8 offset, u8 value) {
    switch (offset) {
        case FB_REG_OFFSET_DIV: {
            // Direct write to DIV ; reset to 0
            resetSysCounter();
            break;
        }
        case FB_REG_OFFSET_P1: {
            // Only bits 4 and 5 are writable
            u8 currentP1 = *(hwIO + FB_REG_OFFSET_P1) & 0b00001111u;
            value = 0b11000000u           // Bits 6 and 7 always read '1'
                  | (value & 0b00110000u) // Keep the two writable bits
                  | currentP1;            // Take the read-only bits from the current P1 value
            *(hwIO + FB_REG_OFFSET_P1) = value;
            break;
        }
        case FB_REG_OFFSET_STAT: {
            // Only bits 3-6 are writable, bit 7 reads always '1'
            value = (value & 0b01111000u) | 0b10000000u;
            value |= *(hwIO + FB_REG_OFFSET_STAT) & 0b00000111u;
            *(hwIO + FB_REG_OFFSET_STAT) = value;
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
        case FB_REG_OFFSET_DIV:
            return sys_counter_msb;
        default:
            return *(hwIO + offset);
    }
}

u8 io_registers::updateJoypad() {
    u8 &p1 = *(hwIO + FB_REG_OFFSET_P1);
    u8 originalValue = p1;
    u8 val = originalValue | 0b11001111u;
    auto &joypad = *controllers->getJoypad();
    if ((originalValue & 0b00100000u) == 0) {
        // Select Button keys
        if (joypad.isKeyPressed(Controller::JOYPAD_A)) {
            val &= 0b11111110u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_B)) {
            val &= 0b11111101u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_SELECT)) {
            val &= 0b11111011u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_START)) {
            val &= 0b11110111u;
        }
    }
    if ((originalValue & 0b00010000u) == 0) {
        // Select Direction keys
        if (joypad.isKeyPressed(Controller::JOYPAD_RIGHT)) {
            val &= 0b11111110u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_LEFT)) {
            val &= 0b11111101u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_UP)) {
            val &= 0b11111011u;
        }
        if (joypad.isKeyPressed(Controller::JOYPAD_DOWN)) {
            val &= 0b11110111u;
        }
    }
    p1 = val;
    return val;
}

void io_registers::updateLCD(bool lcdOn, GPUMode gpuMode, u8 ly) {
    u8 stat = 0b10000000u; // Bit 7 always returns '1'
    if (lcdOn) {
        stat |= static_cast<u8>(gpuMode) & 0b11u;
    }
    *(hwIO + FB_REG_OFFSET_STAT) = stat;

    *(hwIO + FB_REG_OFFSET_LY) = ly;
}