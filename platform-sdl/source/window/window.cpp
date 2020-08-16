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

#include "window.h"

#include <util/fs.h>
#include <controllers/serial_sdl.h>
#include <controllers/joypad_sdl.h>
#include <controllers/display_sdl.h>

using namespace FunkyBoy::SDL;

Window::Window(FunkyBoy::GameBoyType gbType): gbType(gbType)
    , controllers(new Controller::Controllers)
    , emulator(GameBoyType::GameBoyDMG, controllers)
{
}

bool Window::init(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *frameBuffer, int argc, char **argv) {
    controllers->setSerial(std::make_shared<Controller::SerialControllerSDL>());
    controllers->setJoypad(std::make_shared<Controller::JoypadControllerSDL>());
    controllers->setDisplay(std::make_shared<Controller::DisplayControllerSDL>(renderer, frameBuffer));

    if (argc <= 1) {
        std::cerr << "No ROM specified as command line argument" << std::endl;
        return false;
    }
    char *romPath = argv[1];
    std::cout << "Loading ROM from " << romPath << "..." << std::endl;
    auto status = emulator.loadGame(fs::path(argv[1]));
    if (status == CartridgeStatus::Loaded) {
        std::cout << "Loaded ROM at " << romPath << std::endl;

        emulator.loadCartridgeRamFromFS();

        std::string title = reinterpret_cast<const char*>(emulator.getCartridge().getHeader()->title);
        title += " - " FB_NAME;
        SDL_SetWindowTitle(window, title.c_str());
        return true;
    } else {
        std::cerr << "Could not load ROM at " << romPath << " (status=" << status << ")" << std::endl;
        return false;
    }
}

void Window::update(SDL_Window *window) {
    if (emulator.doTick() & FB_RET_NEW_SCANLINE) {
        // Poll keyboard inputs once per frame
        SDL_PollEvent(&sdlEvents);
    }
}

void Window::deinit() {
    emulator.writeCartridgeRamToFS();
}
