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
 * Date:           Generated on               Sun Nov 19 01:07:17 2017
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

#ifndef BCHP_AVS_UART_H__
#define BCHP_AVS_UART_H__

/***************************************************************************
 *AVS_UART - UART Registers
 ***************************************************************************/
#define BCHP_AVS_UART_UARTDR                     0x00204d0000 /* [RW][32] Data register (12 or 8 bits) */
#define BCHP_AVS_UART_UARTRSR                    0x00204d0004 /* [RW][32] Receive status register/error clear register */
#define BCHP_AVS_UART_UARTFR                     0x00204d0018 /* [RO][32] Flag register */
#define BCHP_AVS_UART_UARTILPR                   0x00204d0020 /* [RW][32] IrDA low-power counter register */
#define BCHP_AVS_UART_UARTIBRD                   0x00204d0024 /* [RW][32] Integer baud rate register */
#define BCHP_AVS_UART_UARTFBRD                   0x00204d0028 /* [RW][32] Fractional baud rate register */
#define BCHP_AVS_UART_UARTLCR_H                  0x00204d002c /* [RW][32] Line control register */
#define BCHP_AVS_UART_UARTCR                     0x00204d0030 /* [RW][32] Control register */
#define BCHP_AVS_UART_UARTIFLS                   0x00204d0034 /* [RW][32] Interrupt FIFO level select register */
#define BCHP_AVS_UART_UARTIMSC                   0x00204d0038 /* [RW][32] Interrupt mask set/clear register */
#define BCHP_AVS_UART_UARTRIS                    0x00204d003c /* [RO][32] Raw interrupt status register */
#define BCHP_AVS_UART_UARTMIS                    0x00204d0040 /* [RO][32] Masked interrupt status register */
#define BCHP_AVS_UART_UARTICR                    0x00204d0044 /* [WO][32] Interrupt clear register */
#define BCHP_AVS_UART_UARTDMACR                  0x00204d0048 /* [RW][32] DMA control register */
#define BCHP_AVS_UART_UARTPERIPHID0              0x00204d0fe0 /* [RO][32] UARTPeriphID0 */
#define BCHP_AVS_UART_UARTPERIPHID1              0x00204d0fe4 /* [RO][32] UARTPeriphID1 */
#define BCHP_AVS_UART_UARTPERIPHID2              0x00204d0fe8 /* [RO][32] UARTPeriphID2 */
#define BCHP_AVS_UART_UARTPERIPHID3              0x00204d0fec /* [RO][32] UARTPeriphID3 */
#define BCHP_AVS_UART_UARTPCELLID0               0x00204d0ff0 /* [RO][32] UARTPCellID0 */
#define BCHP_AVS_UART_UARTPCELLID1               0x00204d0ff4 /* [RO][32] UARTPCellID1 */
#define BCHP_AVS_UART_UARTPCELLID2               0x00204d0ff8 /* [RO][32] UARTPCellID2 */
#define BCHP_AVS_UART_UARTPCELLID3               0x00204d0ffc /* [RO][32] UARTPCellID3 */

/***************************************************************************
 *UARTDR - Data register (12 or 8 bits)
 ***************************************************************************/
/* AVS_UART :: UARTDR :: reserved0 [31:12] */
#define BCHP_AVS_UART_UARTDR_reserved0_MASK                        0xfffff000
#define BCHP_AVS_UART_UARTDR_reserved0_SHIFT                       12

/* AVS_UART :: UARTDR :: UARTDR [11:00] */
#define BCHP_AVS_UART_UARTDR_UARTDR_MASK                           0x00000fff
#define BCHP_AVS_UART_UARTDR_UARTDR_SHIFT                          0
#define BCHP_AVS_UART_UARTDR_UARTDR_DEFAULT                        0x00000000

/***************************************************************************
 *UARTRSR - Receive status register/error clear register
 ***************************************************************************/
