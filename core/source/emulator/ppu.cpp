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

#include <util/return_codes.h>
#include <emulator/io_registers.h>

// Lower tile set lives at 0x8000 in memory
#define FB_TILE_DATA_LOWER 0x0000

// Lower tile set lives at 0x8800 in memory.
#define FB_TILE_DATA_UPPER 0x0800

#define FB_SIZEOF_TILE (8 * 2)

#define __fb_lcdc_isOn(lcdc) (lcdc & 0b10000000u)
// Differences here are 1023
#define __fb_lcdc_windowTileMapDisplaySelect(lcdc) ((lcdc & 0b01000000u) ? 0x1C00 : 0x1800)
#define __fb_lcdc_isWindowEnabled(lcdc) (lcdc & 0b00100000u)
// Differences here are 4095
#define __fb_lcdc_useLowerTileData(lcdc) (lcdc & 0b00010000u)
// Differences here are 1023
#define __fb_lcdc_bgTileMapDisplaySelect(lcdc) ((lcdc & 0b00001000u) ? 0x1C00 : 0x1800)
// Returns the height of objects (16 or 8), width is always 8
#define __fb_lcdc_objSpriteSize(lcdc) ((lcdc & 0b00000100u) ? 16 : 8)
#define __fb_lcdc_objEnabled(lcdc) (lcdc & 0b00000010u)
#define __fb_lcdc_isBGEnabled(lcdc) (lcdc & 0b00000001u)

#define __fb_stat_isLYCInterrupt(stat) (stat & 0b01000000u)
#define __fb_stat_isOAMInterrupt(stat) (stat & 0b00100000u)
#define __fb_stat_isVBlankInterrupt(stat) (stat & 0b00010000u)
#define __fb_stat_isHBlankInterrupt(stat) (stat & 0b00001000u)

#define __fb_getTileDataAddress(lcdc) (__fb_lcdc_useLowerTileData(lcdc) ? FB_TILE_DATA_LOWER : FB_TILE_DATA_UPPER)

#define __fb_getTileSetOffset(lcdc, tileData) \
(__fb_lcdc_useLowerTileData(lcdc) ? tileData : (static_cast<i8>(tileData) + 128)) * FB_SIZEOF_TILE

using namespace FunkyBoy;

PPU::PPU(CPUPtr cpu, Controller::ControllersPtr controllers, const io_registers& ioRegisters, const PPUMemory &ppuMemory)
    : cpu(std::move(cpu))
    , controllers(std::move(controllers))
    , ioRegisters(ioRegisters)
    , ppuMemory(ppuMemory)
    , gpuMode(GPUMode::GPUMode_2)
    , modeClocks(0)
    , scanLineBuffer(new u8[FB_GB_DISPLAY_WIDTH])
    , bgColorIndexes(new u8[FB_GB_DISPLAY_WIDTH])
{
    this->ppuMemory.setAccessibilityFromMMU(
            this->gpuMode != GPUMode::GPUMode_3,
            this->gpuMode != GPUMode::GPUMode_2 && this->gpuMode != GPUMode::GPUMode_3
    );
}

