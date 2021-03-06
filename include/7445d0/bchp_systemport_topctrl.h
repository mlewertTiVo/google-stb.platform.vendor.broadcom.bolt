/***************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Mon Apr 28 03:11:15 2014
 *                 Full Compile MD5 Checksum bfcf7125bf05811fa35815b8286b23a0
 *                   (minus title and desc)  
 *                 MD5 Checksum              6043e9abcf818590a0d0d2cb13ebaf4c
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_SYSTEMPORT_TOPCTRL_H__
#define BCHP_SYSTEMPORT_TOPCTRL_H__

/***************************************************************************
 *SYSTEMPORT_TOPCTRL
 ***************************************************************************/
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL         0x004a0000 /* System Port Revision Control Register */
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL    0x004a0004 /* Rx Flush Control Register */
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL    0x004a0008 /* Tx Flush Control Register */
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL        0x004a000c /* Miscellaneous Control Register */

/***************************************************************************
 *REV_CNTL - System Port Revision Control Register
 ***************************************************************************/
/* SYSTEMPORT_TOPCTRL :: REV_CNTL :: reserved0 [31:16] */
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL_reserved0_MASK            0xffff0000
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL_reserved0_SHIFT           16

/* SYSTEMPORT_TOPCTRL :: REV_CNTL :: SYS_PORT_REV [15:00] */
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL_SYS_PORT_REV_MASK         0x0000ffff
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL_SYS_PORT_REV_SHIFT        0
#define BCHP_SYSTEMPORT_TOPCTRL_REV_CNTL_SYS_PORT_REV_DEFAULT      0x00000100

/***************************************************************************
 *RX_FLUSH_CNTL - Rx Flush Control Register
 ***************************************************************************/
/* SYSTEMPORT_TOPCTRL :: RX_FLUSH_CNTL :: reserved0 [31:01] */
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL_reserved0_MASK       0xfffffffe
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL_reserved0_SHIFT      1

/* SYSTEMPORT_TOPCTRL :: RX_FLUSH_CNTL :: RX_FLUSH [00:00] */
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL_RX_FLUSH_MASK        0x00000001
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL_RX_FLUSH_SHIFT       0
#define BCHP_SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL_RX_FLUSH_DEFAULT     0x00000000

/***************************************************************************
 *TX_FLUSH_CNTL - Tx Flush Control Register
 ***************************************************************************/
/* SYSTEMPORT_TOPCTRL :: TX_FLUSH_CNTL :: reserved0 [31:01] */
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL_reserved0_MASK       0xfffffffe
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL_reserved0_SHIFT      1

/* SYSTEMPORT_TOPCTRL :: TX_FLUSH_CNTL :: TX_FLUSH [00:00] */
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL_TX_FLUSH_MASK        0x00000001
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL_TX_FLUSH_SHIFT       0
#define BCHP_SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL_TX_FLUSH_DEFAULT     0x00000000

/***************************************************************************
 *MISC_CNTL - Miscellaneous Control Register
 ***************************************************************************/
/* SYSTEMPORT_TOPCTRL :: MISC_CNTL :: reserved0 [31:02] */
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_reserved0_MASK           0xfffffffc
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_reserved0_SHIFT          2

/* SYSTEMPORT_TOPCTRL :: MISC_CNTL :: TDMA_EOP_SEL [01:01] */
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_TDMA_EOP_SEL_MASK        0x00000002
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_TDMA_EOP_SEL_SHIFT       1
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_TDMA_EOP_SEL_DEFAULT     0x00000001

/* SYSTEMPORT_TOPCTRL :: MISC_CNTL :: SYS_CLK_SEL [00:00] */
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_SYS_CLK_SEL_MASK         0x00000001
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_SYS_CLK_SEL_SHIFT        0
#define BCHP_SYSTEMPORT_TOPCTRL_MISC_CNTL_SYS_CLK_SEL_DEFAULT      0x00000000

#endif /* #ifndef BCHP_SYSTEMPORT_TOPCTRL_H__ */

/* End of File */
