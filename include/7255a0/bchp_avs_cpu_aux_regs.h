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
 * Date:           Generated on               Sun Nov 19 01:07:17 2017
 *                 Full Compile MD5 Checksum  51b81740b386aea70724ff951c628857
 *                     (minus title and desc)
 *                 MD5 Checksum               96520679e55aabb9e30a2b8c1241ef41
 *
 * lock_release:   r_1772
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

#ifndef BCHP_AVS_CPU_AUX_REGS_H__
#define BCHP_AVS_CPU_AUX_REGS_H__

/***************************************************************************
 *AVS_CPU_AUX_REGS - CPU Auxiliary Registers
 ***************************************************************************/
#define BCHP_AVS_CPU_AUX_REGS_STATUS             0x00204ca000 /* [RO][32] Auxiliary Register STATUS */
#define BCHP_AVS_CPU_AUX_REGS_SEMAPHORE          0x00204ca004 /* [RW][32] Inter-process/Host semaphore register */
#define BCHP_AVS_CPU_AUX_REGS_LP_START           0x00204ca008 /* [RW][32] Loop start address (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_LP_END             0x00204ca00c /* [RW][32] Loop end address (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_IDENTITY           0x00204ca010 /* [RO][32] Processor Identification register */
#define BCHP_AVS_CPU_AUX_REGS_DEBUG_AVS          0x00204ca014 /* [RW][32] Debug register */
#define BCHP_AVS_CPU_AUX_REGS_PC                 0x00204ca018 /* [RO][32] Program Counter register (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32           0x00204ca028 /* [RO][32] Status register (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L1        0x00204ca02c /* [RW][32] Status register save for level 1 interrupts */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L2        0x00204ca030 /* [RW][32] Status register save for level 2 interrupts */
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM           0x00204ca060 /* [RW][32] Address of Local Data RAM */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT       0x00204ca084 /* [RW][32] Processor Timer0 Count value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL     0x00204ca088 /* [RW][32] Processor Timer0 Control value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_LIMIT       0x00204ca08c /* [RW][32] Processor Timer0 Limit value */
#define BCHP_AVS_CPU_AUX_REGS_INT_VEC_BASE       0x00204ca094 /* [RW][32] Interrupt Vector Base address */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LV12       0x00204ca10c /* [RW][32] Interrupt Level Status */
#define BCHP_AVS_CPU_AUX_REGS_CRC_BUILD_BCR      0x00204ca188 /* [RO][32] Build configuration register for CRC instruction. */
#define BCHP_AVS_CPU_AUX_REGS_DVBF_BUILD         0x00204ca190 /* [RO][32] Build configuration register for dual viterbi butterfly instruction. */
#define BCHP_AVS_CPU_AUX_REGS_EXT_ARITH_BUILD    0x00204ca194 /* [RO][32] Build configuration register to specify that the processor has the extended arithmetic instructions. */
#define BCHP_AVS_CPU_AUX_REGS_DATASPACE          0x00204ca198 /* [RO][32] Build configuration register for dataspace. */
#define BCHP_AVS_CPU_AUX_REGS_MEMSUBSYS          0x00204ca19c /* [RO][32] Build configuration register for memory subsytem. */
#define BCHP_AVS_CPU_AUX_REGS_VECBASE_AC_BUILD   0x00204ca1a0 /* [RO][32] Build configuration register for ARC600 interrupt vector base address. */
#define BCHP_AVS_CPU_AUX_REGS_P_BASE_ADDR        0x00204ca1a4 /* [RO][32] Build configuration register for peripheral base address. */
#define BCHP_AVS_CPU_AUX_REGS_MPU_BUILD          0x00204ca1b4 /* [RO][32] Build configuration register for memory protection unit. */
#define BCHP_AVS_CPU_AUX_REGS_RF_BUILD           0x00204ca1b8 /* [RO][32] Build configuration register for register file. */
#define BCHP_AVS_CPU_AUX_REGS_D_CACHE_BUILD      0x00204ca1c8 /* [RO][32] Build configuration register for data cache. */
#define BCHP_AVS_CPU_AUX_REGS_MADI_BUILD         0x00204ca1cc /* [RO][32] Build configuration register for multiple ARC debug interface. */
#define BCHP_AVS_CPU_AUX_REGS_DCCM_BUILD         0x00204ca1d0 /* [RO][32] Build configuration register for data closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_TIMER_BUILD        0x00204ca1d4 /* [RO][32] Build configuration register for timers. */
#define BCHP_AVS_CPU_AUX_REGS_AP_BUILD           0x00204ca1d8 /* [RO][32] Build configuration register for actionpoints. */
#define BCHP_AVS_CPU_AUX_REGS_I_CACHE_BUILD      0x00204ca1dc /* [RO][32] Build configuration register for instruction cache. */
#define BCHP_AVS_CPU_AUX_REGS_ICCM_BUILD         0x00204ca1e0 /* [RO][32] Build configuration register for instruction closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_DSPRAM_BUILD       0x00204ca1e4 /* [RO][32] Build configuration register for XY memory. */
#define BCHP_AVS_CPU_AUX_REGS_MAC_BUILD          0x00204ca1e8 /* [RO][32] Build configuration register for Xmac. */
#define BCHP_AVS_CPU_AUX_REGS_MULTIPLY_BUILD     0x00204ca1ec /* [RO][32] Build configuration register for instruction closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_SWAP_BUILD         0x00204ca1f0 /* [RO][32] Build configuration register for swap instruction. */
#define BCHP_AVS_CPU_AUX_REGS_NORM_BUILD         0x00204ca1f4 /* [RO][32] Build configuration register for normalise instruction. */
#define BCHP_AVS_CPU_AUX_REGS_MINMAX_BUILD       0x00204ca1f8 /* [RO][32] Build configuration register for min/max instruction. */
#define BCHP_AVS_CPU_AUX_REGS_BARREL_BUILD       0x00204ca1fc /* [RO][32] Build configuration register for barrel shifter. */
#define BCHP_AVS_CPU_AUX_REGS_ARC600_BUILD       0x00204ca304 /* [RO][32] Build configuration register for ARC 600. */
#define BCHP_AVS_CPU_AUX_REGS_AUX_SYSTEM_BUILD   0x00204ca308 /* [RW][32] Build configuration register for AS221BD. */
#define BCHP_AVS_CPU_AUX_REGS_MCD_BCR            0x00204ca310 /* [RO][32] MCD configuration register for AS221BD. */
#define BCHP_AVS_CPU_AUX_REGS_IFETCHQUEUE_BUILD  0x00204ca3f8 /* [RO][32] Build configuration register for the InstructionFetchQueue component. */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT       0x00204ca400 /* [RW][32] Processor Timer 1 Count value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL     0x00204ca404 /* [RW][32] Processor Timer 1 Control value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_LIMIT       0x00204ca408 /* [RW][32] Processor Timer 1 Limit value */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LEV        0x00204ca800 /* [RW][32] Interrupt Level Programming */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_HINT       0x00204ca804 /* [RW][32] Software Triggered Interrupt */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_CTRL     0x00204ca808 /* [RW][32] Memory Alignment Detection Control */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_INTERRUPT 0x00204caa00 /* [RO][32] Inter-core Interrupt Register */
#define BCHP_AVS_CPU_AUX_REGS_AX_IPC_SEM_N       0x00204caa04 /* [RO][32] Inter-core Sempahore Register */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_ACK 0x00204caa08 /* [RO][32] Inter-core Interrupt Acknowledge Register */