PPU::~PPU() {
    delete[] scanLineBuffer;
    delete[] bgColorIndexes;
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

ret_code PPU::doClocks(u8 clocks) {
    // TODO: Finish implementation
    // See https://gbdev.gg8.se/wiki/articles/Video_Display#VRAM_Tile_Data
    // See http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf (pages 22-27)

    auto lcdc = ioRegisters.getLCDC();
    u8 &ly = ioRegisters.getLY();
    u8 &stat = ioRegisters.getSTAT();

    if (!__fb_lcdc_isOn(lcdc)) {
        ly = 0;
        updateStat(stat, ly, false);
        return FB_RET_SUCCESS;
    }

    ret_code result = FB_RET_SUCCESS;

    modeClocks += clocks;
    switch (gpuMode) {
        case GPUMode::GPUMode_0: {
            if (modeClocks >= 204) {
                modeClocks = 0;
                if (++ly >= FB_GB_DISPLAY_HEIGHT) {
                    gpuMode = GPUMode::GPUMode_1;
                    controllers->getDisplay()->drawScreen();
                    cpu->requestInterrupt(InterruptType::VBLANK);
                    if (__fb_stat_isVBlankInterrupt(stat)) {
                        cpu->requestInterrupt(InterruptType::LCD_STAT);
                    }
                    ppuMemory.setAccessibilityFromMMU(true, true);
                    result |= FB_RET_NEW_FRAME;
                } else {
                    gpuMode = GPUMode::GPUMode_2;
                    if (__fb_stat_isOAMInterrupt(stat)) {
                        cpu->requestInterrupt(InterruptType::LCD_STAT);
                    }
                    ppuMemory.setAccessibilityFromMMU(true, false);
                }
                if (__fb_stat_isLYCInterrupt(stat) && ly == ioRegisters.getLYC()) {
                    cpu->requestInterrupt(InterruptType::LCD_STAT);
                }
            }
            break;
        }
        case GPUMode::GPUMode_1: {
            if (modeClocks >= 4560) { // 10 scan lines
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_2;
                if (__fb_stat_isOAMInterrupt(stat)) {
                    cpu->requestInterrupt(InterruptType::LCD_STAT);
                }
                ppuMemory.setAccessibilityFromMMU(true, false);
                ly = 0;
            } else if (modeClocks % 204 == 0) {
                ly++;
                if (__fb_stat_isLYCInterrupt(stat) && ly == ioRegisters.getLYC()) {
                    cpu->requestInterrupt(InterruptType::LCD_STAT);
                }
            }
            break;
        }
        case GPUMode::GPUMode_2: {
            if (modeClocks >= 80) {
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_3;
                ppuMemory.setAccessibilityFromMMU(false, false);
            }
            break;
        }
        case GPUMode::GPUMode_3: {
            if (modeClocks >= 172) {
                modeClocks = 0;
                gpuMode = GPUMode::GPUMode_0;
                if (__fb_stat_isHBlankInterrupt(stat)) {
                    cpu->requestInterrupt(InterruptType::LCD_STAT);
                }
                ppuMemory.setAccessibilityFromMMU(true, true);
                renderScanline(ly);
                result |= FB_RET_NEW_SCANLINE;
            }
            break;
        }
    }

    updateStat(stat, ly, true);
    return result;
}

void PPU::renderScanline(u8 ly) {
    const u8 &lcdc = ioRegisters.getLCDC();
    const memory_address tileSetAddr = __fb_getTileDataAddress(lcdc);
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
    u8 it;
    if (bgEnabled) {
        const u8 &scx = ioRegisters.getSCX();
        const u8 &scy = ioRegisters.getSCY();
        y = ly + scy;
        tileOffsetX = scx / 8;
        xInTile = scx % 8;
        yInTile = y % 8;
        memory_address tileMapAddr = __fb_lcdc_bgTileMapDisplaySelect(lcdc);
        tileMapAddr += ((y & 255u) / 8) * 32;
        palette = ioRegisters.getBGP();
        tile = ppuMemory.getVRAMByte(tileMapAddr + tileOffsetX);
        u8 &scanLineX = it; // alias for it
        for (scanLineX = 0 ; scanLineX < FB_GB_DISPLAY_WIDTH ; scanLineX++) {
            tileLine = ppuMemory.readVRAM16Bits(tileSetAddr + __fb_getTileSetOffset(lcdc, tile) + (yInTile * 2));
            colorIndex = (tileLine >> (15 - xInTile)) & 1u
                | ((tileLine >> (7 - xInTile)) & 1u) << 1;
            scanLineBuffer[scanLineX] = (palette >> (colorIndex * 2u)) & 3u;
            bgColorIndexes[scanLineX] = colorIndex;
            if (++xInTile >= 8) {
                xInTile = 0;
                tileOffsetX = (tileOffsetX + 1) & 31;
                tile = ppuMemory.getVRAMByte(tileMapAddr + tileOffsetX);
            }
        }
    } else {
        // Clear scan line buffer
        u8 &scanLineX = it; // alias for it
        for (scanLineX = 0 ; scanLineX < FB_GB_DISPLAY_WIDTH ; scanLineX++) {
            scanLineBuffer[scanLineX] = 0;
            bgColorIndexes[scanLineX] = 0;
        }
    }
    if (objEnabled) {
        const u8 objPalette0 = ioRegisters.getOBP0();
        const u8 objPalette1 = ioRegisters.getOBP1();
        const u8 objHeight = __fb_lcdc_objSpriteSize(lcdc);
        memory_address objAddr = 0x0000; // 0x0000 ~> 0xFE00 (start of OAM)
        u8 objY, objX, objFlags;
        bool hide, flipX, flipY;
        u8 yInObj;
        u8 &objIdx = it; // alias for it
        for (objIdx = 0 ; objIdx < 40 ; objIdx++) {
            objY = ppuMemory.getOAMByte(objAddr++) - 16;
            objX = ppuMemory.getOAMByte(objAddr++);
            tile = ppuMemory.getOAMByte(objAddr++);
            objFlags = ppuMemory.getOAMByte(objAddr++);
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

            if (objHeight == 16) {
                // In 8x16 mode, the least significant bit of the tile number is treated as '0'
                tile &= 0b11111110u;
            }
            tileLine = ppuMemory.readVRAM16Bits(FB_TILE_DATA_LOWER + (tile * FB_SIZEOF_TILE) + (yInObj * 2));
            for (u8 xOnObj = 0 ; xOnObj < 8 ; xOnObj++) {
                u8 x = objX + xOnObj - 8;
                if (x >= FB_GB_DISPLAY_WIDTH) {
                    // Out of display bounds
                    continue;
                }
                if (!hide || !bgColorIndexes[x]) {
                    if (flipX) {
                        colorIndex = (tileLine >> (8 + xOnObj)) & 1u
                            | ((tileLine >> xOnObj) & 1u) << 1;
                    } else {
                        colorIndex = (tileLine >> (15 - xOnObj)) & 1u
                            | ((tileLine >> (7 - xOnObj)) & 1u) << 1;
                    }
                    if (colorIndex) {
                        scanLineBuffer[x] = (palette >> (colorIndex * 2u)) & 3u;
                    }
                }
            }
        }
    }
    if (windowEnabled) {
        const u8 wy = ioRegisters.getWY();
        if (ly >= wy) {
            y = ly - wy;
            yInTile = y % 8;
            xInTile = 0;
            tileOffsetX = 0;
            const u8 wx = ioRegisters.getWX() - 7;
            memory_address tileMapAddr = __fb_lcdc_windowTileMapDisplaySelect(lcdc);
            tileMapAddr += ((y & 255u) / 8) * 32;
            palette = ioRegisters.getBGP();
            tile = ppuMemory.getVRAMByte(tileMapAddr + tileOffsetX);
            u8 &scanLineX = it; // alias for it
            for (scanLineX = wx ; scanLineX < FB_GB_DISPLAY_WIDTH ; scanLineX++) {
                tileLine = ppuMemory.readVRAM16Bits(tileSetAddr + __fb_getTileSetOffset(lcdc, tile) + (yInTile * 2));
                colorIndex = (tileLine >> (15 - xInTile)) & 1u
                    | ((tileLine >> (7 - xInTile)) & 1u) << 1;
                scanLineBuffer[scanLineX] = (palette >> (colorIndex * 2u)) & 3u;
                if (++xInTile >= 8) {
                    xInTile = 0;
                    tileOffsetX = (tileOffsetX + 1) & 31;
                    tile = ppuMemory.getVRAMByte(tileMapAddr + tileOffsetX);
                }
            }
        }
    }
    controllers->getDisplay()->drawScanLine(ly, scanLineBuffer);
}

void PPU::updateStat(u8 &stat, u8 ly, bool lcdOn) {
    stat |= 0b10000000u; // Bit 7 always returns '1'

    // Clear bits 0, 1 and 2 so that we can update them correctly (if LCD is on)
    stat &= 0b11111000u;
    if (ly == ioRegisters.getLYC()) {
        stat |= 0b00000100u;
    }
    if (lcdOn) {
        stat |= static_cast<u8>(gpuMode) & 0b11u;
    }
    // TODO: Is it correct to let bits 0 & 1 be '0' is LCD is off?
}
