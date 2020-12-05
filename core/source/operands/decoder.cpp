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

#include "decoder.h"

#include <util/debug.h>

using namespace FunkyBoy;

bool Operands::decodeOpcode(u8 opcode, Operands::operand_buffer operands) {
    switch (opcode) {
        // pop rr
        case 0xC1: case 0xD1: case 0xE1: {
            debug_print_4("pop rr\n");
            operands[0] = Operands::readStackIntoLSB;
            operands[1] = Operands::readStackIntoMSB;
            operands[2] = Operands::write16BitsIntoRR;
            operands[3] = nullptr;
            return true;
        }
        // pop AF
        case 0xF1: {
            debug_print_4("pop AF\n");
            operands[0] = Operands::readStackIntoLSB;
            operands[1] = Operands::readStackIntoMSB;
            operands[2] = Operands::write16BitsIntoAF;
            operands[3] = nullptr;
            return true;
        }
        // push rr
        case 0xC5: case 0xD5: case 0xE5: {
            debug_print_4("push rr\n");
            operands[0] = Operands::_pad_;
            operands[1] = Operands::readRRMSBIntoStack;
            operands[2] = Operands::readRRLSBIntoStack;
            operands[3] = Operands::_pad_;
            operands[4] = nullptr;
            return true;
        }
        // push AF
        case 0xF5: {
            debug_print_4("push AF\n");
            operands[0] = Operands::_pad_;
            operands[1] = Operands::readRegAIntoStack;
            operands[2] = Operands::readRegFIntoStack;
            operands[3] = Operands::_pad_;
            operands[4] = nullptr;
            return true;
        }
        // daa
        case 0x27: {
            debug_print_4("daa\n");
            operands[0] = Operands::daa;
            operands[1] = nullptr;
            return true;
        }
        // cpl
        case 0x2F: {
            debug_print_4("cpl\n");
            operands[0] = Operands::cpl;
            operands[1] = nullptr;
            return true;
        }
        // scf
        case 0x37: {
            debug_print_4("scf\n");
            operands[0] = Operands::scf;
            operands[1] = nullptr;
            return true;
        }
        // ccf
        case 0x3F: {
            debug_print_4("ccf\n");
            operands[0] = Operands::ccf;
            operands[1] = nullptr;
            return true;
        }
        // di
        case 0xF3: {
            debug_print_4("di\n");
            operands[0] = Operands::disableInterrupts;
            operands[1] = nullptr;
            return true;
        }
        // ei
        case 0xFB: {
            debug_print_4("ei\n");
            operands[0] = Operands::enableInterruptsDelayed;
            operands[1] = nullptr;
            return true;
        }
        // stop
        case 0x10: {
            debug_print_4("stop\n");
            operands[0] = Operands::stop;
            operands[1] = nullptr;
            return true;
        }
        // halt
        case 0x76: {
            debug_print_4("halt\n");
            operands[0] = Operands::halt;
            operands[1] = nullptr;
            return true;
        }
        // prefix
        case 0xCB: {
            debug_print_4("prefix (CB)\n");
            operands[0] = Operands::decodePrefix;
            // Small workaround:
            // To avoid our fetch/execution overlap implementation to override the prefix operands fetched in
            // Operands::decodePrefix, we add a dummy operand. Like this when we check if the next operand would
            // have been null, we do not directly replace the prefix operands. We only do it once they have been
            // executed.
            operands[1] = Operands::nop;
            operands[2] = nullptr;
            return true;
        }
        default: {
            return false;
        }
    }
}
