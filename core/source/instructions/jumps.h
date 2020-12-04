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

#ifndef FB_CORE_INSTRUCTIONS_JUMPS_H
#define FB_CORE_INSTRUCTIONS_JUMPS_H

#include <memory/memory.h>
#include "context.h"

namespace FunkyBoy::Instructions::Jumps {

    /**
     * JP (N)Z,a16
     */
    int jp_NZ_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * JP (N)C,a16
     */
    int jp_NC_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * JP a16
     */
    void jp_a16(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * JP HL
     */
     void jp_HL(Instructions::context &context);

    /**
     * JR (N)Z,r8
     */
    int jr_NZ_r8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * JR (N)C,r8
     */
    int jr_NC_r8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * JR r8
     */
    void jr_r8(FunkyBoy::Memory &memory, Instructions::context &context);

}

#endif //FB_CORE_INSTRUCTIONS_JUMPS_H
