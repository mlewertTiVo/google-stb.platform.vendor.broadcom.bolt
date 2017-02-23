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
 * Date:           Generated on               Thu Oct 20 06:10:12 2016
 *                 Full Compile MD5 Checksum  a4cf01343914cbe977ff5dbbfecedfc2
 *                     (minus title and desc)
 *                 MD5 Checksum               79b3267dbc97ab78b1475c8950650765
 *
 * lock_release:   n/a
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

#ifndef BCHP_HIF_SPI_INTR2_H__
#define BCHP_HIF_SPI_INTR2_H__

/***************************************************************************
 *HIF_SPI_INTR2 - HIF Level 2 Interrupt Controller Registers for SPI
 ***************************************************************************/
#define BCHP_HIF_SPI_INTR2_CPU_STATUS            0x20201a00 /* [RO][32] CPU interrupt Status Register */
#define BCHP_HIF_SPI_INTR2_CPU_SET               0x20201a04 /* [WO][32] CPU interrupt Set Register */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR             0x20201a08 /* [WO][32] CPU interrupt Clear Register */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS       0x20201a0c /* [RO][32] CPU interrupt Mask Status Register */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET          0x20201a10 /* [WO][32] CPU interrupt Mask Set Register */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR        0x20201a14 /* [WO][32] CPU interrupt Mask Clear Register */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS            0x20201a18 /* [RO][32] PCI interrupt Status Register */
#define BCHP_HIF_SPI_INTR2_PCI_SET               0x20201a1c /* [WO][32] PCI interrupt Set Register */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR             0x20201a20 /* [WO][32] PCI interrupt Clear Register */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS       0x20201a24 /* [RO][32] PCI interrupt Mask Status Register */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET          0x20201a28 /* [WO][32] PCI interrupt Mask Set Register */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR        0x20201a2c /* [WO][32] PCI interrupt Mask Clear Register */

