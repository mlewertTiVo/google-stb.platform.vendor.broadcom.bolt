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
 * Date:           Generated on               Thu Dec  1 06:17:11 2016
 *                 Full Compile MD5 Checksum  d77d353b4fd33f3b3eb763c0ec6d13e9
 *                     (minus title and desc)
 *                 MD5 Checksum               b9f56ca31bb855d1e4c37875f6b45457
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

#ifndef BCHP_GENET_0_SYS_H__
#define BCHP_GENET_0_SYS_H__

/***************************************************************************
 *GENET_0_SYS
 ***************************************************************************/
#define BCHP_GENET_0_SYS_REV_CNTRL               0x20480000 /* [RO][32] Ethernet Revision Control Register */
#define BCHP_GENET_0_SYS_PORT_CNTRL              0x20480004 /* [RW][32] Port Control Register */
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL        0x20480008 /* [RW][32] RX_Buffer Flush Control Register */
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL        0x2048000c /* [RW][32] TBUF Flush Control Register */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE   0x20480010 /* [RO][32] SCB Client Init State  Register */
#define BCHP_GENET_0_SYS_LED_CNTRL               0x20480014 /* [RW][32] LED Control Register */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL 0x20480018 /* [RW][32] LED Link And Speed Encoding Selection Register */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING 0x2048001c /* [RW][32] LED Link And Speed Encoding Register */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL        0x20480020 /* [RW][32] LED Serial Control Register */
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL    0x20480024 /* [RW][32] Refresh Period Control Register */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL     0x20480028 /* [RW][32] Aggregate LED Control Register */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL 0x2048002c /* [RW][32] Aggregate LED Blink Rate Control Register */

/***************************************************************************
 *REV_CNTRL - Ethernet Revision Control Register
 ***************************************************************************/
/* GENET_0_SYS :: REV_CNTRL :: Ethernet_REV [31:00] */
#define BCHP_GENET_0_SYS_REV_CNTRL_Ethernet_REV_MASK               0xffffffff
#define BCHP_GENET_0_SYS_REV_CNTRL_Ethernet_REV_SHIFT              0

/***************************************************************************
 *PORT_CNTRL - Port Control Register
 ***************************************************************************/
/* GENET_0_SYS :: PORT_CNTRL :: reserved0 [31:09] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved0_MASK                 0xfffffe00
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved0_SHIFT                9

/* GENET_0_SYS :: PORT_CNTRL :: intr_polarity [08:08] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_intr_polarity_MASK             0x00000100
#define BCHP_GENET_0_SYS_PORT_CNTRL_intr_polarity_SHIFT            8
#define BCHP_GENET_0_SYS_PORT_CNTRL_intr_polarity_DEFAULT          0x00000000

/* GENET_0_SYS :: PORT_CNTRL :: reserved1 [07:05] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved1_MASK                 0x000000e0
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved1_SHIFT                5

/* GENET_0_SYS :: PORT_CNTRL :: rvmii_ref_sel [04:04] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_rvmii_ref_sel_MASK             0x00000010
#define BCHP_GENET_0_SYS_PORT_CNTRL_rvmii_ref_sel_SHIFT            4
#define BCHP_GENET_0_SYS_PORT_CNTRL_rvmii_ref_sel_DEFAULT          0x00000000

/* GENET_0_SYS :: PORT_CNTRL :: reserved2 [03:03] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved2_MASK                 0x00000008
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved2_SHIFT                3

/* GENET_0_SYS :: PORT_CNTRL :: port_mode [02:00] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_port_mode_MASK                 0x00000007
#define BCHP_GENET_0_SYS_PORT_CNTRL_port_mode_SHIFT                0
#define BCHP_GENET_0_SYS_PORT_CNTRL_port_mode_DEFAULT              0x00000000

/***************************************************************************
 *RBUF_FLUSH_CNTRL - RX_Buffer Flush Control Register
 ***************************************************************************/
/* GENET_0_SYS :: RBUF_FLUSH_CNTRL :: reserved0 [31:02] */
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_reserved0_MASK           0xfffffffc
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_reserved0_SHIFT          2

/* GENET_0_SYS :: RBUF_FLUSH_CNTRL :: umac_sw_rst [01:01] */
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_umac_sw_rst_MASK         0x00000002
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_umac_sw_rst_SHIFT        1
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_umac_sw_rst_DEFAULT      0x00000001

/* GENET_0_SYS :: RBUF_FLUSH_CNTRL :: rx_flush [00:00] */
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_rx_flush_MASK            0x00000001
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_rx_flush_SHIFT           0
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL_rx_flush_DEFAULT         0x00000000

