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

#ifndef FB_CORE_INSTRUCTIONS_MISC_H
#define FB_CORE_INSTRUCTIONS_MISC_H

#include <instructions/context.h>
#include <memory/memory.h>

namespace FunkyBoy::Instructions::Miscellaneous  {

    /**
     * DAA
     */
    void daa(Instructions::context &context);

    /**
     * CPL
     */
    void cpl(Instructions::context &context);

    /**
     * SCF
     */
    void scf(Instructions::context &context);

    /**
     * CCF
     */
    void ccf(Instructions::context &context);

    /**
     * DI
     */
    void di(Instructions::context &context);

    /**
     * EI
     */
    void ei(Instructions::context &context);

    /**
     * STOP
     */
    void stop(FunkyBoy::Memory &memory, Instructions::context &context);

    /**
     * HALT
     */
    void halt(FunkyBoy::Memory &memory, Instructions::context &context);

}

#endif //FB_CORE_INSTRUCTIONS_MISC_H
