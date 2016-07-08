/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "error.h"
#include "timer.h"
#include "board.h"
#include "board_init.h"
#include "parttbl.h"
#include "bsp_config.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sdio_1_boot.h"
#include "bchp_sdio_1_cfg.h"
#include "bchp_common.h"
#ifdef BCHP_SDIO_0_CFG_REG_START
#include "bchp_sdio_0_cfg.h"
#endif

#include "dev_emmc_core_defs.h"
#include "dev_emmcflash.h"
#include "dev_emmc_debug.h"
#include "dev_emmc_rpmb.h"

#define SECTOR_BUFFER_SIZE (64 * 1024)

void emmc_init_pinmux(int id, sdio_type_e type)
{
	int __maybe_unused val;

	switch (type) {
	case SDIO_TYPE_EMMC:
		val = 1;
		break;
	case SDIO_TYPE_SD:
	case SDIO_TYPE_EMMC_ON_SDPINS:
		val = 2;
		break;
	default:
		val = 0;
	}

	switch (id) {
	case 0:
#if defined(BCHP_SDIO_0_CFG_SD_PIN_SEL)
		BDEV_WR_F(SDIO_0_CFG_SD_PIN_SEL, PIN_SEL, val);
#elif defined(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_sdio0_pin_sel_MASK)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_0, sdio0_pin_sel, val);
#endif
		break;

	case 1:
#if defined(BCHP_SDIO_1_CFG_SD_PIN_SEL)
		BDEV_WR_F(SDIO_1_CFG_SD_PIN_SEL, PIN_SEL, val);
#elif defined(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_sdio1_pin_sel_MASK)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_0, sdio1_pin_sel, val);
#endif
		break;
	}
}


struct gpt_part_entry {
	enum part_type type;
	char name[GUID_PART_NAME_SIZE];
	uint64_t offset;
	uint64_t size;
};


static uint8_t *fd_sectorbuffer;	/* sector copy buffer */


/*
 * This default partition table is used when there's no GPT table found on
 * the eMMC device. This will create temporary partitions needed to boot the
 * kernel which can create the GPT table. We offset these temp partitions
 * to avoid modifying the GPT partition table during debug.
 */
#define SKIP_GPT (34LL * EMMC_BLOCKSIZE)
static const struct gpt_part_entry default_part_table[] = {
	{ PART_TYPE_LINUX, "macadr", SKIP_GPT + 0, 1LL * EMMC_BLOCKSIZE },
	{ PART_TYPE_LINUX, "nvram", SKIP_GPT + EMMC_BLOCKSIZE, 64LL * 1024 },
	{ PART_TYPE_LAST, "", 0ULL, 0ULL }
};

static unsigned char *temp_buffer;


static int check_blk_io_params(struct flash_partition *part,
			       iocb_buffer_t *buffer)
{
	if (buffer->buf_offset + buffer->buf_length > part->size) {
		err_msg("EMMC: Read/Write past end of partition");
		return -1;
	}
	return 0;
}

static int must_be_rpmb(struct emmc_device *dev)
{
	if ((dev->flags & EMMC_DEVICE_FLAGS_RPMB) == 0) {
		err_msg("%s: RPBM operation on non-RPMB partition",
			dev->chip->regs.name);
		return 1;
	}
	return 0;
}



/*  eMMC driver main functions */

/*  *********************************************************************
    *  emmcdrv_open(ctx)
    *
    *  Called when the flash device is opened.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0 if ok else error code
    ********************************************************************* */
static int emmcdrv_open(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	struct emmc_device *dev = flashpart_get_drvdata(part);
	struct emmc_chip *chip = dev->chip;
	int res;

	DBG_MSG_DRV_DISPATCH("\n\n ----->>> emmcdrv_open\n");
	DBG_MSG_DRV_DISPATCH("    fullname    : %s\n",
		    ctx->dev_dev->dev_fullname);
	DBG_MSG_DRV_DISPATCH("    class	    : %d\n",
		    ctx->dev_dev->dev_class);
	DBG_MSG_DRV_DISPATCH("    opencount   : %d\n",
		    ctx->dev_dev->dev_opencount);
	DBG_MSG_DRV_DISPATCH("    description : %s\n",
		    ctx->dev_dev->dev_description);
	DBG_MSG_DRV_DISPATCH("    size	 : %lld (%dKB)\n", dev->size,
		    (uint32_t)(dev->size / 1024));
	DBG_MSG_DRV_DISPATCH("    id : %d\n", dev->id);
	if (dev->flags & EMMC_DEVICE_FLAGS_RPMB) {
		DBG_MSG_DRV_DISPATCH("    RPMB\n");
	}
	res = emmc_select_partition(chip, dev->id);
	if (res)
		err_msg("EMMC: Error selecting partition\n");

	/* Start clean on open */
	dev->dirty = 0;

	if (dev->flags & EMMC_DEVICE_FLAGS_RPMB)
		dev->blocksize = EMMC_BLOCKSIZE_RPMB;
	else
		dev->blocksize = EMMC_BLOCKSIZE;
	return 0;
}

