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

#ifndef FB_CORE_PPU_H
#define FB_CORE_PPU_H

#include <memory/memory.h>
#include <controllers/controllers.h>
#include <emulator/cpu.h>
#include <emulator/io_registers.h>
#include <util/gpumode.h>
#include <util/typedefs.h>

namespace FunkyBoy {

    class PPU {
    private:
        MemoryPtr memory;
        CPUPtr cpu;
        Controller::ControllersPtr controllers;
        io_registers ioRegisters;

        GPUMode gpuMode;

        u16 modeClocks;

        u8 *scanLineBuffer;

        void renderScanline(u8 ly);
    public:
        PPU(MemoryPtr memory, CPUPtr cpu, Controller::ControllersPtr controllers, const io_registers& ioRegisters);
        ~PPU();

        ret_code doClocks(u8 clocks);
    };

}

#endif //FB_CORE_PPU_H
