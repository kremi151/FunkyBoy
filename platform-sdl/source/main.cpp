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
#include <util/typedefs.h>
#include <window/window.h>

#include <chrono>
#include <thread>

// 4194304 Hz
#define fb_clock_frequency (1000000000/4194304)

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
            FB_NAME,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    using clock = std::chrono::high_resolution_clock;
    auto next_frame = clock::now();

    FunkyBoy::SDL::Window fbWindow(FunkyBoy::GameBoyType::GameBoyDMG);
    fbWindow.init(argc, argv);

    while (true) {
        next_frame += std::chrono::nanoseconds(fb_clock_frequency);

        fbWindow.update(window);
        if (fbWindow.hasUserRequestedExit()) {
            break;
        }

        std::this_thread::sleep_until(next_frame);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}