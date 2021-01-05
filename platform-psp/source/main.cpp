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
#include <pspctrl.h>
#include <util/typedefs.h>
#include <util/debug.h>
#include <util/frame_executor.h>
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

void jeremyBearimy() {
    // There is a weird linker issue if I don't call the next functions explicitly
    // TODO: To be investigated why this happens
    struct timeval tp;
    sceKernelLibcTime(nullptr);
    sceKernelLibcGettimeofday(&tp, nullptr);
}

int main() {
    setupExitCallback();

    sceDisplayWaitVblankStart();
    pspDebugScreenInit();
    pspDebugScreenClear();
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("%s version %s\n", FB_NAME, FB_VERSION);

    jeremyBearimy();

    // Emulator config
    auto displayController = std::make_shared<FunkyBoyPSP::Controller::DisplayController>();
    auto controllers = std::make_shared<FunkyBoy::Controller::Controllers>();
    controllers->setDisplay(displayController);

    pspDebugScreenPrintf("Loading game from %s...\n", FB_PSP_ROM_PATH);
    FunkyBoy::Emulator emulator(FunkyBoy::GameBoyType::GameBoyDMG, controllers);
    auto status = emulator.loadGame(FB_PSP_ROM_PATH);

    if (status == FunkyBoy::CartridgeStatus::Loaded) {
        pspDebugScreenPrintf("Loaded ROM at %s\n", FB_PSP_ROM_PATH);
        pspDebugScreenPrintf("ROM title: %s\n", emulator.getROMHeader()->title);
    } else {
        pspDebugScreenPrintf("Could not load ROM at %s (status=%d)\n", FB_PSP_ROM_PATH, status);
        return pressXToExit();
    }

    u32 *vram = (u32*) (0x40000000 | 0x04000000);
    static_cast<FunkyBoyPSP::Controller::DisplayController*>(displayController.get())->frameBuffer = vram;

    sceDisplaySetMode(0, 480, 272);
    sceDisplaySetFrameBuf(
            (void*) vram,
            512,
            PspDisplayPixelFormats::PSP_DISPLAY_PIXEL_FORMAT_8888,
            PspDisplaySetBufSync::PSP_DISPLAY_SETBUF_NEXTFRAME
            );

    sceDisplayWaitVblankStart();
    pspDebugScreenPrintf("Starting game\n");

    unsigned int previousInput = 0;
    unsigned int currentInput;

    FunkyBoy::Util::FrameExecutor executeFrame([&emulator](){
        FunkyBoy::ret_code retCode;
        do {
            retCode = emulator.doTick();
        } while ((retCode & FB_RET_NEW_FRAME) == 0);
    }, FB_TARGET_FPS);

    while (isRunning()) {
        executeFrame();

        Input::poll();
        currentInput = Input::getPressedKeys();

        if (currentInput != previousInput) {
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_A, currentInput & PSP_CTRL_CIRCLE);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_B, currentInput & PSP_CTRL_CROSS);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_START, currentInput & PSP_CTRL_START);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_SELECT, currentInput & PSP_CTRL_SELECT);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_LEFT, currentInput & PSP_CTRL_LEFT);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_UP, currentInput & PSP_CTRL_UP);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_RIGHT, currentInput & PSP_CTRL_RIGHT);
            emulator.setInputState(FunkyBoy::Controller::JOYPAD_DOWN, currentInput & PSP_CTRL_DOWN);
            previousInput = currentInput;
        }
    }

    sceKernelExitGame();
    return 0;
}