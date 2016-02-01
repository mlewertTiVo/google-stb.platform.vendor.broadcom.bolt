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

#include "lib_types.h"
#include "lib_printf.h"
#include "lib_malloc.h"
#include "flash-partitions.h"
#include "flash.h"
#include "board.h"
#include "board_init.h"
#include "common.h"

/* Maximum supported flash devices */
#define MAX_FLASH_DEVS 6

/* All registered flash devices */
static struct flash_dev *flashdevs[MAX_FLASH_DEVS];
/* Number of registered flash devices */
static int num_flash;
/* The index of the boot flash device */
static int boot_flash_idx = -1;

#define for_each_partition_profile(table, profile) \
	for ((profile) = (table); (profile)->min_size != 0; (profile)++)

int get_num_flash()
{
	return num_flash;
}

struct flash_dev *__get_flash(int idx)
{
	return flashdevs[idx];
}

struct flash_dev *get_boot_flash(void)
{
	return boot_flash_idx < 0 ? NULL : flashdevs[boot_flash_idx];
}

/*
 * Maximum number of bad blocks expected per 1024 blocks
 * Only for NAND, since NOR doesn't have bad blocks
 */
#define BAD_BLOCK_LIMIT 20

/**
 * Register a single partition as a device
 *
 * @name: The device name for this partition (e.g., 'kernel' or 'bolt')
 * @part: The flash partition structure, for geometry and flash info
 *
 * Returns 0 on success
 */
static int flash_register_partition(const char *name,
		struct flash_partition *part)
{
	struct flash_dev *flash = part->flash;
	bolt_driver_t *drv = flash->driver;
	uint64_t size = part->size;
	char descr[80];
	char *x = descr;

	x += xsprintf(descr, "%s @ CS%d: 0x%08llX-0x%08llX ",
			drv->drv_description, flash->cs,
			part->offset, part->offset + size);
	if (size > 1024 * 1024 * 2)
		x += xsprintf(x, "(%lluMB)", (size + (1024 * 1024 - 1)) >> 20);
	else if (size > 1024)
		x += xsprintf(x, "(%lluKB)", (size + (1024 - 1)) >> 10);
	else
		x += xsprintf(x, "(%lluB)", size);

	part->name = bolt_attach_idx(drv, flash->cs, part, name, descr);
	return !part->name;
}

#define ALIGN_DOWN_TO_BLOCK(flash, size) \
	ALIGN_TO(size, (flash)->blocksize)

#define ALIGN_UP_TO_BLOCK(flash, size) \
	ALIGN_UP_TO(size, (flash)->blocksize)

static uint32_t flashpart_calc_spare(struct flash_dev *flash,
		uint64_t size)
{
	uint32_t spare;

	if (!flash_can_have_bb(flash))
		return 0;

	spare = (size * BAD_BLOCK_LIMIT + 1023) / 1024;

	/* Align to block size */
	return ALIGN_UP_TO_BLOCK(flash, spare);
}

/*
 * Assign a partition's spare size, aligning both the size and spare size to
 * its block size. If max != 0, then also limit the size to a hard maximum.
 */
static void flashpart_calc(struct flash_partition *part, uint64_t max)
{
	struct flash_dev *flash = part->flash;
	uint64_t size = ALIGN_UP_TO_BLOCK(flash, part->size);

	if (flash_can_have_bb(flash))
		part->sparesize = flashpart_calc_spare(flash, size);
	else
		part->sparesize = 0;

	part->size = size + part->sparesize;
	if (max && part->size > max)
		part->size = max;
}

/**
 * Register a new partition which covers the whole flash
 * @flash: the flash device to add a partition to
 * @return 0 on success
 */
static int flash_register_full_partition(struct flash_dev *flash)
{
	struct flash_partition *part = &flash->parts[flash->nparts];

	part->offset = 0;
	part->size = flash->size;
	part->flash = flash;
	flashpart_calc(part, flash->size);
	part->size = flash->size;
	flash_register_partition(NULL, part);
	flash->nparts++;

	return 0;
}

