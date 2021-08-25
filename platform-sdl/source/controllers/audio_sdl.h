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

#ifndef FB_SDL_CONTROLLERS_AUDIO_SDL_H
#define FB_SDL_CONTROLLERS_AUDIO_SDL_H

#include <controllers/audio.h>
#include <SDL.h>

namespace FunkyBoy::Controller {

    class AudioControllerSDL: public AudioController {
    private:
        SDL_AudioSpec wanted{};
        SDL_AudioSpec obtained{};
        SDL_AudioDeviceID deviceId;
    public:
        const AudioBuffer *lastBuffer{};

        explicit AudioControllerSDL();
        ~AudioControllerSDL() override;

        void bufferCallback(const AudioBuffer *bufferPtr) override;
    };

}

#endif //FB_SDL_CONTROLLERS_AUDIO_SDL_H
