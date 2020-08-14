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

#include "joypad_3ds.h"

#include <3ds.h>

using namespace FunkyBoy::Controller;

bool JoypadController3DS::isKeyPressed(JoypadKey key) {
    u32 kDown = hidKeysDown();
    switch (key) {
        case JoypadKey::JOYPAD_A:
            return kDown & KEY_A;
        case JoypadKey::JOYPAD_B:
            return kDown & KEY_B;
        case JoypadKey::JOYPAD_START:
            return kDown & KEY_START;
        case JoypadKey::JOYPAD_SELECT:
            return kDown & KEY_SELECT;
        case JoypadKey::JOYPAD_LEFT:
            return kDown & KEY_LEFT;
        case JoypadKey::JOYPAD_UP:
            return kDown & KEY_UP;
        case JoypadKey::JOYPAD_RIGHT:
            return kDown & KEY_RIGHT;
        case JoypadKey::JOYPAD_DOWN:
            return kDown & KEY_DOWN;
    }
    return false;
}