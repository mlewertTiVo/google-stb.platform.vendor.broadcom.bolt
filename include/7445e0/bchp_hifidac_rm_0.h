/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on               Thu Dec  4 13:59:01 2014
 *                 Full Compile MD5 Checksum  7f8620d6db569529bdb0529f9fa1fcf2
 *                     (minus title and desc)
 *                 MD5 Checksum               48c8fdf2af4291bac0fa2b0d5245d05c
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15262
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_HIFIDAC_RM_0_H__
#define BCHP_HIFIDAC_RM_0_H__

/***************************************************************************
 *HIFIDAC_RM_0 - HiFiDAC Rate Manager Registers
 ***************************************************************************/
#define BCHP_HIFIDAC_RM_0_CONTROL                0x00cb0a00 /* [RW] Rate Manager Controls */
#define BCHP_HIFIDAC_RM_0_RATE_RATIO             0x00cb0a04 /* [RW] Rate Manager Output Rate Setting I */
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC             0x00cb0a08 /* [RW] Rate Manager Output Rate Setting II */
#define BCHP_HIFIDAC_RM_0_PHASE_INC              0x00cb0a0c /* [RW] Rate Manager NCO Phase Increment */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI          0x00cb0a10 /* [WO] Loop Filter Integrator HI Value */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO          0x00cb0a14 /* [WO] Loop Filter Integrator LO Value */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL    0x00cb0a18 /* [RW] Time Base Skip or Repeat Control Register */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP        0x00cb0a1c /* [RW] Time Base Skip or Repeat Gap Count Register */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER     0x00cb0a20 /* [RW] Time Base Skip or Repeat Count Register */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS   0x00cb0a24 /* [RO] Loop Filter Integrator HI Status */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_STATUS   0x00cb0a28 /* [RO] Loop Filter Integrator LO Status */
#define BCHP_HIFIDAC_RM_0_STATUS                 0x00cb0a2c /* [RO] Rate Manager Status Register */
#define BCHP_HIFIDAC_RM_0_STATUS_CLEAR           0x00cb0a30 /* [RW] Rate Manager Status Clear Register */

/***************************************************************************
 *CONTROL - Rate Manager Controls
 ***************************************************************************/
/* HIFIDAC_RM_0 :: CONTROL :: reserved0 [31:22] */
#define BCHP_HIFIDAC_RM_0_CONTROL_reserved0_MASK                   0xffc00000
#define BCHP_HIFIDAC_RM_0_CONTROL_reserved0_SHIFT                  22

/* HIFIDAC_RM_0 :: CONTROL :: READ_INTEGRATOR [21:21] */
#define BCHP_HIFIDAC_RM_0_CONTROL_READ_INTEGRATOR_MASK             0x00200000
#define BCHP_HIFIDAC_RM_0_CONTROL_READ_INTEGRATOR_SHIFT            21
#define BCHP_HIFIDAC_RM_0_CONTROL_READ_INTEGRATOR_DEFAULT          0x00000000

/* HIFIDAC_RM_0 :: CONTROL :: FREEZE_COUNTERS [20:20] */
#define BCHP_HIFIDAC_RM_0_CONTROL_FREEZE_COUNTERS_MASK             0x00100000
#define BCHP_HIFIDAC_RM_0_CONTROL_FREEZE_COUNTERS_SHIFT            20
#define BCHP_HIFIDAC_RM_0_CONTROL_FREEZE_COUNTERS_DEFAULT          0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_FREEZE_COUNTERS_ENABLE           1
#define BCHP_HIFIDAC_RM_0_CONTROL_FREEZE_COUNTERS_DISABLE          0

/* HIFIDAC_RM_0 :: CONTROL :: EN_FREEZE_COUNTERS [19:19] */
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_FREEZE_COUNTERS_MASK          0x00080000
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_FREEZE_COUNTERS_SHIFT         19
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_FREEZE_COUNTERS_DEFAULT       0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_FREEZE_COUNTERS_ENABLE        1
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_FREEZE_COUNTERS_DISABLE       0

