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

#ifndef FB_LIBRETRO_JOYPAD_LIBRETRO_H
#define FB_LIBRETRO_JOYPAD_LIBRETRO_H

#include <controllers/joypad.h>
#include <libretro.h>

namespace FunkyBoy::Controller {

    class JoypadControllerLibretro: public JoypadController {
    private:
        retro_input_state_t inputStateCb;
        unsigned controllerDevice;
        unsigned controllerPort;
    public:
        JoypadControllerLibretro();

        bool isKeyPressed(JoypadKey key) override;
        void setInputCallback(retro_input_state_t cb, unsigned port, unsigned device);
    };

}

#endif //FB_LIBRETRO_JOYPAD_LIBRETRO_H
