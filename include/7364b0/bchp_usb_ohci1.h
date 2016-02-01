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
 * Date:           Generated on              Wed Apr 23 03:08:23 2014
 *                 Full Compile MD5 Checksum c01f6759a6963808817ded11e11ea0e4
 *                   (minus title and desc)  
 *                 MD5 Checksum              4581f9d1823df7707bae90236036749d
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

#ifndef BCHP_USB_OHCI1_H__
#define BCHP_USB_OHCI1_H__

/***************************************************************************
 *USB_OHCI1 - USB OHCI 1 Control Registers
 ***************************************************************************/
#define BCHP_USB_OHCI1_HcRevision                0x00480600 /* Host Controller Revision Register */
#define BCHP_USB_OHCI1_HcControl                 0x00480604 /* Host Controller Control Register */
#define BCHP_USB_OHCI1_HcCommandStatus           0x00480608 /* Host Controller Command Status Register */
#define BCHP_USB_OHCI1_HcInterruptStatus         0x0048060c /* Host Controller Interrupt Status Register */
#define BCHP_USB_OHCI1_HcInterruptEnable         0x00480610 /* Host Controller Interrupt Enable Register */
#define BCHP_USB_OHCI1_HcInterruptDisable        0x00480614 /* Host Controller Interrupt Disable Register */
#define BCHP_USB_OHCI1_HcHCCA                    0x00480618 /* Host Controller Communication Area Register */
#define BCHP_USB_OHCI1_HcPeriodCurrentED         0x0048061c /* Current Isochronous or Interrupt Endpoint Descriptor Register */
#define BCHP_USB_OHCI1_HcControlHeadED           0x00480620 /* First Endpoint Descriptor of the Control List */
#define BCHP_USB_OHCI1_HcControlCurrentED        0x00480624 /* Current Endpoint Descriptor of the Control List */
#define BCHP_USB_OHCI1_HcBulkHeadED              0x00480628 /* First Endpoint Descriptor of the Bulk List */
#define BCHP_USB_OHCI1_HcBulkCurrentED           0x0048062c /* Current Endpoint Descriptor of the Bulk List */
#define BCHP_USB_OHCI1_HcDoneHead                0x00480630 /* Last Completed Transfer Descriptor Added to the Done Queue */
#define BCHP_USB_OHCI1_HcFmInterval              0x00480634 /* Frame Bit Time Interval Register */
#define BCHP_USB_OHCI1_HcFmRemaining             0x00480638 /* Bit Time Remaining in the Current Frame */
#define BCHP_USB_OHCI1_HcFmNumber                0x0048063c /* Frame Number Register */
#define BCHP_USB_OHCI1_HcPeriodicStart           0x00480640 /* Register to Start Processing the Periodic List */
#define BCHP_USB_OHCI1_HcLSThreshold             0x00480644 /* LS Packet Threshold Register */
#define BCHP_USB_OHCI1_HcRhDescriptorA           0x00480648 /* Root Hub Descriptor A Register */
#define BCHP_USB_OHCI1_HcRhDescriptorB           0x0048064c /* Root Hub Descriptor B Register */
#define BCHP_USB_OHCI1_HcRhStatus                0x00480650 /* Root Hub Status Register */
#define BCHP_USB_OHCI1_HcRhPortStatus1           0x00480654 /* Root Hub Port Status Register for Port 1 */

/***************************************************************************
 *HcRevision - Host Controller Revision Register
 ***************************************************************************/
/* USB_OHCI1 :: HcRevision :: REV [31:00] */
#define BCHP_USB_OHCI1_HcRevision_REV_MASK                         0xffffffff
#define BCHP_USB_OHCI1_HcRevision_REV_SHIFT                        0
#define BCHP_USB_OHCI1_HcRevision_REV_DEFAULT                      0x00000010

/***************************************************************************
 *HcControl - Host Controller Control Register
 ***************************************************************************/
/* USB_OHCI1 :: HcControl :: reserved0 [31:11] */
#define BCHP_USB_OHCI1_HcControl_reserved0_MASK                    0xfffff800
#define BCHP_USB_OHCI1_HcControl_reserved0_SHIFT                   11

/* USB_OHCI1 :: HcControl :: RWE [10:10] */
#define BCHP_USB_OHCI1_HcControl_RWE_MASK                          0x00000400
#define BCHP_USB_OHCI1_HcControl_RWE_SHIFT                         10
#define BCHP_USB_OHCI1_HcControl_RWE_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcControl :: RWC [09:09] */
#define BCHP_USB_OHCI1_HcControl_RWC_MASK                          0x00000200
#define BCHP_USB_OHCI1_HcControl_RWC_SHIFT                         9
#define BCHP_USB_OHCI1_HcControl_RWC_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcControl :: IR [08:08] */
#define BCHP_USB_OHCI1_HcControl_IR_MASK                           0x00000100
#define BCHP_USB_OHCI1_HcControl_IR_SHIFT                          8
#define BCHP_USB_OHCI1_HcControl_IR_DEFAULT                        0x00000000

