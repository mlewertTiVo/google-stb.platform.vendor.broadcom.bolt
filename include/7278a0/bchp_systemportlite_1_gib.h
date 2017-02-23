/********************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use all
 *    reasonable efforts to protect the confidentiality thereof, and to use
 *    this information only in connection with your use of Broadcom integrated
 *    circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 *    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
 *    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
 *    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
 *    ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
 *    THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 *    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
 *    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
 *    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
 *    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
 *    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
 *    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
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
 * Date:           Generated on               Thu Dec  1 06:46:34 2016
 *                 Full Compile MD5 Checksum  20bceb797972bbbfe6886f688dd76df1
 *                     (minus title and desc)
 *                 MD5 Checksum               60da5b9432d9a6239bf2f04b2e62b119
 *
 * lock_release:   r_1099
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     1139
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *                 Script Source              /projects/stbgit/bin/gen_rdb.pl
 *                 DVTSWVER                   n/a
 *
 *
********************************************************************************/

#ifndef BCHP_SYSTEMPORTLITE_1_GIB_H__
#define BCHP_SYSTEMPORTLITE_1_GIB_H__

/***************************************************************************
 *SYSTEMPORTLITE_1_GIB
 ***************************************************************************/
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL        0x01401000 /* [RW][32] GIB Control Register */
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS         0x01401004 /* [RO][32] GIB Status Register */
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1       0x01401008 /* [RW][32] GIB MAC DA 1 Register */
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_0       0x0140100c /* [RW][32] GIB MAC DA 0 Register */
#define BCHP_SYSTEMPORTLITE_1_GIB_RCVD_PKT_ERR_CNT 0x01401010 /* [RW][32] GIB Received Packet Error Counter Register */

/***************************************************************************
 *CONTROL - GIB Control Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GIB :: CONTROL :: reserved0 [31:28] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_reserved0_MASK           0xf0000000
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_reserved0_SHIFT          28

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: PAD_EXTENSION [27:22] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PAD_EXTENSION_MASK       0x0fc00000
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PAD_EXTENSION_SHIFT      22
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PAD_EXTENSION_DEFAULT    0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: IPG_LENGTH [21:16] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_IPG_LENGTH_MASK          0x003f0000
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_IPG_LENGTH_SHIFT         16
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_IPG_LENGTH_DEFAULT       0x00000012

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: PREAMBLE_LENGTH [15:12] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PREAMBLE_LENGTH_MASK     0x0000f000
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PREAMBLE_LENGTH_SHIFT    12
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_PREAMBLE_LENGTH_DEFAULT  0x00000007

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: RX_PAUSE_EN [11:11] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_PAUSE_EN_MASK         0x00000800
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_PAUSE_EN_SHIFT        11
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_PAUSE_EN_DEFAULT      0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: RMT_LOOPBACK_RX_EN [10:10] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_RX_EN_MASK  0x00000400
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_RX_EN_SHIFT 10
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_RX_EN_DEFAULT 0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: RMT_LOOPBACK_EN [09:09] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_EN_MASK     0x00000200
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_EN_SHIFT    9
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RMT_LOOPBACK_EN_DEFAULT  0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: LOCAL_LOOPBACK_TX_EN [08:08] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_TX_EN_MASK 0x00000100
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_TX_EN_SHIFT 8
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_TX_EN_DEFAULT 0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: LOCAL_LOOPBACK_EN [07:07] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_EN_MASK   0x00000080
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_EN_SHIFT  7
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_LOCAL_LOOPBACK_EN_DEFAULT 0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: FCS_STRIP [06:06] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_FCS_STRIP_MASK           0x00000040
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_FCS_STRIP_SHIFT          6
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_FCS_STRIP_DEFAULT        0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: GTX_CLK_SEL [05:04] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_GTX_CLK_SEL_MASK         0x00000030
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_GTX_CLK_SEL_SHIFT        4
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_GTX_CLK_SEL_DEFAULT      0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: RX_FLUSH [03:03] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_FLUSH_MASK            0x00000008
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_FLUSH_SHIFT           3
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_FLUSH_DEFAULT         0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: TX_FLUSH [02:02] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_FLUSH_MASK            0x00000004
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_FLUSH_SHIFT           2
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_FLUSH_DEFAULT         0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: RX_EN [01:01] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_EN_MASK               0x00000002
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_EN_SHIFT              1
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_RX_EN_DEFAULT            0x00000000

/* SYSTEMPORTLITE_1_GIB :: CONTROL :: TX_EN [00:00] */
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_EN_MASK               0x00000001
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_EN_SHIFT              0
#define BCHP_SYSTEMPORTLITE_1_GIB_CONTROL_TX_EN_DEFAULT            0x00000000

/***************************************************************************
 *STATUS - GIB Status Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GIB :: STATUS :: reserved0 [31:02] */
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_reserved0_MASK            0xfffffffc
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_reserved0_SHIFT           2

/* SYSTEMPORTLITE_1_GIB :: STATUS :: TX_FIFO_UNDERFLOW [01:01] */
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_TX_FIFO_UNDERFLOW_MASK    0x00000002
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_TX_FIFO_UNDERFLOW_SHIFT   1
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_TX_FIFO_UNDERFLOW_DEFAULT 0x00000000

/* SYSTEMPORTLITE_1_GIB :: STATUS :: RX_FIFO_OVERFLOW [00:00] */
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_RX_FIFO_OVERFLOW_MASK     0x00000001
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_RX_FIFO_OVERFLOW_SHIFT    0
#define BCHP_SYSTEMPORTLITE_1_GIB_STATUS_RX_FIFO_OVERFLOW_DEFAULT  0x00000000

/***************************************************************************
 *MAC_DA_1 - GIB MAC DA 1 Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GIB :: MAC_DA_1 :: reserved0 [31:16] */
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1_reserved0_MASK          0xffff0000
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1_reserved0_SHIFT         16

/* SYSTEMPORTLITE_1_GIB :: MAC_DA_1 :: MAC_ADDRESS [15:00] */
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1_MAC_ADDRESS_MASK        0x0000ffff
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1_MAC_ADDRESS_SHIFT       0
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_1_MAC_ADDRESS_DEFAULT     0x00000000

/***************************************************************************
 *MAC_DA_0 - GIB MAC DA 0 Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GIB :: MAC_DA_0 :: MAC_ADDRESS [31:00] */
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_0_MAC_ADDRESS_MASK        0xffffffff
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_0_MAC_ADDRESS_SHIFT       0
#define BCHP_SYSTEMPORTLITE_1_GIB_MAC_DA_0_MAC_ADDRESS_DEFAULT     0x00000000

/***************************************************************************
 *RCVD_PKT_ERR_CNT - GIB Received Packet Error Counter Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GIB :: RCVD_PKT_ERR_CNT :: PKT_COUNT [31:00] */
#define BCHP_SYSTEMPORTLITE_1_GIB_RCVD_PKT_ERR_CNT_PKT_COUNT_MASK  0xffffffff
#define BCHP_SYSTEMPORTLITE_1_GIB_RCVD_PKT_ERR_CNT_PKT_COUNT_SHIFT 0
#define BCHP_SYSTEMPORTLITE_1_GIB_RCVD_PKT_ERR_CNT_PKT_COUNT_DEFAULT 0x00000000

#endif /* #ifndef BCHP_SYSTEMPORTLITE_1_GIB_H__ */

/* End of File */
