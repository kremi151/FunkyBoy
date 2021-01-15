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

    MOONEYE_ACCEPTANCE_TEST(OamDmaBasic, oam_dma, basic)

    MOONEYE_ACCEPTANCE_TEST(BootRegsDMGABC, ., boot_regs-dmgABC)

    // TODO: Fix tests
    #ifdef FB_RUN_FAILING_MOONEYE_TESTS
        MOONEYE_ACCEPTANCE_TEST(BitsUnusedHwIoGS, bits, unused_hwio-GS)
        MOONEYE_ACCEPTANCE_TEST(InterruptsIEPush, interrupts, ie_push)
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
        MOONEYE_ACCEPTANCE_TEST(BootDivS, ., boot_div-S)
        MOONEYE_ACCEPTANCE_TEST(BootHwioDMG0, ., boot_hwio-dmg0)
        MOONEYE_ACCEPTANCE_TEST(BootHwioDMGABCMGB, ., boot_hwio-dmgABCmgb)
        MOONEYE_ACCEPTANCE_TEST(BootHwioS, ., boot_hwio-S)
        MOONEYE_ACCEPTANCE_TEST(BootRegsDMG0, ., boot_regs-dmg0)
        MOONEYE_ACCEPTANCE_TEST(BootRegsMGB, ., boot_regs-mgb)
        MOONEYE_ACCEPTANCE_TEST(BootRegsSGB, ., boot_regs-sgb)
        MOONEYE_ACCEPTANCE_TEST(BootRegsSGB2, ., boot_regs-sgb2)
        MOONEYE_ACCEPTANCE_TEST(CallCCTiming, ., call_cc_timing)
        MOONEYE_ACCEPTANCE_TEST(CallCCTiming2, ., call_cc_timing2)
        MOONEYE_ACCEPTANCE_TEST(CallTiming, ., call_timing)
        MOONEYE_ACCEPTANCE_TEST(CallTiming2, ., call_timing2)
        MOONEYE_ACCEPTANCE_TEST(DITiming, ., di_timing-GS)
        MOONEYE_ACCEPTANCE_TEST(DivTiming, ., div_timing)
        MOONEYE_ACCEPTANCE_TEST(EISequence, ., ei_sequence)
        MOONEYE_ACCEPTANCE_TEST(EITiming, ., ei_timing)
        MOONEYE_ACCEPTANCE_TEST(HaltIme0EI, ., halt_ime0_ei)
        MOONEYE_ACCEPTANCE_TEST(HaltIme0NoIntrTiming, ., halt_ime0_nointr_timing)
        MOONEYE_ACCEPTANCE_TEST(HaltIme1Timing, ., halt_ime1_timing)
        MOONEYE_ACCEPTANCE_TEST(HaltIme2GS, ., halt_ime1_timing2-GS)
        MOONEYE_ACCEPTANCE_TEST(IFIERegisters, ., if_ie_registers)
        MOONEYE_ACCEPTANCE_TEST(IntrTiming, ., intr_timing)
        MOONEYE_ACCEPTANCE_TEST(JpCCTiming, ., jp_cc_timing)
        MOONEYE_ACCEPTANCE_TEST(JPTiming, ., jp_timing)
        MOONEYE_ACCEPTANCE_TEST(LDHLSPeTiming, ., ld_hl_sp_e_timing)
        MOONEYE_ACCEPTANCE_TEST(OAMDMARestart, ., oam_dma_restart)
        MOONEYE_ACCEPTANCE_TEST(OAMDMAStart, ., oam_dma_start)
        MOONEYE_ACCEPTANCE_TEST(OAMDMATiming, ., oam_dma_timing)
        MOONEYE_ACCEPTANCE_TEST(PopTiming, ., pop_timing)
        MOONEYE_ACCEPTANCE_TEST(PushTiming, ., push_timing)
        MOONEYE_ACCEPTANCE_TEST(RapidDIEI, ., rapid_di_ei)
        MOONEYE_ACCEPTANCE_TEST(RETCCTiming, ., ret_cc_timing)
        MOONEYE_ACCEPTANCE_TEST(RETTiming, ., ret_timing)
        MOONEYE_ACCEPTANCE_TEST(RETIIntrTiming, ., reti_intr_timing)
        MOONEYE_ACCEPTANCE_TEST(RETITiming, ., reti_timing)
        MOONEYE_ACCEPTANCE_TEST(RSTTiming, ., rst_timing)
#endif

}