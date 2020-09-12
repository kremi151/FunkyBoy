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

#include "display_sdl.h"
#include <util/typedefs.h>

using namespace FunkyBoy::Controller;

DisplayControllerSDL::DisplayControllerSDL(SDL_Renderer *renderer, SDL_Texture *frameBuffer, const FunkyBoy::Palette::palette &palette)
    : renderer(renderer)
    , frameBuffer(frameBuffer)
    , pixels(new uint32_t[FB_GB_DISPLAY_WIDTH * FB_GB_DISPLAY_HEIGHT])
    , palette(palette)
{
}

DisplayControllerSDL::~DisplayControllerSDL() {
    delete[] pixels;
}

void DisplayControllerSDL::drawScanLine(FunkyBoy::u8 y, FunkyBoy::u8 *buffer) {
    uint32_t pixel;
    for (u8 x = 0 ; x < FB_GB_DISPLAY_WIDTH ; x++) {
        auto &color = palette[*(buffer + x)];
        pixel = (255u << 24u) | (color.getRed() << 16) | (color.getGreen() << 8) | color.getBlue();
        pixels[(y * FB_GB_DISPLAY_WIDTH) + x] = pixel;
    }
}

void DisplayControllerSDL::drawScreen() {
    SDL_UpdateTexture(frameBuffer, nullptr, pixels, FB_GB_DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frameBuffer, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
