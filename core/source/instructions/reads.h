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

#ifndef FB_CORE_READS_H
#define FB_CORE_READS_H

#include "instruction_context.h"

namespace FunkyBoy::Instructions {

    /**
     * Reads the next byte into {@code lsb}
     * @param context
     */
    void readLSB(InstrContext &context);

    /**
     * Reads the next byte into {@code msb}
     * @param context
     */
    void readMSB(InstrContext &context);

    /**
     * Reads a signed byte into {@code signedByte}
     * @param context
     */
    void readSigned(InstrContext &context);

    /**
     * Reads C into {@code lsb}, 0xFF into {@code msb}
     * @param context
     */
    void readRegCAsLSB(InstrContext &context);

    /**
     * Reads next unsigned byte into {@code lsb}, 0xFF into {@code msb}
     * @param context
     */
    void readMemAsLSB(InstrContext &context);

}

#endif //FB_CORE_READS_H
