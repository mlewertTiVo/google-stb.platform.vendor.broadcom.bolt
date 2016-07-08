/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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
#include "device.h"
#include "error.h"
#include "devfuncs.h"
#include "common.h"

#include "bolt.h"
#include "fileops.h"

#include "bootblock.h"

#include "loader.h"
#include "zimage.h"

/* Don't artificially split transfers into anything less */
#define LDR_RAW_MIN_CHUNK	(16 * 1024)

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int bolt_rawload(bolt_loadargs_t *la);

const bolt_loader_t rawloader = {
	"raw",
	bolt_rawload,
	0
};

/*  *********************************************************************
    *  bolt_findbootblock(la,fsctx,ref)
    *
    *  Find the boot block on the specified device.
    *
    *  Input parameters:
    *      la - loader args (to be filled in)
    *      ops - file operations
    *      ref - reference for open file handle
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */
static int bolt_findbootblock(bolt_loadargs_t *la,
			     fileio_ctx_t *fsctx,
			     void *ref, struct boot_block *bootblock)
{
	uint32_t checksum = 0;
	uint32_t calcsum = 0;
	int res;
	int curblk;

	/*
	 * Search for the boot block.  Stop when we find
	 * something with a matching checksum and magic
	 * number.
	 */

	fs_seek(fsctx, ref, 0, FILE_SEEK_BEGINNING);

	for (curblk = 0; curblk < BOOT_BLOCK_MAXLOC; curblk++) {

		/* Read a block */

		res = fs_read(fsctx, ref,
			      (unsigned char *)bootblock,
			      sizeof(struct boot_block));

		if (res != sizeof(struct boot_block))
			return BOLT_ERR_IOERR;

		/* Verify magic number */

		if (bootblock->bb_magic != BOOT_MAGIC_NUMBER)
			continue;

		/* Extract fields from block */

		checksum = ((uint32_t)
			    (bootblock->bb_hdrinfo & BOOT_HDR_CHECKSUM_MASK));
		bootblock->bb_hdrinfo &= ~BOOT_HDR_CHECKSUM_MASK;

		/* Verify block's checksum */

		CHECKSUM_BOOT_DATA(&(bootblock->bb_magic), BOOT_BLOCK_SIZE,
				   &calcsum);

		if (checksum == calcsum)
			break;
	}

	/*
	 * Okay, determine if we were successful.
	 */

	if (bootblock->bb_magic != BOOT_MAGIC_NUMBER)
		return BOLT_ERR_INVBOOTBLOCK;

	if (checksum != calcsum)
		return BOLT_ERR_BBCHECKSUM;

	/*
	 * If we get here, we had a valid boot block.
	 */

	return 0;
}

/*  *********************************************************************
    *  bolt_rawload(la)
    *
    *  Read a raw (unformatted) boot file
    *
    *  Input parameters:
    *      la - loader args
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */
static int bolt_rawload(bolt_loadargs_t *la)
{
	int res;
	fileio_ctx_t *fsctx;
	const fileio_dispatch_t *ops;
	void *ref;
	int ttlcopy = 0;
	int findbb = FALSE;
	struct boot_block bootblock;
	uint8_t *ptr;
	uint8_t *bootcode;
	uint32_t checksum, calcsum;
	uint64_t seek_offs = la->la_offs;
	int32_t maxsize;
	int amtcopy;
	int thisamt;
	uint32_t loadflags;
	int onedot;

	loadflags = la->la_flags;
	bootcode = (uint8_t *) la->la_address;
	maxsize = la->la_maxsize;

	if (!(loadflags & LOADFLG_SPECADDR)) {
		int devinfo;

		devinfo = la->la_device ? bolt_getdevinfo(la->la_device) : 0;

		/*
		 * If the device is either a disk or a flash device,
		 * we will expect to find a boot block.
		 * Serial and network devices do not have boot blocks.
		 */
		if ((devinfo >= 0) &&
		    (((devinfo & BOLT_DEV_MASK) == BOLT_DEV_DISK) ||
		     ((devinfo & BOLT_DEV_MASK) == BOLT_DEV_FLASH))) {
			findbb = TRUE;
		}
	}

	/*
	 * merge in any filesystem-specific flags
	 */

	ops = bolt_findfilesys(la->la_filesys);
	if (!ops)
		return BOLT_ERR_FSNOTAVAIL;
	loadflags |= ops->loadflags;

	/*
	 * turn off the boot block if requested.
	 */

	if (loadflags & LOADFLG_NOBB)
		findbb = FALSE;

	/*
	 * Create a file system context
	 */

	res = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (res != 0)
		return res;

	/*
	 * Turn on compression if we're doing that.
	 */

	if (!findbb && (la->la_flags & LOADFLG_COMPRESSED)) {
		res = fs_hook(fsctx, "z");
		if (res != 0)
			return res;
	}

	/*
	 * Open the boot device
	 */

	res = fs_open(fsctx, &ref, la->la_filename, FILE_MODE_READ);
	if (res != 0) {
		fs_uninit(fsctx);
		return res;
	}

	/*
	 * If we need to find a boot block, do it now.
	 */

	if (findbb) {
		res = bolt_findbootblock(la, fsctx, ref, &bootblock);
		if (res) {
			/* Get out now; the disk has no boot block */
			fs_close(fsctx, ref);
			fs_uninit(fsctx);
			return res;
		}

		/*
		 * We found the boot block; seek to the part of the disk where
		 * the boot code is.
		 */
		maxsize = bootblock.bb_secsize & BOOT_SECSIZE_MASK;
		seek_offs += bootblock.bb_secstart;
	}

	/* Seek to the right offset */
	fs_seek(fsctx, ref, seek_offs, FILE_SEEK_BEGINNING);

	/*
	 * Okay, go load the boot file.
	 */

	ptr = bootcode;
	amtcopy = maxsize;
	ttlcopy = 0;

	/* Try for 10 dots for entire load */
	onedot = max(amtcopy / 10, LDR_RAW_MIN_CHUNK);
	/* Align to multiples of MIN_CHUNK */
	onedot = ALIGN_TO(onedot, LDR_RAW_MIN_CHUNK);

	while (amtcopy > 0) {
		thisamt = min(onedot, amtcopy);

		res = fs_read(fsctx, ref, ptr, thisamt);
		if (res <= 0)
			break;
		xprintf(".");

		ptr += res;
		amtcopy -= res;
		ttlcopy += res;
	}

	/*
	 * We're done with the file.
	 */

	xprintf("\n");
	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	/*
	 * Verify the boot loader checksum if we were reading
	 * the disk.
	 */

	if (findbb) {
		CHECKSUM_BOOT_DATA(bootcode, maxsize, &calcsum);
		checksum =
		    (uint32_t) ((bootblock.bb_secsize & BOOT_DATA_CHECKSUM_MASK)
				>> BOOT_DATA_CHECKSUM_SHIFT);

		if (checksum != calcsum)
			return BOLT_ERR_BOOTPROGCHKSUM;
	}

	la->la_entrypt = (uintptr_t) bootcode;

	if (la->la_flags & LOADFLG_NOISY)
		xprintf(" %d bytes read\n", ttlcopy);

	return (res < 0) ? res : ttlcopy;

}
