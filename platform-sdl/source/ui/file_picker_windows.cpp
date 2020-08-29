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
#include <string>
#include <memory>
#include <cstring>

using namespace FunkyBoy::SDL;

void FilePicker::selectFiles(const char *title, const std::vector<std::string> &types, bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles) {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[512];       // buffer for file name

    const std::string typeName = "Gameboy ROM";
    size_t bufferSize = 1; // One additional NUL character

    for (const std::string &type : types) {
        bufferSize += typeName.size();
        bufferSize++; // NUL character
        bufferSize += type.size();
        bufferSize += 2 + 1; // '*.' and NUL character
    }

    std::unique_ptr<char[]> filterStr(new char[bufferSize]{});

    size_t pos = 0;
    for (const std::string &type : types) {
        std::strcat(filterStr.get() + pos, typeName.c_str());
        pos += typeName.size() + 1; // Additional NUL character
        std::strcat(filterStr.get() + pos, "*.");
        std::strcat(filterStr.get() + pos, type.c_str());
        pos += type.size() + 2 + 1; // '*.' and Additional NUL character
    }

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filterStr.get();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        fprintf(stdout, "WIN selected file: %s\n", ofn.lpstrFile);
        outFiles.emplace_back(ofn.lpstrFile);
    }
}

#endif