/*  *********************************************************************
    *  emmcdrv_read(ctx, buffer)
    *
    *  Called when the flash device is read.
    *
    *  Input parameters:
    *	    ctx - device context
    *	    buffer - data buffer context
    *
    *  Return value:
    *	    0 if ok else error code
    ********************************************************************* */

static int emmcdrv_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct emmc_device *dev = flashpart_get_drvdata(part);
	int blocksize = dev->blocksize;
	struct emmc_chip *chip = dev->chip;
	unsigned char *bptr;
	int blen;
	int res = 0;
	int amtcopy;
	uint64_t lba;
	uint64_t offset;
	int rem;
	int (*block_read)(struct emmc_chip *, uint32_t, uint32_t, uint8_t *);
	int blks;

	DBG_MSG_DRV_DISPATCH("\n\n ----->>> emmcdrv_read\n");
	DBG_MSG_DRV_DISPATCH("    offset: %#llx, length: %d, buffer: %#x\n",
			     buffer->buf_offset, buffer->buf_length,
			     (unsigned int)buffer->buf_ptr);
	if (check_blk_io_params(part, buffer))
		return BOLT_ERR_INV_PARAM;
	if (dev->flags & EMMC_DEVICE_FLAGS_RPMB)
		block_read = &emmc_rpmb_block_read;
	else
		block_read = &emmc_block_read;
	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;
	offset = buffer->buf_offset + part->offset;

	if (offset & (blocksize - 1)) {
		lba = (offset / blocksize);
		rem = (offset & (blocksize - 1));
		res = (*block_read)(chip, lba, 1, fd_sectorbuffer);
		if (res < 0)
			goto out;
		amtcopy = blocksize - rem;
		if (amtcopy > blen)
			amtcopy = blen;
		memcpy(bptr, &fd_sectorbuffer[rem], amtcopy);
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen >= blocksize && IS_ALIGNED((uint32_t)bptr, sizeof(uint32_t))) {
		int seccnt;

		lba = (offset / blocksize);
		seccnt = (blen / blocksize);

		res = (*block_read)(chip, lba, seccnt, bptr);
		if (res < 0)
			goto out;

		amtcopy = seccnt * blocksize;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	while (blen) {
		if (blen > SECTOR_BUFFER_SIZE)
			amtcopy = SECTOR_BUFFER_SIZE;
		else
			amtcopy = blen;
		lba = (offset / blocksize);
		blks = (amtcopy + (blocksize  - 1)) / blocksize;
		res = (*block_read)(chip, lba, blks, fd_sectorbuffer);
		if (res < 0)
			goto out;
		memcpy(bptr, fd_sectorbuffer, amtcopy);
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

out:
	buffer->buf_retlen = bptr - buffer->buf_ptr;
	return res;
}

/*  *********************************************************************
    *  emmcdrv_write(ctx, buffer)
    *
    *  Called to write flash device.
    *
    *  Input parameters:
    *	    ctx - device context
    *	    buffer - data buffer context
    *
    *  Return value:
    *	    0 if ok else error code
    ********************************************************************* */

static int emmcdrv_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct emmc_device *dev = flashpart_get_drvdata(part);
	int blocksize = dev->blocksize;
	struct emmc_chip *chip = dev->chip;
	unsigned char *bptr;
	int blen;
	int res = 0;
	int amtcopy;
	uint64_t offset;
	uint64_t lba;
	int rem;
	int (*block_write)(struct emmc_chip *, uint32_t, uint32_t, uint8_t *);
	int blks;

	DBG_MSG_DRV_DISPATCH("\n\n ----->>> emmcdrv_write\n");
	DBG_MSG_DRV_DISPATCH("    offset: %#llx, length: %d, buffer: %#x\n",
			     buffer->buf_offset, buffer->buf_length,
			     (unsigned int)buffer->buf_ptr);
	if (check_blk_io_params(part, buffer))
		return BOLT_ERR_INV_PARAM;
	if (dev->flags & EMMC_DEVICE_FLAGS_RPMB)
		block_write = &emmc_rpmb_block_write;
	else
		block_write = &emmc_block_write;
	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;
	offset = buffer->buf_offset + part->offset;

	if (offset & (blocksize - 1)) {
		lba = (offset / blocksize);
		rem = (offset & (blocksize - 1));
		res = emmc_block_read(chip, lba, 1, fd_sectorbuffer);
		if (res < 0)
			goto out;
		amtcopy = blocksize - rem;
		if (amtcopy > blen)
			amtcopy = blen;
		memcpy(&fd_sectorbuffer[offset & (blocksize - 1)], bptr,
		       amtcopy);
		res = (*block_write)(chip, lba, 1, fd_sectorbuffer);
		if (res < 0)
			goto out;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen >= blocksize && IS_ALIGNED((uint32_t)bptr, sizeof(uint32_t))) {
		int seccnt;

		lba = (offset / blocksize);
		seccnt = (blen / blocksize);

		res = (*block_write)(chip, lba, seccnt, bptr);
		if (res < 0)
			goto out;

		amtcopy = seccnt * blocksize;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	while (blen) {
		if (blen > SECTOR_BUFFER_SIZE)
			amtcopy = SECTOR_BUFFER_SIZE;
		else
			amtcopy = blen;
		lba = (offset / blocksize);
		blks = (amtcopy + (blocksize  - 1)) / blocksize;

		/* partial block */
		if (amtcopy % blocksize) {
			res = emmc_block_read(chip, lba, blks, fd_sectorbuffer);
			if (res < 0)
				goto out;
		}
		memcpy(fd_sectorbuffer, bptr, amtcopy);
		res = (*block_write)(chip, lba, blks, fd_sectorbuffer);
		if (res < 0)
			goto out;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	/* mark hard partition dirty */
	dev->dirty = 1;

out:
	buffer->buf_retlen = bptr - buffer->buf_ptr;
	return res;
}

/*  *********************************************************************
    *  emmcdrv_inpstat(ctx,inpstat)
    *
    *  Return "input status".  For flash devices, we always return true.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   inpstat - input status structure
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */
static int emmcdrv_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	DBG_MSG_DRV_DISPATCH("\n\n ----->>> emmcdrv_inpstat\n\n");
	inpstat->inp_status = 1;

	return 0;
}

/*  *********************************************************************
    *  emmcdrv_ioctl(ctx,buffer)
    *
    *  Handle special IOCTL functions for the flash.  Flash devices
    *  support NVRAM information, sector and chip erase, and a
    *  special IOCTL for updating the running copy of BOLT.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - descriptor for IOCTL parameters
    *
    *  Return value:
    *	   0 if ok else error
    ********************************************************************* */
static int emmcdrv_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct emmc_device *dev = flashpart_get_drvdata(part);
	nvram_info_t *nvinfo;
	struct flash_info *info;

	DBG_MSG_DRV_DISPATCH("\n\n ----->>> emmcdrv_ioctl : cmd = %d\n\n",
			     (int)buffer->buf_ioctlcmd);

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_NVRAM_GETINFO:
		nvinfo = (nvram_info_t *) buffer->buf_ptr;
		if (buffer->buf_length != sizeof(nvram_info_t))
			return BOLT_ERR_INV_PARAM;

		nvinfo->nvram_offs_part = 0;
		nvinfo->nvram_offset = 0;
		nvinfo->nvram_size = part->size;
		nvinfo->nvram_eraseflg = 0;
		buffer->buf_retlen = sizeof(nvram_info_t);
		return 0;
		break;

	/* eMMC doesn't need to do anything */
	case IOCTL_NVRAM_ERASE:
	case IOCTL_NVRAM_UNLOCK:
		return 0;

	case IOCTL_FLASH_GETINFO:
		info = (struct flash_info *) buffer->buf_ptr;
		info->flash_base = 0;
		info->flash_size = part->size;
		info->type = FLASH_TYPE_EMMC;
		info->flags = FLASH_FLAG_NOERASE;
		info->page_size = 0; /* not applicable */

		DBG_MSG_DRV_DISPATCH("  [flash_info]\n");
		DBG_MSG_DRV_DISPATCH("      info->flash_base: %d (0x%08X)\n",
				     (int)info->flash_base,
				     (unsigned int)info->flash_base);
		DBG_MSG_DRV_DISPATCH("      info->flash_size: %d (0x%08X)\n",
				     (int)info->flash_size,
				     (unsigned int)info->flash_size);
		DBG_MSG_DRV_DISPATCH("      info->type      : %d\n",
				     info->type);
		DBG_MSG_DRV_DISPATCH("      info->flags     : %d\n",
				     info->flags);
		return 0;

	case IOCTL_FLASH_GETPARTINFO:
		info = (struct flash_info *) buffer->buf_ptr;
		info->flash_base = 0;
		info->flash_size = dev->size;
		info->type = FLASH_TYPE_EMMC;
		info->flags = FLASH_FLAG_NOERASE;

		DBG_MSG_DRV_DISPATCH("  [flash_info]\n");
		DBG_MSG_DRV_DISPATCH("      info->flash_base: %d (0x%08X)\n",
				     (int)info->flash_base,
				     (unsigned int)info->flash_base);
		DBG_MSG_DRV_DISPATCH("      info->flash_size: %d (0x%08X)\n",
				     (int)info->flash_size,
				     (unsigned int)info->flash_size);
		DBG_MSG_DRV_DISPATCH("      info->type      : %d\n",
				     info->type);
		DBG_MSG_DRV_DISPATCH("      info->flags     : %d\n",
				     info->flags);
		return 0;
	case IOCTL_FLASH_RPMB_GET_WRITE_COUNTER:
		if (must_be_rpmb(dev))
			return BOLT_ERR_INV_PARAM;
		if (buffer->buf_length < sizeof(uint32_t))
			return BOLT_ERR_INV_PARAM;
		buffer->buf_retlen = sizeof(uint32_t);
		return emmc_rpmb_get_write_counter(dev->chip,
						(uint32_t *)buffer->buf_ptr);
	case IOCTL_FLASH_RPMB_USE_KEY:
		if (must_be_rpmb(dev))
			return BOLT_ERR_INV_PARAM;
		return emmc_rpmb_use_key(dev->chip,
					(uint32_t *)buffer->buf_ptr);
	case IOCTL_FLASH_RPMB_PROGRAM_KEY:
		if (must_be_rpmb(dev))
			return BOLT_ERR_INV_PARAM;
		return emmc_rpmb_program_key(dev->chip);
	}

	return BOLT_ERR_INV_PARAM;
}