/* AVS_UART :: UARTRSR :: reserved0 [31:04] */
#define BCHP_AVS_UART_UARTRSR_reserved0_MASK                       0xfffffff0
#define BCHP_AVS_UART_UARTRSR_reserved0_SHIFT                      4

/* AVS_UART :: UARTRSR :: UARTRSR [03:00] */
#define BCHP_AVS_UART_UARTRSR_UARTRSR_MASK                         0x0000000f
#define BCHP_AVS_UART_UARTRSR_UARTRSR_SHIFT                        0
#define BCHP_AVS_UART_UARTRSR_UARTRSR_DEFAULT                      0x00000000

/***************************************************************************
 *UARTFR - Flag register
 ***************************************************************************/
/* AVS_UART :: UARTFR :: reserved0 [31:09] */
#define BCHP_AVS_UART_UARTFR_reserved0_MASK                        0xfffffe00
#define BCHP_AVS_UART_UARTFR_reserved0_SHIFT                       9

/* AVS_UART :: UARTFR :: UARTFR [08:00] */
#define BCHP_AVS_UART_UARTFR_UARTFR_MASK                           0x000001ff
#define BCHP_AVS_UART_UARTFR_UARTFR_SHIFT                          0
#define BCHP_AVS_UART_UARTFR_UARTFR_DEFAULT                        0x00000090

/***************************************************************************
 *UARTILPR - IrDA low-power counter register
 ***************************************************************************/
/* AVS_UART :: UARTILPR :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTILPR_reserved0_MASK                      0xffffff00
#define BCHP_AVS_UART_UARTILPR_reserved0_SHIFT                     8

/* AVS_UART :: UARTILPR :: UARTILPR [07:00] */
#define BCHP_AVS_UART_UARTILPR_UARTILPR_MASK                       0x000000ff
#define BCHP_AVS_UART_UARTILPR_UARTILPR_SHIFT                      0
#define BCHP_AVS_UART_UARTILPR_UARTILPR_DEFAULT                    0x00000000

/***************************************************************************
 *UARTIBRD - Integer baud rate register
 ***************************************************************************/
/* AVS_UART :: UARTIBRD :: reserved0 [31:16] */
#define BCHP_AVS_UART_UARTIBRD_reserved0_MASK                      0xffff0000
#define BCHP_AVS_UART_UARTIBRD_reserved0_SHIFT                     16

/* AVS_UART :: UARTIBRD :: UARTIBRD [15:00] */
#define BCHP_AVS_UART_UARTIBRD_UARTIBRD_MASK                       0x0000ffff
#define BCHP_AVS_UART_UARTIBRD_UARTIBRD_SHIFT                      0
#define BCHP_AVS_UART_UARTIBRD_UARTIBRD_DEFAULT                    0x00000000

/***************************************************************************
 *UARTFBRD - Fractional baud rate register
 ***************************************************************************/
/* AVS_UART :: UARTFBRD :: reserved0 [31:06] */
#define BCHP_AVS_UART_UARTFBRD_reserved0_MASK                      0xffffffc0
#define BCHP_AVS_UART_UARTFBRD_reserved0_SHIFT                     6

/* AVS_UART :: UARTFBRD :: UARTFBRD [05:00] */
#define BCHP_AVS_UART_UARTFBRD_UARTFBRD_MASK                       0x0000003f
#define BCHP_AVS_UART_UARTFBRD_UARTFBRD_SHIFT                      0
#define BCHP_AVS_UART_UARTFBRD_UARTFBRD_DEFAULT                    0x00000000

/***************************************************************************
 *UARTLCR_H - Line control register
 ***************************************************************************/
