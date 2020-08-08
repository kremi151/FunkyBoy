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

#ifndef FB_CORE_CONTROLLERS_JOYPAD_H
#define FB_CORE_CONTROLLERS_JOYPAD_H

#include <memory>
#include <util/typedefs.h>

namespace FunkyBoy::Controller {

    enum JoypadKey {
        JOYPAD_A = 0,
        JOYPAD_B = 1,
        JOYPAD_SELECT = 2,
        JOYPAD_START = 3,
        JOYPAD_RIGHT = 4,
        JOYPAD_LEFT = 5,
        JOYPAD_UP = 6,
        JOYPAD_DOWN = 7
    };

    class JoypadController {
    public:
        virtual ~JoypadController() = default;

        virtual bool isKeyPressed(JoypadKey key) = 0;
    };

    typedef std::shared_ptr<JoypadController> JoypadControllerPtr;

}

#endif //FB_CORE_CONTROLLERS_JOYPAD_H
