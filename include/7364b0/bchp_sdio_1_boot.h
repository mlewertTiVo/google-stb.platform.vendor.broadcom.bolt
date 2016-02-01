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
 * Date:           Generated on              Wed Apr 23 03:08:27 2014
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

#ifndef BCHP_SDIO_1_BOOT_H__
#define BCHP_SDIO_1_BOOT_H__

/***************************************************************************
 *SDIO_1_BOOT - SDIO (EMMC) Boot Registers
 ***************************************************************************/
#define BCHP_SDIO_1_BOOT_MAIN_CTL                0x00440400 /* Main control register */
#define BCHP_SDIO_1_BOOT_STATUS                  0x00440404 /* Status */
#define BCHP_SDIO_1_BOOT_VERSION                 0x00440408 /* Version */
#define BCHP_SDIO_1_BOOT_CLK_DIV                 0x00440410 /* Clock Divide Override */
#define BCHP_SDIO_1_BOOT_RESET_CNT               0x00440414 /* Reset Count */
#define BCHP_SDIO_1_BOOT_RAM_FILL                0x00440418 /* Ram Fill */
#define BCHP_SDIO_1_BOOT_ERROR_ADDR              0x0044041c /* Error Address */
#define BCHP_SDIO_1_BOOT_BASE_ADDR0              0x00440420 /* RAM Base address */
#define BCHP_SDIO_1_BOOT_BASE_ADDR1              0x00440424 /* RAM Base address */
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT            0x00440428 /* RAM Fill Cnt */
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME        0x0044042c /* Time for Data Fetch */
#define BCHP_SDIO_1_BOOT_DEBUG                   0x0044043c /* Debug */

/***************************************************************************
 *MAIN_CTL - Main control register
 ***************************************************************************/
/* SDIO_1_BOOT :: MAIN_CTL :: reserved0 [31:03] */
#define BCHP_SDIO_1_BOOT_MAIN_CTL_reserved0_MASK                   0xfffffff8
#define BCHP_SDIO_1_BOOT_MAIN_CTL_reserved0_SHIFT                  3

/* SDIO_1_BOOT :: MAIN_CTL :: DivSpeedUp [02:02] */
#define BCHP_SDIO_1_BOOT_MAIN_CTL_DivSpeedUp_MASK                  0x00000004
#define BCHP_SDIO_1_BOOT_MAIN_CTL_DivSpeedUp_SHIFT                 2
#define BCHP_SDIO_1_BOOT_MAIN_CTL_DivSpeedUp_DEFAULT               0x00000000

/* SDIO_1_BOOT :: MAIN_CTL :: reserved1 [01:01] */
#define BCHP_SDIO_1_BOOT_MAIN_CTL_reserved1_MASK                   0x00000002
#define BCHP_SDIO_1_BOOT_MAIN_CTL_reserved1_SHIFT                  1

/* SDIO_1_BOOT :: MAIN_CTL :: BootEna [00:00] */
#define BCHP_SDIO_1_BOOT_MAIN_CTL_BootEna_MASK                     0x00000001
#define BCHP_SDIO_1_BOOT_MAIN_CTL_BootEna_SHIFT                    0
#define BCHP_SDIO_1_BOOT_MAIN_CTL_BootEna_DEFAULT                  0x00000000

/***************************************************************************
 *STATUS - Status
 ***************************************************************************/
/* SDIO_1_BOOT :: STATUS :: reserved0 [31:11] */
#define BCHP_SDIO_1_BOOT_STATUS_reserved0_MASK                     0xfffff800
#define BCHP_SDIO_1_BOOT_STATUS_reserved0_SHIFT                    11

