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

#include "jumps.h"

#include <util/endianness.h>
#include <util/flags.h>
#include <util/registers.h>
#include <util/debug.h>

using namespace FunkyBoy;

void Instructions::jp_conditional_zero(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    bool zero = Flags::isZero(context.regF);
    if ((!set && !zero) || (set && zero)) {
        debug_print_4("jp (N)Z a16 from 0x%04X", context.progCounter - 1);
        context.progCounter = address;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::jp_conditional_carry(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    bool carry = Flags::isCarry(context.regF);
    if ((!set && !carry) || (set && carry)) {
        debug_print_4("jp (C)Z a16 from 0x%04X", context.progCounter - 1);
        context.progCounter = address;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::jp(InstrContext &context) {
    context.progCounter = Util::compose16Bits(context.lsb, context.msb);
}

void Instructions::jp_HL(InstrContext &context) {
    context.progCounter = context.readHL();
}

void Instructions::jr_conditional_zero(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    bool zero = Flags::isZero(context.regF);
    if ((!set && !zero) || (set && zero)) {
        debug_print_4("JR (N)Z from 0x%04X + %d", context.progCounter - 1, context.signedByte);
        context.progCounter += context.signedByte;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::jr_conditional_carry(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    bool carry = Flags::isCarry(context.regF);
    if ((!set && !carry) || (set && carry)) {
        debug_print_4("JR (N)C from 0x%04X + %d", context.progCounter - 1, context.signedByte);
        context.progCounter += context.signedByte;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::jr(InstrContext &context) {
    debug_print_4("JR (unconditional) from 0x%04X + %d", context.progCounter, context.signedByte);
    context.progCounter += context.signedByte;
    debug_print_4(" to 0x%04X\n", context.progCounter);
}

void Instructions::call_conditional_zero(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    bool zero = Flags::isZero(context.regF);
    if ((!set && !zero) || (set && zero)) {
        debug_print_4("call from 0x%04X", context.progCounter - 2);
        context.push16Bits(context.progCounter);
        context.progCounter = address;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::call_conditional_carry(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    bool carry = Flags::isCarry(context.regF);
    if ((!set && !carry) || (set && carry)) {
        debug_print_4("call from 0x%04X", context.progCounter - 2);
        context.push16Bits(context.progCounter);
        context.progCounter = address;
        debug_print_4(" to 0x%04X\n", context.progCounter);
    }
}

void Instructions::call(InstrContext &context) {
    memory_address address = Util::compose16Bits(context.lsb, context.msb);
    debug_print_4("call from 0x%04X\n", context.progCounter);
    context.push16Bits(context.progCounter);
    context.progCounter = address;
    debug_print_4(" to 0x%04X\n", context.progCounter);
}

void Instructions::ret_conditional_zero(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    bool zero = Flags::isZero(context.regF);
    if ((!set && !zero) || (set && zero)) {
        context.progCounter = context.pop16Bits();
    }
}

void Instructions::ret_conditional_carry(InstrContext &context) {
    bool set = context.instr & 0b00001000u;
    bool carry = Flags::isCarry(context.regF);
    if ((!set && !carry) || (set && carry)) {
        context.progCounter = context.pop16Bits();
    }
}

void Instructions::ret(InstrContext &context) {
    context.progCounter = context.pop16Bits();
}