static int flash_is_boot_device(struct flash_dev *flash)
{
	switch (flash->type) {
	case FLASH_TYPE_NOR:
	case FLASH_TYPE_SPI:
	case FLASH_TYPE_NAND:
		return board_bootmode() == flash->type && flash->cs == 0;
	default:
		return 0; /* Don't (yet) support eMMC boot? */
	}
}

int flash_register_device(bolt_driver_t *drv, struct flash_dev *flash)
{
	if (num_flash == MAX_FLASH_DEVS) {
		err_msg("flash: too many flash devices");
		return -1;
	}

	if (flash_is_boot_device(flash))
		boot_flash_idx = num_flash; /* This flash is the boot dev */

	/* Stash the flash device */
	flashdevs[num_flash++] = flash;

	flash->driver = drv;

	return 0;
}

/**
 * The minimum partition size for a profile, including spare (for NAND bad
 * blocks)
 */
static uint64_t profile_min_size(struct flash_dev *flash,
		struct partition_profile *profile)
{
	uint64_t size = ALIGN_UP_TO_BLOCK(flash, profile->min_size);

	/* PARTITION_FIXED allocates no additional spare */
	if (profile->flags & PARTITION_FIXED)
		return size;

	return size + flashpart_calc_spare(flash, size);
}

/**
 * Return non-zero if this partition profile entry must be placed on this
 * flash device
 */
static int flash_must_match_profile(struct flash_dev *flash,
		struct partition_profile *profile)
{
	/* All partitions must go on the only flash */
	if (get_num_flash() == 1)
		return 1;

	/* Assign to the boot flash */
	if ((profile->flags & PARTITION_BOOT_DEVICE)
			&& flash == get_boot_flash())
		return 1;

	/* If we only have a single secondary */
	if (get_num_flash() == 2 && (profile->flags & PARTITION_SECONDARY) &&
			flash != get_boot_flash())
		return 1;

	/* The fixed mappings */
	if (profile->flags & PARTITION_CS0)
		return flash->cs == 0;
	if (profile->flags & PARTITION_CS1)
		return flash->cs == 1;
	if (profile->flags & PARTITION_CS2)
		return flash->cs == 2;
	if (profile->flags & PARTITION_CS3)
		return flash->cs == 3;

	return 0;
}

/**
 * Fit a partition profile into a flash device
 *
 * @profile: the partition profile to fit
 * @remaining: the remaining space in each flash, indexed by flash_idx
 * @flash_idx: the index of the flash to fit into
 *
 * @return The minimum size of the partition, if it fits; otherwise 0
 */
static uint64_t flash_test_fit_partition(struct partition_profile *profile,
		uint64_t *remaining, int flash_idx)
{
	struct flash_dev *flash = __get_flash(flash_idx);
	uint64_t minsize = profile_min_size(flash, profile);
	if (minsize > remaining[flash_idx])
		return 0;

	remaining[flash_idx] -= minsize;

	return minsize;
}

/**
 * Calculate and assign the partition size, given the remaining device space
 * and grow-able parameters. This function will set profile->min_size to the
 * final size of the partition.
 *
 * @profile: the partition profile to use
 * @flash: the flash device profile is assigned to
 * @remaining: the space left to assign to this flash
 * @total_grow_to: the maximum grow-able scale factor for this flash. This is
 *   used to scale a partition's 'grow_to' number to the entire flash. Each
 *   growable partition will be scaled so that the sum of all partitions fills
 *   the flash, and that we approximately retain the relative sizing of each
 *   partition. An overprovisioned flash (where percentages add to more than
 *   100%) will be scaled to fit.
 *
 * @return The amount by which we expanded (over the base min_size + spare)
 */
static uint64_t flash_partition_assign_size(struct partition_profile *profile,
		struct flash_dev *flash, uint64_t remaining,
		uint64_t grow_spare, int total_grow_to)
{
	uint64_t grow;

	profile->min_size = profile_min_size(flash, profile);
	if (!total_grow_to || !profile->grow_to)
		return 0;

	grow = profile->grow_to * grow_spare / total_grow_to;
	grow = ALIGN_UP_TO_BLOCK(flash, grow);

	/* Ensure partition (+ mirror) fit in remaining */
	if (profile->flags & PARTITION_MIRROR)
		grow *= 2;
	grow = min(grow, remaining);
	if (profile->flags & PARTITION_MIRROR)
		grow = ALIGN_DOWN_TO_BLOCK(flash, grow / 2);

	profile->min_size += grow;

	return grow << ((profile->flags & PARTITION_MIRROR) ? 1 : 0);
}

