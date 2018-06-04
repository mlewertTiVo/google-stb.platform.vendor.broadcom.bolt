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
 * Date:           Generated on               Thu Nov 23 01:10:03 2017
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

#ifndef BCHP_SATA_AHCI_GHC_H__
#define BCHP_SATA_AHCI_GHC_H__

/***************************************************************************
 *SATA_AHCI_GHC - AHCI Global Control Registers
 ***************************************************************************/
#define BCHP_SATA_AHCI_GHC_HBA_CAP               0x0020b12000 /* [RO][32] Basic HBA Capabilities Register */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control    0x0020b12004 /* [RW][32] Global HBA Control Register */
#define BCHP_SATA_AHCI_GHC_Interrupt_Status      0x0020b12008 /* [RW][32] Interrupt Status Register */
#define BCHP_SATA_AHCI_GHC_Ports_Implemented     0x0020b1200c /* [RO][32] Ports Implemented Register */
#define BCHP_SATA_AHCI_GHC_AHCI_Version          0x0020b12010 /* [RO][32] AHCI Version Register */
#define BCHP_SATA_AHCI_GHC_CCC_CTL               0x0020b12014 /* [RW][32] Command Completion Coalescing Control Register */
#define BCHP_SATA_AHCI_GHC_CCC_PORTS             0x0020b12018 /* [RW][32] Command Completion Coalescing Ports Register */
#define BCHP_SATA_AHCI_GHC_EM_LOC_RES            0x0020b1201c /* [RO][32] Reserved register */
#define BCHP_SATA_AHCI_GHC_EM_CTL_RES            0x0020b12020 /* [RO][32] Reserved register */
#define BCHP_SATA_AHCI_GHC_HBA_CAP2              0x0020b12024 /* [RO][32] HBA Capabilities Extended Register */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status 0x0020b12028 /* [RW][32] BIOS/OS Handoff Control and Status Register */

/***************************************************************************
 *HBA_CAP - Basic HBA Capabilities Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: HBA_CAP :: Support_64bit_Addressing [31:31] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_64bit_Addressing_MASK   0x80000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_64bit_Addressing_SHIFT  31
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_64bit_Addressing_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Native_Command_Queuing [30:30] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Native_Command_Queuing_MASK 0x40000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Native_Command_Queuing_SHIFT 30
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Native_Command_Queuing_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Support_SNotification [29:29] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_SNotification_MASK      0x20000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_SNotification_SHIFT     29
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_SNotification_DEFAULT   0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Mechanical_Presence_Switch [28:28] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Mechanical_Presence_Switch_MASK 0x10000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Mechanical_Presence_Switch_SHIFT 28
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Mechanical_Presence_Switch_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Staggered_Spinup [27:27] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Staggered_Spinup_MASK   0x08000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Staggered_Spinup_SHIFT  27
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Staggered_Spinup_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Aggressive_Link_Power_Management [26:26] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Aggressive_Link_Power_Management_MASK 0x04000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Aggressive_Link_Power_Management_SHIFT 26
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Aggressive_Link_Power_Management_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Activity_LED [25:25] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Activity_LED_MASK       0x02000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Activity_LED_SHIFT      25
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Activity_LED_DEFAULT    0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Command_List_Override [24:24] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_List_Override_MASK 0x01000000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_List_Override_SHIFT 24
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_List_Override_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Interface_Speed_Support [23:20] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Interface_Speed_Support_MASK    0x00f00000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Interface_Speed_Support_SHIFT   20
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Interface_Speed_Support_DEFAULT 0x00000003

/* SATA_AHCI_GHC :: HBA_CAP :: reserved0 [19:19] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_reserved0_MASK                  0x00080000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_reserved0_SHIFT                 19

/* SATA_AHCI_GHC :: HBA_CAP :: Support_AHCI_Mode_Only [18:18] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_AHCI_Mode_Only_MASK     0x00040000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_AHCI_Mode_Only_SHIFT    18
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_AHCI_Mode_Only_DEFAULT  0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Port_Multiplier [17:17] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Port_Multiplier_MASK    0x00020000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Port_Multiplier_SHIFT   17
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Port_Multiplier_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Support_FIS_based_Switching [16:16] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_FIS_based_Switching_MASK 0x00010000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_FIS_based_Switching_SHIFT 16
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_FIS_based_Switching_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: PIO_Multiple_DRQ_Block [15:15] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_PIO_Multiple_DRQ_Block_MASK     0x00008000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_PIO_Multiple_DRQ_Block_SHIFT    15
#define BCHP_SATA_AHCI_GHC_HBA_CAP_PIO_Multiple_DRQ_Block_DEFAULT  0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Slumber_State_Capable [14:14] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Slumber_State_Capable_MASK      0x00004000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Slumber_State_Capable_SHIFT     14
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Slumber_State_Capable_DEFAULT   0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Partial_State_Capable [13:13] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Partial_State_Capable_MASK      0x00002000
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Partial_State_Capable_SHIFT     13
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Partial_State_Capable_DEFAULT   0x00000001

/* SATA_AHCI_GHC :: HBA_CAP :: Number_of_Command_Slots [12:08] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Command_Slots_MASK    0x00001f00
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Command_Slots_SHIFT   8
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Command_Slots_DEFAULT 0x0000001f

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Command_Completion_Coalescing [07:07] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_Completion_Coalescing_MASK 0x00000080
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_Completion_Coalescing_SHIFT 7
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Command_Completion_Coalescing_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_Enclosure_Management [06:06] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Enclosure_Management_MASK 0x00000040
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Enclosure_Management_SHIFT 6
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_Enclosure_Management_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Support_External_SATA [05:05] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_External_SATA_MASK      0x00000020
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_External_SATA_SHIFT     5
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Support_External_SATA_DEFAULT   0x00000000

/* SATA_AHCI_GHC :: HBA_CAP :: Number_of_Ports [04:00] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Ports_MASK            0x0000001f
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Ports_SHIFT           0
#define BCHP_SATA_AHCI_GHC_HBA_CAP_Number_of_Ports_DEFAULT         0x00000000

/***************************************************************************
 *Global_HBA_Control - Global HBA Control Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: Global_HBA_Control :: AHCI_Enable [31:31] */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_AHCI_Enable_MASK     0x80000000
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_AHCI_Enable_SHIFT    31
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_AHCI_Enable_DEFAULT  0x00000000

