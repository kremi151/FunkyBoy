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
#include <chrono>
#include <thread>

// 1048576 Hz = 1/4 * 4194304 Hz
#define fb_clock_frequency (1000000000/1048576)

int main(int argc, char **argv) {
    FunkyBoy::SDL::NativeUI::init(argc, argv);

    SDL_Init(SDL_INIT_VIDEO);

    using clock = std::chrono::high_resolution_clock;
    auto next_frame = clock::now();

    FunkyBoy::SDL::Window fbWindow(FunkyBoy::GameBoyType::GameBoyDMG);
    bool romLoaded = fbWindow.init(argc, argv, FB_GB_DISPLAY_WIDTH * 3, FB_GB_DISPLAY_HEIGHT * 3);
    int retCode = 0;

    if (!romLoaded) {
        goto fb_exit;
    }

    while (true) {
        next_frame += std::chrono::nanoseconds(fb_clock_frequency);

        if (romLoaded) {
            fbWindow.update();
        }
        if (fbWindow.hasUserRequestedExit()) {
            break;
        }

        std::this_thread::sleep_until(next_frame);
    }

fb_exit:
    fbWindow.deinit();

    SDL_Quit();

    FunkyBoy::SDL::NativeUI::deinit();
    return retCode;
}