/*  *********************************************************************
    *  emmcdrv_close(ctx)
    *
    *  Close the flash device.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0
    ********************************************************************* */
static int emmcdrv_close(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	struct emmc_device *dev = flashpart_get_drvdata(part);
	struct emmc_chip *chip = dev->chip;

	/*
	 * If this was the first boot partition and it was written,
	 * make it the selected boot partition.
	 */
	if (dev->dirty && (dev->id == EMMC_HPART_BOOT1)) {
		emmc_set_boot_partition(chip, 0);
	}

	return 0;

}

static void partition_description(bolt_driver_t *drv,
				  struct flash_partition *part,
				  char *buf)
{
	uint64_t size = part->size;
	char *hard_part;
	struct emmc_device *dev = flashpart_get_drvdata(part);

	switch (dev->id) {
	case EMMC_HPART_DATA:
		hard_part = "Data ";
		break;
	case EMMC_HPART_BOOT1:
		hard_part = "Boot1";
		break;
	case EMMC_HPART_BOOT2:
		hard_part = "Boot2";
		break;
	case EMMC_HPART_RPMB:
		hard_part = "RPMB ";
		break;
	case EMMC_HPART_GP0:
		hard_part = "GP0";
		break;
	case EMMC_HPART_GP1:
		hard_part = "GP1";
		break;
	case EMMC_HPART_GP2:
		hard_part = "GP2";
		break;
	case EMMC_HPART_GP3:
		hard_part = "GP3";
		break;
	default:
		hard_part = "Unknown";
		break;
	}

	buf += xsprintf(buf, "%s %s: 0x%09llX-0x%09llX ",
			drv->drv_description, hard_part,
			part->offset, part->offset + size);
	if (size > 1024 * 1024 * 2)
		buf += xsprintf(buf,
				"(%lluMB)", (size + (1024 * 1024 - 1)) >> 20);
	else if (size > 1024)
		buf += xsprintf(buf, "(%lluKB)", (size + (1024 - 1)) >> 10);
	else
		buf += xsprintf(buf, "(%lluB)", size);
}

