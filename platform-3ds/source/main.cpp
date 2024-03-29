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

#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <memory>
#include <fstream>
#include <cstring>

#include <emulator/emulator.h>
#include <controllers/display_3ds.h>
#include <util/fs.h>

#define FB_3DS_ROM_PATH "funkyboy3DS/game.gb"

extern "C" {

    void pressAToExit() {
        printf("Press A to exit\n");
        while (aptMainLoop()) {
            gspWaitForVBlank();
            gfxSwapBuffers();
            hidScanInput();

            u32 kDown = hidKeysDown();
            if (kDown & KEY_A) {
                gfxExit();
                return;
            }
        }
    }

    bool findSDCard() {
        bool isSdDetected;
        auto result = FSUSER_IsSdmcDetected(&isSdDetected);

        if (!isSdDetected) {
            printf("No SD card inserted\n");
            pressAToExit();
            return false;
        }
        printf("SD card found\n");
        return true;
    }

    bool findFile(FS_Archive archive, const char *path, const char *description) {
        printf("Checking for %s at %s ...\n", description, path);

        Handle fileHandle = 0;
        auto result = FSUSER_OpenFile(&fileHandle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ & 3, 0);
        if (!result) {
            printf("No %s found (result=%d)\n", description, result);
            pressAToExit();
            return false;
        }
        FSFILE_Close(fileHandle);
        printf("%s found at %s\n", description, path);
        return true;
    }

    void loadSave(FunkyBoy::Emulator &emulator, const FunkyBoy::fs::path &savePath) {
        if (!savePath.empty() && emulator.getCartridgeRamSize() > 0 && FunkyBoy::fs::exists(savePath)) {
            std::ifstream file(savePath);
            emulator.loadCartridgeRam(file);
        }
    }

    void writeSave(FunkyBoy::Emulator &emulator, const FunkyBoy::fs::path &savePath) {
        if (!savePath.empty() && emulator.getCartridgeRamSize() > 0) {
            std::ofstream file(savePath);
            emulator.writeCartridgeRam(file);
        }
    }

    int main(int argc, char* argv[])
    {
        gfxInit(GSP_RGBA8_OES, GSP_RGBA8_OES, false);
        consoleInit(GFX_BOTTOM, NULL);

        printf("########################\n");
        printf("# FunkyBoy by kremi151 #\n");
        printf("########################\n");
        printf("This is a proof of concept :)\n");
        printf("\n");

        bool isNew3DS;
        bool new3DSModeEnabled = true;
        bool requestedDisable3DSMode = false;
        APT_CheckNew3DS(&isNew3DS);
        osSetSpeedupEnable(true);
        if (!isNew3DS) {
            printf("Running on old 3DS, brace yourself for very slow emulation\n");
            new3DSModeEnabled = false;
        } else {
            std::cout << "Press Y to disable speed mode" << std::endl;
        }

        if (!findSDCard()) {
            return 0;
        }

        FunkyBoy::Emulator emulator(FunkyBoy::GameBoyType::GameBoyDMG);
        emulator.setControllers(FunkyBoy::Controller::Controllers()
            .withDisplay(std::make_shared<FunkyBoy::Controller::DisplayController3DS>()));

        printf("Launching game...\n");
        auto status = emulator.loadGame(FB_3DS_ROM_PATH);

        if (status == FunkyBoy::CartridgeStatus::Loaded) {
            printf("Loaded ROM at %s\n", FB_3DS_ROM_PATH);

            char romTitleSafe[FB_ROM_HEADER_TITLE_BYTES + 1]{};
            std::memcpy(romTitleSafe, emulator.getROMHeader()->title, FB_ROM_HEADER_TITLE_BYTES);
            printf("ROM title: %s\n", romTitleSafe);
        } else {
            std::cerr << "Could not load ROM at " << FB_3DS_ROM_PATH << " (status=" << status << ")" << std::endl;
            pressAToExit();
            return 0;
        }

        FunkyBoy::fs::path savePath = FB_3DS_ROM_PATH;
        savePath.replace_extension(".sav");
        loadSave(emulator, savePath);

        std::cout << "Press X to quit" << std::endl;

        gspWaitForVBlank();
        gfxSwapBuffers();

        FunkyBoy::u32_fast lastKeysDown = 0;
        FunkyBoy::u32_fast lastKeysHeld = 0;
        FunkyBoy::u32_fast currentKeysDown = hidKeysDown();
        FunkyBoy::u32_fast currentKeysHeld = hidKeysHeld();

        // Main loop
        while (aptMainLoop())
        {
            // TODO: Limit frame rate
            if (emulator.doTick() & FB_RET_NEW_FRAME) {
                hidScanInput();
                currentKeysDown = hidKeysDown();
                currentKeysHeld = hidKeysHeld();
                if (currentKeysHeld != lastKeysHeld || currentKeysDown != lastKeysDown) {
                    FunkyBoy::u32_fast kDown = currentKeysDown | currentKeysHeld;
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_A, kDown & KEY_A);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_B, kDown & KEY_B);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_START, kDown & KEY_START);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_SELECT, kDown & KEY_SELECT);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_UP, kDown & KEY_UP);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_DOWN, kDown & KEY_DOWN);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_LEFT, kDown & KEY_LEFT);
                    emulator.setInputState(FunkyBoy::Controller::JOYPAD_RIGHT, kDown & KEY_RIGHT);
                }
                lastKeysDown = currentKeysDown;
                lastKeysHeld = currentKeysHeld;
            }

            // Your code goes here
            if (currentKeysDown & KEY_X) {
                break; // break in order to return to hbmenu
            } else if (currentKeysDown & KEY_Y && isNew3DS) {
                if (!requestedDisable3DSMode) {
                    requestedDisable3DSMode = true;
                    new3DSModeEnabled = !new3DSModeEnabled;
                    osSetSpeedupEnable(new3DSModeEnabled);
                    if (new3DSModeEnabled) {
                        std::cout << "Press Y to disable speed mode" << std::endl;
                    } else {
                        std::cout << "Press Y to enable speed mode" << std::endl;
                    }
                }
            } else {
                requestedDisable3DSMode = false;
            }
        }

        writeSave(emulator, savePath);

        gfxExit();
        return 0;
    }

}
