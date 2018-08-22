/***************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Sun Jul  6 03:06:55 2014
 *                 Full Compile MD5 Checksum fe5bf937ded6451208bcd12a03dadcff
 *                   (minus title and desc)  
 *                 MD5 Checksum              e67548d0bfc4c43233ca60dd5a6dc076
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_AVS_CPU_AUX_REGS_H__
#define BCHP_AVS_CPU_AUX_REGS_H__

/***************************************************************************
 *AVS_CPU_AUX_REGS - CPU Auxiliary Registers
 ***************************************************************************/
#define BCHP_AVS_CPU_AUX_REGS_STATUS             0x004ca000 /* Auxiliary Register STATUS */
#define BCHP_AVS_CPU_AUX_REGS_SEMAPHORE          0x004ca004 /* Inter-process/Host semaphore register */
#define BCHP_AVS_CPU_AUX_REGS_LP_START           0x004ca008 /* Loop start address (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_LP_END             0x004ca00c /* Loop end address (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_IDENTITY           0x004ca010 /* Processor Identification register */
#define BCHP_AVS_CPU_AUX_REGS_DEBUG              0x004ca014 /* Debug register */
#define BCHP_AVS_CPU_AUX_REGS_PC                 0x004ca018 /* Program Counter register (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32           0x004ca028 /* Status register (32-bit) */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L1        0x004ca02c /* Status register save for level 1 interrupts */
#define BCHP_AVS_CPU_AUX_REGS_STATUS32_L2        0x004ca030 /* Status register save for level 2 interrupts */
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM           0x004ca060 /* Status register save for level 2 interrupts */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL     0x004ca084 /* Processor Timer0 Count value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT       0x004ca088 /* Processor Timer0 Control value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_LIMIT       0x004ca08c /* Processor Timer0 Limit value */
#define BCHP_AVS_CPU_AUX_REGS_INT_VEC_BASE       0x004ca094 /* Interrupt Vector Base address */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LV12       0x004ca10c /* Interrupt Level Status */
#define BCHP_AVS_CPU_AUX_REGS_CRC_BUILD_BCR      0x004ca188 /* Build configuration register for CRC instruction. */
#define BCHP_AVS_CPU_AUX_REGS_DVBF_BUILD         0x004ca190 /* Build configuration register for dual viterbi butterfly instruction. */
#define BCHP_AVS_CPU_AUX_REGS_EXT_ARITH_BUILD    0x004ca194 /* Build configuration register to specify that the processor has the extended arithmetic instructions. */
#define BCHP_AVS_CPU_AUX_REGS_DATASPACE          0x004ca198 /* Build configuration register for dataspace. */
#define BCHP_AVS_CPU_AUX_REGS_MEMSUBSYS          0x004ca19c /* Build configuration register for memory subsytem. */
#define BCHP_AVS_CPU_AUX_REGS_VECBASE_AC_BUILD   0x004ca1a0 /* Build configuration register for ARC600 interrupt vector base address. */
#define BCHP_AVS_CPU_AUX_REGS_P_BASE_ADDR        0x004ca1a4 /* Build configuration register for peripheral base address. */
#define BCHP_AVS_CPU_AUX_REGS_MPU_BUILD          0x004ca1b4 /* Build configuration register for memory protection unit. */
#define BCHP_AVS_CPU_AUX_REGS_RF_BUILD           0x004ca1b8 /* Build configuration register for register file. */
#define BCHP_AVS_CPU_AUX_REGS_D_CACHE_BUILD      0x004ca1c8 /* Build configuration register for data cache. */
#define BCHP_AVS_CPU_AUX_REGS_MADI_BUILD         0x004ca1cc /* Build configuration register for multiple ARC debug interface. */
#define BCHP_AVS_CPU_AUX_REGS_DCCM_BUILD         0x004ca1d0 /* Build configuration register for data closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_TIMER_BUILD        0x004ca1d4 /* Build configuration register for timers. */
#define BCHP_AVS_CPU_AUX_REGS_AP_BUILD           0x004ca1d8 /* Build configuration register for actionpoints. */
#define BCHP_AVS_CPU_AUX_REGS_I_CACHE_BUILD      0x004ca1dc /* Build configuration register for instruction cache. */
#define BCHP_AVS_CPU_AUX_REGS_ICCM_BUILD         0x004ca1e0 /* Build configuration register for instruction closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_DSPRAM_BUILD       0x004ca1e4 /* Build configuration register for XY memory. */
#define BCHP_AVS_CPU_AUX_REGS_MAC_BUILD          0x004ca1e8 /* Build configuration register for Xmac. */
#define BCHP_AVS_CPU_AUX_REGS_MULTIPLY_BUILD     0x004ca1ec /* Build configuration register for instruction closely coupled memory. */
#define BCHP_AVS_CPU_AUX_REGS_SWAP_BUILD         0x004ca1f0 /* Build configuration register for swap instruction. */
#define BCHP_AVS_CPU_AUX_REGS_NORM_BUILD         0x004ca1f4 /* Build configuration register for normalise instruction. */
#define BCHP_AVS_CPU_AUX_REGS_MINMAX_BUILD       0x004ca1f8 /* Build configuration register for min/max instruction. */
#define BCHP_AVS_CPU_AUX_REGS_BARREL_BUILD       0x004ca1fc /* Build configuration register for barrel shifter. */
#define BCHP_AVS_CPU_AUX_REGS_ARC600_BUILD       0x004ca304 /* Build configuration register for ARC 600. */
#define BCHP_AVS_CPU_AUX_REGS_AUX_SYSTEM_BUILD   0x004ca308 /* Build configuration register for AS221BD. */
#define BCHP_AVS_CPU_AUX_REGS_MCD_BCR            0x004ca310 /* MCD configuration register for AS221BD. */
#define BCHP_AVS_CPU_AUX_REGS_IFETCHQUEUE_BUILD  0x004ca3f8 /* Build configuration register for the InstructionFetchQueue component. */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL     0x004ca400 /* Processor Timer 1 Count value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT       0x004ca404 /* Processor Timer 1 Control value */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_LIMIT       0x004ca408 /* Processor Timer 1 Limit value */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_LEV        0x004ca800 /* Interrupt Level Programming */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_HINT       0x004ca804 /* Software Triggered Interrupt */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_CTRL     0x004ca808 /* Memory Alignment Detection Control */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_ADDR     0x004ca80c /* Memory Alignment Detected Address */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_SIZE     0x004ca810 /* Memory Alignment Detected Size */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_INTERRUPT 0x004caa00 /* Inter-core Interrupt Register */
#define BCHP_AVS_CPU_AUX_REGS_AX_IPC_SEM_N       0x004caa04 /* Inter-core Sempahore Register */
#define BCHP_AVS_CPU_AUX_REGS_AUX_INTER_CORE_ACK 0x004caa08 /* Inter-core Interrupt Acknowledge Register */
#define BCHP_AVS_CPU_AUX_REGS_ERET               0x004cb000 /* Exception Return Address */
#define BCHP_AVS_CPU_AUX_REGS_ERBTA              0x004cb004 /* Exception Return Branch Target Address */
#define BCHP_AVS_CPU_AUX_REGS_ERSTATUS           0x004cb008 /* Exception Return Status */
#define BCHP_AVS_CPU_AUX_REGS_ECR                0x004cb00c /* Exception Cause Register */
#define BCHP_AVS_CPU_AUX_REGS_EFA                0x004cb010 /* Exception Fault Address */
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE1            0x004cb028 /* Level 1 Interrupt Cause Register */
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE2            0x004cb02c /* Level 2 Interrupt Cause Register */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IENABLE        0x004cb030 /* Interrupt Mask Programming */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ITRIGGER       0x004cb034 /* Interrupt Sensitivity Programming */
#define BCHP_AVS_CPU_AUX_REGS_BTA                0x004cb048 /* Branch Target Address */
#define BCHP_AVS_CPU_AUX_REGS_BTA_L1             0x004cb04c /* Level 1 Return Branch Target */
#define BCHP_AVS_CPU_AUX_REGS_BTA_L2             0x004cb050 /* Level 2 Return Branch Target */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PULSE_CANCEL 0x004cb054 /* Interrupt Pulse Cancel */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PENDING    0x004cb058 /* Interrupt Pending Register */

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
 *DEBUG - Debug register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: DEBUG :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_DEBUG_WORD_MASK                      0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_DEBUG_WORD_SHIFT                     0

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
 *AUX_DCCM - Status register save for level 2 interrupts
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_DCCM :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_DCCM_WORD_SHIFT                  0

