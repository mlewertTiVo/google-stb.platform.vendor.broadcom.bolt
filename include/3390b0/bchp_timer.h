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
 * Date:           Generated on               Mon May  4 10:42:12 2015
 *                 Full Compile MD5 Checksum  3fa71b7874a727210919dce012235db9
 *                     (minus title and desc)
 *                 MD5 Checksum               354cf45b714a5565197034df92d073c7
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15517
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008005
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_TIMER_H__
#define BCHP_TIMER_H__

/***************************************************************************
 *TIMER - Watchdog & Programmable Timers
 ***************************************************************************/
#define BCHP_TIMER_TIMER_IS                      0x2040a100 /* [RW] TIMER INTERRUPT STATUS REGISTER */
#define BCHP_TIMER_TIMER_IE0                     0x2040a104 /* [RW] TIMER CPU INTERRUPT ENABLE REGISTER */
#define BCHP_TIMER_TIMER0_CTRL                   0x2040a108 /* [RW] TIMER0 CONTROL REGISTER */
#define BCHP_TIMER_TIMER1_CTRL                   0x2040a10c /* [RW] TIMER1 CONTROL REGISTER */
#define BCHP_TIMER_TIMER2_CTRL                   0x2040a110 /* [RW] TIMER2 CONTROL REGISTER */
#define BCHP_TIMER_TIMER3_CTRL                   0x2040a114 /* [RW] TIMER3 CONTROL REGISTER */
#define BCHP_TIMER_TIMER0_STAT                   0x2040a118 /* [RO] TIMER0 STATUS REGISTER */
#define BCHP_TIMER_TIMER1_STAT                   0x2040a11c /* [RO] TIMER1 STATUS REGISTER */
#define BCHP_TIMER_TIMER2_STAT                   0x2040a120 /* [RO] TIMER2 STATUS REGISTER */
#define BCHP_TIMER_TIMER3_STAT                   0x2040a124 /* [RO] TIMER3 STATUS REGISTER */
#define BCHP_TIMER_WDTIMEOUT                     0x2040a128 /* [RW] WATCHDOG TIMEOUT REGISTER */
#define BCHP_TIMER_WDCMD                         0x2040a12c /* [WO] WATCHDOG COMMAND REGISTER */
#define BCHP_TIMER_WDCHIPRST_CNT                 0x2040a130 /* [RW] WATCHDOG CHIP RESET COUNT REGISTER */
#define BCHP_TIMER_TIMER_IE1                     0x2040a138 /* [RW] TIMER PCI INTERRUPT ENABLE REGISTER */
#define BCHP_TIMER_WDCTRL                        0x2040a13c /* [RW] WATCHDOG CONTROL REGISTER */

/***************************************************************************
 *TIMER_IS - TIMER INTERRUPT STATUS REGISTER
 ***************************************************************************/
/* TIMER :: TIMER_IS :: reserved0 [31:05] */
#define BCHP_TIMER_TIMER_IS_reserved0_MASK                         0xffffffe0
#define BCHP_TIMER_TIMER_IS_reserved0_SHIFT                        5

/* TIMER :: TIMER_IS :: WDINT [04:04] */
#define BCHP_TIMER_TIMER_IS_WDINT_MASK                             0x00000010
#define BCHP_TIMER_TIMER_IS_WDINT_SHIFT                            4
#define BCHP_TIMER_TIMER_IS_WDINT_DEFAULT                          0x00000000

/* TIMER :: TIMER_IS :: TMR3TO [03:03] */
#define BCHP_TIMER_TIMER_IS_TMR3TO_MASK                            0x00000008
#define BCHP_TIMER_TIMER_IS_TMR3TO_SHIFT                           3
#define BCHP_TIMER_TIMER_IS_TMR3TO_DEFAULT                         0x00000000

/* TIMER :: TIMER_IS :: TMR2TO [02:02] */
#define BCHP_TIMER_TIMER_IS_TMR2TO_MASK                            0x00000004
#define BCHP_TIMER_TIMER_IS_TMR2TO_SHIFT                           2
#define BCHP_TIMER_TIMER_IS_TMR2TO_DEFAULT                         0x00000000

/* TIMER :: TIMER_IS :: TMR1TO [01:01] */
#define BCHP_TIMER_TIMER_IS_TMR1TO_MASK                            0x00000002
#define BCHP_TIMER_TIMER_IS_TMR1TO_SHIFT                           1
#define BCHP_TIMER_TIMER_IS_TMR1TO_DEFAULT                         0x00000000