/* SDIO_1_BOOT :: STATUS :: Boot_Rbus_Error [10:10] */
#define BCHP_SDIO_1_BOOT_STATUS_Boot_Rbus_Error_MASK               0x00000400
#define BCHP_SDIO_1_BOOT_STATUS_Boot_Rbus_Error_SHIFT              10
#define BCHP_SDIO_1_BOOT_STATUS_Boot_Rbus_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: AHB_Slave_Error [09:09] */
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Slave_Error_MASK               0x00000200
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Slave_Error_SHIFT              9
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Slave_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: AHB_Master_Error [08:08] */
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Master_Error_MASK              0x00000100
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Master_Error_SHIFT             8
#define BCHP_SDIO_1_BOOT_STATUS_AHB_Master_Error_DEFAULT           0x00000000

/* SDIO_1_BOOT :: STATUS :: SDIO_Host_Error [07:07] */
#define BCHP_SDIO_1_BOOT_STATUS_SDIO_Host_Error_MASK               0x00000080
#define BCHP_SDIO_1_BOOT_STATUS_SDIO_Host_Error_SHIFT              7
#define BCHP_SDIO_1_BOOT_STATUS_SDIO_Host_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: BusWidth [06:05] */
#define BCHP_SDIO_1_BOOT_STATUS_BusWidth_MASK                      0x00000060
#define BCHP_SDIO_1_BOOT_STATUS_BusWidth_SHIFT                     5

/* SDIO_1_BOOT :: STATUS :: BigEndian [04:04] */
#define BCHP_SDIO_1_BOOT_STATUS_BigEndian_MASK                     0x00000010
#define BCHP_SDIO_1_BOOT_STATUS_BigEndian_SHIFT                    4

/* SDIO_1_BOOT :: STATUS :: FetchActive [03:03] */
#define BCHP_SDIO_1_BOOT_STATUS_FetchActive_MASK                   0x00000008
#define BCHP_SDIO_1_BOOT_STATUS_FetchActive_SHIFT                  3
#define BCHP_SDIO_1_BOOT_STATUS_FetchActive_DEFAULT                0x00000000

/* SDIO_1_BOOT :: STATUS :: RamValid1 [02:02] */
#define BCHP_SDIO_1_BOOT_STATUS_RamValid1_MASK                     0x00000004
#define BCHP_SDIO_1_BOOT_STATUS_RamValid1_SHIFT                    2
#define BCHP_SDIO_1_BOOT_STATUS_RamValid1_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: STATUS :: RamValid0 [01:01] */
#define BCHP_SDIO_1_BOOT_STATUS_RamValid0_MASK                     0x00000002
#define BCHP_SDIO_1_BOOT_STATUS_RamValid0_SHIFT                    1
#define BCHP_SDIO_1_BOOT_STATUS_RamValid0_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: STATUS :: BootMode [00:00] */
#define BCHP_SDIO_1_BOOT_STATUS_BootMode_MASK                      0x00000001
#define BCHP_SDIO_1_BOOT_STATUS_BootMode_SHIFT                     0

/***************************************************************************
 *VERSION - Version
 ***************************************************************************/
/* SDIO_1_BOOT :: VERSION :: reserved0 [31:24] */
#define BCHP_SDIO_1_BOOT_VERSION_reserved0_MASK                    0xff000000
#define BCHP_SDIO_1_BOOT_VERSION_reserved0_SHIFT                   24

/* SDIO_1_BOOT :: VERSION :: MajorRev [23:16] */
#define BCHP_SDIO_1_BOOT_VERSION_MajorRev_MASK                     0x00ff0000
#define BCHP_SDIO_1_BOOT_VERSION_MajorRev_SHIFT                    16
#define BCHP_SDIO_1_BOOT_VERSION_MajorRev_DEFAULT                  0x00000001

/* SDIO_1_BOOT :: VERSION :: MinorRev [15:08] */
#define BCHP_SDIO_1_BOOT_VERSION_MinorRev_MASK                     0x0000ff00
#define BCHP_SDIO_1_BOOT_VERSION_MinorRev_SHIFT                    8
#define BCHP_SDIO_1_BOOT_VERSION_MinorRev_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: VERSION :: reserved1 [07:04] */
#define BCHP_SDIO_1_BOOT_VERSION_reserved1_MASK                    0x000000f0
#define BCHP_SDIO_1_BOOT_VERSION_reserved1_SHIFT                   4

