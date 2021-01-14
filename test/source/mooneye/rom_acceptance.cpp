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

#include <acacia.h>

#include "commons.h"

// #define FB_RUN_FAILING_MOONEYE_TESTS

#define MOONEYE_ACCEPTANCE_TEST(testName, folder, romName) \
TEST(testMooneyeAcceptance_##testName) { \
    FunkyBoy::fs::path romPath = FunkyBoy::fs::path("..") / "mooneye-test-roms" / "acceptance" / #folder / #romName ".gb"; \
    testUsingMooneyeROM(romPath, 5120000); \
}

TEST_SUITE(mooneyeROMAcceptance) {

    MOONEYE_ACCEPTANCE_TEST(BitsMemOam, bits, mem_oam)

    MOONEYE_ACCEPTANCE_TEST(BitsRegF, bits, reg_f)

    MOONEYE_ACCEPTANCE_TEST(InstrDaa, instr, daa)

    MOONEYE_ACCEPTANCE_TEST(OamDmaRegRead, oam_dma, reg_read)

    MOONEYE_ACCEPTANCE_TEST(Timer_DivWrite, timer, div_write)

    MOONEYE_ACCEPTANCE_TEST(Timer_Tma_WriteReloading, timer, tma_write_reloading)

    // TODO: Fix tests
    #ifdef FB_RUN_FAILING_MOONEYE_TESTS
        MOONEYE_ACCEPTANCE_TEST(BitsUnusedHwIoGS, bits, unused_hwio-GS)
        MOONEYE_ACCEPTANCE_TEST(InterruptsIEPush, interrupts, ie_push)
        MOONEYE_ACCEPTANCE_TEST(OamDmaBasic, oam_dma, basic)
        MOONEYE_ACCEPTANCE_TEST(OamDmaSourcesGS, oam_dma, sources-GS) // Unsupported MBC
        MOONEYE_ACCEPTANCE_TEST(PPU_HblankLyScxTimingGS, ppu, hblank_ly_scx_timing-GS)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_1_2_TimingGS, ppu, intr_1_2_timing-GS)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_2_0_Timing, ppu, intr_2_0_timing)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_2_mode0_Timing, ppu, intr_2_mode0_timing)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_2_mode0_TimingSprites, ppu, intr_2_mode0_timing_sprites)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_2_mode3_Timing, ppu, intr_2_mode3_timing)
        MOONEYE_ACCEPTANCE_TEST(PPU_Intr_2_OamOk_Timing, ppu, intr_2_oam_ok_timing)
        MOONEYE_ACCEPTANCE_TEST(PPU_LcdOn_TimingGS, ppu, lcdon_timing-GS)
        MOONEYE_ACCEPTANCE_TEST(PPU_LcdOnWriteTiming, ppu, lcdon_write_timing-GS)
        MOONEYE_ACCEPTANCE_TEST(PPU_Stat_Irq_Blocking, ppu, stat_irq_blocking)
        MOONEYE_ACCEPTANCE_TEST(PPU_Stat_LYC_OnOff, ppu, stat_lyc_onoff)
        MOONEYE_ACCEPTANCE_TEST(PPU_Stat_Intr_GS, ppu, vblank_stat_intr-GS)
        MOONEYE_ACCEPTANCE_TEST(Serial_Sclk_Align_DmgABCMgb, serial, boot_sclk_align-dmgABCmgb)
        MOONEYE_ACCEPTANCE_TEST(Timer_RapidToggle, timer, rapid_toggle)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim00, timer, tim00)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim01, timer, tim01)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim10, timer, tim10)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim11, timer, tim11)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim00_DivTrigger, timer, tim00_div_trigger)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim01_DivTrigger, timer, tim01_div_trigger)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim10_DivTrigger, timer, tim10_div_trigger)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tim11_DivTrigger, timer, tim11_div_trigger)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tima_Reload, timer, tima_reload)
        MOONEYE_ACCEPTANCE_TEST(Timer_Tima_WriteReloading, timer, tima_write_reloading)
        MOONEYE_ACCEPTANCE_TEST(AddSpETiming, ., add_sp_e_timing)
        MOONEYE_ACCEPTANCE_TEST(BootDiv2_S, ., boot_div2-S)
        MOONEYE_ACCEPTANCE_TEST(BootDivDmg0, ., boot_div-dmg0)
        MOONEYE_ACCEPTANCE_TEST(BootDivDmgABCMgb, ., boot_div-dmgABCmgb)
    #endif

}