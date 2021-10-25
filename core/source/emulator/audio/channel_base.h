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

#ifndef FB_CORE_EMULATOR_CHANNEL_BASE_H
#define FB_CORE_EMULATOR_CHANNEL_BASE_H

#include <iostream>
#include <util/typedefs.h>
#include <util/serialization.h>

namespace FunkyBoy::Sound {

    typedef struct BaseChannelType {
        bool channelEnabled{};

        u16_fast lengthTimer{}; // 16 bits because channel 3 can go up to 256
        u16_fast freqTimer{};

        bool dacEnabled{};

        FB_DECLARE_SERIALIZATION_ESTIMATABLE()
        FB_DECLARE_SERIALIZATION_VIRTUAL()
    } BaseChannel;

}

#endif //FB_CORE_EMULATOR_CHANNEL_BASE_H
