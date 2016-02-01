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

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "timer.h"
#include "error.h"

#include "ui_command.h"
#include "bolt.h"

#include "fileops.h"
#include "boot.h"

#include "loader.h"

#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"

#include "initdata.h"
#include "board.h"
#include "chipid.h"
#include "reboot.h"

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>


/**********************************************************************
 *  board_update_now()
 *
 *  Patch the BOLT binary image in FLASH. Used to update
 * 'struct board_nvm_info' and is not available for SECURE_BOOT=y
 * builds as that option is usually used in the process of making
 * signed BOLT images.
 *
 *  Input parameters:
 *	src: source patch data bytes.
 *	len: length of patch, in bytes.
 *	offs: offset into BOLT image, 0 based.
 *
 *  Return value:
 *	0: success.
 *     !0: fail, see include/error.h
 *
 **********************************************************************/
static int board_update_now(void *src, uint64_t len, uint64_t offs)
{
#if defined(SECURE_BOOT)
	return BOLT_OK; /* secure boot: don't write inside BOLT image */
#else
	char *flashdev = NULL;
	int fh, rc = 0, bm;
	unsigned int amtcopy = 0;
	struct flash_info fi;
	flash_sector_t	fs;
	flash_range_t	fr;
	uint64_t at = 0, top = 0, memoffs;
	uint8_t *mem = NULL;
	int nand = 0;

	bm = board_bootmode();

	/* may have dev names as a cfg option,
	 if non-defaults are required.
	*/
	switch (bm) {
	case BOOT_FROM_EMMC:
		flashdev = "flash1";
		break;

	case BOOT_FROM_NAND:
		nand = 1;
		/* fallthrough */
	case BOOT_FROM_SPI:
	case BOOT_FROM_NOR:
	default:
		flashdev = "flash0.bolt";
		break;
	}

	if (!flashdev) {
#if (CFG_CMD_LEVEL >= 5)
		xprintf("unsupported boot device %d\n", bm);
#endif
		return BOLT_ERR_UNSUPPORTED;
	}

	fh = bolt_open(flashdev);
	if (fh < 0) {
		xprintf("Could not open device '%s'\n", flashdev);
		ui_showerror(fh, "Failed.");
		return BOLT_ERR_DEVNOTFOUND;
	}

#if (CFG_CMD_LEVEL >= 5)
	xprintf("opened device '%s'\n", flashdev);
	xprintf("write %llu bytes @ 0x%p to flash at offset %#llx\n",
			len, src, offs);
#endif

	do {
		if (bm == BOOT_FROM_EMMC) {
			rc = bolt_writeblk(fh, offs, src, len);
			if (rc != (int)len)
				rc = BOLT_ERR_IOERR;
			else
				rc = 0;
			break;
		}


		rc = bolt_ioctl(fh, IOCTL_FLASH_GETPARTINFO, &fi,
				 sizeof(fi), NULL, 0);
		if (rc)
			break;

		/* Sanity check in case we have a very small flash partition.
		*/
#if (CFG_CMD_LEVEL >= 5)
		xprintf("flash base %#16llx\n", fi.flash_base);
		xprintf("flash size %#16llx\n", fi.flash_size);
		xprintf("offs %#16llx -> %#16llx\n", offs, offs+len);
#endif
		if ((offs+len) > fi.flash_size) {
			rc = BOLT_ERR_INV_PARAM;
			xprintf("(%#16llx + %#16llx) > %#16llx\n", offs, len,
					 fi.flash_size);
			break;
		}

		/* find which sector (flash eraseblock) the offset lives in */
		fs.flash_sector_idx = 0;

		/* only reports size. See: nand_block_query() */
		fs.flash_sector_offset = 0;

		rc = BOLT_ERR_INV_PARAM;
		do {
			rc = bolt_ioctl(fh, IOCTL_FLASH_GETSECTORS, &fs,
					sizeof(fs), NULL, 0);
			if (rc)
				break;

			at = fs.flash_sector_offset;
			top = at + fs.flash_sector_size;

#if (CFG_CMD_LEVEL >= 5)
			xprintf("   idx  %#x\n", fs.flash_sector_idx);
			xprintf("status  %#x\n", fs.flash_sector_status);
			xprintf("offset  %#x\n", fs.flash_sector_offset);
			xprintf("size %#x @ %#llx\n", fs.flash_sector_size, at);
#endif
			/* must start in a sector and not cross a sec boundary
			*/
			if ((offs >= at) && (offs < top)) {
#if (CFG_CMD_LEVEL >= 5)
				xprintf("in sector #%d %llx -> %llx -> %llx\n",
					fs.flash_sector_idx, at, offs, top);
#endif
				rc = 0;
				break;
			}

			if ((uint64_t)fs.flash_sector_size < offs) {
#if (CFG_CMD_LEVEL >= 5)
				xprintf("skip sector #%d %x < %llx\n",
					fs.flash_sector_idx,
					fs.flash_sector_size, offs);
#endif
			}

			fs.flash_sector_idx++;
			if (nand)
				fs.flash_sector_offset += fs.flash_sector_size;

		} while (at < fi.flash_size);

		if (rc)
			break;

		rc = BOLT_ERR;

		if ((uint64_t)fs.flash_sector_size < len) {
#if (CFG_CMD_LEVEL >= 5)
			xprintf("sector #%d too small %x < %llx\n",
				fs.flash_sector_idx, fs.flash_sector_size, len);
#endif
			break;
		}

		mem = KMALLOC(fs.flash_sector_size, sizeof(uint32_t));
		if (!mem)
			break;

		fr.range_base   = fs.flash_sector_offset;
		fr.range_length = fs.flash_sector_size;

		memoffs = offs - fr.range_base;

#if (CFG_CMD_LEVEL >= 5)
		xprintf("read from %llx to %llx to patch @ %llx\n",
			fr.range_base,
			fr.range_base + fr.range_length,
			memoffs);
#endif
		amtcopy = bolt_readblk(fh, fr.range_base, mem, fr.range_length);
		if (fr.range_length != (uint64_t)amtcopy)
			break;

		/* Patch in the updated board info */
		memcpy(&mem[memoffs], src, len);

#if (CFG_CMD_LEVEL >= 5)
		xprintf("erase sector #%d, %llx to %llx\n",
			fs.flash_sector_idx, fr.range_base, fr.range_length);
#endif
		rc = bolt_ioctl(fh, IOCTL_FLASH_ERASE_RANGE,
				&fr, sizeof(fr), NULL, 0);
		if (rc)	{
			printf("Failed to erase the flash\n");
			break;
		}

#if (CFG_CMD_LEVEL >= 5)
		xprintf(
			"write mem @ 0x%p to %s, offset %#llx of length %#llx...",
			mem, flashdev, fr.range_base, fr.range_length);
#endif
		rc = 0;
		amtcopy = bolt_writeblk(fh,
			fr.range_base, mem, fr.range_length);
		if (fr.range_length == amtcopy) {
#if (CFG_CMD_LEVEL >= 5)
			xprintf("done. %d bytes written\n", amtcopy);
#endif
		} else {
			rc = BOLT_ERR_IOERR;
		}
	} while (0);

	bolt_close(fh);

	if (mem)
		KFREE(mem);

	if (rc)
		ui_showerror(rc, "board update");

	return rc;
#endif	/* defined(SECURE_BOOT) */
}


