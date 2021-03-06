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
 * Date:           Generated on               Thu Feb  5 03:15:10 2015
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

#ifndef BCHP_IRQ0_H__
#define BCHP_IRQ0_H__

/***************************************************************************
 *IRQ0 - Level 2 CPU Interrupt Enable/Status
 ***************************************************************************/
#define BCHP_IRQ0_IRQEN                          0x0040a580 /* [RW] Interrupt Enable */
#define BCHP_IRQ0_IRQSTAT                        0x0040a584 /* [RW] Interrupt Status */

/***************************************************************************
 *IRQEN - Interrupt Enable
 ***************************************************************************/
/* IRQ0 :: IRQEN :: reserved0 [31:13] */
#define BCHP_IRQ0_IRQEN_reserved0_MASK                             0xffffe000
#define BCHP_IRQ0_IRQEN_reserved0_SHIFT                            13

/* IRQ0 :: IRQEN :: irb_irqen [12:12] */
#define BCHP_IRQ0_IRQEN_irb_irqen_MASK                             0x00001000
#define BCHP_IRQ0_IRQEN_irb_irqen_SHIFT                            12
#define BCHP_IRQ0_IRQEN_irb_irqen_DEFAULT                          0x00000000

/* IRQ0 :: IRQEN :: gio_irqen [11:11] */
#define BCHP_IRQ0_IRQEN_gio_irqen_MASK                             0x00000800
#define BCHP_IRQ0_IRQEN_gio_irqen_SHIFT                            11
#define BCHP_IRQ0_IRQEN_gio_irqen_DEFAULT                          0x00000000

/* IRQ0 :: IRQEN :: iicg_irqen [10:10] */
#define BCHP_IRQ0_IRQEN_iicg_irqen_MASK                            0x00000400
#define BCHP_IRQ0_IRQEN_iicg_irqen_SHIFT                           10
#define BCHP_IRQ0_IRQEN_iicg_irqen_DEFAULT                         0x00000000

/* IRQ0 :: IRQEN :: iicf_irqen [09:09] */
#define BCHP_IRQ0_IRQEN_iicf_irqen_MASK                            0x00000200
#define BCHP_IRQ0_IRQEN_iicf_irqen_SHIFT                           9
#define BCHP_IRQ0_IRQEN_iicf_irqen_DEFAULT                         0x00000000

/* IRQ0 :: IRQEN :: iice_irqen [08:08] */
#define BCHP_IRQ0_IRQEN_iice_irqen_MASK                            0x00000100
#define BCHP_IRQ0_IRQEN_iice_irqen_SHIFT                           8
#define BCHP_IRQ0_IRQEN_iice_irqen_DEFAULT                         0x00000000

/* IRQ0 :: IRQEN :: iicb_irqen [07:07] */
#define BCHP_IRQ0_IRQEN_iicb_irqen_MASK                            0x00000080
#define BCHP_IRQ0_IRQEN_iicb_irqen_SHIFT                           7
#define BCHP_IRQ0_IRQEN_iicb_irqen_DEFAULT                         0x00000000

/* IRQ0 :: IRQEN :: iica_irqen [06:06] */
#define BCHP_IRQ0_IRQEN_iica_irqen_MASK                            0x00000040
#define BCHP_IRQ0_IRQEN_iica_irqen_SHIFT                           6
#define BCHP_IRQ0_IRQEN_iica_irqen_DEFAULT                         0x00000000

/* IRQ0 :: IRQEN :: uc_irqen [05:05] */
#define BCHP_IRQ0_IRQEN_uc_irqen_MASK                              0x00000020
#define BCHP_IRQ0_IRQEN_uc_irqen_SHIFT                             5
#define BCHP_IRQ0_IRQEN_uc_irqen_DEFAULT                           0x00000000

/* IRQ0 :: IRQEN :: ub_irqen [04:04] */
#define BCHP_IRQ0_IRQEN_ub_irqen_MASK                              0x00000010
#define BCHP_IRQ0_IRQEN_ub_irqen_SHIFT                             4
#define BCHP_IRQ0_IRQEN_ub_irqen_DEFAULT                           0x00000000

/* IRQ0 :: IRQEN :: ua_irqen [03:03] */
#define BCHP_IRQ0_IRQEN_ua_irqen_MASK                              0x00000008
#define BCHP_IRQ0_IRQEN_ua_irqen_SHIFT                             3
#define BCHP_IRQ0_IRQEN_ua_irqen_DEFAULT                           0x00000000

/* IRQ0 :: IRQEN :: uartc_irqen [02:02] */
#define BCHP_IRQ0_IRQEN_uartc_irqen_MASK                           0x00000004
#define BCHP_IRQ0_IRQEN_uartc_irqen_SHIFT                          2
#define BCHP_IRQ0_IRQEN_uartc_irqen_DEFAULT                        0x00000000

/* IRQ0 :: IRQEN :: uartb_irqen [01:01] */
#define BCHP_IRQ0_IRQEN_uartb_irqen_MASK                           0x00000002
#define BCHP_IRQ0_IRQEN_uartb_irqen_SHIFT                          1
#define BCHP_IRQ0_IRQEN_uartb_irqen_DEFAULT                        0x00000000

