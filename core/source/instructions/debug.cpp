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

#include "debug.h"

#ifdef FB_DEBUG_WRITE_EXECUTION_LOG

#include <instructions/context.h>

using namespace FunkyBoy;

void Debug::writeExecutionToLog(uint8_t discriminator, std::ofstream &file, FunkyBoy::Instructions::context &context, opcode_t opcode) {
    file << discriminator << " ";
    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (opcode & 0xffu);
    file << " B=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regB & 0xffu);
    file << " C=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regC & 0xffu);
    file << " D=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regD & 0xffu);
    file << " E=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regE & 0xffu);
    file << " H=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regH & 0xffu);
    file << " L=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regL & 0xffu);
    file << " A=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regA & 0xffu);
    file << " F=0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (*context.regF & 0xffu);
    file << " PC=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (context.progCounter - 1);
    file << " SP=0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << context.stackPointer;
    file << std::endl;
}

void Debug::writeInterruptToLog(uint16_t interrupt, std::ofstream &file) {
    file << "Int 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << interrupt;
    file << std::endl;
}

#endif