/* USB_OHCI1 :: HcControl :: HCFS [07:06] */
#define BCHP_USB_OHCI1_HcControl_HCFS_MASK                         0x000000c0
#define BCHP_USB_OHCI1_HcControl_HCFS_SHIFT                        6
#define BCHP_USB_OHCI1_HcControl_HCFS_DEFAULT                      0x00000000

/* USB_OHCI1 :: HcControl :: BLE [05:05] */
#define BCHP_USB_OHCI1_HcControl_BLE_MASK                          0x00000020
#define BCHP_USB_OHCI1_HcControl_BLE_SHIFT                         5
#define BCHP_USB_OHCI1_HcControl_BLE_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcControl :: CLE [04:04] */
#define BCHP_USB_OHCI1_HcControl_CLE_MASK                          0x00000010
#define BCHP_USB_OHCI1_HcControl_CLE_SHIFT                         4
#define BCHP_USB_OHCI1_HcControl_CLE_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcControl :: IE [03:03] */
#define BCHP_USB_OHCI1_HcControl_IE_MASK                           0x00000008
#define BCHP_USB_OHCI1_HcControl_IE_SHIFT                          3
#define BCHP_USB_OHCI1_HcControl_IE_DEFAULT                        0x00000000

/* USB_OHCI1 :: HcControl :: PLE [02:02] */
#define BCHP_USB_OHCI1_HcControl_PLE_MASK                          0x00000004
#define BCHP_USB_OHCI1_HcControl_PLE_SHIFT                         2
#define BCHP_USB_OHCI1_HcControl_PLE_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcControl :: CBSR [01:00] */
#define BCHP_USB_OHCI1_HcControl_CBSR_MASK                         0x00000003
#define BCHP_USB_OHCI1_HcControl_CBSR_SHIFT                        0
#define BCHP_USB_OHCI1_HcControl_CBSR_DEFAULT                      0x00000000

/***************************************************************************
 *HcCommandStatus - Host Controller Command Status Register
 ***************************************************************************/
/* USB_OHCI1 :: HcCommandStatus :: reserved0 [31:18] */
#define BCHP_USB_OHCI1_HcCommandStatus_reserved0_MASK              0xfffc0000
#define BCHP_USB_OHCI1_HcCommandStatus_reserved0_SHIFT             18

/* USB_OHCI1 :: HcCommandStatus :: SOC [17:16] */
#define BCHP_USB_OHCI1_HcCommandStatus_SOC_MASK                    0x00030000
#define BCHP_USB_OHCI1_HcCommandStatus_SOC_SHIFT                   16
#define BCHP_USB_OHCI1_HcCommandStatus_SOC_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcCommandStatus :: reserved1 [15:04] */
#define BCHP_USB_OHCI1_HcCommandStatus_reserved1_MASK              0x0000fff0
#define BCHP_USB_OHCI1_HcCommandStatus_reserved1_SHIFT             4

/* USB_OHCI1 :: HcCommandStatus :: OCR [03:03] */
#define BCHP_USB_OHCI1_HcCommandStatus_OCR_MASK                    0x00000008
#define BCHP_USB_OHCI1_HcCommandStatus_OCR_SHIFT                   3
#define BCHP_USB_OHCI1_HcCommandStatus_OCR_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcCommandStatus :: BLF [02:02] */
#define BCHP_USB_OHCI1_HcCommandStatus_BLF_MASK                    0x00000004
#define BCHP_USB_OHCI1_HcCommandStatus_BLF_SHIFT                   2
#define BCHP_USB_OHCI1_HcCommandStatus_BLF_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcCommandStatus :: CLF [01:01] */
#define BCHP_USB_OHCI1_HcCommandStatus_CLF_MASK                    0x00000002
#define BCHP_USB_OHCI1_HcCommandStatus_CLF_SHIFT                   1
#define BCHP_USB_OHCI1_HcCommandStatus_CLF_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcCommandStatus :: HCR [00:00] */
#define BCHP_USB_OHCI1_HcCommandStatus_HCR_MASK                    0x00000001
#define BCHP_USB_OHCI1_HcCommandStatus_HCR_SHIFT                   0
#define BCHP_USB_OHCI1_HcCommandStatus_HCR_DEFAULT                 0x00000000

/***************************************************************************
 *HcInterruptStatus - Host Controller Interrupt Status Register
 ***************************************************************************/
/* USB_OHCI1 :: HcInterruptStatus :: reserved0 [31:31] */
#define BCHP_USB_OHCI1_HcInterruptStatus_reserved0_MASK            0x80000000
#define BCHP_USB_OHCI1_HcInterruptStatus_reserved0_SHIFT           31

