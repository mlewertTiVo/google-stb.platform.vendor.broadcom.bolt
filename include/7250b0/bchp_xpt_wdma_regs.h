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
 * Date:           Generated on              Tue Mar 25 03:09:24 2014
 *                 Full Compile MD5 Checksum 9df3b42a7e9d8f74079cd20feb650cd4
 *                   (minus title and desc)  
 *                 MD5 Checksum              b2e9ed931ea5aec568db0bca7f615fea
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

#ifndef BCHP_XPT_WDMA_REGS_H__
#define BCHP_XPT_WDMA_REGS_H__

/***************************************************************************
 *XPT_WDMA_REGS
 ***************************************************************************/
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR         0x00a69000 /* Set and Clear for the RUN bit */
#define BCHP_XPT_WDMA_REGS_WAKE_SET              0x00a69004 /* Set for the WAKE bit */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG      0x00a69008 /* RBUF Mode configuration */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG 0x00a6900c /* Timestamp Mode configuration */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG 0x00a69010 /* Address Mode configuration */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG       0x00a69014 /* Local Descriptor Loading Mode configuration */
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD         0x00a69018 /* Soft Reset Command for a DMQ FIFO controller */
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD          0x00a6901c /* Push Command for a DMQ FIFO controller */
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31         0x00a69020 /* Run bits for all channels */
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31        0x00a69024 /* WAKE bits for all channels */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31   0x00a69028 /* Address Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31 0x00a6902c /* Timestamp Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31    0x00a69030 /* Local Descriptor Load Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31 0x00a69034 /* Address Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31     0x00a69038 /* SLEEP Status for all channels */
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31    0x00a6903c /* CONTEXT_VALID Status for channels in Indirect Address Mode */
#define BCHP_XPT_WDMA_REGS_DATA_STALL_TIMEOUT    0x00a69040 /* Data Stall Timeout */
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE 0x00a69044 /* Speculative Read Enable */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31 0x00a69048 /* Outstanding Read Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31 0x00a6904c /* Outstanding Write Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR 0x00a69050 /* Clear Outstanding Read Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR 0x00a69054 /* Clear Outstanding Write Flag */
#define BCHP_XPT_WDMA_REGS_DRR_STATE             0x00a69058 /* Bits of Descriptor Read Request (DRR) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_DRC_STATE             0x00a6905c /* Bits of Descriptor Read Completion (DRC) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_DAP_STATE             0x00a69060 /* Bits of Data Processor (DAP) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_RPT_STATE             0x00a69064 /* Bits of Reporping (RPT) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE    0x00a69068 /* Ready / Accept signals on the ports of WDMA in real time */

/***************************************************************************
 *RUN_SET_CLEAR - Set and Clear for the RUN bit
 ***************************************************************************/
/* XPT_WDMA_REGS :: RUN_SET_CLEAR :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_reserved0_MASK            0xfffffe00
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_reserved0_SHIFT           9

/* XPT_WDMA_REGS :: RUN_SET_CLEAR :: SET_CLEAR [08:08] */
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_SET_CLEAR_MASK            0x00000100
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_SET_CLEAR_SHIFT           8
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_SET_CLEAR_DEFAULT         0x00000000

/* XPT_WDMA_REGS :: RUN_SET_CLEAR :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_CHANNEL_NUM_MASK          0x000000ff
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_CHANNEL_NUM_SHIFT         0
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR_CHANNEL_NUM_DEFAULT       0x00000000

/***************************************************************************
 *WAKE_SET - Set for the WAKE bit
 ***************************************************************************/
/* XPT_WDMA_REGS :: WAKE_SET :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_WAKE_SET_reserved0_MASK                 0xfffffe00
#define BCHP_XPT_WDMA_REGS_WAKE_SET_reserved0_SHIFT                9

/* XPT_WDMA_REGS :: WAKE_SET :: SET [08:08] */
#define BCHP_XPT_WDMA_REGS_WAKE_SET_SET_MASK                       0x00000100
#define BCHP_XPT_WDMA_REGS_WAKE_SET_SET_SHIFT                      8
#define BCHP_XPT_WDMA_REGS_WAKE_SET_SET_DEFAULT                    0x00000000

