/******************************************************************************
 *  Copyright (C) 2017 Broadcom. The term "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
 *
 *  This program is the proprietary software of Broadcom and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 * The launch point for all information concerning RDB is found at:
 *   http://bcgbu.broadcom.com/RDB/SitePages/Home.aspx
 *
 * Date:           Generated on               Tue Oct 10 01:11:41 2017
 *                 Full Compile MD5 Checksum  ea353cbbaf7534b1acfabe87187756c5
 *                     (minus title and desc)
 *                 MD5 Checksum               bd09f6ea4577b9d8ea88a425effe4ef4
 *
 * lock_release:   n/a
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     1777
 *                 unknown                    unknown
 *                 Perl Interpreter           5.014001
 *                 Operating System           linux
 *                 Script Source              projects/stbgit/bin/gen_rdb.pl
 *                 DVTSWVER                   LOCAL projects/stbgit/bin/gen_rdb.pl
 *
 *
********************************************************************************/

#ifndef BCHP_AVS_HW_MNTR_H__
#define BCHP_AVS_HW_MNTR_H__

/***************************************************************************
 *AVS_HW_MNTR - AVS HW Monitor Core
 ***************************************************************************/
#define BCHP_AVS_HW_MNTR_SW_CONTROLS             0x0000152000 /* [RW][32] Software control command registers for AVS */
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY 0x0000152004 /* [RO][32] Indicate measurement unit is busy and SW should not de-assert sw_takeover while this is asserted */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR 0x0000152008 /* [RW][32] Software to reset the pvt monitors measurements' valid bits in RO registers */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0 0x000015200c /* [RW][32] Software to reset the central roscs measurements' valid bits in RO registers */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1 0x0000152010 /* [RW][32] Software to reset the central roscs measurements' valid bits in RO registers */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG 0x0000152034 /* [RW][32] Software to reset the power watchdog measurment's valid bits in RO registers */
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT          0x0000152038 /* [RW][32] Initialize the sensor sequencer */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR 0x000015203c /* [RW][32] Indicate which PVT Monitor measurements should  be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0 0x0000152040 /* [RW][32] Indicate which central ring oscillators should  be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1 0x0000152044 /* [RW][32] Indicate which central ring oscillators should  be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR 0x0000152068 /* [RW][32] Enabling/Disabling PVT monitor */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0 0x000015206c /* [RW][32] Enabling/Disabling of central ring oscillators */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1 0x0000152070 /* [RW][32] Enabling/Disabling of central ring oscillators */
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL 0x0000152074 /* [RW][32] Control the time taken for a rosc/pwd measurement */
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE      0x0000152078 /* [RW][32] Control the counting event for rosc signal counter */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN   0x000015207c /* [RW][32] Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_1 0x0000152080 /* [RW][32] Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_2 0x0000152084 /* [RW][32] Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_3 0x0000152088 /* [RW][32] Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt */
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN 0x000015208c /* [RW][32] Enable to trigger an interrupt when the measurement is done */
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR    0x0000152090 /* [RO][32] The last sensor that has been measured */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS     0x0000152094 /* [RO][32] Indicate AVS interrupts status whether they are triggered or not */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR 0x0000152098 /* [RW][32] Software to clear the respective AVS interrupt flags */
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS     0x00001520ac /* [RW][32] To lock read/write accesses to AVS sensors and regulators registers for security purpose */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE 0x00001520b0 /* [RW][32] Software to enable chip's temperature monitoring and temperature reset */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD   0x00001520b4 /* [RW][32] Threshold value for chip's temperature monitoring */
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_0 0x00001520b8 /* [RW][32] Set the output value of ring oscillators when not enabled. */
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1 0x00001520bc /* [RW][32] Set the output value of ring oscillators when not enabled. */
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME       0x00001520c0 /* [RW][32] Define the number of samples output by ADC to skip following the change in ADC source select */
#define BCHP_AVS_HW_MNTR_AVS_SPARE_0             0x00001520c4 /* [RW][32] Spare register 0 for AVS HW MONITOR core */
#define BCHP_AVS_HW_MNTR_AVS_SPARE_1             0x00001520c8 /* [RW][32] Spare register 1 for AVS HW MONITOR core */

