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
 * Date:           Generated on               Sun Nov 19 01:07:16 2017
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

#ifndef BCHP_DDR34_PHY_PHYBIST_0_H__
#define BCHP_DDR34_PHY_PHYBIST_0_H__

/***************************************************************************
 *DDR34_PHY_PHYBIST_0 - DDR34 self-test registers
 ***************************************************************************/
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL         0x0021122000 /* [RW][32] PhyBist Control Register */
#define BCHP_DDR34_PHY_PHYBIST_0_SEED            0x0021122004 /* [RW][32] PhyBist Seed Register */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK         0x0021122008 /* [RW][32] PhyBist Command/Address Bus Mask */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0        0x002112200c /* [RW][32] PhyBist Data Bus Mask for Byte Lane #0 */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1        0x0021122010 /* [RW][32] PhyBist Data Bus Mask for Byte Lane #1 */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2        0x0021122014 /* [RW][32] PhyBist Data Bus Mask for Byte Lane #2 */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3        0x0021122018 /* [RW][32] PhyBist Data Bus Mask for Byte Lane #3 */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS          0x0021122030 /* [RO][32] PhyBist General Status Register */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS      0x0021122034 /* [RO][32] PhyBist Per-Bit Control Pad Status Register */
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS      0x0021122038 /* [RO][32] PhyBist Byte Lane #0 Status Register */
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS      0x002112203c /* [RO][32] PhyBist Byte Lane #1 Status Register */
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS      0x0021122040 /* [RO][32] PhyBist Byte Lane #2 Status Register */
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS      0x0021122044 /* [RO][32] PhyBist Byte Lane #3 Status Register */

