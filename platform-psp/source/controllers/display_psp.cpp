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

#include "display_psp.h"

#include <palette/dmg_palette.h>

using namespace FunkyBoyPSP::Controller;

DisplayController::DisplayController()
        : frameBuffer(new uint32_t[FB_GB_DISPLAY_WIDTH * FB_GB_DISPLAY_HEIGHT]{})
{
}

DisplayController::~DisplayController() {
    delete[] frameBuffer;
}

void DisplayController::drawScanLine(FunkyBoy::u8 y, FunkyBoy::u8 *buffer) {
    uint32_t pixel;
    for (FunkyBoy::u8 x = 0 ; x < FB_GB_DISPLAY_WIDTH ; x++) {
        auto &color = FunkyBoy::Palette::ARGB8888::DMG[*(buffer + x)];
        pixel = (255u << 24u) | (color[0] << 16) | (color[1] << 8) | color[2];
        frameBuffer[(y * FB_GB_DISPLAY_WIDTH) + x] = pixel;
    }
}

void DisplayController::drawScreen() {
    /*gspWaitForVBlank();
    gfxSwapBuffers();
    frameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &frameWidth, &frameHeight);*/
}