/**********************************************************************
 *  board_check_id()
 *
 *  Informational messages for Broacom reference boards only.
 *
 *  Input parameters:
 *	inf: data passed from FSBL to SSBL
 *
 *  Return value:
 *	nothing
 *
 **********************************************************************/
static void board_check_id(struct fsbl_info *inf)
{
#if CFG_BOARD_ID
	uint8_t id = inf->bid;
	unsigned int i, found = 0;
	const char *eco = "You need to hardware fix (ECO) it!";
	uint32_t prid;

	if (!id) {
		err_msg("Board ID is zero! - %s", eco);
		return;
	}

	if (!(id & 0xf0))
		err_msg("Bad board ID format '0.Y' 0x%02x - %s", id, eco);

	/* ignore the minor chip revision for the purpose of board detection */
	prid = BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID) & ~CHIPID_MINOR_REV_MASK;

	for (i = 0 ; i < inf->n_boards; i++)	{

		if (id == inf->board_types[i].bid) {
			found++;

			if (prid == inf->board_types[i].prid) {
				if (inf->board_idx != i)
					err_msg(
					"Not using default board #%c, but board #%c",
					board_idx_to_char(i),
					board_idx_to_char(inf->board_idx));
				return;
			}
		}
	}

	if (!found) {
		err_msg("Board id of 0x%x not found! - %s", id,
			(id & 0xf0) ? "upgrade your BOLT" : "it's bad anyway");
		return;
	}

	err_msg("Board id 0x%x has no matching product ID: 0x%08x", id, prid);