static int add_partition_entry(bolt_driver_t *drv,
			       struct flash_dev *flash,
			       uint64_t size,
			       uint64_t offset,
			       const char *name)
{
	int nparts = flash->nparts;
	struct flash_partition *part;

	/* Check for MAX partitions */
	if (nparts == (FLASH_MAX_PARTITIONS - 1))
		return 1;
	flash->nparts++;
	part = &flash->parts[nparts];
	part->flash = flash;
	part->size = size;
	part->offset = offset;
	part->name = name;
	return 0;
}

static void register_partition(bolt_driver_t *drv,
			       struct flash_partition *part)
{
	struct flash_dev *flash = part->flash;
	char desc[80];

	partition_description(drv, part, desc);
	part->name = bolt_attach_idx(drv, flash->cs, part,
				     part->name, desc);
}

static void strcpy_utf16_to_utf8(char *dst, uint16_t *src, int max)
{
	while (max--) {
		*dst++ = (uint8_t)(*src & 0xff);
		if (*src++ == 0)
			break;
	}
}

/*
 * Look for and, if found, parse the GPT partition table on the device.
 * Return an array where each entry is a struct gpt_part_entry that
 * describes one GPT partition. This array is malloc'd and must be free'd
 * by the calling routine.
 */
static struct gpt_part_entry *gpt_to_part_table(struct flash_dev *flash)
{
	int fd;
	unsigned int i;
	int x;
	int res;
	gpt_hdr_t *gpt_hdr = NULL;
	guid_part_entry_t *entry;
	uint64_t prev_lba = ~0;
	struct gpt_part_entry *gparts = NULL;
	const char *devname = flash->parts[0].name;

