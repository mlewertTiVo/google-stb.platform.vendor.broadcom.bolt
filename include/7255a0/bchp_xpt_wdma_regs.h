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
 * Date:           Generated on               Sun Nov 19 01:07:18 2017
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

#ifndef BCHP_XPT_WDMA_REGS_H__
#define BCHP_XPT_WDMA_REGS_H__

/***************************************************************************
 *XPT_WDMA_REGS - WDMA Top Control Registers
 ***************************************************************************/
#define BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR         0x0020a69000 /* [RW][32] Set and Clear for the RUN bit */
#define BCHP_XPT_WDMA_REGS_WAKE_SET              0x0020a69004 /* [RW][32] Set for the WAKE bit */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_CONFIG      0x0020a69008 /* [RW][32] RBUF Mode configuration */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_CONFIG 0x0020a6900c /* [RW][32] Timestamp Mode configuration */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG 0x0020a69010 /* [RW][32] Address Mode configuration */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_CONFIG       0x0020a69014 /* [RW][32] Local Descriptor Loading Mode configuration */
#define BCHP_XPT_WDMA_REGS_DMQ_RESET_CMD         0x0020a69018 /* [RW][32] Soft Reset Command for a DMQ FIFO controller */
#define BCHP_XPT_WDMA_REGS_DMQ_PUSH_CMD          0x0020a6901c /* [RW][32] Push Command for a DMQ FIFO controller */
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31         0x0020a69020 /* [RO][32] Run bits for all channels */
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31        0x0020a69024 /* [RO][32] WAKE bits for all channels */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31   0x0020a69028 /* [RO][32] Address Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31 0x0020a6902c /* [RO][32] Timestamp Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31    0x0020a69030 /* [RO][32] Local Descriptor Load Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31 0x0020a69034 /* [RO][32] Address Mode bits for all channels */
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31     0x0020a69038 /* [RO][32] SLEEP Status for all channels */
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31    0x0020a6903c /* [RO][32] CONTEXT_VALID Status for channels in Indirect Address Mode */
#define BCHP_XPT_WDMA_REGS_DATA_STALL_TIMEOUT    0x0020a69040 /* [RW][32] Data Stall Timeout */
#define BCHP_XPT_WDMA_REGS_SPECULATIVE_READ_ENABLE 0x0020a69044 /* [RW][32] Speculative Read Enable */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31 0x0020a69048 /* [RO][32] Outstanding Read Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31 0x0020a6904c /* [RO][32] Outstanding Write Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_CLEAR 0x0020a69050 /* [RW][32] Clear Outstanding Read Flag */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_CLEAR 0x0020a69054 /* [RW][32] Clear Outstanding Write Flag */
#define BCHP_XPT_WDMA_REGS_DRR_STATE             0x0020a69058 /* [RO][32] Bits of Descriptor Read Request (DRR) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_DRC_STATE             0x0020a6905c /* [RO][32] Bits of Descriptor Read Completion (DRC) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_DAP_STATE             0x0020a69060 /* [RW][32] Bits of Data Processor (DAP) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_RPT_STATE             0x0020a69064 /* [RO][32] Bits of Reporping (RPT) State Machine in real time */
#define BCHP_XPT_WDMA_REGS_READY_ACCEPT_PROBE    0x0020a69068 /* [RO][32] Ready / Accept signals on the ports of WDMA in real time */

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
/* XPT_WDMA_REGS :: RUN_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_reserved0_MASK            0xffffff00
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_reserved0_SHIFT           8

/* XPT_WDMA_REGS :: RUN_BITS_0_31 :: RUN_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_MASK             0x000000ff
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_SHIFT            0
#define BCHP_XPT_WDMA_REGS_RUN_BITS_0_31_RUN_BITS_DEFAULT          0x00000000

/***************************************************************************
 *WAKE_BITS_0_31 - WAKE bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: WAKE_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_reserved0_MASK           0xffffff00
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_reserved0_SHIFT          8

/* XPT_WDMA_REGS :: WAKE_BITS_0_31 :: WAKE_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_MASK           0x000000ff
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_SHIFT          0
#define BCHP_XPT_WDMA_REGS_WAKE_BITS_0_31_WAKE_BITS_DEFAULT        0x00000000

/***************************************************************************
 *RBUF_MODE_BITS_0_31 - Address Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: RBUF_MODE_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_reserved0_MASK      0xffffff00
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_reserved0_SHIFT     8

/* XPT_WDMA_REGS :: RBUF_MODE_BITS_0_31 :: RBUF_MODE_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_RBUF_MODE_BITS_0_31_RBUF_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *TIMESTAMP_MODE_BITS_0_31 - Timestamp Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: TIMESTAMP_MODE_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_reserved0_SHIFT 8

/* XPT_WDMA_REGS :: TIMESTAMP_MODE_BITS_0_31 :: TIMESTAMP_MODE_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_TIMESTAMP_MODE_BITS_0_31_TIMESTAMP_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *LDL_MODE_BITS_0_31 - Local Descriptor Load Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: LDL_MODE_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_reserved0_MASK       0xffffff00
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_reserved0_SHIFT      8

/* XPT_WDMA_REGS :: LDL_MODE_BITS_0_31 :: LDL_MODE_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_MASK   0x000000ff
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_SHIFT  0
#define BCHP_XPT_WDMA_REGS_LDL_MODE_BITS_0_31_LDL_MODE_BITS_DEFAULT 0x00000000

/***************************************************************************
 *MATCH_RUN_VERSION_BITS_0_31 - Address Mode bits for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_BITS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_reserved0_SHIFT 8

/* XPT_WDMA_REGS :: MATCH_RUN_VERSION_BITS_0_31 :: MATCH_RUN_VERSION_BITS [07:00] */
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_MATCH_RUN_VERSION_BITS_0_31_MATCH_RUN_VERSION_BITS_DEFAULT 0x00000000

