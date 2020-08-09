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
    , scanLineBuffer(new u8[FB_GB_DISPLAY_WIDTH])
{
}

PPU::~PPU() {
    delete[] scanLineBuffer;
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
    const memory_address tileSetAddr = __fb_lcdc_bgAndWindowTileDataSelect(lcdc);
    const bool bgEnabled = __fb_lcdc_isBGEnabled(lcdc);
    const bool objEnabled = __fb_lcdc_objEnabled(lcdc);
    const bool windowEnabled = __fb_lcdc_isWindowEnabled(lcdc);
    u16 y;
    u8 tileOffsetX;
    u8 xInTile;
    u8 yInTile;
    u16 tile;
    u16 tileLine;
    u8 palette;
    u8 colorIndex;
    if (bgEnabled) {
        const u8 scx = memory->read8BitsAt(FB_REG_SCX);
        const u8 scy = memory->read8BitsAt(FB_REG_SCY);
        y = ly + scy;
        tileOffsetX = scx / 8;
        xInTile = scx % 8;
        yInTile = y % 8;
        memory_address tileMapAddr = __fb_lcdc_bgTileMapDisplaySelect(lcdc);
        tileMapAddr += ((y & 255u) / 8) * 32;
        palette = memory->read8BitsAt(FB_REG_BGP);
        tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
        for (u8 scanLineX = 0 ; scanLineX < FB_GB_DISPLAY_WIDTH ; scanLineX++) {
            tileLine = memory->read16BitsAt(tileSetAddr + (tile * 16) + (yInTile * 2));
            colorIndex = (tileLine >> (15 - (xInTile & 7u))) & 1u;
            colorIndex |= ((tileLine >> (7 - (xInTile & 7u))) & 1u) << 1;
            scanLineBuffer[scanLineX] = (palette >> (colorIndex * 2u)) & 3u;
            if (++xInTile >= 8) {
                xInTile = 0;
                tileOffsetX = (tileOffsetX + 1) & 31;
                tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
            }
        }
    }
    if (objEnabled) {
        const u8 objPalette0 = memory->read8BitsAt(FB_REG_OBP0);
        const u8 objPalette1 = memory->read8BitsAt(FB_REG_OBP1);
        const u8 objHeight = __fb_lcdc_objSpriteSize(lcdc);
        memory_address objAddr = 0xFE00;
        u8 objY, objX, objFlags;
        bool hide, flipX, flipY;
        u8 yInObj;
        for (u8 objIdx = 0 ; objIdx < 40 ; objIdx++) {
            objY = memory->read8BitsAt(objAddr++) - 16;
            objX = memory->read8BitsAt(objAddr++);
            tile = memory->read8BitsAt(objAddr++);
            objFlags = memory->read8BitsAt(objAddr++);
            if (ly < objY || ly >= objY + objHeight) {
                continue;
            }
            palette = (objFlags & 0b00010000u) ? objPalette1 : objPalette0;
            flipX = objFlags & 0b00100000u;
            flipY = objFlags & 0b01000000u;
            hide  = objFlags & 0b10000000u;
            if (flipY) {
                yInObj = objHeight - (ly - objY) - 1;
            } else {
                yInObj = (ly - objY);
            }

            tileLine = memory->read16BitsAt(FB_TILE_DATA_OBJ + (tile * 16) + (yInObj * 2));
            for (u8 xOnObj = 0 ; xOnObj < 8 ; xOnObj++) {
                u8 x = objX + xOnObj - 8;
                if (x >= FB_GB_DISPLAY_WIDTH) {
                    // Out of display bounds
                    continue;
                }
                if (!hide || !scanLineBuffer[x]) {
                    if (flipX) {
                        colorIndex = (tileLine >> (xOnObj & 7u)) & 1u;
                        colorIndex |= ((tileLine >> (xOnObj & 7u)) & 1u) << 1;
                    } else {
                        colorIndex = (tileLine >> (15 - (xOnObj & 7u))) & 1u;
                        colorIndex |= ((tileLine >> (7 - (xOnObj & 7u))) & 1u) << 1;
                    }
                    scanLineBuffer[x] = (palette >> (colorIndex * 2u)) & 3u;
                }
            }
        }
    }
    if (windowEnabled) {
        u8 wy = memory->read8BitsAt(FB_REG_WY);
        if (ly >= wy) {
            y = ly - wy;
            yInTile = y % 8;
            xInTile = 0;
            tileOffsetX = 0;
            u8 wx = memory->read8BitsAt(FB_REG_WX) - 7;
            memory_address tileMapAddr = __fb_lcdc_windowTileMapDisplaySelect(lcdc);
            tileMapAddr += ((y & 255u) / 8) * 32;
            palette = memory->read8BitsAt(FB_REG_BGP);
            tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
            for (u8 scanLineX = wx ; scanLineX < FB_GB_DISPLAY_WIDTH ; scanLineX++) {
                tileLine = memory->read16BitsAt(tileSetAddr + (tile * 16) + (yInTile * 2));
                colorIndex = (tileLine >> (15 - (xInTile & 7u))) & 1u;
                colorIndex |= ((tileLine >> (7 - (xInTile & 7u))) & 1u) << 1;
                scanLineBuffer[scanLineX] = (palette >> (colorIndex * 2u)) & 3u;
                if (++xInTile >= 8) {
                    xInTile = 0;
                    tileOffsetX = (tileOffsetX + 1) & 31;
                    tile = memory->read8BitsAt(tileMapAddr + tileOffsetX);
                }
            }
        }
    }
    controllers->getDisplay()->drawScanLine(ly, scanLineBuffer);
}