/***************************************************************************
 *TBUF_FLUSH_CNTRL - TBUF Flush Control Register
 ***************************************************************************/
/* GENET_0_SYS :: TBUF_FLUSH_CNTRL :: reserved0 [31:01] */
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL_reserved0_MASK           0xfffffffe
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL_reserved0_SHIFT          1

/* GENET_0_SYS :: TBUF_FLUSH_CNTRL :: tx_flush [00:00] */
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL_tx_flush_MASK            0x00000001
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL_tx_flush_SHIFT           0
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL_tx_flush_DEFAULT         0x00000000

/***************************************************************************
 *SCB_CLIENT_INIT_STATE - SCB Client Init State  Register
 ***************************************************************************/
/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: reserved0 [31:06] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_reserved0_MASK      0xffffffc0
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_reserved0_SHIFT     6

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb2_dma_wr_init_state [05:05] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb2_dma_wr_init_state_MASK 0x00000020
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb2_dma_wr_init_state_SHIFT 5

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb2_dma_rd_init_state [04:04] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb2_dma_rd_init_state_MASK 0x00000010
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb2_dma_rd_init_state_SHIFT 4

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb1_dma_wr_init_state [03:03] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb1_dma_wr_init_state_MASK 0x00000008
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb1_dma_wr_init_state_SHIFT 3

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb1_dma_rd_init_state [02:02] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb1_dma_rd_init_state_MASK 0x00000004
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb1_dma_rd_init_state_SHIFT 2

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb0_dma_wr_init_state [01:01] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb0_dma_wr_init_state_MASK 0x00000002
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb0_dma_wr_init_state_SHIFT 1

/* GENET_0_SYS :: SCB_CLIENT_INIT_STATE :: scb0_dma_rd_init_state [00:00] */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb0_dma_rd_init_state_MASK 0x00000001
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE_scb0_dma_rd_init_state_SHIFT 0

/***************************************************************************
 *LED_CNTRL - LED Control Register
 ***************************************************************************/
/* GENET_0_SYS :: LED_CNTRL :: reserved0 [31:16] */
#define BCHP_GENET_0_SYS_LED_CNTRL_reserved0_MASK                  0xffff0000
#define BCHP_GENET_0_SYS_LED_CNTRL_reserved0_SHIFT                 16

/* GENET_0_SYS :: LED_CNTRL :: lnk_ovrd_en [15:15] */
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_ovrd_en_MASK                0x00008000
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_ovrd_en_SHIFT               15
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_ovrd_en_DEFAULT             0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spd_ovrd_en [14:14] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spd_ovrd_en_MASK                0x00004000
#define BCHP_GENET_0_SYS_LED_CNTRL_spd_ovrd_en_SHIFT               14
#define BCHP_GENET_0_SYS_LED_CNTRL_spd_ovrd_en_DEFAULT             0x00000000

/* GENET_0_SYS :: LED_CNTRL :: lnk_status_ovrd [13:13] */
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_status_ovrd_MASK            0x00002000
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_status_ovrd_SHIFT           13
#define BCHP_GENET_0_SYS_LED_CNTRL_lnk_status_ovrd_DEFAULT         0x00000000

/* GENET_0_SYS :: LED_CNTRL :: led_spd_ovrd [12:10] */
#define BCHP_GENET_0_SYS_LED_CNTRL_led_spd_ovrd_MASK               0x00001c00
#define BCHP_GENET_0_SYS_LED_CNTRL_led_spd_ovrd_SHIFT              10
#define BCHP_GENET_0_SYS_LED_CNTRL_led_spd_ovrd_DEFAULT            0x00000000

/* GENET_0_SYS :: LED_CNTRL :: act_led_pol_sel [09:09] */
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_pol_sel_MASK            0x00000200
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_pol_sel_SHIFT           9
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_pol_sel_DEFAULT         0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led2_act_pol_sel [08:08] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_pol_sel_MASK    0x00000100
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_pol_sel_SHIFT   8
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_pol_sel_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led1_act_pol_sel [07:07] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_pol_sel_MASK    0x00000080
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_pol_sel_SHIFT   7
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_pol_sel_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led0_act_pol_sel [06:06] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_pol_sel_MASK    0x00000040
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_pol_sel_SHIFT   6
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_pol_sel_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_CNTRL :: act_led_act_sel [05:05] */
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_act_sel_MASK            0x00000020
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_act_sel_SHIFT           5
#define BCHP_GENET_0_SYS_LED_CNTRL_act_led_act_sel_DEFAULT         0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led2_act_sel [04:04] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_sel_MASK        0x00000010
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_sel_SHIFT       4
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led2_act_sel_DEFAULT     0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led1_act_sel [03:03] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_sel_MASK        0x00000008
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_sel_SHIFT       3
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led1_act_sel_DEFAULT     0x00000000

