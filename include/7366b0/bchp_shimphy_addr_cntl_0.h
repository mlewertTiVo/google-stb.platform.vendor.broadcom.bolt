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
 * Date:           Generated on              Thu Apr 24 03:08:55 2014
 *                 Full Compile MD5 Checksum 1af2c2cf28828ece5496e1312aac0166
 *                   (minus title and desc)  
 *                 MD5 Checksum              3203ffce85e3fc9a10f84123e54057b0
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

#ifndef BCHP_SHIMPHY_ADDR_CNTL_0_H__
#define BCHP_SHIMPHY_ADDR_CNTL_0_H__

/***************************************************************************
 *SHIMPHY_ADDR_CNTL_0 - DDR SHIMPHY   Control Registers
 ***************************************************************************/
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG          0x00508000 /* SHIMPHY Config register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID  0x00508004 /* SHIMPHY Revision ID Register. */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET           0x00508008 /* DDR soft reset register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO   0x00508028 /* Command and Data FIFO Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH     0x0050802c /* Read Datapath Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_FLAG_BUS        0x00508030 /* TP_OUT bus value Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC            0x00508034 /* Miscellaneous Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL     0x00508038 /* DFI Interface Control Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS      0x0050803c /* DFI Interface Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT    0x00508040 /* PHY Power Control Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS    0x00508044 /* DDR4 Alert Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING 0x00508048 /* DDR PHY Idle power saving Control register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL   0x0050808c /* DDR Pad control register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS  0x0050809c /* SHIMPHY Status Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RW       0x005080a4 /* Spare register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RW       0x005080a8 /* Spare register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RO       0x005080ac /* Spare register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RO       0x005080b0 /* Spare register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL 0x005080b4 /* FORCE_DDR3_RESET Deassert  Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL 0x005080b8 /* GDDR5 CRC CONTROL Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_31_0 0x005080bc /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_63_32 0x005080c0 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_95_64 0x005080c4 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_127_96 0x005080c8 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_159_128 0x005080cc /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_191_160 0x005080d0 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_223_192 0x005080d4 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_255_224 0x005080d8 /* GDDR5 Client CRC Enable Register */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_WRITE_ERROR_CNT 0x005080dc /* GDDR5 Write CRC Error Count */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_READ_ERROR_CNT 0x005080e0 /* GDDR5 Read CRC Error Count */