/***************************************************************************
 *SW_CONTROLS - Software control command registers for AVS
 ***************************************************************************/
/* AVS_HW_MNTR :: SW_CONTROLS :: reserved0 [31:16] */
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_reserved0_MASK                0xffff0000
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_reserved0_SHIFT               16

/* AVS_HW_MNTR :: SW_CONTROLS :: sw_sensor_idx [15:08] */
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_sensor_idx_MASK            0x0000ff00
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_sensor_idx_SHIFT           8
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_sensor_idx_DEFAULT         0x00000000

/* AVS_HW_MNTR :: SW_CONTROLS :: reserved1 [07:02] */
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_reserved1_MASK                0x000000fc
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_reserved1_SHIFT               2

/* AVS_HW_MNTR :: SW_CONTROLS :: sw_do_measure [01:01] */
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_do_measure_MASK            0x00000002
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_do_measure_SHIFT           1
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_do_measure_DEFAULT         0x00000000

/* AVS_HW_MNTR :: SW_CONTROLS :: sw_takeover [00:00] */
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_takeover_MASK              0x00000001
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_takeover_SHIFT             0
#define BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_takeover_DEFAULT           0x00000000

/***************************************************************************
 *SW_MEASUREMENT_UNIT_BUSY - Indicate measurement unit is busy and SW should not de-assert sw_takeover while this is asserted
 ***************************************************************************/
/* AVS_HW_MNTR :: SW_MEASUREMENT_UNIT_BUSY :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_reserved0_MASK   0xfffffffe
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_reserved0_SHIFT  1

/* AVS_HW_MNTR :: SW_MEASUREMENT_UNIT_BUSY :: busy [00:00] */
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_busy_MASK        0x00000001
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_busy_SHIFT       0
#define BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_busy_DEFAULT     0x00000000

/***************************************************************************
 *MEASUREMENTS_INIT_PVT_MNTR - Software to reset the pvt monitors measurements' valid bits in RO registers
 ***************************************************************************/
/* AVS_HW_MNTR :: MEASUREMENTS_INIT_PVT_MNTR :: reserved0 [31:08] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR_reserved0_MASK 0xffffff00
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR_reserved0_SHIFT 8

/* AVS_HW_MNTR :: MEASUREMENTS_INIT_PVT_MNTR :: m_init_pvt_mntr [07:00] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR_m_init_pvt_mntr_MASK 0x000000ff
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR_m_init_pvt_mntr_SHIFT 0
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTR_m_init_pvt_mntr_DEFAULT 0x00000000

/***************************************************************************
 *MEASUREMENTS_INIT_CEN_ROSC_0 - Software to reset the central roscs measurements' valid bits in RO registers
 ***************************************************************************/
/* AVS_HW_MNTR :: MEASUREMENTS_INIT_CEN_ROSC_0 :: m_init_cen_rosc [31:00] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0_m_init_cen_rosc_MASK 0xffffffff
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0_m_init_cen_rosc_SHIFT 0
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0_m_init_cen_rosc_DEFAULT 0x00000000

/***************************************************************************
 *MEASUREMENTS_INIT_CEN_ROSC_1 - Software to reset the central roscs measurements' valid bits in RO registers
 ***************************************************************************/
/* AVS_HW_MNTR :: MEASUREMENTS_INIT_CEN_ROSC_1 :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1_reserved0_MASK 0xfffffff0
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1_reserved0_SHIFT 4

/* AVS_HW_MNTR :: MEASUREMENTS_INIT_CEN_ROSC_1 :: m_init_cen_rosc [03:00] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1_m_init_cen_rosc_MASK 0x0000000f
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1_m_init_cen_rosc_SHIFT 0
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1_m_init_cen_rosc_DEFAULT 0x00000000

/***************************************************************************
 *MEASUREMENTS_INIT_POW_WDOG - Software to reset the power watchdog measurment's valid bits in RO registers
 ***************************************************************************/
