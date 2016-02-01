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
 * Date:           Generated on              Mon Apr 28 03:11:13 2014
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

#ifndef BCHP_CNTCONTROLBASE_H__
#define BCHP_CNTCONTROLBASE_H__

/***************************************************************************
 *CNTControlBase - CPU System Counter Control Base
 ***************************************************************************/
#define BCHP_CNTControlBase_CNTCR                0x00412000 /* CPU System Counter Control Register */
#define BCHP_CNTControlBase_CNTSR                0x00412004 /* CPU System Counter Status Register */
#define BCHP_CNTControlBase_CNTCV_LO             0x00412008 /* CPU System Counter Count Value Lower Register (Bits 31 to 0) */
#define BCHP_CNTControlBase_CNTCV_HI             0x0041200c /* CPU System Counter Count Value Upper Register (Bits 63 to 32) */
#define BCHP_CNTControlBase_CNTFID0              0x00412020 /* CPU System Counter Frequency ID Register 0 */
#define BCHP_CNTControlBase_CNTFID1              0x00412024 /* CPU System Counter Frequency ID Register 1 */
#define BCHP_CNTControlBase_CounterID0           0x00412fd0 /* CPU System Counter ID 0 Register */
#define BCHP_CNTControlBase_CounterID1           0x00412fd4 /* CPU System Counter ID 1 Register */
#define BCHP_CNTControlBase_CounterID2           0x00412fd8 /* CPU System Counter ID 2 Register */
#define BCHP_CNTControlBase_CounterID3           0x00412fdc /* CPU System Counter ID 3 Register */
#define BCHP_CNTControlBase_CounterID4           0x00412fe0 /* CPU System Counter ID 4 Register */
#define BCHP_CNTControlBase_CounterID5           0x00412fe4 /* CPU System Counter ID 5 Register */
#define BCHP_CNTControlBase_CounterID6           0x00412fe8 /* CPU System Counter ID 6 Register */
#define BCHP_CNTControlBase_CounterID7           0x00412fec /* CPU System Counter ID 7 Register */
#define BCHP_CNTControlBase_CounterID8           0x00412ff0 /* CPU System Counter ID 8 Register */
#define BCHP_CNTControlBase_CounterID9           0x00412ff4 /* CPU System Counter ID 9 Register */
#define BCHP_CNTControlBase_CounterID10          0x00412ff8 /* CPU System Counter ID 10 Register */
#define BCHP_CNTControlBase_CounterID11          0x00412ffc /* CPU System Counter ID 11 Register */

/***************************************************************************
 *CNTCR - CPU System Counter Control Register
 ***************************************************************************/
/* CNTControlBase :: CNTCR :: reserved0 [31:09] */
#define BCHP_CNTControlBase_CNTCR_reserved0_MASK                   0xfffffe00
#define BCHP_CNTControlBase_CNTCR_reserved0_SHIFT                  9

/* CNTControlBase :: CNTCR :: FCREQ [08:08] */
#define BCHP_CNTControlBase_CNTCR_FCREQ_MASK                       0x00000100
#define BCHP_CNTControlBase_CNTCR_FCREQ_SHIFT                      8
#define BCHP_CNTControlBase_CNTCR_FCREQ_DEFAULT                    0x00000000
#define BCHP_CNTControlBase_CNTCR_FCREQ_RESET_FREQ_ONE_HOT         0
#define BCHP_CNTControlBase_CNTCR_FCREQ_BASE_FREQ_ONE_HOT          1

/* CNTControlBase :: CNTCR :: reserved1 [07:02] */
#define BCHP_CNTControlBase_CNTCR_reserved1_MASK                   0x000000fc
#define BCHP_CNTControlBase_CNTCR_reserved1_SHIFT                  2

/* CNTControlBase :: CNTCR :: HDBG [01:01] */
#define BCHP_CNTControlBase_CNTCR_HDBG_MASK                        0x00000002
#define BCHP_CNTControlBase_CNTCR_HDBG_SHIFT                       1
#define BCHP_CNTControlBase_CNTCR_HDBG_DEFAULT                     0x00000000

/* CNTControlBase :: CNTCR :: EN [00:00] */
#define BCHP_CNTControlBase_CNTCR_EN_MASK                          0x00000001
#define BCHP_CNTControlBase_CNTCR_EN_SHIFT                         0
#define BCHP_CNTControlBase_CNTCR_EN_DEFAULT                       0x00000000

/***************************************************************************
 *CNTSR - CPU System Counter Status Register
 ***************************************************************************/
/* CNTControlBase :: CNTSR :: reserved0 [31:09] */
#define BCHP_CNTControlBase_CNTSR_reserved0_MASK                   0xfffffe00
#define BCHP_CNTControlBase_CNTSR_reserved0_SHIFT                  9

/* CNTControlBase :: CNTSR :: FCACK [08:08] */
#define BCHP_CNTControlBase_CNTSR_FCACK_MASK                       0x00000100
#define BCHP_CNTControlBase_CNTSR_FCACK_SHIFT                      8
#define BCHP_CNTControlBase_CNTSR_FCACK_DEFAULT                    0x00000000
#define BCHP_CNTControlBase_CNTSR_FCACK_RESET_FREQ_ONE_HOT         0
#define BCHP_CNTControlBase_CNTSR_FCACK_BASE_FREQ_ONE_HOT          1

/* CNTControlBase :: CNTSR :: reserved1 [07:02] */
#define BCHP_CNTControlBase_CNTSR_reserved1_MASK                   0x000000fc
#define BCHP_CNTControlBase_CNTSR_reserved1_SHIFT                  2

