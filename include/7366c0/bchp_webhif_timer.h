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
 * Date:           Generated on               Wed Jul 29 03:14:26 2015
 *                 Full Compile MD5 Checksum  86ebfd14fa235167490b0fd28f5e6c5e
 *                     (minus title and desc)
 *                 MD5 Checksum               f257ce64a4906fa4bfc607d4b67e8459
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15517
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_WEBHIF_TIMER_H__
#define BCHP_WEBHIF_TIMER_H__

/***************************************************************************
 *WEBHIF_TIMER - WEBHIF timers and watchdog registers
 ***************************************************************************/
#define BCHP_WEBHIF_TIMER_TIMER_IS               0x00464900 /* [RW] TIMER INTERRUPT STATUS REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER_IE0              0x00464904 /* [RW] TIMER CPU INTERRUPT ENABLE REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL            0x00464908 /* [RW] TIMER0 CONTROL REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL            0x0046490c /* [RW] TIMER1 CONTROL REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL            0x00464910 /* [RW] TIMER2 CONTROL REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL            0x00464914 /* [RW] TIMER3 CONTROL REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER0_STAT            0x00464918 /* [RO] TIMER0 STATUS REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER1_STAT            0x0046491c /* [RO] TIMER1 STATUS REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER2_STAT            0x00464920 /* [RO] TIMER2 STATUS REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER3_STAT            0x00464924 /* [RO] TIMER3 STATUS REGISTER */
#define BCHP_WEBHIF_TIMER_WDTIMEOUT              0x00464928 /* [RW] WATCHDOG TIMEOUT REGISTER */
#define BCHP_WEBHIF_TIMER_WDCMD                  0x0046492c /* [WO] WATCHDOG COMMAND REGISTER */
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT          0x00464930 /* [RW] WATCHDOG CHIP RESET COUNT REGISTER */
#define BCHP_WEBHIF_TIMER_TIMER_IE1              0x00464938 /* [RW] TIMER PCI INTERRUPT ENABLE REGISTER */
#define BCHP_WEBHIF_TIMER_WDCTRL                 0x0046493c /* [RW] WATCHDOG CONTROL REGISTER */

/***************************************************************************
 *TIMER_IS - TIMER INTERRUPT STATUS REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER_IS :: reserved0 [31:05] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_reserved0_MASK                  0xffffffe0
#define BCHP_WEBHIF_TIMER_TIMER_IS_reserved0_SHIFT                 5

/* WEBHIF_TIMER :: TIMER_IS :: WDINT [04:04] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_WDINT_MASK                      0x00000010
#define BCHP_WEBHIF_TIMER_TIMER_IS_WDINT_SHIFT                     4
#define BCHP_WEBHIF_TIMER_TIMER_IS_WDINT_DEFAULT                   0x00000000

/* WEBHIF_TIMER :: TIMER_IS :: TMR3TO [03:03] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR3TO_MASK                     0x00000008
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR3TO_SHIFT                    3
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR3TO_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER_IS :: TMR2TO [02:02] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR2TO_MASK                     0x00000004
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR2TO_SHIFT                    2
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR2TO_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER_IS :: TMR1TO [01:01] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR1TO_MASK                     0x00000002
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR1TO_SHIFT                    1
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR1TO_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER_IS :: TMR0TO [00:00] */
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR0TO_MASK                     0x00000001
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR0TO_SHIFT                    0
#define BCHP_WEBHIF_TIMER_TIMER_IS_TMR0TO_DEFAULT                  0x00000000

/***************************************************************************
 *TIMER_IE0 - TIMER CPU INTERRUPT ENABLE REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER_IE0 :: reserved0 [31:05] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_reserved0_MASK                 0xffffffe0
#define BCHP_WEBHIF_TIMER_TIMER_IE0_reserved0_SHIFT                5

/* WEBHIF_TIMER :: TIMER_IE0 :: WDINTMASK [04:04] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_WDINTMASK_MASK                 0x00000010
#define BCHP_WEBHIF_TIMER_TIMER_IE0_WDINTMASK_SHIFT                4
#define BCHP_WEBHIF_TIMER_TIMER_IE0_WDINTMASK_DEFAULT              0x00000000

/* WEBHIF_TIMER :: TIMER_IE0 :: TMR3TO [03:03] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR3TO_MASK                    0x00000008
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR3TO_SHIFT                   3
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR3TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE0 :: TMR2TO [02:02] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR2TO_MASK                    0x00000004
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR2TO_SHIFT                   2
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR2TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE0 :: TMR1TO [01:01] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR1TO_MASK                    0x00000002
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR1TO_SHIFT                   1
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR1TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE0 :: TMR0TO [00:00] */
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR0TO_MASK                    0x00000001
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR0TO_SHIFT                   0
#define BCHP_WEBHIF_TIMER_TIMER_IE0_TMR0TO_DEFAULT                 0x00000000

