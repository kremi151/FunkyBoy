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

#include "display_libretro.h"

#include <palette/dmg_palette.h>

using namespace FunkyBoy::Controller;

DisplayControllerLibretro::DisplayControllerLibretro()
    : pixels(new uint32_t[FB_GB_DISPLAY_WIDTH * FB_GB_DISPLAY_HEIGHT]{})
    , videoCb(nullptr)
{
}

DisplayControllerLibretro::~DisplayControllerLibretro() {
    delete[] pixels;
}

void DisplayControllerLibretro::drawScanLine(FunkyBoy::u8_fast y, FunkyBoy::u8 *buffer) {
    uint32_t pixel;
    for (u8 x = 0 ; x < FB_GB_DISPLAY_WIDTH ; x++) {
        auto &color = Palette::ARGB8888::DMG[*(buffer + x)];
        pixel = (255u << 24u) | (color[0] << 16) | (color[1] << 8) | color[2];
        pixels[(y * FB_GB_DISPLAY_WIDTH) + x] = pixel;
    }
}

void DisplayControllerLibretro::drawScreen() {
    if (videoCb != nullptr) {
        videoCb(pixels, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT, FB_GB_DISPLAY_WIDTH * sizeof(uint32_t));
    }
}

void DisplayControllerLibretro::setVideoCallback(retro_video_refresh_t cb) {
    videoCb = cb;
}