/* USB_OHCI1 :: HcInterruptStatus :: OC [30:30] */
#define BCHP_USB_OHCI1_HcInterruptStatus_OC_MASK                   0x40000000
#define BCHP_USB_OHCI1_HcInterruptStatus_OC_SHIFT                  30
#define BCHP_USB_OHCI1_HcInterruptStatus_OC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: reserved1 [29:07] */
#define BCHP_USB_OHCI1_HcInterruptStatus_reserved1_MASK            0x3fffff80
#define BCHP_USB_OHCI1_HcInterruptStatus_reserved1_SHIFT           7

/* USB_OHCI1 :: HcInterruptStatus :: RHSC [06:06] */
#define BCHP_USB_OHCI1_HcInterruptStatus_RHSC_MASK                 0x00000040
#define BCHP_USB_OHCI1_HcInterruptStatus_RHSC_SHIFT                6
#define BCHP_USB_OHCI1_HcInterruptStatus_RHSC_DEFAULT              0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: FNO [05:05] */
#define BCHP_USB_OHCI1_HcInterruptStatus_FNO_MASK                  0x00000020
#define BCHP_USB_OHCI1_HcInterruptStatus_FNO_SHIFT                 5
#define BCHP_USB_OHCI1_HcInterruptStatus_FNO_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: UE [04:04] */
#define BCHP_USB_OHCI1_HcInterruptStatus_UE_MASK                   0x00000010
#define BCHP_USB_OHCI1_HcInterruptStatus_UE_SHIFT                  4
#define BCHP_USB_OHCI1_HcInterruptStatus_UE_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: RD [03:03] */
#define BCHP_USB_OHCI1_HcInterruptStatus_RD_MASK                   0x00000008
#define BCHP_USB_OHCI1_HcInterruptStatus_RD_SHIFT                  3
#define BCHP_USB_OHCI1_HcInterruptStatus_RD_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: SF [02:02] */
#define BCHP_USB_OHCI1_HcInterruptStatus_SF_MASK                   0x00000004
#define BCHP_USB_OHCI1_HcInterruptStatus_SF_SHIFT                  2
#define BCHP_USB_OHCI1_HcInterruptStatus_SF_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: WDH [01:01] */
#define BCHP_USB_OHCI1_HcInterruptStatus_WDH_MASK                  0x00000002
#define BCHP_USB_OHCI1_HcInterruptStatus_WDH_SHIFT                 1
#define BCHP_USB_OHCI1_HcInterruptStatus_WDH_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptStatus :: SO [00:00] */
#define BCHP_USB_OHCI1_HcInterruptStatus_SO_MASK                   0x00000001
#define BCHP_USB_OHCI1_HcInterruptStatus_SO_SHIFT                  0
#define BCHP_USB_OHCI1_HcInterruptStatus_SO_DEFAULT                0x00000000

/***************************************************************************
 *HcInterruptEnable - Host Controller Interrupt Enable Register
 ***************************************************************************/
/* USB_OHCI1 :: HcInterruptEnable :: MIE [31:31] */
#define BCHP_USB_OHCI1_HcInterruptEnable_MIE_MASK                  0x80000000
#define BCHP_USB_OHCI1_HcInterruptEnable_MIE_SHIFT                 31
#define BCHP_USB_OHCI1_HcInterruptEnable_MIE_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: OC [30:30] */
#define BCHP_USB_OHCI1_HcInterruptEnable_OC_MASK                   0x40000000
#define BCHP_USB_OHCI1_HcInterruptEnable_OC_SHIFT                  30
#define BCHP_USB_OHCI1_HcInterruptEnable_OC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: reserved0 [29:07] */
#define BCHP_USB_OHCI1_HcInterruptEnable_reserved0_MASK            0x3fffff80
#define BCHP_USB_OHCI1_HcInterruptEnable_reserved0_SHIFT           7

/* USB_OHCI1 :: HcInterruptEnable :: RHSC [06:06] */
#define BCHP_USB_OHCI1_HcInterruptEnable_RHSC_MASK                 0x00000040
#define BCHP_USB_OHCI1_HcInterruptEnable_RHSC_SHIFT                6
#define BCHP_USB_OHCI1_HcInterruptEnable_RHSC_DEFAULT              0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: FNO [05:05] */
#define BCHP_USB_OHCI1_HcInterruptEnable_FNO_MASK                  0x00000020
#define BCHP_USB_OHCI1_HcInterruptEnable_FNO_SHIFT                 5
#define BCHP_USB_OHCI1_HcInterruptEnable_FNO_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: UE [04:04] */
#define BCHP_USB_OHCI1_HcInterruptEnable_UE_MASK                   0x00000010
#define BCHP_USB_OHCI1_HcInterruptEnable_UE_SHIFT                  4
#define BCHP_USB_OHCI1_HcInterruptEnable_UE_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: RD [03:03] */
#define BCHP_USB_OHCI1_HcInterruptEnable_RD_MASK                   0x00000008
#define BCHP_USB_OHCI1_HcInterruptEnable_RD_SHIFT                  3
#define BCHP_USB_OHCI1_HcInterruptEnable_RD_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: SF [02:02] */
#define BCHP_USB_OHCI1_HcInterruptEnable_SF_MASK                   0x00000004
#define BCHP_USB_OHCI1_HcInterruptEnable_SF_SHIFT                  2
#define BCHP_USB_OHCI1_HcInterruptEnable_SF_DEFAULT                0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: WDH [01:01] */
#define BCHP_USB_OHCI1_HcInterruptEnable_WDH_MASK                  0x00000002
#define BCHP_USB_OHCI1_HcInterruptEnable_WDH_SHIFT                 1
#define BCHP_USB_OHCI1_HcInterruptEnable_WDH_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptEnable :: SO [00:00] */
#define BCHP_USB_OHCI1_HcInterruptEnable_SO_MASK                   0x00000001
#define BCHP_USB_OHCI1_HcInterruptEnable_SO_SHIFT                  0
#define BCHP_USB_OHCI1_HcInterruptEnable_SO_DEFAULT                0x00000000

