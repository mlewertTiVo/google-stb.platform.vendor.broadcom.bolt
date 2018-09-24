/******************************************************************************
 *  Copyright (C) 2018 Broadcom. The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
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
 ******************************************************************************/

/******************************************************************************
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 * The launch point for all information concerning RDB is found at:
 *   http://bcgbu.broadcom.com/RDB/SitePages/Home.aspx
 *
 * Date:           Generated on               Tue May 22 01:07:29 2018
 *                 Full Compile MD5 Checksum  dab887c3545fa4f530f23348fd71a75c
 *                     (minus title and desc)
 *                 MD5 Checksum               3a5dff4eaab81f8a2ec44b9652a44aa9
 *
 * lock_release:   n/a
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     2270
 *                 unknown                    unknown
 *                 Perl Interpreter           5.022001
 *                 Operating System           linux
 *                 Script Source              scripts/bin/gen_rdb.pl
 *                 DVTSWVER                   LOCAL scripts/bin/gen_rdb.pl
 *
 *
********************************************************************************/

#ifndef BCHP_UPG_AUX_AON_INTR2_H__
#define BCHP_UPG_AUX_AON_INTR2_H__

/***************************************************************************
 *UPG_AUX_AON_INTR2 - UPG AUX AON Level 2 Interrupt Registers
 ***************************************************************************/
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS        0x00417540 /* [RO][32] CPU interrupt Status Register */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET           0x00417544 /* [WO][32] CPU interrupt Set Register */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR         0x00417548 /* [WO][32] CPU interrupt Clear Register */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS   0x0041754c /* [RO][32] CPU interrupt Mask Status Register */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET      0x00417550 /* [WO][32] CPU interrupt Mask Set Register */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR    0x00417554 /* [WO][32] CPU interrupt Mask Clear Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS        0x00417558 /* [RO][32] PCI interrupt Status Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET           0x0041755c /* [WO][32] PCI interrupt Set Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR         0x00417560 /* [WO][32] PCI interrupt Clear Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS   0x00417564 /* [RO][32] PCI interrupt Mask Status Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET      0x00417568 /* [WO][32] PCI interrupt Mask Set Register */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR    0x0041756c /* [WO][32] PCI interrupt Mask Clear Register */

