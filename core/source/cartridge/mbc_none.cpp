/**
 * Copyright 2020 Michel Kremer (kremi151)
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

#include "mbc_none.h"

using namespace FunkyBoy;

u8 MBCNone::readFromROMAt(memory_address offset, u8 *rom) {
    return *(rom + offset);
}

void MBCNone::interceptROMWrite(memory_address, FunkyBoy::u8) {
    // Do nothing
}

u8 MBCNone::readFromRAMAt(memory_address offset, u8 *ram) {
    return *(ram + offset);
}

void MBCNone::writeToRAMAt(memory_address offset, u8 val, u8 *ram) {
    *(ram + offset) = val;
}