/***************************************************************************
 *TIMER0_CTRL - TIMER0 CONTROL REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER0_CTRL :: ENA [31:31] */
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_ENA_MASK                     0x80000000
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_ENA_SHIFT                    31
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_ENA_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER0_CTRL :: MODE [30:30] */
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_MODE_MASK                    0x40000000
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_MODE_SHIFT                   30
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_MODE_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER0_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_TIMEOUT_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_TIMEOUT_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER0_CTRL_TIMEOUT_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER1_CTRL - TIMER1 CONTROL REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER1_CTRL :: ENA [31:31] */
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_ENA_MASK                     0x80000000
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_ENA_SHIFT                    31
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_ENA_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER1_CTRL :: MODE [30:30] */
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_MODE_MASK                    0x40000000
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_MODE_SHIFT                   30
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_MODE_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER1_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_TIMEOUT_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_TIMEOUT_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER1_CTRL_TIMEOUT_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER2_CTRL - TIMER2 CONTROL REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER2_CTRL :: ENA [31:31] */
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_ENA_MASK                     0x80000000
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_ENA_SHIFT                    31
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_ENA_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER2_CTRL :: MODE [30:30] */
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_MODE_MASK                    0x40000000
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_MODE_SHIFT                   30
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_MODE_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER2_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_TIMEOUT_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_TIMEOUT_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER2_CTRL_TIMEOUT_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER3_CTRL - TIMER3 CONTROL REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER3_CTRL :: ENA [31:31] */
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_ENA_MASK                     0x80000000
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_ENA_SHIFT                    31
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_ENA_DEFAULT                  0x00000000

/* WEBHIF_TIMER :: TIMER3_CTRL :: MODE [30:30] */
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_MODE_MASK                    0x40000000
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_MODE_SHIFT                   30
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_MODE_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER3_CTRL :: TIMEOUT_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_TIMEOUT_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_TIMEOUT_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER3_CTRL_TIMEOUT_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER0_STAT - TIMER0 STATUS REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER0_STAT :: SPARE [31:30] */
#define BCHP_WEBHIF_TIMER_TIMER0_STAT_SPARE_MASK                   0xc0000000
#define BCHP_WEBHIF_TIMER_TIMER0_STAT_SPARE_SHIFT                  30

/* WEBHIF_TIMER :: TIMER0_STAT :: COUNTER_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER0_STAT_COUNTER_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER0_STAT_COUNTER_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER0_STAT_COUNTER_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER1_STAT - TIMER1 STATUS REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER1_STAT :: SPARE [31:30] */
#define BCHP_WEBHIF_TIMER_TIMER1_STAT_SPARE_MASK                   0xc0000000
#define BCHP_WEBHIF_TIMER_TIMER1_STAT_SPARE_SHIFT                  30

/* WEBHIF_TIMER :: TIMER1_STAT :: COUNTER_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER1_STAT_COUNTER_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER1_STAT_COUNTER_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER1_STAT_COUNTER_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER2_STAT - TIMER2 STATUS REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER2_STAT :: SPARE [31:30] */
#define BCHP_WEBHIF_TIMER_TIMER2_STAT_SPARE_MASK                   0xc0000000
#define BCHP_WEBHIF_TIMER_TIMER2_STAT_SPARE_SHIFT                  30

/* WEBHIF_TIMER :: TIMER2_STAT :: COUNTER_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER2_STAT_COUNTER_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER2_STAT_COUNTER_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER2_STAT_COUNTER_VAL_DEFAULT          0x00000000

/***************************************************************************
 *TIMER3_STAT - TIMER3 STATUS REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER3_STAT :: SPARE [31:30] */
#define BCHP_WEBHIF_TIMER_TIMER3_STAT_SPARE_MASK                   0xc0000000
#define BCHP_WEBHIF_TIMER_TIMER3_STAT_SPARE_SHIFT                  30