/* SDIO_1_BOOT :: VERSION :: MetalRev [03:00] */
#define BCHP_SDIO_1_BOOT_VERSION_MetalRev_MASK                     0x0000000f
#define BCHP_SDIO_1_BOOT_VERSION_MetalRev_SHIFT                    0
#define BCHP_SDIO_1_BOOT_VERSION_MetalRev_DEFAULT                  0x00000000

/***************************************************************************
 *CLK_DIV - Clock Divide Override
 ***************************************************************************/
/* SDIO_1_BOOT :: CLK_DIV :: reserved0 [31:12] */
#define BCHP_SDIO_1_BOOT_CLK_DIV_reserved0_MASK                    0xfffff000
#define BCHP_SDIO_1_BOOT_CLK_DIV_reserved0_SHIFT                   12

/* SDIO_1_BOOT :: CLK_DIV :: CmdDiv [11:08] */
#define BCHP_SDIO_1_BOOT_CLK_DIV_CmdDiv_MASK                       0x00000f00
#define BCHP_SDIO_1_BOOT_CLK_DIV_CmdDiv_SHIFT                      8
#define BCHP_SDIO_1_BOOT_CLK_DIV_CmdDiv_DEFAULT                    0x00000000

/* SDIO_1_BOOT :: CLK_DIV :: reserved1 [07:04] */
#define BCHP_SDIO_1_BOOT_CLK_DIV_reserved1_MASK                    0x000000f0
#define BCHP_SDIO_1_BOOT_CLK_DIV_reserved1_SHIFT                   4

/* SDIO_1_BOOT :: CLK_DIV :: DataDiv [03:00] */
#define BCHP_SDIO_1_BOOT_CLK_DIV_DataDiv_MASK                      0x0000000f
#define BCHP_SDIO_1_BOOT_CLK_DIV_DataDiv_SHIFT                     0
#define BCHP_SDIO_1_BOOT_CLK_DIV_DataDiv_DEFAULT                   0x00000000

/***************************************************************************
 *RESET_CNT - Reset Count
 ***************************************************************************/
/* SDIO_1_BOOT :: RESET_CNT :: reserved0 [31:16] */
#define BCHP_SDIO_1_BOOT_RESET_CNT_reserved0_MASK                  0xffff0000
#define BCHP_SDIO_1_BOOT_RESET_CNT_reserved0_SHIFT                 16

/* SDIO_1_BOOT :: RESET_CNT :: ResetCnt [15:00] */
#define BCHP_SDIO_1_BOOT_RESET_CNT_ResetCnt_MASK                   0x0000ffff
#define BCHP_SDIO_1_BOOT_RESET_CNT_ResetCnt_SHIFT                  0
#define BCHP_SDIO_1_BOOT_RESET_CNT_ResetCnt_DEFAULT                0x00000000

/***************************************************************************
 *RAM_FILL - Ram Fill
 ***************************************************************************/
/* SDIO_1_BOOT :: RAM_FILL :: reserved0 [31:22] */
#define BCHP_SDIO_1_BOOT_RAM_FILL_reserved0_MASK                   0xffc00000
#define BCHP_SDIO_1_BOOT_RAM_FILL_reserved0_SHIFT                  22

/* SDIO_1_BOOT :: RAM_FILL :: FillAddr [21:00] */
#define BCHP_SDIO_1_BOOT_RAM_FILL_FillAddr_MASK                    0x003fffff
#define BCHP_SDIO_1_BOOT_RAM_FILL_FillAddr_SHIFT                   0
#define BCHP_SDIO_1_BOOT_RAM_FILL_FillAddr_DEFAULT                 0x00000000

/***************************************************************************
 *ERROR_ADDR - Error Address
 ***************************************************************************/
