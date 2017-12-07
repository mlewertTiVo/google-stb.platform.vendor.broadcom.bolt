/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bchp_common.h>
#include <bchp_nand.h>
#include <bchp_sun_top_ctrl.h>

#include "fsbl.h"
#include <nand_chips.h>
#include "nand-common.h"


#ifndef CFG_EMULATION

enum fsbl_flash_type {
	FLASH_TYPE_NONE		= 0,
	/* SPI NOR, P-NOR, and eMMC are pretty similar w.r.t. FSBL */
	FLASH_TYPE_NORMAL,
	FLASH_TYPE_NAND,
};

struct fsbl_flash {
	int cs;
	enum fsbl_flash_type type;
	uint32_t page_size;
	uint32_t block_size;
	uint64_t size;
};

/* Only support chip-select 0 */
#define FSBL_FLASH_NUM_CS	1

static struct fsbl_flash flash_list[FSBL_FLASH_NUM_CS];

static inline struct fsbl_flash *fsbl_get_flash(int cs)
{
	if (cs < 0 || cs >= FSBL_FLASH_NUM_CS)
		return NULL;
	return &flash_list[cs];
}

#define NAND_FC_SIZE		512
#define NAND_CMD_PAGE_READ	0x1
#define NAND_CMD_READ_ID	0x7

#if NAND_CONTROLLER_REVISION < 0x701
/* NAND controller 7.0 or lower */

static uint32_t get_block_size_index(void)
{
	return BDEV_RD_F(NAND_CONFIG_CS0, BLOCK_SIZE);
}

static uint32_t get_page_size_index(void)
{
	return BDEV_RD_F(NAND_CONFIG_CS0, PAGE_SIZE);
}

#else
/* NAND controller 7.1 or higher */

static uint32_t get_block_size_index(void)
{
	return BDEV_RD_F(NAND_CONFIG_EXT_CS0, BLOCK_SIZE);
}

static uint32_t get_page_size_index(void)
{
	return BDEV_RD_F(NAND_CONFIG_EXT_CS0, PAGE_SIZE);
}

#endif

/**
 * Convert an offset address on the boot flash to a memory-mapped virtual
 * address
 */
static inline void *flash_offs_to_va(uint32_t offs)
{
	return (void *)BOLT_TEXT_ADDR + offs;
}

static void do_nand_cmd(uint32_t cmd, uint32_t addr)
{
#if NAND_CONTROLLER_REVISION >= 0x703
	BDEV_WR64(BCHP_NAND_CMD_ADDRESS, addr);
#else
	BDEV_WR(BCHP_NAND_CMD_EXT_ADDRESS, 0);
	BDEV_WR(BCHP_NAND_CMD_ADDRESS, addr);
#endif
	BDEV_WR_F(NAND_CMD_START, OPCODE, cmd);

	while (!BDEV_RD_F(NAND_INTFC_STATUS, CTLR_READY))
		;
}

/**
 * Identify NAND flash chip, returning chip index
 */
static int identify_nand(struct fsbl_flash *flash)
{
	int i, j;
	uint32_t id0, id1;
	const struct nand_chip *chip;

	do_nand_cmd(NAND_CMD_READ_ID, 0);

	id0 = BDEV_RD(BCHP_NAND_FLASH_DEVICE_ID);
	id1 = BDEV_RD(BCHP_NAND_FLASH_DEVICE_ID_EXT);

	for (i = 0; i < (int)ARRAY_SIZE(known_nand_chips); i++) {
		chip = &known_nand_chips[i];
		if (((id0 & chip->id_mask[0]) == chip->id_val[0]) &&
		    ((id1 & chip->id_mask[1]) == chip->id_val[1])) {
			flash->block_size = chip->block_size;
			flash->page_size = chip->page_size;
			flash->size = chip->size;
			return i;
		}
	}

	i = get_block_size_index();
	j = get_page_size_index();
	if (BDEV_RD_F(NAND_ONFI_STATUS, ONFI_detected) &&
			i < (int)ARRAY_SIZE(nand_block_sizes) &&
			j < (int)ARRAY_SIZE(nand_page_sizes)) {
		flash->block_size = nand_block_sizes[i];
		flash->page_size = nand_page_sizes[j];
		/* FIXME: device size is not auto-initialized */
		return NAND_CHIP_ONFI;
	}

	report_hex("@NAND ID: ", id0);
	report_hex(" ", id1);

	return NAND_CHIP_NONE;
}

