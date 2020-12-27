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

#ifndef FB_CORE_OPERANDS_PREFIX_PREFIX_H
#define FB_CORE_OPERANDS_PREFIX_PREFIX_H

#include "instruction_context.h"

namespace FunkyBoy::Operands {

    /**
     * Decodes a prefix instruction
     * @param context
     * @return
     */
    const Operand *decodePrefix(u8 opcode);

}

#endif //FB_CORE_OPERANDS_PREFIX_PREFIX_H