/* TIMER :: TIMER_IS :: TMR0TO [00:00] */
#define BCHP_TIMER_TIMER_IS_TMR0TO_MASK                            0x00000001
#define BCHP_TIMER_TIMER_IS_TMR0TO_SHIFT                           0
#define BCHP_TIMER_TIMER_IS_TMR0TO_DEFAULT                         0x00000000

/***************************************************************************
 *TIMER_IE0 - TIMER CPU INTERRUPT ENABLE REGISTER
 ***************************************************************************/
/* TIMER :: TIMER_IE0 :: reserved0 [31:05] */
#define BCHP_TIMER_TIMER_IE0_reserved0_MASK                        0xffffffe0
#define BCHP_TIMER_TIMER_IE0_reserved0_SHIFT                       5

/* TIMER :: TIMER_IE0 :: WDINTMASK [04:04] */
#define BCHP_TIMER_TIMER_IE0_WDINTMASK_MASK                        0x00000010
#define BCHP_TIMER_TIMER_IE0_WDINTMASK_SHIFT                       4
#define BCHP_TIMER_TIMER_IE0_WDINTMASK_DEFAULT                     0x00000000

/* TIMER :: TIMER_IE0 :: TMR3TO [03:03] */
#define BCHP_TIMER_TIMER_IE0_TMR3TO_MASK                           0x00000008
#define BCHP_TIMER_TIMER_IE0_TMR3TO_SHIFT                          3
#define BCHP_TIMER_TIMER_IE0_TMR3TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE0 :: TMR2TO [02:02] */
#define BCHP_TIMER_TIMER_IE0_TMR2TO_MASK                           0x00000004
#define BCHP_TIMER_TIMER_IE0_TMR2TO_SHIFT                          2
#define BCHP_TIMER_TIMER_IE0_TMR2TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE0 :: TMR1TO [01:01] */
#define BCHP_TIMER_TIMER_IE0_TMR1TO_MASK                           0x00000002
#define BCHP_TIMER_TIMER_IE0_TMR1TO_SHIFT                          1
#define BCHP_TIMER_TIMER_IE0_TMR1TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE0 :: TMR0TO [00:00] */
#define BCHP_TIMER_TIMER_IE0_TMR0TO_MASK                           0x00000001
#define BCHP_TIMER_TIMER_IE0_TMR0TO_SHIFT                          0
#define BCHP_TIMER_TIMER_IE0_TMR0TO_DEFAULT                        0x00000000

/***************************************************************************
 *TIMER0_CTRL - TIMER0 CONTROL REGISTER
 ***************************************************************************/
/* TIMER :: TIMER0_CTRL :: ENA [31:31] */
#define BCHP_TIMER_TIMER0_CTRL_ENA_MASK                            0x80000000
#define BCHP_TIMER_TIMER0_CTRL_ENA_SHIFT                           31
#define BCHP_TIMER_TIMER0_CTRL_ENA_DEFAULT                         0x00000000

/* TIMER :: TIMER0_CTRL :: MODE [30:30] */
#define BCHP_TIMER_TIMER0_CTRL_MODE_MASK                           0x40000000
#define BCHP_TIMER_TIMER0_CTRL_MODE_SHIFT                          30
#define BCHP_TIMER_TIMER0_CTRL_MODE_DEFAULT                        0x00000000

/* TIMER :: TIMER0_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_TIMER_TIMER0_CTRL_TIMEOUT_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER0_CTRL_TIMEOUT_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER0_CTRL_TIMEOUT_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER1_CTRL - TIMER1 CONTROL REGISTER
 ***************************************************************************/
/* TIMER :: TIMER1_CTRL :: ENA [31:31] */
#define BCHP_TIMER_TIMER1_CTRL_ENA_MASK                            0x80000000
#define BCHP_TIMER_TIMER1_CTRL_ENA_SHIFT                           31
#define BCHP_TIMER_TIMER1_CTRL_ENA_DEFAULT                         0x00000000

/* TIMER :: TIMER1_CTRL :: MODE [30:30] */
#define BCHP_TIMER_TIMER1_CTRL_MODE_MASK                           0x40000000
#define BCHP_TIMER_TIMER1_CTRL_MODE_SHIFT                          30
#define BCHP_TIMER_TIMER1_CTRL_MODE_DEFAULT                        0x00000000

/* TIMER :: TIMER1_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_TIMER_TIMER1_CTRL_TIMEOUT_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER1_CTRL_TIMEOUT_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER1_CTRL_TIMEOUT_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER2_CTRL - TIMER2 CONTROL REGISTER
 ***************************************************************************/
