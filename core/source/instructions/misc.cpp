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

bool Instructions::nop(InstrContext &context) {
    // do nothing
    return true;
}

bool Instructions::_pad_(InstrContext &context) {
    // do nothing
    return true;
}

bool Instructions::enableInterruptsImmediately(InstrContext &context) {
    context.interruptMasterEnable = IMEState::ENABLED;
    return true;
}

bool Instructions::enableInterruptsDelayed(InstrContext &context) {
    context.interruptMasterEnable = IMEState::REQUEST_ENABLE;
    return true;
}

bool Instructions::disableInterrupts(InstrContext &context) {
    context.interruptMasterEnable = IMEState::DISABLED;
    return true;
}

bool Instructions::stop(InstrContext &context) {
    context.cpuState = CPUState::STOPPED;
    return true;
}

bool Instructions::halt(InstrContext &context) {
    if (context.interruptMasterEnable == IMEState::ENABLED) {
        context.cpuState = CPUState::HALTED;
    } else if (context.gbType == GameBoyType::GameBoyCGB) {
        // On non-GBC devices, the next instruction is skipped if HALT was requested with IME being disabled
        context.progCounter++;
    }
    return true;
}

bool Instructions::daa(InstrContext &context) {
    u8 val = *context.regA;
    if (Flags::isSubstraction(context.regF)) {
        if (Flags::isCarry(context.regF)) {
            val -= 0x60;
        }
        if (Flags::isHalfCarry(context.regF)) {
            val -= 0x06;
        }
    } else {
        if (Flags::isCarry(context.regF) || val > 0x99) {
            val += 0x60;
            Flags::setCarry(context.regF, true);
        }
        if (Flags::isHalfCarry(context.regF) || (val & 0xf) > 0x09) {
            val += 0x06;
        }
    }
    *context.regA = val;
    Flags::setZero(context.regF, val == 0);
    Flags::setHalfCarry(context.regF, false);
    return true;
}

bool Instructions::cpl(InstrContext &context) {
    *context.regA = ~*context.regA;
    Flags::setSubstraction(context.regF, true);
    Flags::setHalfCarry(context.regF, true);
    return true;
}

bool Instructions::scf(InstrContext &context) {
    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, false, true);
    return true;
}

bool Instructions::ccf(InstrContext &context) {
    Flags::setFlags(context.regF, Flags::isZero(context.regF), false, false, !Flags::isCarry(context.regF));
    return true;
}