/* WEBHIF_TIMER :: TIMER3_STAT :: COUNTER_VAL [29:00] */
#define BCHP_WEBHIF_TIMER_TIMER3_STAT_COUNTER_VAL_MASK             0x3fffffff
#define BCHP_WEBHIF_TIMER_TIMER3_STAT_COUNTER_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_TIMER3_STAT_COUNTER_VAL_DEFAULT          0x00000000

/***************************************************************************
 *WDTIMEOUT - WATCHDOG TIMEOUT REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: WDTIMEOUT :: WDTIMEOUT_VAL [31:00] */
#define BCHP_WEBHIF_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_MASK             0xffffffff
#define BCHP_WEBHIF_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_SHIFT            0
#define BCHP_WEBHIF_TIMER_WDTIMEOUT_WDTIMEOUT_VAL_DEFAULT          0x00000000

/***************************************************************************
 *WDCMD - WATCHDOG COMMAND REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: WDCMD :: WDCMD [31:00] */
#define BCHP_WEBHIF_TIMER_WDCMD_WDCMD_MASK                         0xffffffff
#define BCHP_WEBHIF_TIMER_WDCMD_WDCMD_SHIFT                        0
#define BCHP_WEBHIF_TIMER_WDCMD_WDCMD_DEFAULT                      0x00000000

/***************************************************************************
 *WDCHIPRST_CNT - WATCHDOG CHIP RESET COUNT REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: WDCHIPRST_CNT :: reserved0 [31:26] */
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT_reserved0_MASK             0xfc000000
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT_reserved0_SHIFT            26

/* WEBHIF_TIMER :: WDCHIPRST_CNT :: WDCHIPRST_CNT [25:00] */
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_MASK         0x03ffffff
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_SHIFT        0
#define BCHP_WEBHIF_TIMER_WDCHIPRST_CNT_WDCHIPRST_CNT_DEFAULT      0x02ffffff

/***************************************************************************
 *TIMER_IE1 - TIMER PCI INTERRUPT ENABLE REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: TIMER_IE1 :: reserved0 [31:05] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_reserved0_MASK                 0xffffffe0
#define BCHP_WEBHIF_TIMER_TIMER_IE1_reserved0_SHIFT                5

/* WEBHIF_TIMER :: TIMER_IE1 :: WDINTMASK [04:04] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_WDINTMASK_MASK                 0x00000010
#define BCHP_WEBHIF_TIMER_TIMER_IE1_WDINTMASK_SHIFT                4
#define BCHP_WEBHIF_TIMER_TIMER_IE1_WDINTMASK_DEFAULT              0x00000000

/* WEBHIF_TIMER :: TIMER_IE1 :: TMR3TO [03:03] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR3TO_MASK                    0x00000008
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR3TO_SHIFT                   3
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR3TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE1 :: TMR2TO [02:02] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR2TO_MASK                    0x00000004
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR2TO_SHIFT                   2
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR2TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE1 :: TMR1TO [01:01] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR1TO_MASK                    0x00000002
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR1TO_SHIFT                   1
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR1TO_DEFAULT                 0x00000000

/* WEBHIF_TIMER :: TIMER_IE1 :: TMR0TO [00:00] */
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR0TO_MASK                    0x00000001
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR0TO_SHIFT                   0
#define BCHP_WEBHIF_TIMER_TIMER_IE1_TMR0TO_DEFAULT                 0x00000000

/***************************************************************************
 *WDCTRL - WATCHDOG CONTROL REGISTER
 ***************************************************************************/
/* WEBHIF_TIMER :: WDCTRL :: reserved0 [31:03] */
#define BCHP_WEBHIF_TIMER_WDCTRL_reserved0_MASK                    0xfffffff8
#define BCHP_WEBHIF_TIMER_WDCTRL_reserved0_SHIFT                   3

/* WEBHIF_TIMER :: WDCTRL :: WD_COUNT_MODE [02:02] */
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_COUNT_MODE_MASK                0x00000004
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_COUNT_MODE_SHIFT               2
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_COUNT_MODE_DEFAULT             0x00000000

/* WEBHIF_TIMER :: WDCTRL :: WD_EVENT_MODE [01:00] */
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_EVENT_MODE_MASK                0x00000003
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_EVENT_MODE_SHIFT               0
#define BCHP_WEBHIF_TIMER_WDCTRL_WD_EVENT_MODE_DEFAULT             0x00000000

#endif /* #ifndef BCHP_WEBHIF_TIMER_H__ */

/* End of File */