/* Must be called before any flash copy functions */
void get_flash_info(struct fsbl_info *info)
{
	int i;
	uint32_t boot_shape;
	struct fsbl_flash *flash = fsbl_get_flash(0); /* CS0 */

	info->nand_chip_idx = NAND_CHIP_NONE;
	/* Assume non-NAND flash until proven otherwise */
	flash->type = FLASH_TYPE_NORMAL;

	/* Disable direct addressing + XOR for all NAND CS */
	BDEV_UNSET(BCHP_NAND_CS_NAND_SELECT, 0xff);
	BDEV_UNSET(BCHP_NAND_CS_NAND_XOR, 0xff);

	/*
	 * clear the "page valid" signal that might have been set by BSP
	 * See: CRNAND-46
	 */
	BDEV_WR_F(NAND_ACC_CONTROL_CS0, PAGE_HIT_EN, 0);
	BDEV_WR_F(NAND_ACC_CONTROL_CS0, PAGE_HIT_EN, 1);

	boot_shape = BDEV_RD_F(SUN_TOP_CTRL_STRAP_VALUE_0, strap_boot_shape);
	for (i = 0; ; i++) {
		const struct boot_shape_mask *sh;

		/* no match?  not NAND. */
		if (i == (int)ARRAY_SIZE(nand_boot_shapes))
			return;

		sh = &nand_boot_shapes[i];
		if ((boot_shape & sh->mask) == sh->val)
			break;
	}

	info->nand_chip_idx = identify_nand(flash);
	if (info->nand_chip_idx == NAND_CHIP_NONE)
		sys_die(DIE_UNRECOGNIZED_NAND_CHIP,
			"unrecognized NAND chip");
	else
		flash->type = FLASH_TYPE_NAND;
}


static int nand_check_page(uint32_t page_size, uint32_t addr)
{
	do_nand_cmd(NAND_CMD_PAGE_READ, addr);
	return (BDEV_RD(BCHP_NAND_SPARE_AREA_READ_OFS_0) &
		BCHP_NAND_SPARE_AREA_READ_OFS_0_BYTE_OFS_0_MASK) !=
		BCHP_NAND_SPARE_AREA_READ_OFS_0_BYTE_OFS_0_MASK;
}


/* Check first byte of spare area in first two and last pages */
static int nand_block_is_bad(struct fsbl_flash *flash, uint32_t addr)
{
	uint32_t page_size = flash->page_size;
	uint32_t block_size = flash->block_size;

	return nand_check_page(page_size, addr) ||
		nand_check_page(page_size, addr + page_size) ||
		nand_check_page(page_size, addr + block_size - page_size);
}

/**
 * Read (some portion of) a single NAND page into a buffer
 *
 * @flash: the flash to use
 * @addr: the flash address to read from; must be 4-byte aligned
 * @dst: the buffer to write to
 * @len: the length of the data to read from flash; must be 4-byte aligned and
 *       less than @page_size
 */
static void nand_copy_page(struct fsbl_flash *flash, uint32_t addr,
			   uint32_t *dst, int len)
{
	uint32_t page_size = flash->page_size;
	uint32_t page_addr = ALIGN_TO(addr, page_size);
	uint32_t offs = addr & (page_size - 1);
	unsigned int i, j;

	/* Clear ECC errors */
	BDEV_WR(BCHP_NAND_ECC_UNC_ADDR, 0);
	for (i = 0; i < page_size; i += NAND_FC_SIZE) {
		uint32_t eaddr;

		do_nand_cmd(NAND_CMD_PAGE_READ, page_addr + i);
		eaddr = BDEV_RD(BCHP_NAND_ECC_UNC_ADDR);
		if (eaddr) {
			report_hex("NAND: uncorrectable error @ ", eaddr);
			/* FIXME: should we just die on errors? */
			sys_die(DIE_NAND_FLASH_IS_CORRUPT,
				"NAND flash is corrupt");
		}
		for (j = 0; j < NAND_FC_SIZE; j += 4)
			if (i + j < offs + len && i + j >= offs)
				*(dst++) = BDEV_RD(BCHP_NAND_FLASH_CACHEi_ARRAY_BASE + j);
	}
}

/*
 * Translate a partition + offset into a raw flash address, accounting for bad
 * blocks. Because we don't cache results or use a bad block table, we may
 * re-check the same block many times. But this should be negligible.
 */
static int get_nand_offset(uint32_t *ret_offs, struct fsbl_flash *flash,
			   struct fsbl_flash_partition *part,
			   uint32_t text_offs)
{
	uint32_t block_size, blk_mask;
	uint32_t check_offs = 0; /* keep track of bad block shifting */

	block_size = flash->block_size;
	blk_mask = block_size - 1;

	if (part->part_offs & blk_mask) {
		puts("bad partition alignment");
		return -1;
	}

	while (check_offs <= ALIGN_TO(text_offs, block_size)) {
		if (part->part_size && check_offs >= part->part_size) {
			report_hex("out of good blocks in partition @ ",
				part->part_offs);
			return -1;
		}
		if (nand_block_is_bad(flash, part->part_offs + check_offs)) {
			report_hex("Skipping bad block ",
				part->part_offs + check_offs);
			text_offs += block_size;
		}
		check_offs += block_size;
	}

	*ret_offs = part->part_offs + text_offs;
	return 0;
}