/* SDIO_1_BOOT :: ERROR_ADDR :: ErrorAddr [31:00] */
#define BCHP_SDIO_1_BOOT_ERROR_ADDR_ErrorAddr_MASK                 0xffffffff
#define BCHP_SDIO_1_BOOT_ERROR_ADDR_ErrorAddr_SHIFT                0
#define BCHP_SDIO_1_BOOT_ERROR_ADDR_ErrorAddr_DEFAULT              0x00000000

/***************************************************************************
 *BASE_ADDR0 - RAM Base address
 ***************************************************************************/
/* SDIO_1_BOOT :: BASE_ADDR0 :: reserved0 [31:22] */
#define BCHP_SDIO_1_BOOT_BASE_ADDR0_reserved0_MASK                 0xffc00000
#define BCHP_SDIO_1_BOOT_BASE_ADDR0_reserved0_SHIFT                22

/* SDIO_1_BOOT :: BASE_ADDR0 :: BaseAddr [21:00] */
#define BCHP_SDIO_1_BOOT_BASE_ADDR0_BaseAddr_MASK                  0x003fffff
#define BCHP_SDIO_1_BOOT_BASE_ADDR0_BaseAddr_SHIFT                 0
#define BCHP_SDIO_1_BOOT_BASE_ADDR0_BaseAddr_DEFAULT               0x00000000

/***************************************************************************
 *BASE_ADDR1 - RAM Base address
 ***************************************************************************/
/* SDIO_1_BOOT :: BASE_ADDR1 :: reserved0 [31:22] */
#define BCHP_SDIO_1_BOOT_BASE_ADDR1_reserved0_MASK                 0xffc00000
#define BCHP_SDIO_1_BOOT_BASE_ADDR1_reserved0_SHIFT                22

/* SDIO_1_BOOT :: BASE_ADDR1 :: BaseAddr [21:00] */
#define BCHP_SDIO_1_BOOT_BASE_ADDR1_BaseAddr_MASK                  0x003fffff
#define BCHP_SDIO_1_BOOT_BASE_ADDR1_BaseAddr_SHIFT                 0
#define BCHP_SDIO_1_BOOT_BASE_ADDR1_BaseAddr_DEFAULT               0x00000000

/***************************************************************************
 *RAM_FILL_CNT - RAM Fill Cnt
 ***************************************************************************/
/* SDIO_1_BOOT :: RAM_FILL_CNT :: reserved0 [31:11] */
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT_reserved0_MASK               0xfffff800
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT_reserved0_SHIFT              11

/* SDIO_1_BOOT :: RAM_FILL_CNT :: RamFillCnt [10:00] */
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT_RamFillCnt_MASK              0x000007ff
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT_RamFillCnt_SHIFT             0
#define BCHP_SDIO_1_BOOT_RAM_FILL_CNT_RamFillCnt_DEFAULT           0x00000000

/***************************************************************************
 *DATA_ACCESS_TIME - Time for Data Fetch
 ***************************************************************************/
/* SDIO_1_BOOT :: DATA_ACCESS_TIME :: reserved0 [31:16] */
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME_reserved0_MASK           0xffff0000
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME_reserved0_SHIFT          16

/* SDIO_1_BOOT :: DATA_ACCESS_TIME :: DataAccessTime [15:00] */
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME_DataAccessTime_MASK      0x0000ffff
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME_DataAccessTime_SHIFT     0
#define BCHP_SDIO_1_BOOT_DATA_ACCESS_TIME_DataAccessTime_DEFAULT   0x00000000

/***************************************************************************
 *DEBUG - Debug
 ***************************************************************************/
/* SDIO_1_BOOT :: DEBUG :: Debug [31:00] */
#define BCHP_SDIO_1_BOOT_DEBUG_Debug_MASK                          0xffffffff
#define BCHP_SDIO_1_BOOT_DEBUG_Debug_SHIFT                         0

#endif /* #ifndef BCHP_SDIO_1_BOOT_H__ */

/* End of File */
