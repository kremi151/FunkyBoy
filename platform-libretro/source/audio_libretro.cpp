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

#include "audio_libretro.h"

#define FB_MAX_AMPLITUDE 32767

using namespace FunkyBoy::Controller;

AudioControllerLibretro::AudioControllerLibretro()
    : audio_cb(nullptr)
{
}

AudioControllerLibretro::~AudioControllerLibretro() = default;

void AudioControllerLibretro::pushSample(float left, float right) {
    if (audio_cb != nullptr) {
        audio_cb(left * FB_MAX_AMPLITUDE, right * FB_MAX_AMPLITUDE);
    }
}

void AudioControllerLibretro::setAudioCallback(retro_audio_sample_t cb) {
    this->audio_cb = cb;
}