/* GENET_0_SYS :: LED_CNTRL :: spdlnk_led0_act_sel [02:02] */
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_sel_MASK        0x00000004
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_sel_SHIFT       2
#define BCHP_GENET_0_SYS_LED_CNTRL_spdlnk_led0_act_sel_DEFAULT     0x00000000

/* GENET_0_SYS :: LED_CNTRL :: tx_act_en [01:01] */
#define BCHP_GENET_0_SYS_LED_CNTRL_tx_act_en_MASK                  0x00000002
#define BCHP_GENET_0_SYS_LED_CNTRL_tx_act_en_SHIFT                 1
#define BCHP_GENET_0_SYS_LED_CNTRL_tx_act_en_DEFAULT               0x00000001

/* GENET_0_SYS :: LED_CNTRL :: rx_act_en [00:00] */
#define BCHP_GENET_0_SYS_LED_CNTRL_rx_act_en_MASK                  0x00000001
#define BCHP_GENET_0_SYS_LED_CNTRL_rx_act_en_SHIFT                 0
#define BCHP_GENET_0_SYS_LED_CNTRL_rx_act_en_DEFAULT               0x00000001

/***************************************************************************
 *LED_LINK_AND_SPEED_ENCODING_SEL - LED Link And Speed Encoding Selection Register
 ***************************************************************************/
/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: reserved0 [31:24] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_reserved0_MASK 0xff000000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_reserved0_SHIFT 24

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: rsvd_sel_spd_encode_2 [23:21] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_2_MASK 0x00e00000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_2_SHIFT 21
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_2_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: rsvd_sel_spd_encode_1 [20:18] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_1_MASK 0x001c0000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_1_SHIFT 18
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_rsvd_sel_spd_encode_1_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_10G_encode [17:15] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10G_encode_MASK 0x00038000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10G_encode_SHIFT 15
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10G_encode_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_2500m_encode [14:12] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_2500m_encode_MASK 0x00007000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_2500m_encode_SHIFT 12
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_2500m_encode_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_1000m_encode [11:09] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_1000m_encode_MASK 0x00000e00
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_1000m_encode_SHIFT 9
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_1000m_encode_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_100m_encode [08:06] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_100m_encode_MASK 0x000001c0
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_100m_encode_SHIFT 6
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_100m_encode_DEFAULT 0x00000001

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_10m_encode [05:03] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10m_encode_MASK 0x00000038
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10m_encode_SHIFT 3
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_10m_encode_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING_SEL :: sel_no_link_encode [02:00] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_no_link_encode_MASK 0x00000007
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_no_link_encode_SHIFT 0
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_SEL_sel_no_link_encode_DEFAULT 0x00000000

/***************************************************************************
 *LED_LINK_AND_SPEED_ENCODING - LED Link And Speed Encoding Register
 ***************************************************************************/
/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: reserved0 [31:24] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_reserved0_MASK 0xff000000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_reserved0_SHIFT 24

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: rsvd_spd_encode_2 [23:21] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_2_MASK 0x00e00000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_2_SHIFT 21
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_2_DEFAULT 0x00000007

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: rsvd_spd_encode_1 [20:18] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_1_MASK 0x001c0000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_1_SHIFT 18
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_rsvd_spd_encode_1_DEFAULT 0x00000007

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: m10g_encode [17:15] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10g_encode_MASK 0x00038000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10g_encode_SHIFT 15
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10g_encode_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: m2500_encode [14:12] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m2500_encode_MASK 0x00007000
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m2500_encode_SHIFT 12
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m2500_encode_DEFAULT 0x00000004

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: m1000_encode [11:09] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m1000_encode_MASK 0x00000e00
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m1000_encode_SHIFT 9
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m1000_encode_DEFAULT 0x00000003

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: m100_encode [08:06] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m100_encode_MASK 0x000001c0
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m100_encode_SHIFT 6
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m100_encode_DEFAULT 0x00000005

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: m10_encode [05:03] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10_encode_MASK 0x00000038
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10_encode_SHIFT 3
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_m10_encode_DEFAULT 0x00000006

/* GENET_0_SYS :: LED_LINK_AND_SPEED_ENCODING :: no_link_encode [02:00] */
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_no_link_encode_MASK 0x00000007
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_no_link_encode_SHIFT 0
#define BCHP_GENET_0_SYS_LED_LINK_AND_SPEED_ENCODING_no_link_encode_DEFAULT 0x00000007

