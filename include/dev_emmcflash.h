/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEV_EMMCFLASH_H__
#define __DEV_EMMCFLASH_H__

#include "flash.h"
#include "dev_emmc_core.h"
#include "ssbl-common.h"

/* One per hard partition */
struct emmc_device {
	struct emmc_chip *chip;
	uint64_t size;
	int id;
	int dirty;                      /* part has been written since open */
	struct flash_dev flash;		/* soft partitions info */
	int flags;
#define EMMC_DEVICE_FLAGS_RPMB 0x00000001	/* RPMB partition */
	int blocksize;			/* 512 for data parts, 256 for RPMB */
};

void emmc_init_pinmux(int id, sdio_type_e type);

#endif /*__DEV_EMMCFLASH_H__ */
