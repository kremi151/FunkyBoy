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

#include "joypad_libretro.h"

using namespace FunkyBoy::Controller;

JoypadControllerLibretro::JoypadControllerLibretro()
    : inputStateCb(nullptr)
    , controllerDevice(0)
    , controllerPort(0)
{
}

bool JoypadControllerLibretro::isKeyPressed(JoypadKey key) {
    if (inputStateCb == nullptr) {
        return false;
    }
    switch (key) {
        case JoypadKey::JOYPAD_A:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_A);
        case JoypadKey::JOYPAD_B:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_B);
        case JoypadKey::JOYPAD_UP:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_UP);
        case JoypadKey::JOYPAD_RIGHT:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
        case JoypadKey::JOYPAD_DOWN:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
        case JoypadKey::JOYPAD_LEFT:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
        case JoypadKey::JOYPAD_START:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_START);
        case JoypadKey::JOYPAD_SELECT:
            return inputStateCb(controllerPort, controllerDevice, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
        default:
            return false;
    }
}

void JoypadControllerLibretro::setInputCallback(retro_input_state_t cb, unsigned port, unsigned device) {
    inputStateCb = cb;
    controllerDevice = device;
    controllerPort = port;
}