/**
 * Register a partition and its mirror (if applicable)
 *
 * @profile: The partition profile to handle; profile->min_size is now used as
 *   its _maximum_ size; if this profile has the PARTITION_MIRROR flag, this
 *   will register two partitions
 * @flash: The flash device on which to register this partition
 * @offset: The offset within the flash at which to register
 *
 * @return an offset immediately after the registered partition(s)
 */
uint64_t flash_register_profile(struct partition_profile *profile,
		struct flash_dev *flash, uint64_t offset)
{
	int i;

	for (i = 0; i < ((profile->flags & PARTITION_MIRROR) ? 2 : 1); i++) {
		char str[40];
		const char *name = profile->name;
		struct flash_partition *part = &flash->parts[flash->nparts];

		part->offset = offset;
		part->size = profile->min_size;
		part->flash = flash;
		flashpart_calc(part, part->size);
		if (profile->flags & PARTITION_MIRROR) {
			xsprintf(str, "%s%d", name, i);
			name = str;
		}
		flash_register_partition(name, part);
		flash->nparts++;
		offset += part->size;
	}

	return offset;
}

/**
 * Layout the partition map--by assigning each partition to a flash and
 * calculating its size--according to a given partition profile table. The
 * essential highlights of the layout algorithm:
 *
 *   - Fixed partitions (with PARTITION_CS[0-3], PARTITION_EMMC_{BOOT,DATA}
 *     will always be assigned exactly to the specific flash
 *   - "Class"-assigned partitions (PARTITION_{BOOT_DEVICE,SECONDARY}) will
 *     be placed on their intended flash type (e.g., the secondary flash),
 *     if available. On a single-flash system, they will all be placed on
 *     the boot flash.
 *   - Other devices will be assigned as space allows, with the boot flash
 *     given priority
 *   - PARTITION_MIRROR: a mirrored partition will be duplicated (e.g., as
 *     'rootfs0' and 'rootfs1') if there is available space on the assigned
 *     flash
 *   - partition_profile.grow_to: used to scale a partiton to use an
 *     approximate percentage of the unused space on a flash. We
 *     automatically scale accordingly if, for example, two partitions are
 *     both provisioned for 100% of the remaining space.
 *   - PARTITION_FIXED: a partition will not grow at all, even to fit spare
 *     blocks (only affects NAND)
 *
 * @table: The partition profile table
 * @flash_nums: An array of length num_parts; will contain a mapping from
 *   partitions to the index of the flash to which they are assigned
 * @num_parts: The number of partition profiles in table
 *
 * Output: the resulting size is assigned to each partition profile's
 *   'min_size' entry. The flash assignments are given in 'flash_nums'.
 *
 * @return 0 on success; otherwise nonzero
 */
