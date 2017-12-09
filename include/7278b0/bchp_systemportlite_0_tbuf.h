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
 * Date:           Generated on               Tue Oct 10 01:11:39 2017
 *                 Full Compile MD5 Checksum  ea353cbbaf7534b1acfabe87187756c5
 *                     (minus title and desc)
 *                 MD5 Checksum               bd09f6ea4577b9d8ea88a425effe4ef4
 *
 * lock_release:   n/a
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

#ifndef BCHP_SYSTEMPORTLITE_0_TBUF_H__
#define BCHP_SYSTEMPORTLITE_0_TBUF_H__

/***************************************************************************
 *SYSTEMPORTLITE_0_TBUF
 ***************************************************************************/
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL  0x0001300600 /* [RW][32] TBUF Control Register */

/***************************************************************************
 *TBUF_CONTROL - TBUF Control Register
 ***************************************************************************/
/* SYSTEMPORTLITE_0_TBUF :: TBUF_CONTROL :: reserved0 [31:16] */
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_reserved0_MASK     0xffff0000
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_reserved0_SHIFT    16

/* SYSTEMPORTLITE_0_TBUF :: TBUF_CONTROL :: full_threshold [15:08] */
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_full_threshold_MASK 0x0000ff00
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_full_threshold_SHIFT 8
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_full_threshold_DEFAULT 0x00000000

/* SYSTEMPORTLITE_0_TBUF :: TBUF_CONTROL :: reserved1 [07:07] */
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_reserved1_MASK     0x00000080
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_reserved1_SHIFT    7

/* SYSTEMPORTLITE_0_TBUF :: TBUF_CONTROL :: max_pkt_thresold [06:01] */
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_max_pkt_thresold_MASK 0x0000007e
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_max_pkt_thresold_SHIFT 1
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_max_pkt_thresold_DEFAULT 0x00000003

/* SYSTEMPORTLITE_0_TBUF :: TBUF_CONTROL :: backpressure_en [00:00] */
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_backpressure_en_MASK 0x00000001
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_backpressure_en_SHIFT 0
#define BCHP_SYSTEMPORTLITE_0_TBUF_TBUF_CONTROL_backpressure_en_DEFAULT 0x00000000

#endif /* #ifndef BCHP_SYSTEMPORTLITE_0_TBUF_H__ */

/* End of File */
