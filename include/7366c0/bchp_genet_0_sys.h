/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on               Thu Feb  5 03:15:09 2015
 *                 Full Compile MD5 Checksum  ca339b82db08da0250a17ca09932699d
 *                     (minus title and desc)
 *                 MD5 Checksum               502556bfbdc2f4341f93db8b4326b3ab
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15517
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_GENET_0_SYS_H__
#define BCHP_GENET_0_SYS_H__

/***************************************************************************
 *GENET_0_SYS
 ***************************************************************************/
#define BCHP_GENET_0_SYS_REV_CNTRL               0x00b60000 /* [RO] Ethernet Revision Control Register */
#define BCHP_GENET_0_SYS_PORT_CNTRL              0x00b60004 /* [RW] Port Control Register */
#define BCHP_GENET_0_SYS_RBUF_FLUSH_CNTRL        0x00b60008 /* [RW] RX_Buffer Flush Control Register */
#define BCHP_GENET_0_SYS_TBUF_FLUSH_CNTRL        0x00b6000c /* [RW] TBUF Flush Control Register */
#define BCHP_GENET_0_SYS_SCB_CLIENT_INIT_STATE   0x00b60010 /* [RO] SCB Client Init State  Register */

/***************************************************************************
 *REV_CNTRL - Ethernet Revision Control Register
 ***************************************************************************/
/* GENET_0_SYS :: REV_CNTRL :: Ethernet_REV [31:00] */
#define BCHP_GENET_0_SYS_REV_CNTRL_Ethernet_REV_MASK               0xffffffff
#define BCHP_GENET_0_SYS_REV_CNTRL_Ethernet_REV_SHIFT              0

/***************************************************************************
 *PORT_CNTRL - Port Control Register
 ***************************************************************************/
/* GENET_0_SYS :: PORT_CNTRL :: reserved0 [31:11] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved0_MASK                 0xfffff800
#define BCHP_GENET_0_SYS_PORT_CNTRL_reserved0_SHIFT                11

/* GENET_0_SYS :: PORT_CNTRL :: led_link_source [10:10] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_link_source_MASK           0x00000400
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_link_source_SHIFT          10
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_link_source_DEFAULT        0x00000000

/* GENET_0_SYS :: PORT_CNTRL :: led_act_source [09:09] */
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_act_source_MASK            0x00000200
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_act_source_SHIFT           9
#define BCHP_GENET_0_SYS_PORT_CNTRL_led_act_source_DEFAULT         0x00000000

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

#endif /* #ifndef BCHP_GENET_0_SYS_H__ */

/* End of File */