#endif
}


/**********************************************************************
 *  board_check()
 *
 *  Check and apply any changes to the 'struct board_nvm_info' in FLASH
 * if we have updated to a new board or other setting that must survive
 * a cold boot cycle such as a new RTS selection or AVS startup indication.
 *
 *  Input parameters:
 *	force: Force an update to the BOLT binary image in FLASH.
 *
 *  Return value:
 *	0: success.
 *     !0: fail, see include/error.h
 *
 **********************************************************************/
int board_check(int force)
{
	struct fsbl_info *inf = board_info();
	struct board_nvm_info *s, new;
	uint64_t offs;
	char c = '?', d = '?';
	const uint32_t prid =
		BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID) & ~CHIPID_MINOR_REV_MASK;

	if (!inf) {
		err_msg("FSBL board info is not valid!");
		return BOLT_ERR_BADADDR;
	}

	board_check_id(inf);

	s = &(inf->saved_board);
	if (!s)
		return BOLT_ERR_BADADDR;

	if (s->magic != BOARD_NVM_MAGIC) {
		err_msg("Bad NVM magic: 0x%08x", s->magic);
		force = 1; /* try to recover */
	}

	offs = (uint64_t)s->romoffset - (uint64_t)s->romstart;

	if (s->board_idx == inf->board_idx) {
		/* no change on board selection, check any change on PROD_ID */
		if (prid != s->product_id) {
			err_msg("Product ID was 0x%08x, now 0x%08x",
				s->product_id, prid);
			err_msg("Re-select board type!!!");

			/* force board selection by saving an invalid board
			 index and rebooting the board
			*/
			memcpy(&new, s, sizeof(*s));
			new.magic = BOARD_NVM_MAGIC;
			new.board_idx = inf->n_boards;
			new.product_id = prid;
			(void)board_update_now(&new, sizeof(*s), offs);
			bolt_master_reboot();
		}

		/* unless forced to save once again, do nothing */
		if (!force)
			return BOLT_OK;
	}

	if (s->board_idx <= inf->n_boards)
		c = board_idx_to_char(s->board_idx);

	if (inf->board_idx <= inf->n_boards)
		d = board_idx_to_char(inf->board_idx);

#if CFG_BOARD_ID
	/* Check the current board is the default *RUNTIME* detected
	* one by BID & PRID in FBSL and that we have not yet changed
	* from the board build time default. This means we've
	* not deviated from the automatic choice BOLT made and
	* the user has not selected some other board.
	*  If the user forces a write e.g. for RTS, AVS or a new
	* board selection then the board *is* updated and written back
	* to flash.
	*/
	if (!force && (inf->runflags & FSBL_RUNFLAG_BID_MATCH) &&
		(s->board_idx == (uint32_t)(CFG_BOARDDEFAULT - 1)))
			return BOLT_OK;
#endif
	info_msg("board selection update!");
	info_msg("board was %c, now %c (flags:0x%08x)", c, d, s->hardflags);
#if (CFG_CMD_LEVEL >= 5)
	xprintf("fsbl offsets %#x %#x\n", s->romstart, s->romoffset);
#endif

	memcpy(&new, s, sizeof(*s));

	new.magic = BOARD_NVM_MAGIC;
	new.board_idx = inf->board_idx;
	new.product_id = prid;

	return board_update_now(&new, sizeof(*s), offs);
}
