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
 * Date:           Generated on               Thu Nov 23 01:10:02 2017
 *                 Full Compile MD5 Checksum  9c249593c44f5a3709c1aa2deb2a32b9
 *                     (minus title and desc)
 *                 MD5 Checksum               bedd24dac2c620a65db09cca853e657e
 *
 * lock_release:   r_1255
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

#ifndef BCHP_USB_EHCI1_H__
#define BCHP_USB_EHCI1_H__

/***************************************************************************
 *USB_EHCI1 - USB EHCI 1 Control Registers
 ***************************************************************************/
#define BCHP_USB_EHCI1_HCCAPBASE                 0x0020b00500 /* [RO][32] EHCI Capability Register */
#define BCHP_USB_EHCI1_HCSPARAMS                 0x0020b00504 /* [RO][32] EHCI Structural Parameter */
#define BCHP_USB_EHCI1_HCCPARAMS                 0x0020b00508 /* [RO][32] EHCI Capability Parameter */
#define BCHP_USB_EHCI1_USBCMD                    0x0020b00510 /* [RW][32] USB Command Register */
#define BCHP_USB_EHCI1_USBSTS                    0x0020b00514 /* [RW][32] USB Status  Register */
#define BCHP_USB_EHCI1_USBINTR                   0x0020b00518 /* [RW][32] USB Interrupt Enable Register */
#define BCHP_USB_EHCI1_FRINDEX                   0x0020b0051c /* [RW][32] USB Frame Index Register */
#define BCHP_USB_EHCI1_CTRLDSSEGMENT             0x0020b00520 /* [RW][32] Control Data Structure Segment Register */
#define BCHP_USB_EHCI1_PERIODICLISTBASE          0x0020b00524 /* [RW][32] Periodic Frame List Base Address Register */
#define BCHP_USB_EHCI1_ASYNCLISTADDR             0x0020b00528 /* [RW][32] Asynchronous List Address */
#define BCHP_USB_EHCI1_CONFIGFLAG                0x0020b00550 /* [RW][32] Configured Flag Register */
#define BCHP_USB_EHCI1_PORTSC_0                  0x0020b00554 /* [RW][32] Port Status/Control Register for Port 0 */
#define BCHP_USB_EHCI1_INSNREG00                 0x0020b00590 /* [RW][32] Microframe Base Value Register */
#define BCHP_USB_EHCI1_INSNREG01                 0x0020b00594 /* [RW][32] Packet Buffer OUT/IN Threshold Register */
#define BCHP_USB_EHCI1_INSNREG02                 0x0020b00598 /* [RW][32] Packet Buffer Depth Register */
#define BCHP_USB_EHCI1_INSNREG03                 0x0020b0059c /* [RW][32] Break Memory Transfer Register */
#define BCHP_USB_EHCI1_INSNREG04                 0x0020b005a0 /* [RW][32] Debug Register */
#define BCHP_USB_EHCI1_INSNREG05                 0x0020b005a4 /* [RW][32] UTMI Control and Status Register */

/***************************************************************************
 *HCCAPBASE - EHCI Capability Register
 ***************************************************************************/
/* USB_EHCI1 :: HCCAPBASE :: HCIVERSION [31:16] */
#define BCHP_USB_EHCI1_HCCAPBASE_HCIVERSION_MASK                   0xffff0000
#define BCHP_USB_EHCI1_HCCAPBASE_HCIVERSION_SHIFT                  16
#define BCHP_USB_EHCI1_HCCAPBASE_HCIVERSION_DEFAULT                0x00000100

/* USB_EHCI1 :: HCCAPBASE :: reserved0 [15:08] */
#define BCHP_USB_EHCI1_HCCAPBASE_reserved0_MASK                    0x0000ff00
#define BCHP_USB_EHCI1_HCCAPBASE_reserved0_SHIFT                   8

/* USB_EHCI1 :: HCCAPBASE :: CAPLENGTH [07:00] */
#define BCHP_USB_EHCI1_HCCAPBASE_CAPLENGTH_MASK                    0x000000ff
#define BCHP_USB_EHCI1_HCCAPBASE_CAPLENGTH_SHIFT                   0
#define BCHP_USB_EHCI1_HCCAPBASE_CAPLENGTH_DEFAULT                 0x00000010

/***************************************************************************
 *HCSPARAMS - EHCI Structural Parameter
 ***************************************************************************/
/* USB_EHCI1 :: HCSPARAMS :: reserved0 [31:24] */
#define BCHP_USB_EHCI1_HCSPARAMS_reserved0_MASK                    0xff000000
#define BCHP_USB_EHCI1_HCSPARAMS_reserved0_SHIFT                   24