/* XPT_WDMA_REGS :: WAKE_SET :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_WAKE_SET_CHANNEL_NUM_MASK               0x000000ff
#define BCHP_XPT_WDMA_REGS_WAKE_SET_CHANNEL_NUM_SHIFT              0
#define BCHP_XPT_WDMA_REGS_WAKE_SET_CHANNEL_NUM_DEFAULT            0x00000000

/***************************************************************************
 *RBUF_MODE_CONFIG - RBUF Mode configuration
 ***************************************************************************/
/* XPT_WDMA_REGS :: RBUF_MODE_CONFIG :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_reserved0_MASK         0xfffffe00
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_reserved0_SHIFT        9

/* XPT_WDMA_REGS :: RBUF_MODE_CONFIG :: CONFIG [08:08] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CONFIG_MASK            0x00000100
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CONFIG_SHIFT           8
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CONFIG_DEFAULT         0x00000000

/* XPT_WDMA_REGS :: RBUF_MODE_CONFIG :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CHANNEL_NUM_MASK       0x000000ff
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CHANNEL_NUM_SHIFT      0
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG_CHANNEL_NUM_DEFAULT    0x00000000

/***************************************************************************
 *TIMESTAMP_MODE_CONFIG - Timestamp Mode configuration
 ***************************************************************************/
/* XPT_WDMA_REGS :: TIMESTAMP_MODE_CONFIG :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_reserved0_MASK    0xfffffe00
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_reserved0_SHIFT   9

/* XPT_WDMA_REGS :: TIMESTAMP_MODE_CONFIG :: CONFIG [08:08] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CONFIG_MASK       0x00000100
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CONFIG_SHIFT      8
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CONFIG_DEFAULT    0x00000000

/* XPT_WDMA_REGS :: TIMESTAMP_MODE_CONFIG :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CHANNEL_NUM_MASK  0x000000ff
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CHANNEL_NUM_SHIFT 0
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG_CHANNEL_NUM_DEFAULT 0x00000000

/***************************************************************************
 *MATCH_RUN_VERSION_CONFIG - Address Mode configuration
 ***************************************************************************/
/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_CONFIG :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_reserved0_MASK 0xfffffe00
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_reserved0_SHIFT 9

/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_CONFIG :: CONFIG [08:08] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CONFIG_MASK    0x00000100
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CONFIG_SHIFT   8
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CONFIG_DEFAULT 0x00000000

/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_CONFIG :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CHANNEL_NUM_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CHANNEL_NUM_SHIFT 0
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG_CHANNEL_NUM_DEFAULT 0x00000000

/***************************************************************************
 *LDL_MODE_CONFIG - Local Descriptor Loading Mode configuration
 ***************************************************************************/
/* XPT_WDMA_REGS :: LDL_MODE_CONFIG :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_reserved0_MASK          0xfffffe00
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_reserved0_SHIFT         9

/* XPT_WDMA_REGS :: LDL_MODE_CONFIG :: LDL_MODE [08:08] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_LDL_MODE_MASK           0x00000100
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_LDL_MODE_SHIFT          8
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_LDL_MODE_DEFAULT        0x00000000

/* XPT_WDMA_REGS :: LDL_MODE_CONFIG :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_CHANNEL_NUM_MASK        0x000000ff
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_CHANNEL_NUM_SHIFT       0
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG_CHANNEL_NUM_DEFAULT     0x00000000

/***************************************************************************
 *DMQ_RESET_CMD - Soft Reset Command for a DMQ FIFO controller
 ***************************************************************************/
/* XPT_WDMA_REGS :: DMQ_RESET_CMD :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_reserved0_MASK            0xfffffe00
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_reserved0_SHIFT           9

/* XPT_WDMA_REGS :: DMQ_RESET_CMD :: RESET_DMQ [08:08] */
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_RESET_DMQ_MASK            0x00000100
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_RESET_DMQ_SHIFT           8
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_RESET_DMQ_DEFAULT         0x00000000

