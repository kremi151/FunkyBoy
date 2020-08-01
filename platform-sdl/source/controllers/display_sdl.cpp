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

DisplayControllerSDL::DisplayControllerSDL(SDL_Renderer *renderer, SDL_Texture *frameBuffer)
    : renderer(renderer)
    , frameBuffer(frameBuffer)
    , pixels(new uint32_t[FB_GB_DISPLAY_WIDTH * FB_GB_DISPLAY_HEIGHT])
{
}

DisplayControllerSDL::~DisplayControllerSDL() {
    delete[] pixels;
}

void DisplayControllerSDL::bufferPixel(FunkyBoy::u8 x, FunkyBoy::u8 y, FunkyBoy::u8 r, FunkyBoy::u8 g, FunkyBoy::u8 b) {
    uint32_t pixel = (255u << 24u) | (r << 16) | (g << 8) | b;
    pixels[(y * FB_GB_DISPLAY_WIDTH) + x] = pixel;
}

void DisplayControllerSDL::drawScreen() {
    SDL_UpdateTexture(frameBuffer, nullptr, pixels, FB_GB_DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frameBuffer, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