/* USB_EHCI1 :: HCSPARAMS :: DEBUG_PORT_NUMBER [23:20] */
#define BCHP_USB_EHCI1_HCSPARAMS_DEBUG_PORT_NUMBER_MASK            0x00f00000
#define BCHP_USB_EHCI1_HCSPARAMS_DEBUG_PORT_NUMBER_SHIFT           20
#define BCHP_USB_EHCI1_HCSPARAMS_DEBUG_PORT_NUMBER_DEFAULT         0x00000000

/* USB_EHCI1 :: HCSPARAMS :: reserved1 [19:17] */
#define BCHP_USB_EHCI1_HCSPARAMS_reserved1_MASK                    0x000e0000
#define BCHP_USB_EHCI1_HCSPARAMS_reserved1_SHIFT                   17

/* USB_EHCI1 :: HCSPARAMS :: P_INDICATOR [16:16] */
#define BCHP_USB_EHCI1_HCSPARAMS_P_INDICATOR_MASK                  0x00010000
#define BCHP_USB_EHCI1_HCSPARAMS_P_INDICATOR_SHIFT                 16
#define BCHP_USB_EHCI1_HCSPARAMS_P_INDICATOR_DEFAULT               0x00000000

/* USB_EHCI1 :: HCSPARAMS :: N_CC [15:12] */
#define BCHP_USB_EHCI1_HCSPARAMS_N_CC_MASK                         0x0000f000
#define BCHP_USB_EHCI1_HCSPARAMS_N_CC_SHIFT                        12
#define BCHP_USB_EHCI1_HCSPARAMS_N_CC_DEFAULT                      0x00000001

/* USB_EHCI1 :: HCSPARAMS :: N_PCC [11:08] */
#define BCHP_USB_EHCI1_HCSPARAMS_N_PCC_MASK                        0x00000f00
#define BCHP_USB_EHCI1_HCSPARAMS_N_PCC_SHIFT                       8
#define BCHP_USB_EHCI1_HCSPARAMS_N_PCC_DEFAULT                     0x00000001

/* USB_EHCI1 :: HCSPARAMS :: PORT_ROUTING_RULES [07:07] */
#define BCHP_USB_EHCI1_HCSPARAMS_PORT_ROUTING_RULES_MASK           0x00000080
#define BCHP_USB_EHCI1_HCSPARAMS_PORT_ROUTING_RULES_SHIFT          7
#define BCHP_USB_EHCI1_HCSPARAMS_PORT_ROUTING_RULES_DEFAULT        0x00000000

/* USB_EHCI1 :: HCSPARAMS :: reserved2 [06:05] */
#define BCHP_USB_EHCI1_HCSPARAMS_reserved2_MASK                    0x00000060
#define BCHP_USB_EHCI1_HCSPARAMS_reserved2_SHIFT                   5

/* USB_EHCI1 :: HCSPARAMS :: PPC [04:04] */
#define BCHP_USB_EHCI1_HCSPARAMS_PPC_MASK                          0x00000010
#define BCHP_USB_EHCI1_HCSPARAMS_PPC_SHIFT                         4
#define BCHP_USB_EHCI1_HCSPARAMS_PPC_DEFAULT                       0x00000001

/* USB_EHCI1 :: HCSPARAMS :: N_PORTS [03:00] */
#define BCHP_USB_EHCI1_HCSPARAMS_N_PORTS_MASK                      0x0000000f
#define BCHP_USB_EHCI1_HCSPARAMS_N_PORTS_SHIFT                     0
#define BCHP_USB_EHCI1_HCSPARAMS_N_PORTS_DEFAULT                   0x00000001

/***************************************************************************
 *HCCPARAMS - EHCI Capability Parameter
 ***************************************************************************/
/* USB_EHCI1 :: HCCPARAMS :: reserved0 [31:16] */
#define BCHP_USB_EHCI1_HCCPARAMS_reserved0_MASK                    0xffff0000
#define BCHP_USB_EHCI1_HCCPARAMS_reserved0_SHIFT                   16

/* USB_EHCI1 :: HCCPARAMS :: EECP [15:08] */
#define BCHP_USB_EHCI1_HCCPARAMS_EECP_MASK                         0x0000ff00
#define BCHP_USB_EHCI1_HCCPARAMS_EECP_SHIFT                        8
#define BCHP_USB_EHCI1_HCCPARAMS_EECP_DEFAULT                      0x000000a0

/* USB_EHCI1 :: HCCPARAMS :: ISOCHRONOUS_SCHEDULING_THRESHOLD [07:04] */
#define BCHP_USB_EHCI1_HCCPARAMS_ISOCHRONOUS_SCHEDULING_THRESHOLD_MASK 0x000000f0
#define BCHP_USB_EHCI1_HCCPARAMS_ISOCHRONOUS_SCHEDULING_THRESHOLD_SHIFT 4
#define BCHP_USB_EHCI1_HCCPARAMS_ISOCHRONOUS_SCHEDULING_THRESHOLD_DEFAULT 0x00000001