/* SATA_AHCI_GHC :: Global_HBA_Control :: reserved0 [30:03] */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_reserved0_MASK       0x7ffffff8
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_reserved0_SHIFT      3

/* SATA_AHCI_GHC :: Global_HBA_Control :: MSI_Revert_to_Single_Message [02:02] */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_MSI_Revert_to_Single_Message_MASK 0x00000004
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_MSI_Revert_to_Single_Message_SHIFT 2
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_MSI_Revert_to_Single_Message_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: Global_HBA_Control :: Interrupt_Enable [01:01] */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_Interrupt_Enable_MASK 0x00000002
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_Interrupt_Enable_SHIFT 1
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_Interrupt_Enable_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: Global_HBA_Control :: HBA_Reset [00:00] */
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_HBA_Reset_MASK       0x00000001
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_HBA_Reset_SHIFT      0
#define BCHP_SATA_AHCI_GHC_Global_HBA_Control_HBA_Reset_DEFAULT    0x00000000

/***************************************************************************
 *Interrupt_Status - Interrupt Status Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: Interrupt_Status :: Interrupt_Pending_Status [31:00] */
#define BCHP_SATA_AHCI_GHC_Interrupt_Status_Interrupt_Pending_Status_MASK 0xffffffff
#define BCHP_SATA_AHCI_GHC_Interrupt_Status_Interrupt_Pending_Status_SHIFT 0
#define BCHP_SATA_AHCI_GHC_Interrupt_Status_Interrupt_Pending_Status_DEFAULT 0x00000000

/***************************************************************************
 *Ports_Implemented - Ports Implemented Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: Ports_Implemented :: Ports_Implement [31:00] */
#define BCHP_SATA_AHCI_GHC_Ports_Implemented_Ports_Implement_MASK  0xffffffff
#define BCHP_SATA_AHCI_GHC_Ports_Implemented_Ports_Implement_SHIFT 0
#define BCHP_SATA_AHCI_GHC_Ports_Implemented_Ports_Implement_DEFAULT 0x00000001

/***************************************************************************
 *AHCI_Version - AHCI Version Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: AHCI_Version :: Major_Version_Number [31:16] */
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Major_Version_Number_MASK  0xffff0000
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Major_Version_Number_SHIFT 16
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Major_Version_Number_DEFAULT 0x00000001

/* SATA_AHCI_GHC :: AHCI_Version :: Minor_Version_Number [15:00] */
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Minor_Version_Number_MASK  0x0000ffff
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Minor_Version_Number_SHIFT 0
#define BCHP_SATA_AHCI_GHC_AHCI_Version_Minor_Version_Number_DEFAULT 0x00000300

/***************************************************************************
 *CCC_CTL - Command Completion Coalescing Control Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: CCC_CTL :: Timeout_Value [31:16] */
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Timeout_Value_MASK              0xffff0000
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Timeout_Value_SHIFT             16
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Timeout_Value_DEFAULT           0x00000001

/* SATA_AHCI_GHC :: CCC_CTL :: Command_Completions [15:08] */
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Command_Completions_MASK        0x0000ff00
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Command_Completions_SHIFT       8
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Command_Completions_DEFAULT     0x00000001

