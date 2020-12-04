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

#include "jumps.h"

#include <util/endianness.h>
#include <util/debug.h>

using namespace FunkyBoy;

bool Operands::rst(InstrContext &context, Memory &memory) {
    u8 rstAddr = (context.instr >> 3u & 7u) * 8u;
    debug_print_4("rst %02XH\n", rstAddr);
    context.push16Bits(memory, context.progCounter);
    context.progCounter = rstAddr;
    return true;
}