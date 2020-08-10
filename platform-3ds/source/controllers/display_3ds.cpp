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

#include <3ds.h>

using namespace FunkyBoy::Controller;

DisplayController3DS::DisplayController3DS()
    : offsetEffectiveX(0)
    , offsetEffectiveY(0)
    , offsetsCalculated(false)
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

void DisplayController3DS::drawScanLine(FunkyBoy::u8 y, FunkyBoy::u8 *buffer) {
    u16 width, height;
    u8 *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);
    if (!offsetsCalculated) {
        offsetEffectiveX = (height - FB_GB_DISPLAY_WIDTH) / 2;
        offsetEffectiveY = (width - FB_GB_DISPLAY_HEIGHT) / 2;
        offsetsCalculated = true;
    }
    y = offsetEffectiveY + FB_GB_DISPLAY_HEIGHT - y - 1;
    for (u8 x = 0; x < FB_GB_DISPLAY_WIDTH; x++) {
        u8 *fbOffset = fb + ((offsetEffectiveX + x) * width * 4) + (y * 4);
        *(fbOffset++) = 0xff;
        *(fbOffset++) = dmgPalette[*buffer & 3u][0];
        *(fbOffset++) = dmgPalette[*buffer & 3u][1];
        *(fbOffset) = dmgPalette[*buffer & 3u][2];
        buffer++;
    }
}

void DisplayController3DS::drawScreen() {
    gspWaitForVBlank();
    gfxSwapBuffers();
}