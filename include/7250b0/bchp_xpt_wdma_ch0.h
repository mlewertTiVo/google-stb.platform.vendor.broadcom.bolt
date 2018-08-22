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
 * Date:           Generated on              Sun Oct 19 03:09:57 2014
 *                 Full Compile MD5 Checksum 70e56d904a1a6d7fd3ed680066b764b4
 *                   (minus title and desc)  
 *                 MD5 Checksum              beea26d4e908bc85e703b35e2190ef3e
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

#ifndef BCHP_XPT_WDMA_CH0_H__
#define BCHP_XPT_WDMA_CH0_H__

/***************************************************************************
 *XPT_WDMA_CH0 - WDMA Channel 0 Configuration
 ***************************************************************************/
#define BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR        0x00a6a000 /* First Descriptor Address */
#define BCHP_XPT_WDMA_CH0_NEXT_DESC_ADDR         0x00a6a004 /* Next Descriptor Address */
#define BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS 0x00a6a008 /* Completed Descriptor Address */
#define BCHP_XPT_WDMA_CH0_BTP_PACKET_GROUP_ID    0x00a6a00c /* Packet Group ID reported per BTP command */
#define BCHP_XPT_WDMA_CH0_RUN_VERSION_CONFIG     0x00a6a010 /* RUN_VERSION configuration */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS       0x00a6a014 /* Overflow Reason */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT     0x00a6a018 /* Descriptor Memory Queue Control Structure */
#define BCHP_XPT_WDMA_CH0_DATA_CONTROL           0x00a6a01c /* Data Control */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_HI  0x00a6a080 /* DRAM Buffer Base Pointer - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR     0x00a6a084 /* DRAM Buffer Base Pointer - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_HI   0x00a6a088 /* DRAM Buffer End Pointer - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR      0x00a6a08c /* DRAM Buffer End Pointer - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_HI    0x00a6a090 /* DRAM Buffer Read Pointer - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR       0x00a6a094 /* DRAM Buffer Read Pointer - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_HI    0x00a6a098 /* DRAM Buffer Write Pointer - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR       0x00a6a09c /* DRAM Buffer Write Pointer - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_HI 0x00a6a0a0 /* DRAM Buffer Valid Pointer - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR    0x00a6a0a4 /* DRAM Buffer Valid Pointer - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_HI 0x00a6a0a8 /* DRAM Buffer Lower Threshold - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD 0x00a6a0ac /* DRAM Buffer Lower Threshold - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_HI 0x00a6a0b0 /* DRAM Buffer Upper Threshold - Upper bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD 0x00a6a0b4 /* DRAM Buffer Upper Threshold - Lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS       0x00a6a0b8 /* DRAM Buffer Status */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL      0x00a6a0bc /* DRAM Buffer Control */
#define BCHP_XPT_WDMA_CH0_DMQ_0_0                0x00a6a0c0 /* DMQ descriptor 0 - Write Address, Upper bits */
#define BCHP_XPT_WDMA_CH0_DMQ_0_1                0x00a6a0c4 /* DMQ descriptor 0 - Write Address, lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DMQ_0_2                0x00a6a0c8 /* DMQ descriptor 0 - Transfer Size */
#define BCHP_XPT_WDMA_CH0_DMQ_0_3                0x00a6a0cc /* DMQ descriptor 0 - Current Descriptor Address and Control */
#define BCHP_XPT_WDMA_CH0_DMQ_1_0                0x00a6a0d0 /* DMQ descriptor 1 - Write Address, Upper bits */
#define BCHP_XPT_WDMA_CH0_DMQ_1_1                0x00a6a0d4 /* DMQ descriptor 1 - Write Address, lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DMQ_1_2                0x00a6a0d8 /* DMQ descriptor 1 - Transfer Size */
#define BCHP_XPT_WDMA_CH0_DMQ_1_3                0x00a6a0dc /* DMQ descriptor 1 - Current Descriptor Address and Control */
#define BCHP_XPT_WDMA_CH0_DMQ_2_0                0x00a6a0e0 /* DMQ descriptor 2 - Write Address, Upper bits */
#define BCHP_XPT_WDMA_CH0_DMQ_2_1                0x00a6a0e4 /* DMQ descriptor 2 - Write Address, lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DMQ_2_2                0x00a6a0e8 /* DMQ descriptor 2 - Transfer Size */
#define BCHP_XPT_WDMA_CH0_DMQ_2_3                0x00a6a0ec /* DMQ descriptor 2 - Current Descriptor Address and Control */
#define BCHP_XPT_WDMA_CH0_DMQ_3_0                0x00a6a0f0 /* DMQ descriptor 3 - Write Address, Upper bits */
#define BCHP_XPT_WDMA_CH0_DMQ_3_1                0x00a6a0f4 /* DMQ descriptor 3 - Write Address, lower 32 bits */
#define BCHP_XPT_WDMA_CH0_DMQ_3_2                0x00a6a0f8 /* DMQ descriptor 3 - Transfer Size */
#define BCHP_XPT_WDMA_CH0_DMQ_3_3                0x00a6a0fc /* DMQ descriptor 3 - Current Descriptor Address and Control */

