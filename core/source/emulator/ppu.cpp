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

// 256 px * 256 px * 4 bytes for RGBA
#define FB_PPU_BGBUFFER_SIZE (256 * 256 * 4)

#define __fb_lcdc_isOn(lcdc) (lcdc & 0b10000000u)
// Differences here are 1023
#define __fb_lcdc_windowTileMapDisplaySelect(lcdc) ((lcdc & 0b01000000u) ? 0x9C00 : 0x9800)
#define __fb_lcdc_isWindowEnabled(lcdc) (lcdc & 0b00100000u)
// Differences here are 4095
#define __fb_lcdc_bgAndWindowTileDataSelect(lcdc) ((lcdc & 0b00010000u) ? 0x8000 : 0x8800)
// Differences here are 1023
#define __fb_lcdc_bgTileMapDisplaySelect(lcdc) ((lcdc & 0b00001000u) ? 0x9C00 : 0x9800)
// Returns the height of objects (16 or 8), width is always 8
#define __fb_lcdc_objSpriteSize(lcdc) ((lcdc & 0b00000100u) ? 16 : 8)
#define __fb_lcsc_objEnabled(lcdc) (lcdc & 0b00000010u)
#define __fb_lcdc_isBGEnabled(lcdc) (lcdc & 0b00000001u)

using namespace FunkyBoy;

PPU::PPU(FunkyBoy::MemoryPtr memory, Controller::ControllersPtr controllers): memory(std::move(memory))
, controllers(std::move(controllers))
, lx(0)
, bgBuffer(new u8[FB_PPU_BGBUFFER_SIZE])
{
    for (int i = 0 ; i < FB_PPU_BGBUFFER_SIZE ; i++) {
        bgBuffer[i] = 0;
    }
}

PPU::~PPU() {
    delete[] bgBuffer;
}

void PPU::doClock() {
    // TODO: Finish implementation
    // See https://gbdev.gg8.se/wiki/articles/Video_Display#VRAM_Tile_Data

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

void PPU::drawTilePixel(memory_address tileAddress, u8 tx, u8 ty, u8 dx, u8 dy) {
    u16 tileLine = memory->read16BitsAt(tileAddress + (ty * 2));
    u8 colorIndex = (tileLine >> (8 + (tx & 7u))) & 1u;
    colorIndex |= ((tileLine >> (tx & 7u)) & 1u) << 1;
    controllers->getDisplay()->drawAt(dx, dy, colorIndex);
}