/***************************************************************************
 *SLEEP_STATUS_0_31 - SLEEP Status for all channels
 ***************************************************************************/
/* XPT_WDMA_REGS :: SLEEP_STATUS_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_reserved0_MASK        0xffffff00
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_reserved0_SHIFT       8

/* XPT_WDMA_REGS :: SLEEP_STATUS_0_31 :: SLEEP_STATUS [07:00] */
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_MASK     0x000000ff
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_SHIFT    0
#define BCHP_XPT_WDMA_REGS_SLEEP_STATUS_0_31_SLEEP_STATUS_DEFAULT  0x00000000

/***************************************************************************
 *CONTEXT_VALID_0_31 - CONTEXT_VALID Status for channels in Indirect Address Mode
 ***************************************************************************/
/* XPT_WDMA_REGS :: CONTEXT_VALID_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_reserved0_MASK       0xffffff00
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_reserved0_SHIFT      8

/* XPT_WDMA_REGS :: CONTEXT_VALID_0_31 :: CONTEXT_VALID [07:00] */
#define BCHP_XPT_WDMA_REGS_CONTEXT_VALID_0_31_CONTEXT_VALID_MASK   0x000000ff
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
/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_reserved0_SHIFT 8

/* XPT_WDMA_REGS :: OUTSTANDING_READ_FLAG_0_31 :: OUTSTANDING_READ_FLAGS [07:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_MASK 0x000000ff
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_SHIFT 0
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_READ_FLAG_0_31_OUTSTANDING_READ_FLAGS_DEFAULT 0x00000000

/***************************************************************************
 *OUTSTANDING_WRITE_FLAG_0_31 - Outstanding Write Flag
 ***************************************************************************/
/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_0_31 :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_reserved0_SHIFT 8

/* XPT_WDMA_REGS :: OUTSTANDING_WRITE_FLAG_0_31 :: OUTSTANDING_WRITE_FLAGS [07:00] */
#define BCHP_XPT_WDMA_REGS_OUTSTANDING_WRITE_FLAG_0_31_OUTSTANDING_WRITE_FLAGS_MASK 0x000000ff
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
/* XPT_WDMA_REGS :: DAP_STATE :: DMQ_EMPTY_FALLBACK [31:31] */
#define BCHP_XPT_WDMA_REGS_DAP_STATE_DMQ_EMPTY_FALLBACK_MASK       0x80000000
#define BCHP_XPT_WDMA_REGS_DAP_STATE_DMQ_EMPTY_FALLBACK_SHIFT      31
#define BCHP_XPT_WDMA_REGS_DAP_STATE_DMQ_EMPTY_FALLBACK_DEFAULT    0x00000000

/* XPT_WDMA_REGS :: DAP_STATE :: reserved0 [30:05] */
#define BCHP_XPT_WDMA_REGS_DAP_STATE_reserved0_MASK                0x7fffffe0
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
