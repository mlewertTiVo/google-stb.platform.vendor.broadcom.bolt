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
 * Date:           Generated on               Sat Nov 19 04:13:56 2016
 *                 Full Compile MD5 Checksum  f8dd6cca50f91ddd1a5c7a281fbd865c
 *                     (minus title and desc)
 *                 MD5 Checksum               c2181d725169d4b962f1233910797e78
 *
 * lock_release:   r_287
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

#ifndef BCHP_HIF_CONTINUATION_H__
#define BCHP_HIF_CONTINUATION_H__

/***************************************************************************
 *HIF_CONTINUATION - HIF Boot Continuation Registers
 ***************************************************************************/
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0  0x20452000 /* [RO][32] Higher 8-bit of HIF's Read-only STB Boot Continuation Address 0 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR0     0x20452004 /* [RO][32] Lower 32-bit of HIF's Read-only STB Boot Continuation Address 0 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1  0x20452008 /* [RW][32] Higher 8-bit of HIF's STB Boot Continuation Address 1 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR1     0x2045200c /* [RW][32] Lower 32-bit of HIF's STB Boot Continuation Address 1 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2  0x20452010 /* [RW][32] Higher 8-bit of HIF's STB Boot Continuation Address 2 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR2     0x20452014 /* [RW][32] Lower 32-bit of HIF's STB Boot Continuation Address 2 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3  0x20452018 /* [RW][32] Higher 8-bit of HIF's STB Boot Continuation Address 3 Register */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR3     0x2045201c /* [RW][32] Lower 32-bit of HIF's STB Boot Continuation Address 3 Register */
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0  0x204520f8 /* [RW][32] Higher 8-bit of HIF's WEB Boot Continuation Address 0 Register */
#define BCHP_HIF_CONTINUATION_WEB_BOOT_ADDR0     0x204520fc /* [RW][32] Lower 32-bit of HIF's WEB Boot Continuation Address 0 Register */

/***************************************************************************
 *STB_BOOT_HI_ADDR0 - Higher 8-bit of HIF's Read-only STB Boot Continuation Address 0 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR0 :: reserved0 [31:08] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0_reserved0_MASK     0xffffff00
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0_reserved0_SHIFT    8

/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR0 :: ADDRESS [07:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0_ADDRESS_MASK       0x000000ff
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0_ADDRESS_SHIFT      0
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0_ADDRESS_DEFAULT    0x00000000

/***************************************************************************
 *STB_BOOT_ADDR0 - Lower 32-bit of HIF's Read-only STB Boot Continuation Address 0 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_ADDR0 :: ADDRESS [31:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR0_ADDRESS_MASK          0xffffffff
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR0_ADDRESS_SHIFT         0
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR0_ADDRESS_DEFAULT       0xe0000000

/***************************************************************************
 *STB_BOOT_HI_ADDR1 - Higher 8-bit of HIF's STB Boot Continuation Address 1 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR1 :: reserved0 [31:08] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1_reserved0_MASK     0xffffff00
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1_reserved0_SHIFT    8

/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR1 :: ADDRESS [07:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1_ADDRESS_MASK       0x000000ff
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1_ADDRESS_SHIFT      0
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR1_ADDRESS_DEFAULT    0x00000000

/***************************************************************************
 *STB_BOOT_ADDR1 - Lower 32-bit of HIF's STB Boot Continuation Address 1 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_ADDR1 :: ADDRESS [31:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR1_ADDRESS_MASK          0xffffffff
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR1_ADDRESS_SHIFT         0
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR1_ADDRESS_DEFAULT       0xffff0000

/***************************************************************************
 *STB_BOOT_HI_ADDR2 - Higher 8-bit of HIF's STB Boot Continuation Address 2 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR2 :: reserved0 [31:08] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2_reserved0_MASK     0xffffff00
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2_reserved0_SHIFT    8

/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR2 :: ADDRESS [07:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2_ADDRESS_MASK       0x000000ff
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2_ADDRESS_SHIFT      0
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR2_ADDRESS_DEFAULT    0x00000000

/***************************************************************************
 *STB_BOOT_ADDR2 - Lower 32-bit of HIF's STB Boot Continuation Address 2 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_ADDR2 :: ADDRESS [31:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR2_ADDRESS_MASK          0xffffffff
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR2_ADDRESS_SHIFT         0
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR2_ADDRESS_DEFAULT       0xffff0000

/***************************************************************************
 *STB_BOOT_HI_ADDR3 - Higher 8-bit of HIF's STB Boot Continuation Address 3 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR3 :: reserved0 [31:08] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3_reserved0_MASK     0xffffff00
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3_reserved0_SHIFT    8

/* HIF_CONTINUATION :: STB_BOOT_HI_ADDR3 :: ADDRESS [07:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3_ADDRESS_MASK       0x000000ff
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3_ADDRESS_SHIFT      0
#define BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR3_ADDRESS_DEFAULT    0x00000000

/***************************************************************************
 *STB_BOOT_ADDR3 - Lower 32-bit of HIF's STB Boot Continuation Address 3 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: STB_BOOT_ADDR3 :: ADDRESS [31:00] */
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR3_ADDRESS_MASK          0xffffffff
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR3_ADDRESS_SHIFT         0
#define BCHP_HIF_CONTINUATION_STB_BOOT_ADDR3_ADDRESS_DEFAULT       0xffff0000

/***************************************************************************
 *WEB_BOOT_HI_ADDR0 - Higher 8-bit of HIF's WEB Boot Continuation Address 0 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: WEB_BOOT_HI_ADDR0 :: reserved0 [31:08] */
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0_reserved0_MASK     0xffffff00
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0_reserved0_SHIFT    8

/* HIF_CONTINUATION :: WEB_BOOT_HI_ADDR0 :: ADDRESS [07:00] */
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0_ADDRESS_MASK       0x000000ff
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0_ADDRESS_SHIFT      0
#define BCHP_HIF_CONTINUATION_WEB_BOOT_HI_ADDR0_ADDRESS_DEFAULT    0x00000000

/***************************************************************************
 *WEB_BOOT_ADDR0 - Lower 32-bit of HIF's WEB Boot Continuation Address 0 Register
 ***************************************************************************/
/* HIF_CONTINUATION :: WEB_BOOT_ADDR0 :: ADDRESS [31:00] */
#define BCHP_HIF_CONTINUATION_WEB_BOOT_ADDR0_ADDRESS_MASK          0xffffffff
#define BCHP_HIF_CONTINUATION_WEB_BOOT_ADDR0_ADDRESS_SHIFT         0
#define BCHP_HIF_CONTINUATION_WEB_BOOT_ADDR0_ADDRESS_DEFAULT       0xffff0000

#endif /* #ifndef BCHP_HIF_CONTINUATION_H__ */

/* End of File */