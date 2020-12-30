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

#ifndef FB_CORE_OPERANDS_DEBUG_H
#define FB_CORE_OPERANDS_DEBUG_H

// Uncomment to enable printing current opcode and register values to a file
// #define FB_DEBUG_WRITE_EXECUTION_LOG

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG

#include <iostream>
#include <iomanip>
#include <fstream>

namespace FunkyBoy {
    class InstrContext;
    class Memory;
}

namespace FunkyBoy::Debug {
    void writeExecutionToLog(uint8_t discriminator, std::ofstream &file, FunkyBoy::InstrContext &instrContext, FunkyBoy::Memory &memory);
    void writeInterruptToLog(uint16_t interrupt, std::ofstream &file);
}

#endif

#endif //FB_CORE_OPERANDS_DEBUG_H
