/**
 * Copyright 2021 Michel Kremer (kremi151)
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

#ifndef FB_LIBRETRO_AUDIO_LIBRETRO_H
#define FB_LIBRETRO_AUDIO_LIBRETRO_H

#include <controllers/audio.h>
#include <libretro.h>

namespace FunkyBoy::Controller {

    class AudioControllerLibretro: public AudioController {
    private:
        retro_audio_sample_t audio_cb;
    public:
        explicit AudioControllerLibretro();
        ~AudioControllerLibretro() override;

        void pushSample(float left, float right) override;

        void setAudioCallback(retro_audio_sample_t audio_cb);
    };

}

#endif //FB_LIBRETRO_AUDIO_LIBRETRO_H
