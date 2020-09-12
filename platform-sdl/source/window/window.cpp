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
#include <ui/native_ui.h>
#include <fstream>

using namespace FunkyBoy::SDL;

Window::Window(FunkyBoy::GameBoyType gbType): gbType(gbType)
    , controllers(new Controller::Controllers)
    , emulator(GameBoyType::GameBoyDMG, controllers)
    , window(nullptr)
    , renderer(nullptr)
    , frameBuffer(nullptr)
{
}

Window::~Window() {
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (frameBuffer != nullptr) {
        SDL_DestroyTexture(frameBuffer);
    }
}

bool Window::init(int argc, char **argv) {
    window = SDL_CreateWindow(
            FB_NAME,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            FB_GB_DISPLAY_WIDTH * 3,
            FB_GB_DISPLAY_HEIGHT * 3,
            SDL_WINDOW_RESIZABLE
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    frameBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_RenderSetLogicalSize(renderer, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT);

    controllers->setSerial(std::make_shared<Controller::SerialControllerSDL>());
    controllers->setJoypad(std::make_shared<Controller::JoypadControllerSDL>());
    controllers->setDisplay(std::make_shared<Controller::DisplayControllerSDL>(renderer, frameBuffer));

    fs::path romPath;
    if (argc <= 1) {
        std::cerr << "No ROM specified as command line argument" << std::endl;

        std::vector<NativeUI::file_type> romExtensions;
        romExtensions.push_back({ "gb", "GameBoy ROM" });
        romExtensions.push_back({ "bin", "GameBoy ROM" });

        std::vector<fs::path> selectedPaths;

        NativeUI::selectFiles(window, "Select a Gameboy ROM", romExtensions, false, selectedPaths);

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

void Window::update() {
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
