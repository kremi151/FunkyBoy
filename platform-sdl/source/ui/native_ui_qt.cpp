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

#ifdef OS_LINUX

#include "native_ui.h"
#include <QApplication>
#include <QFileDialog>
#include <QWindow>
#include <memory>
#include <sstream>
#include <SDL_syswm.h>

using namespace FunkyBoy::SDL;

static int __fb_qApplication_argc;
static std::unique_ptr<QApplication> __fb_qApplication;

void NativeUI::init(int argc, char **argv) {
    // Qt reads argc as a reference and expects it to be valid for the lifetime of the application
    __fb_qApplication_argc = argc;
    __fb_qApplication = std::make_unique<QApplication>(__fb_qApplication_argc, argv);
}

void NativeUI::deinit() {
    __fb_qApplication.release();
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