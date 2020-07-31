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

#ifndef FB_SDL_WINDOW_H
#define FB_SDL_WINDOW_H

#include <SDL.h>
#include <util/typedefs.h>
#include <emulator/emulator.h>

namespace FunkyBoy::SDL {

    class Window {
    private:
        const GameBoyType gbType;

        SDL_Event sdlEvents{};

        Controller::ControllersPtr controllers;
        Emulator emulator;
    public:
        explicit Window(GameBoyType gbType);

        void init(SDL_Window *window, int argc, char **argv);
        void update(SDL_Window *window);

        fb_inline bool hasUserRequestedExit();
    };

}

#endif //FB_SDL_WINDOW_H
