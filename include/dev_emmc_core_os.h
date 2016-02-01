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

#ifndef __DEV_EMMC_CORE_OS_H__
#define __DEV_EMMC_CORE_OS_H__

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "common.h"
#include "timer.h"
#include "cache_ops.h"
#include "bsp_config.h"
#include "byteorder.h"

#define HOST_REG_RD(regs, offset)				\
	(BDEV_RD((regs)->reg_bases[EMMC_REG_BLK_BASE_HOST] +	\
			SDHC_REG_##offset))
#define HOST_REG_WR(regs, offset, value)			\
	BDEV_WR(((regs)->reg_bases[EMMC_REG_BLK_BASE_HOST] +	\
			SDHC_REG_##offset), value)
#define BOOT_REG_RD(regs, offset)				\
	(BDEV_RD((regs)->reg_bases[EMMC_REG_BLK_BASE_BOOT] +	\
		SDIO_BOOT_REG_##offset))
#define BOOT_REG_WR(regs, offset, value)			\
	BDEV_WR((regs)->reg_bases[EMMC_REG_BLK_BASE_BOOT] +	\
		SDIO_BOOT_REG_##offset, value)

#define os_msleep bolt_msleep
#define os_usleep bolt_usleep
#define os_printf xprintf

#endif /*__DEV_EMMC_CORE_OS_H__ */