/* SATA_AHCI_GHC :: CCC_CTL :: Interrupt [07:03] */
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Interrupt_MASK                  0x000000f8
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Interrupt_SHIFT                 3
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Interrupt_DEFAULT               0x00000000

/* SATA_AHCI_GHC :: CCC_CTL :: reserved0 [02:01] */
#define BCHP_SATA_AHCI_GHC_CCC_CTL_reserved0_MASK                  0x00000006
#define BCHP_SATA_AHCI_GHC_CCC_CTL_reserved0_SHIFT                 1

/* SATA_AHCI_GHC :: CCC_CTL :: Enable [00:00] */
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Enable_MASK                     0x00000001
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Enable_SHIFT                    0
#define BCHP_SATA_AHCI_GHC_CCC_CTL_Enable_DEFAULT                  0x00000000

/***************************************************************************
 *CCC_PORTS - Command Completion Coalescing Ports Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: CCC_PORTS :: Ports [31:00] */
#define BCHP_SATA_AHCI_GHC_CCC_PORTS_Ports_MASK                    0xffffffff
#define BCHP_SATA_AHCI_GHC_CCC_PORTS_Ports_SHIFT                   0
#define BCHP_SATA_AHCI_GHC_CCC_PORTS_Ports_DEFAULT                 0x00000000

/***************************************************************************
 *EM_LOC_RES - Reserved register
 ***************************************************************************/
/* SATA_AHCI_GHC :: EM_LOC_RES :: field_reserved [31:00] */
#define BCHP_SATA_AHCI_GHC_EM_LOC_RES_field_reserved_MASK          0xffffffff
#define BCHP_SATA_AHCI_GHC_EM_LOC_RES_field_reserved_SHIFT         0

/***************************************************************************
 *EM_CTL_RES - Reserved register
 ***************************************************************************/
/* SATA_AHCI_GHC :: EM_CTL_RES :: field_reserved [31:00] */
#define BCHP_SATA_AHCI_GHC_EM_CTL_RES_field_reserved_MASK          0xffffffff
#define BCHP_SATA_AHCI_GHC_EM_CTL_RES_field_reserved_SHIFT         0

/***************************************************************************
 *HBA_CAP2 - HBA Capabilities Extended Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: HBA_CAP2 :: reserved0 [31:03] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_reserved0_MASK                 0xfffffff8
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_reserved0_SHIFT                3

/* SATA_AHCI_GHC :: HBA_CAP2 :: Automatic_Partial_to_Slumber_Transitions [02:02] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_Automatic_Partial_to_Slumber_Transitions_MASK 0x00000004
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_Automatic_Partial_to_Slumber_Transitions_SHIFT 2
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_Automatic_Partial_to_Slumber_Transitions_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: HBA_CAP2 :: NVMHCI_Present [01:01] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_NVMHCI_Present_MASK            0x00000002
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_NVMHCI_Present_SHIFT           1
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_NVMHCI_Present_DEFAULT         0x00000000

/* SATA_AHCI_GHC :: HBA_CAP2 :: BIOS_OS_Handoff [00:00] */
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_BIOS_OS_Handoff_MASK           0x00000001
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_BIOS_OS_Handoff_SHIFT          0
#define BCHP_SATA_AHCI_GHC_HBA_CAP2_BIOS_OS_Handoff_DEFAULT        0x00000000

/***************************************************************************
 *BO_Handoff_Control_and_Status - BIOS/OS Handoff Control and Status Register
 ***************************************************************************/
/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: reserved0 [31:05] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_reserved0_MASK 0xffffffe0
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_reserved0_SHIFT 5

/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: BIOS_Busy [04:04] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Busy_MASK 0x00000010
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Busy_SHIFT 4
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Busy_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: OS_Ownership_Change [03:03] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Ownership_Change_MASK 0x00000008
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Ownership_Change_SHIFT 3
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Ownership_Change_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: SMI_on_OS_Ownership_Change_Enable [02:02] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_SMI_on_OS_Ownership_Change_Enable_MASK 0x00000004
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_SMI_on_OS_Ownership_Change_Enable_SHIFT 2
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_SMI_on_OS_Ownership_Change_Enable_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: OS_Owned_Semaphore [01:01] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Owned_Semaphore_MASK 0x00000002
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Owned_Semaphore_SHIFT 1
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_OS_Owned_Semaphore_DEFAULT 0x00000000

/* SATA_AHCI_GHC :: BO_Handoff_Control_and_Status :: BIOS_Owned_Semaphore [00:00] */
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Owned_Semaphore_MASK 0x00000001
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Owned_Semaphore_SHIFT 0
#define BCHP_SATA_AHCI_GHC_BO_Handoff_Control_and_Status_BIOS_Owned_Semaphore_DEFAULT 0x00000000

#endif /* #ifndef BCHP_SATA_AHCI_GHC_H__ */

/* End of File */