static int flash_layout_partitions(struct partition_profile *table,
		int *flash_nums, int num_parts)
{
	struct flash_dev *flash;
	struct partition_profile *profile;
	uint64_t remaining[MAX_FLASH_DEVS];
	uint64_t grow_spare[MAX_FLASH_DEVS]; /* leftover, for growing into */
	int *grow_to; /* Total 'grow_to' for each flash idx */
	int i, j;

	/* Initialize the "remaining" for each flash */
	for_each_flash_dev(flash, i)
		remaining[i] = flash->size;

	/* Assign devices, allocated space for all fixed mappings */
	for (i = 0; i < num_parts; i++) {
		int flash_idx = -1;
		profile = &table[i];
		for_each_flash_dev(flash, j) {
			if (flash_must_match_profile(flash, profile)) {
				flash_idx = j;
				break;
			}
		}
		flash_nums[i] = flash_idx;

		/* Skip non-fixed partitions */
		if (flash_idx < 0)
			continue;

		if (!flash_test_fit_partition(profile, remaining, flash_idx)) {
			err_msg("no space on flash %d (partition '%s'): please "
					"repartition flash",
					flash_idx, profile->name);
			return -1;
		}
	}

	/* Allocate space for non-fixed mappings */
	for (i = 0; i < num_parts; i++) {
		profile = &table[i];

		/* Skip assigned partitions */
		if (flash_nums[i] >= 0)
			continue;

		/* Try to place on boot device first */
		if (!(profile->flags & PARTITION_SECONDARY)) {
			if (flash_test_fit_partition(profile, remaining,
						boot_flash_idx)) {
				flash_nums[i] = boot_flash_idx;
				continue;
			}
		}

		/* Try non-boot devices, in order of registration */
		/* coverity[returned_pointer] */
		for_each_flash_dev(flash, j) {
			if (j == boot_flash_idx)
				continue;
			if (flash_test_fit_partition(profile, remaining, j)) {
				flash_nums[i] = j;
				break;
			}
		}

		/* Never found a device? */
		if (flash_nums[i] < 0) {
			err_msg("warning: no space left for partition '%s'",
					profile->name);
		}
	}

	/* Allocate mirrored partitions */
	for (i = 0; i < num_parts; i++) {
		if ((table[i].flags & PARTITION_MIRROR) && flash_nums[i] >= 0)
			if (!flash_test_fit_partition(&table[i], remaining,
						flash_nums[i]))
				/* Clear the MIRROR flag if it didn't fit */
				table[i].flags &= ~PARTITION_MIRROR;
	}

	grow_to = KMALLOC(get_num_flash() * sizeof(*grow_to), 0);
	memset(grow_to, 0, get_num_flash() * sizeof(*grow_to));

	/* Total up the 'grow_to' values */
	for (i = 0; i < num_parts; i++) {
		if (flash_nums[i] < 0)
			continue;
		grow_to[flash_nums[i]] += table[i].grow_to;
		/* Add again for mirrored */
		if (table[i].flags & PARTITION_MIRROR)
			grow_to[flash_nums[i]] += table[i].grow_to;
	}

	/* Retain snapshot of leftover space before "growing" */
	for (i = 0; i < (int)ARRAY_SIZE(remaining); i++)
		grow_spare[i] = remaining[i];

	/* Grow partitions to the flash */
	for (i = 0; i < num_parts; i++) {
		int flash_idx = flash_nums[i];
		if (flash_idx < 0)
			continue;
		flash = __get_flash(flash_idx);
		remaining[flash_idx] -= flash_partition_assign_size(&table[i],
				flash,
				remaining[flash_idx],
				grow_spare[flash_idx],
				grow_to[flash_idx]);
	}

	KFREE(grow_to);
	return 0;
}

/**
 * Determine the size and placement of partitions, then register them, all
 * according to the set of partition profiles provided
 *
 * Returns 0 on success
 */
static int flash_configure_partitions(struct partition_profile *table)
{
	struct partition_profile *profile;
	struct flash_dev *flash;
	int *flash_nums; /* Map partition profile to flash idx */
	int num_parts = 0, ret, i, j;

	if (table == NULL)
		return 0;

	/* Count the number of partition profiles */
	for_each_partition_profile(table, profile)
		num_parts++;

	flash_nums = KMALLOC(num_parts * sizeof(*flash_nums), 0);

	ret = flash_layout_partitions(table, flash_nums, num_parts);
	if (ret)
		goto out;

	/* Layout offsets, register partitions */
	for_each_flash_dev(flash, i) {
		uint64_t offset = 0;
		for (j = 0; j < num_parts; j++) {
			profile = &table[j];
			if (flash_nums[j] == i) {
				offset = flash_register_profile(profile,
						flash, offset);
			}
		}

		/* Add a whole-device partition */
		flash_register_full_partition(flash);
	}

out:
	KFREE(flash_nums);

	return ret;
}

/**
 * To be called after all flash driver registration, to configure partitioning
 * and populate devices.
 *
 * Return 0 if all flash configuration was successful
 */
int flash_configure_finalize(void)
{
	/*
	 * eMMC uses a fixed partition map based on the GPT table on the
	 * device so don't do dynamic layout.
	 */
	if (board_bootmode() == BOOT_FROM_EMMC)
		return 0;

	return flash_configure_partitions(board_flash_partition_table());
}
