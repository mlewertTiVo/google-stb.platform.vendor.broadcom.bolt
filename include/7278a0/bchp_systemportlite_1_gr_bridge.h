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
 * Date:           Generated on               Thu Dec  1 06:46:35 2016
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

#ifndef BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_H__
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_H__

/***************************************************************************
 *SYSTEMPORTLITE_1_GR_BRIDGE
 ***************************************************************************/
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION 0x01217800 /* [RO][32] GR Bridge Revision */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL     0x01217804 /* [CFG][32] GR Bridge Control Register */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD       0x01217808 /* [CFG][32] GR Bridge Power Down Register */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0 0x0121780c /* [CFG][32] GR Bridge Software Init 0 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GR_BRIDGE :: REVISION :: reserved0 [31:16] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_reserved0_MASK    0xffff0000
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_reserved0_SHIFT   16

/* SYSTEMPORTLITE_1_GR_BRIDGE :: REVISION :: MAJOR [15:08] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MAJOR_MASK        0x0000ff00
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MAJOR_SHIFT       8
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MAJOR_DEFAULT     0x00000007

/* SYSTEMPORTLITE_1_GR_BRIDGE :: REVISION :: MINOR [07:00] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MINOR_MASK        0x000000ff
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MINOR_SHIFT       0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_REVISION_MINOR_DEFAULT     0x00000000

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GR_BRIDGE :: CTRL :: reserved0 [31:01] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_reserved0_MASK        0xfffffffe
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_reserved0_SHIFT       1

/* SYSTEMPORTLITE_1_GR_BRIDGE :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_gisb_error_intr_MASK  0x00000001
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_gisb_error_intr_SHIFT 0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_gisb_error_intr_INTR_DISABLE 0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_CTRL_gisb_error_intr_INTR_ENABLE 1

/***************************************************************************
 *PD - GR Bridge Power Down Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GR_BRIDGE :: PD :: reserved0 [31:01] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD_reserved0_MASK          0xfffffffe
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD_reserved0_SHIFT         1

/* SYSTEMPORTLITE_1_GR_BRIDGE :: PD :: power_down [00:00] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD_power_down_MASK         0x00000001
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD_power_down_SHIFT        0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_PD_power_down_DEFAULT      0x00000000

/***************************************************************************
 *SW_INIT_0 - GR Bridge Software Init 0 Register
 ***************************************************************************/
/* SYSTEMPORTLITE_1_GR_BRIDGE :: SW_INIT_0 :: reserved0 [31:01] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_reserved0_MASK   0xfffffffe
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_reserved0_SHIFT  1

/* SYSTEMPORTLITE_1_GR_BRIDGE :: SW_INIT_0 :: SPARE_SW_INIT [00:00] */
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_SPARE_SW_INIT_MASK 0x00000001
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_SPARE_SW_INIT_SHIFT 0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_SPARE_SW_INIT_DEFAULT 0x00000000
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_SPARE_SW_INIT_DEASSERT 0
#define BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_SW_INIT_0_SPARE_SW_INIT_ASSERT 1

#endif /* #ifndef BCHP_SYSTEMPORTLITE_1_GR_BRIDGE_H__ */

/* End of File */