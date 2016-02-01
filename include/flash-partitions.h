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

#ifndef __FLASH_PARTITIONS_H__
#define __FLASH_PARTITIONS_H__

#include "lib_types.h"

/**
 * Partition profile flags
 *
 * These flags can be provided as additional suggestions for partition
 * placement, layout, etc. In the absence of flags, BOLT may place the
 * partition on any available flash device.
 */
enum partition_profile_flag {
	/* For placing on a fixed flash device */
	PARTITION_CS0		= (1 << 0),  /* Place on CS0 flash */
	PARTITION_CS1		= (1 << 1),  /* Place on CS1 flash */
	PARTITION_CS2		= (1 << 2),  /* Place on CS2 flash */
	PARTITION_CS3		= (1 << 3),  /* Place on CS3 flash */
	PARTITION_EMMC_BOOT	= (1 << 4),  /* Place on eMMC Boot[01] */
	PARTITION_EMMC_DATA	= (1 << 5),  /* Place on eMMC User */

	/* For placing on a class of device */
	PARTITION_BOOT_DEVICE	= (1 << 8),  /* Place on boot device */
	PARTITION_SECONDARY	= (1 << 9),  /* Place on non-boot device */

	/* Miscellaneous options */
	PARTITION_MIRROR	= (1 << 10), /* Duplicate, adding suffix 0, 1 */
	PARTITION_FIXED		= (1 << 11), /* Hard cap on size */
};

/* Allow "percentage" precision of up to (1 / x) */
#define PARTITION_GROWTH_PRECISION	512

/*
 * Converts a percentage to a 'grow_to' number; see partition_profile.grow_to.
 * The percentage may be either a floating point or an integer constant.
 */
#define GROW_TO_PERCENT(percent) \
	(int)(((percent) * PARTITION_GROWTH_PRECISION + 99) / 100)

/**
 * A partition profile provides a superset of the functionality of a fixed
 * flash map. A profile provides flexibility information, so that a single
 * profile can be applied to various flash configurations. For instance, a
 * rootfs partition might be small on a board with a single, small SPI NOR
 * flash but might expand and provide mirrored (ping-pong) partitions on a
 * large NAND flash.
 *
 * To achieve fixed partitions, use the FIXED_PARTITION macro
 *
 * For more information on the partition layout algorithm, see
 * flash_layout_partitions()
 */
struct partition_profile {
	/* Partition name */
	const char *name;

	/* Minimum partition size -- 0 is a table terminator */
	uint64_t min_size;

	/*
	 * Partition may grow to a fraction of the total device size. See
	 * GROW_TO_PERCENT().
	 * 0 means partition will not grow (that is, fixed-size partition)
	 */
	int grow_to;

	/* See enum partition_profile_flag */
	uint32_t flags;
};

/**
 * Macro for a fixed partition map
 *
 * @_name: partition name
 * @_size: partition size
 * @_device: flash device: CS0, CS1, CS2, or CS3
 */
#define FIXED_PARTITION(_name, _size, _device) { \
	.name		= (_name), \
	.min_size	= (_size), \
	.grow_to	= 0, \
	.flags		= PARTITION_FIXED | PARTITION_##_device, \
}

/** Macro for placing a growable partition within an otherwise-fixed map */
#define GROWABLE_PARTITION(_name, _device) { \
	.name		= (_name), \
	.min_size	= 1, \
	.grow_to	= GROW_TO_PERCENT(100), \
	.flags		= PARTITION_##_device, \
}

#endif /* __FLASH_PARTITIONS_H__ */
