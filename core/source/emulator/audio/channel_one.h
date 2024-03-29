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

#ifndef FB_CORE_EMULATOR_CHANNEL_ONE_H
#define FB_CORE_EMULATOR_CHANNEL_ONE_H

#include "channel_tone.h"

namespace FunkyBoy::Sound {

    typedef struct ChannelOneType : ToneChannel {
        bool sweepEnabled{};
        u8_fast sweepTimer{};
        u8_fast shadowFrequency{};

        void serialize(std::ostream &stream) const override;
        void deserialize(std::istream &stream) override;
    } ChannelOne;

}

#endif //FB_CORE_EMULATOR_CHANNEL_ONE_H
