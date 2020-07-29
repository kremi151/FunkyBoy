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

#include "ppu.h"

#include <util/typedefs.h>
#include <emulator/io_registers.h>

#define __fb_lcdc_isOn(lcdc) (lcdc & 0b10000000u)
// Bit 6 - Window Tile Map
#define __fb_lcdc_isWindowEnabled(lcdc) (lcdc & 0b00100000u)
// Bit 4 - BG & Window Tileset
// Bit 3 - BG Tile Map
// Bit 2 - Sprite Size
// Bit 1 - Sprites Enabled
#define __fb_lcdc_isBGEnabled(lcdc) (lcdc & 0b00000001u)

using namespace FunkyBoy;

PPU::PPU(FunkyBoy::MemoryPtr memory): memory(std::move(memory)) {
}

void PPU::doPixel() {
    auto lcdc = memory->read8BitsAt(FB_REG_LCDC);
    if (!__fb_lcdc_isOn(lcdc)) {
        memory->write8BitsTo(FB_REG_LY, 0x00);
        return; // TODO: Correct?
    }
    auto ly = memory->read8BitsAt(FB_REG_LY);
    if (__fb_lcdc_isBGEnabled(lcdc)) {
        // TODO: Draw background
    }
    if (__fb_lcdc_isWindowEnabled(lcdc)) {
        // TODO: Draw window
    }
    if (++ly == FB_GB_DISPLAY_HEIGHT) {
        // TODO: Draw one line per tick or pixel by pixel?
        // TODO: Trigger interrupt
    } else if (ly >= 153) {
        ly = 0;
    }
    memory->write8BitsTo(FB_REG_LCDC, ly);
}