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

#ifndef FB_SDL_UI_FILE_PICKER_H
#define FB_SDL_UI_FILE_PICKER_H

#include <SDL.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <util/fs.h>

namespace FunkyBoy::SDL::FilePicker {

    typedef struct {
        std::string extension;
        std::string description;
    } file_type;

    void init(int argc, char **argv);
    void deinit();

    void selectFiles(SDL_Window *window, const char *title, const std::vector<file_type> &types, bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles);

}

#endif // FB_SDL_UI_FILE_PICKER_H
