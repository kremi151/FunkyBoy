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

#include "channel_tone.h"

using namespace FunkyBoy::Sound;

size_t ToneChannelType::serializationSize(bool full) const {
    return EnvelopeChannelType::serializationSize(full)
            + WaveChannelType::serializationSize(full);
}

void ToneChannelType::serialize(std::ostream &stream) const {
    EnvelopeChannelType::serialize(stream);
    WaveChannelType::serialize(stream);
}

void ToneChannelType::deserialize(std::istream &stream) {
    EnvelopeChannelType::deserialize(stream);
    WaveChannelType::deserialize(stream);
}
