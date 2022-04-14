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

#ifndef FB_CORE_OPERANDS_JUMPS_H
#define FB_CORE_OPERANDS_JUMPS_H

#ifndef FB_USE_SWITCH_FOR_INSTRUCTIONS

#include <operands/instruction_context.h>

namespace FunkyBoy::Operands {

    bool jp(InstrContext &context, Memory &memory);
    bool jp_HL(InstrContext &context, Memory &memory);

    bool jr(InstrContext &context, Memory &memory);

    bool call(InstrContext &context, Memory &memory);

    bool ret(InstrContext &context, Memory &memory);

    bool rst(InstrContext &context, Memory &memory);

}

#endif

#endif //FB_CORE_OPERANDS_JUMPS_H
