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

#ifdef FB_USE_QT

#include "native_ui.h"
#include <QFileDialog>
#include <QWindow>
#include <sstream>
#include <SDL_syswm.h>

using namespace FunkyBoy::SDL;

void NativeUI::init(int &argc, char **argv) {
}

void NativeUI::deinit() {
}

void NativeUI::selectFiles(SDL_Window *window, const char *title, const std::vector<file_type> &types,
                             bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles) {
    std::stringstream typeStrBuilder;
    bool first = true;
    for (const file_type &type : types) {
        if (!first) {
            typeStrBuilder << ";;";
        } else {
            first = false;
        }
        typeStrBuilder << type.description << " (*." << type.extension << ")";
    }
    std::string typeStr = typeStrBuilder.str();

    // When QWidget::createWindowContainer is called, it resizes the window and does no longer respect the constraints set using SDL
    // TODO: Find a better solution
    /*SDL_SysWMinfo wmi = {0};
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        fprintf(stderr, "SDL_GetWindowWMInfo error %s : %lu\n", SDL_GetError(), wmi.info.x11.window);
    }
    auto qwindow = QWindow::fromWinId(wmi.info.x11.window);
    auto parent = QWidget::createWindowContainer(qwindow);

    auto fileName = QFileDialog::getOpenFileName(parent, QString::fromUtf8(title), "", QString::fromStdString(typeStr));*/

    if (allowMultiple) {
        auto fileNames = QFileDialog::getOpenFileNames(nullptr, QString::fromUtf8(title), "", QString::fromStdString(typeStr));
        for (const QString &fileName : fileNames) {
            outFiles.emplace_back(fileName.toStdString());
        }
    } else {
        auto fileName = QFileDialog::getOpenFileName(nullptr, QString::fromUtf8(title), "", QString::fromStdString(typeStr));
        outFiles.emplace_back(fileName.toStdString());
    }
}

#endif