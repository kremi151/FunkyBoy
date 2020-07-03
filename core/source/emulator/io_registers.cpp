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

#include "io_registers.h"

using namespace FunkyBoy;

io_registers::io_registers(): div_lsb(0), div_msb(0) {
}

void io_registers::incrementDiv() {
    if (++div_lsb != 0) {
        return;
    }
    if (++div_msb == 0) {
        div_lsb = 0;
    }
}

void io_registers::resetDiv() {
    div_lsb = 0;
    div_msb = 0;
}