/* HIFIDAC_RM_0 :: CONTROL :: LOAD_INTEGRATOR [18:18] */
#define BCHP_HIFIDAC_RM_0_CONTROL_LOAD_INTEGRATOR_MASK             0x00040000
#define BCHP_HIFIDAC_RM_0_CONTROL_LOAD_INTEGRATOR_SHIFT            18
#define BCHP_HIFIDAC_RM_0_CONTROL_LOAD_INTEGRATOR_DEFAULT          0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_LOAD_INTEGRATOR_ENABLE           1
#define BCHP_HIFIDAC_RM_0_CONTROL_LOAD_INTEGRATOR_DISABLE          0

/* HIFIDAC_RM_0 :: CONTROL :: EN_LOAD_INTEGRATOR [17:17] */
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_LOAD_INTEGRATOR_MASK          0x00020000
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_LOAD_INTEGRATOR_SHIFT         17
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_LOAD_INTEGRATOR_DEFAULT       0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_LOAD_INTEGRATOR_ENABLE        1
#define BCHP_HIFIDAC_RM_0_CONTROL_EN_LOAD_INTEGRATOR_DISABLE       0

/* HIFIDAC_RM_0 :: CONTROL :: DISABLE_FREQUENCY_CONFIG [16:16] */
#define BCHP_HIFIDAC_RM_0_CONTROL_DISABLE_FREQUENCY_CONFIG_MASK    0x00010000
#define BCHP_HIFIDAC_RM_0_CONTROL_DISABLE_FREQUENCY_CONFIG_SHIFT   16
#define BCHP_HIFIDAC_RM_0_CONTROL_DISABLE_FREQUENCY_CONFIG_DEFAULT 0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_DISABLE_FREQUENCY_CONFIG_ENABLE  0
#define BCHP_HIFIDAC_RM_0_CONTROL_DISABLE_FREQUENCY_CONFIG_DISABLE 1

/* HIFIDAC_RM_0 :: CONTROL :: TRACKING_RANGE [15:13] */
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_MASK              0x0000e000
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_SHIFT             13
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_DEFAULT           0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_15616     6
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_7808      5
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_3904      4
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_1952      3
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_976       2
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_488       1
#define BCHP_HIFIDAC_RM_0_CONTROL_TRACKING_RANGE_ZERO_to_244       0

/* HIFIDAC_RM_0 :: CONTROL :: RESET [12:12] */
#define BCHP_HIFIDAC_RM_0_CONTROL_RESET_MASK                       0x00001000
#define BCHP_HIFIDAC_RM_0_CONTROL_RESET_SHIFT                      12
#define BCHP_HIFIDAC_RM_0_CONTROL_RESET_DEFAULT                    0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_RESET_RESET_ON                   1
#define BCHP_HIFIDAC_RM_0_CONTROL_RESET_RESET_OFF                  0

/* HIFIDAC_RM_0 :: CONTROL :: INT_GAIN [11:09] */
#define BCHP_HIFIDAC_RM_0_CONTROL_INT_GAIN_MASK                    0x00000e00
#define BCHP_HIFIDAC_RM_0_CONTROL_INT_GAIN_SHIFT                   9
#define BCHP_HIFIDAC_RM_0_CONTROL_INT_GAIN_DEFAULT                 0x00000004

/* HIFIDAC_RM_0 :: CONTROL :: DIRECT_GAIN [08:06] */
#define BCHP_HIFIDAC_RM_0_CONTROL_DIRECT_GAIN_MASK                 0x000001c0
#define BCHP_HIFIDAC_RM_0_CONTROL_DIRECT_GAIN_SHIFT                6
#define BCHP_HIFIDAC_RM_0_CONTROL_DIRECT_GAIN_DEFAULT              0x00000004

