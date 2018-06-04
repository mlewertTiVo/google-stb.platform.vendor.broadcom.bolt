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
 * Date:           Generated on               Sun Nov 19 01:07:17 2017
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

#ifndef BCHP_PCIE_0_RGR1_H__
#define BCHP_PCIE_0_RGR1_H__

/***************************************************************************
 *PCIE_0_RGR1 - PCIe RBUS-GISB-RBUS Bridge Registers
 ***************************************************************************/
#define BCHP_PCIE_0_RGR1_REVISION                0x0020469200 /* [RO][32] RGR Bridge Revision */
#define BCHP_PCIE_0_RGR1_CTRL                    0x0020469204 /* [RW][32] RGR Bridge Control Register */
#define BCHP_PCIE_0_RGR1_RBUS_TIMER              0x0020469208 /* [RW][32] RGR Bridge RBUS Timer Register */
#define BCHP_PCIE_0_RGR1_SW_INIT_0               0x002046920c /* [RW][32] RGR Bridge Software Reset 0 Register */
#define BCHP_PCIE_0_RGR1_SW_INIT_1               0x0020469210 /* [RW][32] RGR Bridge Software Reset 1 Register */

/***************************************************************************
 *REVISION - RGR Bridge Revision
 ***************************************************************************/
/* PCIE_0_RGR1 :: REVISION :: reserved0 [31:16] */
#define BCHP_PCIE_0_RGR1_REVISION_reserved0_MASK                   0xffff0000
#define BCHP_PCIE_0_RGR1_REVISION_reserved0_SHIFT                  16

/* PCIE_0_RGR1 :: REVISION :: MAJOR [15:08] */
#define BCHP_PCIE_0_RGR1_REVISION_MAJOR_MASK                       0x0000ff00
#define BCHP_PCIE_0_RGR1_REVISION_MAJOR_SHIFT                      8
#define BCHP_PCIE_0_RGR1_REVISION_MAJOR_DEFAULT                    0x00000002

/* PCIE_0_RGR1 :: REVISION :: MINOR [07:00] */
#define BCHP_PCIE_0_RGR1_REVISION_MINOR_MASK                       0x000000ff
#define BCHP_PCIE_0_RGR1_REVISION_MINOR_SHIFT                      0
#define BCHP_PCIE_0_RGR1_REVISION_MINOR_DEFAULT                    0x00000000

/***************************************************************************
 *CTRL - RGR Bridge Control Register
 ***************************************************************************/
/* PCIE_0_RGR1 :: CTRL :: reserved0 [31:02] */
#define BCHP_PCIE_0_RGR1_CTRL_reserved0_MASK                       0xfffffffc
#define BCHP_PCIE_0_RGR1_CTRL_reserved0_SHIFT                      2

/* PCIE_0_RGR1 :: CTRL :: rbus_error_intr [01:01] */
#define BCHP_PCIE_0_RGR1_CTRL_rbus_error_intr_MASK                 0x00000002
#define BCHP_PCIE_0_RGR1_CTRL_rbus_error_intr_SHIFT                1
#define BCHP_PCIE_0_RGR1_CTRL_rbus_error_intr_DEFAULT              0x00000000
#define BCHP_PCIE_0_RGR1_CTRL_rbus_error_intr_INTR_DISABLE         0
#define BCHP_PCIE_0_RGR1_CTRL_rbus_error_intr_INTR_ENABLE          1

/* PCIE_0_RGR1 :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_PCIE_0_RGR1_CTRL_gisb_error_intr_MASK                 0x00000001
#define BCHP_PCIE_0_RGR1_CTRL_gisb_error_intr_SHIFT                0
#define BCHP_PCIE_0_RGR1_CTRL_gisb_error_intr_DEFAULT              0x00000000
#define BCHP_PCIE_0_RGR1_CTRL_gisb_error_intr_INTR_DISABLE         0
#define BCHP_PCIE_0_RGR1_CTRL_gisb_error_intr_INTR_ENABLE          1

/***************************************************************************
 *RBUS_TIMER - RGR Bridge RBUS Timer Register
 ***************************************************************************/
/* PCIE_0_RGR1 :: RBUS_TIMER :: timer_value [31:00] */
#define BCHP_PCIE_0_RGR1_RBUS_TIMER_timer_value_MASK               0xffffffff
#define BCHP_PCIE_0_RGR1_RBUS_TIMER_timer_value_SHIFT              0
#define BCHP_PCIE_0_RGR1_RBUS_TIMER_timer_value_DEFAULT            0x0e297d00

/***************************************************************************
 *SW_INIT_0 - RGR Bridge Software Reset 0 Register
 ***************************************************************************/
/* PCIE_0_RGR1 :: SW_INIT_0 :: reserved0 [31:01] */
#define BCHP_PCIE_0_RGR1_SW_INIT_0_reserved0_MASK                  0xfffffffe
#define BCHP_PCIE_0_RGR1_SW_INIT_0_reserved0_SHIFT                 1

/* PCIE_0_RGR1 :: SW_INIT_0 :: SPARE_SW_INIT [00:00] */
#define BCHP_PCIE_0_RGR1_SW_INIT_0_SPARE_SW_INIT_MASK              0x00000001
#define BCHP_PCIE_0_RGR1_SW_INIT_0_SPARE_SW_INIT_SHIFT             0
#define BCHP_PCIE_0_RGR1_SW_INIT_0_SPARE_SW_INIT_DEFAULT           0x00000000
#define BCHP_PCIE_0_RGR1_SW_INIT_0_SPARE_SW_INIT_DEASSERT          0
#define BCHP_PCIE_0_RGR1_SW_INIT_0_SPARE_SW_INIT_ASSERT            1

/***************************************************************************
 *SW_INIT_1 - RGR Bridge Software Reset 1 Register
 ***************************************************************************/
/* PCIE_0_RGR1 :: SW_INIT_1 :: reserved0 [31:02] */
#define BCHP_PCIE_0_RGR1_SW_INIT_1_reserved0_MASK                  0xfffffffc
#define BCHP_PCIE_0_RGR1_SW_INIT_1_reserved0_SHIFT                 2

/* PCIE_0_RGR1 :: SW_INIT_1 :: PCIE_BRIDGE_SW_INIT [01:01] */
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_BRIDGE_SW_INIT_MASK        0x00000002
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_BRIDGE_SW_INIT_SHIFT       1
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_BRIDGE_SW_INIT_DEFAULT     0x00000001
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_BRIDGE_SW_INIT_DEASSERT    0
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_BRIDGE_SW_INIT_ASSERT      1

/* PCIE_0_RGR1 :: SW_INIT_1 :: PCIE_SW_PERST [00:00] */
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_SW_PERST_MASK              0x00000001
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_SW_PERST_SHIFT             0
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_SW_PERST_DEFAULT           0x00000001
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_SW_PERST_DEASSERT          0
#define BCHP_PCIE_0_RGR1_SW_INIT_1_PCIE_SW_PERST_ASSERT            1

#endif /* #ifndef BCHP_PCIE_0_RGR1_H__ */

/* End of File */