/* AVS_HW_MNTR :: MEASUREMENTS_INIT_POW_WDOG :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG_reserved0_MASK 0xfffffffe
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG_reserved0_SHIFT 1

/* AVS_HW_MNTR :: MEASUREMENTS_INIT_POW_WDOG :: m_init_pow_wdog [00:00] */
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG_m_init_pow_wdog_MASK 0x00000001
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG_m_init_pow_wdog_SHIFT 0
#define BCHP_AVS_HW_MNTR_MEASUREMENTS_INIT_POW_WDOG_m_init_pow_wdog_DEFAULT 0x00000000

/***************************************************************************
 *SEQUENCER_INIT - Initialize the sensor sequencer
 ***************************************************************************/
/* AVS_HW_MNTR :: SEQUENCER_INIT :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT_reserved0_MASK             0xfffffffe
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT_reserved0_SHIFT            1

/* AVS_HW_MNTR :: SEQUENCER_INIT :: sequencer_init [00:00] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT_sequencer_init_MASK        0x00000001
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT_sequencer_init_SHIFT       0
#define BCHP_AVS_HW_MNTR_SEQUENCER_INIT_sequencer_init_DEFAULT     0x00000000

/***************************************************************************
 *SEQUENCER_MASK_PVT_MNTR - Indicate which PVT Monitor measurements should  be masked(skipped) in the measurement sequence
 ***************************************************************************/
/* AVS_HW_MNTR :: SEQUENCER_MASK_PVT_MNTR :: reserved0 [31:07] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR_reserved0_MASK    0xffffff80
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR_reserved0_SHIFT   7

/* AVS_HW_MNTR :: SEQUENCER_MASK_PVT_MNTR :: seq_mask_pvt_mntr [06:00] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR_seq_mask_pvt_mntr_MASK 0x0000007f
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR_seq_mask_pvt_mntr_SHIFT 0
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR_seq_mask_pvt_mntr_DEFAULT 0x00000000

/***************************************************************************
 *SEQUENCER_MASK_CEN_ROSC_0 - Indicate which central ring oscillators should  be masked(skipped) in the measurement sequence
 ***************************************************************************/
/* AVS_HW_MNTR :: SEQUENCER_MASK_CEN_ROSC_0 :: seq_mask_cen_rosc [31:00] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0_seq_mask_cen_rosc_MASK 0xffffffff
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0_seq_mask_cen_rosc_SHIFT 0
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0_seq_mask_cen_rosc_DEFAULT 0x00000000

/***************************************************************************
 *SEQUENCER_MASK_CEN_ROSC_1 - Indicate which central ring oscillators should  be masked(skipped) in the measurement sequence
 ***************************************************************************/
/* AVS_HW_MNTR :: SEQUENCER_MASK_CEN_ROSC_1 :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1_reserved0_MASK  0xfffffff0
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1_reserved0_SHIFT 4

/* AVS_HW_MNTR :: SEQUENCER_MASK_CEN_ROSC_1 :: seq_mask_cen_rosc [03:00] */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1_seq_mask_cen_rosc_MASK 0x0000000f
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1_seq_mask_cen_rosc_SHIFT 0
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1_seq_mask_cen_rosc_DEFAULT 0x00000000

/***************************************************************************
 *ENABLE_DEFAULT_PVT_MNTR - Enabling/Disabling PVT monitor
 ***************************************************************************/
/* AVS_HW_MNTR :: ENABLE_DEFAULT_PVT_MNTR :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR_reserved0_MASK    0xfffffffe
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR_reserved0_SHIFT   1

/* AVS_HW_MNTR :: ENABLE_DEFAULT_PVT_MNTR :: pvt_mntr_pwrdn_default [00:00] */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR_pvt_mntr_pwrdn_default_MASK 0x00000001
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR_pvt_mntr_pwrdn_default_SHIFT 0
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_PVT_MNTR_pvt_mntr_pwrdn_default_DEFAULT 0x00000000