/* USB_EHCI1 :: HCCPARAMS :: reserved1 [03:03] */
#define BCHP_USB_EHCI1_HCCPARAMS_reserved1_MASK                    0x00000008
#define BCHP_USB_EHCI1_HCCPARAMS_reserved1_SHIFT                   3

/* USB_EHCI1 :: HCCPARAMS :: ASYNCHRONOUS_SCHEDULE_PARK_CAPABILITY [02:02] */
#define BCHP_USB_EHCI1_HCCPARAMS_ASYNCHRONOUS_SCHEDULE_PARK_CAPABILITY_MASK 0x00000004
#define BCHP_USB_EHCI1_HCCPARAMS_ASYNCHRONOUS_SCHEDULE_PARK_CAPABILITY_SHIFT 2
#define BCHP_USB_EHCI1_HCCPARAMS_ASYNCHRONOUS_SCHEDULE_PARK_CAPABILITY_DEFAULT 0x00000000

/* USB_EHCI1 :: HCCPARAMS :: PROGRAMMABLE_FRAME_LIST_FLAG [01:01] */
#define BCHP_USB_EHCI1_HCCPARAMS_PROGRAMMABLE_FRAME_LIST_FLAG_MASK 0x00000002
#define BCHP_USB_EHCI1_HCCPARAMS_PROGRAMMABLE_FRAME_LIST_FLAG_SHIFT 1
#define BCHP_USB_EHCI1_HCCPARAMS_PROGRAMMABLE_FRAME_LIST_FLAG_DEFAULT 0x00000000

/* USB_EHCI1 :: HCCPARAMS :: SIXTY_FOUR_BIT_ADDRESSING_CAPABILITY [00:00] */
#define BCHP_USB_EHCI1_HCCPARAMS_SIXTY_FOUR_BIT_ADDRESSING_CAPABILITY_MASK 0x00000001
#define BCHP_USB_EHCI1_HCCPARAMS_SIXTY_FOUR_BIT_ADDRESSING_CAPABILITY_SHIFT 0
#define BCHP_USB_EHCI1_HCCPARAMS_SIXTY_FOUR_BIT_ADDRESSING_CAPABILITY_DEFAULT 0x00000000

/***************************************************************************
 *USBCMD - USB Command Register
 ***************************************************************************/
/* USB_EHCI1 :: USBCMD :: reserved3 [31:24] */
#define BCHP_USB_EHCI1_USBCMD_reserved3_MASK                       0xff000000
#define BCHP_USB_EHCI1_USBCMD_reserved3_SHIFT                      24

/* USB_EHCI1 :: USBCMD :: INTERRUPT_THRESHOLD_CONTROL [23:16] */
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_THRESHOLD_CONTROL_MASK     0x00ff0000
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_THRESHOLD_CONTROL_SHIFT    16
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_THRESHOLD_CONTROL_DEFAULT  0x00000008

/* USB_EHCI1 :: USBCMD :: reserved2 [15:12] */
#define BCHP_USB_EHCI1_USBCMD_reserved2_MASK                       0x0000f000
#define BCHP_USB_EHCI1_USBCMD_reserved2_SHIFT                      12

/* USB_EHCI1 :: USBCMD :: ASYNCHRONOUS_SCHEDULE_PARK_MODE_ENABLE [11:11] */
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_ENABLE_MASK 0x00000800
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_ENABLE_SHIFT 11
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_ENABLE_DEFAULT 0x00000000

/* USB_EHCI1 :: USBCMD :: reserved1 [10:10] */
#define BCHP_USB_EHCI1_USBCMD_reserved1_MASK                       0x00000400
#define BCHP_USB_EHCI1_USBCMD_reserved1_SHIFT                      10

/* USB_EHCI1 :: USBCMD :: ASYNCHRONOUS_SCHEDULE_PARK_MODE_COUNT [09:08] */
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_COUNT_MASK 0x00000300
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_COUNT_SHIFT 8
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_PARK_MODE_COUNT_DEFAULT 0x00000000

/* USB_EHCI1 :: USBCMD :: LIGHT_HOST_CONTROLLER_RESET [07:07] */
#define BCHP_USB_EHCI1_USBCMD_LIGHT_HOST_CONTROLLER_RESET_MASK     0x00000080
#define BCHP_USB_EHCI1_USBCMD_LIGHT_HOST_CONTROLLER_RESET_SHIFT    7
#define BCHP_USB_EHCI1_USBCMD_LIGHT_HOST_CONTROLLER_RESET_DEFAULT  0x00000000

/* USB_EHCI1 :: USBCMD :: INTERRUPT_ON_ASYNC_ADVANCE_DOORBELL [06:06] */
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_ON_ASYNC_ADVANCE_DOORBELL_MASK 0x00000040
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_ON_ASYNC_ADVANCE_DOORBELL_SHIFT 6
#define BCHP_USB_EHCI1_USBCMD_INTERRUPT_ON_ASYNC_ADVANCE_DOORBELL_DEFAULT 0x00000000

