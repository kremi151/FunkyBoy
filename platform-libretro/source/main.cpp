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

#include <cstdarg>
#include <cstring>

#include "display_libretro.h"
#include "joypad_libretro.h"

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
    static std::shared_ptr<Controller::JoypadController> joypadController;

    void retro_init(void) {
        displayController = std::make_shared<Controller::DisplayControllerLibretro>();
        joypadController = std::make_shared<Controller::JoypadControllerLibretro>();

        dynamic_cast<Controller::DisplayControllerLibretro&>(*displayController).setVideoCallback(video_cb);
        dynamic_cast<Controller::JoypadControllerLibretro&>(*joypadController).setInputCallback(input_state_cb, 0, 0);

        auto controllers = std::make_shared<Controller::Controllers>();
        controllers->setDisplay(displayController);
        controllers->setJoypad(joypadController);
        emulator = std::make_unique<Emulator>(GameBoyType::GameBoyDMG, controllers);
    }

    void retro_deinit(void) {
        auto ptr = emulator.release();
        delete ptr;

        displayController.reset();
        joypadController.reset();
    }

    unsigned retro_api_version(void) {
        return RETRO_API_VERSION;
    }

    void retro_set_controller_port_device(unsigned port, unsigned device) {
        log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);

        // TODO: Filter supported devices (like JOYPAD or KEYBOARD)
        //current_controller_port = port;
        //current_controller_device = device;
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

        info->timing.fps = 59.7154;

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
        if (joypadController) {
            dynamic_cast<Controller::JoypadControllerLibretro&>(*joypadController).setInputCallback(cb, 0, 0);
        }
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

    void retro_run(void) {
        if (emulator->doTick() & FB_RET_NEW_SCANLINE) {
            input_poll_cb();
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
                // Load save game
                emulator->loadCartridgeRamFromFS();
                return true;
            }
            default: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "An unexpected error occurred\n");
                return false;
            }
        }
    }

    void retro_unload_game(void) {
        // Write save game
        emulator->writeCartridgeRamToFS();

        auto ptr = emulator.release();
        delete ptr;
    }

    unsigned retro_get_region(void) {
        auto header = emulator->getCartridge().getHeader();
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