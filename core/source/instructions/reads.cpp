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

#include "reads.h"

#include <util/registers.h>

using namespace FunkyBoy;

bool Operands::readLSB(InstrContext &context) {
    context.lsb = context.memory->read8BitsAt(context.progCounter++);
    return true;
}

bool Operands::readMSB(InstrContext &context) {
    context.msb = context.memory->read8BitsAt(context.progCounter++);
    return true;
}

bool Operands::readSigned(InstrContext &context) {
    context.signedByte = context.memory->readSigned8BitsAt(context.progCounter++);
    return true;
}

bool Operands::readRegCAsLSB(InstrContext &context) {
    context.lsb = *context.regC;
    context.msb = 0xFF;
    return true;
}

bool Operands::readMemAsLSB(InstrContext &context) {
    context.lsb = context.memory->read8BitsAt(context.progCounter++);
    context.msb = 0xFF;
    return true;
}

bool Operands::readRRLSBIntoStack(InstrContext &context) {
    u8 *reg = Util::decodeRRAddressFromOpcode(context.registers, context.instr) + 1;
    context.stackPointer--;
    context.memory->write8BitsTo(context.stackPointer, *reg);
    return true;
}

bool Operands::readRRMSBIntoStack(InstrContext &context) {
    u8 *reg = Util::decodeRRAddressFromOpcode(context.registers, context.instr);
    context.stackPointer--;
    context.memory->write8BitsTo(context.stackPointer, *reg);
    return true;
}

bool Operands::readStackIntoLSB(InstrContext &context) {
    context.lsb = context.memory->read8BitsAt(context.stackPointer++);
    return true;
}

bool Operands::readStackIntoMSB(InstrContext &context) {
    context.msb = context.memory->read8BitsAt(context.stackPointer++);
    return true;
}

bool Operands::readRegAIntoStack(InstrContext &context) {
    context.stackPointer--;
    context.memory->write8BitsTo(context.stackPointer, *context.regA);
    return true;
}

bool Operands::readRegFIntoStack(InstrContext &context) {
    context.stackPointer--;
    context.memory->write8BitsTo(context.stackPointer, *context.regF);
    return true;
}

bool Operands::readHLMem(InstrContext &context) {
    context.lsb = context.memory->read8BitsAt(context.readHL());
    return true;
}