/***************************************************************************
 *HcInterruptDisable - Host Controller Interrupt Disable Register
 ***************************************************************************/
/* USB_OHCI1 :: HcInterruptDisable :: MIE [31:31] */
#define BCHP_USB_OHCI1_HcInterruptDisable_MIE_MASK                 0x80000000
#define BCHP_USB_OHCI1_HcInterruptDisable_MIE_SHIFT                31
#define BCHP_USB_OHCI1_HcInterruptDisable_MIE_DEFAULT              0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: OC [30:30] */
#define BCHP_USB_OHCI1_HcInterruptDisable_OC_MASK                  0x40000000
#define BCHP_USB_OHCI1_HcInterruptDisable_OC_SHIFT                 30
#define BCHP_USB_OHCI1_HcInterruptDisable_OC_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: reserved0 [29:07] */
#define BCHP_USB_OHCI1_HcInterruptDisable_reserved0_MASK           0x3fffff80
#define BCHP_USB_OHCI1_HcInterruptDisable_reserved0_SHIFT          7

/* USB_OHCI1 :: HcInterruptDisable :: RHSC [06:06] */
#define BCHP_USB_OHCI1_HcInterruptDisable_RHSC_MASK                0x00000040
#define BCHP_USB_OHCI1_HcInterruptDisable_RHSC_SHIFT               6
#define BCHP_USB_OHCI1_HcInterruptDisable_RHSC_DEFAULT             0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: FNO [05:05] */
#define BCHP_USB_OHCI1_HcInterruptDisable_FNO_MASK                 0x00000020
#define BCHP_USB_OHCI1_HcInterruptDisable_FNO_SHIFT                5
#define BCHP_USB_OHCI1_HcInterruptDisable_FNO_DEFAULT              0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: UE [04:04] */
#define BCHP_USB_OHCI1_HcInterruptDisable_UE_MASK                  0x00000010
#define BCHP_USB_OHCI1_HcInterruptDisable_UE_SHIFT                 4
#define BCHP_USB_OHCI1_HcInterruptDisable_UE_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: RD [03:03] */
#define BCHP_USB_OHCI1_HcInterruptDisable_RD_MASK                  0x00000008
#define BCHP_USB_OHCI1_HcInterruptDisable_RD_SHIFT                 3
#define BCHP_USB_OHCI1_HcInterruptDisable_RD_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: SF [02:02] */
#define BCHP_USB_OHCI1_HcInterruptDisable_SF_MASK                  0x00000004
#define BCHP_USB_OHCI1_HcInterruptDisable_SF_SHIFT                 2
#define BCHP_USB_OHCI1_HcInterruptDisable_SF_DEFAULT               0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: WDH [01:01] */
#define BCHP_USB_OHCI1_HcInterruptDisable_WDH_MASK                 0x00000002
#define BCHP_USB_OHCI1_HcInterruptDisable_WDH_SHIFT                1
#define BCHP_USB_OHCI1_HcInterruptDisable_WDH_DEFAULT              0x00000000

/* USB_OHCI1 :: HcInterruptDisable :: SO [00:00] */
#define BCHP_USB_OHCI1_HcInterruptDisable_SO_MASK                  0x00000001
#define BCHP_USB_OHCI1_HcInterruptDisable_SO_SHIFT                 0
#define BCHP_USB_OHCI1_HcInterruptDisable_SO_DEFAULT               0x00000000

/***************************************************************************
 *HcHCCA - Host Controller Communication Area Register
 ***************************************************************************/
/* USB_OHCI1 :: HcHCCA :: HCCA [31:08] */
#define BCHP_USB_OHCI1_HcHCCA_HCCA_MASK                            0xffffff00
#define BCHP_USB_OHCI1_HcHCCA_HCCA_SHIFT                           8
#define BCHP_USB_OHCI1_HcHCCA_HCCA_DEFAULT                         0x00000000

/* USB_OHCI1 :: HcHCCA :: reserved0 [07:00] */
#define BCHP_USB_OHCI1_HcHCCA_reserved0_MASK                       0x000000ff
#define BCHP_USB_OHCI1_HcHCCA_reserved0_SHIFT                      0

