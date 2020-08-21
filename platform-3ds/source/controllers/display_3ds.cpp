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

#include "display_3ds.h"

#include <palette/dmg_palette.h>
#include <3ds.h>

using namespace FunkyBoy::Controller;

DisplayController3DS::DisplayController3DS()
    : offsetEffectiveX(0)
    , offsetEffectiveY(0)
    , offsetsCalculated(false)
    , frameBuffer(nullptr)
    , frameWidth(0)
    , frameHeight(0)
{
}

void DisplayController3DS::drawScanLine(FunkyBoy::u8 y, FunkyBoy::u8 *buffer) {
    if (!frameBuffer) {
        frameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &frameWidth, &frameHeight);
    }
    if (!offsetsCalculated) {
        offsetEffectiveX = (frameHeight - FB_GB_DISPLAY_WIDTH) / 2;
        offsetEffectiveY = (frameWidth - FB_GB_DISPLAY_HEIGHT) / 2;
        offsetsCalculated = true;
    }
    y = offsetEffectiveY + FB_GB_DISPLAY_HEIGHT - y - 1;
    for (u8 x = 0; x < FB_GB_DISPLAY_WIDTH; x++) {
        u8 *fbOffset = frameBuffer + ((offsetEffectiveX + x) * frameWidth * 4) + (y * 4);
        *(fbOffset++) = 0xff;
        *(fbOffset++) = Palette::ARGB8888::DMG[*buffer & 3u][0];
        *(fbOffset++) = Palette::ARGB8888::DMG[*buffer & 3u][1];
        *(fbOffset) = Palette::ARGB8888::DMG[*buffer & 3u][2];
        buffer++;
    }
}

void DisplayController3DS::drawScreen() {
    gspWaitForVBlank();
    gfxSwapBuffers();
    frameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &frameWidth, &frameHeight);
}