/* HIFIDAC_RM_0 :: CONTROL :: DITHER [05:05] */
#define BCHP_HIFIDAC_RM_0_CONTROL_DITHER_MASK                      0x00000020
#define BCHP_HIFIDAC_RM_0_CONTROL_DITHER_SHIFT                     5
#define BCHP_HIFIDAC_RM_0_CONTROL_DITHER_DEFAULT                   0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_DITHER_DITHER_ON                 1
#define BCHP_HIFIDAC_RM_0_CONTROL_DITHER_DITHER_OFF                0

/* HIFIDAC_RM_0 :: CONTROL :: FREE_RUN [04:04] */
#define BCHP_HIFIDAC_RM_0_CONTROL_FREE_RUN_MASK                    0x00000010
#define BCHP_HIFIDAC_RM_0_CONTROL_FREE_RUN_SHIFT                   4
#define BCHP_HIFIDAC_RM_0_CONTROL_FREE_RUN_DEFAULT                 0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_FREE_RUN_FREE_RUN_ON             1
#define BCHP_HIFIDAC_RM_0_CONTROL_FREE_RUN_TIMEBASE                0

/* HIFIDAC_RM_0 :: CONTROL :: TIMEBASE [03:00] */
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_MASK                    0x0000000f
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_SHIFT                   0
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_DEFAULT                 0x00000000
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_15             15
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_14             14
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_13             13
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_12             12
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_11             11
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_10             10
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_9              9
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_8              8
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_7              7
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_6              6
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_5              5
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_4              4
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_3              3
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_2              2
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_1              1
#define BCHP_HIFIDAC_RM_0_CONTROL_TIMEBASE_TIMEBASE_0              0

/***************************************************************************
 *RATE_RATIO - Rate Manager Output Rate Setting I
 ***************************************************************************/
/* HIFIDAC_RM_0 :: RATE_RATIO :: reserved0 [31:24] */
#define BCHP_HIFIDAC_RM_0_RATE_RATIO_reserved0_MASK                0xff000000
#define BCHP_HIFIDAC_RM_0_RATE_RATIO_reserved0_SHIFT               24

/* HIFIDAC_RM_0 :: RATE_RATIO :: DENOMINATOR [23:00] */
#define BCHP_HIFIDAC_RM_0_RATE_RATIO_DENOMINATOR_MASK              0x00ffffff
#define BCHP_HIFIDAC_RM_0_RATE_RATIO_DENOMINATOR_SHIFT             0
#define BCHP_HIFIDAC_RM_0_RATE_RATIO_DENOMINATOR_DEFAULT           0x00000001

/***************************************************************************
 *SAMPLE_INC - Rate Manager Output Rate Setting II
 ***************************************************************************/
/* HIFIDAC_RM_0 :: SAMPLE_INC :: NUMERATOR [31:08] */
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_NUMERATOR_MASK                0xffffff00
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_NUMERATOR_SHIFT               8
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_NUMERATOR_DEFAULT             0x00000000

/* HIFIDAC_RM_0 :: SAMPLE_INC :: SAMPLE_INC [07:00] */
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_SAMPLE_INC_MASK               0x000000ff
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_SAMPLE_INC_SHIFT              0
#define BCHP_HIFIDAC_RM_0_SAMPLE_INC_SAMPLE_INC_DEFAULT            0x00000001

/***************************************************************************
 *PHASE_INC - Rate Manager NCO Phase Increment
 ***************************************************************************/
/* HIFIDAC_RM_0 :: PHASE_INC :: reserved0 [31:24] */
#define BCHP_HIFIDAC_RM_0_PHASE_INC_reserved0_MASK                 0xff000000
#define BCHP_HIFIDAC_RM_0_PHASE_INC_reserved0_SHIFT                24

/* HIFIDAC_RM_0 :: PHASE_INC :: PHASE_INC [23:00] */
#define BCHP_HIFIDAC_RM_0_PHASE_INC_PHASE_INC_MASK                 0x00ffffff
#define BCHP_HIFIDAC_RM_0_PHASE_INC_PHASE_INC_SHIFT                0
#define BCHP_HIFIDAC_RM_0_PHASE_INC_PHASE_INC_DEFAULT              0x00000000

