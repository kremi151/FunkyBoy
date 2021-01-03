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

#include <libretro.h>
#include <emulator/emulator.h>
#include <controllers/display.h>
#include <util/frame_executor.h>

#include <cstdarg>
#include <cstring>
#include <fstream>

#include "display_libretro.h"

using namespace FunkyBoy;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

extern "C" {

    static retro_video_refresh_t video_cb;
    static retro_audio_sample_t audio_cb;
    static retro_audio_sample_batch_t audio_batch_cb;
    static retro_environment_t environ_cb;
    static retro_input_poll_t input_poll_cb;
    static retro_input_state_t input_state_cb;

    static struct retro_log_callback logging;
    static retro_log_printf_t log_cb;

    static std::unique_ptr<Emulator> emulator;
    static std::shared_ptr<Controller::DisplayController> displayController;

    static fs::path savePath;
    static FunkyBoy::Util::FrameExecutor executeFrame(nullptr, 1.0);

    static unsigned currentControllerDevice;
    static unsigned currentControllerPort;

    static bool btnAWasPressed = false;
    static bool btnBWasPressed = false;
    static bool btnSelectWasPressed = false;
    static bool btnStartWasPressed = false;
    static bool btnUpWasPressed = false;
    static bool btnDownWasPressed = false;
    static bool btnLeftWasPressed = false;
    static bool btnRightWasPressed = false;

    void update_inputs();

    void retro_init(void) {
        currentControllerDevice = RETRO_DEVICE_JOYPAD;
        currentControllerPort = 0;

        displayController = std::make_shared<Controller::DisplayControllerLibretro>();

        dynamic_cast<Controller::DisplayControllerLibretro&>(*displayController).setVideoCallback(video_cb);

        auto controllers = std::make_shared<Controller::Controllers>();
        controllers->setDisplay(displayController);
        emulator = std::make_unique<Emulator>(GameBoyType::GameBoyDMG, controllers);

        executeFrame = FunkyBoy::Util::FrameExecutor([&]() {
            ret_code result;
            do {
                result = emulator->doTick();
            } while (!(result & FB_RET_NEW_FRAME));
            update_inputs();
        }, FB_TARGET_FPS);
    }

    void retro_deinit(void) {
        auto ptr = emulator.release();
        delete ptr;

        displayController.reset();
    }

    unsigned retro_api_version(void) {
        return RETRO_API_VERSION;
    }

    void retro_set_controller_port_device(unsigned port, unsigned device) {
        log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);

        currentControllerPort = port;
        currentControllerDevice = device;
    }

    void retro_get_system_info(struct retro_system_info *info) {
        memset(info, 0, sizeof(*info));
        info->library_name = FB_NAME;
        info->library_version = FB_VERSION;
        info->need_fullpath = true;
        info->valid_extensions = "bin|gb";
    }

    void retro_get_system_av_info(struct retro_system_av_info *info) {
        float aspect = (float) FB_GB_DISPLAY_WIDTH / (float) FB_GB_DISPLAY_HEIGHT;

        info->timing.fps = FB_TARGET_FPS;
        info->timing.sample_rate = 0.0;

        // TODO: Implement sound
        // info->timing.sample_rate = sampling_rate;

        info->geometry.base_width = FB_GB_DISPLAY_WIDTH;
        info->geometry.base_height = FB_GB_DISPLAY_HEIGHT;
        info->geometry.max_width = FB_GB_DISPLAY_WIDTH;
        info->geometry.max_height = FB_GB_DISPLAY_HEIGHT;
        info->geometry.aspect_ratio = aspect;
    }

    void retro_set_environment(retro_environment_t cb) {
        environ_cb = cb;

        if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging)) {
            log_cb = logging.log;
        } else {
            log_cb = fallback_log;
        }
    }

    void retro_set_audio_sample(retro_audio_sample_t cb) {
        audio_cb = cb;
    }

    void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
        audio_batch_cb = cb;
    }

    void retro_set_input_poll(retro_input_poll_t cb) {
        input_poll_cb = cb;
    }

    void retro_set_input_state(retro_input_state_t cb) {
        input_state_cb = cb;
    }

    void retro_set_video_refresh(retro_video_refresh_t cb) {
        video_cb = cb;
        if (displayController) {
            dynamic_cast<Controller::DisplayControllerLibretro&>(*displayController).setVideoCallback(cb);
        }
    }

    void retro_reset(void) {
        // TODO: Reset
    }

    void render_audio(retro_audio_sample_batch_t cb) {
        // TODO: Implement sound
    }

