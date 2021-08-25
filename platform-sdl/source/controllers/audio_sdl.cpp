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

#include "audio_sdl.h"

#include <string>
#include <exception/state_exception.h>

namespace FunkyBoySDL::Controller::SoundInternal {

    void fill_audio(void *udata, Uint8 *stream, int len) {
        // FunkyBoy::Controller::AudioControllerSDL &audio = *static_cast<FunkyBoy::Controller::AudioControllerSDL*>(udata);
        if (udata == nullptr) {
            return;
        }
        auto audio = static_cast<FunkyBoy::Controller::AudioControllerSDL*>(udata);
        if (audio->lastBuffer != nullptr) {
            const auto &bufferObj = *audio->lastBuffer;
            auto buffer = static_cast<const Uint8*>(static_cast<const void*>(bufferObj.buffer));
            //fprintf(stdout, "# fill_audio A %f %f\n", audio->lastBuffer[0], audio->lastBuffer[1]);
            fprintf(stdout, "# fill_audio A %d %d\n", buffer[0], buffer[1]);
            memcpy(stream, buffer, len);
        }
    }

}

using namespace FunkyBoy::Controller;

AudioControllerSDL::AudioControllerSDL() {
    wanted.freq = 48000; // TODO: Verify
    wanted.format = AUDIO_F32SYS;
    wanted.channels = 2;
    wanted.samples = FB_AUDIO_BUFFER_SIZE / 2;
    wanted.callback = FunkyBoySDL::Controller::SoundInternal::fill_audio;
    wanted.userdata = this;
    fprintf(stdout, "Opening audio...\n");
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
    if (deviceId <= 0) {
        fprintf(stderr, "Opening audio failed: %s\n", SDL_GetError());
        throw FunkyBoy::Exception::WrongStateException(std::string("Could not open audio: ") + SDL_GetError());
    }
    fprintf(stdout, "Audio opened on device %d\n", deviceId);
    SDL_PauseAudioDevice(deviceId, 0);
}

AudioControllerSDL::~AudioControllerSDL() {
    SDL_CloseAudioDevice(deviceId);
}

// TODO: Refactor this by having an open method that provides a callback function pointer
void AudioControllerSDL::bufferCallback(const AudioBuffer *bufferPtr) {
    this->lastBuffer = bufferPtr;
    obtained.samples = bufferPtr->bufferPosition / 2;
}