/***************************************************************************
 *INTEGRATOR_HI - Loop Filter Integrator HI Value
 ***************************************************************************/
/* HIFIDAC_RM_0 :: INTEGRATOR_HI :: reserved0 [31:05] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_reserved0_MASK             0xffffffe0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_reserved0_SHIFT            5

/* HIFIDAC_RM_0 :: INTEGRATOR_HI :: INTEGRATOR_HI [04:00] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_INTEGRATOR_HI_MASK         0x0000001f
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_INTEGRATOR_HI_SHIFT        0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_INTEGRATOR_HI_DEFAULT      0x00000000

/***************************************************************************
 *INTEGRATOR_LO - Loop Filter Integrator LO Value
 ***************************************************************************/
/* HIFIDAC_RM_0 :: INTEGRATOR_LO :: INTEGRATOR_LO [31:00] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_INTEGRATOR_LO_MASK         0xffffffff
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_INTEGRATOR_LO_SHIFT        0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_INTEGRATOR_LO_DEFAULT      0x00000000

/***************************************************************************
 *SKIP_REPEAT_CONTROL - Time Base Skip or Repeat Control Register
 ***************************************************************************/
/* HIFIDAC_RM_0 :: SKIP_REPEAT_CONTROL :: reserved0 [31:03] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_reserved0_MASK       0xfffffff8
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_reserved0_SHIFT      3

/* HIFIDAC_RM_0 :: SKIP_REPEAT_CONTROL :: SKIP_REPEAT_FOREVER [02:02] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_SKIP_REPEAT_FOREVER_MASK 0x00000004
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_SKIP_REPEAT_FOREVER_SHIFT 2
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_SKIP_REPEAT_FOREVER_DEFAULT 0x00000000
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_SKIP_REPEAT_FOREVER_ENABLE 1
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_SKIP_REPEAT_FOREVER_DISABLE 0

/* HIFIDAC_RM_0 :: SKIP_REPEAT_CONTROL :: MODE [01:01] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_MODE_MASK            0x00000002
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_MODE_SHIFT           1
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_MODE_DEFAULT         0x00000000
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_MODE_SKIP            1
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_MODE_REPEAT          0

/* HIFIDAC_RM_0 :: SKIP_REPEAT_CONTROL :: ENABLE [00:00] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_ENABLE_MASK          0x00000001
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_ENABLE_SHIFT         0
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_CONTROL_ENABLE_DEFAULT       0x00000000

/***************************************************************************
 *SKIP_REPEAT_GAP - Time Base Skip or Repeat Gap Count Register
 ***************************************************************************/
/* HIFIDAC_RM_0 :: SKIP_REPEAT_GAP :: reserved0 [31:20] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP_reserved0_MASK           0xfff00000
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP_reserved0_SHIFT          20

/* HIFIDAC_RM_0 :: SKIP_REPEAT_GAP :: GAP_COUNT [19:00] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP_GAP_COUNT_MASK           0x000fffff
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP_GAP_COUNT_SHIFT          0
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_GAP_GAP_COUNT_DEFAULT        0x00000000

/***************************************************************************
 *SKIP_REPEAT_NUMBER - Time Base Skip or Repeat Count Register
 ***************************************************************************/
/* HIFIDAC_RM_0 :: SKIP_REPEAT_NUMBER :: reserved0 [31:20] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER_reserved0_MASK        0xfff00000
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER_reserved0_SHIFT       20

/* HIFIDAC_RM_0 :: SKIP_REPEAT_NUMBER :: COUNT [19:00] */
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER_COUNT_MASK            0x000fffff
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER_COUNT_SHIFT           0
#define BCHP_HIFIDAC_RM_0_SKIP_REPEAT_NUMBER_COUNT_DEFAULT         0x00000000

/***************************************************************************
 *INTEGRATOR_HI_STATUS - Loop Filter Integrator HI Status
 ***************************************************************************/
