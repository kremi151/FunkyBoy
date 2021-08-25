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

#ifndef FB_CORE_CONTROLLERS_AUDIO_H
#define FB_CORE_CONTROLLERS_AUDIO_H

#include <memory>

#define FB_AUDIO_BUFFER_SIZE 4096

namespace FunkyBoy {

    typedef struct {
        float buffer[FB_AUDIO_BUFFER_SIZE];
        size_t bufferPosition;
    } AudioBuffer;

}

namespace FunkyBoy::Controller {

    class AudioController {
    public:
        virtual ~AudioController() = default;

        virtual void bufferCallback(const AudioBuffer *bufferPtr) = 0;
    };

    typedef std::shared_ptr<AudioController> AudioControllerPtr;

}

#endif //FB_CORE_CONTROLLERS_AUDIO_H
