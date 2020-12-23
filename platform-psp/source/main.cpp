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

#include <pspkernel.h>
#include <pspdisplay.h>
#include <util/typedefs.h>
#include <util/debug.h>
#include <emulator/emulator.h>
#include <controllers/display_psp.h>
#include "callback.h"
#include "user_input.h"

PSP_MODULE_INFO(FB_NAME, 0, FB_VERSION_MAJOR, FB_VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

#define FB_PSP_ROM_PATH "ms0:/funkyboy/game.gb"

using namespace FunkyBoyPSP;

int pressXToExit() {
    pspDebugScreenPrintf("Press X to exit\n");
    while (isRunning()) {
        Input::poll();
        if (Input::getX()) {
            break;
        }
    }
    sceKernelExitGame();
    return 0;
}

int main() {
    setupExitCallback();

    sceDisplayWaitVblankStart();
    pspDebugScreenInit();
    pspDebugScreenClear();
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("%s version %s\n", FB_NAME, FB_VERSION);

    // Emulator config
    auto displayController = std::make_shared<FunkyBoyPSP::Controller::DisplayController>();
    auto controllers = std::make_shared<FunkyBoy::Controller::Controllers>();
    controllers->setDisplay(displayController);
    //controllers->setJoypad(std::make_shared<FunkyBoy::Controller::JoypadController3DS>());

    pspDebugScreenPrintf("Loading game from %s...\n", FB_PSP_ROM_PATH);
    FunkyBoy::Emulator emulator(FunkyBoy::GameBoyType::GameBoyDMG);
    auto status = emulator.loadGame(FB_PSP_ROM_PATH);

    if (status == FunkyBoy::CartridgeStatus::Loaded) {
        pspDebugScreenPrintf("Loaded ROM at %s\n", FB_PSP_ROM_PATH);
        pspDebugScreenPrintf("ROM title: %s\n", emulator.getCartridge().getHeader()->title);
    } else {
        pspDebugScreenPrintf("Could not load ROM at %s (status=%d)\n", FB_PSP_ROM_PATH, status);
        return pressXToExit();
    }

    sceDisplaySetMode(0, FB_GB_DISPLAY_WIDTH, FB_GB_DISPLAY_HEIGHT);
    sceDisplaySetFrameBuf(
            static_cast<FunkyBoyPSP::Controller::DisplayController*>(displayController.get())->frameBuffer,
            FB_GB_DISPLAY_WIDTH,
            PspDisplayPixelFormats::PSP_DISPLAY_PIXEL_FORMAT_8888,
            PspDisplaySetBufSync::PSP_DISPLAY_SETBUF_NEXTFRAME
            );

    sceDisplayWaitVblankStart();

    FunkyBoy::ret_code retCode;
    while (isRunning()) {
        // TODO: Limit frame rate
        retCode = emulator.doTick();
        if (retCode & FB_RET_NEW_SCANLINE) {
            Input::poll();
        }
        if (retCode & FB_RET_NEW_FRAME) {
            sceDisplayWaitVblankStart();
        }
    }

    sceKernelExitGame();
    return 0;
}