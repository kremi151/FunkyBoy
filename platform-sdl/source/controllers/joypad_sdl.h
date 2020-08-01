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

#ifndef FB_SDL_CONTROLLERS_JOYPAD_SDL_H
#define FB_SDL_CONTROLLERS_JOYPAD_SDL_H

#include <controllers/joypad.h>
#include <SDL.h>

namespace FunkyBoy::Controller {

    class JoypadControllerSDL: public JoypadController {
    private:
        // Managed by the SDL library, so we must not manually free this pointer
        const Uint8 *keyboardState;
    public:
        JoypadControllerSDL();

        bool isKeyPressed(JoypadKey key) override;
    };

}

#endif //FB_SDL_CONTROLLERS_JOYPAD_SDL_H