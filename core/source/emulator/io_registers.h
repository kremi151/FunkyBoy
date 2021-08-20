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

#ifndef FB_CORE_IO_REGISTERS_H
#define FB_CORE_IO_REGISTERS_H

#include <util/typedefs.h>
#include <util/testing.h>
#include <controllers/controllers.h>
#include <util/gpumode.h>

#include <iostream>

#define FB_REG_P1 0xFF00
#define FB_REG_SB 0xFF01
#define FB_REG_SC 0xFF02
#define FB_REG_DIV 0xFF04
#define FB_REG_TIMA 0xFF05
#define FB_REG_TMA 0xFF06
#define FB_REG_TAC 0xFF07
#define FB_REG_IF 0xFF0F
#define FB_REG_NR10 0xFF10
#define FB_REG_NR11 0xFF11
#define FB_REG_NR12 0xFF12
#define FB_REG_NR13 0xFF13
#define FB_REG_NR14 0xFF14
#define FB_REG_NR21 0xFF16
#define FB_REG_NR22 0xFF17
#define FB_REG_NR23 0xFF18
#define FB_REG_NR24 0xFF19
#define FB_REG_NR31 0xFF1B
#define FB_REG_NR32 0xFF1C
#define FB_REG_NR33 0xFF1D
#define FB_REG_NR34 0xFF1E
#define FB_REG_NR41 0xFF20
#define FB_REG_NR42 0xFF21
#define FB_REG_NR44 0xFF23
#define FB_REG_LCDC 0xFF40
#define FB_REG_STAT 0xFF41
#define FB_REG_SCY 0xFF42
#define FB_REG_SCX 0xFF43
#define FB_REG_LY 0xFF44
#define FB_REG_LYC 0xFF45
#define FB_REG_DMA 0xFF46
#define FB_REG_BGP 0xFF47
#define FB_REG_OBP0 0xFF48
#define FB_REG_OBP1 0xFF49
#define FB_REG_WY 0xFF4A
#define FB_REG_WX 0xFF4B
#define FB_REG_IE 0xFFFF

#define __FB_REG_OFFSET_P1 (FB_REG_P1 - 0xFF00)
#define __FB_REG_OFFSET_DIV (FB_REG_DIV - 0xFF00)
#define __FB_REG_OFFSET_TIMA (FB_REG_TIMA - 0xFF00)
#define __FB_REG_OFFSET_TMA (FB_REG_TMA - 0xFF00)
#define __FB_REG_OFFSET_TAC (FB_REG_TAC - 0xFF00)
#define __FB_REG_OFFSET_IF (FB_REG_IF - 0xFF00)
#define __FB_REG_OFFSET_LCDC (FB_REG_LCDC - 0xFF00)
#define __FB_REG_OFFSET_STAT (FB_REG_STAT - 0xFF00)
#define __FB_REG_OFFSET_SCY (FB_REG_SCY - 0xFF00)
#define __FB_REG_OFFSET_SCX (FB_REG_SCX - 0xFF00)
#define __FB_REG_OFFSET_LY (FB_REG_LY - 0xFF00)
#define __FB_REG_OFFSET_LYC (FB_REG_LYC - 0xFF00)
#define __FB_REG_OFFSET_BGP (FB_REG_BGP - 0xFF00)
#define __FB_REG_OFFSET_OBP0 (FB_REG_OBP0 - 0xFF00)
#define __FB_REG_OFFSET_OBP1 (FB_REG_OBP1 - 0xFF00)
#define __FB_REG_OFFSET_WY (FB_REG_WY - 0xFF00)
#define __FB_REG_OFFSET_WX (FB_REG_WX - 0xFF00)

#define __FB_REG_GETTER(name, offset) \
inline u8 &get ## name () { \
    return *(hwIO + offset - 0xFF00); \
} \
inline u8 get ## name () const { \
    return *(hwIO + offset - 0xFF00); \
}

namespace FunkyBoy {

    /* Forward declaration */
    class CPU;