/***************************************************************************
 *FIRST_DESC_ADDR - First Descriptor Address
 ***************************************************************************/
/* XPT_WDMA_CH0 :: FIRST_DESC_ADDR :: FIRST_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR_FIRST_DESC_ADDRESS_MASK  0xfffffff0
#define BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR_FIRST_DESC_ADDRESS_SHIFT 4

/* XPT_WDMA_CH0 :: FIRST_DESC_ADDR :: reserved0 [03:00] */
#define BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR_reserved0_MASK           0x0000000f
#define BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR_reserved0_SHIFT          0

/***************************************************************************
 *NEXT_DESC_ADDR - Next Descriptor Address
 ***************************************************************************/
/* XPT_WDMA_CH0 :: NEXT_DESC_ADDR :: NEXT_DESC_ADDR [31:04] */
#define BCHP_XPT_WDMA_CH0_NEXT_DESC_ADDR_NEXT_DESC_ADDR_MASK       0xfffffff0
#define BCHP_XPT_WDMA_CH0_NEXT_DESC_ADDR_NEXT_DESC_ADDR_SHIFT      4

/* XPT_WDMA_CH0 :: NEXT_DESC_ADDR :: reserved0 [03:00] */
#define BCHP_XPT_WDMA_CH0_NEXT_DESC_ADDR_reserved0_MASK            0x0000000f
#define BCHP_XPT_WDMA_CH0_NEXT_DESC_ADDR_reserved0_SHIFT           0

/***************************************************************************
 *COMPLETED_DESC_ADDRESS - Completed Descriptor Address
 ***************************************************************************/
/* XPT_WDMA_CH0 :: COMPLETED_DESC_ADDRESS :: COMPLETED_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS_COMPLETED_DESC_ADDRESS_MASK 0xfffffff0
#define BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS_COMPLETED_DESC_ADDRESS_SHIFT 4

/* XPT_WDMA_CH0 :: COMPLETED_DESC_ADDRESS :: reserved0 [03:00] */
#define BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS_reserved0_MASK    0x0000000f
#define BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS_reserved0_SHIFT   0

/***************************************************************************
 *BTP_PACKET_GROUP_ID - Packet Group ID reported per BTP command
 ***************************************************************************/
/* XPT_WDMA_CH0 :: BTP_PACKET_GROUP_ID :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_BTP_PACKET_GROUP_ID_reserved0_MASK       0xffffff00
#define BCHP_XPT_WDMA_CH0_BTP_PACKET_GROUP_ID_reserved0_SHIFT      8

/* XPT_WDMA_CH0 :: BTP_PACKET_GROUP_ID :: BTP_PACKET_GROUP_ID [07:00] */
#define BCHP_XPT_WDMA_CH0_BTP_PACKET_GROUP_ID_BTP_PACKET_GROUP_ID_MASK 0x000000ff
#define BCHP_XPT_WDMA_CH0_BTP_PACKET_GROUP_ID_BTP_PACKET_GROUP_ID_SHIFT 0

