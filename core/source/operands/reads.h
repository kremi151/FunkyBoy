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

#ifndef FB_CORE_INSTRUCTIONS__READS_H
#define FB_CORE_INSTRUCTIONS__READS_H

#include "instruction_context.h"

namespace FunkyBoy::Operands {

    /**
     * Reads the next byte into {@code lsb}
     * @param context
     */
    bool readLSB(InstrContext &context, Memory &memory);

    /**
     * Reads the next byte into {@code msb}
     * @param context
     */
    bool readMSB(InstrContext &context, Memory &memory);

    /**
     * Reads a signed byte into {@code signedByte}
     * @param context
     */
    bool readSigned(InstrContext &context, Memory &memory);

    /**
     * Reads C into {@code lsb}, 0xFF into {@code msb}
     * @param context
     */
    bool readRegCAsLSB(InstrContext &context, Memory &memory);

    /**
     * Reads next unsigned byte into {@code lsb}, 0xFF into {@code msb}
     * @param context
     */
    bool readMemAsLSB(InstrContext &context, Memory &memory);

    /**
     * Decrements SP (SP--) first, then reads LSB(rr) into memory pointed by SP
     * This should be executed after {@code readRRMSBIntoStack}
     * @param context
     * @return
     */
    bool readRRLSBIntoStack(InstrContext &context, Memory &memory);

    /**
     * Decrements SP (SP--) first, then reads MSB(rr) into memory pointed by SP.
     * This should be executed before {@code readRRLSBIntoStack}
     * @param context
     * @return
     */
    bool readRRMSBIntoStack(InstrContext &context, Memory &memory);

    /**
     * Reads memory pointed by SP into {@code lsb}, then increments SP (SP++).
     * Should be executed before {@code readStackIntoRRMSB}
     * @param context
     * @return
     */
    bool readStackIntoLSB(InstrContext &context, Memory &memory);

    /**
     * Reads memory pointed by SP into {@code msb}, then increments SP (SP++).
     * Should be executed after {@code readStackIntoRRLSB}
     * @param context
     * @return
     */
    bool readStackIntoMSB(InstrContext &context, Memory &memory);

    /**
     * Decrements SP (SP--) first, then reads register F into memory pointed by SP
     * This should be executed after {@code readRegAIntoStack}
     * @param context
     * @return
     */
    bool readRegFIntoStack(InstrContext &context, Memory &memory);

    /**
     * Decrements SP (SP--) first, then reads register A into memory pointed by SP.
     * This should be executed before {@code readRegFIntoStack}
     * @param context
     * @return
     */
    bool readRegAIntoStack(InstrContext &context, Memory &memory);

    /**
     * Reads byte at address (HL) into {@code lsb}
     * @param context
     * @return
     */
    bool readHLMem(InstrContext &context, Memory &memory);

}

#endif //FB_CORE_INSTRUCTIONS__READS_H
