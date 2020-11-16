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
        // ld (a16),A
        case 0xEA: {
            debug_print_4("ld (a16),A\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = Operands::_pad_; // TODO: do something useful here
            operands[3] = Operands::load_mem_dd_A;
            operands[4] = nullptr;
            return true;
        }
        // ld A,(a16)
        case 0xFA: {
            debug_print_4("ld A,(a16)\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = Operands::_pad_; // TODO: do something useful here
            operands[3] = Operands::load_A_mem_dd;
            operands[4] = nullptr;
            return true;
        }
        // ld (C),A
        case 0xE2: {
            debug_print_4("ld (C),A\n");
            operands[0] = Operands::readRegCAsLSB;
            operands[1] = Operands::load_mem_dd_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(C)
        case 0xF2: {
            debug_print_4("ld A,(C)\n");
            operands[0] = Operands::readRegCAsLSB;
            operands[1] = Operands::load_A_mem_dd;
            operands[2] = nullptr;
            return true;
        }
        // ld A,d8
        case 0x3E: {
            debug_print_4("ld A,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::load_A_d;
            operands[2] = nullptr;
            return true;
        }
        // ld (ss),d16
        case 0x01: case 0x11: case 0x21: {
            debug_print_4("ld (ss),d16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = Operands::load_dd_nn;
            operands[3] = nullptr;
            return true;
        }
        // ld SP,d16
        case 0x31: {
            debug_print_4("ld SP,d16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = Operands::load_SP_nn;
            operands[3] = nullptr;
            return true;
        }
        // ld (a16),SP
        case 0x08: {
            debug_print_4("ld (a16),SP\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[2] = Operands::_pad_;
            operands[3] = Operands::_pad_;
            //
            operands[4] = Operands::load_nn_SP;
            operands[5] = nullptr;
            return true;
        }
        // ld r,d8
        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: {
            debug_print_4("ld r,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::load_r_n;
            operands[2] = nullptr;
            return true;
        }
        // ld (HL),d8
        case 0x36: {
            debug_print_4("ld (HL),d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::_pad_; // TODO: do something useful here
            operands[2] = Operands::load_HL_n;
            operands[3] = nullptr;
            return true;
        }
        // ld (ss),A
        case 0x02: case 0x12: {
            debug_print_4("ld (ss),A\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_reg_dd_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(ss)
        case 0x0A: case 0x1A: {
            debug_print_4("ld A,(ss)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_A_reg_dd;
            operands[2] = nullptr;
            return true;
        }
        // ld (HLI),A
        case 0x22: {
            debug_print_4("ld (HLI),A\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_HLI_A;
            operands[2] = nullptr;
            return true;
        }
        // ld (HLD),A
        case 0x32: {
            debug_print_4("ld (HLD),A\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_HLD_A;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(HLI)
        case 0x2A: {
            debug_print_4("ld A,(HLI)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_A_HLI;
            operands[2] = nullptr;
            return true;
        }
        // ld A,(HLD)
        case 0x3A: {
            debug_print_4("ld A,(HLD)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_A_HLD;
            operands[2] = nullptr;
            return true;
        }
        // ld (HL),r
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: {
            debug_print_4("ld (HL),r\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_HL_r;
            operands[2] = nullptr;
            return true;
        }
        // ld r,(HL)
        case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E: {
            debug_print_4("ld r,(HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_r_HL;
            operands[2] = nullptr;
            return true;
        }
        // ld SP,HL
        case 0xF9: {
            debug_print_4("ld SP,HL\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::load_SP_HL;
            operands[2] = nullptr;
            return true;
        }
        // ld HL,SP+e8
        case 0xF8: {
            debug_print_4("ld HL,SP+e8\n");
            operands[0] = Operands::readSigned;
            operands[1] = Operands::_pad_; // TODO: do something useful here
            operands[2] = Operands::load_HL_SPe;
            operands[3] = nullptr;
            return true;
        }
        // ldh (a8),A
        case 0xE0: {
            debug_print_4("ldh (a8),A\n");
            operands[0] = Operands::readMemAsLSB;
            operands[1] = Operands::_pad_; // TODO: do something useful here
            operands[2] = Operands::load_mem_dd_A;
            operands[3] = nullptr;
            return true;
        }
        // ldh A,(a8)
        case 0xF0: {
            debug_print_4("ldh A,(a8)\n");
            operands[0] = Operands::readMemAsLSB;
            operands[1] = Operands::_pad_; // TODO: do something useful here
            operands[2] = Operands::load_A_mem_dd;
            operands[3] = nullptr;
            return true;
        }
        // add a,reg
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: {
            debug_print_4("add A,r\n");
            operands[0] = Operands::add_A_r;
            operands[1] = nullptr;
            return true;
        }
        // adc a,reg
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: {
            debug_print_4("adc A,r\n");
            operands[0] = Operands::adc_A_r;
            operands[1] = nullptr;
            return true;
        }
        // add A,d8
        case 0xC6: {
            debug_print_4("add A,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::add_A_d;
            operands[2] = nullptr;
            return true;
        }
        // adc A,d8
        case 0xCE: {
            debug_print_4("add A,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::adc_A_d;
            operands[2] = nullptr;
            return true;
        }
        // add HL,ss
        case 0x09: case 0x19: case 0x29: {
            debug_print_4("add HL,ss\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::add_HL_ss;
            operands[2] = nullptr;
            return true;
        }
        // add HL,SP
        case 0x39: {
            debug_print_4("add HL,SP\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::add_HL_SP;
            operands[2] = nullptr;
            return true;
        }
        // add SP,r8
        case 0xE8: {
            debug_print_4("add SP,e8\n");
            operands[0] = Operands::readSigned;
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[1] = Operands::_pad_;
            operands[2] = Operands::_pad_;
            //
            operands[3] = Operands::add_SP_e;
            operands[4] = nullptr;
            return true;
        }
        // add A,(HL)
        case 0x86: {
            debug_print_4("add A,(HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::add_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // adc A,(HL)
        case 0x8E: {
            debug_print_4("adc A,(HL)\n");
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::adc_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // sub a,reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: {
            debug_print_4("sub A,r\n");
            operands[0] = Operands::sub_A_r;
            operands[1] = nullptr;
            return true;
        }
        // sbc a,reg
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f: {
            debug_print_4("sbc A,r\n");
            operands[0] = Operands::sbc_A_r;
            operands[1] = nullptr;
            return true;
        }
        // sub A,d8
        case 0xD6: {
            debug_print_4("sub A,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::sub_A_d;
            operands[2] = nullptr;
            return true;
        }
        // sbc A,d8
        case 0xDE: {
            debug_print_4("sbc A,d8\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::sbc_A_d;
            operands[2] = nullptr;
            return true;
        }
        // sub (HL)
        case 0x96: {
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::sub_HL;
            operands[2] = nullptr;
            return true;
        }
        // sbc (HL)
        case 0x9E: {
            operands[0] = Operands::_pad_; // TODO: do something useful here
            operands[1] = Operands::sbc_A_HL;
            operands[2] = nullptr;
            return true;
        }
        // jp (N)Z,a16
        case 0xC2: case 0xCA: {
            debug_print_4("jp (N)Z,a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = (opcode == 0xC2) ? Operands::checkIsNotZero : Operands::checkIsZero;
            operands[3] = Operands::jp;
            operands[4] = nullptr;
            return true;
        }
        // jp (N)C,a16
        case 0xD2: case 0xDA: {
            debug_print_4("jp (N)C,a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = (opcode == 0xD2) ? Operands::checkIsNotCarry : Operands::checkIsCarry;
            operands[3] = Operands::jp;
            operands[4] = nullptr;
            return true;
        }
        // unconditional jp
        case 0xC3:
        {
            debug_print_4("jp a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = Operands::_pad_;
            operands[3] = Operands::jp;
            operands[4] = nullptr;
            return true;
        }
        // jp HL
        case 0xE9:
        {
            debug_print_4("jp HL\n");
            operands[0] = Operands::jp_HL;
            operands[1] = nullptr;
            return true;
        }
        // jr (N)Z,r8
        case 0x20: case 0x28: {
            debug_print_4("jr (N)Z,r8\n");
            operands[0] = Operands::readSigned;
            operands[1] = (opcode == 0x20) ? Operands::checkIsNotZero : Operands::checkIsZero;
            operands[2] = Operands::jr;
            operands[3] = nullptr;
            return true;
        }
        // jr (N)C,r8
        case 0x30: case 0x38: {
            debug_print_4("jr (N)C,r8\n");
            operands[0] = Operands::readSigned;
            operands[1] = (opcode == 0x30) ? Operands::checkIsNotCarry : Operands::checkIsCarry;
            operands[2] = Operands::jr;
            operands[3] = nullptr;
            return true;
        }
        // unconditional jr
        case 0x18: {
            debug_print_4("jr r8\n");
            operands[0] = Operands::readSigned;
            operands[1] = Operands::_pad_;
            operands[2] = Operands::jr;
            operands[3] = nullptr;
            return true;
        }
        // call (N)Z,a16
        case 0xC4: case 0xCC: {
            debug_print_4("call (N)Z,a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = (opcode == 0xC4) ? Operands::checkIsNotZero : Operands::checkIsZero;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[3] = Operands::_pad_;
            operands[4] = Operands::_pad_;
            //
            operands[5] = Operands::call;
            operands[6] = nullptr;
            return true;
        }
        // call (N)C,a16
        case 0xD4: case 0xDC: {
            debug_print_4("call (N)C,a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            operands[2] = (opcode == 0xD4) ? Operands::checkIsNotCarry : Operands::checkIsCarry;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[3] = Operands::_pad_;
            operands[4] = Operands::_pad_;
            //
            operands[5] = Operands::call;
            operands[6] = nullptr;
            return true;
        }
        // call a16
        case 0xCD: {
            debug_print_4("call a16\n");
            operands[0] = Operands::readLSB;
            operands[1] = Operands::readMSB;
            // Pad artificially to 6 machine cycles TODO: do something useful here
            operands[2] = Operands::_pad_;
            operands[3] = Operands::_pad_;
            operands[4] = Operands::_pad_;
            //
            operands[5] = Operands::call;
            operands[6] = nullptr;
            return true;
        }
        // ret (N)Z
        case 0xC0: case 0xC8: {
            debug_print_4("ret (N)Z\n");
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_; // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            operands[1] = (opcode == 0xC0) ? Operands::checkIsNotZero : Operands::checkIsZero;
            operands[2] = Operands::_pad_;
            operands[3] = Operands::_pad_;
            operands[4] = Operands::ret;
            operands[5] = nullptr;
            return true;
        }
        // ret (N)C
        case 0xD0: case 0xD8: {
            debug_print_4("ret (N)C\n");
            // Pad artificially to 5 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_; // This has to happen before the ret condition (unmet condition -> 2 M cycles)
            operands[1] = (opcode == 0xD0) ? Operands::checkIsNotCarry : Operands::checkIsCarry;
            operands[2] = Operands::_pad_;
            operands[3] = Operands::_pad_;
            operands[4] = Operands::ret;
            operands[5] = nullptr;
            return true;
        }
        // ret
        case 0xC9: {
            debug_print_4("ret\n");
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[0] = Operands::_pad_;
            operands[1] = Operands::_pad_;
            operands[2] = Operands::_pad_;
            //
            operands[3] = Operands::ret;
            operands[4] = nullptr;
            return true;
        }
        // reti
        case 0xD9: {
            debug_print_4("reti\n");
            operands[0] = Operands::enableInterruptsImmediately;
            // Pad artificially to 4 machine cycles TODO: do something useful here
            operands[1] = Operands::_pad_;
            operands[2] = Operands::_pad_;
            //
            operands[3] = Operands::ret;
            operands[4] = nullptr;
            return true;
        }
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