/***************************************************************************
 *ENABLE_DEFAULT_CEN_ROSC_0 - Enabling/Disabling of central ring oscillators
 ***************************************************************************/
/* AVS_HW_MNTR :: ENABLE_DEFAULT_CEN_ROSC_0 :: cen_rosc_enable_default [31:00] */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0_cen_rosc_enable_default_MASK 0xffffffff
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0_cen_rosc_enable_default_SHIFT 0
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0_cen_rosc_enable_default_DEFAULT 0xffffffff

/***************************************************************************
 *ENABLE_DEFAULT_CEN_ROSC_1 - Enabling/Disabling of central ring oscillators
 ***************************************************************************/
/* AVS_HW_MNTR :: ENABLE_DEFAULT_CEN_ROSC_1 :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1_reserved0_MASK  0xfffffff0
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1_reserved0_SHIFT 4

/* AVS_HW_MNTR :: ENABLE_DEFAULT_CEN_ROSC_1 :: cen_rosc_enable_default [03:00] */
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1_cen_rosc_enable_default_MASK 0x0000000f
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1_cen_rosc_enable_default_SHIFT 0
#define BCHP_AVS_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1_cen_rosc_enable_default_DEFAULT 0x0000000f

/***************************************************************************
 *ROSC_MEASUREMENT_TIME_CONTROL - Control the time taken for a rosc/pwd measurement
 ***************************************************************************/
/* AVS_HW_MNTR :: ROSC_MEASUREMENT_TIME_CONTROL :: reserved0 [31:08] */
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL_reserved0_MASK 0xffffff00
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL_reserved0_SHIFT 8

/* AVS_HW_MNTR :: ROSC_MEASUREMENT_TIME_CONTROL :: limit [07:00] */
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL_limit_MASK  0x000000ff
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL_limit_SHIFT 0
#define BCHP_AVS_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROL_limit_DEFAULT 0x0000001f

/***************************************************************************
 *ROSC_COUNTING_MODE - Control the counting event for rosc signal counter
 ***************************************************************************/
/* AVS_HW_MNTR :: ROSC_COUNTING_MODE :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE_reserved0_MASK         0xfffffffe
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE_reserved0_SHIFT        1

/* AVS_HW_MNTR :: ROSC_COUNTING_MODE :: mode [00:00] */
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE_mode_MASK              0x00000001
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE_mode_SHIFT             0
#define BCHP_AVS_HW_MNTR_ROSC_COUNTING_MODE_mode_DEFAULT           0x00000000

/***************************************************************************
 *INTERRUPT_POW_WDOG_EN - Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt
 ***************************************************************************/
/* AVS_HW_MNTR :: INTERRUPT_POW_WDOG_EN :: irq_en [31:00] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_irq_en_MASK         0xffffffff
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_irq_en_SHIFT        0
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_irq_en_DEFAULT      0x00000000

/***************************************************************************
 *INTERRUPT_POW_WDOG_EN_1 - Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt
 ***************************************************************************/
/* AVS_HW_MNTR :: INTERRUPT_POW_WDOG_EN_1 :: irq_en [31:00] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_1_irq_en_MASK       0xffffffff
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_1_irq_en_SHIFT      0
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_1_irq_en_DEFAULT    0x00000000

/***************************************************************************
 *INTERRUPT_POW_WDOG_EN_2 - Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt
 ***************************************************************************/
/* AVS_HW_MNTR :: INTERRUPT_POW_WDOG_EN_2 :: irq_en [31:00] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_2_irq_en_MASK       0xffffffff
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_2_irq_en_SHIFT      0
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_2_irq_en_DEFAULT    0x00000000

/***************************************************************************
 *INTERRUPT_POW_WDOG_EN_3 - Software to program a mask(1 bit per PWD) to indicate which PWD signals may trigger an interrupt
 ***************************************************************************/
