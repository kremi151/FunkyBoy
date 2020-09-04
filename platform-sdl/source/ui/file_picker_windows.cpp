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

#ifdef _WIN32

#include "file_picker.h"

#include <windows.h>
#include <SDL_syswm.h>

#include <string>
#include <memory>
#include <cstring>
#include <util/fs.h>

using namespace FunkyBoy::SDL;

void FilePicker::init(int argc, char **argv) {
}

void FilePicker::deinit() {
}

void FilePicker::selectFiles(SDL_Window *window, const char *title, const std::vector<std::string> &types, bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles) {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[512];       // buffer for file name

    std::vector<std::string> typeDescriptions;

    size_t bufferSize = 1; // One additional NUL character

    for (const std::string &type : types) {
        std::string description = type + (allowMultiple ? " files" : " file");
        typeDescriptions.emplace_back(description);

        bufferSize += description.size();
        bufferSize++; // NUL character
        bufferSize += type.size();
        bufferSize += 2 + 1; // '*.' and NUL character
    }

    std::unique_ptr<char[]> filterStr(new char[bufferSize]{});

    size_t pos = 0;
    for (size_t i = 0 ; i < types.size() ; i++) {
        const std::string &type = types[i];
        const std::string &description = typeDescriptions[i];
        std::strcat(filterStr.get() + pos, description.c_str());
        pos += description.size() + 1; // Additional NUL character
        std::strcat(filterStr.get() + pos, "*.");
        std::strcat(filterStr.get() + pos, type.c_str());
        pos += type.size() + 2 + 1; // '*.' and Additional NUL character
    }

    SDL_SysWMinfo wmi = {0};
    SDL_GetWindowWMInfo(window, &wmi);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = wmi.info.win.window;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filterStr.get();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
    if (allowMultiple) {
        ofn.Flags |= OFN_ALLOWMULTISELECT;
    }

    if (!GetOpenFileName(&ofn)) {
        return;
    }

    std::string mainToken = ofn.lpstrFile;
    if (!fs::is_directory(mainToken)) {
        outFiles.emplace_back(mainToken);
        return;
    }

    fs::path rootDirectory = mainToken;

    std::string fileName;
    char *resultPtr = ofn.lpstrFile + mainToken.size() + 1;
    while (*resultPtr != '\0') {
        fileName = resultPtr;
        fs::path filePath = rootDirectory / fileName;
        outFiles.emplace_back(filePath);
        resultPtr += fileName.size() + 1;
    }
}

#endif