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
 * Date:           Generated on               Thu Feb  5 03:15:08 2015
 *                 Full Compile MD5 Checksum  ca339b82db08da0250a17ca09932699d
 *                     (minus title and desc)
 *                 MD5 Checksum               502556bfbdc2f4341f93db8b4326b3ab
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15517
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_MEMC_MISC_0_H__
#define BCHP_MEMC_MISC_0_H__

/***************************************************************************
 *MEMC_MISC_0 - MEMSYS Misc (Soft-Resets/Configuration) Registers
 ***************************************************************************/
#define BCHP_MEMC_MISC_0_SOFT_RESET              0x00505000 /* [RW] MEMC_TOP layout block Soft Reset */
#define BCHP_MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG   0x00505004 /* [RW] MEMC_STRAP_DDR_CONFIG Control Register */
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL       0x00505008 /* [RW] MEMC MBIST TM Control Register */
#define BCHP_MEMC_MISC_0_FSBL_STATE              0x0050500c /* [RW] Firmware State Scratchpad */
#define BCHP_MEMC_MISC_0_SCRATCH_0               0x00505010 /* [RW] Scratch Register */

/***************************************************************************
 *SOFT_RESET - MEMC_TOP layout block Soft Reset
 ***************************************************************************/
/* MEMC_MISC_0 :: SOFT_RESET :: reserved0 [31:05] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_reserved0_MASK                 0xffffffe0
#define BCHP_MEMC_MISC_0_SOFT_RESET_reserved0_SHIFT                5

/* MEMC_MISC_0 :: SOFT_RESET :: MEMC_IOBUF_RBUS [04:04] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_RBUS_MASK           0x00000010
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_RBUS_SHIFT          4
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_RBUS_DEFAULT        0x00000000

/* MEMC_MISC_0 :: SOFT_RESET :: MEMC_IOBUF [03:03] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_MASK                0x00000008
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_SHIFT               3
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_IOBUF_DEFAULT             0x00000000

/* MEMC_MISC_0 :: SOFT_RESET :: MEMC_DRAM_INIT [02:02] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_DRAM_INIT_MASK            0x00000004
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_DRAM_INIT_SHIFT           2
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_DRAM_INIT_DEFAULT         0x00000000

/* MEMC_MISC_0 :: SOFT_RESET :: MEMC_RBUS [01:01] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_RBUS_MASK                 0x00000002
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_RBUS_SHIFT                1
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_RBUS_DEFAULT              0x00000000

/* MEMC_MISC_0 :: SOFT_RESET :: MEMC_CORE [00:00] */
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_CORE_MASK                 0x00000001
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_CORE_SHIFT                0
#define BCHP_MEMC_MISC_0_SOFT_RESET_MEMC_CORE_DEFAULT              0x00000000

/***************************************************************************
 *MEMC_STRAP_DDR_CONFIG - MEMC_STRAP_DDR_CONFIG Control Register
 ***************************************************************************/
/* MEMC_MISC_0 :: MEMC_STRAP_DDR_CONFIG :: reserved_for_eco0 [31:00] */
#define BCHP_MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG_reserved_for_eco0_MASK 0xffffffff
#define BCHP_MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG_reserved_for_eco0_SHIFT 0
#define BCHP_MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *MEMC_TOP_TM_CNTRL - MEMC MBIST TM Control Register
 ***************************************************************************/
/* MEMC_MISC_0 :: MEMC_TOP_TM_CNTRL :: reserved0 [31:06] */
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_reserved0_MASK          0xffffffc0
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_reserved0_SHIFT         6

/* MEMC_MISC_0 :: MEMC_TOP_TM_CNTRL :: MEMC_PFRI_FIFO_2 [05:04] */
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_2_MASK   0x00000030
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_2_SHIFT  4
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_2_DEFAULT 0x00000000

/* MEMC_MISC_0 :: MEMC_TOP_TM_CNTRL :: MEMC_PFRI_FIFO_1 [03:02] */
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_1_MASK   0x0000000c
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_1_SHIFT  2
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_1_DEFAULT 0x00000000

/* MEMC_MISC_0 :: MEMC_TOP_TM_CNTRL :: MEMC_PFRI_FIFO_0 [01:00] */
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_0_MASK   0x00000003
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_0_SHIFT  0
#define BCHP_MEMC_MISC_0_MEMC_TOP_TM_CNTRL_MEMC_PFRI_FIFO_0_DEFAULT 0x00000000

/***************************************************************************
 *FSBL_STATE - Firmware State Scratchpad
 ***************************************************************************/
/* MEMC_MISC_0 :: FSBL_STATE :: STATE [31:00] */
#define BCHP_MEMC_MISC_0_FSBL_STATE_STATE_MASK                     0xffffffff
#define BCHP_MEMC_MISC_0_FSBL_STATE_STATE_SHIFT                    0
#define BCHP_MEMC_MISC_0_FSBL_STATE_STATE_DEFAULT                  0x00000000

/***************************************************************************
 *SCRATCH_0 - Scratch Register
 ***************************************************************************/
/* MEMC_MISC_0 :: SCRATCH_0 :: VALUE [31:00] */
#define BCHP_MEMC_MISC_0_SCRATCH_0_VALUE_MASK                      0xffffffff
#define BCHP_MEMC_MISC_0_SCRATCH_0_VALUE_SHIFT                     0
#define BCHP_MEMC_MISC_0_SCRATCH_0_VALUE_DEFAULT                   0x00000000

#endif /* #ifndef BCHP_MEMC_MISC_0_H__ */

/* End of File */