/* USB_EHCI1 :: USBCMD :: ASYNCHRONOUS_SCHEDULE_ENABLE [05:05] */
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_ENABLE_MASK    0x00000020
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_ENABLE_SHIFT   5
#define BCHP_USB_EHCI1_USBCMD_ASYNCHRONOUS_SCHEDULE_ENABLE_DEFAULT 0x00000000

/* USB_EHCI1 :: USBCMD :: PERIODIC_SCHEDULE_ENABLE [04:04] */
#define BCHP_USB_EHCI1_USBCMD_PERIODIC_SCHEDULE_ENABLE_MASK        0x00000010
#define BCHP_USB_EHCI1_USBCMD_PERIODIC_SCHEDULE_ENABLE_SHIFT       4
#define BCHP_USB_EHCI1_USBCMD_PERIODIC_SCHEDULE_ENABLE_DEFAULT     0x00000000

/* USB_EHCI1 :: USBCMD :: FRAME_LIST_SIZE [03:02] */
#define BCHP_USB_EHCI1_USBCMD_FRAME_LIST_SIZE_MASK                 0x0000000c
#define BCHP_USB_EHCI1_USBCMD_FRAME_LIST_SIZE_SHIFT                2
#define BCHP_USB_EHCI1_USBCMD_FRAME_LIST_SIZE_DEFAULT              0x00000000

/* USB_EHCI1 :: USBCMD :: HCRESET [01:01] */
#define BCHP_USB_EHCI1_USBCMD_HCRESET_MASK                         0x00000002
#define BCHP_USB_EHCI1_USBCMD_HCRESET_SHIFT                        1
#define BCHP_USB_EHCI1_USBCMD_HCRESET_DEFAULT                      0x00000000

/* USB_EHCI1 :: USBCMD :: RUN_STOP [00:00] */
#define BCHP_USB_EHCI1_USBCMD_RUN_STOP_MASK                        0x00000001
#define BCHP_USB_EHCI1_USBCMD_RUN_STOP_SHIFT                       0
#define BCHP_USB_EHCI1_USBCMD_RUN_STOP_DEFAULT                     0x00000000

/***************************************************************************
 *USBSTS - USB Status  Register
 ***************************************************************************/
/* USB_EHCI1 :: USBSTS :: reserved0 [31:16] */
#define BCHP_USB_EHCI1_USBSTS_reserved0_MASK                       0xffff0000
#define BCHP_USB_EHCI1_USBSTS_reserved0_SHIFT                      16

/* USB_EHCI1 :: USBSTS :: ASYNCHRONOUS_SCHEDULE_STATUS [15:15] */
#define BCHP_USB_EHCI1_USBSTS_ASYNCHRONOUS_SCHEDULE_STATUS_MASK    0x00008000
#define BCHP_USB_EHCI1_USBSTS_ASYNCHRONOUS_SCHEDULE_STATUS_SHIFT   15
#define BCHP_USB_EHCI1_USBSTS_ASYNCHRONOUS_SCHEDULE_STATUS_DEFAULT 0x00000000

/* USB_EHCI1 :: USBSTS :: PERIODIC_SCHEDULE_STATUS [14:14] */
#define BCHP_USB_EHCI1_USBSTS_PERIODIC_SCHEDULE_STATUS_MASK        0x00004000
#define BCHP_USB_EHCI1_USBSTS_PERIODIC_SCHEDULE_STATUS_SHIFT       14
#define BCHP_USB_EHCI1_USBSTS_PERIODIC_SCHEDULE_STATUS_DEFAULT     0x00000000

/* USB_EHCI1 :: USBSTS :: RECLAMATION [13:13] */
#define BCHP_USB_EHCI1_USBSTS_RECLAMATION_MASK                     0x00002000
#define BCHP_USB_EHCI1_USBSTS_RECLAMATION_SHIFT                    13
#define BCHP_USB_EHCI1_USBSTS_RECLAMATION_DEFAULT                  0x00000000

/* USB_EHCI1 :: USBSTS :: HCHALTED [12:12] */
#define BCHP_USB_EHCI1_USBSTS_HCHALTED_MASK                        0x00001000
#define BCHP_USB_EHCI1_USBSTS_HCHALTED_SHIFT                       12
#define BCHP_USB_EHCI1_USBSTS_HCHALTED_DEFAULT                     0x00000001

/* USB_EHCI1 :: USBSTS :: reserved1 [11:06] */
#define BCHP_USB_EHCI1_USBSTS_reserved1_MASK                       0x00000fc0
#define BCHP_USB_EHCI1_USBSTS_reserved1_SHIFT                      6

/* USB_EHCI1 :: USBSTS :: INTERRUPT_ON_ASYNC_ADVANCE [05:05] */
#define BCHP_USB_EHCI1_USBSTS_INTERRUPT_ON_ASYNC_ADVANCE_MASK      0x00000020
#define BCHP_USB_EHCI1_USBSTS_INTERRUPT_ON_ASYNC_ADVANCE_SHIFT     5
#define BCHP_USB_EHCI1_USBSTS_INTERRUPT_ON_ASYNC_ADVANCE_DEFAULT   0x00000000

