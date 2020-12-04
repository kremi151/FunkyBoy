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

#include <util/flags.h>
#include <util/debug.h>

using namespace FunkyBoy::Instructions;

#define __fb_doJumpExact(offset) \
debug_print_4("JP from 0x%04X", context.progCounter); \
context.progCounter = offset; \
debug_print_4(" to 0x%04X\n", context.progCounter)

#define __fb_doJumpRelative(offset) \
debug_print_4("JR from 0x%04X + %d", context.progCounter, offset); \
context.progCounter += offset; \
debug_print_4(" to 0x%04X\n", context.progCounter)

#define __fb_doCall(offset) \
debug_print_4("call from 0x%04X\n", context.progCounter); \
context.push16Bits(memory, context.progCounter); \
context.progCounter = offset; \
debug_print_4(" to 0x%04X\n", context.progCounter)

int Jumps::jp_NZ_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    if (opcode == 0xC2) {
        if (Flags::isZeroFast(*context.regF)) {
            return 12;
        }
    } else {
        if (!Flags::isZeroFast(*context.regF)) {
            return 12;
        }
    }
    __fb_doJumpExact(Util::compose16Bits(lsb, msb));
    return 16;
}

int Jumps::jp_NC_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    if (opcode == 0xD2) {
        if (Flags::isCarryFast(*context.regF)) {
            return 12;
        }
    } else {
        if (!Flags::isCarryFast(*context.regF)) {
            return 12;
        }
    }
    __fb_doJumpExact(Util::compose16Bits(lsb, msb));
    return 16;
}

void Jumps::jp_a16(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    __fb_doJumpExact(Util::compose16Bits(lsb, msb));
}

void Jumps::jp_HL(Instructions::context &context) {
    context.progCounter = context.readHL();
}

int Jumps::jr_NZ_r8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    i8_fast signedByte = memory.readSigned8BitsAt(context.progCounter++);
    if (opcode == 0x20) {
        if (Flags::isZeroFast(*context.regF)) {
            return 8;
        }
    } else {
        if (!Flags::isZeroFast(*context.regF)) {
            return 8;
        }
    }
    __fb_doJumpRelative(signedByte);
    return 12;
}

int Jumps::jr_NC_r8(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    i8_fast signedByte = memory.readSigned8BitsAt(context.progCounter++);
    if (opcode == 0x30) {
        if (Flags::isCarryFast(*context.regF)) {
            return 8;
        }
    } else {
        if (!Flags::isCarryFast(*context.regF)) {
            return 8;
        }
    }
    __fb_doJumpRelative(signedByte);
    return 12;
}

void Jumps::jr_r8(FunkyBoy::Memory &memory, Instructions::context &context) {
    i8_fast signedByte = memory.readSigned8BitsAt(context.progCounter++);
    __fb_doJumpRelative(signedByte);
}

int Jumps::call_NZ_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    if (opcode == 0xC4) {
        if (Flags::isZeroFast(*context.regF)) {
            return 12;
        }
    } else {
        if (!Flags::isZeroFast(*context.regF)) {
            return 12;
        }
    }
    memory_address address = Util::compose16Bits(lsb, msb);
    __fb_doCall(address);
    return 24;
}

int Jumps::call_NC_a16(opcode_t opcode, FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    if (opcode == 0xD4) {
        if (Flags::isCarryFast(*context.regF)) {
            return 12;
        }
    } else {
        if (!Flags::isCarryFast(*context.regF)) {
            return 12;
        }
    }
    memory_address address = Util::compose16Bits(lsb, msb);
    __fb_doCall(address);
    return 24;
}

void Jumps::call_a16(FunkyBoy::Memory &memory, Instructions::context &context) {
    u8_fast lsb = memory.read8BitsAt(context.progCounter++);
    u8_fast msb = memory.read8BitsAt(context.progCounter++);
    memory_address address = Util::compose16Bits(lsb, msb);
    __fb_doCall(address);
}