/* AVS_HW_MNTR :: INTERRUPT_POW_WDOG_EN_3 :: irq_en [31:00] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_3_irq_en_MASK       0xffffffff
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_3_irq_en_SHIFT      0
#define BCHP_AVS_HW_MNTR_INTERRUPT_POW_WDOG_EN_3_irq_en_DEFAULT    0x00000000

/***************************************************************************
 *INTERRUPT_SW_MEASUREMENT_DONE_EN - Enable to trigger an interrupt when the measurement is done
 ***************************************************************************/
/* AVS_HW_MNTR :: INTERRUPT_SW_MEASUREMENT_DONE_EN :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN_reserved0_MASK 0xfffffffe
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN_reserved0_SHIFT 1

/* AVS_HW_MNTR :: INTERRUPT_SW_MEASUREMENT_DONE_EN :: irq_en [00:00] */
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN_irq_en_MASK 0x00000001
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN_irq_en_SHIFT 0
#define BCHP_AVS_HW_MNTR_INTERRUPT_SW_MEASUREMENT_DONE_EN_irq_en_DEFAULT 0x00000000

/***************************************************************************
 *LAST_MEASURED_SENSOR - The last sensor that has been measured
 ***************************************************************************/
/* AVS_HW_MNTR :: LAST_MEASURED_SENSOR :: reserved0 [31:08] */
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR_reserved0_MASK       0xffffff00
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR_reserved0_SHIFT      8

/* AVS_HW_MNTR :: LAST_MEASURED_SENSOR :: sensor_idx [07:00] */
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR_sensor_idx_MASK      0x000000ff
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR_sensor_idx_SHIFT     0
#define BCHP_AVS_HW_MNTR_LAST_MEASURED_SENSOR_sensor_idx_DEFAULT   0x0000003f

/***************************************************************************
 *AVS_INTERRUPT_FLAGS - Indicate AVS interrupts status whether they are triggered or not
 ***************************************************************************/
/* AVS_HW_MNTR :: AVS_INTERRUPT_FLAGS :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_reserved0_MASK        0xfffffff0
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_reserved0_SHIFT       4

/* AVS_HW_MNTR :: AVS_INTERRUPT_FLAGS :: flag_on [03:00] */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_flag_on_MASK          0x0000000f
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_flag_on_SHIFT         0
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_flag_on_DEFAULT       0x00000000

/***************************************************************************
 *AVS_INTERRUPT_FLAGS_CLEAR - Software to clear the respective AVS interrupt flags
 ***************************************************************************/
/* AVS_HW_MNTR :: AVS_INTERRUPT_FLAGS_CLEAR :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR_reserved0_MASK  0xfffffff0
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR_reserved0_SHIFT 4

/* AVS_HW_MNTR :: AVS_INTERRUPT_FLAGS_CLEAR :: clear [03:00] */
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR_clear_MASK      0x0000000f
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR_clear_SHIFT     0
#define BCHP_AVS_HW_MNTR_AVS_INTERRUPT_FLAGS_CLEAR_clear_DEFAULT   0x00000000

/***************************************************************************
 *AVS_REGISTERS_LOCKS - To lock read/write accesses to AVS sensors and regulators registers for security purpose
 ***************************************************************************/
/* AVS_HW_MNTR :: AVS_REGISTERS_LOCKS :: reserved0 [31:03] */
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS_reserved0_MASK        0xfffffff8
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS_reserved0_SHIFT       3

/* AVS_HW_MNTR :: AVS_REGISTERS_LOCKS :: lock [02:00] */
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS_lock_MASK             0x00000007
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS_lock_SHIFT            0
#define BCHP_AVS_HW_MNTR_AVS_REGISTERS_LOCKS_lock_DEFAULT          0x00000000

/***************************************************************************
 *TEMPERATURE_RESET_ENABLE - Software to enable chip's temperature monitoring and temperature reset
 ***************************************************************************/
/* AVS_HW_MNTR :: TEMPERATURE_RESET_ENABLE :: reserved0 [31:01] */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE_reserved0_MASK   0xfffffffe
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE_reserved0_SHIFT  1