/* USB_EHCI1 :: USBSTS :: HOST_SYSTEM_ERROR [04:04] */
#define BCHP_USB_EHCI1_USBSTS_HOST_SYSTEM_ERROR_MASK               0x00000010
#define BCHP_USB_EHCI1_USBSTS_HOST_SYSTEM_ERROR_SHIFT              4
#define BCHP_USB_EHCI1_USBSTS_HOST_SYSTEM_ERROR_DEFAULT            0x00000000

/* USB_EHCI1 :: USBSTS :: FRAME_LIST_ROLLOVER [03:03] */
#define BCHP_USB_EHCI1_USBSTS_FRAME_LIST_ROLLOVER_MASK             0x00000008
#define BCHP_USB_EHCI1_USBSTS_FRAME_LIST_ROLLOVER_SHIFT            3
#define BCHP_USB_EHCI1_USBSTS_FRAME_LIST_ROLLOVER_DEFAULT          0x00000000

/* USB_EHCI1 :: USBSTS :: PORT_CHANGE_DETECT [02:02] */
#define BCHP_USB_EHCI1_USBSTS_PORT_CHANGE_DETECT_MASK              0x00000004
#define BCHP_USB_EHCI1_USBSTS_PORT_CHANGE_DETECT_SHIFT             2
#define BCHP_USB_EHCI1_USBSTS_PORT_CHANGE_DETECT_DEFAULT           0x00000000

/* USB_EHCI1 :: USBSTS :: USBERRINT [01:01] */
#define BCHP_USB_EHCI1_USBSTS_USBERRINT_MASK                       0x00000002
#define BCHP_USB_EHCI1_USBSTS_USBERRINT_SHIFT                      1
#define BCHP_USB_EHCI1_USBSTS_USBERRINT_DEFAULT                    0x00000000

/* USB_EHCI1 :: USBSTS :: USBINT [00:00] */
#define BCHP_USB_EHCI1_USBSTS_USBINT_MASK                          0x00000001
#define BCHP_USB_EHCI1_USBSTS_USBINT_SHIFT                         0
#define BCHP_USB_EHCI1_USBSTS_USBINT_DEFAULT                       0x00000000

/***************************************************************************
 *USBINTR - USB Interrupt Enable Register
 ***************************************************************************/
/* USB_EHCI1 :: USBINTR :: reserved0 [31:06] */
#define BCHP_USB_EHCI1_USBINTR_reserved0_MASK                      0xffffffc0
#define BCHP_USB_EHCI1_USBINTR_reserved0_SHIFT                     6

/* USB_EHCI1 :: USBINTR :: INTERRUPT_ON_ASYNC_ADVANCE_ENABLE [05:05] */
#define BCHP_USB_EHCI1_USBINTR_INTERRUPT_ON_ASYNC_ADVANCE_ENABLE_MASK 0x00000020
#define BCHP_USB_EHCI1_USBINTR_INTERRUPT_ON_ASYNC_ADVANCE_ENABLE_SHIFT 5
#define BCHP_USB_EHCI1_USBINTR_INTERRUPT_ON_ASYNC_ADVANCE_ENABLE_DEFAULT 0x00000000

/* USB_EHCI1 :: USBINTR :: HOST_SYSTEM_ERROR_ENABLE [04:04] */
#define BCHP_USB_EHCI1_USBINTR_HOST_SYSTEM_ERROR_ENABLE_MASK       0x00000010
#define BCHP_USB_EHCI1_USBINTR_HOST_SYSTEM_ERROR_ENABLE_SHIFT      4
#define BCHP_USB_EHCI1_USBINTR_HOST_SYSTEM_ERROR_ENABLE_DEFAULT    0x00000000

/* USB_EHCI1 :: USBINTR :: FRAME_LIST_ROLLOVER_ENABLE [03:03] */
#define BCHP_USB_EHCI1_USBINTR_FRAME_LIST_ROLLOVER_ENABLE_MASK     0x00000008
#define BCHP_USB_EHCI1_USBINTR_FRAME_LIST_ROLLOVER_ENABLE_SHIFT    3
#define BCHP_USB_EHCI1_USBINTR_FRAME_LIST_ROLLOVER_ENABLE_DEFAULT  0x00000000

/* USB_EHCI1 :: USBINTR :: PORT_CHANGE_INTERRUPT_ENABLE [02:02] */
#define BCHP_USB_EHCI1_USBINTR_PORT_CHANGE_INTERRUPT_ENABLE_MASK   0x00000004
#define BCHP_USB_EHCI1_USBINTR_PORT_CHANGE_INTERRUPT_ENABLE_SHIFT  2
#define BCHP_USB_EHCI1_USBINTR_PORT_CHANGE_INTERRUPT_ENABLE_DEFAULT 0x00000000

