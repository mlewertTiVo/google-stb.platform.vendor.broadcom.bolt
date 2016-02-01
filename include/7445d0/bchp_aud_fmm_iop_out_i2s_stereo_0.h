/***************************************************************************
 *     Copyright (c) 1999-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Mon Sep 23 09:50:37 2013
 *                 Full Compile MD5 Checksum fcccce298b546dd6a1f4cbad288478da
 *                   (minus title and desc)  
 *                 MD5 Checksum              211556602e37a33262598b3d5eeba81c
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

#ifndef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_H__
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_H__

/***************************************************************************
 *AUD_FMM_IOP_OUT_I2S_STEREO_0
 ***************************************************************************/
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0 0x00cb0000 /* Stream configuration */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG 0x00cb0020 /* I2S formatter configuration */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR 0x00cb0024 /* I2S crossbar control */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0 0x00cb0030 /* I2S MCLK configuration */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS 0x00cb0070 /* Error Status Register */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET 0x00cb0074 /* Error Set Register */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR 0x00cb0078 /* Error Clear Register */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK 0x00cb007c /* Mask Status Register */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET 0x00cb0080 /* Mask Set Register */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR 0x00cb0084 /* Mask Clear Register */

/***************************************************************************
 *STREAM_CFG_0 - Stream configuration
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: ENA [31:31] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_ENA_MASK    0x80000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_ENA_SHIFT   31
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_ENA_DEFAULT 0x00000000

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: reserved0 [30:28] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_reserved0_MASK 0x70000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_reserved0_SHIFT 28

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: CHANNEL_GROUPING [27:24] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_CHANNEL_GROUPING_MASK 0x0f000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_CHANNEL_GROUPING_SHIFT 24
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_CHANNEL_GROUPING_DEFAULT 0x00000001

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: GROUP_ID [23:20] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_MASK 0x00f00000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_SHIFT 20
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_DEFAULT 0x00000000

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: STREAM_BIT_RESOLUTION [19:16] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_MASK 0x000f0000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_SHIFT 16
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_DEFAULT 0x00000008
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_16_Bit 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_17_Bit 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_18_Bit 2
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_19_Bit 3
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_20_Bit 4
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_21_Bit 5
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_22_Bit 6
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_23_Bit 7
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_STREAM_BIT_RESOLUTION_Res_24_Bit 8

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: WAIT_FOR_VALID [15:15] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_WAIT_FOR_VALID_MASK 0x00008000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_WAIT_FOR_VALID_SHIFT 15
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_WAIT_FOR_VALID_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_WAIT_FOR_VALID_Holdoff_request 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_WAIT_FOR_VALID_Keep_requesting 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: IGNORE_FIRST_UNDERFLOW [14:14] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_IGNORE_FIRST_UNDERFLOW_MASK 0x00004000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_IGNORE_FIRST_UNDERFLOW_SHIFT 14
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_IGNORE_FIRST_UNDERFLOW_DEFAULT 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_IGNORE_FIRST_UNDERFLOW_Ignore 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_IGNORE_FIRST_UNDERFLOW_Dont_ignore 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: INIT_SM [13:13] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INIT_SM_MASK 0x00002000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INIT_SM_SHIFT 13
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INIT_SM_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INIT_SM_Init 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INIT_SM_Normal 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: INS_INVAL [12:12] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INS_INVAL_MASK 0x00001000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INS_INVAL_SHIFT 12
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INS_INVAL_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INS_INVAL_Invalid 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_INS_INVAL_Valid 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: reserved1 [11:10] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_reserved1_MASK 0x00000c00
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_reserved1_SHIFT 10

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: STREAM_CFG_0 :: FCI_ID [09:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_FCI_ID_MASK 0x000003ff
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_FCI_ID_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_FCI_ID_DEFAULT 0x000003ff

/***************************************************************************
 *I2S_CFG - I2S formatter configuration
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: reserved0 [31:24] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved0_MASK   0xff000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved0_SHIFT  24

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: DATA_JUSTIFICATION [23:23] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_JUSTIFICATION_MASK 0x00800000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_JUSTIFICATION_SHIFT 23
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_JUSTIFICATION_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_JUSTIFICATION_LSB 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_JUSTIFICATION_MSB 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: DATA_ALIGNMENT [22:22] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ALIGNMENT_MASK 0x00400000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ALIGNMENT_SHIFT 22
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ALIGNMENT_DEFAULT 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ALIGNMENT_Delayed 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ALIGNMENT_Aligned 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: SCLK_POLARITY [21:21] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLK_POLARITY_MASK 0x00200000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLK_POLARITY_SHIFT 21
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLK_POLARITY_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLK_POLARITY_Rising_aligned_with_sdata 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLK_POLARITY_Falling_aligned_with_sdata 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: LRCK_POLARITY [20:20] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_LRCK_POLARITY_MASK 0x00100000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_LRCK_POLARITY_SHIFT 20
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_LRCK_POLARITY_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_LRCK_POLARITY_High_for_left 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_LRCK_POLARITY_Low_for_left 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: SCLKS_PER_1FS_DIV32 [19:16] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLKS_PER_1FS_DIV32_MASK 0x000f0000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLKS_PER_1FS_DIV32_SHIFT 16
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_SCLKS_PER_1FS_DIV32_DEFAULT 0x00000002

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: reserved1 [15:13] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved1_MASK   0x0000e000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved1_SHIFT  13

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: BITS_PER_SAMPLE [12:08] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_MASK 0x00001f00
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_SHIFT 8
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_Bitwidth32 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_Bitwidth24 24
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_Bitwidth20 20
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_Bitwidth18 18
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_BITS_PER_SAMPLE_Bitwidth16 16

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: reserved2 [07:02] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved2_MASK   0x000000fc
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_reserved2_SHIFT  2

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: DATA_ENABLE [01:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ENABLE_MASK 0x00000002
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ENABLE_SHIFT 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ENABLE_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ENABLE_Enable 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_DATA_ENABLE_Disable 0

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CFG :: CLOCK_ENABLE [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_CLOCK_ENABLE_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_CLOCK_ENABLE_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_CLOCK_ENABLE_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_CLOCK_ENABLE_Enable 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG_CLOCK_ENABLE_Disable 0

/***************************************************************************
 *I2S_CROSSBAR - I2S crossbar control
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CROSSBAR :: reserved0 [31:05] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_reserved0_MASK 0xffffffe0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_reserved0_SHIFT 5

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CROSSBAR :: OUT_R [04:04] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_R_MASK  0x00000010
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_R_SHIFT 4
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_R_DEFAULT 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_R_In_l  0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_R_In_r  1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CROSSBAR :: reserved1 [03:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_reserved1_MASK 0x0000000e
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_reserved1_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: I2S_CROSSBAR :: OUT_L [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_L_MASK  0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_L_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_L_DEFAULT 0x00000000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_L_In_l  0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR_OUT_L_In_r  1

/***************************************************************************
 *MCLK_CFG_0 - I2S MCLK configuration
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: MCLK_CFG_0 :: reserved0 [31:20] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_reserved0_MASK 0xfff00000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_reserved0_SHIFT 20

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: MCLK_CFG_0 :: MCLK_RATE [19:16] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_MASK 0x000f0000
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_SHIFT 16
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_DEFAULT 0x00000002
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_MCLK_512fs_SCLK_64fs 4
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_MCLK_384fs_SCLK_64fs 3
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_MCLK_256fs_SCLK_64fs 2
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_MCLK_RATE_MCLK_128fs_SCLK_64fs 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: MCLK_CFG_0 :: reserved1 [15:04] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_reserved1_MASK 0x0000fff0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_reserved1_SHIFT 4

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: MCLK_CFG_0 :: PLLCLKSEL [03:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_MASK 0x0000000f
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_DEFAULT 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch1 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch2 1
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch3 2
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch1 3
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch2 4
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch3 5
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch1 6
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch2 7
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch3 8
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen0 9
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen1 10
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen2 11
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen3 12
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen4 13
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen5 14
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen6 15

/***************************************************************************
 *ESR_STATUS - Error Status Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_reserved0_MASK 0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_STREAM_UNDERFLOW_DEFAULT 0x00000000

/***************************************************************************
 *ESR_STATUS_SET - Error Set Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS_SET :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET_reserved0_MASK 0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS_SET :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_SET_STREAM_UNDERFLOW_DEFAULT 0x00000000

/***************************************************************************
 *ESR_STATUS_CLEAR - Error Clear Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS_CLEAR :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_STATUS_CLEAR :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_STATUS_CLEAR_STREAM_UNDERFLOW_DEFAULT 0x00000000

/***************************************************************************
 *ESR_MASK - Mask Status Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_reserved0_MASK  0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_STREAM_UNDERFLOW_DEFAULT 0x00000001

/***************************************************************************
 *ESR_MASK_SET - Mask Set Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK_SET :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET_reserved0_MASK 0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK_SET :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_SET_STREAM_UNDERFLOW_DEFAULT 0x00000001

/***************************************************************************
 *ESR_MASK_CLEAR - Mask Clear Register
 ***************************************************************************/
/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK_CLEAR :: reserved0 [31:01] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR_reserved0_MASK 0xfffffffe
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR_reserved0_SHIFT 1

/* AUD_FMM_IOP_OUT_I2S_STEREO_0 :: ESR_MASK_CLEAR :: STREAM_UNDERFLOW [00:00] */
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR_STREAM_UNDERFLOW_MASK 0x00000001
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR_STREAM_UNDERFLOW_SHIFT 0
#define BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_ESR_MASK_CLEAR_STREAM_UNDERFLOW_DEFAULT 0x00000001

#endif /* #ifndef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_H__ */

/* End of File */