/* AVS_UART :: UARTLCR_H :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTLCR_H_reserved0_MASK                     0xffffff00
#define BCHP_AVS_UART_UARTLCR_H_reserved0_SHIFT                    8

/* AVS_UART :: UARTLCR_H :: UARTLCR_H [07:00] */
#define BCHP_AVS_UART_UARTLCR_H_UARTLCR_H_MASK                     0x000000ff
#define BCHP_AVS_UART_UARTLCR_H_UARTLCR_H_SHIFT                    0
#define BCHP_AVS_UART_UARTLCR_H_UARTLCR_H_DEFAULT                  0x00000000

/***************************************************************************
 *UARTCR - Control register
 ***************************************************************************/
/* AVS_UART :: UARTCR :: reserved0 [31:16] */
#define BCHP_AVS_UART_UARTCR_reserved0_MASK                        0xffff0000
#define BCHP_AVS_UART_UARTCR_reserved0_SHIFT                       16

/* AVS_UART :: UARTCR :: UARTCR [15:00] */
#define BCHP_AVS_UART_UARTCR_UARTCR_MASK                           0x0000ffff
#define BCHP_AVS_UART_UARTCR_UARTCR_SHIFT                          0
#define BCHP_AVS_UART_UARTCR_UARTCR_DEFAULT                        0x00000300

/***************************************************************************
 *UARTIFLS - Interrupt FIFO level select register
 ***************************************************************************/
/* AVS_UART :: UARTIFLS :: reserved0 [31:06] */
#define BCHP_AVS_UART_UARTIFLS_reserved0_MASK                      0xffffffc0
#define BCHP_AVS_UART_UARTIFLS_reserved0_SHIFT                     6

/* AVS_UART :: UARTIFLS :: UARTIFLS [05:00] */
#define BCHP_AVS_UART_UARTIFLS_UARTIFLS_MASK                       0x0000003f
#define BCHP_AVS_UART_UARTIFLS_UARTIFLS_SHIFT                      0
#define BCHP_AVS_UART_UARTIFLS_UARTIFLS_DEFAULT                    0x00000012

/***************************************************************************
 *UARTIMSC - Interrupt mask set/clear register
 ***************************************************************************/
/* AVS_UART :: UARTIMSC :: reserved0 [31:11] */
#define BCHP_AVS_UART_UARTIMSC_reserved0_MASK                      0xfffff800
#define BCHP_AVS_UART_UARTIMSC_reserved0_SHIFT                     11

/* AVS_UART :: UARTIMSC :: UARTIMSC [10:00] */
#define BCHP_AVS_UART_UARTIMSC_UARTIMSC_MASK                       0x000007ff
#define BCHP_AVS_UART_UARTIMSC_UARTIMSC_SHIFT                      0
#define BCHP_AVS_UART_UARTIMSC_UARTIMSC_DEFAULT                    0x00000000

/***************************************************************************
 *UARTRIS - Raw interrupt status register
 ***************************************************************************/
/* AVS_UART :: UARTRIS :: reserved0 [31:11] */
#define BCHP_AVS_UART_UARTRIS_reserved0_MASK                       0xfffff800
#define BCHP_AVS_UART_UARTRIS_reserved0_SHIFT                      11

/* AVS_UART :: UARTRIS :: UARTRIS [10:00] */
#define BCHP_AVS_UART_UARTRIS_UARTRIS_MASK                         0x000007ff
#define BCHP_AVS_UART_UARTRIS_UARTRIS_SHIFT                        0
#define BCHP_AVS_UART_UARTRIS_UARTRIS_DEFAULT                      0x00000000

/***************************************************************************
 *UARTMIS - Masked interrupt status register
 ***************************************************************************/
/* AVS_UART :: UARTMIS :: reserved0 [31:11] */
#define BCHP_AVS_UART_UARTMIS_reserved0_MASK                       0xfffff800
#define BCHP_AVS_UART_UARTMIS_reserved0_SHIFT                      11

/* AVS_UART :: UARTMIS :: UARTMIS [10:00] */
#define BCHP_AVS_UART_UARTMIS_UARTMIS_MASK                         0x000007ff
#define BCHP_AVS_UART_UARTMIS_UARTMIS_SHIFT                        0
#define BCHP_AVS_UART_UARTMIS_UARTMIS_DEFAULT                      0x00000000

