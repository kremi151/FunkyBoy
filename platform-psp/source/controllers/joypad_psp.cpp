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

#include "joypad_psp.h"
#include <user_input.h>
#include <pspctrl.h>

using namespace FunkyBoyPSP::Controller;

bool JoypadController::isKeyPressed(FunkyBoy::Controller::JoypadKey key) {
    uint_fast32_t kDown = Input::getPressedKeys();
    switch (key) {
        case FunkyBoy::Controller::JoypadKey::JOYPAD_A:
            return kDown & PSP_CTRL_CIRCLE;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_B:
            return kDown & PSP_CTRL_CROSS;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_START:
            return kDown & PSP_CTRL_START;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_SELECT:
            return kDown & PSP_CTRL_SELECT;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_LEFT:
            return kDown & PSP_CTRL_LEFT;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_UP:
            return kDown & PSP_CTRL_UP;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_RIGHT:
            return kDown & PSP_CTRL_RIGHT;
        case FunkyBoy::Controller::JoypadKey::JOYPAD_DOWN:
            return kDown & PSP_CTRL_DOWN;
    }
    return false;
}