/***************************************************************************
 *CPU_STATUS - CPU interrupt Status Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_STATUS :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_reserved0_MASK               0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_reserved0_SHIFT              7

/* HIF_SPI_INTR2 :: CPU_STATUS :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_HALTED_MASK             0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_HALTED_SHIFT            6
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_HALTED_DEFAULT          0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_DONE_MASK               0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_DONE_SHIFT              5
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_MSPI_DONE_DEFAULT            0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_OVERREAD_MASK         0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_OVERREAD_SHIFT        4
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_OVERREAD_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_DONE_MASK     0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_DONE_SHIFT    3
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_DONE_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_IMPATIENT_MASK        0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_IMPATIENT_SHIFT       2
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_IMPATIENT_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_ABORTED_MASK  0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_STATUS :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_CPU_STATUS_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *CPU_SET - CPU interrupt Set Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_SET :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_reserved0_MASK                  0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_SET_reserved0_SHIFT                 7

/* HIF_SPI_INTR2 :: CPU_SET :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_HALTED_MASK                0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_HALTED_SHIFT               6
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_HALTED_DEFAULT             0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_DONE_MASK                  0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_DONE_SHIFT                 5
#define BCHP_HIF_SPI_INTR2_CPU_SET_MSPI_DONE_DEFAULT               0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_OVERREAD_MASK            0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_OVERREAD_SHIFT           4
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_OVERREAD_DEFAULT         0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_DONE_MASK        0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_DONE_SHIFT       3
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_DONE_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_IMPATIENT_MASK           0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_IMPATIENT_SHIFT          2
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_IMPATIENT_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_ABORTED_MASK     0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_ABORTED_SHIFT    1
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_SESSION_ABORTED_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: CPU_SET :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_FULLNESS_REACHED_MASK    0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_FULLNESS_REACHED_SHIFT   0
#define BCHP_HIF_SPI_INTR2_CPU_SET_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *CPU_CLEAR - CPU interrupt Clear Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_CLEAR :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_reserved0_MASK                0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_reserved0_SHIFT               7

/* HIF_SPI_INTR2 :: CPU_CLEAR :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_HALTED_MASK              0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_HALTED_SHIFT             6
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_HALTED_DEFAULT           0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_DONE_MASK                0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_DONE_SHIFT               5
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_MSPI_DONE_DEFAULT             0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_OVERREAD_MASK          0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_OVERREAD_SHIFT         4
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_OVERREAD_DEFAULT       0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_DONE_MASK      0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_DONE_SHIFT     3
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_DONE_DEFAULT   0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_IMPATIENT_MASK         0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_IMPATIENT_SHIFT        2
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_IMPATIENT_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_ABORTED_MASK   0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_ABORTED_SHIFT  1
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_CLEAR :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_FULLNESS_REACHED_MASK  0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_CPU_CLEAR_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *CPU_MASK_STATUS - CPU interrupt Mask Status Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_reserved0_MASK          0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_reserved0_SHIFT         7

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_HALTED_MASK        0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_HALTED_SHIFT       6
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_HALTED_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_DONE_MASK          0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_DONE_SHIFT         5
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_MSPI_DONE_DEFAULT       0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_OVERREAD_MASK    0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_OVERREAD_SHIFT   4
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_OVERREAD_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_DONE_MASK 0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_DONE_SHIFT 3
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_IMPATIENT_MASK   0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_IMPATIENT_SHIFT  2
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_IMPATIENT_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_STATUS :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_CPU_MASK_STATUS_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *CPU_MASK_SET - CPU interrupt Mask Set Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_MASK_SET :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_reserved0_MASK             0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_reserved0_SHIFT            7

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_HALTED_MASK           0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_HALTED_SHIFT          6
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_HALTED_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_DONE_MASK             0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_DONE_SHIFT            5
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_MSPI_DONE_DEFAULT          0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_OVERREAD_MASK       0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_OVERREAD_SHIFT      4
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_OVERREAD_DEFAULT    0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_DONE_MASK   0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_DONE_SHIFT  3
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_IMPATIENT_MASK      0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_IMPATIENT_SHIFT     2
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_IMPATIENT_DEFAULT   0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_SET :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_CPU_MASK_SET_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *CPU_MASK_CLEAR - CPU interrupt Mask Clear Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_reserved0_MASK           0xffffff80
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_reserved0_SHIFT          7

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_HALTED_MASK         0x00000040
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_HALTED_SHIFT        6
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_HALTED_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_DONE_MASK           0x00000020
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_DONE_SHIFT          5
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_MSPI_DONE_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_OVERREAD_MASK     0x00000010
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_OVERREAD_SHIFT    4
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_OVERREAD_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_DONE_MASK 0x00000008
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_DONE_SHIFT 3
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_IMPATIENT_MASK    0x00000004
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_IMPATIENT_SHIFT   2
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_IMPATIENT_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: CPU_MASK_CLEAR :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_CPU_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_STATUS - PCI interrupt Status Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_STATUS :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_reserved0_MASK               0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_reserved0_SHIFT              7

/* HIF_SPI_INTR2 :: PCI_STATUS :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_HALTED_MASK             0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_HALTED_SHIFT            6
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_HALTED_DEFAULT          0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_DONE_MASK               0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_DONE_SHIFT              5
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_MSPI_DONE_DEFAULT            0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_OVERREAD_MASK         0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_OVERREAD_SHIFT        4
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_OVERREAD_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_DONE_MASK     0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_DONE_SHIFT    3
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_DONE_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_IMPATIENT_MASK        0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_IMPATIENT_SHIFT       2
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_IMPATIENT_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_ABORTED_MASK  0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_STATUS :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_PCI_STATUS_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_SET - PCI interrupt Set Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_SET :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_reserved0_MASK                  0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_SET_reserved0_SHIFT                 7

/* HIF_SPI_INTR2 :: PCI_SET :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_HALTED_MASK                0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_HALTED_SHIFT               6
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_HALTED_DEFAULT             0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_DONE_MASK                  0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_DONE_SHIFT                 5
#define BCHP_HIF_SPI_INTR2_PCI_SET_MSPI_DONE_DEFAULT               0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_OVERREAD_MASK            0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_OVERREAD_SHIFT           4
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_OVERREAD_DEFAULT         0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_DONE_MASK        0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_DONE_SHIFT       3
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_DONE_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_IMPATIENT_MASK           0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_IMPATIENT_SHIFT          2
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_IMPATIENT_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_ABORTED_MASK     0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_ABORTED_SHIFT    1
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_SESSION_ABORTED_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: PCI_SET :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_FULLNESS_REACHED_MASK    0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_FULLNESS_REACHED_SHIFT   0
#define BCHP_HIF_SPI_INTR2_PCI_SET_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_CLEAR - PCI interrupt Clear Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_CLEAR :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_reserved0_MASK                0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_reserved0_SHIFT               7

/* HIF_SPI_INTR2 :: PCI_CLEAR :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_HALTED_MASK              0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_HALTED_SHIFT             6
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_HALTED_DEFAULT           0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_DONE_MASK                0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_DONE_SHIFT               5
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_MSPI_DONE_DEFAULT             0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_OVERREAD_MASK          0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_OVERREAD_SHIFT         4
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_OVERREAD_DEFAULT       0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_DONE_MASK      0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_DONE_SHIFT     3
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_DONE_DEFAULT   0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_IMPATIENT_MASK         0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_IMPATIENT_SHIFT        2
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_IMPATIENT_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_ABORTED_MASK   0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_ABORTED_SHIFT  1
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_CLEAR :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_FULLNESS_REACHED_MASK  0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_PCI_CLEAR_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_MASK_STATUS - PCI interrupt Mask Status Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_reserved0_MASK          0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_reserved0_SHIFT         7

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_HALTED_MASK        0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_HALTED_SHIFT       6
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_HALTED_DEFAULT     0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_DONE_MASK          0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_DONE_SHIFT         5
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_MSPI_DONE_DEFAULT       0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_OVERREAD_MASK    0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_OVERREAD_SHIFT   4
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_OVERREAD_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_DONE_MASK 0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_DONE_SHIFT 3
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_IMPATIENT_MASK   0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_IMPATIENT_SHIFT  2
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_IMPATIENT_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_STATUS :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_PCI_MASK_STATUS_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_MASK_SET - PCI interrupt Mask Set Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_MASK_SET :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_reserved0_MASK             0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_reserved0_SHIFT            7

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_HALTED_MASK           0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_HALTED_SHIFT          6
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_HALTED_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_DONE_MASK             0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_DONE_SHIFT            5
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_MSPI_DONE_DEFAULT          0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_OVERREAD_MASK       0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_OVERREAD_SHIFT      4
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_OVERREAD_DEFAULT    0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_DONE_MASK   0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_DONE_SHIFT  3
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_IMPATIENT_MASK      0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_IMPATIENT_SHIFT     2
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_IMPATIENT_DEFAULT   0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_SET :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_PCI_MASK_SET_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

/***************************************************************************
 *PCI_MASK_CLEAR - PCI interrupt Mask Clear Register
 ***************************************************************************/