/* TIMER :: TIMER2_CTRL :: ENA [31:31] */
#define BCHP_TIMER_TIMER2_CTRL_ENA_MASK                            0x80000000
#define BCHP_TIMER_TIMER2_CTRL_ENA_SHIFT                           31
#define BCHP_TIMER_TIMER2_CTRL_ENA_DEFAULT                         0x00000000

/* TIMER :: TIMER2_CTRL :: MODE [30:30] */
#define BCHP_TIMER_TIMER2_CTRL_MODE_MASK                           0x40000000
#define BCHP_TIMER_TIMER2_CTRL_MODE_SHIFT                          30
#define BCHP_TIMER_TIMER2_CTRL_MODE_DEFAULT                        0x00000000

/* TIMER :: TIMER2_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_TIMER_TIMER2_CTRL_TIMEOUT_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER2_CTRL_TIMEOUT_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER2_CTRL_TIMEOUT_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER3_CTRL - TIMER3 CONTROL REGISTER
 ***************************************************************************/
/* TIMER :: TIMER3_CTRL :: ENA [31:31] */
#define BCHP_TIMER_TIMER3_CTRL_ENA_MASK                            0x80000000
#define BCHP_TIMER_TIMER3_CTRL_ENA_SHIFT                           31
#define BCHP_TIMER_TIMER3_CTRL_ENA_DEFAULT                         0x00000000

/* TIMER :: TIMER3_CTRL :: MODE [30:30] */
#define BCHP_TIMER_TIMER3_CTRL_MODE_MASK                           0x40000000
#define BCHP_TIMER_TIMER3_CTRL_MODE_SHIFT                          30
#define BCHP_TIMER_TIMER3_CTRL_MODE_DEFAULT                        0x00000000

/* TIMER :: TIMER3_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_TIMER_TIMER3_CTRL_TIMEOUT_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER3_CTRL_TIMEOUT_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER3_CTRL_TIMEOUT_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER0_STAT - TIMER0 STATUS REGISTER
 ***************************************************************************/
/* TIMER :: TIMER0_STAT :: SPARE [31:30] */
#define BCHP_TIMER_TIMER0_STAT_SPARE_MASK                          0xc0000000
#define BCHP_TIMER_TIMER0_STAT_SPARE_SHIFT                         30

/* TIMER :: TIMER0_STAT :: COUNTER_VAL [29:00] */
#define BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER1_STAT - TIMER1 STATUS REGISTER
 ***************************************************************************/
/* TIMER :: TIMER1_STAT :: SPARE [31:30] */
#define BCHP_TIMER_TIMER1_STAT_SPARE_MASK                          0xc0000000
#define BCHP_TIMER_TIMER1_STAT_SPARE_SHIFT                         30

/* TIMER :: TIMER1_STAT :: COUNTER_VAL [29:00] */
#define BCHP_TIMER_TIMER1_STAT_COUNTER_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER1_STAT_COUNTER_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER1_STAT_COUNTER_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER2_STAT - TIMER2 STATUS REGISTER
 ***************************************************************************/
/* TIMER :: TIMER2_STAT :: SPARE [31:30] */
#define BCHP_TIMER_TIMER2_STAT_SPARE_MASK                          0xc0000000
#define BCHP_TIMER_TIMER2_STAT_SPARE_SHIFT                         30

/* TIMER :: TIMER2_STAT :: COUNTER_VAL [29:00] */
#define BCHP_TIMER_TIMER2_STAT_COUNTER_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER2_STAT_COUNTER_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER2_STAT_COUNTER_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER3_STAT - TIMER3 STATUS REGISTER
 ***************************************************************************/
/* TIMER :: TIMER3_STAT :: SPARE [31:30] */
#define BCHP_TIMER_TIMER3_STAT_SPARE_MASK                          0xc0000000
#define BCHP_TIMER_TIMER3_STAT_SPARE_SHIFT                         30

/* TIMER :: TIMER3_STAT :: COUNTER_VAL [29:00] */
#define BCHP_TIMER_TIMER3_STAT_COUNTER_VAL_MASK                    0x3fffffff
#define BCHP_TIMER_TIMER3_STAT_COUNTER_VAL_SHIFT                   0
#define BCHP_TIMER_TIMER3_STAT_COUNTER_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *WDTIMEOUT - WATCHDOG TIMEOUT REGISTER
 ***************************************************************************/
/* TIMER :: WDTIMEOUT :: WDTIMEOUT_VAL [31:00] */
#define BCHP_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_MASK                    0xffffffff
#define BCHP_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_SHIFT                   0
#define BCHP_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_DEFAULT                 0x00000000