/***************************************************************************
 *CPU_STATUS - CPU interrupt Status Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_reserved0_MASK           0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_reserved0_SHIFT          9

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_WKTMR_ALARM_INTR_MASK    0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_WKTMR_ALARM_INTR_SHIFT   8
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_WKTMR_ALARM_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_reserved_for_eco1_MASK   0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_reserved_for_eco1_SHIFT  7
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_OF_INTR_MASK  0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_OF_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_LVL_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_LVL_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_INACT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT3_INTR_MASK 0x00000008
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT3_INTR_SHIFT 3
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT2_INTR_MASK 0x00000004
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT2_INTR_SHIFT 2
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT1_INTR_MASK 0x00000002
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT1_INTR_SHIFT 1
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_STATUS :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT0_INTR_MASK 0x00000001
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT0_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_CPU_STATUS_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *CPU_SET - CPU interrupt Set Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_SET :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_reserved0_MASK              0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_reserved0_SHIFT             9

/* UPG_AUX_AON_INTR2 :: CPU_SET :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_WKTMR_ALARM_INTR_MASK       0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_WKTMR_ALARM_INTR_SHIFT      8
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_WKTMR_ALARM_INTR_DEFAULT    0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_reserved_for_eco1_MASK      0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_reserved_for_eco1_SHIFT     7
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_reserved_for_eco1_DEFAULT   0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_OF_INTR_MASK     0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_OF_INTR_SHIFT    6
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_OF_INTR_DEFAULT  0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_LVL_INTR_MASK    0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_LVL_INTR_SHIFT   5
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_INACT_INTR_MASK  0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT3_INTR_MASK    0x00000008
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT3_INTR_SHIFT   3
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT2_INTR_MASK    0x00000004
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT2_INTR_SHIFT   2
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT1_INTR_MASK    0x00000002
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT1_INTR_SHIFT   1
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_SET :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT0_INTR_MASK    0x00000001
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT0_INTR_SHIFT   0
#define BCHP_UPG_AUX_AON_INTR2_CPU_SET_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *CPU_CLEAR - CPU interrupt Clear Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_reserved0_MASK            0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_reserved0_SHIFT           9

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_WKTMR_ALARM_INTR_MASK     0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_WKTMR_ALARM_INTR_SHIFT    8
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_WKTMR_ALARM_INTR_DEFAULT  0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_reserved_for_eco1_MASK    0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_reserved_for_eco1_SHIFT   7
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_OF_INTR_MASK   0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_OF_INTR_SHIFT  6
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_OF_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_LVL_INTR_MASK  0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_LVL_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_INACT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT3_INTR_MASK  0x00000008
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT3_INTR_SHIFT 3
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT2_INTR_MASK  0x00000004
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT2_INTR_SHIFT 2
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT1_INTR_MASK  0x00000002
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT1_INTR_SHIFT 1
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_CLEAR :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT0_INTR_MASK  0x00000001
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT0_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_CPU_CLEAR_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *CPU_MASK_STATUS - CPU interrupt Mask Status Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_reserved0_MASK      0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_reserved0_SHIFT     9

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_WKTMR_ALARM_INTR_MASK 0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_STATUS :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_SET - CPU interrupt Mask Set Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_reserved0_MASK         0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_reserved0_SHIFT        9

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_WKTMR_ALARM_INTR_MASK  0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_SET :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_CLEAR - CPU interrupt Mask Clear Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_reserved0_MASK       0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_reserved0_SHIFT      9

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_WKTMR_ALARM_INTR_MASK 0x00000100
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: CPU_MASK_CLEAR :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_CPU_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_STATUS - PCI interrupt Status Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_reserved0_MASK           0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_reserved0_SHIFT          9

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_WKTMR_ALARM_INTR_MASK    0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_WKTMR_ALARM_INTR_SHIFT   8
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_WKTMR_ALARM_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_reserved_for_eco1_MASK   0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_reserved_for_eco1_SHIFT  7
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_OF_INTR_MASK  0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_OF_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_LVL_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_LVL_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_INACT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT3_INTR_MASK 0x00000008
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT3_INTR_SHIFT 3
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT2_INTR_MASK 0x00000004
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT2_INTR_SHIFT 2
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT1_INTR_MASK 0x00000002
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT1_INTR_SHIFT 1
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_STATUS :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT0_INTR_MASK 0x00000001
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT0_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_PCI_STATUS_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *PCI_SET - PCI interrupt Set Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_SET :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_reserved0_MASK              0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_reserved0_SHIFT             9

/* UPG_AUX_AON_INTR2 :: PCI_SET :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_WKTMR_ALARM_INTR_MASK       0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_WKTMR_ALARM_INTR_SHIFT      8
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_WKTMR_ALARM_INTR_DEFAULT    0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_reserved_for_eco1_MASK      0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_reserved_for_eco1_SHIFT     7
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_reserved_for_eco1_DEFAULT   0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_OF_INTR_MASK     0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_OF_INTR_SHIFT    6
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_OF_INTR_DEFAULT  0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_LVL_INTR_MASK    0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_LVL_INTR_SHIFT   5
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_INACT_INTR_MASK  0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT3_INTR_MASK    0x00000008
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT3_INTR_SHIFT   3
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT2_INTR_MASK    0x00000004
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT2_INTR_SHIFT   2
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT1_INTR_MASK    0x00000002
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT1_INTR_SHIFT   1
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_SET :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT0_INTR_MASK    0x00000001
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT0_INTR_SHIFT   0
#define BCHP_UPG_AUX_AON_INTR2_PCI_SET_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *PCI_CLEAR - PCI interrupt Clear Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_reserved0_MASK            0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_reserved0_SHIFT           9

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_WKTMR_ALARM_INTR_MASK     0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_WKTMR_ALARM_INTR_SHIFT    8
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_WKTMR_ALARM_INTR_DEFAULT  0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_reserved_for_eco1_MASK    0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_reserved_for_eco1_SHIFT   7
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_OF_INTR_MASK   0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_OF_INTR_SHIFT  6
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_OF_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_FIFO_LVL_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_LVL_INTR_MASK  0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_LVL_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_LVL_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_FIFO_INACT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_INACT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_INACT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_FIFO_INACT_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_TIMEOUT3_INTR [03:03] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT3_INTR_MASK  0x00000008
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT3_INTR_SHIFT 3
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT3_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_TIMEOUT2_INTR [02:02] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT2_INTR_MASK  0x00000004
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT2_INTR_SHIFT 2
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT2_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_TIMEOUT1_INTR [01:01] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT1_INTR_MASK  0x00000002
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT1_INTR_SHIFT 1
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT1_INTR_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_CLEAR :: BICAP_TIMEOUT0_INTR [00:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT0_INTR_MASK  0x00000001
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT0_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_PCI_CLEAR_BICAP_TIMEOUT0_INTR_DEFAULT 0x00000000

/***************************************************************************
 *PCI_MASK_STATUS - PCI interrupt Mask Status Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_reserved0_MASK      0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_reserved0_SHIFT     9

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_WKTMR_ALARM_INTR_MASK 0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_STATUS :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_STATUS_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_SET - PCI interrupt Mask Set Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_reserved0_MASK         0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_reserved0_SHIFT        9

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_WKTMR_ALARM_INTR_MASK  0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_SET :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_SET_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_CLEAR - PCI interrupt Mask Clear Register
 ***************************************************************************/
/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: reserved0 [31:09] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_reserved0_MASK       0xfffffe00
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_reserved0_SHIFT      9

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: WKTMR_ALARM_INTR [08:08] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_WKTMR_ALARM_INTR_MASK 0x00000100
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_WKTMR_ALARM_INTR_SHIFT 8
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_WKTMR_ALARM_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: reserved_for_eco1 [07:07] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_reserved_for_eco1_MASK 0x00000080
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_reserved_for_eco1_SHIFT 7
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_reserved_for_eco1_DEFAULT 0x00000000

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: BICAP_FIFO_OF_INTR [06:06] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_OF_INTR_MASK 0x00000040
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_OF_INTR_SHIFT 6
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_OF_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: BICAP_FIFO_IRQ_INTR [05:05] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_MASK 0x00000020
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_SHIFT 5
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_FIFO_IRQ_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: BICAP_CPU_TIMEOUT_INTR [04:04] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_MASK 0x00000010
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_SHIFT 4
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_CPU_TIMEOUT_INTR_DEFAULT 0x00000001

/* UPG_AUX_AON_INTR2 :: PCI_MASK_CLEAR :: BICAP_EDGE_TIMEOUT_INTR [03:00] */
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_MASK 0x0000000f
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_SHIFT 0
#define BCHP_UPG_AUX_AON_INTR2_PCI_MASK_CLEAR_BICAP_EDGE_TIMEOUT_INTR_DEFAULT 0x00000001

#endif /* #ifndef BCHP_UPG_AUX_AON_INTR2_H__ */

/* End of File */
