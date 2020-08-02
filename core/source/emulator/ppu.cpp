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

PPU::PPU(FunkyBoy::MemoryPtr memory, CPUPtr cpu, Controller::ControllersPtr controllers)
    : memory(std::move(memory))
    , cpu(std::move(cpu))
    , controllers(std::move(controllers))
    , gpuMode(GPUMode::GPUMode_2)
    , modeClocks(0)
{
    dmgPalette[0][0] = 255;
    dmgPalette[0][1] = 255;
    dmgPalette[0][2] = 255;

    dmgPalette[1][0] = 192;
    dmgPalette[1][1] = 192;
    dmgPalette[1][2] = 192;

    dmgPalette[2][0] = 96;
    dmgPalette[2][1] = 96;
    dmgPalette[2][2] = 96;

    dmgPalette[3][0] = 0;
    dmgPalette[3][1] = 0;
    dmgPalette[3][2] = 0;
}

// GPU Lifecycle:
//
// Period 1: Scanline (Accessing OAM)  | GPU mode 2 | 80 clocks
// Period 2: Scanline (Accessing VRAM) | GPU mode 3 | 172 clocks
// Period 3: Horizontal blank          | GPU mode 0 | 204 clocks
//
// Period X: Vertical blank            | GPU mode 1 | 4560 clocks (10 lines above 144)
//
// In total for 155 scanlines => 70224 clocks

void PPU::doClocks(u8 clocks) {
    // TODO: Finish implementation
    // See https://gbdev.gg8.se/wiki/articles/Video_Display#VRAM_Tile_Data
    // See http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf (pages 22-27)

    auto lcdc = memory->read8BitsAt(FB_REG_LCDC);
    if (!__fb_lcdc_isOn(lcdc)) {
        memory->write8BitsTo(FB_REG_LY, 0x00);
        return; // TODO: Correct?
    }

    auto ly = memory->read8BitsAt(FB_REG_LY);

    modeClocks += clocks;
    switch (gpuMode) {
        case GPUMode::GPUMode_0: {
            if (modeClocks >= 204) {
                modeClocks = 0;
                if (++ly >= FB_GB_DISPLAY_HEIGHT) {
                    gpuMode = GPUMode::GPUMode_1;
                    controllers->getDisplay()->drawScreen();
                    cpu->requestInterrupt(InterruptType::VBLANK);
                } else {
                    gpuMode = GPUMode::GPUMode_2;
                }
            }
            break;
        }
        case GPUMode::GPUMode_1: {
            if (modeClocks >= 4560) { // 10 scan lines
                gpuMode = GPUMode::GPUMode_2;
                ly = 0;
            }
            break;
        }
        case GPUMode::GPUMode_2: {
            if (modeClocks >= 80) {
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_3;
            }
            break;
        }
        case GPUMode::GPUMode_3: {
            if (modeClocks >= 172) {
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_0;
                renderScanline(ly);
            }
            break;
        }
    }

    // TODO: Compare LY with LYC and trigger interrupt

    memory->write8BitsTo(FB_REG_LY, ly);

    /*
    if (__fb_lcdc_isBGEnabled(lcdc)) {
        // TODO: Draw background
    }
    if (__fb_lcdc_isWindowEnabled(lcdc)) {
        // TODO: Draw window
    }
    */
}

void PPU::renderScanline(u8 ly) {
    const u8 lcdc = memory->read8BitsAt(FB_REG_LCDC);
    const u8 scx = memory->read8BitsAt(FB_REG_SCX);
    const u8 scy = memory->read8BitsAt(FB_REG_SCY);
    const u16 y = ly + scy;
    memory_address tileMapAddr = __fb_lcdc_bgTileMapDisplaySelect(lcdc);
    tileMapAddr += ((y & 255u) / 8) * 32;
    memory_address tileSetAddr = __fb_lcdc_bgAndWindowTileDataSelect(lcdc);
    u8 tileOffsetX = scx / 8;
    u8 xInTile = scx % 8;
    const u8 yInTile = y % 8;
    u8 *color;
    u16 tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
    u16 tileLine;
    for (u8 scanLineX = 0 ; scanLineX < 160 ; scanLineX++) {
        tileLine = memory->read16BitsAt(tileSetAddr + (tile * 16) + (yInTile * 2));
        u8 colorIndex = (tileLine >> (15 - (xInTile & 7u))) & 1u;
        colorIndex |= ((tileLine >> (7 - (xInTile & 7u))) & 1u) << 1;
        color = dmgPalette[colorIndex & 3u];
        controllers->getDisplay()->bufferPixel(scanLineX, ly, color[0], color[1], color[2]);
        if (++xInTile >= 8) {
            xInTile = 0;
            tileOffsetX = (tileOffsetX + 1) & 31;
            tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
        }
    }
}