/***************************************************************************
 *WDCMD - WATCHDOG COMMAND REGISTER
 ***************************************************************************/
/* TIMER :: WDCMD :: WDCMD [31:00] */
#define BCHP_TIMER_WDCMD_WDCMD_MASK                                0xffffffff
#define BCHP_TIMER_WDCMD_WDCMD_SHIFT                               0
#define BCHP_TIMER_WDCMD_WDCMD_DEFAULT                             0x00000000

/***************************************************************************
 *WDCHIPRST_CNT - WATCHDOG CHIP RESET COUNT REGISTER
 ***************************************************************************/
/* TIMER :: WDCHIPRST_CNT :: reserved0 [31:26] */
#define BCHP_TIMER_WDCHIPRST_CNT_reserved0_MASK                    0xfc000000
#define BCHP_TIMER_WDCHIPRST_CNT_reserved0_SHIFT                   26

/* TIMER :: WDCHIPRST_CNT :: WDCHIPRST_CNT [25:00] */
#define BCHP_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_MASK                0x03ffffff
#define BCHP_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_SHIFT               0
#define BCHP_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_DEFAULT             0x02ffffff

/***************************************************************************
 *TIMER_IE1 - TIMER PCI INTERRUPT ENABLE REGISTER
 ***************************************************************************/
/* TIMER :: TIMER_IE1 :: reserved0 [31:05] */
#define BCHP_TIMER_TIMER_IE1_reserved0_MASK                        0xffffffe0
#define BCHP_TIMER_TIMER_IE1_reserved0_SHIFT                       5

/* TIMER :: TIMER_IE1 :: WDINTMASK [04:04] */
#define BCHP_TIMER_TIMER_IE1_WDINTMASK_MASK                        0x00000010
#define BCHP_TIMER_TIMER_IE1_WDINTMASK_SHIFT                       4
#define BCHP_TIMER_TIMER_IE1_WDINTMASK_DEFAULT                     0x00000000

/* TIMER :: TIMER_IE1 :: TMR3TO [03:03] */
#define BCHP_TIMER_TIMER_IE1_TMR3TO_MASK                           0x00000008
#define BCHP_TIMER_TIMER_IE1_TMR3TO_SHIFT                          3
#define BCHP_TIMER_TIMER_IE1_TMR3TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE1 :: TMR2TO [02:02] */
#define BCHP_TIMER_TIMER_IE1_TMR2TO_MASK                           0x00000004
#define BCHP_TIMER_TIMER_IE1_TMR2TO_SHIFT                          2
#define BCHP_TIMER_TIMER_IE1_TMR2TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE1 :: TMR1TO [01:01] */
#define BCHP_TIMER_TIMER_IE1_TMR1TO_MASK                           0x00000002
#define BCHP_TIMER_TIMER_IE1_TMR1TO_SHIFT                          1
#define BCHP_TIMER_TIMER_IE1_TMR1TO_DEFAULT                        0x00000000

/* TIMER :: TIMER_IE1 :: TMR0TO [00:00] */
#define BCHP_TIMER_TIMER_IE1_TMR0TO_MASK                           0x00000001
#define BCHP_TIMER_TIMER_IE1_TMR0TO_SHIFT                          0
#define BCHP_TIMER_TIMER_IE1_TMR0TO_DEFAULT                        0x00000000

/***************************************************************************
 *WDCTRL - WATCHDOG CONTROL REGISTER
 ***************************************************************************/
/* TIMER :: WDCTRL :: reserved0 [31:03] */
#define BCHP_TIMER_WDCTRL_reserved0_MASK                           0xfffffff8
#define BCHP_TIMER_WDCTRL_reserved0_SHIFT                          3

/* TIMER :: WDCTRL :: WD_COUNT_MODE [02:02] */
#define BCHP_TIMER_WDCTRL_WD_COUNT_MODE_MASK                       0x00000004
#define BCHP_TIMER_WDCTRL_WD_COUNT_MODE_SHIFT                      2
#define BCHP_TIMER_WDCTRL_WD_COUNT_MODE_DEFAULT                    0x00000000

/* TIMER :: WDCTRL :: WD_EVENT_MODE [01:00] */
#define BCHP_TIMER_WDCTRL_WD_EVENT_MODE_MASK                       0x00000003
#define BCHP_TIMER_WDCTRL_WD_EVENT_MODE_SHIFT                      0
#define BCHP_TIMER_WDCTRL_WD_EVENT_MODE_DEFAULT                    0x00000000

#endif /* #ifndef BCHP_TIMER_H__ */

/* End of File */