/*
 * Translates an image offset into an offset on the flash, accounting for bad
 * blocks. All offsets are 0-based (i.e., not based on the EBI address
 * mapping).
 *
 * The offset returned by this function is only guaranteed to be valid within a
 * 128KB-aligned region (the minimum NAND eraseblock size). A caller should
 * call this function every time it crosses a 128KB boundary.
 *
 * @ret_offs: a pass-by-reference return pointer, where we store the flash
 *      offset
 * @text_offs: the image offset within the partition (assuming no bad blocks)
 * @part: flash partition information (chip-select, offset, ...); currently
 *      supports only chip-select 0.
 *
 * Return the adjusted flash offset, relative to the beginning of the flash
 *
 * ***WARNING*** we don't currently check that the partition doesn't extend
 * beyond the end of the flash
 *
 * Returns 0 on success, negative on error (or sys_die()'s)
 */
int get_flash_offset(uint32_t *ret_offs, uint32_t text_offs,
		     struct fsbl_flash_partition *part)
{
	struct fsbl_flash *flash;

	if (!part)
		return -1;

	flash = fsbl_get_flash(part->cs);
	if (!flash)
		sys_die(DIE_CHIP_SELECT_NOT_SUPPORTED,
			"chip-select not supported");

	if (flash->type == FLASH_TYPE_NAND)
		return get_nand_offset(ret_offs, flash, part, text_offs);

	*ret_offs = part->part_offs + text_offs;
	return 0;
}

/**
 * Copy data from NAND flash to memory
 *
 * @flash: the flash to use
 * @part: flash partition info
 * @dst: destination buffer address
 * @flash_offs: offset within the partition to read from; must be 4-byte
 *     aligned
 * @len: length of the data to copy; must be 4-byte aligned
 */
static int copy_from_nand(struct fsbl_flash *flash,
			  struct fsbl_flash_partition *part,
			  uint32_t *dst, uint32_t flash_offs, unsigned int len)
{
	unsigned int i;
	uint32_t page_size = flash->page_size, block_size = flash->block_size;
	uint32_t blk_mask = block_size - 1;
	uint32_t addr;

	while (len > 0) {
		/* Bad block skipping */
		if (get_nand_offset(&addr, flash, part, flash_offs))
			return -1;

		if (addr & 0x03 || len & 0x03)
			sys_die(DIE_UNALIGNED_ACCESS_TO_NAND_FLASH,
				"unaligned access to NAND flash");

		/* Copy the current block */
		for (i = addr & blk_mask; i < block_size && len > 0;
				i += page_size) {
			unsigned int copy = min(len, page_size - (addr & (page_size - 1)));
			nand_copy_page(flash, addr, dst, copy);
			addr += copy;
			flash_offs += copy;
			dst += copy / 4;
			len -= copy;
		}
	}

	return 0;
}

/**
 * Load from flash
 *
 * @dst: destination buffer address
 * @flash_offs: offset within flash to read from; must be 4-byte aligned
 * @len: length of the data to read from flash
 * @part: partition and device information
 *
 * Returns non-zero on errors (or we may sys_die())
 *
 * ***WARNING*** we don't currently check that the partition doesn't extend
 * beyond the end of the flash
 */
int load_from_flash_ext(void *dst, uint32_t flash_offs, size_t len,
			struct fsbl_flash_partition *part)
{
	struct fsbl_flash *flash;

	if (!part)
		return -1;

	flash = fsbl_get_flash(part->cs);
	if (!flash)
		sys_die(DIE_CHIP_SELECT_NOT_SUPPORTED,
			"chip-select not supported");

	if (flash->type == FLASH_TYPE_NAND)
		return copy_from_nand(flash, part, dst, flash_offs, len);
	else
		fastcopy(dst, flash_offs_to_va(part->part_offs + flash_offs),
				len);

	return 0;
}

/* Assumes reading from the boot flash, BOLT partition */
int load_from_flash(void *dst, uint32_t flash_offs, size_t len)
{
	struct fsbl_flash_partition part = {
		.cs		= 0,
		.part_offs	= 0,
		.part_size	= 0,
	};
	return load_from_flash_ext(dst, flash_offs, len, &part);
}

#else

int load_from_flash(void *dst, uint32_t flash_offs, size_t len)
{
	/* dummy call for CFG_EMULATION=1 & AVS - we won't
	 be doing AVS or SHMOO but jumping to Linux via
	 a shortened FSBL
	*/
	return 0;
}

#endif	/* !CFG_EMULATION */