/***************************************************************************
 *RUN_VERSION_CONFIG - RUN_VERSION configuration
 ***************************************************************************/
/* XPT_WDMA_CH0 :: RUN_VERSION_CONFIG :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CH0_RUN_VERSION_CONFIG_reserved0_MASK        0xffffffe0
#define BCHP_XPT_WDMA_CH0_RUN_VERSION_CONFIG_reserved0_SHIFT       5

/* XPT_WDMA_CH0 :: RUN_VERSION_CONFIG :: RUN_VERSION [04:00] */
#define BCHP_XPT_WDMA_CH0_RUN_VERSION_CONFIG_RUN_VERSION_MASK      0x0000001f
#define BCHP_XPT_WDMA_CH0_RUN_VERSION_CONFIG_RUN_VERSION_SHIFT     0

/***************************************************************************
 *OVERFLOW_REASONS - Overflow Reason
 ***************************************************************************/
/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: reserved0 [31:05] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_reserved0_MASK          0xffffffe0
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_reserved0_SHIFT         5

/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: PACKET_SYNC_ERROR [04:04] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_PACKET_SYNC_ERROR_MASK  0x00000010
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_PACKET_SYNC_ERROR_SHIFT 4

/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: RING_BUFFER_FULL [03:03] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_RING_BUFFER_FULL_MASK   0x00000008
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_RING_BUFFER_FULL_SHIFT  3

/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: RUN_NOT_SET [02:02] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_RUN_NOT_SET_MASK        0x00000004
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_RUN_NOT_SET_SHIFT       2

/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: SLEEP_NO_WAKE [01:01] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_SLEEP_NO_WAKE_MASK      0x00000002
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_SLEEP_NO_WAKE_SHIFT     1

/* XPT_WDMA_CH0 :: OVERFLOW_REASONS :: DATA_STALL_TIMEOUT [00:00] */
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_DATA_STALL_TIMEOUT_MASK 0x00000001
#define BCHP_XPT_WDMA_CH0_OVERFLOW_REASONS_DATA_STALL_TIMEOUT_SHIFT 0

/***************************************************************************
 *DMQ_CONTROL_STRUCT - Descriptor Memory Queue Control Structure
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_CONTROL_STRUCT :: reserved0 [31:06] */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_reserved0_MASK        0xffffffc0
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_reserved0_SHIFT       6

/* XPT_WDMA_CH0 :: DMQ_CONTROL_STRUCT :: FULL [05:05] */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_FULL_MASK             0x00000020
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_FULL_SHIFT            5

/* XPT_WDMA_CH0 :: DMQ_CONTROL_STRUCT :: EMPTY [04:04] */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_EMPTY_MASK            0x00000010
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_EMPTY_SHIFT           4

/* XPT_WDMA_CH0 :: DMQ_CONTROL_STRUCT :: WRITE_PTR [03:02] */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_WRITE_PTR_MASK        0x0000000c
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_WRITE_PTR_SHIFT       2

/* XPT_WDMA_CH0 :: DMQ_CONTROL_STRUCT :: READ_PTR [01:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_READ_PTR_MASK         0x00000003
#define BCHP_XPT_WDMA_CH0_DMQ_CONTROL_STRUCT_READ_PTR_SHIFT        0

/***************************************************************************
 *DATA_CONTROL - Data Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DATA_CONTROL :: reserved0 [31:01] */
#define BCHP_XPT_WDMA_CH0_DATA_CONTROL_reserved0_MASK              0xfffffffe
#define BCHP_XPT_WDMA_CH0_DATA_CONTROL_reserved0_SHIFT             1

