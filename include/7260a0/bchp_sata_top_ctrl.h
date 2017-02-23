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
 * Date:           Generated on               Wed Jul 20 03:09:21 2016
 *                 Full Compile MD5 Checksum  a2b82ec5680f6066255d5d5e4c190ff8
 *                     (minus title and desc)
 *                 MD5 Checksum               5e375d275648c6fb0816b8b6e9d8b76e
 *
 * lock_release:   n/a
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     1066
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *                 Script Source              /projects/stbgit/bin/gen_rdb.pl
 *                 DVTSWVER                   n/a
 *
 *
********************************************************************************/

#ifndef BCHP_SATA_TOP_CTRL_H__
#define BCHP_SATA_TOP_CTRL_H__

/***************************************************************************
 *SATA_TOP_CTRL - SATA3 MISC Control Registers
 ***************************************************************************/
#define BCHP_SATA_TOP_CTRL_Version               0x20b10040 /* [RO] SATA3 Version Register */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL              0x20b10044 /* [RW] SATA3 System Bus Access Control Register */
#define BCHP_SATA_TOP_CTRL_TP_CTRL               0x20b10048 /* [RW] SATA3 Test Port Control register */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1            0x20b1004c /* [RW] SATA3 Port 0 phy interface control 1 */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2            0x20b10050 /* [RW] SATA3 Port 0 phy interface control 2 */
#define BCHP_SATA_TOP_CTRL_SATA_TP_OUT           0x20b1005c /* [RO] SATA3 TP OUT */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL      0x20b10060 /* [RW] SATA3 CLIENT INIT */

/***************************************************************************
 *Version - SATA3 Version Register
 ***************************************************************************/
/* SATA_TOP_CTRL :: Version :: reserved0 [31:08] */
#define BCHP_SATA_TOP_CTRL_Version_reserved0_MASK                  0xffffff00
#define BCHP_SATA_TOP_CTRL_Version_reserved0_SHIFT                 8

/* SATA_TOP_CTRL :: Version :: major_version [07:04] */
#define BCHP_SATA_TOP_CTRL_Version_major_version_MASK              0x000000f0
#define BCHP_SATA_TOP_CTRL_Version_major_version_SHIFT             4
#define BCHP_SATA_TOP_CTRL_Version_major_version_DEFAULT           0x00000001

/* SATA_TOP_CTRL :: Version :: minor_version [03:00] */
#define BCHP_SATA_TOP_CTRL_Version_minor_version_MASK              0x0000000f
#define BCHP_SATA_TOP_CTRL_Version_minor_version_SHIFT             0
#define BCHP_SATA_TOP_CTRL_Version_minor_version_DEFAULT           0x00000000

/***************************************************************************
 *BUS_CTRL - SATA3 System Bus Access Control Register
 ***************************************************************************/
/* SATA_TOP_CTRL :: BUS_CTRL :: bus_ctrl_31_17 [31:17] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_31_17_MASK            0xfffe0000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_31_17_SHIFT           17
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_31_17_DEFAULT         0x00000000

/* SATA_TOP_CTRL :: BUS_CTRL :: overide_hwinit_registers [16:16] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_overide_hwinit_registers_MASK  0x00010000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_overide_hwinit_registers_SHIFT 16
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_overide_hwinit_registers_DEFAULT 0x00000000

/* SATA_TOP_CTRL :: BUS_CTRL :: bus_ctrl_15_8 [15:08] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_15_8_MASK             0x0000ff00
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_15_8_SHIFT            8
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_bus_ctrl_15_8_DEFAULT          0x00000000

/* SATA_TOP_CTRL :: BUS_CTRL :: piodata_endian_ctrl [07:06] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_MASK       0x000000c0
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_SHIFT      6
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_DEFAULT    0x00000000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_SwapEndianess 2
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_HalfWordSwap 1
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_piodata_endian_ctrl_NoSwap     0

/* SATA_TOP_CTRL :: BUS_CTRL :: dmadata_endian_ctrl [05:04] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_MASK       0x00000030
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_SHIFT      4
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_DEFAULT    0x00000000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_SwapEndianess 2
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_HalfWordSwap 1
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadata_endian_ctrl_NoSwap     0

/* SATA_TOP_CTRL :: BUS_CTRL :: dmadesc_endian_ctrl [03:02] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_MASK       0x0000000c
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_SHIFT      2
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_DEFAULT    0x00000000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_SwapEndianess 2
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_HalfWordSwap 1
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_dmadesc_endian_ctrl_NoSwap     0

/* SATA_TOP_CTRL :: BUS_CTRL :: reg_endian_ctrl [01:00] */
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_MASK           0x00000003
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_SHIFT          0
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_DEFAULT        0x00000000
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_SwapEndianess  2
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_HalfWordSwap   1
#define BCHP_SATA_TOP_CTRL_BUS_CTRL_reg_endian_ctrl_NoSwap         0

