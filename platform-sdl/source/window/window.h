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
#include <util/fs.h>

namespace FunkyBoy::SDL {

    class Window
    {
    private:
        const GameBoyType gbType;

        SDL_Event sdlEvents{};

        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *frameBuffer;

        // Memory is managed by SDL, so we do not free it
        const Uint8 *keyboardState;
        bool fullscreenRequestedPreviously;
        bool saveStateRequestedPreviously;
        bool loadStateRequestedPreviously;
        bool autoResume;

        Emulator emulator;

        fs::path savePath;

        bool btnAWasPressed;
        bool btnBWasPressed;
        bool btnSelectWasPressed;
        bool btnStartWasPressed;
        bool btnUpWasPressed;
        bool btnDownWasPressed;
        bool btnLeftWasPressed;
        bool btnRightWasPressed;

        void updateInputs();

        void saveState();
        void loadState();

        void loadSave();
        void writeSave();
    public:
        explicit Window(GameBoyType gbType);
        ~Window();

        bool init(int argc, char **argv, size_t width, size_t height);
        void onGameLaunched();
        void doFrame();
        void deinit();

        void toggleFullscreen();

        [[nodiscard]] inline bool hasUserRequestedExit() const {
            return sdlEvents.type == SDL_QUIT;
        }
    };

}

#endif //FB_SDL_WINDOW_H