/* XPT_WDMA_CH0 :: DATA_CONTROL :: INV_STRAP_ENDIAN_CTRL [00:00] */
#define BCHP_XPT_WDMA_CH0_DATA_CONTROL_INV_STRAP_ENDIAN_CTRL_MASK  0x00000001
#define BCHP_XPT_WDMA_CH0_DATA_CONTROL_INV_STRAP_ENDIAN_CTRL_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_BASE_PTR_HI - DRAM Buffer Base Pointer - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_BASE_PTR_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_HI_reserved0_MASK     0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_HI_reserved0_SHIFT    8

/* XPT_WDMA_CH0 :: DRAM_BUFF_BASE_PTR_HI :: BASE_PTR_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_HI_BASE_PTR_HI_MASK   0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_HI_BASE_PTR_HI_SHIFT  0

/***************************************************************************
 *DRAM_BUFF_BASE_PTR - DRAM Buffer Base Pointer - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_BASE_PTR :: BASE_PTR [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_BASE_PTR_MASK         0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_BASE_PTR_BASE_PTR_SHIFT        0

/***************************************************************************
 *DRAM_BUFF_END_PTR_HI - DRAM Buffer End Pointer - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_END_PTR_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_HI_reserved0_MASK      0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_HI_reserved0_SHIFT     8

/* XPT_WDMA_CH0 :: DRAM_BUFF_END_PTR_HI :: END_PTR_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_HI_END_PTR_HI_MASK     0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_HI_END_PTR_HI_SHIFT    0

/***************************************************************************
 *DRAM_BUFF_END_PTR - DRAM Buffer End Pointer - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_END_PTR :: END_PTR [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_END_PTR_MASK           0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_END_PTR_END_PTR_SHIFT          0

/***************************************************************************
 *DRAM_BUFF_RD_PTR_HI - DRAM Buffer Read Pointer - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_RD_PTR_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_HI_reserved0_MASK       0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_HI_reserved0_SHIFT      8

/* XPT_WDMA_CH0 :: DRAM_BUFF_RD_PTR_HI :: RD_PTR_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_HI_RD_PTR_HI_MASK       0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_HI_RD_PTR_HI_SHIFT      0

/***************************************************************************
 *DRAM_BUFF_RD_PTR - DRAM Buffer Read Pointer - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_RD_PTR :: RD_PTR [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_RD_PTR_MASK             0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_RD_PTR_RD_PTR_SHIFT            0

/***************************************************************************
 *DRAM_BUFF_WR_PTR_HI - DRAM Buffer Write Pointer - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_WR_PTR_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_HI_reserved0_MASK       0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_HI_reserved0_SHIFT      8

/* XPT_WDMA_CH0 :: DRAM_BUFF_WR_PTR_HI :: WR_PTR_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_HI_WR_PTR_HI_MASK       0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_HI_WR_PTR_HI_SHIFT      0

/***************************************************************************
 *DRAM_BUFF_WR_PTR - DRAM Buffer Write Pointer - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_WR_PTR :: WR_PTR [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_WR_PTR_MASK             0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_WR_PTR_WR_PTR_SHIFT            0

/***************************************************************************
 *DRAM_BUFF_VALID_PTR_HI - DRAM Buffer Valid Pointer - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_VALID_PTR_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_HI_reserved0_MASK    0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_HI_reserved0_SHIFT   8

/* XPT_WDMA_CH0 :: DRAM_BUFF_VALID_PTR_HI :: VALID_PTR_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_HI_VALID_PTR_HI_MASK 0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_HI_VALID_PTR_HI_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_VALID_PTR - DRAM Buffer Valid Pointer - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_VALID_PTR :: VALID_PTR_HI [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_VALID_PTR_HI_MASK    0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_VALID_PTR_VALID_PTR_HI_SHIFT   0

/***************************************************************************
 *DRAM_BUFF_LOWER_THRESHOLD_HI - DRAM Buffer Lower Threshold - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_LOWER_THRESHOLD_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_HI_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_HI_reserved0_SHIFT 8

/* XPT_WDMA_CH0 :: DRAM_BUFF_LOWER_THRESHOLD_HI :: LOWER_THRESHOLD_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_HI_LOWER_THRESHOLD_HI_MASK 0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_HI_LOWER_THRESHOLD_HI_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_LOWER_THRESHOLD - DRAM Buffer Lower Threshold - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_LOWER_THRESHOLD :: LOWER_THRESHOLD [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_LOWER_THRESHOLD_MASK 0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_LOWER_THRESHOLD_LOWER_THRESHOLD_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_UPPER_THRESHOLD_HI - DRAM Buffer Upper Threshold - Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_UPPER_THRESHOLD_HI :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_HI_reserved0_MASK 0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_HI_reserved0_SHIFT 8

/* XPT_WDMA_CH0 :: DRAM_BUFF_UPPER_THRESHOLD_HI :: UPPER_THRESHOLD_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_HI_UPPER_THRESHOLD_HI_MASK 0x000000ff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_HI_UPPER_THRESHOLD_HI_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_UPPER_THRESHOLD - DRAM Buffer Upper Threshold - Lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_UPPER_THRESHOLD :: LOWER_THRESHOLD [31:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_LOWER_THRESHOLD_MASK 0xffffffff
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_UPPER_THRESHOLD_LOWER_THRESHOLD_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_STATUS - DRAM Buffer Status
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_STATUS :: reserved0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_reserved0_MASK          0xffffff00
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_reserved0_SHIFT         8

/* XPT_WDMA_CH0 :: DRAM_BUFF_STATUS :: reserved_for_eco1 [07:02] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_reserved_for_eco1_MASK  0x000000fc
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_reserved_for_eco1_SHIFT 2

/* XPT_WDMA_CH0 :: DRAM_BUFF_STATUS :: RING_BUFFER_FULL [01:01] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_RING_BUFFER_FULL_MASK   0x00000002
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_RING_BUFFER_FULL_SHIFT  1

/* XPT_WDMA_CH0 :: DRAM_BUFF_STATUS :: RING_BUFFER_EMPTY [00:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_RING_BUFFER_EMPTY_MASK  0x00000001
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_STATUS_RING_BUFFER_EMPTY_SHIFT 0

/***************************************************************************
 *DRAM_BUFF_CONTROL - DRAM Buffer Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DRAM_BUFF_CONTROL :: CURRENT_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_CURRENT_DESC_ADDRESS_MASK 0xfffffff0
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_CURRENT_DESC_ADDRESS_SHIFT 4

/* XPT_WDMA_CH0 :: DRAM_BUFF_CONTROL :: DATA_UNIT_SIZE [03:02] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_DATA_UNIT_SIZE_MASK    0x0000000c
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_DATA_UNIT_SIZE_SHIFT   2

/* XPT_WDMA_CH0 :: DRAM_BUFF_CONTROL :: INT_EN [01:01] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_INT_EN_MASK            0x00000002
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_INT_EN_SHIFT           1

/* XPT_WDMA_CH0 :: DRAM_BUFF_CONTROL :: reserved_for_eco0 [00:00] */
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_reserved_for_eco0_MASK 0x00000001
#define BCHP_XPT_WDMA_CH0_DRAM_BUFF_CONTROL_reserved_for_eco0_SHIFT 0