/***************************************************************************
 *CONTROL - PhyBist Control Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: CONTROL :: reserved0 [31:30] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_reserved0_MASK            0xc0000000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_reserved0_SHIFT           30

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: PATTERN_SEL [29:28] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_PATTERN_SEL_MASK          0x30000000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_PATTERN_SEL_SHIFT         28
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_PATTERN_SEL_DEFAULT       0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: FORCE_DQ_ERROR_SEL [27:24] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DQ_ERROR_SEL_MASK   0x0f000000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DQ_ERROR_SEL_SHIFT  24
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DQ_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: FORCE_BL_ERROR_SEL [23:20] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_BL_ERROR_SEL_MASK   0x00f00000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_BL_ERROR_SEL_SHIFT  20
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_BL_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: reserved_for_eco1 [19:18] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_reserved_for_eco1_MASK    0x000c0000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_reserved_for_eco1_SHIFT   18
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_reserved_for_eco1_DEFAULT 0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: CA_DDR [17:17] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_DDR_MASK               0x00020000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_DDR_SHIFT              17
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_DDR_DEFAULT            0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: FORCE_CTL_ERROR_SEL [16:12] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_SEL_MASK  0x0001f000
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_SEL_SHIFT 12
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: FORCE_DAT_ERROR [11:11] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DAT_ERROR_MASK      0x00000800
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DAT_ERROR_SHIFT     11
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_DAT_ERROR_DEFAULT   0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: FORCE_CTL_ERROR [10:10] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_MASK      0x00000400
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_SHIFT     10
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_FORCE_CTL_ERROR_DEFAULT   0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: DATA_ONLY [09:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_DATA_ONLY_MASK            0x00000200
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_DATA_ONLY_SHIFT           9
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_DATA_ONLY_DEFAULT         0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: CA_ONLY [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_ONLY_MASK              0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_ONLY_SHIFT             8
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_CA_ONLY_DEFAULT           0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: SSO [07:06] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_SSO_MASK                  0x000000c0
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_SSO_SHIFT                 6
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_SSO_DEFAULT               0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: LENGTH [05:04] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_LENGTH_MASK               0x00000030
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_LENGTH_SHIFT              4
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_LENGTH_DEFAULT            0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: MODE [03:01] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_MODE_MASK                 0x0000000e
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_MODE_SHIFT                1
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_MODE_DEFAULT              0x00000000

/* DDR34_PHY_PHYBIST_0 :: CONTROL :: ENABLE [00:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_ENABLE_MASK               0x00000001
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_ENABLE_SHIFT              0
#define BCHP_DDR34_PHY_PHYBIST_0_CONTROL_ENABLE_DEFAULT            0x00000000

/***************************************************************************
 *SEED - PhyBist Seed Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: SEED :: SEED [31:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_SEED_SEED_MASK                    0xffffffff
#define BCHP_DDR34_PHY_PHYBIST_0_SEED_SEED_SHIFT                   0
#define BCHP_DDR34_PHY_PHYBIST_0_SEED_SEED_DEFAULT                 0xba5eba11

/***************************************************************************
 *CA_MASK - PhyBist Command/Address Bus Mask
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: ALERT_N [31:31] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ALERT_N_MASK              0x80000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ALERT_N_SHIFT             31
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ALERT_N_DEFAULT           0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: CS0_N [30:30] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS0_N_MASK                0x40000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS0_N_SHIFT               30
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS0_N_DEFAULT             0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: CS1_N [29:29] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS1_N_MASK                0x20000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS1_N_SHIFT               29
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CS1_N_DEFAULT             0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: RAS_N [28:28] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RAS_N_MASK                0x10000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RAS_N_SHIFT               28
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RAS_N_DEFAULT             0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: CAS_N [27:27] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CAS_N_MASK                0x08000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CAS_N_SHIFT               27
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CAS_N_DEFAULT             0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: WE_N [26:26] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_WE_N_MASK                 0x04000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_WE_N_SHIFT                26
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_WE_N_DEFAULT              0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: CKE [25:25] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CKE_MASK                  0x02000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CKE_SHIFT                 25
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_CKE_DEFAULT               0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: PAR [24:24] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_PAR_MASK                  0x01000000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_PAR_SHIFT                 24
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_PAR_DEFAULT               0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: ODT [23:23] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ODT_MASK                  0x00800000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ODT_SHIFT                 23
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_ODT_DEFAULT               0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: RST_N [22:22] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RST_N_MASK                0x00400000
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RST_N_SHIFT               22
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_RST_N_DEFAULT             0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: AD [21:06] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AD_MASK                   0x003fffc0
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AD_SHIFT                  6
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AD_DEFAULT                0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: BA [05:03] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_BA_MASK                   0x00000038
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_BA_SHIFT                  3
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_BA_DEFAULT                0x00000000

/* DDR34_PHY_PHYBIST_0 :: CA_MASK :: AUX [02:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AUX_MASK                  0x00000007
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AUX_SHIFT                 0
#define BCHP_DDR34_PHY_PHYBIST_0_CA_MASK_AUX_DEFAULT               0x00000000

/***************************************************************************
 *DQ_MASK0 - PhyBist Data Bus Mask for Byte Lane #0
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: DQ_MASK0 :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_reserved0_MASK           0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_reserved0_SHIFT          9

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK0 :: DM_MASK [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DM_MASK_MASK             0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DM_MASK_SHIFT            8
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DM_MASK_DEFAULT          0x00000000

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK0 :: DQ_MASK [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DQ_MASK_MASK             0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DQ_MASK_SHIFT            0
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK0_DQ_MASK_DEFAULT          0x00000000

/***************************************************************************
 *DQ_MASK1 - PhyBist Data Bus Mask for Byte Lane #1
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: DQ_MASK1 :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_reserved0_MASK           0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_reserved0_SHIFT          9

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK1 :: DM_MASK [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DM_MASK_MASK             0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DM_MASK_SHIFT            8
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DM_MASK_DEFAULT          0x00000000

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK1 :: DQ_MASK [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DQ_MASK_MASK             0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DQ_MASK_SHIFT            0
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK1_DQ_MASK_DEFAULT          0x00000000

/***************************************************************************
 *DQ_MASK2 - PhyBist Data Bus Mask for Byte Lane #2
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: DQ_MASK2 :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_reserved0_MASK           0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_reserved0_SHIFT          9

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK2 :: DM_MASK [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DM_MASK_MASK             0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DM_MASK_SHIFT            8
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DM_MASK_DEFAULT          0x00000000

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK2 :: DQ_MASK [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DQ_MASK_MASK             0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DQ_MASK_SHIFT            0
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK2_DQ_MASK_DEFAULT          0x00000000

/***************************************************************************
 *DQ_MASK3 - PhyBist Data Bus Mask for Byte Lane #3
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: DQ_MASK3 :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_reserved0_MASK           0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_reserved0_SHIFT          9

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK3 :: DM_MASK [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DM_MASK_MASK             0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DM_MASK_SHIFT            8
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DM_MASK_DEFAULT          0x00000000

/* DDR34_PHY_PHYBIST_0 :: DQ_MASK3 :: DQ_MASK [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DQ_MASK_MASK             0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DQ_MASK_SHIFT            0
#define BCHP_DDR34_PHY_PHYBIST_0_DQ_MASK3_DQ_MASK_DEFAULT          0x00000000

/***************************************************************************
 *STATUS - PhyBist General Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: STATUS :: reserved0 [31:04] */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_reserved0_MASK             0xfffffff0
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_reserved0_SHIFT            4

/* DDR34_PHY_PHYBIST_0 :: STATUS :: DAT_PASS [03:03] */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_DAT_PASS_MASK              0x00000008
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_DAT_PASS_SHIFT             3

