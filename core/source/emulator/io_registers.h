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

#define FB_REG_P1 0xFF00
#define FB_REG_SB 0xFF01
#define FB_REG_SC 0xFF02
#define FB_REG_DIV 0xFF04
#define FB_REG_TIMA 0xFF05
#define FB_REG_TMA 0xFF06
#define FB_REG_TAC 0xFF07
#define FB_REG_IF 0xFF0F
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
#define FB_REG_IE 0xFFFF

namespace FunkyBoy {

    /* Forward declaration */
    class CPU;

    class io_registers {
    private:
        u16 *ptrCounter;
        u8 *hwIO;
        Controller::ControllersPtr controllers;
        void setSysCounter(u16 counter);
        void resetSysCounter();
    test_public:
        u8 *sys_counter_lsb;
        u8 *sys_counter_msb;
    public:
        io_registers(const io_registers &registers);
        explicit io_registers(Controller::ControllersPtr controllers);
        ~io_registers();

        fb_inline u16 getSysCounter();

        void handleMemoryWrite(u8 offset, u8 value);
        u8 handleMemoryRead(u8 offset);

        u8 updateJoypad();
        void updateLCD(bool lcdOn, GPUMode gpuMode, u8 ly);

        u8 &getP1();
        u8 &getIF();

        friend class CPU;
    };

}

#endif //FB_CORE_IO_REGISTERS_H
