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
#include <ui/file_picker.h>
#include <fstream>

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

    fs::path romPath;
    if (argc <= 1) {
        std::cerr << "No ROM specified as command line argument" << std::endl;

        std::vector<std::string> romExtensions;
        romExtensions.push_back("gb");
        romExtensions.push_back("bin");

        std::vector<fs::path> selectedPaths;

        FilePicker::selectFiles("Select a Gameboy ROM", romExtensions, false, selectedPaths);

        if (selectedPaths.size() > 0) {
            romPath = selectedPaths[0];
        }
    } else {
        romPath = argv[1];
    }
    if (romPath.empty()) {
        std::cerr << "Empty ROM path specified" << std::endl;
        return false;
    }
    std::cout << "Loading ROM from " << romPath << "..." << std::endl;
    auto status = emulator.loadGame(fs::path(romPath));
    if (status == CartridgeStatus::Loaded) {
        std::cout << "Loaded ROM at " << romPath << std::endl;

        savePath = romPath;
        savePath.replace_extension(".sav");

        loadSave();

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

void Window::loadSave() {
    if (!savePath.empty() && emulator.getCartridge().getRamSize() > 0 && fs::exists(savePath)) {
        std::ifstream file(savePath);
        emulator.loadCartridgeRam(file);
    }
}

void Window::writeSave() {
    if (!savePath.empty() && emulator.getCartridge().getRamSize() > 0) {
        std::ofstream file(savePath);
        emulator.writeCartridgeRam(file);
    }
}

void Window::deinit() {
    writeSave();
}