	DBG_MSG_PART("\n\nStart GPT parsing for %s\n", devname);
	fd = bolt_open((char *)devname);
	if (fd < 0) {
		xprintf("Error opening %s to read GPT: %d\n", devname, fd);
		goto err_exit;
	}
	res = bolt_readblk(fd, 0, temp_buffer, EMMC_BLOCKSIZE);
	if (res != EMMC_BLOCKSIZE) {
		xprintf("Read error: %d\n", res);
		goto err_exit;
	}
	if (!gpt_is_protective_mbr(temp_buffer, EMMC_BLOCKSIZE)) {
		DBG_MSG_PART("GPT PMBR not found\n");
		goto err_exit;
	}
	res = bolt_readblk(fd, 1 * EMMC_BLOCKSIZE, temp_buffer, EMMC_BLOCKSIZE);
	if (res != EMMC_BLOCKSIZE) {
		xprintf("Read error: %d\n", res);
		goto err_exit;
	}
	res = gpt_alloc_hdr(temp_buffer, EMMC_BLOCKSIZE, &gpt_hdr);
	if (res < 0) {
		DBG_MSG_PART("gpt_alloc_hdr failed: %d\n", res);
		goto err_exit;
	}

	/* GPT header is legit, look for partitions */
	gparts = (struct gpt_part_entry *)
		 KMALLOC(sizeof(*gparts) * (gpt_hdr->part_entry_cnt + 1), 0);
	if (!gparts)
		goto err_exit;