/***************************************************************************
 *DMQ_0_0 - DMQ descriptor 0 - Write Address, Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_0_0 :: reserved_for_eco0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_0_reserved_for_eco0_MASK           0xffffff00
#define BCHP_XPT_WDMA_CH0_DMQ_0_0_reserved_for_eco0_SHIFT          8

/* XPT_WDMA_CH0 :: DMQ_0_0 :: WRITE_ADDRESS_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_0_WRITE_ADDRESS_HI_MASK            0x000000ff
#define BCHP_XPT_WDMA_CH0_DMQ_0_0_WRITE_ADDRESS_HI_SHIFT           0

/***************************************************************************
 *DMQ_0_1 - DMQ descriptor 0 - Write Address, lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_0_1 :: WRITE_ADDRESS [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_1_WRITE_ADDRESS_MASK               0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_0_1_WRITE_ADDRESS_SHIFT              0

/***************************************************************************
 *DMQ_0_2 - DMQ descriptor 0 - Transfer Size
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_0_2 :: TXFER_SIZE [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_2_TXFER_SIZE_MASK                  0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_0_2_TXFER_SIZE_SHIFT                 0

/***************************************************************************
 *DMQ_0_3 - DMQ descriptor 0 - Current Descriptor Address and Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_0_3 :: CURRENT_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_CURRENT_DESC_ADDRESS_MASK        0xfffffff0
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_CURRENT_DESC_ADDRESS_SHIFT       4

/* XPT_WDMA_CH0 :: DMQ_0_3 :: DATA_UNIT_SIZE [03:02] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_DATA_UNIT_SIZE_MASK              0x0000000c
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_DATA_UNIT_SIZE_SHIFT             2

/* XPT_WDMA_CH0 :: DMQ_0_3 :: INT_EN [01:01] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_INT_EN_MASK                      0x00000002
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_INT_EN_SHIFT                     1

/* XPT_WDMA_CH0 :: DMQ_0_3 :: LAST [00:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_LAST_MASK                        0x00000001
#define BCHP_XPT_WDMA_CH0_DMQ_0_3_LAST_SHIFT                       0

/***************************************************************************
 *DMQ_1_0 - DMQ descriptor 1 - Write Address, Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_1_0 :: reserved_for_eco0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_0_reserved_for_eco0_MASK           0xffffff00
#define BCHP_XPT_WDMA_CH0_DMQ_1_0_reserved_for_eco0_SHIFT          8

/* XPT_WDMA_CH0 :: DMQ_1_0 :: WRITE_ADDRESS_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_0_WRITE_ADDRESS_HI_MASK            0x000000ff
#define BCHP_XPT_WDMA_CH0_DMQ_1_0_WRITE_ADDRESS_HI_SHIFT           0

/***************************************************************************
 *DMQ_1_1 - DMQ descriptor 1 - Write Address, lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_1_1 :: WRITE_ADDRESS [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_1_WRITE_ADDRESS_MASK               0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_1_1_WRITE_ADDRESS_SHIFT              0

/***************************************************************************
 *DMQ_1_2 - DMQ descriptor 1 - Transfer Size
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_1_2 :: TXFER_SIZE [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_2_TXFER_SIZE_MASK                  0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_1_2_TXFER_SIZE_SHIFT                 0

/***************************************************************************
 *DMQ_1_3 - DMQ descriptor 1 - Current Descriptor Address and Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_1_3 :: CURRENT_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_CURRENT_DESC_ADDRESS_MASK        0xfffffff0
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_CURRENT_DESC_ADDRESS_SHIFT       4

/* XPT_WDMA_CH0 :: DMQ_1_3 :: DATA_UNIT_SIZE [03:02] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_DATA_UNIT_SIZE_MASK              0x0000000c
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_DATA_UNIT_SIZE_SHIFT             2

/* XPT_WDMA_CH0 :: DMQ_1_3 :: INT_EN [01:01] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_INT_EN_MASK                      0x00000002
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_INT_EN_SHIFT                     1

/* XPT_WDMA_CH0 :: DMQ_1_3 :: LAST [00:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_LAST_MASK                        0x00000001
#define BCHP_XPT_WDMA_CH0_DMQ_1_3_LAST_SHIFT                       0

/***************************************************************************
 *DMQ_2_0 - DMQ descriptor 2 - Write Address, Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_2_0 :: reserved_for_eco0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_0_reserved_for_eco0_MASK           0xffffff00
#define BCHP_XPT_WDMA_CH0_DMQ_2_0_reserved_for_eco0_SHIFT          8

/* XPT_WDMA_CH0 :: DMQ_2_0 :: WRITE_ADDRESS_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_0_WRITE_ADDRESS_HI_MASK            0x000000ff
#define BCHP_XPT_WDMA_CH0_DMQ_2_0_WRITE_ADDRESS_HI_SHIFT           0

/***************************************************************************
 *DMQ_2_1 - DMQ descriptor 2 - Write Address, lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_2_1 :: WRITE_ADDRESS [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_1_WRITE_ADDRESS_MASK               0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_2_1_WRITE_ADDRESS_SHIFT              0

/***************************************************************************
 *DMQ_2_2 - DMQ descriptor 2 - Transfer Size
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_2_2 :: TXFER_SIZE [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_2_TXFER_SIZE_MASK                  0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_2_2_TXFER_SIZE_SHIFT                 0

/***************************************************************************
 *DMQ_2_3 - DMQ descriptor 2 - Current Descriptor Address and Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_2_3 :: CURRENT_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_CURRENT_DESC_ADDRESS_MASK        0xfffffff0
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_CURRENT_DESC_ADDRESS_SHIFT       4

/* XPT_WDMA_CH0 :: DMQ_2_3 :: DATA_UNIT_SIZE [03:02] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_DATA_UNIT_SIZE_MASK              0x0000000c
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_DATA_UNIT_SIZE_SHIFT             2

/* XPT_WDMA_CH0 :: DMQ_2_3 :: INT_EN [01:01] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_INT_EN_MASK                      0x00000002
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_INT_EN_SHIFT                     1

/* XPT_WDMA_CH0 :: DMQ_2_3 :: LAST [00:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_LAST_MASK                        0x00000001
#define BCHP_XPT_WDMA_CH0_DMQ_2_3_LAST_SHIFT                       0

/***************************************************************************
 *DMQ_3_0 - DMQ descriptor 3 - Write Address, Upper bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_3_0 :: reserved_for_eco0 [31:08] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_0_reserved_for_eco0_MASK           0xffffff00
#define BCHP_XPT_WDMA_CH0_DMQ_3_0_reserved_for_eco0_SHIFT          8

/* XPT_WDMA_CH0 :: DMQ_3_0 :: WRITE_ADDRESS_HI [07:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_0_WRITE_ADDRESS_HI_MASK            0x000000ff
#define BCHP_XPT_WDMA_CH0_DMQ_3_0_WRITE_ADDRESS_HI_SHIFT           0

/***************************************************************************
 *DMQ_3_1 - DMQ descriptor 3 - Write Address, lower 32 bits
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_3_1 :: WRITE_ADDRESS [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_1_WRITE_ADDRESS_MASK               0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_3_1_WRITE_ADDRESS_SHIFT              0

/***************************************************************************
 *DMQ_3_2 - DMQ descriptor 3 - Transfer Size
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_3_2 :: TXFER_SIZE [31:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_2_TXFER_SIZE_MASK                  0xffffffff
#define BCHP_XPT_WDMA_CH0_DMQ_3_2_TXFER_SIZE_SHIFT                 0

/***************************************************************************
 *DMQ_3_3 - DMQ descriptor 3 - Current Descriptor Address and Control
 ***************************************************************************/
/* XPT_WDMA_CH0 :: DMQ_3_3 :: CURRENT_DESC_ADDRESS [31:04] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_CURRENT_DESC_ADDRESS_MASK        0xfffffff0
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_CURRENT_DESC_ADDRESS_SHIFT       4

/* XPT_WDMA_CH0 :: DMQ_3_3 :: DATA_UNIT_SIZE [03:02] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_DATA_UNIT_SIZE_MASK              0x0000000c
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_DATA_UNIT_SIZE_SHIFT             2

/* XPT_WDMA_CH0 :: DMQ_3_3 :: INT_EN [01:01] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_INT_EN_MASK                      0x00000002
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_INT_EN_SHIFT                     1

/* XPT_WDMA_CH0 :: DMQ_3_3 :: LAST [00:00] */
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_LAST_MASK                        0x00000001
#define BCHP_XPT_WDMA_CH0_DMQ_3_3_LAST_SHIFT                       0

#endif /* #ifndef BCHP_XPT_WDMA_CH0_H__ */

/* End of File */