/* CNTControlBase :: CNTSR :: DBGH [01:01] */
#define BCHP_CNTControlBase_CNTSR_DBGH_MASK                        0x00000002
#define BCHP_CNTControlBase_CNTSR_DBGH_SHIFT                       1

/* CNTControlBase :: CNTSR :: reserved2 [00:00] */
#define BCHP_CNTControlBase_CNTSR_reserved2_MASK                   0x00000001
#define BCHP_CNTControlBase_CNTSR_reserved2_SHIFT                  0

/***************************************************************************
 *CNTCV_LO - CPU System Counter Count Value Lower Register (Bits 31 to 0)
 ***************************************************************************/
/* CNTControlBase :: CNTCV_LO :: CountValue [31:00] */
#define BCHP_CNTControlBase_CNTCV_LO_CountValue_MASK               0xffffffff
#define BCHP_CNTControlBase_CNTCV_LO_CountValue_SHIFT              0
#define BCHP_CNTControlBase_CNTCV_LO_CountValue_DEFAULT            0x00000000

/***************************************************************************
 *CNTCV_HI - CPU System Counter Count Value Upper Register (Bits 63 to 32)
 ***************************************************************************/
/* CNTControlBase :: CNTCV_HI :: CountValue [31:00] */
#define BCHP_CNTControlBase_CNTCV_HI_CountValue_MASK               0xffffffff
#define BCHP_CNTControlBase_CNTCV_HI_CountValue_SHIFT              0
#define BCHP_CNTControlBase_CNTCV_HI_CountValue_DEFAULT            0x00000000

/***************************************************************************
 *CNTFID0 - CPU System Counter Frequency ID Register 0
 ***************************************************************************/
/* CNTControlBase :: CNTFID0 :: CNTFID [31:00] */
#define BCHP_CNTControlBase_CNTFID0_CNTFID_MASK                    0xffffffff
#define BCHP_CNTControlBase_CNTFID0_CNTFID_SHIFT                   0
#define BCHP_CNTControlBase_CNTFID0_CNTFID_DEFAULT                 0x019bfcc0

/***************************************************************************
 *CNTFID1 - CPU System Counter Frequency ID Register 1
 ***************************************************************************/
/* CNTControlBase :: CNTFID1 :: CNTFID [31:00] */
#define BCHP_CNTControlBase_CNTFID1_CNTFID_MASK                    0xffffffff
#define BCHP_CNTControlBase_CNTFID1_CNTFID_SHIFT                   0
#define BCHP_CNTControlBase_CNTFID1_CNTFID_DEFAULT                 0x00000000

/***************************************************************************
 *CounterID0 - CPU System Counter ID 0 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID0 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID0_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID0_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID0_COUNTERID_DEFAULT           0x00000001

/***************************************************************************
 *CounterID1 - CPU System Counter ID 1 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID1 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID1_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID1_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID1_COUNTERID_DEFAULT           0x00000000

/***************************************************************************
 *CounterID2 - CPU System Counter ID 2 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID2 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID2_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID2_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID2_COUNTERID_DEFAULT           0x00000000

/***************************************************************************
 *CounterID3 - CPU System Counter ID 3 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID3 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID3_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID3_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID3_COUNTERID_DEFAULT           0x00000000

/***************************************************************************
 *CounterID4 - CPU System Counter ID 4 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID4 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID4_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID4_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID4_COUNTERID_DEFAULT           0x00000045

/***************************************************************************
 *CounterID5 - CPU System Counter ID 5 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID5 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID5_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID5_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID5_COUNTERID_DEFAULT           0x000000f4

/***************************************************************************
 *CounterID6 - CPU System Counter ID 6 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID6 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID6_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID6_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID6_COUNTERID_DEFAULT           0x0000000d

/***************************************************************************
 *CounterID7 - CPU System Counter ID 7 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID7 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID7_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID7_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID7_COUNTERID_DEFAULT           0x00000000

/***************************************************************************
 *CounterID8 - CPU System Counter ID 8 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID8 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID8_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID8_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID8_COUNTERID_DEFAULT           0x0000000d

/***************************************************************************
 *CounterID9 - CPU System Counter ID 9 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID9 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID9_COUNTERID_MASK              0xffffffff
#define BCHP_CNTControlBase_CounterID9_COUNTERID_SHIFT             0
#define BCHP_CNTControlBase_CounterID9_COUNTERID_DEFAULT           0x000000f0

/***************************************************************************
 *CounterID10 - CPU System Counter ID 10 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID10 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID10_COUNTERID_MASK             0xffffffff
#define BCHP_CNTControlBase_CounterID10_COUNTERID_SHIFT            0
#define BCHP_CNTControlBase_CounterID10_COUNTERID_DEFAULT          0x00000005

/***************************************************************************
 *CounterID11 - CPU System Counter ID 11 Register
 ***************************************************************************/
/* CNTControlBase :: CounterID11 :: COUNTERID [31:00] */
#define BCHP_CNTControlBase_CounterID11_COUNTERID_MASK             0xffffffff
#define BCHP_CNTControlBase_CounterID11_COUNTERID_SHIFT            0
#define BCHP_CNTControlBase_CounterID11_COUNTERID_DEFAULT          0x000000b1

#endif /* #ifndef BCHP_CNTCONTROLBASE_H__ */

/* End of File */
