/***************************************************************************
 *     Copyright (c) 2015 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEV_EMMC_DEBUG_H__
#define __DEV_EMMC_DEBUG_H__

/* Basic Debugging */
#define DEBUG_EMMC_CMD           0	/* eMMC command */
#define DEBUG_EMMC_STATUS        0	/* card status */
#define DEBUG_EMMC_CONFIG        0	/* eMMC configuration data */
#define DEBUG_EMMC_DRV           0	/* driver code */
#define DEBUG_EMMC_DRV_DISPATCH  0	/* driver dispatch functions */
#define DEBUG_EMMC_PART          0	/* Partition config */

/*
 * Define debug print macros
 */
#if DEBUG_EMMC_DRV
#define DBG_MSG_DRV(...) os_printf(__VA_ARGS__)
#else
#define DBG_MSG_DRV(...)
#endif
#if DEBUG_EMMC_DRV_DISPATCH
#define DBG_MSG_DRV_DISPATCH(...) os_printf(__VA_ARGS__)
#else
#define DBG_MSG_DRV_DISPATCH(...)
#endif
#if DEBUG_EMMC_PART
#define DBG_MSG_PART(...) os_printf(__VA_ARGS__)
#else
#define DBG_MSG_PART(...)
#endif
#if DEBUG_EMMC_CMD
#define DBG_MSG_CMD(...) os_printf(__VA_ARGS__)
#else
#define DBG_MSG_CMD(...)
#endif

#endif /* __DEV_EMMC_DEBUG_H__ */
