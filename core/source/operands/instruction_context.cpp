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

#include "instruction_context.h"

#include <exception/read_exception.h>
#include <cstring>
#include <util/stream_utils.h>

using namespace FunkyBoy;

InstrContext::InstrContext(FunkyBoy::GameBoyType gbType)
    : gbType(gbType)
{
    regB = registers;
    regC = registers + 1;
    regD = registers + 2;
    regE = registers + 3;
    regH = registers + 4;
    regL = registers + 5;
    regF = registers + 6;
    regA = registers + 7;
}

void InstrContext::push16Bits(Memory &memory, u16 val) {
    stackPointer -= 2;
    memory.write8BitsTo(stackPointer, (u8) (val));
    memory.write8BitsTo(stackPointer + 1, (u8) (val >> 8u));
}

void InstrContext::push16Bits(Memory &memory, u8 inMsb, u8 inLsb) {
    stackPointer -= 2;
    memory.write8BitsTo(stackPointer, inLsb);
    memory.write8BitsTo(stackPointer + 1, inMsb);
}

u16 InstrContext::pop16Bits(Memory &memory) {
    u16 val = memory.read8BitsAt(stackPointer) | (memory.read8BitsAt(stackPointer + 1) << 8u);
    stackPointer += 2;
    return val;
}

void InstrContext::writeHL(u16 val) {
    *regL = val & 0xffu;
    *regH = (val >> 8u) & 0xffu;
}

void InstrContext::write16BitRegister(u8 position, u16 val) {
    u8 *reg = registers + (position * 2);
    *reg = (val >> 8u) & 0xffu;
    *(reg + 1) = val & 0xffu;
}

u16 InstrContext::read16BitRegister(u8 position) {
    u8 *reg = registers + (position * 2);
    return (*reg << 8u) | (*(reg + 1u) & 0xffu);
}

size_t InstrContext::serializationSize() {
    return 8           // instr + cbInstr + lsb + msb + signedByte + cpuState + interruptMasterEnable + haltBugRequested
            + 8        // registers
            + (2 * 2); // progCounter + stackPointer
}

void InstrContext::serialize(std::ostream &ostream) const {
    ostream.put(instr);
    ostream.put(cbInstr);
    ostream.write(reinterpret_cast<const char*>(registers), 8);
    ostream.put(lsb);
    ostream.put(msb);
    ostream.put(signedByte);
    ostream.put(cpuState);
    ostream.put(interruptMasterEnable);
    ostream.put(haltBugRequested);
    Util::Stream::write16Bits(progCounter, ostream);
    Util::Stream::write16Bits(stackPointer, ostream);
}

void InstrContext::deserialize(std::istream &istream) {
    char buffer[16];
    istream.read(buffer, sizeof(buffer));
    if (!istream) {
        throw Exception::ReadException("Stream is too short (Instruction context)");
    }
    instr = buffer[0];
    cbInstr = buffer[1];
    std::memcpy(registers, buffer + 2, 8);
    lsb = buffer[10];
    msb = buffer[11];
    signedByte = buffer[12];
    cpuState = static_cast<CPUState>(buffer[13]);
    interruptMasterEnable = static_cast<IMEState>(buffer[14]);
    haltBugRequested = buffer[15] != 0;
    progCounter = Util::Stream::read16Bits(istream);
    stackPointer = Util::Stream::read16Bits(istream);
}