/***************************************************************************
 *STATUS - Auxiliary Register STATUS
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: STATUS :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_STATUS_WORD_MASK                     0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_STATUS_WORD_SHIFT                    0

/***************************************************************************
 *SEMAPHORE - Inter-process/Host semaphore register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: SEMAPHORE :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_SEMAPHORE_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_SEMAPHORE_WORD_SHIFT                 0

/***************************************************************************
 *LP_START - Loop start address (32-bit)
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: LP_START :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_LP_START_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_LP_START_WORD_SHIFT                  0

/***************************************************************************
 *LP_END - Loop end address (32-bit)
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: LP_END :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_LP_END_WORD_MASK                     0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_LP_END_WORD_SHIFT                    0

/***************************************************************************
 *IDENTITY - Processor Identification register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: IDENTITY :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_IDENTITY_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_IDENTITY_WORD_SHIFT                  0

/***************************************************************************
 *DEBUG_AVS - Debug register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DEBUG_AVS :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DEBUG_AVS_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DEBUG_AVS_WORD_SHIFT                 0

/***************************************************************************
 *PC - Program Counter register (32-bit)
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: PC :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_PC_WORD_MASK                         0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_PC_WORD_SHIFT                        0

/***************************************************************************
 *STATUS32 - Status register (32-bit)
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: STATUS32 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_WORD_SHIFT                  0

/***************************************************************************
 *STATUS32_L1 - Status register save for level 1 interrupts
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: STATUS32_L1 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L1_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L1_WORD_SHIFT               0

/***************************************************************************
 *STATUS32_L2 - Status register save for level 2 interrupts
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: STATUS32_L2 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L2_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L2_WORD_SHIFT               0

/***************************************************************************
 *AUX_DCCM - Address of Local Data RAM
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_DCCM :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM_WORD_SHIFT                  0

/***************************************************************************
 *TIMER0_COUNT - Processor Timer0 Count value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER0_COUNT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT_WORD_SHIFT              0

/***************************************************************************
 *TIMER0_CONTROL - Processor Timer0 Control value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER0_CONTROL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL_WORD_SHIFT            0

/***************************************************************************
 *TIMER0_LIMIT - Processor Timer0 Limit value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER0_LIMIT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_LIMIT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_LIMIT_WORD_SHIFT              0

/***************************************************************************
 *INT_VEC_BASE - Interrupt Vector Base address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: INT_VEC_BASE :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_INT_VEC_BASE_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_INT_VEC_BASE_WORD_SHIFT              0

/***************************************************************************
 *AUX_IRQ_LV12 - Interrupt Level Status
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IRQ_LV12 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LV12_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LV12_WORD_SHIFT              0

/***************************************************************************
 *CRC_BUILD_BCR - Build configuration register for CRC instruction.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: CRC_BUILD_BCR :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_CRC_BUILD_BCR_WORD_MASK              0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_CRC_BUILD_BCR_WORD_SHIFT             0

/***************************************************************************
 *DVBF_BUILD - Build configuration register for dual viterbi butterfly instruction.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DVBF_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DVBF_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DVBF_BUILD_WORD_SHIFT                0

/***************************************************************************
 *EXT_ARITH_BUILD - Build configuration register to specify that the processor has the extended arithmetic instructions.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: EXT_ARITH_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_EXT_ARITH_BUILD_WORD_MASK            0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_EXT_ARITH_BUILD_WORD_SHIFT           0

/***************************************************************************
 *DATASPACE - Build configuration register for dataspace.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DATASPACE :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DATASPACE_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DATASPACE_WORD_SHIFT                 0

/***************************************************************************
 *MEMSUBSYS - Build configuration register for memory subsytem.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MEMSUBSYS :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MEMSUBSYS_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MEMSUBSYS_WORD_SHIFT                 0

/***************************************************************************
 *VECBASE_AC_BUILD - Build configuration register for ARC600 interrupt vector base address.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: VECBASE_AC_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_VECBASE_AC_BUILD_WORD_MASK           0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_VECBASE_AC_BUILD_WORD_SHIFT          0

/***************************************************************************
 *P_BASE_ADDR - Build configuration register for peripheral base address.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: P_BASE_ADDR :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_P_BASE_ADDR_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_P_BASE_ADDR_WORD_SHIFT               0

/***************************************************************************
 *MPU_BUILD - Build configuration register for memory protection unit.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MPU_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MPU_BUILD_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MPU_BUILD_WORD_SHIFT                 0

/***************************************************************************
 *RF_BUILD - Build configuration register for register file.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: RF_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_RF_BUILD_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_RF_BUILD_WORD_SHIFT                  0

/***************************************************************************
 *D_CACHE_BUILD - Build configuration register for data cache.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: D_CACHE_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_D_CACHE_BUILD_WORD_MASK              0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_D_CACHE_BUILD_WORD_SHIFT             0

/***************************************************************************
 *MADI_BUILD - Build configuration register for multiple ARC debug interface.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MADI_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MADI_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MADI_BUILD_WORD_SHIFT                0

/***************************************************************************
 *DCCM_BUILD - Build configuration register for data closely coupled memory.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DCCM_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DCCM_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DCCM_BUILD_WORD_SHIFT                0

/***************************************************************************
 *TIMER_BUILD - Build configuration register for timers.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER_BUILD_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER_BUILD_WORD_SHIFT               0

/***************************************************************************
 *AP_BUILD - Build configuration register for actionpoints.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AP_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AP_BUILD_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AP_BUILD_WORD_SHIFT                  0

/***************************************************************************
 *I_CACHE_BUILD - Build configuration register for instruction cache.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: I_CACHE_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_I_CACHE_BUILD_WORD_MASK              0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_I_CACHE_BUILD_WORD_SHIFT             0

/***************************************************************************
 *ICCM_BUILD - Build configuration register for instruction closely coupled memory.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ICCM_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ICCM_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ICCM_BUILD_WORD_SHIFT                0

/***************************************************************************
 *DSPRAM_BUILD - Build configuration register for XY memory.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DSPRAM_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DSPRAM_BUILD_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DSPRAM_BUILD_WORD_SHIFT              0

/***************************************************************************
 *MAC_BUILD - Build configuration register for Xmac.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MAC_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MAC_BUILD_WORD_MASK                  0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MAC_BUILD_WORD_SHIFT                 0

/***************************************************************************
 *MULTIPLY_BUILD - Build configuration register for instruction closely coupled memory.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MULTIPLY_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MULTIPLY_BUILD_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MULTIPLY_BUILD_WORD_SHIFT            0

/***************************************************************************
 *SWAP_BUILD - Build configuration register for swap instruction.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: SWAP_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_SWAP_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_SWAP_BUILD_WORD_SHIFT                0

/***************************************************************************
 *NORM_BUILD - Build configuration register for normalise instruction.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: NORM_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_NORM_BUILD_WORD_MASK                 0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_NORM_BUILD_WORD_SHIFT                0

/***************************************************************************
 *MINMAX_BUILD - Build configuration register for min/max instruction.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MINMAX_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MINMAX_BUILD_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MINMAX_BUILD_WORD_SHIFT              0

/***************************************************************************
 *BARREL_BUILD - Build configuration register for barrel shifter.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: BARREL_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_BARREL_BUILD_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_BARREL_BUILD_WORD_SHIFT              0

/***************************************************************************
 *ARC600_BUILD - Build configuration register for ARC 600.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ARC600_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ARC600_BUILD_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ARC600_BUILD_WORD_SHIFT              0

/***************************************************************************
 *AUX_SYSTEM_BUILD - Build configuration register for AS221BD.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_SYSTEM_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_SYSTEM_BUILD_WORD_MASK           0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_SYSTEM_BUILD_WORD_SHIFT          0

/***************************************************************************
 *MCD_BCR - MCD configuration register for AS221BD.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: MCD_BCR :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_MCD_BCR_WORD_MASK                    0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_MCD_BCR_WORD_SHIFT                   0

/***************************************************************************
 *IFETCHQUEUE_BUILD - Build configuration register for the InstructionFetchQueue component.
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: IFETCHQUEUE_BUILD :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_IFETCHQUEUE_BUILD_WORD_MASK          0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_IFETCHQUEUE_BUILD_WORD_SHIFT         0

/***************************************************************************
 *TIMER1_COUNT - Processor Timer 1 Count value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER1_COUNT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT_WORD_SHIFT              0

/***************************************************************************
 *TIMER1_CONTROL - Processor Timer 1 Control value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER1_CONTROL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL_WORD_SHIFT            0

/***************************************************************************
 *TIMER1_LIMIT - Processor Timer 1 Limit value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER1_LIMIT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_LIMIT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_LIMIT_WORD_SHIFT              0

/***************************************************************************
 *AUX_IRQ_LEV - Interrupt Level Programming
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IRQ_LEV :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LEV_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LEV_WORD_SHIFT               0

/***************************************************************************
 *AUX_IRQ_HINT - Software Triggered Interrupt
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IRQ_HINT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_HINT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_HINT_WORD_SHIFT              0

/***************************************************************************
 *AUX_ALIGN_CTRL - Memory Alignment Detection Control
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_ALIGN_CTRL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_CTRL_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_CTRL_WORD_SHIFT            0

/***************************************************************************
 *AUX_INTER_CORE_INTERRUPT - Inter-core Interrupt Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_INTER_CORE_INTERRUPT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_INTERRUPT_WORD_MASK   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_INTERRUPT_WORD_SHIFT  0

/***************************************************************************
 *AX_IPC_SEM_N - Inter-core Sempahore Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AX_IPC_SEM_N :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AX_IPC_SEM_N_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AX_IPC_SEM_N_WORD_SHIFT              0

/***************************************************************************
 *AUX_INTER_CORE_ACK - Inter-core Interrupt Acknowledge Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_INTER_CORE_ACK :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_ACK_WORD_MASK         0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_ACK_WORD_SHIFT        0

#endif /* #ifndef BCHP_AVS_CPU_AUX_REGS_H__ */

/* End of File */
