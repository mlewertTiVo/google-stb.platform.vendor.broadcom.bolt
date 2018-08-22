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
 * Date:           Generated on              Tue Mar 25 03:09:25 2014
 *                 Full Compile MD5 Checksum 9df3b42a7e9d8f74079cd20feb650cd4
 *                   (minus title and desc)  
 *                 MD5 Checksum              b2e9ed931ea5aec568db0bca7f615fea
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

#ifndef BCHP_BSP_CMDBUF_H__
#define BCHP_BSP_CMDBUF_H__

/***************************************************************************
 *BSP_CMDBUF - BSP Command Buffer
 ***************************************************************************/

/***************************************************************************
 *DMEM%i - Data Memory Address 0..511
 ***************************************************************************/
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE                           0x0032c800
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_START                          0
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_END                            511
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_ELEMENT_SIZE                   32

/***************************************************************************
 *DMEM%i - Data Memory Address 0..511
 ***************************************************************************/
/* BSP_CMDBUF :: DMEMi :: DATA [31:00] */
#define BCHP_BSP_CMDBUF_DMEMi_DATA_MASK                            0xffffffff
#define BCHP_BSP_CMDBUF_DMEMi_DATA_SHIFT                           0


#endif /* #ifndef BCHP_BSP_CMDBUF_H__ */

/* End of File */