    class io_registers {
    private:
        u16 *ptrCounter;
        u8 *hwIO;
        Controller::ControllersPtr controllers;
        void resetSysCounter();

        u8_fast *inputsDPad;
        u8_fast *inputsButtons;
    test_public:
        u16 *sys_counter;
    public:
        io_registers(const io_registers &registers);
        explicit io_registers(Controller::ControllersPtr controllers);
        ~io_registers();

        inline u16 &getSysCounter() {
            return *sys_counter;
        }

        inline void setSysCounter(u16 counter) {
            *sys_counter = counter;
        }

        void handleMemoryWrite(u8 offset, u8 value);
        u8 handleMemoryRead(u8 offset);

        void setInputState(Controller::JoypadKey key, bool pressed);

        u8_fast updateJoypad();

        inline u8 &getP1() {
            return *(hwIO + __FB_REG_OFFSET_P1);
        }

        __FB_REG_GETTER(DIV, FB_REG_DIV)

        inline u8 &getTIMA() {
            return *(hwIO + __FB_REG_OFFSET_TIMA);
        }

        inline u8 &getTMA() {
            return *(hwIO + __FB_REG_OFFSET_TMA);
        }

        inline u8 &getTAC() {
            return *(hwIO + __FB_REG_OFFSET_TAC);
        }

        inline u8 &getIF() {
            return *(hwIO + __FB_REG_OFFSET_IF);
        }

        inline u8 &getLCDC() {
            return *(hwIO + __FB_REG_OFFSET_LCDC);
        }

        inline u8 &getSTAT() {
            return *(hwIO + __FB_REG_OFFSET_STAT);
        }

        inline u8 &getSCX() {
            return *(hwIO + __FB_REG_OFFSET_SCX);
        }

        inline u8 &getSCY() {
            return *(hwIO + __FB_REG_OFFSET_SCY);
        }

        inline u8 &getLY() {
            return *(hwIO + __FB_REG_OFFSET_LY);
        }

        inline u8 &getLYC() {
            return *(hwIO + __FB_REG_OFFSET_LYC);
        }

        inline u8 &getBGP() {
            return *(hwIO + __FB_REG_OFFSET_BGP);
        }

        inline u8 &getOBP0() {
            return *(hwIO + __FB_REG_OFFSET_OBP0);
        }

        inline u8 &getOBP1() {
            return *(hwIO + __FB_REG_OFFSET_OBP1);
        }

        inline u8 &getWX() {
            return *(hwIO + __FB_REG_OFFSET_WX);
        }

        inline u8 &getWY() {
            return *(hwIO + __FB_REG_OFFSET_WY);
        }

        __FB_REG_GETTER(NR10, FB_REG_NR10)
        __FB_REG_GETTER(NR11, FB_REG_NR11)
        __FB_REG_GETTER(NR12, FB_REG_NR12)
        __FB_REG_GETTER(NR13, FB_REG_NR13)
        __FB_REG_GETTER(NR14, FB_REG_NR14)
        __FB_REG_GETTER(NR21, FB_REG_NR21)
        __FB_REG_GETTER(NR22, FB_REG_NR22)
        __FB_REG_GETTER(NR23, FB_REG_NR23)
        __FB_REG_GETTER(NR24, FB_REG_NR24)
        __FB_REG_GETTER(NR31, FB_REG_NR31)
        __FB_REG_GETTER(NR32, FB_REG_NR32)
        __FB_REG_GETTER(NR33, FB_REG_NR33)
        __FB_REG_GETTER(NR34, FB_REG_NR34)
        __FB_REG_GETTER(NR41, FB_REG_NR41)
        __FB_REG_GETTER(NR42, FB_REG_NR42)
        __FB_REG_GETTER(NR44, FB_REG_NR44)

        void serialize(std::ostream &ostream) const;
        void deserialize(std::istream &istream);

        friend class CPU;
    };

}

#undef __FB_REG_GETTER

#endif //FB_CORE_IO_REGISTERS_H