/***************************************************************************
 *TP_CTRL - SATA3 Test Port Control register
 ***************************************************************************/
/* SATA_TOP_CTRL :: TP_CTRL :: tp_ctrl [31:00] */
#define BCHP_SATA_TOP_CTRL_TP_CTRL_tp_ctrl_MASK                    0xffffffff
#define BCHP_SATA_TOP_CTRL_TP_CTRL_tp_ctrl_SHIFT                   0
#define BCHP_SATA_TOP_CTRL_TP_CTRL_tp_ctrl_DEFAULT                 0x00000000

/***************************************************************************
 *PHY_CTRL_1 - SATA3 Port 0 phy interface control 1
 ***************************************************************************/
/* SATA_TOP_CTRL :: PHY_CTRL_1 :: PHY_DEBUG_CTRL_2 [31:15] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_2_MASK        0xffff8000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_2_SHIFT       15
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_2_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: PHY_DEFAULT_POWER_STATE [14:14] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEFAULT_POWER_STATE_MASK 0x00004000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEFAULT_POWER_STATE_SHIFT 14
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEFAULT_POWER_STATE_DEFAULT 0x00000001

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: EN_SATA_MDIO [13:13] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SATA_MDIO_MASK            0x00002000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SATA_MDIO_SHIFT           13
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SATA_MDIO_DEFAULT         0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_RST_PLL [12:12] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_PLL_MASK          0x00001000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_PLL_SHIFT         12
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_PLL_DEFAULT       0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: PHY_DEBUG_CTRL_1 [11:09] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_1_MASK        0x00000e00
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_1_SHIFT       9
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_1_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_SLUMBER [08:08] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_SLUMBER_MASK          0x00000100
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_SLUMBER_SHIFT         8
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_SLUMBER_DEFAULT       0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_PARTIAL [07:07] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_PARTIAL_MASK          0x00000080
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_PARTIAL_SHIFT         7
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_PARTIAL_DEFAULT       0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_TX_ENABLE [06:06] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_ENABLE_MASK        0x00000040
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_ENABLE_SHIFT       6
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_ENABLE_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_TX_FIFO_ON [05:05] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_FIFO_ON_MASK       0x00000020
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_FIFO_ON_SHIFT      5
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_TX_FIFO_ON_DEFAULT    0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_RST_TX [04:04] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_TX_MASK           0x00000010
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_TX_SHIFT          4
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_TX_DEFAULT        0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: DIS_HW_RST_RX [03:03] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_RX_MASK           0x00000008
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_RX_SHIFT          3
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_DIS_HW_RST_RX_DEFAULT        0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: PHY_DEBUG_CTRL_0 [02:01] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_0_MASK        0x00000006
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_0_SHIFT       1
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_PHY_DEBUG_CTRL_0_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_1 :: EN_SW_RST_PLL [00:00] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SW_RST_PLL_MASK           0x00000001
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SW_RST_PLL_SHIFT          0
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_1_EN_SW_RST_PLL_DEFAULT        0x00000000

/***************************************************************************
 *PHY_CTRL_2 - SATA3 Port 0 phy interface control 2
 ***************************************************************************/