/* DDR34_PHY_PHYBIST_0 :: STATUS :: CTL_PASS [02:02] */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_CTL_PASS_MASK              0x00000004
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_CTL_PASS_SHIFT             2

/* DDR34_PHY_PHYBIST_0 :: STATUS :: DAT_DONE [01:01] */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_DAT_DONE_MASK              0x00000002
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_DAT_DONE_SHIFT             1

/* DDR34_PHY_PHYBIST_0 :: STATUS :: CTL_DONE [00:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_CTL_DONE_MASK              0x00000001
#define BCHP_DDR34_PHY_PHYBIST_0_STATUS_CTL_DONE_SHIFT             0

/***************************************************************************
 *CTL_STATUS - PhyBist Per-Bit Control Pad Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: ALERT_N [31:31] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_ALERT_N_MASK           0x80000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_ALERT_N_SHIFT          31

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: CS0_N [30:30] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CS0_N_MASK             0x40000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CS0_N_SHIFT            30

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: CS1_N [29:29] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CS1_N_MASK             0x20000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CS1_N_SHIFT            29

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: RAS_N [28:28] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_RAS_N_MASK             0x10000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_RAS_N_SHIFT            28

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: CAS_N [27:27] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CAS_N_MASK             0x08000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CAS_N_SHIFT            27

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: WE_N [26:26] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_WE_N_MASK              0x04000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_WE_N_SHIFT             26

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: CKE [25:25] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CKE_MASK               0x02000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_CKE_SHIFT              25

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: PAR [24:24] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_PAR_MASK               0x01000000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_PAR_SHIFT              24

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: ODT [23:23] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_ODT_MASK               0x00800000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_ODT_SHIFT              23

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: RST_N [22:22] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_RST_N_MASK             0x00400000
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_RST_N_SHIFT            22

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: AD [21:06] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_AD_MASK                0x003fffc0
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_AD_SHIFT               6

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: BA [05:03] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_BA_MASK                0x00000038
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_BA_SHIFT               3

/* DDR34_PHY_PHYBIST_0 :: CTL_STATUS :: AUX [02:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_AUX_MASK               0x00000007
#define BCHP_DDR34_PHY_PHYBIST_0_CTL_STATUS_AUX_SHIFT              0

/***************************************************************************
 *BL0_STATUS - PhyBist Byte Lane #0 Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: BL0_STATUS :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_reserved0_MASK         0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_reserved0_SHIFT        9

/* DDR34_PHY_PHYBIST_0 :: BL0_STATUS :: DM [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_DM_MASK                0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_DM_SHIFT               8

/* DDR34_PHY_PHYBIST_0 :: BL0_STATUS :: DQ [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_DQ_MASK                0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_BL0_STATUS_DQ_SHIFT               0

/***************************************************************************
 *BL1_STATUS - PhyBist Byte Lane #1 Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: BL1_STATUS :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_reserved0_MASK         0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_reserved0_SHIFT        9

/* DDR34_PHY_PHYBIST_0 :: BL1_STATUS :: DM [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_DM_MASK                0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_DM_SHIFT               8

/* DDR34_PHY_PHYBIST_0 :: BL1_STATUS :: DQ [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_DQ_MASK                0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_BL1_STATUS_DQ_SHIFT               0

/***************************************************************************
 *BL2_STATUS - PhyBist Byte Lane #2 Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: BL2_STATUS :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_reserved0_MASK         0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_reserved0_SHIFT        9

/* DDR34_PHY_PHYBIST_0 :: BL2_STATUS :: DM [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_DM_MASK                0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_DM_SHIFT               8

/* DDR34_PHY_PHYBIST_0 :: BL2_STATUS :: DQ [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_DQ_MASK                0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_BL2_STATUS_DQ_SHIFT               0

/***************************************************************************
 *BL3_STATUS - PhyBist Byte Lane #3 Status Register
 ***************************************************************************/
/* DDR34_PHY_PHYBIST_0 :: BL3_STATUS :: reserved0 [31:09] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_reserved0_MASK         0xfffffe00
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_reserved0_SHIFT        9

/* DDR34_PHY_PHYBIST_0 :: BL3_STATUS :: DM [08:08] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_DM_MASK                0x00000100
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_DM_SHIFT               8

/* DDR34_PHY_PHYBIST_0 :: BL3_STATUS :: DQ [07:00] */
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_DQ_MASK                0x000000ff
#define BCHP_DDR34_PHY_PHYBIST_0_BL3_STATUS_DQ_SHIFT               0

#endif /* #ifndef BCHP_DDR34_PHY_PHYBIST_0_H__ */

/* End of File */
