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

#include "joypad_sdl.h"

#include <SDL.h>

using namespace FunkyBoy::Controller;

JoypadControllerSDL::JoypadControllerSDL(): keyboardState(SDL_GetKeyboardState(nullptr)) {
}

bool JoypadControllerSDL::isKeyPressed(JoypadKey key) {
    switch (key) {
        case FunkyBoy::Controller::JoypadKey::JOYPAD_A:
            return keyboardState[SDL_SCANCODE_Q];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_B:
            return keyboardState[SDL_SCANCODE_W];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_START:
            return keyboardState[SDL_SCANCODE_O];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_SELECT:
            return keyboardState[SDL_SCANCODE_P];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_LEFT:
            return keyboardState[SDL_SCANCODE_LEFT];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_RIGHT:
            return keyboardState[SDL_SCANCODE_RIGHT];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_UP:
            return keyboardState[SDL_SCANCODE_UP];
        case FunkyBoy::Controller::JoypadKey::JOYPAD_DOWN:
            return keyboardState[SDL_SCANCODE_DOWN];
        default:
            return false;
    }
}