#define IS_PRESSED(key) input_state_cb(currentControllerPort, currentControllerDevice, 0, key)

    void update_inputs() {
        input_poll_cb();
        bool pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_A);
        if (pressed != btnAWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_A, pressed);
            btnAWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_B);
        if (pressed != btnBWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_B, pressed);
            btnBWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_START);
        if (pressed != btnStartWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_START, pressed);
            btnStartWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_SELECT);
        if (pressed != btnSelectWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_SELECT, pressed);
            btnSelectWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_LEFT);
        if (pressed != btnLeftWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_LEFT, pressed);
            btnLeftWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_UP);
        if (pressed != btnUpWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_UP, pressed);
            btnUpWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_RIGHT);
        if (pressed != btnRightWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_RIGHT, pressed);
            btnRightWasPressed = pressed;
        }
        pressed = IS_PRESSED(RETRO_DEVICE_ID_JOYPAD_DOWN);
        if (pressed != btnDownWasPressed) {
            emulator->setInputState(Controller::JoypadKey::JOYPAD_DOWN, pressed);
            btnDownWasPressed = pressed;
        }
    }

#undef IS_PRESSED

    void retro_run(void) {
        executeFrame();
    }

    void fb_loadSave() {
        if (!savePath.empty() && emulator->getCartridgeRamSize() > 0 && fs::exists(savePath)) {
            std::ifstream file(savePath, std::ios::binary | std::ios::in);
            emulator->loadCartridgeRam(file);
        }
    }

    void fb_writeSave() {
        if (!savePath.empty() && emulator->getCartridgeRamSize() > 0) {
            std::ofstream file(savePath, std::ios::binary | std::ios::out);
            emulator->writeCartridgeRam(file);
        }
    }

    bool retro_load_game(const struct retro_game_info *info) {
        auto pixel_fmt = RETRO_PIXEL_FORMAT_XRGB8888;
        if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_fmt)) {
            log_cb(retro_log_level::RETRO_LOG_ERROR, "Required pixel format XRGB8888 is not supported\n");
            return false;
        }

        if (info == nullptr || info->path == nullptr || info->path[0] == '\0') {
            log_cb(retro_log_level::RETRO_LOG_ERROR, "No ROM path specified\n");
            return false;
        }

        auto status = emulator->loadGame(info->path);
        switch (status) {
            case CartridgeStatus::ROMFileNotReadable:
            case CartridgeStatus::ROMParseError: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "Cannot open ROM: %s\n", info->path);
                return false;
            }
            case CartridgeStatus::ROMTooBig:
            case CartridgeStatus::ROMSizeMismatch: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "The provided ROM file is larger than the allowed 32 MiB.\n");
                return false;
            }
            case CartridgeStatus::ROMUnsupportedMBC:
            case CartridgeStatus::RAMSizeUnsupported: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "The provided ROM is not supported.\n");
                return false;
            }
            case CartridgeStatus::Loaded: {
                savePath = info->path;
                savePath.replace_extension(".sav");
                fb_loadSave();
                return true;
            }
            default: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "An unexpected error occurred\n");
                return false;
            }
        }
    }

    void retro_unload_game(void) {
        fb_writeSave();
        savePath.clear();

        auto ptr = emulator.release();
        delete ptr;
    }

    unsigned retro_get_region(void) {
        auto header = emulator->getROMHeader();
        return header->destinationCode ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
    }

    bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) {
        //TODO
        return retro_load_game(info);
    }

    size_t retro_serialize_size(void) {
        //TODO
        return 0;
    }

    bool retro_serialize(void *data_, size_t size) {
        //TODO
        return true;
    }

    bool retro_unserialize(const void *data_, size_t size) {
        //TODO
        return true;
    }

    void *retro_get_memory_data(unsigned id) {
        //TODO
        (void) id;
        return nullptr;
    }

    size_t retro_get_memory_size(unsigned id) {
        //TODO
        (void) id;
        return 0;
    }

    void retro_cheat_reset(void) {
        //TODO
        // Nothing.
    }

    void retro_cheat_set(unsigned index, bool enabled, const char *code) {
        //TODO
        (void) index;
        (void) enabled;
        (void) code;
    }

}