/* XPT_WDMA_REGS :: DMQ_RESET_CMD :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_CHANNEL_NUM_MASK          0x000000ff
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_CHANNEL_NUM_SHIFT         0
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD_CHANNEL_NUM_DEFAULT       0x00000000

/***************************************************************************
 *DMQ_PUSH_CMD - Push Command for a DMQ FIFO controller
 ***************************************************************************/
/* XPT_WDMA_REGS :: DMQ_PUSH_CMD :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_reserved0_MASK             0xfffffe00
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_reserved0_SHIFT            9

/* XPT_WDMA_REGS :: DMQ_PUSH_CMD :: PUSH_DMQ [08:08] */
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_PUSH_DMQ_MASK              0x00000100
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_PUSH_DMQ_SHIFT             8
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_PUSH_DMQ_DEFAULT           0x00000000

/* XPT_WDMA_REGS :: DMQ_PUSH_CMD :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_CHANNEL_NUM_MASK           0x000000ff
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_CHANNEL_NUM_SHIFT          0
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD_CHANNEL_NUM_DEFAULT        0x00000000

/***************************************************************************
 *RUN_BITS_0_31 - Run bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: RUN_BITS_0_31 :: RUN_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_MASK             0xffffffff
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_SHIFT            0
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_DEFAULT          0x00000000

/***************************************************************************
 *WAKE_BITS_0_31 - WAKE bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: WAKE_BITS_0_31 :: WAKE_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_MASK           0xffffffff
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_SHIFT          0
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_DEFAULT        0x00000000

/***************************************************************************
 *RBUF_MODE_BITS_0_31 - Address Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: RBUF_MODE_BITS_0_31 :: RBUF_MODE_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_MASK 0xffffffff
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *TIMESTAMP_MODE_BITS_0_31 - Timestamp Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: TIMESTAMP_MODE_BITS_0_31 :: TIMESTAMP_MODE_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_MASK 0xffffffff
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *LDL_MODE_BITS_0_31 - Local Descriptor Load Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: LDL_MODE_BITS_0_31 :: LDL_MODE_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_MASK   0xffffffff
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_SHIFT  0
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *MATCH_RUN_VERSION_BITS_0_31 - Address Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_BITS_0_31 :: MATCH_RUN_VERSION_BITS [31:00] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_MASK 0xffffffff
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_DEFAULT 0x00000000

/***************************************************************************
 *SLEEP_STATUS_0_31 - SLEEP Status for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: SLEEP_STATUS_0_31 :: SLEEP_STATUS [31:00] */
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_MASK     0xffffffff
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_SHIFT    0
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_DEFAULT  0x00000000

/***************************************************************************
 *CONTEXT_VALID_0_31 - CONTEXT_VALID Status for channels in Indirect Address Mode
 ***************************************************************************/
/* XPT_WDMA_REGS :: CONTEXT_VALID_0_31 :: CONTEXT_VALID [31:00] */
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_CONTEXT_VALID_MASK   0xffffffff
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_CONTEXT_VALID_SHIFT  0
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_CONTEXT_VALID_DEFAULT 0x00000000

/***************************************************************************
 *DATA_STALL_TIMEOUT - Data Stall Timeout
 ***************************************************************************/
/* XPT_WDMA_REGS :: DATA_STALL_TIMEOUT :: TIMEOUT_CLOCKS [31:00] */
#define BCHP_XPT_WDMA_REGS_DATA_STALL_TIMEOUT_TIMEOUT_CLOCKS_MASK  0xffffffff
#define BCHP_XPT_WDMA_REGS_DATA_STALL_TIMEOUT_TIMEOUT_CLOCKS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_DATA_STALL_TIMEOUT_TIMEOUT_CLOCKS_DEFAULT 0x0001a5e0

/***************************************************************************
 *SPECULATIVE_READ_ENABLE - Speculative Read Enable
 ***************************************************************************/
