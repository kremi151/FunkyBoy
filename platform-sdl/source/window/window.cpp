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
#include <controllers/display_sdl.h>
#include <ui/native_ui.h>
#include <fstream>
#include <cstring>

using namespace FunkyBoy::SDL;

Window::Window(FunkyBoy::GameBoyType gbType)
    : gbType(gbType)
    , controllers(new Controller::Controllers)
    , emulator(GameBoyType::GameBoyDMG, controllers)
    , window(nullptr)
    , renderer(nullptr)
    , frameBuffer(nullptr)
    , keyboardState(SDL_GetKeyboardState(nullptr))
    , fullscreenRequestedPreviously(false)
    , btnAWasPressed(false)
    , btnBWasPressed(false)
    , btnStartWasPressed(false)
    , btnSelectWasPressed(false)
    , btnUpWasPressed(false)
    , btnDownWasPressed(false)
    , btnLeftWasPressed(false)
    , btnRightWasPressed(false)
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

bool Window::init(int argc, char **argv, size_t width, size_t height) {
    window = SDL_CreateWindow(
            FB_NAME,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_RESIZABLE
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    frameBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_RenderSetLogicalSize(renderer, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT);

    controllers->setSerial(std::make_shared<Controller::SerialControllerSDL>());
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
        emulator.savePath = savePath;

        loadSave();

        char romTitleSafe[FB_ROM_HEADER_TITLE_BYTES + 1]{};
        std::memcpy(romTitleSafe, reinterpret_cast<const char*>(emulator.getROMHeader()->title), FB_ROM_HEADER_TITLE_BYTES);
        std::string title = romTitleSafe;
        title += " - " FB_NAME;
        SDL_SetWindowTitle(window, title.c_str());
        return true;
    } else {
        std::cerr << "Could not load ROM at " << romPath << " (status=" << status << ")" << std::endl;
        return false;
    }
}

void Window::updateInputs() {
    // Poll keyboard inputs once per frame
    SDL_PollEvent(&sdlEvents);

    if (sdlEvents.type == SDL_KEYDOWN || sdlEvents.type == SDL_KEYUP) {
        auto scancode = sdlEvents.key.keysym.scancode;
        bool pressed = sdlEvents.type == SDL_KEYDOWN;
        bool wasPressed;
        Controller::JoypadKey key;
        if (scancode == SDL_SCANCODE_Q) {
            key = Controller::JoypadKey::JOYPAD_A;
            wasPressed = btnAWasPressed;
            btnAWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_W) {
            key = Controller::JoypadKey::JOYPAD_B;
            wasPressed = btnBWasPressed;
            btnBWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_P) {
            key = Controller::JoypadKey::JOYPAD_SELECT;
            wasPressed = btnSelectWasPressed;
            btnSelectWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_O) {
            key = Controller::JoypadKey::JOYPAD_START;
            wasPressed = btnStartWasPressed;
            btnStartWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_UP) {
            key = Controller::JoypadKey::JOYPAD_UP;
            wasPressed = btnUpWasPressed;
            btnUpWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_DOWN) {
            key = Controller::JoypadKey::JOYPAD_DOWN;
            wasPressed = btnDownWasPressed;
            btnDownWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_LEFT) {
            key = Controller::JoypadKey::JOYPAD_LEFT;
            wasPressed = btnLeftWasPressed;
            btnLeftWasPressed = pressed;
        } else if (scancode == SDL_SCANCODE_RIGHT) {
            key = Controller::JoypadKey::JOYPAD_RIGHT;
            wasPressed = btnRightWasPressed;
            btnRightWasPressed = pressed;
        } else {
            return;
        }
        if (wasPressed != pressed) {
            emulator.setInputState(key, pressed);
        }
    }
}

void Window::doFrame() {
    ret_code result;
    do {
        result = emulator.doTick();
    } while ((result & FB_RET_NEW_FRAME) == 0);

    updateInputs();

    // Toggle fullscreen mode
    if (keyboardState[SDL_SCANCODE_F]) {
        if (!fullscreenRequestedPreviously) {
            toggleFullscreen();
        }
        fullscreenRequestedPreviously = true;
    } else {
        fullscreenRequestedPreviously = false;
    }
}

void Window::loadSave() {
    if (!savePath.empty() && emulator.getCartridgeRamSize() > 0 && fs::exists(savePath)) {
        std::ifstream file(savePath, std::ios::binary | std::ios::in);
        emulator.loadCartridgeRam(file);
    }
}

void Window::writeSave() {
    if (!savePath.empty() && emulator.getCartridgeRamSize() > 0) {
        std::ofstream file(savePath, std::ios::binary | std::ios::out);
        emulator.writeCartridgeRam(file);
    }
}

void Window::toggleFullscreen() {
    auto flags = SDL_GetWindowFlags(window);
    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

void Window::deinit() {
    writeSave();
}
