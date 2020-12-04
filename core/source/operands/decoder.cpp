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
        // rst vec
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: {
            debug_print_4("rst vec\n");
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_;
            operands[1] = Operands::_pad_;
            operands[2] = Operands::_pad_;
            //
            operands[3] = Operands::rst;
            operands[4] = nullptr;
            return true;
        }
        // cp r
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
            debug_print_4("cp r\n");
            operands[0] = Operands::cp_r;
            operands[1] = nullptr;
            return true;
        }
        // cp (HL)
        case 0xBE: {
            debug_print_4("cp (HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::cp_HL;
            operands[2] = nullptr;
            return true;
        }
        // cp d8
        case 0xFE: {
            debug_print_4("cp d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::cp_d;
            operands[2] = nullptr;
            return true;
        }
        // inc ss
        case 0x03: case 0x13: case 0x23: {
            debug_print_4("inc ss\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::inc_ss;
            operands[2] = nullptr;
            return true;
        }
        // inc SP
        case 0x33: {
            debug_print_4("inc SP\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::inc_SP;
            operands[2] = nullptr;
            return true;
        }
        // inc (HL)
        case 0x34: {
            debug_print_4("inc (HL)\n");
            // Pad artificially to 3 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_;
            operands[1] = Operands::_pad_;
            //
            operands[2] = Operands::inc_HL;
            operands[3] = nullptr;
            return true;
        }
        // inc r
        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x3C: {
            debug_print_4("inc r\n");
            operands[0] = Operands::inc_r;
            operands[1] = nullptr;
            return true;
        }
        // dec ss
        case 0x0B: case 0x1B: case 0x2B: {
            debug_print_4("dec ss\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::dec_ss;
            operands[2] = nullptr;
            return true;
        }
        // dec SP
        case 0x3B: {
            debug_print_4("dec SP\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::dec_SP;
            operands[2] = nullptr;
            return true;
        }
        // dec (HL)
        case 0x35: {
            debug_print_4("dec (HL)\n");
            // Pad artificially to 3 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_;
            operands[1] = Operands::_pad_;
            //
            operands[2] = Operands::dec_HL;
            operands[3] = nullptr;
            return true;
        }
        // dec s
        case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: case 0x3D: {
            debug_print_4("dec r\n");
            operands[0] = Operands::dec_r;
            operands[1] = nullptr;
            return true;
        }
        // or r
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: {
            debug_print_4("or r\n");
            operands[0] = Operands::or_r;
            operands[1] = nullptr;
            return true;
        }
        // or (HL)
        case 0xB6: {
            debug_print_4("or (HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::or_HL;
            operands[2] = nullptr;
            return true;
        }
        // or d8
        case 0xF6: {
            debug_print_4("or d\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::or_d;
            operands[2] = nullptr;
            return true;
        }
        // and r
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: {
            debug_print_4("and r\n");
            operands[0] = Operands::and_r;
            operands[1] = nullptr;
            return true;
        }
        // and (HL)
        case 0xA6: {
            debug_print_4("and (HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::and_HL;
            operands[2] = nullptr;
            return true;
        }
        // and d8
        case 0xE6: {
            debug_print_4("and d\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::and_d;
            operands[2] = nullptr;
            return true;
        }
        // xor r
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: {
            debug_print_4("xor r\n");
            operands[0] = Operands::xor_r;
            operands[1] = nullptr;
            return true;
        }
        // xor (HL)
        case 0xAE: {
            debug_print_4("xor (HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::xor_HL;
            operands[2] = nullptr;
            return true;
        }
        // xor d8
        case 0xEE: {
            debug_print_4("xor d\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::xor_d;
            operands[2] = nullptr;
            return true;
        }
        // rrca
        case 0x0F: {
            debug_print_4("rrca\n");
            operands[0] = Operands::rrca;
            operands[1] = nullptr;
            return true;
        }
        // rlca
        case 0x07: {
            debug_print_4("rlca\n");
            operands[0] = Operands::rlca;
            operands[1] = nullptr;
            return true;
        }
        // rra
        case 0x1F: {
            debug_print_4("rra\n");
            operands[0] = Operands::rra;
            operands[1] = nullptr;
            return true;
        }
        // rla
        case 0x17: {
            debug_print_4("rla\n");
            operands[0] = Operands::rla;
            operands[1] = nullptr;
            return true;
        }
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