/* USB_EHCI1 :: USBINTR :: USB_ERROR_INTERRUPT_ENABLE [01:01] */
#define BCHP_USB_EHCI1_USBINTR_USB_ERROR_INTERRUPT_ENABLE_MASK     0x00000002
#define BCHP_USB_EHCI1_USBINTR_USB_ERROR_INTERRUPT_ENABLE_SHIFT    1
#define BCHP_USB_EHCI1_USBINTR_USB_ERROR_INTERRUPT_ENABLE_DEFAULT  0x00000000

/* USB_EHCI1 :: USBINTR :: USB_INTERRUPT_ENABLE [00:00] */
#define BCHP_USB_EHCI1_USBINTR_USB_INTERRUPT_ENABLE_MASK           0x00000001
#define BCHP_USB_EHCI1_USBINTR_USB_INTERRUPT_ENABLE_SHIFT          0
#define BCHP_USB_EHCI1_USBINTR_USB_INTERRUPT_ENABLE_DEFAULT        0x00000000

/***************************************************************************
 *FRINDEX - USB Frame Index Register
 ***************************************************************************/
/* USB_EHCI1 :: FRINDEX :: reserved0 [31:14] */
#define BCHP_USB_EHCI1_FRINDEX_reserved0_MASK                      0xffffc000
#define BCHP_USB_EHCI1_FRINDEX_reserved0_SHIFT                     14

/* USB_EHCI1 :: FRINDEX :: FRAME_INDEX [13:00] */
#define BCHP_USB_EHCI1_FRINDEX_FRAME_INDEX_MASK                    0x00003fff
#define BCHP_USB_EHCI1_FRINDEX_FRAME_INDEX_SHIFT                   0
#define BCHP_USB_EHCI1_FRINDEX_FRAME_INDEX_DEFAULT                 0x00000000

/***************************************************************************
 *CTRLDSSEGMENT - Control Data Structure Segment Register
 ***************************************************************************/
/* USB_EHCI1 :: CTRLDSSEGMENT :: CTRLDSSEGMENT [31:00] */
#define BCHP_USB_EHCI1_CTRLDSSEGMENT_CTRLDSSEGMENT_MASK            0xffffffff
#define BCHP_USB_EHCI1_CTRLDSSEGMENT_CTRLDSSEGMENT_SHIFT           0
#define BCHP_USB_EHCI1_CTRLDSSEGMENT_CTRLDSSEGMENT_DEFAULT         0x00000000

/***************************************************************************
 *PERIODICLISTBASE - Periodic Frame List Base Address Register
 ***************************************************************************/
/* USB_EHCI1 :: PERIODICLISTBASE :: BASE_ADDRESS_LOW [31:12] */
#define BCHP_USB_EHCI1_PERIODICLISTBASE_BASE_ADDRESS_LOW_MASK      0xfffff000
#define BCHP_USB_EHCI1_PERIODICLISTBASE_BASE_ADDRESS_LOW_SHIFT     12
#define BCHP_USB_EHCI1_PERIODICLISTBASE_BASE_ADDRESS_LOW_DEFAULT   0x00000000

/* USB_EHCI1 :: PERIODICLISTBASE :: reserved0 [11:00] */
#define BCHP_USB_EHCI1_PERIODICLISTBASE_reserved0_MASK             0x00000fff
#define BCHP_USB_EHCI1_PERIODICLISTBASE_reserved0_SHIFT            0

/***************************************************************************
 *ASYNCLISTADDR - Asynchronous List Address
 ***************************************************************************/
/* USB_EHCI1 :: ASYNCLISTADDR :: LPL [31:05] */
#define BCHP_USB_EHCI1_ASYNCLISTADDR_LPL_MASK                      0xffffffe0
#define BCHP_USB_EHCI1_ASYNCLISTADDR_LPL_SHIFT                     5
#define BCHP_USB_EHCI1_ASYNCLISTADDR_LPL_DEFAULT                   0x00000000

/* USB_EHCI1 :: ASYNCLISTADDR :: reserved0 [04:00] */
#define BCHP_USB_EHCI1_ASYNCLISTADDR_reserved0_MASK                0x0000001f
#define BCHP_USB_EHCI1_ASYNCLISTADDR_reserved0_SHIFT               0

/***************************************************************************
 *CONFIGFLAG - Configured Flag Register
 ***************************************************************************/
/* USB_EHCI1 :: CONFIGFLAG :: reserved0 [31:01] */
#define BCHP_USB_EHCI1_CONFIGFLAG_reserved0_MASK                   0xfffffffe
#define BCHP_USB_EHCI1_CONFIGFLAG_reserved0_SHIFT                  1

