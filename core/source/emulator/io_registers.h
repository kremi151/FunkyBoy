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

#ifndef FB_CORE_IO_REGISTERS_H
#define FB_CORE_IO_REGISTERS_H

#include <util/typedefs.h>
#include <util/testing.h>
#include <memory>

#define FB_REG_P1 0xFF00
#define FB_REG_DIV 0xFF04
#define FB_REG_TIMA 0xFF05
#define FB_REG_TMA 0xFF06
#define FB_REG_TAC 0xFF07
#define FB_REG_IE 0xFFFF

namespace FunkyBoy {

    class io_registers {
    test_public:
        u8 sys_counter_lsb;
        u8 sys_counter_msb;
    public:

        io_registers();

        void incrementSysCounter();
        void resetSysCounter();

        fb_inline u16 getSysCounter();

        fb_inline const u8 &sysCounterLsb();
        fb_inline const u8 &sysCounterMsb();
    };

    typedef std::shared_ptr<io_registers> io_registers_ptr;

}

#endif //FB_CORE_IO_REGISTERS_H
