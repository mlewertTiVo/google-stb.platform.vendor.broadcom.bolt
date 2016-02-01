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

#ifndef __FLASH_H__
#define __FLASH_H__

#include "lib_types.h"
#include "flash-partitions.h"
#include "device.h"
#include "ioctl.h"

/**
 * Flash partition structure
 *
 * This structure gets registered as device data to represent the high-level
 * flash information. It presents a pointer to the (private) low-level driver
 * in the priv field.
 */
struct flash_partition {
	uint64_t size;
	uint64_t offset;
	uint32_t sparesize;
	const char *name;

	struct flash_dev *flash;
};

#define FLASH_MAX_PARTITIONS 32

/**
 * Flash device structure
 */
struct flash_dev {
	/* Total device size */
	uint64_t size;

	/* Typical eraseblock size */
	uint32_t blocksize;

	/* Minimum write size */
	uint32_t writesize;

	/* The flash type (FLASH_TYPE_xxx) */
	enum flash_type type;

	/* Chip select */
	int cs;

	/* Number of bits corrected per ECC step (0 for non-ECC flash) */
	int ecc_strength;

	/* ECC step size (e.g., 512, 1024 bytes) */
	int ecc_step;

	/* Number of partitions */
	int nparts;

	struct flash_partition parts[FLASH_MAX_PARTITIONS];

	/* Private driver data */
	void *priv;

	bolt_driver_t *driver;
};

int flash_register_device(bolt_driver_t *drv, struct flash_dev *flash);

int flash_configure_finalize(void);

static inline void *flashpart_get_drvdata(struct flash_partition *part)
{
	return part->flash->priv;
}

/* Can this flash have bad blocks? */
static inline int flash_can_have_bb(struct flash_dev *flash)
{
	return flash->type == FLASH_TYPE_NAND;
}

int get_num_flash();
struct flash_dev *__get_flash(int idx);
#define for_each_flash_dev(flash, iter) \
	for (iter = 0, flash = __get_flash(iter); \
			iter < get_num_flash(); \
			iter++, flash = __get_flash(iter))

struct flash_dev *get_boot_flash(void);

#endif /* __FLASH_H__ */
