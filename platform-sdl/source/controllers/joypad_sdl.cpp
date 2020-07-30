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

using namespace FunkyBoy::Controller;

JoypadControllerSDL::JoypadControllerSDL(const SDL_Event &sdlEvents): sdlEvents(sdlEvents) {
}

bool JoypadControllerSDL::isKeyPressed(JoypadKey key) {
    if (sdlEvents.type != SDL_KEYDOWN) {
        return false;
    }
    auto &keysym = sdlEvents.key.keysym;
    switch (key) {
        case FunkyBoy::Controller::JoypadKey::JOYPAD_A:
            return keysym.sym == SDL_KeyCode::SDLK_q;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_B:
            return keysym.sym == SDL_KeyCode::SDLK_b;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_START:
            return keysym.sym == SDL_KeyCode::SDLK_o;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_SELECT:
            return keysym.sym == SDL_KeyCode::SDLK_p;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_LEFT:
            return keysym.sym == SDL_KeyCode::SDLK_LEFT;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_RIGHT:
            return keysym.sym == SDL_KeyCode::SDLK_RIGHT;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_UP:
            return keysym.sym == SDL_KeyCode::SDLK_UP;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_DOWN:
            return keysym.sym == SDL_KeyCode::SDLK_DOWN;
        default:
            return false;
    }
}