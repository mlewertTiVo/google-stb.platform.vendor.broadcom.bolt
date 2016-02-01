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
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Wed Jul 30 03:07:24 2014
 *                 Full Compile MD5 Checksum 10187d4079392bab2546025f43274d34
 *                   (minus title and desc)  
 *                 MD5 Checksum              c1587c5e16f21f52e852e7c7a65c7811
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

#ifndef BCHP_IRQ0_AON_H__
#define BCHP_IRQ0_AON_H__

/***************************************************************************
 *IRQ0_AON - Level 2 CPU Interrupt Enable/Status
 ***************************************************************************/
#define BCHP_IRQ0_AON_IRQEN                      0x00417280 /* Interrupt Enable */
#define BCHP_IRQ0_AON_IRQSTAT                    0x00417284 /* Interrupt Status */

/***************************************************************************
 *IRQEN - Interrupt Enable
 ***************************************************************************/
/* IRQ0_AON :: IRQEN :: reserved0 [31:09] */
#define BCHP_IRQ0_AON_IRQEN_reserved0_MASK                         0xfffffe00
#define BCHP_IRQ0_AON_IRQEN_reserved0_SHIFT                        9

/* IRQ0_AON :: IRQEN :: icap_irqen [08:08] */
#define BCHP_IRQ0_AON_IRQEN_icap_irqen_MASK                        0x00000100
#define BCHP_IRQ0_AON_IRQEN_icap_irqen_SHIFT                       8
#define BCHP_IRQ0_AON_IRQEN_icap_irqen_DEFAULT                     0x00000000

/* IRQ0_AON :: IRQEN :: spi_irqen [07:07] */
#define BCHP_IRQ0_AON_IRQEN_spi_irqen_MASK                         0x00000080
#define BCHP_IRQ0_AON_IRQEN_spi_irqen_SHIFT                        7
#define BCHP_IRQ0_AON_IRQEN_spi_irqen_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQEN :: ldk_irqen [06:06] */
#define BCHP_IRQ0_AON_IRQEN_ldk_irqen_MASK                         0x00000040
#define BCHP_IRQ0_AON_IRQEN_ldk_irqen_SHIFT                        6
#define BCHP_IRQ0_AON_IRQEN_ldk_irqen_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQEN :: gio_irqen [05:05] */
#define BCHP_IRQ0_AON_IRQEN_gio_irqen_MASK                         0x00000020
#define BCHP_IRQ0_AON_IRQEN_gio_irqen_SHIFT                        5
#define BCHP_IRQ0_AON_IRQEN_gio_irqen_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQEN :: iicd_irqen [04:04] */
#define BCHP_IRQ0_AON_IRQEN_iicd_irqen_MASK                        0x00000010
#define BCHP_IRQ0_AON_IRQEN_iicd_irqen_SHIFT                       4
#define BCHP_IRQ0_AON_IRQEN_iicd_irqen_DEFAULT                     0x00000000

/* IRQ0_AON :: IRQEN :: iicc_irqen [03:03] */
#define BCHP_IRQ0_AON_IRQEN_iicc_irqen_MASK                        0x00000008
#define BCHP_IRQ0_AON_IRQEN_iicc_irqen_SHIFT                       3
#define BCHP_IRQ0_AON_IRQEN_iicc_irqen_DEFAULT                     0x00000000

/* IRQ0_AON :: IRQEN :: kbd3_irqen [02:02] */
#define BCHP_IRQ0_AON_IRQEN_kbd3_irqen_MASK                        0x00000004
#define BCHP_IRQ0_AON_IRQEN_kbd3_irqen_SHIFT                       2
#define BCHP_IRQ0_AON_IRQEN_kbd3_irqen_DEFAULT                     0x00000000

/* IRQ0_AON :: IRQEN :: kbd2_irqen [01:01] */
#define BCHP_IRQ0_AON_IRQEN_kbd2_irqen_MASK                        0x00000002
#define BCHP_IRQ0_AON_IRQEN_kbd2_irqen_SHIFT                       1
#define BCHP_IRQ0_AON_IRQEN_kbd2_irqen_DEFAULT                     0x00000000