/***************************************************************************
 *UARTICR - Interrupt clear register
 ***************************************************************************/
/* AVS_UART :: UARTICR :: reserved0 [31:11] */
#define BCHP_AVS_UART_UARTICR_reserved0_MASK                       0xfffff800
#define BCHP_AVS_UART_UARTICR_reserved0_SHIFT                      11

/* AVS_UART :: UARTICR :: UARTICR [10:00] */
#define BCHP_AVS_UART_UARTICR_UARTICR_MASK                         0x000007ff
#define BCHP_AVS_UART_UARTICR_UARTICR_SHIFT                        0

/***************************************************************************
 *UARTDMACR - DMA control register
 ***************************************************************************/
/* AVS_UART :: UARTDMACR :: reserved0 [31:03] */
#define BCHP_AVS_UART_UARTDMACR_reserved0_MASK                     0xfffffff8
#define BCHP_AVS_UART_UARTDMACR_reserved0_SHIFT                    3

/* AVS_UART :: UARTDMACR :: UARTDMACR [02:00] */
#define BCHP_AVS_UART_UARTDMACR_UARTDMACR_MASK                     0x00000007
#define BCHP_AVS_UART_UARTDMACR_UARTDMACR_SHIFT                    0
#define BCHP_AVS_UART_UARTDMACR_UARTDMACR_DEFAULT                  0x00000000

/***************************************************************************
 *UARTPERIPHID0 - UARTPeriphID0
 ***************************************************************************/
/* AVS_UART :: UARTPERIPHID0 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPERIPHID0_reserved0_MASK                 0xffffff00
#define BCHP_AVS_UART_UARTPERIPHID0_reserved0_SHIFT                8

/* AVS_UART :: UARTPERIPHID0 :: UARTPERIPHID0 [07:00] */
#define BCHP_AVS_UART_UARTPERIPHID0_UARTPERIPHID0_MASK             0x000000ff
#define BCHP_AVS_UART_UARTPERIPHID0_UARTPERIPHID0_SHIFT            0
#define BCHP_AVS_UART_UARTPERIPHID0_UARTPERIPHID0_DEFAULT          0x00000011

/***************************************************************************
 *UARTPERIPHID1 - UARTPeriphID1
 ***************************************************************************/
/* AVS_UART :: UARTPERIPHID1 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPERIPHID1_reserved0_MASK                 0xffffff00
#define BCHP_AVS_UART_UARTPERIPHID1_reserved0_SHIFT                8

/* AVS_UART :: UARTPERIPHID1 :: UARTPERIPHID1 [07:00] */
#define BCHP_AVS_UART_UARTPERIPHID1_UARTPERIPHID1_MASK             0x000000ff
#define BCHP_AVS_UART_UARTPERIPHID1_UARTPERIPHID1_SHIFT            0
#define BCHP_AVS_UART_UARTPERIPHID1_UARTPERIPHID1_DEFAULT          0x00000010

/***************************************************************************
 *UARTPERIPHID2 - UARTPeriphID2
 ***************************************************************************/
/* AVS_UART :: UARTPERIPHID2 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPERIPHID2_reserved0_MASK                 0xffffff00
#define BCHP_AVS_UART_UARTPERIPHID2_reserved0_SHIFT                8

/* AVS_UART :: UARTPERIPHID2 :: UARTPERIPHID2 [07:00] */
#define BCHP_AVS_UART_UARTPERIPHID2_UARTPERIPHID2_MASK             0x000000ff
#define BCHP_AVS_UART_UARTPERIPHID2_UARTPERIPHID2_SHIFT            0
#define BCHP_AVS_UART_UARTPERIPHID2_UARTPERIPHID2_DEFAULT          0x00000014

/***************************************************************************
 *UARTPERIPHID3 - UARTPeriphID3
 ***************************************************************************/
