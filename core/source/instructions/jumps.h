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

#include <instructions/instruction_context.h>

namespace FunkyBoy::Instructions {

    void jp_conditional_zero(InstrContext &context);
    void jp_conditional_carry(InstrContext &context);
    void jp(InstrContext &context);
    void jp_HL(InstrContext &context);

    void jr_conditional_zero(InstrContext &context);
    void jr_conditional_carry(InstrContext &context);
    void jr(InstrContext &context);

    void call_conditional_zero(InstrContext &context);
    void call_conditional_carry(InstrContext &context);
    void call(InstrContext &context);

    void ret_conditional_zero(InstrContext &context);
    void ret_conditional_carry(InstrContext &context);
    void ret(InstrContext &context);

}

#endif //FB_CORE_INSTRUCTIONS_JUMPS_H