/* XPT_WDMA_REGS :: SPECULATIVE_READ_ENABLE :: reserved0 [31:01] */
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE_reserved0_MASK  0xfffffffe
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE_reserved0_SHIFT 1

/* XPT_WDMA_REGS :: SPECULATIVE_READ_ENABLE :: ENABLE [00:00] */
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE_ENABLE_MASK     0x00000001
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE_ENABLE_SHIFT    0
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE_ENABLE_DEFAULT  0x00000000

/***************************************************************************
 *OUTSTANDING_READ_FLAG_0_31 - Outstanding Read Flag
 ***************************************************************************/
/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_0_31 :: OUTSTANDING_READ_FLAGS [31:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_MASK 0xffffffff
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_DEFAULT 0x00000000

/***************************************************************************
 *OUTSTANDING_WRITE_FLAG_0_31 - Outstanding Write Flag
 ***************************************************************************/
/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_0_31 :: OUTSTANDING_WRITE_FLAGS [31:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_OUTSTANDING_WRITE_FLAGS_MASK 0xffffffff
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_OUTSTANDING_WRITE_FLAGS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_OUTSTANDING_WRITE_FLAGS_DEFAULT 0x00000000

/***************************************************************************
 *OUTSTANDING_READ_FLAG_CLEAR - Clear Outstanding Read Flag
 ***************************************************************************/
/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_CLEAR :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_reserved0_MASK 0xfffffe00
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_reserved0_SHIFT 9

/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_CLEAR :: CLEAR_FLAG [08:08] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CLEAR_FLAG_MASK 0x00000100
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CLEAR_FLAG_SHIFT 8
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CLEAR_FLAG_DEFAULT 0x00000000

/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_CLEAR :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CHANNEL_NUM_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CHANNEL_NUM_SHIFT 0
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR_CHANNEL_NUM_DEFAULT 0x00000000

/***************************************************************************
 *OUTSTANDING_WRITE_FLAG_CLEAR - Clear Outstanding Write Flag
 ***************************************************************************/
/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_CLEAR :: reserved0 [31:09] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_reserved0_MASK 0xfffffe00
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_reserved0_SHIFT 9

/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_CLEAR :: CLEAR_FLAG [08:08] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CLEAR_FLAG_MASK 0x00000100
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CLEAR_FLAG_SHIFT 8
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CLEAR_FLAG_DEFAULT 0x00000000

/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_CLEAR :: CHANNEL_NUM [07:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CHANNEL_NUM_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CHANNEL_NUM_SHIFT 0
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR_CHANNEL_NUM_DEFAULT 0x00000000

/***************************************************************************
 *DRR_STATE - Bits of Descriptor Read Request (DRR) State Machine in real time
 ***************************************************************************/
/* XPT_WDMA_REGS :: DRR_STATE :: reserved0 [31:04] */
#define BCHP_XPT_WDMA_REGS_DRR_STATE_reserved0_MASK                0xfffffff0
#define BCHP_XPT_WDMA_REGS_DRR_STATE_reserved0_SHIFT               4

/* XPT_WDMA_REGS :: DRR_STATE :: DRR_STATE [03:00] */
#define BCHP_XPT_WDMA_REGS_DRR_STATE_DRR_STATE_MASK                0x0000000f
#define BCHP_XPT_WDMA_REGS_DRR_STATE_DRR_STATE_SHIFT               0

/***************************************************************************
 *DRC_STATE - Bits of Descriptor Read Completion (DRC) State Machine in real time
 ***************************************************************************/
/* XPT_WDMA_REGS :: DRC_STATE :: reserved0 [31:03] */
#define BCHP_XPT_WDMA_REGS_DRC_STATE_reserved0_MASK                0xfffffff8
#define BCHP_XPT_WDMA_REGS_DRC_STATE_reserved0_SHIFT               3

/* XPT_WDMA_REGS :: DRC_STATE :: RDC_STATE [02:00] */
#define BCHP_XPT_WDMA_REGS_DRC_STATE_RDC_STATE_MASK                0x00000007
#define BCHP_XPT_WDMA_REGS_DRC_STATE_RDC_STATE_SHIFT               0

/***************************************************************************
 *DAP_STATE - Bits of Data Processor (DAP) State Machine in real time
 ***************************************************************************/
/* XPT_WDMA_REGS :: DAP_STATE :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_REGS_DAP_STATE_reserved0_MASK                0xffffffe0
#define BCHP_XPT_WDMA_REGS_DAP_STATE_reserved0_SHIFT               5

/* XPT_WDMA_REGS :: DAP_STATE :: DAP_STATE [04:00] */
#define BCHP_XPT_WDMA_REGS_DAP_STATE_DAP_STATE_MASK                0x0000001f
#define BCHP_XPT_WDMA_REGS_DAP_STATE_DAP_STATE_SHIFT               0

/***************************************************************************
 *RPT_STATE - Bits of Reporping (RPT) State Machine in real time
 ***************************************************************************/
/* XPT_WDMA_REGS :: RPT_STATE :: reserved0 [31:03] */
#define BCHP_XPT_WDMA_REGS_RPT_STATE_reserved0_MASK                0xfffffff8
#define BCHP_XPT_WDMA_REGS_RPT_STATE_reserved0_SHIFT               3

/* XPT_WDMA_REGS :: RPT_STATE :: RPT_STATE [02:00] */
#define BCHP_XPT_WDMA_REGS_RPT_STATE_RPT_STATE_MASK                0x00000007
#define BCHP_XPT_WDMA_REGS_RPT_STATE_RPT_STATE_SHIFT               0

/***************************************************************************
 *READY_ACCEPT_PROBE - Ready / Accept signals on the ports of WDMA in real time
 ***************************************************************************/
/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: reserved0 [31:18] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved0_MASK       0xfffc0000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved0_SHIFT      18

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: RD_XM2LC_READY [17:17] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_XM2LC_READY_MASK  0x00020000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_XM2LC_READY_SHIFT 17

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: RD_XM2LC_ACCEPT [16:16] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_XM2LC_ACCEPT_MASK 0x00010000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_XM2LC_ACCEPT_SHIFT 16

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: reserved1 [15:14] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved1_MASK       0x0000c000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved1_SHIFT      14

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: RD_LC2XM_READY [13:13] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_LC2XM_READY_MASK  0x00002000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_LC2XM_READY_SHIFT 13

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: RD_LC2XM_ACCEPT [12:12] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_LC2XM_ACCEPT_MASK 0x00001000
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_RD_LC2XM_ACCEPT_SHIFT 12

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: reserved2 [11:10] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved2_MASK       0x00000c00
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved2_SHIFT      10

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: WR_XM2LC_READY [09:09] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_XM2LC_READY_MASK  0x00000200
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_XM2LC_READY_SHIFT 9

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: WR_XM2LC_ACCEPT [08:08] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_XM2LC_ACCEPT_MASK 0x00000100
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_XM2LC_ACCEPT_SHIFT 8

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: reserved3 [07:06] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved3_MASK       0x000000c0
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved3_SHIFT      6

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: WR_LC2XM_READY [05:05] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_LC2XM_READY_MASK  0x00000020
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_LC2XM_READY_SHIFT 5

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: WR_LC2XM_ACCEPT [04:04] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_LC2XM_ACCEPT_MASK 0x00000010
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_WR_LC2XM_ACCEPT_SHIFT 4

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: reserved4 [03:02] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved4_MASK       0x0000000c
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_reserved4_SHIFT      2

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: SEC_READY [01:01] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_SEC_READY_MASK       0x00000002
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_SEC_READY_SHIFT      1

/* XPT_WDMA_REGS :: READY_ACCEPT_PROBE :: SEC_ACCEPT [00:00] */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_SEC_ACCEPT_MASK      0x00000001
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE_SEC_ACCEPT_SHIFT     0

#endif /* #ifndef BCHP_XPT_WDMA_REGS_H__ */

/* End of File */