/***************************************************************************
 *HcPeriodCurrentED - Current Isochronous or Interrupt Endpoint Descriptor Register
 ***************************************************************************/
/* USB_OHCI1 :: HcPeriodCurrentED :: PCED [31:04] */
#define BCHP_USB_OHCI1_HcPeriodCurrentED_PCED_MASK                 0xfffffff0
#define BCHP_USB_OHCI1_HcPeriodCurrentED_PCED_SHIFT                4
#define BCHP_USB_OHCI1_HcPeriodCurrentED_PCED_DEFAULT              0x00000000

/* USB_OHCI1 :: HcPeriodCurrentED :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcPeriodCurrentED_reserved0_MASK            0x0000000f
#define BCHP_USB_OHCI1_HcPeriodCurrentED_reserved0_SHIFT           0

/***************************************************************************
 *HcControlHeadED - First Endpoint Descriptor of the Control List
 ***************************************************************************/
/* USB_OHCI1 :: HcControlHeadED :: CHED [31:04] */
#define BCHP_USB_OHCI1_HcControlHeadED_CHED_MASK                   0xfffffff0
#define BCHP_USB_OHCI1_HcControlHeadED_CHED_SHIFT                  4
#define BCHP_USB_OHCI1_HcControlHeadED_CHED_DEFAULT                0x00000000

/* USB_OHCI1 :: HcControlHeadED :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcControlHeadED_reserved0_MASK              0x0000000f
#define BCHP_USB_OHCI1_HcControlHeadED_reserved0_SHIFT             0

/***************************************************************************
 *HcControlCurrentED - Current Endpoint Descriptor of the Control List
 ***************************************************************************/
/* USB_OHCI1 :: HcControlCurrentED :: CCED [31:04] */
#define BCHP_USB_OHCI1_HcControlCurrentED_CCED_MASK                0xfffffff0
#define BCHP_USB_OHCI1_HcControlCurrentED_CCED_SHIFT               4
#define BCHP_USB_OHCI1_HcControlCurrentED_CCED_DEFAULT             0x00000000

/* USB_OHCI1 :: HcControlCurrentED :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcControlCurrentED_reserved0_MASK           0x0000000f
#define BCHP_USB_OHCI1_HcControlCurrentED_reserved0_SHIFT          0

/***************************************************************************
 *HcBulkHeadED - First Endpoint Descriptor of the Bulk List
 ***************************************************************************/
/* USB_OHCI1 :: HcBulkHeadED :: BHED [31:04] */
#define BCHP_USB_OHCI1_HcBulkHeadED_BHED_MASK                      0xfffffff0
#define BCHP_USB_OHCI1_HcBulkHeadED_BHED_SHIFT                     4
#define BCHP_USB_OHCI1_HcBulkHeadED_BHED_DEFAULT                   0x00000000

/* USB_OHCI1 :: HcBulkHeadED :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcBulkHeadED_reserved0_MASK                 0x0000000f
#define BCHP_USB_OHCI1_HcBulkHeadED_reserved0_SHIFT                0

/***************************************************************************
 *HcBulkCurrentED - Current Endpoint Descriptor of the Bulk List
 ***************************************************************************/
/* USB_OHCI1 :: HcBulkCurrentED :: BCED [31:04] */
#define BCHP_USB_OHCI1_HcBulkCurrentED_BCED_MASK                   0xfffffff0
#define BCHP_USB_OHCI1_HcBulkCurrentED_BCED_SHIFT                  4
#define BCHP_USB_OHCI1_HcBulkCurrentED_BCED_DEFAULT                0x00000000

/* USB_OHCI1 :: HcBulkCurrentED :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcBulkCurrentED_reserved0_MASK              0x0000000f
#define BCHP_USB_OHCI1_HcBulkCurrentED_reserved0_SHIFT             0

/***************************************************************************
 *HcDoneHead - Last Completed Transfer Descriptor Added to the Done Queue
 ***************************************************************************/
/* USB_OHCI1 :: HcDoneHead :: DH [31:04] */
#define BCHP_USB_OHCI1_HcDoneHead_DH_MASK                          0xfffffff0
#define BCHP_USB_OHCI1_HcDoneHead_DH_SHIFT                         4
#define BCHP_USB_OHCI1_HcDoneHead_DH_DEFAULT                       0x00000000

/* USB_OHCI1 :: HcDoneHead :: reserved0 [03:00] */
#define BCHP_USB_OHCI1_HcDoneHead_reserved0_MASK                   0x0000000f
#define BCHP_USB_OHCI1_HcDoneHead_reserved0_SHIFT                  0

/***************************************************************************
 *HcFmInterval - Frame Bit Time Interval Register
 ***************************************************************************/