/* USB_EHCI1 :: CONFIGFLAG :: CONFIGURE_FLAG [00:00] */
#define BCHP_USB_EHCI1_CONFIGFLAG_CONFIGURE_FLAG_MASK              0x00000001
#define BCHP_USB_EHCI1_CONFIGFLAG_CONFIGURE_FLAG_SHIFT             0
#define BCHP_USB_EHCI1_CONFIGFLAG_CONFIGURE_FLAG_DEFAULT           0x00000000

/***************************************************************************
 *PORTSC_0 - Port Status/Control Register for Port 0
 ***************************************************************************/
/* USB_EHCI1 :: PORTSC_0 :: reserved0 [31:23] */
#define BCHP_USB_EHCI1_PORTSC_0_reserved0_MASK                     0xff800000
#define BCHP_USB_EHCI1_PORTSC_0_reserved0_SHIFT                    23

/* USB_EHCI1 :: PORTSC_0 :: WKOC_E [22:22] */
#define BCHP_USB_EHCI1_PORTSC_0_WKOC_E_MASK                        0x00400000
#define BCHP_USB_EHCI1_PORTSC_0_WKOC_E_SHIFT                       22
#define BCHP_USB_EHCI1_PORTSC_0_WKOC_E_DEFAULT                     0x00000000

/* USB_EHCI1 :: PORTSC_0 :: WKDSCNNT_E [21:21] */
#define BCHP_USB_EHCI1_PORTSC_0_WKDSCNNT_E_MASK                    0x00200000
#define BCHP_USB_EHCI1_PORTSC_0_WKDSCNNT_E_SHIFT                   21
#define BCHP_USB_EHCI1_PORTSC_0_WKDSCNNT_E_DEFAULT                 0x00000000

/* USB_EHCI1 :: PORTSC_0 :: WKCNNT_E [20:20] */
#define BCHP_USB_EHCI1_PORTSC_0_WKCNNT_E_MASK                      0x00100000
#define BCHP_USB_EHCI1_PORTSC_0_WKCNNT_E_SHIFT                     20
#define BCHP_USB_EHCI1_PORTSC_0_WKCNNT_E_DEFAULT                   0x00000000

/* USB_EHCI1 :: PORTSC_0 :: PORT_TEST_CONTROL [19:16] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_TEST_CONTROL_MASK             0x000f0000
#define BCHP_USB_EHCI1_PORTSC_0_PORT_TEST_CONTROL_SHIFT            16
#define BCHP_USB_EHCI1_PORTSC_0_PORT_TEST_CONTROL_DEFAULT          0x00000000

/* USB_EHCI1 :: PORTSC_0 :: PORT_INDICATOR_CONTROL [15:14] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_INDICATOR_CONTROL_MASK        0x0000c000
#define BCHP_USB_EHCI1_PORTSC_0_PORT_INDICATOR_CONTROL_SHIFT       14
#define BCHP_USB_EHCI1_PORTSC_0_PORT_INDICATOR_CONTROL_DEFAULT     0x00000000

/* USB_EHCI1 :: PORTSC_0 :: PORT_OWNER [13:13] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_OWNER_MASK                    0x00002000
#define BCHP_USB_EHCI1_PORTSC_0_PORT_OWNER_SHIFT                   13
#define BCHP_USB_EHCI1_PORTSC_0_PORT_OWNER_DEFAULT                 0x00000001

/* USB_EHCI1 :: PORTSC_0 :: PP [12:12] */
#define BCHP_USB_EHCI1_PORTSC_0_PP_MASK                            0x00001000
#define BCHP_USB_EHCI1_PORTSC_0_PP_SHIFT                           12
#define BCHP_USB_EHCI1_PORTSC_0_PP_DEFAULT                         0x00000000

/* USB_EHCI1 :: PORTSC_0 :: LINE_STATUS [11:10] */
#define BCHP_USB_EHCI1_PORTSC_0_LINE_STATUS_MASK                   0x00000c00
#define BCHP_USB_EHCI1_PORTSC_0_LINE_STATUS_SHIFT                  10
#define BCHP_USB_EHCI1_PORTSC_0_LINE_STATUS_DEFAULT                0x00000000

/* USB_EHCI1 :: PORTSC_0 :: reserved1 [09:09] */
#define BCHP_USB_EHCI1_PORTSC_0_reserved1_MASK                     0x00000200
#define BCHP_USB_EHCI1_PORTSC_0_reserved1_SHIFT                    9

/* USB_EHCI1 :: PORTSC_0 :: PORT_RESET [08:08] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_RESET_MASK                    0x00000100
#define BCHP_USB_EHCI1_PORTSC_0_PORT_RESET_SHIFT                   8
#define BCHP_USB_EHCI1_PORTSC_0_PORT_RESET_DEFAULT                 0x00000000

/* USB_EHCI1 :: PORTSC_0 :: SUSPEND [07:07] */
#define BCHP_USB_EHCI1_PORTSC_0_SUSPEND_MASK                       0x00000080
#define BCHP_USB_EHCI1_PORTSC_0_SUSPEND_SHIFT                      7
#define BCHP_USB_EHCI1_PORTSC_0_SUSPEND_DEFAULT                    0x00000000

