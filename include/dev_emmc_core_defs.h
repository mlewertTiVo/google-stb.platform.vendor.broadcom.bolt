/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _EMMC_CORE_DEFS_H
#define _EMMC_CORE_DEFS_H

/* eMMC Version */
#define JESD84_V42    2
#define JESD84_V43    3
#define JESD84_V44    4
#define JESD84_V441   5
#define JESD84_V45    6

/* RCA */
#define EMMC_RCA 0x00020000  /* From 0x00020000, 0x00030000, ..., 0xFFFF0000 */

/* HS_TIMING_OPTION */
#define HS_TIMING_FULL  0	/* 0 ~  26MHz, Backwards Compatibility */
#define HS_TIMING_HS    1	/* 0 ~  52MHz, High Speed Mode */
#define HS_TIMING_HS200 2	/* 0 ~ 200MHz, HS200 Mode */

/* BUS_FREQ_OPTION */
#define BUS_FREQ_52MHZ  1	/* 100MHz/2^1, Base clock = 100MHz */
#define BUS_FREQ_26MHZ  2
#define BUS_FREQ_13MHZ  4
#define BUS_FREQ_06MHZ  8
#define BUS_FREQ_03MHZ  16

/* BUS_WIDTH_OPTION */
#define BUS_WIDTH_1BIT  0
#define BUS_WIDTH_4BIT  1
#define BUS_WIDTH_8BIT  2

/* BUS_VOLTAGE_OPTION */
#define BUS_VOLTAGE_33  7	/* 3.3 volts */
#define BUS_VOLTAGE_30  6	/* 3.0 volts */
#define BUS_VOLTAGE_18  5	/* 1.8 volts */

/* ARASAN IP Specification */
#define ARASAN_SDMA_MAX_BLKS	512
#define EMMC_BLOCK_SIZE		512
#define EMMC_BLOCK_SHIFT	9	/* 512Byte : 9bit */

/* Interrupt bit defines */
#define HOST_INT_STATUS_ALL 0xffffffff
/*
 * Enable the following interrupt bits:
 * [28] - TARGET_RESP_ERR_INT
 * [25] - ADMA_ERR_INT
 * [24] - AUTO_CMD_ERR_INT
 * [23] - CURRENT_LIMIT_ERR_INT
 * [22] - CURRENT_LIMIT_ERR_INT
 * [21] - DATA_CRC_ERR_INT
 * [20] - DATA_TIMEOUT_ERR_INT
 * [19] - CMD_INDEX_ERR_INT
 * [18] - CMD_END_BIT_ERR_INT
 * [17] - CMD_CRC_ERR_INT
 * [16] - CMD_TIMEOUT_ERR_INT
 * [10] - INT_B
 * [9] - INT_A
 * [5] - BUFFER_READ_INT
 * [4] - BUFFER_WRITE_INT
 * [3] - DMA_INT
 * [2] - BLOCK_GAP_INT
 * [1] - TRANSFER_COMPLETE_INT
 * [0] - COMMAND_COMPLETE_INT
 * Disable the following interrupt bits:
 * [26] - TUNE_ERR_STAT_EN
 * [14] - BOOT_TERM_EN
 * [13] - BOOT_ACK_RCV_EN
 * [12] - RETUNE_EVENT_EN
 * [11] - INT_C_EN
 * [08] - CARD_INT_ENA
 * [07] - CAR_REMOVAL_INT_ENA
 * [06] - CAR_INSERT_INT_ENA
 */
#define HOST_INT_ENABLES	0x13FF0607
#define HOST_INT_ENABLES_DMA	0x13FF060F
#define HOST_INT_ENABLES_PIO	0x13FF0637
/* FSBL eMMC driver is slightly different */
#define HOST_INT_ENABLES_FSBL	0x13FF063F

#endif /*_EMMC_CORE_DEFS_H  */