/* USB_OHCI1 :: HcFmInterval :: FIT [31:31] */
#define BCHP_USB_OHCI1_HcFmInterval_FIT_MASK                       0x80000000
#define BCHP_USB_OHCI1_HcFmInterval_FIT_SHIFT                      31
#define BCHP_USB_OHCI1_HcFmInterval_FIT_DEFAULT                    0x00000000

/* USB_OHCI1 :: HcFmInterval :: FSMPS [30:16] */
#define BCHP_USB_OHCI1_HcFmInterval_FSMPS_MASK                     0x7fff0000
#define BCHP_USB_OHCI1_HcFmInterval_FSMPS_SHIFT                    16
#define BCHP_USB_OHCI1_HcFmInterval_FSMPS_DEFAULT                  0x00000000

/* USB_OHCI1 :: HcFmInterval :: reserved0 [15:14] */
#define BCHP_USB_OHCI1_HcFmInterval_reserved0_MASK                 0x0000c000
#define BCHP_USB_OHCI1_HcFmInterval_reserved0_SHIFT                14

/* USB_OHCI1 :: HcFmInterval :: FI [13:00] */
#define BCHP_USB_OHCI1_HcFmInterval_FI_MASK                        0x00003fff
#define BCHP_USB_OHCI1_HcFmInterval_FI_SHIFT                       0
#define BCHP_USB_OHCI1_HcFmInterval_FI_DEFAULT                     0x00002edf

/***************************************************************************
 *HcFmRemaining - Bit Time Remaining in the Current Frame
 ***************************************************************************/
/* USB_OHCI1 :: HcFmRemaining :: FRT [31:31] */
#define BCHP_USB_OHCI1_HcFmRemaining_FRT_MASK                      0x80000000
#define BCHP_USB_OHCI1_HcFmRemaining_FRT_SHIFT                     31
#define BCHP_USB_OHCI1_HcFmRemaining_FRT_DEFAULT                   0x00000000

/* USB_OHCI1 :: HcFmRemaining :: reserved0 [30:14] */
#define BCHP_USB_OHCI1_HcFmRemaining_reserved0_MASK                0x7fffc000
#define BCHP_USB_OHCI1_HcFmRemaining_reserved0_SHIFT               14

/* USB_OHCI1 :: HcFmRemaining :: FR [13:00] */
#define BCHP_USB_OHCI1_HcFmRemaining_FR_MASK                       0x00003fff
#define BCHP_USB_OHCI1_HcFmRemaining_FR_SHIFT                      0
#define BCHP_USB_OHCI1_HcFmRemaining_FR_DEFAULT                    0x00000000

/***************************************************************************
 *HcFmNumber - Frame Number Register
 ***************************************************************************/
/* USB_OHCI1 :: HcFmNumber :: reserved0 [31:16] */
#define BCHP_USB_OHCI1_HcFmNumber_reserved0_MASK                   0xffff0000
#define BCHP_USB_OHCI1_HcFmNumber_reserved0_SHIFT                  16

/* USB_OHCI1 :: HcFmNumber :: FN [15:00] */
#define BCHP_USB_OHCI1_HcFmNumber_FN_MASK                          0x0000ffff
#define BCHP_USB_OHCI1_HcFmNumber_FN_SHIFT                         0
#define BCHP_USB_OHCI1_HcFmNumber_FN_DEFAULT                       0x00000000

/***************************************************************************
 *HcPeriodicStart - Register to Start Processing the Periodic List
 ***************************************************************************/
/* USB_OHCI1 :: HcPeriodicStart :: reserved0 [31:14] */
#define BCHP_USB_OHCI1_HcPeriodicStart_reserved0_MASK              0xffffc000
#define BCHP_USB_OHCI1_HcPeriodicStart_reserved0_SHIFT             14

/* USB_OHCI1 :: HcPeriodicStart :: PS [13:00] */
#define BCHP_USB_OHCI1_HcPeriodicStart_PS_MASK                     0x00003fff
#define BCHP_USB_OHCI1_HcPeriodicStart_PS_SHIFT                    0
#define BCHP_USB_OHCI1_HcPeriodicStart_PS_DEFAULT                  0x00000000

/***************************************************************************
 *HcLSThreshold - LS Packet Threshold Register
 ***************************************************************************/
/* USB_OHCI1 :: HcLSThreshold :: reserved0 [31:12] */
#define BCHP_USB_OHCI1_HcLSThreshold_reserved0_MASK                0xfffff000
#define BCHP_USB_OHCI1_HcLSThreshold_reserved0_SHIFT               12

/* USB_OHCI1 :: HcLSThreshold :: LST [11:00] */
#define BCHP_USB_OHCI1_HcLSThreshold_LST_MASK                      0x00000fff
#define BCHP_USB_OHCI1_HcLSThreshold_LST_SHIFT                     0
#define BCHP_USB_OHCI1_HcLSThreshold_LST_DEFAULT                   0x00000628

/***************************************************************************
 *HcRhDescriptorA - Root Hub Descriptor A Register
 ***************************************************************************/