/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: reserved0 [31:07] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_reserved0_MASK           0xffffff80
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_reserved0_SHIFT          7

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: MSPI_HALTED [06:06] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_HALTED_MASK         0x00000040
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_HALTED_SHIFT        6
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_HALTED_DEFAULT      0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: MSPI_DONE [05:05] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_DONE_MASK           0x00000020
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_DONE_SHIFT          5
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_MSPI_DONE_DEFAULT        0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: SPI_LR_OVERREAD [04:04] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_OVERREAD_MASK     0x00000010
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_OVERREAD_SHIFT    4
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_OVERREAD_DEFAULT  0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: SPI_LR_SESSION_DONE [03:03] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_DONE_MASK 0x00000008
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_DONE_SHIFT 3
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_DONE_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: SPI_LR_IMPATIENT [02:02] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_IMPATIENT_MASK    0x00000004
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_IMPATIENT_SHIFT   2
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_IMPATIENT_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: SPI_LR_SESSION_ABORTED [01:01] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_ABORTED_MASK 0x00000002
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_ABORTED_SHIFT 1
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_SESSION_ABORTED_DEFAULT 0x00000000

/* HIF_SPI_INTR2 :: PCI_MASK_CLEAR :: SPI_LR_FULLNESS_REACHED [00:00] */
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_MASK 0x00000001
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_SHIFT 0
#define BCHP_HIF_SPI_INTR2_PCI_MASK_CLEAR_SPI_LR_FULLNESS_REACHED_DEFAULT 0x00000000

#endif /* #ifndef BCHP_HIF_SPI_INTR2_H__ */

/* End of File */