	for (i = 0, x = 0; i < gpt_hdr->part_entry_cnt; i++) {
		uint64_t part_tbl_lba;
		uint32_t entry_ofs;
		uint64_t rel_entry_lba;
		uint32_t rel_entry_ofs;

		res = gpt_get_part_entry_loc(gpt_hdr, i, &part_tbl_lba,
					     &entry_ofs);
		if (res < 0)
			goto err_exit;

		/* GPT is sector-size agnostic, so calculate the LBA */
		rel_entry_lba = part_tbl_lba + (entry_ofs / EMMC_BLOCKSIZE);
		rel_entry_ofs = entry_ofs % EMMC_BLOCKSIZE;

		/*
		 * Partition entries are sub-block length, so avoid repeated
		 * disk accesses.
		 */
		if (prev_lba != rel_entry_lba) {
			res = bolt_readblk(fd,
					(uint32_t)rel_entry_lba *
					EMMC_BLOCKSIZE,
					temp_buffer,
					EMMC_BLOCKSIZE);
			if (res != EMMC_BLOCKSIZE) {
				xprintf("Read error: %d\n", res);
				goto err_exit;
			}
			prev_lba = rel_entry_lba;
		}

		entry = (guid_part_entry_t *)(temp_buffer + rel_entry_ofs);
		if (!gpt_is_part_type(entry, PART_TYPE_INVALID)) {
			gparts[x].offset = entry->first_lba * EMMC_BLOCKSIZE;
			gparts[x].size =
				(entry->last_lba + 1 - entry->first_lba)
				* EMMC_BLOCKSIZE;
			gparts[x].type = PART_TYPE_LINUX;
			strcpy_utf16_to_utf8(gparts[x].name, entry->part_name,
					     sizeof(gparts[x].name));
			DBG_MSG_PART("ENTRY name: %s, start: 0x%lld, "
				     "end: 0x%lld, type: %#x\n",
				     gparts[x].name,
				     entry->first_lba, entry->last_lba,
				     (unsigned int)entry->attrib_flags);
			x++;
		}
	}
	bolt_close(fd);
	gparts[x].type = PART_TYPE_LAST;		/* terminator */
	gpt_free_hdr(gpt_hdr);
	DBG_MSG_PART("SUCCESS reading GPT\n");
	return gparts;

err_exit:
	if (fd >= 0)
		bolt_close(fd);
	gpt_free_hdr(gpt_hdr);
	KFREE(gparts);
	return NULL;
}

/*
 * See if the GPT contains at least the partition entries listed in
 * the default_part_table[]. This is the minimum needed for operation.
 */
static int check_for_mandatory_parts(struct gpt_part_entry *gparts)
{
	const struct gpt_part_entry *mand_gparts = &default_part_table[0];
	struct gpt_part_entry *new_gparts;

	if (gparts == NULL) {
		xprintf("GPT Partition table not found on flash device\n");
		goto bad_gpt;
	}
	while (mand_gparts->type != PART_TYPE_LAST) {
		new_gparts = gparts;

		while (new_gparts->type != PART_TYPE_LAST) {
			if (strcmp(mand_gparts->name, new_gparts->name) == 0) {
				break;
			}
			new_gparts++;
		}
		if (new_gparts->type == PART_TYPE_LAST) {
			xprintf("GPT partition table entry for \"%s\" is "
				"missing\n", mand_gparts->name);
			goto bad_gpt;
		}
		mand_gparts++;
	}
	return 1;

bad_gpt:
	xprintf("Using default partitions\n");
	return 0;
}

/*
 * Register a partition for each GPT partition found on the device.
 * If this is the boot device and the GPT partition table on the device
 * isn't found or it's empty or the mandatory partitions are missing or
 * too small, force a default set of partitions so we can a least
 * boot from the net.
 */
static void init_soft_partitions(bolt_driver_t *drv,
				struct emmc_chip *chip,
				struct flash_dev *flash,
				int check)
{
	struct gpt_part_entry *gparts;
	const struct gpt_part_entry *parts;

	gparts = gpt_to_part_table(flash);
	if (check && !check_for_mandatory_parts(gparts))
		parts = default_part_table;
	else
		parts = gparts;
	if (parts == NULL)
		return;
	while (parts->type != PART_TYPE_LAST) {
		if (add_partition_entry(drv, flash, parts->size,
					parts->offset, parts->name)) {
			err_msg("EMMC: Exceeded MAX partitions (%d), ignoring "
				"the rest", FLASH_MAX_PARTITIONS);
			break;
		}
		register_partition(drv, &flash->parts[flash->nparts - 1]);
		parts++;
	}
	KFREE(gparts);
}

static struct flash_dev *init_hard_partition(bolt_driver_t *drv,
					struct emmc_chip *chip,
					int id,
					int instance)
{
	struct emmc_device *dev;
	struct flash_dev *flash;
	int offset = instance * EMMC_HPART_MAX;

	dev = (struct emmc_device *)KMALLOC(sizeof(struct emmc_device), 0);
	if (!dev)
		return NULL;
	memset(dev, 0, sizeof(struct emmc_device));

	flash = &dev->flash;
	dev->chip = chip;
	dev->id = id;
	dev->size = chip->dev_config.hard_part_sizes[id];
	flash->cs = offset + id;

	flash->type = FLASH_TYPE_EMMC;
	flash->priv = dev;
	flash->blocksize = EMMC_BLOCKSIZE;
	flash->size = dev->size;
	if (id == EMMC_HPART_RPMB)
		dev->flags |= EMMC_DEVICE_FLAGS_RPMB;

	/* Make the first soft partition the entire hard partition */
	add_partition_entry(drv, flash, flash->size, 0, NULL);
	register_partition(drv, &flash->parts[0]);
	return flash;
}