/* AVS_UART :: UARTPERIPHID3 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPERIPHID3_reserved0_MASK                 0xffffff00
#define BCHP_AVS_UART_UARTPERIPHID3_reserved0_SHIFT                8

/* AVS_UART :: UARTPERIPHID3 :: UARTPERIPHID3 [07:00] */
#define BCHP_AVS_UART_UARTPERIPHID3_UARTPERIPHID3_MASK             0x000000ff
#define BCHP_AVS_UART_UARTPERIPHID3_UARTPERIPHID3_SHIFT            0
#define BCHP_AVS_UART_UARTPERIPHID3_UARTPERIPHID3_DEFAULT          0x00000000

/***************************************************************************
 *UARTPCELLID0 - UARTPCellID0
 ***************************************************************************/
/* AVS_UART :: UARTPCELLID0 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPCELLID0_reserved0_MASK                  0xffffff00
#define BCHP_AVS_UART_UARTPCELLID0_reserved0_SHIFT                 8

/* AVS_UART :: UARTPCELLID0 :: UARTPCELLID0 [07:00] */
#define BCHP_AVS_UART_UARTPCELLID0_UARTPCELLID0_MASK               0x000000ff
#define BCHP_AVS_UART_UARTPCELLID0_UARTPCELLID0_SHIFT              0
#define BCHP_AVS_UART_UARTPCELLID0_UARTPCELLID0_DEFAULT            0x0000000d

/***************************************************************************
 *UARTPCELLID1 - UARTPCellID1
 ***************************************************************************/
/* AVS_UART :: UARTPCELLID1 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPCELLID1_reserved0_MASK                  0xffffff00
#define BCHP_AVS_UART_UARTPCELLID1_reserved0_SHIFT                 8

/* AVS_UART :: UARTPCELLID1 :: UARTPCELLID1 [07:00] */
#define BCHP_AVS_UART_UARTPCELLID1_UARTPCELLID1_MASK               0x000000ff
#define BCHP_AVS_UART_UARTPCELLID1_UARTPCELLID1_SHIFT              0
#define BCHP_AVS_UART_UARTPCELLID1_UARTPCELLID1_DEFAULT            0x000000f0

/***************************************************************************
 *UARTPCELLID2 - UARTPCellID2
 ***************************************************************************/
/* AVS_UART :: UARTPCELLID2 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPCELLID2_reserved0_MASK                  0xffffff00
#define BCHP_AVS_UART_UARTPCELLID2_reserved0_SHIFT                 8

/* AVS_UART :: UARTPCELLID2 :: UARTPCELLID2 [07:00] */
#define BCHP_AVS_UART_UARTPCELLID2_UARTPCELLID2_MASK               0x000000ff
#define BCHP_AVS_UART_UARTPCELLID2_UARTPCELLID2_SHIFT              0
#define BCHP_AVS_UART_UARTPCELLID2_UARTPCELLID2_DEFAULT            0x00000005

/***************************************************************************
 *UARTPCELLID3 - UARTPCellID3
 ***************************************************************************/
/* AVS_UART :: UARTPCELLID3 :: reserved0 [31:08] */
#define BCHP_AVS_UART_UARTPCELLID3_reserved0_MASK                  0xffffff00
#define BCHP_AVS_UART_UARTPCELLID3_reserved0_SHIFT                 8

/* AVS_UART :: UARTPCELLID3 :: UARTPCELLID3 [07:00] */
#define BCHP_AVS_UART_UARTPCELLID3_UARTPCELLID3_MASK               0x000000ff
#define BCHP_AVS_UART_UARTPCELLID3_UARTPCELLID3_SHIFT              0
#define BCHP_AVS_UART_UARTPCELLID3_UARTPCELLID3_DEFAULT            0x000000b1

#endif /* #ifndef BCHP_AVS_UART_H__ */

/* End of File */