/***************************************************************************
 *CONFIG - SHIMPHY Config register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: DFI_CLK_DISABLE [31:31] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DFI_CLK_DISABLE_MASK       0x80000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DFI_CLK_DISABLE_SHIFT      31
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DFI_CLK_DISABLE_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: DRAM_NOP_OR_DSEL_CMD [30:30] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DRAM_NOP_OR_DSEL_CMD_MASK  0x40000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DRAM_NOP_OR_DSEL_CMD_SHIFT 30
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_DRAM_NOP_OR_DSEL_CMD_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: LAST_RD_STRETCH [29:29] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_RD_STRETCH_MASK       0x20000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_RD_STRETCH_SHIFT      29
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_RD_STRETCH_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: reserved0 [28:24] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_reserved0_MASK             0x1f000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_reserved0_SHIFT            24

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: LAST_READ_LATENCY [23:16] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_READ_LATENCY_MASK     0x00ff0000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_READ_LATENCY_SHIFT    16
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_LAST_READ_LATENCY_DEFAULT  0x0000000b

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: READ_LATENCY [15:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_READ_LATENCY_MASK          0x0000ff00
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_READ_LATENCY_SHIFT         8
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_READ_LATENCY_DEFAULT       0x00000007

/* SHIMPHY_ADDR_CNTL_0 :: CONFIG :: WRITE_LATENCY [07:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_WRITE_LATENCY_MASK         0x000000ff
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_WRITE_LATENCY_SHIFT        0
#define BCHP_SHIMPHY_ADDR_CNTL_0_CONFIG_WRITE_LATENCY_DEFAULT      0x0000000e

/***************************************************************************
 *SHIMPHY_REV_ID - SHIMPHY Revision ID Register.
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SHIMPHY_REV_ID :: reserved0 [31:16] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_reserved0_MASK     0xffff0000
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_reserved0_SHIFT    16

/* SHIMPHY_ADDR_CNTL_0 :: SHIMPHY_REV_ID :: MAJOR_ID [15:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MAJOR_ID_MASK      0x0000ff00
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MAJOR_ID_SHIFT     8
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MAJOR_ID_DEFAULT   0x00000001

/* SHIMPHY_ADDR_CNTL_0 :: SHIMPHY_REV_ID :: MINOR_ID [07:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MINOR_ID_MASK      0x000000ff
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MINOR_ID_SHIFT     0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MINOR_ID_DEFAULT   0x00000000

/***************************************************************************
 *RESET - DDR soft reset register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: RESET :: reserved0 [31:03] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_reserved0_MASK              0xfffffff8
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_reserved0_SHIFT             3

/* SHIMPHY_ADDR_CNTL_0 :: RESET :: DATAPATH_216_RESET [02:02] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_216_RESET_MASK     0x00000004
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_216_RESET_SHIFT    2
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_216_RESET_DEFAULT  0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: RESET :: DATAPATH_DDR_RESET [01:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_DDR_RESET_MASK     0x00000002
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_DDR_RESET_SHIFT    1
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_DATAPATH_DDR_RESET_DEFAULT  0x00000001

/* SHIMPHY_ADDR_CNTL_0 :: RESET :: PHY_PWRUP_RSB [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_PHY_PWRUP_RSB_MASK          0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_PHY_PWRUP_RSB_SHIFT         0
#define BCHP_SHIMPHY_ADDR_CNTL_0_RESET_PHY_PWRUP_RSB_DEFAULT       0x00000000

/***************************************************************************
 *CMD_DATA_FIFO - Command and Data FIFO Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: reserved0 [31:26] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_reserved0_MASK      0xfc000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_reserved0_SHIFT     26

/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: FIFO_FULL [25:25] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_FIFO_FULL_MASK      0x02000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_FIFO_FULL_SHIFT     25

/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: FIFO_EMPTY [24:24] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_FIFO_EMPTY_MASK     0x01000000
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_FIFO_EMPTY_SHIFT    24

/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: reserved1 [23:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_reserved1_MASK      0x00fffc00
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_reserved1_SHIFT     10

/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: WR_PNTR [09:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_WR_PNTR_MASK        0x000003e0
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_WR_PNTR_SHIFT       5

/* SHIMPHY_ADDR_CNTL_0 :: CMD_DATA_FIFO :: RD_PNTR [04:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_RD_PNTR_MASK        0x0000001f
#define BCHP_SHIMPHY_ADDR_CNTL_0_CMD_DATA_FIFO_RD_PNTR_SHIFT       0

/***************************************************************************
 *RD_DATAPATH - Read Datapath Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: RD_DATAPATH :: reserved0 [31:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_reserved0_MASK        0xfffffc00
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_reserved0_SHIFT       10

/* SHIMPHY_ADDR_CNTL_0 :: RD_DATAPATH :: WR_PNTR [09:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_WR_PNTR_MASK          0x000003e0
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_WR_PNTR_SHIFT         5

/* SHIMPHY_ADDR_CNTL_0 :: RD_DATAPATH :: RD_PNTR [04:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_RD_PNTR_MASK          0x0000001f
#define BCHP_SHIMPHY_ADDR_CNTL_0_RD_DATAPATH_RD_PNTR_SHIFT         0

/***************************************************************************
 *FLAG_BUS - TP_OUT bus value Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: FLAG_BUS :: FLAG_BUS [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_FLAG_BUS_FLAG_BUS_MASK            0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_FLAG_BUS_FLAG_BUS_SHIFT           0

/***************************************************************************
 *MISC - Miscellaneous Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: MISC :: reserved0 [31:20] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved0_MASK               0xfff00000
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved0_SHIFT              20

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: ASYNC_FIFO_AF_THRESHOLD [19:15] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_ASYNC_FIFO_AF_THRESHOLD_MASK 0x000f8000
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_ASYNC_FIFO_AF_THRESHOLD_SHIFT 15
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_ASYNC_FIFO_AF_THRESHOLD_DEFAULT 0x0000000a

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: reserved_for_eco1 [14:12] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved_for_eco1_MASK       0x00007000
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved_for_eco1_SHIFT      12
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved_for_eco1_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: DFI_ERROR_STATUS_CLR [11:11] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DFI_ERROR_STATUS_CLR_MASK    0x00000800
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DFI_ERROR_STATUS_CLR_SHIFT   11
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DFI_ERROR_STATUS_CLR_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: EDC_MONTIOR_STATUS_CLR [10:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_EDC_MONTIOR_STATUS_CLR_MASK  0x00000400
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_EDC_MONTIOR_STATUS_CLR_SHIFT 10
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_EDC_MONTIOR_STATUS_CLR_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: VDL_MONITOR_STATUS_CLR [09:09] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_VDL_MONITOR_STATUS_CLR_MASK  0x00000200
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_VDL_MONITOR_STATUS_CLR_SHIFT 9
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_VDL_MONITOR_STATUS_CLR_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: FUNC1 [08:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC1_MASK                   0x00000100
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC1_SHIFT                  8
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC1_DEFAULT                0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: FUNC0 [07:07] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC0_MASK                   0x00000080
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC0_SHIFT                  7
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_FUNC0_DEFAULT                0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: C2IO_INIT_RDY_OVR [06:06] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_C2IO_INIT_RDY_OVR_MASK       0x00000040
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_C2IO_INIT_RDY_OVR_SHIFT      6
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_C2IO_INIT_RDY_OVR_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: RD_FIFO_HOLD_CLR [05:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_RD_FIFO_HOLD_CLR_MASK        0x00000020
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_RD_FIFO_HOLD_CLR_SHIFT       5
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_RD_FIFO_HOLD_CLR_DEFAULT     0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: CMD_FIFO_HOLD_CLR [04:04] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_CMD_FIFO_HOLD_CLR_MASK       0x00000010
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_CMD_FIFO_HOLD_CLR_SHIFT      4
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_CMD_FIFO_HOLD_CLR_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: reserved2 [03:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved2_MASK               0x0000000e
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_reserved2_SHIFT              1

/* SHIMPHY_ADDR_CNTL_0 :: MISC :: DATA_OVERRUN_CLR [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DATA_OVERRUN_CLR_MASK        0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DATA_OVERRUN_CLR_SHIFT       0
#define BCHP_SHIMPHY_ADDR_CNTL_0_MISC_DATA_OVERRUN_CLR_DEFAULT     0x00000000

/***************************************************************************
 *DFI_CONTROL - DFI Interface Control Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: DFI_ERROR_STATUS_INTR_ENA [31:16] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_DFI_ERROR_STATUS_INTR_ENA_MASK 0xffff0000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_DFI_ERROR_STATUS_INTR_ENA_SHIFT 16
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_DFI_ERROR_STATUS_INTR_ENA_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_PLL_RESETB [15:15] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_RESETB_MASK   0x00008000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_RESETB_SHIFT  15
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_RESETB_DEFAULT 0x00000001

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_ODT [14:14] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ODT_MASK          0x00004000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ODT_SHIFT         14
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ODT_DEFAULT       0x00000001

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_ISO_PHY_PLL [13:13] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ISO_PHY_PLL_MASK  0x00002000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ISO_PHY_PLL_SHIFT 13
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_ISO_PHY_PLL_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_PLL_PWRDWN [12:12] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_PWRDWN_MASK   0x00001000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_PWRDWN_SHIFT  12
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_PWRDWN_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_PLL_CTRL_RDB_OVERRIDE [11:11] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_CTRL_RDB_OVERRIDE_MASK 0x00000800
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_CTRL_RDB_OVERRIDE_SHIFT 11
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_CTRL_RDB_OVERRIDE_DEFAULT 0x00000001

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_FORCE_CKE_RSTB [10:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_FORCE_CKE_RSTB_MASK 0x00000400
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_FORCE_CKE_RSTB_SHIFT 10
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_FORCE_CKE_RSTB_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: TM2_MUX_SEL [09:09] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_TM2_MUX_SEL_MASK      0x00000200
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_TM2_MUX_SEL_SHIFT     9
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_TM2_MUX_SEL_DEFAULT   0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: PHY_PLL_HOLD_CH [08:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_HOLD_CH_MASK  0x00000100
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_HOLD_CH_SHIFT 8
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_PHY_PLL_HOLD_CH_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: CS1_OVERRIDE_VALUE [07:07] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_VALUE_MASK 0x00000080
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_VALUE_SHIFT 7
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_VALUE_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: CS1_OVERRIDE [06:06] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_MASK     0x00000040
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_SHIFT    6
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS1_OVERRIDE_DEFAULT  0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: CS0_OVERRIDE_VALUE [05:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_VALUE_MASK 0x00000020
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_VALUE_SHIFT 5
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_VALUE_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: CS0_OVERRIDE [04:04] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_MASK     0x00000010
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_SHIFT    4
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_CS0_OVERRIDE_DEFAULT  0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: reserved0 [03:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_reserved0_MASK        0x0000000e
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_reserved0_SHIFT       1

/* SHIMPHY_ADDR_CNTL_0 :: DFI_CONTROL :: LATCH_FIRST_ERROR [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_LATCH_FIRST_ERROR_MASK 0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_LATCH_FIRST_ERROR_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_CONTROL_LATCH_FIRST_ERROR_DEFAULT 0x00000001

/***************************************************************************
 *DFI_STATUS - DFI Interface Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: reserved0 [31:19] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved0_MASK         0xfff80000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved0_SHIFT        19

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: PHY_PLL_LOCK [18:18] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_PLL_LOCK_MASK      0x00040000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_PLL_LOCK_SHIFT     18
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_PLL_LOCK_DEFAULT   0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: PHY_EDC_MONITOR_STATUS [17:17] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_EDC_MONITOR_STATUS_MASK 0x00020000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_EDC_MONITOR_STATUS_SHIFT 17
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_EDC_MONITOR_STATUS_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: PHY_VDL_MONITOR_STATUS [16:16] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_VDL_MONITOR_STATUS_MASK 0x00010000
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_VDL_MONITOR_STATUS_SHIFT 16
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_PHY_VDL_MONITOR_STATUS_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: reserved1 [15:09] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved1_MASK         0x0000fe00
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved1_SHIFT        9

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: ERROR_VALID [08:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_VALID_MASK       0x00000100
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_VALID_SHIFT      8
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_VALID_DEFAULT    0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: reserved2 [07:04] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved2_MASK         0x000000f0
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_reserved2_SHIFT        4

/* SHIMPHY_ADDR_CNTL_0 :: DFI_STATUS :: ERROR_INFO [03:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_INFO_MASK        0x0000000f
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_INFO_SHIFT       0
#define BCHP_SHIMPHY_ADDR_CNTL_0_DFI_STATUS_ERROR_INFO_DEFAULT     0x00000000

/***************************************************************************
 *PHY_LPM_STAT - PHY Power Control Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: PHY_LPM_STAT :: reserved0 [31:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_reserved0_MASK       0xfffffc00
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_reserved0_SHIFT      10

/* SHIMPHY_ADDR_CNTL_0 :: PHY_LPM_STAT :: PHY_RBUS_IS_IDLE [09:09] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_RBUS_IS_IDLE_MASK 0x00000200
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_RBUS_IS_IDLE_SHIFT 9
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_RBUS_IS_IDLE_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: PHY_LPM_STAT :: PHY_IS_IDLE [08:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_IS_IDLE_MASK     0x00000100
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_IS_IDLE_SHIFT    8
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_PHY_IS_IDLE_DEFAULT  0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: PHY_LPM_STAT :: reserved1 [07:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_reserved1_MASK       0x000000ff
#define BCHP_SHIMPHY_ADDR_CNTL_0_PHY_LPM_STAT_reserved1_SHIFT      0

/***************************************************************************
 *ALERT_STATUS - DDR4 Alert Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: ALERT_STATUS :: reserved0 [31:02] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_reserved0_MASK       0xfffffffc
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_reserved0_SHIFT      2

/* SHIMPHY_ADDR_CNTL_0 :: ALERT_STATUS :: ERROR_CRC_WRITE [01:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CRC_WRITE_MASK 0x00000002
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CRC_WRITE_SHIFT 1
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CRC_WRITE_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: ALERT_STATUS :: ERROR_CMD_PARITY [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CMD_PARITY_MASK 0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CMD_PARITY_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_ALERT_STATUS_ERROR_CMD_PARITY_DEFAULT 0x00000000

/***************************************************************************
 *IDLE_POWER_SAVING - DDR PHY Idle power saving Control register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: IDLE_POWER_SAVING :: reserved0 [31:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_reserved0_MASK  0xffffffe0
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_reserved0_SHIFT 5

/* SHIMPHY_ADDR_CNTL_0 :: IDLE_POWER_SAVING :: PhyAddrCntl [04:04] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_PhyAddrCntl_MASK 0x00000010
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_PhyAddrCntl_SHIFT 4
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_PhyAddrCntl_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: IDLE_POWER_SAVING :: reserved1 [03:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_reserved1_MASK  0x0000000f
#define BCHP_SHIMPHY_ADDR_CNTL_0_IDLE_POWER_SAVING_reserved1_SHIFT 0

/***************************************************************************
 *DDR_PAD_CNTRL - DDR Pad control register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: reserved0 [31:10] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved0_MASK      0xfffffc00
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved0_SHIFT     10

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: GATE_PLL_S3 [09:09] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_S3_MASK    0x00000200
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_S3_SHIFT   9
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_S3_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: GATE_PLL_ON_SELFREF [08:08] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_ON_SELFREF_MASK 0x00000100
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_ON_SELFREF_SHIFT 8
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_GATE_PLL_ON_SELFREF_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: reserved1 [07:07] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved1_MASK      0x00000080
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved1_SHIFT     7

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: IDDQ_MODE_ON_SELFREF [06:06] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_IDDQ_MODE_ON_SELFREF_MASK 0x00000040
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_IDDQ_MODE_ON_SELFREF_SHIFT 6
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_IDDQ_MODE_ON_SELFREF_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: PHY_IDLE_ENABLE [05:05] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_PHY_IDLE_ENABLE_MASK 0x00000020
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_PHY_IDLE_ENABLE_SHIFT 5
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_PHY_IDLE_ENABLE_DEFAULT 0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: reserved2 [04:04] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved2_MASK      0x00000010
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_reserved2_SHIFT     4

/* SHIMPHY_ADDR_CNTL_0 :: DDR_PAD_CNTRL :: CNTRL [03:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_CNTRL_MASK          0x0000000f
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_CNTRL_SHIFT         0
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL_CNTRL_DEFAULT       0x00000000

/***************************************************************************
 *SHIMPHY_STATUS - SHIMPHY Status Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SHIMPHY_STATUS :: reserved0 [31:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS_reserved0_MASK     0xfffffffe
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS_reserved0_SHIFT    1

/* SHIMPHY_ADDR_CNTL_0 :: SHIMPHY_STATUS :: READY [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS_READY_MASK         0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS_READY_SHIFT        0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_STATUS_READY_DEFAULT      0x00000000

/***************************************************************************
 *SPARE0_RW - Spare register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SPARE0_RW :: reserved_for_eco0 [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RW_reserved_for_eco0_MASK  0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RW_reserved_for_eco0_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RW_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *SPARE1_RW - Spare register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SPARE1_RW :: reserved_for_eco0 [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RW_reserved_for_eco0_MASK  0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RW_reserved_for_eco0_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RW_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *SPARE0_RO - Spare register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SPARE0_RO :: reserved_for_eco0 [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RO_reserved_for_eco0_MASK  0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RO_reserved_for_eco0_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE0_RO_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *SPARE1_RO - Spare register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: SPARE1_RO :: reserved_for_eco0 [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RO_reserved_for_eco0_MASK  0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RO_reserved_for_eco0_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_SPARE1_RO_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *DDR3_RESET_CNTRL - FORCE_DDR3_RESET Deassert  Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: DDR3_RESET_CNTRL :: UNUSED [31:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_UNUSED_MASK      0xfffffffe
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_UNUSED_SHIFT     1
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_UNUSED_DEFAULT   0x00000000

/* SHIMPHY_ADDR_CNTL_0 :: DDR3_RESET_CNTRL :: FORCE_DDR3_RESET [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_FORCE_DDR3_RESET_MASK 0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_FORCE_DDR3_RESET_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_DDR3_RESET_CNTRL_FORCE_DDR3_RESET_DEFAULT 0x00000001

/***************************************************************************
 *GDDR5_CRC_CONTROL - GDDR5 CRC CONTROL Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CRC_CONTROL :: reserved0 [31:01] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL_reserved0_MASK  0xfffffffe
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL_reserved0_SHIFT 1

/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CRC_CONTROL :: GDDR5_CRC_ENABLE [00:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL_GDDR5_CRC_ENABLE_MASK 0x00000001
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL_GDDR5_CRC_ENABLE_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_CONTROL_GDDR5_CRC_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_31_0 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_31_0 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_31_0_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_31_0_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_31_0_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_63_32 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_63_32 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_63_32_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_63_32_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_63_32_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_95_64 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_95_64 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_95_64_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_95_64_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_95_64_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_127_96 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_127_96 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_127_96_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_127_96_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_127_96_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_159_128 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_159_128 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_159_128_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_159_128_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_159_128_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_191_160 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_191_160 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_191_160_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_191_160_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_191_160_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_223_192 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_223_192 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_223_192_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_223_192_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_223_192_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CLIENT_CRC_EN_255_224 - GDDR5 Client CRC Enable Register
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CLIENT_CRC_EN_255_224 :: GDDR5_CLIENT_CRC_EN [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_255_224_GDDR5_CLIENT_CRC_EN_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_255_224_GDDR5_CLIENT_CRC_EN_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CLIENT_CRC_EN_255_224_GDDR5_CLIENT_CRC_EN_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CRC_WRITE_ERROR_CNT - GDDR5 Write CRC Error Count
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CRC_WRITE_ERROR_CNT :: GDDR5_CRC_ERROR_CNT [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_WRITE_ERROR_CNT_GDDR5_CRC_ERROR_CNT_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_WRITE_ERROR_CNT_GDDR5_CRC_ERROR_CNT_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_WRITE_ERROR_CNT_GDDR5_CRC_ERROR_CNT_DEFAULT 0x00000000

/***************************************************************************
 *GDDR5_CRC_READ_ERROR_CNT - GDDR5 Read CRC Error Count
 ***************************************************************************/
/* SHIMPHY_ADDR_CNTL_0 :: GDDR5_CRC_READ_ERROR_CNT :: GDDR5_CRC_ERROR_CNT [31:00] */
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_READ_ERROR_CNT_GDDR5_CRC_ERROR_CNT_MASK 0xffffffff
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_READ_ERROR_CNT_GDDR5_CRC_ERROR_CNT_SHIFT 0
#define BCHP_SHIMPHY_ADDR_CNTL_0_GDDR5_CRC_READ_ERROR_CNT_GDDR5_CRC_ERROR_CNT_DEFAULT 0x00000000

#endif /* #ifndef BCHP_SHIMPHY_ADDR_CNTL_0_H__ */

/* End of File */
