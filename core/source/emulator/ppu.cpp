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

#define FB_TILE_DATA_OBJ 0x8000

#define __fb_lcdc_isOn(lcdc) (lcdc & 0b10000000u)
// Differences here are 1023
#define __fb_lcdc_windowTileMapDisplaySelect(lcdc) ((lcdc & 0b01000000u) ? 0x9C00 : 0x9800)
#define __fb_lcdc_isWindowEnabled(lcdc) (lcdc & 0b00100000u)
// Differences here are 4095
#define __fb_lcdc_bgAndWindowTileDataSelect(lcdc) ((lcdc & 0b00010000u) ? FB_TILE_DATA_OBJ : 0x8800)
// Differences here are 1023
#define __fb_lcdc_bgTileMapDisplaySelect(lcdc) ((lcdc & 0b00001000u) ? 0x9C00 : 0x9800)
// Returns the height of objects (16 or 8), width is always 8
#define __fb_lcdc_objSpriteSize(lcdc) ((lcdc & 0b00000100u) ? 16 : 8)
#define __fb_lcdc_objEnabled(lcdc) (lcdc & 0b00000010u)
#define __fb_lcdc_isBGEnabled(lcdc) (lcdc & 0b00000001u)

using namespace FunkyBoy;

PPU::PPU(FunkyBoy::MemoryPtr memory, CPUPtr cpu, Controller::ControllersPtr controllers, io_registers_ptr ioRegisters)
    : memory(std::move(memory))
    , cpu(std::move(cpu))
    , controllers(std::move(controllers))
    , ioRegisters(std::move(ioRegisters))
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
        ioRegisters->updateLCD(false, gpuMode /* TODO: Correct* */, 0x00);
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
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_2;
                ly = 0;
            } else if (modeClocks % 204 == 0) {
                ly++;
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

    ioRegisters->updateLCD(true, gpuMode, ly);
}

void PPU::renderScanline(u8 ly) {
    const u8 lcdc = memory->read8BitsAt(FB_REG_LCDC);
    const u8 scx = memory->read8BitsAt(FB_REG_SCX);
    const u8 scy = memory->read8BitsAt(FB_REG_SCY);
    const u16 y = ly + scy;
    const memory_address tileSetAddr = __fb_lcdc_bgAndWindowTileDataSelect(lcdc);
    const bool bgEnabled = __fb_lcdc_isBGEnabled(lcdc);
    const bool objEnabled = __fb_lcdc_objEnabled(lcdc);
    memory_address bgTileMapAddr = __fb_lcdc_bgTileMapDisplaySelect(lcdc);
    bgTileMapAddr += ((y & 255u) / 8) * 32;
    u8 bgTileOffsetX = scx / 8;
    u8 xInBgTile = scx % 8;
    const u8 yInTile = y % 8;
    u8 *color;
    u16 tile = 0;
    u16 tileLine;
    if (bgEnabled) {
        const u8 bgPalette = memory->read8BitsAt(FB_REG_BGP);
        tile = memory->read8BitsAt(bgTileMapAddr + bgTileOffsetX);
        for (u8 scanLineX = 0 ; scanLineX < 160 ; scanLineX++) {
            tileLine = memory->read16BitsAt(tileSetAddr + (tile * 16) + (yInTile * 2));
            u8 colorIndex = (tileLine >> (15 - (xInBgTile & 7u))) & 1u;
            colorIndex |= ((tileLine >> (7 - (xInBgTile & 7u))) & 1u) << 1;
            color = dmgPalette[(bgPalette >> (colorIndex * 2u)) & 3u];
            controllers->getDisplay()->bufferPixel(scanLineX, ly, color[0], color[1], color[2]);
            if (++xInBgTile >= 8) {
                xInBgTile = 0;
                bgTileOffsetX = (bgTileOffsetX + 1) & 31;
                tile = memory->read8BitsAt(bgTileMapAddr + bgTileOffsetX);
            }
        }
    }
    if (objEnabled) {
        const u8 objPalette0 = memory->read8BitsAt(FB_REG_OBP0);
        const u8 objPalette1 = memory->read8BitsAt(FB_REG_OBP1);
        const u8 objHeight = __fb_lcdc_objSpriteSize(lcdc);
        memory_address objAddr = 0xFE00;
        u8 objY, objX, objTile, objFlags;
        for (u8 objIdx = 0 ; objIdx < 40 ; objIdx++) {
            objY = memory->read8BitsAt(objAddr++) - 16;
            objX = memory->read8BitsAt(objAddr++);
            objTile = memory->read8BitsAt(objAddr++);
            objFlags = memory->read8BitsAt(objAddr++);
            if (ly < objY || ly >= objY + objHeight) {
                continue;
            }
            const u8 objPalette = (objFlags & 0b00010000u) ? objPalette1 : objPalette0;
            const u8 yInObj = (ly - objY);

            tile = memory->read8BitsAt(FB_TILE_DATA_OBJ + objTile);
            tileLine = memory->read16BitsAt(tileSetAddr + (tile * 16) + (yInObj * 2));
            for (u8 xOnObj = 0 ; xOnObj < 8 ; xOnObj++) {
                u8 x = objX + xOnObj - 8;
                if (x >= FB_GB_DISPLAY_WIDTH) {
                    // Out of display bounds
                    continue;
                }
                u8 colorIndex = (tileLine >> (15 - (xOnObj & 7u))) & 1u;
                colorIndex |= ((tileLine >> (7 - (xOnObj & 7u))) & 1u) << 1;
                color = dmgPalette[(objPalette >> (colorIndex * 2u)) & 3u];
                controllers->getDisplay()->bufferPixel(x, ly, color[0], color[1], color[2]);
            }
        }
    }
}
