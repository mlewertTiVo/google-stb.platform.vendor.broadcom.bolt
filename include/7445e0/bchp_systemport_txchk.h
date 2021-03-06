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
 * Date:           Generated on              Tue Dec  9 03:24:11 2014
 *                 Full Compile MD5 Checksum 64f140304e8246661fd9087cde57d639
 *                   (minus title and desc)  
 *                 MD5 Checksum              e8d48cd8753ccd56859cb6aa8018567f
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

#ifndef BCHP_SYSTEMPORT_TXCHK_H__
#define BCHP_SYSTEMPORT_TXCHK_H__

/***************************************************************************
 *SYSTEMPORT_TXCHK
 ***************************************************************************/
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD 0x004a0380 /* TXCHK Packet Ready Threshold Register */

/***************************************************************************
 *TXCHK_PKT_RDY_THRESHOLD - TXCHK Packet Ready Threshold Register
 ***************************************************************************/
/* SYSTEMPORT_TXCHK :: TXCHK_PKT_RDY_THRESHOLD :: reserved0 [31:08] */
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD_reserved0_MASK 0xffffff00
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD_reserved0_SHIFT 8

/* SYSTEMPORT_TXCHK :: TXCHK_PKT_RDY_THRESHOLD :: pkt_rdy_threshold [07:00] */
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_MASK 0x000000ff
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_SHIFT 0
#define BCHP_SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD_pkt_rdy_threshold_DEFAULT 0x00000080

#endif /* #ifndef BCHP_SYSTEMPORT_TXCHK_H__ */

/* End of File */