/* IRQ0 :: IRQEN :: uarta_irqen [00:00] */
#define BCHP_IRQ0_IRQEN_uarta_irqen_MASK                           0x00000001
#define BCHP_IRQ0_IRQEN_uarta_irqen_SHIFT                          0
#define BCHP_IRQ0_IRQEN_uarta_irqen_DEFAULT                        0x00000000

/***************************************************************************
 *IRQSTAT - Interrupt Status
 ***************************************************************************/
/* IRQ0 :: IRQSTAT :: reserved0 [31:13] */
#define BCHP_IRQ0_IRQSTAT_reserved0_MASK                           0xffffe000
#define BCHP_IRQ0_IRQSTAT_reserved0_SHIFT                          13

/* IRQ0 :: IRQSTAT :: irbirq [12:12] */
#define BCHP_IRQ0_IRQSTAT_irbirq_MASK                              0x00001000
#define BCHP_IRQ0_IRQSTAT_irbirq_SHIFT                             12
#define BCHP_IRQ0_IRQSTAT_irbirq_DEFAULT                           0x00000000

/* IRQ0 :: IRQSTAT :: gioirq [11:11] */
#define BCHP_IRQ0_IRQSTAT_gioirq_MASK                              0x00000800
#define BCHP_IRQ0_IRQSTAT_gioirq_SHIFT                             11
#define BCHP_IRQ0_IRQSTAT_gioirq_DEFAULT                           0x00000000

/* IRQ0 :: IRQSTAT :: iicgirq [10:10] */
#define BCHP_IRQ0_IRQSTAT_iicgirq_MASK                             0x00000400
#define BCHP_IRQ0_IRQSTAT_iicgirq_SHIFT                            10
#define BCHP_IRQ0_IRQSTAT_iicgirq_DEFAULT                          0x00000000

/* IRQ0 :: IRQSTAT :: iicfirq [09:09] */
#define BCHP_IRQ0_IRQSTAT_iicfirq_MASK                             0x00000200
#define BCHP_IRQ0_IRQSTAT_iicfirq_SHIFT                            9
#define BCHP_IRQ0_IRQSTAT_iicfirq_DEFAULT                          0x00000000

/* IRQ0 :: IRQSTAT :: iiceirq [08:08] */
#define BCHP_IRQ0_IRQSTAT_iiceirq_MASK                             0x00000100
#define BCHP_IRQ0_IRQSTAT_iiceirq_SHIFT                            8
#define BCHP_IRQ0_IRQSTAT_iiceirq_DEFAULT                          0x00000000

/* IRQ0 :: IRQSTAT :: iicbirq [07:07] */
#define BCHP_IRQ0_IRQSTAT_iicbirq_MASK                             0x00000080
#define BCHP_IRQ0_IRQSTAT_iicbirq_SHIFT                            7
#define BCHP_IRQ0_IRQSTAT_iicbirq_DEFAULT                          0x00000000

/* IRQ0 :: IRQSTAT :: iicairq [06:06] */
#define BCHP_IRQ0_IRQSTAT_iicairq_MASK                             0x00000040
#define BCHP_IRQ0_IRQSTAT_iicairq_SHIFT                            6
#define BCHP_IRQ0_IRQSTAT_iicairq_DEFAULT                          0x00000000

/* IRQ0 :: IRQSTAT :: ucirq [05:05] */
#define BCHP_IRQ0_IRQSTAT_ucirq_MASK                               0x00000020
#define BCHP_IRQ0_IRQSTAT_ucirq_SHIFT                              5
#define BCHP_IRQ0_IRQSTAT_ucirq_DEFAULT                            0x00000000

/* IRQ0 :: IRQSTAT :: ubirq [04:04] */
#define BCHP_IRQ0_IRQSTAT_ubirq_MASK                               0x00000010
#define BCHP_IRQ0_IRQSTAT_ubirq_SHIFT                              4
#define BCHP_IRQ0_IRQSTAT_ubirq_DEFAULT                            0x00000000

/* IRQ0 :: IRQSTAT :: uairq [03:03] */
#define BCHP_IRQ0_IRQSTAT_uairq_MASK                               0x00000008
#define BCHP_IRQ0_IRQSTAT_uairq_SHIFT                              3
#define BCHP_IRQ0_IRQSTAT_uairq_DEFAULT                            0x00000000

/* IRQ0 :: IRQSTAT :: uartc_irq [02:02] */
#define BCHP_IRQ0_IRQSTAT_uartc_irq_MASK                           0x00000004
#define BCHP_IRQ0_IRQSTAT_uartc_irq_SHIFT                          2
#define BCHP_IRQ0_IRQSTAT_uartc_irq_DEFAULT                        0x00000000

/* IRQ0 :: IRQSTAT :: uartb_irq [01:01] */
#define BCHP_IRQ0_IRQSTAT_uartb_irq_MASK                           0x00000002
#define BCHP_IRQ0_IRQSTAT_uartb_irq_SHIFT                          1
#define BCHP_IRQ0_IRQSTAT_uartb_irq_DEFAULT                        0x00000000

/* IRQ0 :: IRQSTAT :: uarta_irq [00:00] */
#define BCHP_IRQ0_IRQSTAT_uarta_irq_MASK                           0x00000001
#define BCHP_IRQ0_IRQSTAT_uarta_irq_SHIFT                          0
#define BCHP_IRQ0_IRQSTAT_uarta_irq_DEFAULT                        0x00000000

#endif /* #ifndef BCHP_IRQ0_H__ */

/* End of File */
