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

namespace FunkyBoy {

    enum GPUMode {
        /**
         * Horizontal blank
         */
        GPUMode_0,

        /**
         * Vertical blank
         *
         */
        GPUMode_1,

        /**
         * OAM read mode, drawing scanline
         */
        GPUMode_2,

        /**
         * VRAM read mode, drawing scanline
         */
        GPUMode_3
    };

    class PPU {
    private:
        MemoryPtr memory;
        Controller::ControllersPtr controllers;

        u8 *bgBuffer;

        GPUMode gpuMode;

        u8 scanLine;
        u16 modeClocks;

        void drawTilePixel(memory_address tileAddress, u8 tx, u8 ty, u8 dx, u8 dy);
    public:
        PPU(MemoryPtr memory, Controller::ControllersPtr controllers);
        ~PPU();

        void doClocks(u8 clocks);
    };

}

#endif //FB_CORE_PPU_H