/* SATA_TOP_CTRL :: PHY_CTRL_2 :: PHY_DEBUG_CTRL_4 [31:15] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_4_MASK        0xffff8000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_4_SHIFT       15
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_4_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: PHY_GLOBAL_RST [14:14] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_GLOBAL_RST_MASK          0x00004000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_GLOBAL_RST_SHIFT         14
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_GLOBAL_RST_DEFAULT       0x00000001

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: PHY_DEBUG_CTRL_3 [13:13] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_3_MASK        0x00002000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_3_SHIFT       13
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_3_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_EN_SLUMBER [12:12] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_SLUMBER_MASK           0x00001000
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_SLUMBER_SHIFT          12
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_SLUMBER_DEFAULT        0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_EN_PARTIAL [11:11] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_PARTIAL_MASK           0x00000800
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_PARTIAL_SHIFT          11
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_EN_PARTIAL_DEFAULT        0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_TX_ENABLE [10:10] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_ENABLE_MASK            0x00000400
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_ENABLE_SHIFT           10
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_ENABLE_DEFAULT         0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_TX_FIFO_ON [09:09] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_FIFO_ON_MASK           0x00000200
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_FIFO_ON_SHIFT          9
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_FIFO_ON_DEFAULT        0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: PHY_DEBUG_CTRL_2 [08:06] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_2_MASK        0x000001c0
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_2_SHIFT       6
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_PHY_DEBUG_CTRL_2_DEFAULT     0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_TX_PWRDN [05:05] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_PWRDN_MASK             0x00000020
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_PWRDN_SHIFT            5
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_TX_PWRDN_DEFAULT          0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_RX_PWRDN [04:04] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RX_PWRDN_MASK             0x00000010
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RX_PWRDN_SHIFT            4
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RX_PWRDN_DEFAULT          0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_RST_TX [03:03] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_TX_MASK               0x00000008
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_TX_SHIFT              3
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_TX_DEFAULT            0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_RST_RX [02:02] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_RX_MASK               0x00000004
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_RX_SHIFT              2
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_RX_DEFAULT            0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_RST_OOB [01:01] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_OOB_MASK              0x00000002
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_OOB_SHIFT             1
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_OOB_DEFAULT           0x00000000

/* SATA_TOP_CTRL :: PHY_CTRL_2 :: SW_RST_MDIOREG [00:00] */
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_MDIOREG_MASK          0x00000001
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_MDIOREG_SHIFT         0
#define BCHP_SATA_TOP_CTRL_PHY_CTRL_2_SW_RST_MDIOREG_DEFAULT       0x00000000

/***************************************************************************
 *SATA_TP_OUT - SATA3 TP OUT
 ***************************************************************************/
/* SATA_TOP_CTRL :: SATA_TP_OUT :: TP_OUT [31:00] */
#define BCHP_SATA_TOP_CTRL_SATA_TP_OUT_TP_OUT_MASK                 0xffffffff
#define BCHP_SATA_TOP_CTRL_SATA_TP_OUT_TP_OUT_SHIFT                0

/***************************************************************************
 *CLIENT_INIT_CTRL - SATA3 CLIENT INIT
 ***************************************************************************/
/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: reserved0 [31:11] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved0_MASK         0xfffff800
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved0_SHIFT        11

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port1_memc2_client_init_state [10:10] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc2_client_init_state_MASK 0x00000400
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc2_client_init_state_SHIFT 10

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port1_memc1_client_init_state [09:09] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc1_client_init_state_MASK 0x00000200
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc1_client_init_state_SHIFT 9

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port1_memc0_client_init_state [08:08] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc0_client_init_state_MASK 0x00000100
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port1_memc0_client_init_state_SHIFT 8

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: reserved1 [07:07] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved1_MASK         0x00000080
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved1_SHIFT        7

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port0_memc2_client_init_state [06:06] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc2_client_init_state_MASK 0x00000040
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc2_client_init_state_SHIFT 6

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port0_memc1_client_init_state [05:05] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc1_client_init_state_MASK 0x00000020
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc1_client_init_state_SHIFT 5

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: port0_memc0_client_init_state [04:04] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc0_client_init_state_MASK 0x00000010
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_port0_memc0_client_init_state_SHIFT 4

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: reserved2 [03:01] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved2_MASK         0x0000000e
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_reserved2_SHIFT        1

/* SATA_TOP_CTRL :: CLIENT_INIT_CTRL :: initiate_client_init [00:00] */
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_initiate_client_init_MASK 0x00000001
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_initiate_client_init_SHIFT 0
#define BCHP_SATA_TOP_CTRL_CLIENT_INIT_CTRL_initiate_client_init_DEFAULT 0x00000000

#endif /* #ifndef BCHP_SATA_TOP_CTRL_H__ */

/* End of File */