/* USB_EHCI1 :: PORTSC_0 :: FORCE_PORT_RESUME [06:06] */
#define BCHP_USB_EHCI1_PORTSC_0_FORCE_PORT_RESUME_MASK             0x00000040
#define BCHP_USB_EHCI1_PORTSC_0_FORCE_PORT_RESUME_SHIFT            6
#define BCHP_USB_EHCI1_PORTSC_0_FORCE_PORT_RESUME_DEFAULT          0x00000000

/* USB_EHCI1 :: PORTSC_0 :: OVER_CURRENT_CHANGE [05:05] */
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_CHANGE_MASK           0x00000020
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_CHANGE_SHIFT          5
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_CHANGE_DEFAULT        0x00000000

/* USB_EHCI1 :: PORTSC_0 :: OVER_CURRENT_ACTIVE [04:04] */
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_ACTIVE_MASK           0x00000010
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_ACTIVE_SHIFT          4
#define BCHP_USB_EHCI1_PORTSC_0_OVER_CURRENT_ACTIVE_DEFAULT        0x00000000

/* USB_EHCI1 :: PORTSC_0 :: PORT_ENABLE_DISABLE_CHANGE [03:03] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLE_DISABLE_CHANGE_MASK    0x00000008
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLE_DISABLE_CHANGE_SHIFT   3
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLE_DISABLE_CHANGE_DEFAULT 0x00000000

/* USB_EHCI1 :: PORTSC_0 :: PORT_ENABLED_DISABLED [02:02] */
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLED_DISABLED_MASK         0x00000004
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLED_DISABLED_SHIFT        2
#define BCHP_USB_EHCI1_PORTSC_0_PORT_ENABLED_DISABLED_DEFAULT      0x00000000

/* USB_EHCI1 :: PORTSC_0 :: CONNECT_STATUS_CHANGE [01:01] */
#define BCHP_USB_EHCI1_PORTSC_0_CONNECT_STATUS_CHANGE_MASK         0x00000002
#define BCHP_USB_EHCI1_PORTSC_0_CONNECT_STATUS_CHANGE_SHIFT        1
#define BCHP_USB_EHCI1_PORTSC_0_CONNECT_STATUS_CHANGE_DEFAULT      0x00000000

/* USB_EHCI1 :: PORTSC_0 :: CURRENT_CONNECT_STATUS [00:00] */
#define BCHP_USB_EHCI1_PORTSC_0_CURRENT_CONNECT_STATUS_MASK        0x00000001
#define BCHP_USB_EHCI1_PORTSC_0_CURRENT_CONNECT_STATUS_SHIFT       0
#define BCHP_USB_EHCI1_PORTSC_0_CURRENT_CONNECT_STATUS_DEFAULT     0x00000000

/***************************************************************************
 *INSNREG00 - Microframe Base Value Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG00 :: INSNREG00 [31:00] */
#define BCHP_USB_EHCI1_INSNREG00_INSNREG00_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG00_INSNREG00_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG00_INSNREG00_DEFAULT                 0x00000000

/***************************************************************************
 *INSNREG01 - Packet Buffer OUT/IN Threshold Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG01 :: INSNREG01 [31:00] */
#define BCHP_USB_EHCI1_INSNREG01_INSNREG01_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG01_INSNREG01_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG01_INSNREG01_DEFAULT                 0x00200020

/***************************************************************************
 *INSNREG02 - Packet Buffer Depth Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG02 :: INSNREG02 [31:00] */
#define BCHP_USB_EHCI1_INSNREG02_INSNREG02_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG02_INSNREG02_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG02_INSNREG02_DEFAULT                 0x00000080

/***************************************************************************
 *INSNREG03 - Break Memory Transfer Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG03 :: INSNREG03 [31:00] */
#define BCHP_USB_EHCI1_INSNREG03_INSNREG03_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG03_INSNREG03_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG03_INSNREG03_DEFAULT                 0x00000000

/***************************************************************************
 *INSNREG04 - Debug Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG04 :: INSNREG04 [31:00] */
#define BCHP_USB_EHCI1_INSNREG04_INSNREG04_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG04_INSNREG04_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG04_INSNREG04_DEFAULT                 0x00000000

/***************************************************************************
 *INSNREG05 - UTMI Control and Status Register
 ***************************************************************************/
/* USB_EHCI1 :: INSNREG05 :: INSNREG05 [31:00] */
#define BCHP_USB_EHCI1_INSNREG05_INSNREG05_MASK                    0xffffffff
#define BCHP_USB_EHCI1_INSNREG05_INSNREG05_SHIFT                   0
#define BCHP_USB_EHCI1_INSNREG05_INSNREG05_DEFAULT                 0x00001000

#endif /* #ifndef BCHP_USB_EHCI1_H__ */

/* End of File */