/* USB_OHCI1 :: HcRhDescriptorA :: POTPGT [31:24] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_POTPGT_MASK                 0xff000000
#define BCHP_USB_OHCI1_HcRhDescriptorA_POTPGT_SHIFT                24
#define BCHP_USB_OHCI1_HcRhDescriptorA_POTPGT_DEFAULT              0x00000002

/* USB_OHCI1 :: HcRhDescriptorA :: reserved0 [23:13] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_reserved0_MASK              0x00ffe000
#define BCHP_USB_OHCI1_HcRhDescriptorA_reserved0_SHIFT             13

/* USB_OHCI1 :: HcRhDescriptorA :: NOCP [12:12] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_NOCP_MASK                   0x00001000
#define BCHP_USB_OHCI1_HcRhDescriptorA_NOCP_SHIFT                  12
#define BCHP_USB_OHCI1_HcRhDescriptorA_NOCP_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhDescriptorA :: OCPM [11:11] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_OCPM_MASK                   0x00000800
#define BCHP_USB_OHCI1_HcRhDescriptorA_OCPM_SHIFT                  11
#define BCHP_USB_OHCI1_HcRhDescriptorA_OCPM_DEFAULT                0x00000001

/* USB_OHCI1 :: HcRhDescriptorA :: DT [10:10] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_DT_MASK                     0x00000400
#define BCHP_USB_OHCI1_HcRhDescriptorA_DT_SHIFT                    10
#define BCHP_USB_OHCI1_HcRhDescriptorA_DT_DEFAULT                  0x00000000

/* USB_OHCI1 :: HcRhDescriptorA :: NPS [09:09] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_NPS_MASK                    0x00000200
#define BCHP_USB_OHCI1_HcRhDescriptorA_NPS_SHIFT                   9
#define BCHP_USB_OHCI1_HcRhDescriptorA_NPS_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhDescriptorA :: PSM [08:08] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_PSM_MASK                    0x00000100
#define BCHP_USB_OHCI1_HcRhDescriptorA_PSM_SHIFT                   8
#define BCHP_USB_OHCI1_HcRhDescriptorA_PSM_DEFAULT                 0x00000001

/* USB_OHCI1 :: HcRhDescriptorA :: NDP [07:00] */
#define BCHP_USB_OHCI1_HcRhDescriptorA_NDP_MASK                    0x000000ff
#define BCHP_USB_OHCI1_HcRhDescriptorA_NDP_SHIFT                   0
#define BCHP_USB_OHCI1_HcRhDescriptorA_NDP_DEFAULT                 0x00000001

/***************************************************************************
 *HcRhDescriptorB - Root Hub Descriptor B Register
 ***************************************************************************/
/* USB_OHCI1 :: HcRhDescriptorB :: PPCM [31:16] */
#define BCHP_USB_OHCI1_HcRhDescriptorB_PPCM_MASK                   0xffff0000
#define BCHP_USB_OHCI1_HcRhDescriptorB_PPCM_SHIFT                  16
#define BCHP_USB_OHCI1_HcRhDescriptorB_PPCM_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhDescriptorB :: DR [15:00] */
#define BCHP_USB_OHCI1_HcRhDescriptorB_DR_MASK                     0x0000ffff
#define BCHP_USB_OHCI1_HcRhDescriptorB_DR_SHIFT                    0
#define BCHP_USB_OHCI1_HcRhDescriptorB_DR_DEFAULT                  0x00000000

/***************************************************************************
 *HcRhStatus - Root Hub Status Register
 ***************************************************************************/
/* USB_OHCI1 :: HcRhStatus :: CRWE [31:31] */
#define BCHP_USB_OHCI1_HcRhStatus_CRWE_MASK                        0x80000000
#define BCHP_USB_OHCI1_HcRhStatus_CRWE_SHIFT                       31
#define BCHP_USB_OHCI1_HcRhStatus_CRWE_DEFAULT                     0x00000000

/* USB_OHCI1 :: HcRhStatus :: reserved0 [30:18] */
#define BCHP_USB_OHCI1_HcRhStatus_reserved0_MASK                   0x7ffc0000
#define BCHP_USB_OHCI1_HcRhStatus_reserved0_SHIFT                  18

/* USB_OHCI1 :: HcRhStatus :: OCIC [17:17] */
#define BCHP_USB_OHCI1_HcRhStatus_OCIC_MASK                        0x00020000
#define BCHP_USB_OHCI1_HcRhStatus_OCIC_SHIFT                       17
#define BCHP_USB_OHCI1_HcRhStatus_OCIC_DEFAULT                     0x00000000

/* USB_OHCI1 :: HcRhStatus :: LPSC [16:16] */
#define BCHP_USB_OHCI1_HcRhStatus_LPSC_MASK                        0x00010000
#define BCHP_USB_OHCI1_HcRhStatus_LPSC_SHIFT                       16
#define BCHP_USB_OHCI1_HcRhStatus_LPSC_DEFAULT                     0x00000000