/*  *********************************************************************
    *  emmcdrv_probe(drv, probe_ptr, emmc_config_ptr)
    *
    *  Device probe routine.  Attach the flash device to
    *  BOLT's device table.
    *
    *  Input parameters:
    *	   drv - driver descriptor
    *	   boot_flag - true to indicate boot from this eMMC device
    *	   bus_width - width of the eMMC bus
    *	   probe_ptr - pointer to register addresses for this device
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void emmcdrv_probe(bolt_driver_t *drv, unsigned long boot_flag,
			  unsigned long bus_width, void *probe_ptr)
{
	struct emmc_chip  *chip = NULL;
	struct flash_dev *flash;
	static int instance_boot;
	static int instance_secondary;
	int *instancep;
	int x;
	struct emmc_host_config host_config;

	/*
	 * If this is not the boot device, use a different name
	 * so it doesn't conflict with NOR/NAND devices
	 */
	if (boot_flag) {
		instancep = &instance_boot;
		drv->drv_bootname = "flash";
	} else {
		instancep = &instance_secondary;
		drv->drv_bootname = "emmcflash";
	}

	DBG_MSG_DRV("Probe : %s: %s, boot: %d, instance:%d\n",
		     drv->drv_bootname, drv->drv_description,
		     (int)boot_flag, *instancep);
	chip = (struct emmc_chip *)KMALLOC(sizeof(struct emmc_chip), 0);
	fd_sectorbuffer = KMALLOC(SECTOR_BUFFER_SIZE,
				EMMC_DMA_BUF_ALIGN);
	temp_buffer = KMALLOC(EMMC_BLOCKSIZE,
			EMMC_DMA_BUF_ALIGN);
	if (!fd_sectorbuffer || !temp_buffer || !chip)
		goto err;
	memset(&host_config, 0, sizeof(struct emmc_host_config));

	/* Setup host config */
	host_config.hosths_on = EMMC_HOST_HS;
	host_config.bus_voltage = EMMC_BUS_VOLTAGE;
	host_config.disable_dma = 0;
	switch (bus_width) {
	case 8:
		host_config.bus_width = BUS_WIDTH_8BIT;
		break;
	case 4:
		host_config.bus_width = BUS_WIDTH_4BIT;
		break;
	default:
		host_config.bus_width = BUS_WIDTH_1BIT;
		break;
	}

	if (emmc_initialize((struct emmc_registers *)probe_ptr,
				chip, boot_flag, &host_config))
		goto err;

	for (x = 0; x < EMMC_HPART_MAX; x++) {
		if (chip->dev_config.hard_part_sizes[x]) {
			flash = init_hard_partition(drv, chip, x, *instancep);
			if (flash != NULL)
				/*
				 * Don't look for a GPT partition table on
				 * these hard partitions
				 */
				switch (x) {
				case EMMC_HPART_BOOT1:
				case EMMC_HPART_BOOT2:
				case EMMC_HPART_RPMB:
					break;
				default:
					init_soft_partitions(drv, chip, flash,
						x == EMMC_HPART_DATA &&
						chip->boot_device);
				}
		}
	}
	(*instancep)++;
	return;

err:
	if (fd_sectorbuffer)
		KFREE(fd_sectorbuffer);
	if (temp_buffer)
		KFREE(temp_buffer);
	if (chip)
		KFREE(chip);
}

static const bolt_devdisp_t emmcdrv_dispatch = {
	emmcdrv_open,
	emmcdrv_read,
	emmcdrv_inpstat,
	emmcdrv_write,
	emmcdrv_ioctl,
	emmcdrv_close,
	NULL,
	NULL
};

bolt_driver_t emmcflashdrv = {
	"EMMC flash",
	"",
	BOLT_DEV_FLASH,
	&emmcdrv_dispatch,
	&emmcdrv_probe
};
