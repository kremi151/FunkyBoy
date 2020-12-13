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

#include "controllers_sdl.h"
#include <palette/dmg_palette.h>
#include <SDL.h>
#include <iostream>

namespace FunkyBoy::Controllers::SDL {
    uint32_t *pixelBuffer;

    // Managed by the SDL library, so we must not manually free this pointer
    const Uint8 *keyboardState;
}

void FunkyBoy::Controllers::init() {
    std::cout << "init\n";
    SDL::pixelBuffer = new uint32_t[FB_GB_DISPLAY_WIDTH * FB_GB_DISPLAY_HEIGHT];
    SDL::keyboardState = SDL_GetKeyboardState(nullptr);
}

void FunkyBoy::Controllers::destroy() {
    std::cout << "destroy\n";
    delete[] SDL::pixelBuffer;
}

using namespace FunkyBoy::Controllers;

void Display::drawScanLine(u8 y, const u8 *buffer) {
    uint32_t pixel;
    for (u8 x = 0 ; x < FB_GB_DISPLAY_WIDTH ; x++) {
        auto &color = Palette::ARGB8888::DMG[*(buffer + x)];
        pixel = (255u << 24u) | (color[0] << 16) | (color[1] << 8) | color[2];
        SDL::pixelBuffer[(y * FB_GB_DISPLAY_WIDTH) + x] = pixel;
    }
}

void Display::drawScreen() {
    SDL_UpdateTexture(SDL::frameBuffer, nullptr, SDL::pixelBuffer, FB_GB_DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(SDL::renderer);
    SDL_RenderCopy(SDL::renderer, SDL::frameBuffer, nullptr, nullptr);
    SDL_RenderPresent(SDL::renderer);
}

bool Joypad::isKeyPressed(JoypadKey key) {
    switch (key) {
        case Joypad::JoypadKey::JOYPAD_A:
            return SDL::keyboardState[SDL_SCANCODE_Q];
        case Joypad::JoypadKey::JOYPAD_B:
            return SDL::keyboardState[SDL_SCANCODE_W];
        case Joypad::JoypadKey::JOYPAD_START:
            return SDL::keyboardState[SDL_SCANCODE_O];
        case Joypad::JoypadKey::JOYPAD_SELECT:
            return SDL::keyboardState[SDL_SCANCODE_P];
        case Joypad::JoypadKey::JOYPAD_LEFT:
            return SDL::keyboardState[SDL_SCANCODE_LEFT];
        case Joypad::JoypadKey::JOYPAD_RIGHT:
            return SDL::keyboardState[SDL_SCANCODE_RIGHT];
        case Joypad::JoypadKey::JOYPAD_UP:
            return SDL::keyboardState[SDL_SCANCODE_UP];
        case Joypad::JoypadKey::JOYPAD_DOWN:
            return SDL::keyboardState[SDL_SCANCODE_DOWN];
        default:
            return false;
    }
}

void Serial::sendByte(u8 data) {
    // TODO: This is only temporary so that I see whether my emulator is doing something, to be removed
    std::cout << data;
}