/* USB_OHCI1 :: HcRhStatus :: DRWE [15:15] */
#define BCHP_USB_OHCI1_HcRhStatus_DRWE_MASK                        0x00008000
#define BCHP_USB_OHCI1_HcRhStatus_DRWE_SHIFT                       15
#define BCHP_USB_OHCI1_HcRhStatus_DRWE_DEFAULT                     0x00000000

/* USB_OHCI1 :: HcRhStatus :: reserved1 [14:02] */
#define BCHP_USB_OHCI1_HcRhStatus_reserved1_MASK                   0x00007ffc
#define BCHP_USB_OHCI1_HcRhStatus_reserved1_SHIFT                  2

/* USB_OHCI1 :: HcRhStatus :: OCI [01:01] */
#define BCHP_USB_OHCI1_HcRhStatus_OCI_MASK                         0x00000002
#define BCHP_USB_OHCI1_HcRhStatus_OCI_SHIFT                        1
#define BCHP_USB_OHCI1_HcRhStatus_OCI_DEFAULT                      0x00000000

/* USB_OHCI1 :: HcRhStatus :: LPS [00:00] */
#define BCHP_USB_OHCI1_HcRhStatus_LPS_MASK                         0x00000001
#define BCHP_USB_OHCI1_HcRhStatus_LPS_SHIFT                        0
#define BCHP_USB_OHCI1_HcRhStatus_LPS_DEFAULT                      0x00000000

/***************************************************************************
 *HcRhPortStatus1 - Root Hub Port Status Register for Port 1
 ***************************************************************************/
/* USB_OHCI1 :: HcRhPortStatus1 :: reserved0 [31:21] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved0_MASK              0xffe00000
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved0_SHIFT             21

/* USB_OHCI1 :: HcRhPortStatus1 :: PRSC [20:20] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRSC_MASK                   0x00100000
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRSC_SHIFT                  20
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRSC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: OCIC [19:19] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_OCIC_MASK                   0x00080000
#define BCHP_USB_OHCI1_HcRhPortStatus1_OCIC_SHIFT                  19
#define BCHP_USB_OHCI1_HcRhPortStatus1_OCIC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: PSSC [18:18] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSSC_MASK                   0x00040000
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSSC_SHIFT                  18
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSSC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: PESC [17:17] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PESC_MASK                   0x00020000
#define BCHP_USB_OHCI1_HcRhPortStatus1_PESC_SHIFT                  17
#define BCHP_USB_OHCI1_HcRhPortStatus1_PESC_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: CSC [16:16] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_CSC_MASK                    0x00010000
#define BCHP_USB_OHCI1_HcRhPortStatus1_CSC_SHIFT                   16
#define BCHP_USB_OHCI1_HcRhPortStatus1_CSC_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: reserved1 [15:10] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved1_MASK              0x0000fc00
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved1_SHIFT             10

/* USB_OHCI1 :: HcRhPortStatus1 :: LSDA [09:09] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_LSDA_MASK                   0x00000200
#define BCHP_USB_OHCI1_HcRhPortStatus1_LSDA_SHIFT                  9
#define BCHP_USB_OHCI1_HcRhPortStatus1_LSDA_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: PPS [08:08] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PPS_MASK                    0x00000100
#define BCHP_USB_OHCI1_HcRhPortStatus1_PPS_SHIFT                   8
#define BCHP_USB_OHCI1_HcRhPortStatus1_PPS_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: reserved2 [07:05] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved2_MASK              0x000000e0
#define BCHP_USB_OHCI1_HcRhPortStatus1_reserved2_SHIFT             5

/* USB_OHCI1 :: HcRhPortStatus1 :: PRS [04:04] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRS_MASK                    0x00000010
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRS_SHIFT                   4
#define BCHP_USB_OHCI1_HcRhPortStatus1_PRS_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: POCI [03:03] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_POCI_MASK                   0x00000008
#define BCHP_USB_OHCI1_HcRhPortStatus1_POCI_SHIFT                  3
#define BCHP_USB_OHCI1_HcRhPortStatus1_POCI_DEFAULT                0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: PSS [02:02] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSS_MASK                    0x00000004
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSS_SHIFT                   2
#define BCHP_USB_OHCI1_HcRhPortStatus1_PSS_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: PES [01:01] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_PES_MASK                    0x00000002
#define BCHP_USB_OHCI1_HcRhPortStatus1_PES_SHIFT                   1
#define BCHP_USB_OHCI1_HcRhPortStatus1_PES_DEFAULT                 0x00000000

/* USB_OHCI1 :: HcRhPortStatus1 :: CCS [00:00] */
#define BCHP_USB_OHCI1_HcRhPortStatus1_CCS_MASK                    0x00000001
#define BCHP_USB_OHCI1_HcRhPortStatus1_CCS_SHIFT                   0
#define BCHP_USB_OHCI1_HcRhPortStatus1_CCS_DEFAULT                 0x00000000

#endif /* #ifndef BCHP_USB_OHCI1_H__ */

/* End of File */
