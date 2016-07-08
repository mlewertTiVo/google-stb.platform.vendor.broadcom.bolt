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

#include "bolt.h"
#include "fileops.h"

#include "loader.h"
#include "zimage.h"
#include "arch_ops.h"

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int bolt_zimgload(bolt_loadargs_t *la);

const bolt_loader_t zimgloader = {
	"zimg",
	bolt_zimgload,
	0
};

static int check_zimage(void *ptr)
{
	struct bolt_zimage_s *hdr = ptr;

	if (hdr->magic != DT_ZIMAGE_SIGNATURE)
		return BOLT_ERR_BADEXE;
	if (hdr->astart >= hdr->aend)
		return BOLT_ERR_BADEXE;
	/* TODO: any more overflow checks needed? */
	return hdr->aend - hdr->astart;
}

static int check_image(void *ptr)
{
	struct bolt_image_header *hdr = ptr;

	if (hdr->magic != IMAGE_HEADER_MAGIC)
		return BOLT_ERR_BADEXE;

	return BOLT_OK;
}

static int check_gz(void *ptr)
{
	uint8_t *byte = (uint8_t *)ptr;

	/* http://www.forensicswiki.org/wiki/Gzip#File_header */
	if ((byte[0] == 0x1f) && (byte[1] == 0x8b) && (byte[2] == 0x08))
		return BOLT_OK;

	return BOLT_ERR;
}


static int detect_gz(bolt_loadargs_t *la)
{
	int rc;
	fileio_ctx_t *fsctx;
	void *ref;
	uint8_t *ptr = (uint8_t *)la->la_address;

	rc = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (rc != 0)
		return rc;

	rc = fs_open(fsctx, &ref, la->la_filename, FILE_MODE_READ);
	if (rc != 0) {
		fs_uninit(fsctx);
		return rc;
	}

	rc = fs_read(fsctx, ref, ptr, 4);
	if (rc < 0)
		goto out;

	if (rc < 4) {
		rc = BOLT_ERR_BADEXE;
		goto out;
	}

	rc = BOLT_OK;

	if (check_gz(ptr) == BOLT_OK)
		la->la_flags |= LOADFLG_COMPRESSED;

out:
	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	return rc;
}


/*  *********************************************************************
    *  bolt_zimgload(la)
    *
    *  Read a zImage boot file
    *
    *  Input parameters:
    *      la - loader args
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */
static int bolt_zimgload(bolt_loadargs_t *la)
{
	int res;
	fileio_ctx_t *fsctx;
	void *ref;
	int ttlcopy = 0;
	uint8_t *ptr;
	uint8_t *bootcode;
	int32_t maxsize;
	int amtcopy;
	int thisamt;
	int onedot;
	int imagebytes;

	if (la->la_offs) {
		xprintf("Error: zImage loader does not support seek to offset\n");
		return BOLT_ERR_UNSUPPORTED;
	}

	res = detect_gz(la);
	if (res != BOLT_OK)
		return res;

	bootcode = (uint8_t *) la->la_address;
	maxsize = la->la_maxsize;

	/*
	 * Create a file system context
	 */

	res = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (res != 0)
		return res;

	/* We may have compression for Image files, and will
	* test if its unsupported zImage later on. */
	if (la->la_flags & LOADFLG_COMPRESSED) {
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
	 * Okay, go load the boot file.
	 */

	ptr = bootcode;
	amtcopy = maxsize;

	res = fs_read(fsctx, ref, ptr, ZIMAGE_HEADER);
	if (res < 0) {
		goto error;
	} else if (res < ZIMAGE_HEADER) {
		res = BOLT_ERR_BADEXE;
		goto error;
	}

	/* Try zImage first */
	res = check_zimage(ptr);
	if (res >= 0) {
		/* SWBOLT-340: Compression not allowed for zImage
		*/
		if (la->la_flags & LOADFLG_COMPRESSED) {
			res =  BOLT_ERR_INV_PARAM;
			goto error;
		}
		/* zImage is for 32 bit Linux only, so nuke
		 * the request to load a 64 bit Linux.
		 */
		la->la_flags &= ~(LOADFLG_APP64);

#ifdef BOOT_START_ADDRESS_ZIMAGE
		/* If we have an alternate default start address for zImages. */
		if ((la->la_address == BOOT_START_ADDRESS) &&
			(BOOT_START_ADDRESS != BOOT_START_ADDRESS_ZIMAGE)) {

			la->la_address = BOOT_START_ADDRESS_ZIMAGE;
			bootcode = (uint8_t *)la->la_address;
			memcpy(bootcode, ptr, ZIMAGE_HEADER);
			ptr = bootcode;
		}
#endif
		imagebytes = res;
		xprintf("Reading %d bytes from zImage", imagebytes);

		amtcopy = imagebytes - ZIMAGE_HEADER;
		bolt_zimage_setenv_end((unsigned int)bootcode+imagebytes);
	} else {
		res = check_image(ptr);
		if (res < 0) /* not an Image either */
			goto error;

		if (!arch_booted64())
			warn_msg("64-bit images can't be executed from"
				"a 32-bit boot! Please confirm your "
				"hardware configuration");

		/* Force us to 64 bit Linux. Use the 'raw' loader
		 * to avoid these checks if you need something else.
		 * The header is not skipped and we can't find the size
		 * from the header so just load to the max. BOLT will
		 * ignore this flag if we're not capable of a 64 bit
		 * boot.
		 */
		la->la_flags |= LOADFLG_APP64;

		/* already read, so bump it */
		ttlcopy += ZIMAGE_HEADER;
		xprintf("Reading up to %d bytes from Image", maxsize);

	}

	ptr += ZIMAGE_HEADER;
	ttlcopy = 0;

	onedot = amtcopy / 10;	/* ten dots for entire load */
	if (onedot < 4096)
		onedot = 4096;	/* but minimum 4096 bytes per dot */
	onedot = (onedot + 1) & ~4095;	/* round to multiple of 4096 */

	while (amtcopy > 0) {
		thisamt = onedot;
		if (thisamt > amtcopy)
			thisamt = amtcopy;

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

error:
	xprintf("\n");
	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	la->la_entrypt = (uintptr_t) bootcode;

	if (la->la_flags & LOADFLG_NOISY)
		xprintf(" %d bytes read\n", ttlcopy);

	return (res < 0) ? res : ttlcopy;

}
