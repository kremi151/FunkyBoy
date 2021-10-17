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

#include "channel_envelope.h"

using namespace FunkyBoy::Sound;

size_t EnvelopeChannelType::serializationSize(bool full) const {
    return BaseChannelType::serializationSize(full)
            + 2; // periodTimer + currentVolume;
}

void EnvelopeChannelType::serialize(std::ostream &stream) const {
    BaseChannelType::serialize(stream);

    stream.put(periodTimer);
    stream.put(currentVolume);
}

void EnvelopeChannelType::deserialize(std::istream &stream) {
    BaseChannelType::deserialize(stream);

    periodTimer = stream.get();
    currentVolume = stream.get();
}