/* IRQ0_AON :: IRQEN :: kbd1_irqen [00:00] */
#define BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK                        0x00000001
#define BCHP_IRQ0_AON_IRQEN_kbd1_irqen_SHIFT                       0
#define BCHP_IRQ0_AON_IRQEN_kbd1_irqen_DEFAULT                     0x00000000

/***************************************************************************
 *IRQSTAT - Interrupt Status
 ***************************************************************************/
/* IRQ0_AON :: IRQSTAT :: reserved0 [31:09] */
#define BCHP_IRQ0_AON_IRQSTAT_reserved0_MASK                       0xfffffe00
#define BCHP_IRQ0_AON_IRQSTAT_reserved0_SHIFT                      9

/* IRQ0_AON :: IRQSTAT :: icapirq [08:08] */
#define BCHP_IRQ0_AON_IRQSTAT_icapirq_MASK                         0x00000100
#define BCHP_IRQ0_AON_IRQSTAT_icapirq_SHIFT                        8
#define BCHP_IRQ0_AON_IRQSTAT_icapirq_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQSTAT :: spiirq [07:07] */
#define BCHP_IRQ0_AON_IRQSTAT_spiirq_MASK                          0x00000080
#define BCHP_IRQ0_AON_IRQSTAT_spiirq_SHIFT                         7
#define BCHP_IRQ0_AON_IRQSTAT_spiirq_DEFAULT                       0x00000000

/* IRQ0_AON :: IRQSTAT :: ldkirq [06:06] */
#define BCHP_IRQ0_AON_IRQSTAT_ldkirq_MASK                          0x00000040
#define BCHP_IRQ0_AON_IRQSTAT_ldkirq_SHIFT                         6
#define BCHP_IRQ0_AON_IRQSTAT_ldkirq_DEFAULT                       0x00000000

/* IRQ0_AON :: IRQSTAT :: gioirq [05:05] */
#define BCHP_IRQ0_AON_IRQSTAT_gioirq_MASK                          0x00000020
#define BCHP_IRQ0_AON_IRQSTAT_gioirq_SHIFT                         5
#define BCHP_IRQ0_AON_IRQSTAT_gioirq_DEFAULT                       0x00000000

/* IRQ0_AON :: IRQSTAT :: iicdirq [04:04] */
#define BCHP_IRQ0_AON_IRQSTAT_iicdirq_MASK                         0x00000010
#define BCHP_IRQ0_AON_IRQSTAT_iicdirq_SHIFT                        4
#define BCHP_IRQ0_AON_IRQSTAT_iicdirq_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQSTAT :: iiccirq [03:03] */
#define BCHP_IRQ0_AON_IRQSTAT_iiccirq_MASK                         0x00000008
#define BCHP_IRQ0_AON_IRQSTAT_iiccirq_SHIFT                        3
#define BCHP_IRQ0_AON_IRQSTAT_iiccirq_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQSTAT :: kbd3irq [02:02] */
#define BCHP_IRQ0_AON_IRQSTAT_kbd3irq_MASK                         0x00000004
#define BCHP_IRQ0_AON_IRQSTAT_kbd3irq_SHIFT                        2
#define BCHP_IRQ0_AON_IRQSTAT_kbd3irq_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQSTAT :: kbd2irq [01:01] */
#define BCHP_IRQ0_AON_IRQSTAT_kbd2irq_MASK                         0x00000002
#define BCHP_IRQ0_AON_IRQSTAT_kbd2irq_SHIFT                        1
#define BCHP_IRQ0_AON_IRQSTAT_kbd2irq_DEFAULT                      0x00000000

/* IRQ0_AON :: IRQSTAT :: kbd1irq [00:00] */
#define BCHP_IRQ0_AON_IRQSTAT_kbd1irq_MASK                         0x00000001
#define BCHP_IRQ0_AON_IRQSTAT_kbd1irq_SHIFT                        0
#define BCHP_IRQ0_AON_IRQSTAT_kbd1irq_DEFAULT                      0x00000000

#endif /* #ifndef BCHP_IRQ0_AON_H__ */

/* End of File */
