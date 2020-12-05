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

#include "misc.h"

#include <util/flags.h>

using namespace FunkyBoy;

bool Operands::nop(InstrContext &context, Memory &memory) {
    // do nothing
    return true;
}

bool Operands::_pad_(InstrContext &context, Memory &memory) {
    // do nothing
    return true;
}

bool Operands::enableInterruptsImmediately(InstrContext &context, Memory &memory) {
    context.interruptMasterEnable = IMEState::ENABLED;
    return true;
}

bool Operands::enableInterruptsDelayed(InstrContext &context, Memory &memory) {
    context.interruptMasterEnable = IMEState::REQUEST_ENABLE;
    return true;
}

bool Operands::disableInterrupts(InstrContext &context, Memory &memory) {
    context.interruptMasterEnable = IMEState::DISABLED;
    return true;
}

bool Operands::stop(InstrContext &context, Memory &memory) {
    *context.regA = 0;
    *context.regB = memory.read8BitsAt(FB_REG_IE);
    memory.write8BitsTo(FB_REG_IE, 0);
    memory.write8BitsTo(FB_REG_P1, 0);
    context.cpuState = CPUState::STOPPED;
    return true;
}

bool Operands::halt(InstrContext &context, Memory &memory) {
    if (context.interruptMasterEnable == IMEState::ENABLED || (memory.read8BitsAt(FB_REG_IE) & memory.read8BitsAt(FB_REG_IF) & 0x1f) == 0) {
        context.cpuState = CPUState::HALTED;
    } else if (context.gbType != GameBoyType::GameBoyCGB) {
        // On non-GBC devices, the next instruction is skipped if HALT was requested with IME being disabled
        context.haltBugRequested = true;
    }
    return true;
}

bool Operands::cpl(InstrContext &context, Memory &memory) {
    *context.regA = ~*context.regA;
    Flags::setSubstraction(context.regF, true);
    Flags::setHalfCarry(context.regF, true);
    return true;
}

bool Operands::scf(InstrContext &context, Memory &memory) {
    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, false, true);
    return true;
}

bool Operands::ccf(InstrContext &context, Memory &memory) {
    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, false, !Flags::isCarry(context.regF));
    return true;
}
