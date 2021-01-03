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

#include <SDL.h>
#include <window/window.h>
#include <ui/native_ui.h>
#include <util/frame_executor.h>

void runGame(FunkyBoy::SDL::Window &window);

int main(int argc, char **argv) {
    FunkyBoy::SDL::NativeUI::init(argc, argv);

    SDL_Init(SDL_INIT_VIDEO);

    FunkyBoy::SDL::Window fbWindow(FunkyBoy::GameBoyType::GameBoyDMG);
    bool romLoaded = fbWindow.init(argc, argv, FB_GB_DISPLAY_WIDTH * 3, FB_GB_DISPLAY_HEIGHT * 3);
    int retCode = 0;

    if (romLoaded) {
        runGame(fbWindow);
    }

    fbWindow.deinit();

    SDL_Quit();

    FunkyBoy::SDL::NativeUI::deinit();
    return retCode;
}

void runGame(FunkyBoy::SDL::Window &window) {
    bool running = true;
    FunkyBoy::Util::FrameExecutor executeFrame([&](){
        window.doFrame();
        if (window.hasUserRequestedExit()) {
            running = false;
        }
    }, FB_TARGET_FPS);

    while (running) {
        executeFrame();
    }
}