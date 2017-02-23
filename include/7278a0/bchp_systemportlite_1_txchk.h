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
 * Date:           Generated on               Thu Dec  1 06:45:31 2016
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

#ifndef BCHP_SYSTEMPORTLITE_1_TXCHK_H__
#define BCHP_SYSTEMPORTLITE_1_TXCHK_H__

/***************************************************************************
 *SYSTEMPORTLITE_1_TXCHK
 ***************************************************************************/
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD 0x01400380 /* [RW][32] TXCHK Packet Ready Threshold Register */

/***************************************************************************
 *TXCHK_PKT_RDY_THRESHOLD - TXCHK Packet Ready Threshold Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_TXCHK :: TXCHK_PKT_RDY_THRESHOLD :: reserved0 [31:08] */
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD_reserved0_MASK 0xffffff00
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD_reserved0_SHIFT 8

/* SYSTEMPORTLITE_1_TXCHK :: TXCHK_PKT_RDY_THRESHOLD :: pkt_rdy_threshold [07:00] */
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_MASK 0x000000ff
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_SHIFT 0
#define BCHP_SYSTEMPORTLITE_1_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_DEFAULT 0x00000080

#endif /* #ifndef BCHP_SYSTEMPORTLITE_1_TXCHK_H__ */

/* End of File */