/* AVS_HW_MNTR :: TEMPERATURE_RESET_ENABLE :: reset_enable [00:00] */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE_reset_enable_MASK 0x00000001
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE_reset_enable_SHIFT 0
#define BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE_reset_enable_DEFAULT 0x00000000

/***************************************************************************
 *TEMPERATURE_THRESHOLD - Threshold value for chip's temperature monitoring
 ***************************************************************************/
/* AVS_HW_MNTR :: TEMPERATURE_THRESHOLD :: reserved0 [31:10] */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD_reserved0_MASK      0xfffffc00
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD_reserved0_SHIFT     10

/* AVS_HW_MNTR :: TEMPERATURE_THRESHOLD :: threshold [09:00] */
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD_threshold_MASK      0x000003ff
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD_threshold_SHIFT     0
#define BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD_threshold_DEFAULT   0x00000216

/***************************************************************************
 *IDLE_STATE_0_CEN_ROSC_0 - Set the output value of ring oscillators when not enabled.
 ***************************************************************************/
/* AVS_HW_MNTR :: IDLE_STATE_0_CEN_ROSC_0 :: cen_rosc_idle_state_0 [31:00] */
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_0_cen_rosc_idle_state_0_MASK 0xffffffff
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_0_cen_rosc_idle_state_0_SHIFT 0
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_0_cen_rosc_idle_state_0_DEFAULT 0x00000000

/***************************************************************************
 *IDLE_STATE_0_CEN_ROSC_1 - Set the output value of ring oscillators when not enabled.
 ***************************************************************************/
/* AVS_HW_MNTR :: IDLE_STATE_0_CEN_ROSC_1 :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1_reserved0_MASK    0xfffffff0
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1_reserved0_SHIFT   4

/* AVS_HW_MNTR :: IDLE_STATE_0_CEN_ROSC_1 :: cen_rosc_idle_state_0 [03:00] */
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1_cen_rosc_idle_state_0_MASK 0x0000000f
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1_cen_rosc_idle_state_0_SHIFT 0
#define BCHP_AVS_HW_MNTR_IDLE_STATE_0_CEN_ROSC_1_cen_rosc_idle_state_0_DEFAULT 0x00000000

/***************************************************************************
 *ADC_SETTLING_TIME - Define the number of samples output by ADC to skip following the change in ADC source select
 ***************************************************************************/
/* AVS_HW_MNTR :: ADC_SETTLING_TIME :: reserved0 [31:04] */
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME_reserved0_MASK          0xfffffff0
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME_reserved0_SHIFT         4

/* AVS_HW_MNTR :: ADC_SETTLING_TIME :: samples [03:00] */
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME_samples_MASK            0x0000000f
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME_samples_SHIFT           0
#define BCHP_AVS_HW_MNTR_ADC_SETTLING_TIME_samples_DEFAULT         0x00000005

/***************************************************************************
 *AVS_SPARE_0 - Spare register 0 for AVS HW MONITOR core
 ***************************************************************************/
/* AVS_HW_MNTR :: AVS_SPARE_0 :: spare [31:00] */
#define BCHP_AVS_HW_MNTR_AVS_SPARE_0_spare_MASK                    0xffffffff
#define BCHP_AVS_HW_MNTR_AVS_SPARE_0_spare_SHIFT                   0
#define BCHP_AVS_HW_MNTR_AVS_SPARE_0_spare_DEFAULT                 0x00000000

/***************************************************************************
 *AVS_SPARE_1 - Spare register 1 for AVS HW MONITOR core
 ***************************************************************************/
/* AVS_HW_MNTR :: AVS_SPARE_1 :: spare [31:00] */
#define BCHP_AVS_HW_MNTR_AVS_SPARE_1_spare_MASK                    0xffffffff
#define BCHP_AVS_HW_MNTR_AVS_SPARE_1_spare_SHIFT                   0
#define BCHP_AVS_HW_MNTR_AVS_SPARE_1_spare_DEFAULT                 0x00000000

#endif /* #ifndef BCHP_AVS_HW_MNTR_H__ */

/* End of File */
