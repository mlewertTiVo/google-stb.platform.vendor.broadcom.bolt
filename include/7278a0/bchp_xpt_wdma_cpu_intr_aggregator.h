/********************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
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
 * Date:           Generated on               Fri Feb  3 03:58:03 2017
 *                 Full Compile MD5 Checksum  a1c6c8febe8655e5f88617a38e9f3696
 *                     (minus title and desc)
 *                 MD5 Checksum               918b80f3bbb6ba9c70cb6227fdd48d46
 *
 * lock_release:   r_1099
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     1255
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *                 Script Source              projects/stbgit/bin/gen_rdb.pl
 *                 DVTSWVER                   LOCAL
 *
 *
********************************************************************************/

#ifndef BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_H__
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_H__

/***************************************************************************
 *XPT_WDMA_CPU_INTR_AGGREGATOR - CPU Interrupt Aggregator
 ***************************************************************************/
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS 0x02280020 /* [RO][32] Interrupt Status Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_STATUS 0x02280024 /* [RO][32] Interrupt Status Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS 0x02280028 /* [RO][32] Interrupt Mask Status Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_STATUS 0x0228002c /* [RO][32] Interrupt Mask Status Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET 0x02280030 /* [WO][32] Interrupt Mask Set Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_SET 0x02280034 /* [WO][32] Interrupt Mask Set Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR 0x02280038 /* [WO][32] Interrupt Mask Clear Register */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_CLEAR 0x0228003c /* [WO][32] Interrupt Mask Clear Register */

/***************************************************************************
 *INTR_W0_STATUS - Interrupt Status Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_reserved0_MASK 0xffffffe0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_reserved0_SHIFT 5

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: END_OF_SEQ_INTR [04:04] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_END_OF_SEQ_INTR_MASK 0x00000010
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_END_OF_SEQ_INTR_SHIFT 4
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_END_OF_SEQ_INTR_DEFAULT 0x00000000

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: PM_INTR [03:03] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_PM_INTR_MASK 0x00000008
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_PM_INTR_SHIFT 3

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: BTP_INTR [02:02] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_BTP_INTR_MASK 0x00000004
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_BTP_INTR_SHIFT 2
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_BTP_INTR_DEFAULT 0x00000000

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: OVERFLOW_INTR [01:01] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_OVERFLOW_INTR_MASK 0x00000002
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_OVERFLOW_INTR_SHIFT 1
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_OVERFLOW_INTR_DEFAULT 0x00000000

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_STATUS :: DESC_DONE_INTR [00:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_DESC_DONE_INTR_MASK 0x00000001
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_DESC_DONE_INTR_SHIFT 0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_STATUS_DESC_DONE_INTR_DEFAULT 0x00000000

/***************************************************************************
 *INTR_W1_STATUS - Interrupt Status Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W1_STATUS :: reserved0 [31:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_STATUS_reserved0_MASK 0xffffffff
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_STATUS_reserved0_SHIFT 0

/***************************************************************************
 *INTR_W0_MASK_STATUS - Interrupt Mask Status Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_reserved0_MASK 0xffffffe0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_reserved0_SHIFT 5

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: END_OF_SEQ_INTR [04:04] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_END_OF_SEQ_INTR_MASK 0x00000010
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_END_OF_SEQ_INTR_SHIFT 4
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_END_OF_SEQ_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: PM_INTR [03:03] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_PM_INTR_MASK 0x00000008
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_PM_INTR_SHIFT 3

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: BTP_INTR [02:02] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_BTP_INTR_MASK 0x00000004
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_BTP_INTR_SHIFT 2
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_BTP_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: OVERFLOW_INTR [01:01] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_OVERFLOW_INTR_MASK 0x00000002
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_OVERFLOW_INTR_SHIFT 1
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_OVERFLOW_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_STATUS :: DESC_DONE_INTR [00:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_DESC_DONE_INTR_MASK 0x00000001
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_DESC_DONE_INTR_SHIFT 0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_STATUS_DESC_DONE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *INTR_W1_MASK_STATUS - Interrupt Mask Status Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W1_MASK_STATUS :: reserved0 [31:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_STATUS_reserved0_MASK 0xffffffff
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_STATUS_reserved0_SHIFT 0

/***************************************************************************
 *INTR_W0_MASK_SET - Interrupt Mask Set Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_reserved0_MASK 0xffffffe0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_reserved0_SHIFT 5

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: END_OF_SEQ_INTR [04:04] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_END_OF_SEQ_INTR_MASK 0x00000010
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_END_OF_SEQ_INTR_SHIFT 4
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_END_OF_SEQ_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: PM_INTR [03:03] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_PM_INTR_MASK 0x00000008
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_PM_INTR_SHIFT 3

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: BTP_INTR [02:02] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_BTP_INTR_MASK 0x00000004
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_BTP_INTR_SHIFT 2
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_BTP_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: OVERFLOW_INTR [01:01] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_OVERFLOW_INTR_MASK 0x00000002
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_OVERFLOW_INTR_SHIFT 1
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_OVERFLOW_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_SET :: DESC_DONE_INTR [00:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_DESC_DONE_INTR_MASK 0x00000001
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_DESC_DONE_INTR_SHIFT 0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_SET_DESC_DONE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *INTR_W1_MASK_SET - Interrupt Mask Set Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W1_MASK_SET :: reserved0 [31:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_SET_reserved0_MASK 0xffffffff
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_SET_reserved0_SHIFT 0

/***************************************************************************
 *INTR_W0_MASK_CLEAR - Interrupt Mask Clear Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_reserved0_MASK 0xffffffe0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_reserved0_SHIFT 5

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: END_OF_SEQ_INTR [04:04] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_END_OF_SEQ_INTR_MASK 0x00000010
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_END_OF_SEQ_INTR_SHIFT 4
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_END_OF_SEQ_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: PM_INTR [03:03] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_PM_INTR_MASK 0x00000008
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_PM_INTR_SHIFT 3

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: BTP_INTR [02:02] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_BTP_INTR_MASK 0x00000004
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_BTP_INTR_SHIFT 2
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_BTP_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: OVERFLOW_INTR [01:01] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_OVERFLOW_INTR_MASK 0x00000002
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_OVERFLOW_INTR_SHIFT 1
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_OVERFLOW_INTR_DEFAULT 0x00000001

/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W0_MASK_CLEAR :: DESC_DONE_INTR [00:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_DESC_DONE_INTR_MASK 0x00000001
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_DESC_DONE_INTR_SHIFT 0
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR_DESC_DONE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *INTR_W1_MASK_CLEAR - Interrupt Mask Clear Register
 ***************************************************************************/
/* XPT_WDMA_CPU_INTR_AGGREGATOR :: INTR_W1_MASK_CLEAR :: reserved0 [31:00] */
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_CLEAR_reserved0_MASK 0xffffffff
#define BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W1_MASK_CLEAR_reserved0_SHIFT 0

#endif /* #ifndef BCHP_XPT_WDMA_CPU_INTR_AGGREGATOR_H__ */

/* End of File */