/***************************************************************************
 *TIMER0_CONTROL - Processor Timer0 Count value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER0_CONTROL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_CONTROL_WORD_SHIFT            0

/***************************************************************************
 *TIMER0_COUNT - Processor Timer0 Control value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER0_COUNT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER0_COUNT_WORD_SHIFT              0

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
 *TIMER1_CONTROL - Processor Timer 1 Count value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER1_CONTROL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_CONTROL_WORD_SHIFT            0

/***************************************************************************
 *TIMER1_COUNT - Processor Timer 1 Control value
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: TIMER1_COUNT :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_TIMER1_COUNT_WORD_SHIFT              0

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
 *AUX_ALIGN_ADDR - Memory Alignment Detected Address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_ALIGN_ADDR :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_ADDR_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_ADDR_WORD_SHIFT            0

/***************************************************************************
 *AUX_ALIGN_SIZE - Memory Alignment Detected Size
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_ALIGN_SIZE :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_SIZE_WORD_MASK             0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_ALIGN_SIZE_WORD_SHIFT            0

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

/***************************************************************************
 *ERET - Exception Return Address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ERET :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ERET_WORD_MASK                       0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ERET_WORD_SHIFT                      0

/***************************************************************************
 *ERBTA - Exception Return Branch Target Address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ERBTA :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ERBTA_WORD_MASK                      0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ERBTA_WORD_SHIFT                     0

/***************************************************************************
 *ERSTATUS - Exception Return Status
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ERSTATUS :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ERSTATUS_WORD_MASK                   0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ERSTATUS_WORD_SHIFT                  0

/***************************************************************************
 *ECR - Exception Cause Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ECR :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ECR_WORD_MASK                        0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ECR_WORD_SHIFT                       0

/***************************************************************************
 *EFA - Exception Fault Address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: EFA :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_EFA_WORD_MASK                        0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_EFA_WORD_SHIFT                       0

/***************************************************************************
 *ICAUSE1 - Level 1 Interrupt Cause Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ICAUSE1 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE1_WORD_MASK                    0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE1_WORD_SHIFT                   0

/***************************************************************************
 *ICAUSE2 - Level 2 Interrupt Cause Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: ICAUSE2 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE2_WORD_MASK                    0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_ICAUSE2_WORD_SHIFT                   0

/***************************************************************************
 *AUX_IENABLE - Interrupt Mask Programming
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IENABLE :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IENABLE_WORD_MASK                0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IENABLE_WORD_SHIFT               0

/***************************************************************************
 *AUX_ITRIGGER - Interrupt Sensitivity Programming
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_ITRIGGER :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_ITRIGGER_WORD_MASK               0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_ITRIGGER_WORD_SHIFT              0

/***************************************************************************
 *BTA - Branch Target Address
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: BTA :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_BTA_WORD_MASK                        0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_BTA_WORD_SHIFT                       0

/***************************************************************************
 *BTA_L1 - Level 1 Return Branch Target
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: BTA_L1 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_BTA_L1_WORD_MASK                     0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_BTA_L1_WORD_SHIFT                    0

/***************************************************************************
 *BTA_L2 - Level 2 Return Branch Target
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: BTA_L2 :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_BTA_L2_WORD_MASK                     0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_BTA_L2_WORD_SHIFT                    0

/***************************************************************************
 *AUX_IRQ_PULSE_CANCEL - Interrupt Pulse Cancel
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IRQ_PULSE_CANCEL :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PULSE_CANCEL_WORD_MASK       0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PULSE_CANCEL_WORD_SHIFT      0

/***************************************************************************
 *AUX_IRQ_PENDING - Interrupt Pending Register
 ***************************************************************************/
/* AVS_CPU_AUX_REGS :: AUX_IRQ_PENDING :: WORD [31:00] */
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PENDING_WORD_MASK            0xffffffff
#define BCHP_AVS_CPU_AUX_REGS_AUX_IRQ_PENDING_WORD_SHIFT           0

#endif /* #ifndef BCHP_AVS_CPU_AUX_REGS_H__ */

/* End of File */
