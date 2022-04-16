/**
 * Copyright 2022 Michel Kremer (kremi151)
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

#ifndef FB_CORE_OPERANDS_COMMON_H
#define FB_CORE_OPERANDS_COMMON_H

#include <util/flags.h>
#include <operands/instruction_context.h>

namespace FunkyBoy::Instructions {

    inline void adc(u8 *flags, u8 *regA, u8_fast val, bool carry) {
        u8_fast carryVal = carry ? 1 : 0;
        u8_fast newVal = *regA + val + carryVal;
        Flags::setFlags(flags, newVal == 0, false, ((*regA & 0xf) + (val & 0xf) + carryVal) > 0xf, (*regA & 0xff) + (val & 0xff) + carryVal > 0xff);
        *regA = newVal;
    }

    inline void addToHL(InstrContext &context, u16_fast val) {
        u16_fast oldVal = context.readHL();
        u16_fast newVal = oldVal + val;

        Flags::setFlags(context.regF, Flags::isZero(context.regF), false, ((oldVal & 0xfff) + (val & 0xfff)) > 0xfff, (oldVal & 0xffff) + (val & 0xffff) > 0xffff);

        context.writeHL(newVal);
    }

    // TODO: Check if it is safe to use *_fast types here
    inline void sbc(u8 *flags, u8 *regA, u8 val, bool carry) {
        u8 carryVal = carry ? 1 : 0;
        u8 newVal = *regA - val - carryVal;
        Flags::setFlags(flags, newVal == 0, true, (*regA & 0xf) - (val & 0xf) - carryVal < 0, *regA < (val + carryVal));
        *regA = newVal;
    }

    // TODO: Check if it is safe to use *_fast types here
    inline void cp(u8 *flags, const u8 *regA, u8 val) {
        // See http://z80-heaven.wikidot.com/instructions-set:cp
        Flags::setFlags(flags, *regA == val, true, (*regA & 0xf) - (val & 0xf) < 0, *regA < val);
    }

    inline void alu_or(u8 *flags, u8 *regA, u8_fast val) {
        *regA |= val;
        Flags::setFlags(flags, *regA == 0, false, false, false);
    }

    inline void alu_and(u8 *flags, u8 *regA, u8_fast val) {
        *regA &= val;
        //TODO: To be verified:
        Flags::setFlags(flags, *regA == 0, false, true, false);
    }

    inline void alu_xor(u8 *flags, u8 *regA, u8_fast val) {
        *regA ^= val;
        Flags::setFlags(flags, *regA == 0, false, false, false);
    }

}

#endif //FB_CORE_OPERANDS_COMMON_H