/***************************************************************************
 *LED_SERIAL_CNTRL - LED Serial Control Register
 ***************************************************************************/
/* GENET_0_SYS :: LED_SERIAL_CNTRL :: reserved0 [31:25] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_reserved0_MASK           0xfe000000
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_reserved0_SHIFT          25

/* GENET_0_SYS :: LED_SERIAL_CNTRL :: smode [24:23] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_smode_MASK               0x01800000
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_smode_SHIFT              23
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_smode_DEFAULT            0x00000000

/* GENET_0_SYS :: LED_SERIAL_CNTRL :: sled_clk_frequency [22:22] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_frequency_MASK  0x00400000
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_frequency_SHIFT 22
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_frequency_DEFAULT 0x00000000

/* GENET_0_SYS :: LED_SERIAL_CNTRL :: sled_clk_pol [21:21] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_pol_MASK        0x00200000
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_pol_SHIFT       21
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_sled_clk_pol_DEFAULT     0x00000000

/* GENET_0_SYS :: LED_SERIAL_CNTRL :: refresh_period [20:16] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_refresh_period_MASK      0x001f0000
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_refresh_period_SHIFT     16
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_refresh_period_DEFAULT   0x00000004

/* GENET_0_SYS :: LED_SERIAL_CNTRL :: port_en [15:00] */
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_port_en_MASK             0x0000ffff
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_port_en_SHIFT            0
#define BCHP_GENET_0_SYS_LED_SERIAL_CNTRL_port_en_DEFAULT          0x00000000

/***************************************************************************
 *REFRESH_PERIOD_CNTRL - Refresh Period Control Register
 ***************************************************************************/
/* GENET_0_SYS :: REFRESH_PERIOD_CNTRL :: reserved0 [31:24] */
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL_reserved0_MASK       0xff000000
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL_reserved0_SHIFT      24

/* GENET_0_SYS :: REFRESH_PERIOD_CNTRL :: refresh_period_cnt [23:00] */
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL_refresh_period_cnt_MASK 0x00ffffff
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL_refresh_period_cnt_SHIFT 0
#define BCHP_GENET_0_SYS_REFRESH_PERIOD_CNTRL_refresh_period_cnt_DEFAULT 0x0001e847

/***************************************************************************
 *AGGREGATE_LED_CNTRL - Aggregate LED Control Register
 ***************************************************************************/
/* GENET_0_SYS :: AGGREGATE_LED_CNTRL :: reserved0 [31:19] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_reserved0_MASK        0xfff80000
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_reserved0_SHIFT       19

/* GENET_0_SYS :: AGGREGATE_LED_CNTRL :: lnk_pol_sel [18:18] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_lnk_pol_sel_MASK      0x00040000
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_lnk_pol_sel_SHIFT     18
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_lnk_pol_sel_DEFAULT   0x00000000

/* GENET_0_SYS :: AGGREGATE_LED_CNTRL :: act_pol_sel [17:17] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_pol_sel_MASK      0x00020000
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_pol_sel_SHIFT     17
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_pol_sel_DEFAULT   0x00000000

/* GENET_0_SYS :: AGGREGATE_LED_CNTRL :: act_sel [16:16] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_sel_MASK          0x00010000
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_sel_SHIFT         16
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_act_sel_DEFAULT       0x00000000

/* GENET_0_SYS :: AGGREGATE_LED_CNTRL :: port_en [15:00] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_port_en_MASK          0x0000ffff
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_port_en_SHIFT         0
#define BCHP_GENET_0_SYS_AGGREGATE_LED_CNTRL_port_en_DEFAULT       0x00000000

/***************************************************************************
 *AGGREGATE_LED_BLINK_RATE_CNTRL - Aggregate LED Blink Rate Control Register
 ***************************************************************************/
/* GENET_0_SYS :: AGGREGATE_LED_BLINK_RATE_CNTRL :: led_on_time [31:16] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_on_time_MASK 0xffff0000
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_on_time_SHIFT 16
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_on_time_DEFAULT 0x00000320

/* GENET_0_SYS :: AGGREGATE_LED_BLINK_RATE_CNTRL :: led_off_time [15:00] */
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_off_time_MASK 0x0000ffff
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_off_time_SHIFT 0
#define BCHP_GENET_0_SYS_AGGREGATE_LED_BLINK_RATE_CNTRL_led_off_time_DEFAULT 0x00000320

#endif /* #ifndef BCHP_GENET_0_SYS_H__ */

/* End of File */