/* HIFIDAC_RM_0 :: INTEGRATOR_HI_STATUS :: reserved0 [31:05] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS_reserved0_MASK      0xffffffe0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS_reserved0_SHIFT     5

/* HIFIDAC_RM_0 :: INTEGRATOR_HI_STATUS :: INTEGRATOR_HI_STATUS [04:00] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS_INTEGRATOR_HI_STATUS_MASK 0x0000001f
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS_INTEGRATOR_HI_STATUS_SHIFT 0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_HI_STATUS_INTEGRATOR_HI_STATUS_DEFAULT 0x00000000

/***************************************************************************
 *INTEGRATOR_LO_STATUS - Loop Filter Integrator LO Status
 ***************************************************************************/
/* HIFIDAC_RM_0 :: INTEGRATOR_LO_STATUS :: INTEGRATOR_LO_STATUS [31:00] */
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_STATUS_INTEGRATOR_LO_STATUS_MASK 0xffffffff
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_STATUS_INTEGRATOR_LO_STATUS_SHIFT 0
#define BCHP_HIFIDAC_RM_0_INTEGRATOR_LO_STATUS_INTEGRATOR_LO_STATUS_DEFAULT 0x00000000

/***************************************************************************
 *STATUS - Rate Manager Status Register
 ***************************************************************************/
/* HIFIDAC_RM_0 :: STATUS :: reserved0 [31:29] */
#define BCHP_HIFIDAC_RM_0_STATUS_reserved0_MASK                    0xe0000000
#define BCHP_HIFIDAC_RM_0_STATUS_reserved0_SHIFT                   29

/* HIFIDAC_RM_0 :: STATUS :: MAX_ERR [28:15] */
#define BCHP_HIFIDAC_RM_0_STATUS_MAX_ERR_MASK                      0x1fff8000
#define BCHP_HIFIDAC_RM_0_STATUS_MAX_ERR_SHIFT                     15
#define BCHP_HIFIDAC_RM_0_STATUS_MAX_ERR_DEFAULT                   0x00000000

/* HIFIDAC_RM_0 :: STATUS :: MIN_ERR [14:01] */
#define BCHP_HIFIDAC_RM_0_STATUS_MIN_ERR_MASK                      0x00007ffe
#define BCHP_HIFIDAC_RM_0_STATUS_MIN_ERR_SHIFT                     1
#define BCHP_HIFIDAC_RM_0_STATUS_MIN_ERR_DEFAULT                   0x00000000

/* HIFIDAC_RM_0 :: STATUS :: RM_UNLOCK [00:00] */
#define BCHP_HIFIDAC_RM_0_STATUS_RM_UNLOCK_MASK                    0x00000001
#define BCHP_HIFIDAC_RM_0_STATUS_RM_UNLOCK_SHIFT                   0
#define BCHP_HIFIDAC_RM_0_STATUS_RM_UNLOCK_LOCK                    0
#define BCHP_HIFIDAC_RM_0_STATUS_RM_UNLOCK_UNLOCK                  1

/***************************************************************************
 *STATUS_CLEAR - Rate Manager Status Clear Register
 ***************************************************************************/
/* HIFIDAC_RM_0 :: STATUS_CLEAR :: reserved0 [31:01] */
#define BCHP_HIFIDAC_RM_0_STATUS_CLEAR_reserved0_MASK              0xfffffffe
#define BCHP_HIFIDAC_RM_0_STATUS_CLEAR_reserved0_SHIFT             1

/* HIFIDAC_RM_0 :: STATUS_CLEAR :: RM_UNLOCK_CLR [00:00] */
#define BCHP_HIFIDAC_RM_0_STATUS_CLEAR_RM_UNLOCK_CLR_MASK          0x00000001
#define BCHP_HIFIDAC_RM_0_STATUS_CLEAR_RM_UNLOCK_CLR_SHIFT         0

#endif /* #ifndef BCHP_HIFIDAC_RM_0_H__ */

/* End of File */
