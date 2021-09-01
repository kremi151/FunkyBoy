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

using namespace FunkyBoy::Controller;

AudioControllerSDL::AudioControllerSDL() {
    wanted.freq = 48000; // TODO: Verify
    wanted.format = AUDIO_F32SYS;
    wanted.channels = 2;
    wanted.samples = FB_AUDIO_BUFFER_SIZE / sizeof(float) / 2; // TODO: Needed?
    wanted.userdata = this;
    fprintf(stdout, "Opening audio...\n");
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
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

void AudioControllerSDL::pushSample(float left, float right) {
    buffer[bufferPosition++] = left;
    buffer[bufferPosition++] = right;
    if (bufferPosition >= FB_AUDIO_BUFFER_SIZE) {
        SDL_QueueAudio(deviceId, static_cast<const void*>(buffer), bufferPosition * sizeof(float));
        bufferPosition = 0;
        while (SDL_GetQueuedAudioSize(deviceId) > FB_AUDIO_BUFFER_SIZE * sizeof(float)) {
            // Wait for Audio to be played to reduce latency
        }
    }
}
