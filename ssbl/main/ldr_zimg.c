/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
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
#include "device.h"
#include "error.h"
#include "devfuncs.h"

#include "bolt.h"
#include "fileops.h"

#include "boot.h"

#include "loader.h"
#include "zimage.h"

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

	bootcode = (uint8_t *) la->la_address;
	maxsize = la->la_maxsize;

	/* SWBOLT-340: Compression not allowed for zImage
	*/
	if (la->la_flags & LOADFLG_COMPRESSED)
		return BOLT_ERR_INV_PARAM;

	/*
	 * Create a file system context
	 */

	res = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (res != 0)
		return res;

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

	res = check_zimage(ptr);
	if (res < 0)
		goto error;

	imagebytes = res;

	xprintf("Reading %d bytes from zImage", imagebytes);
	amtcopy = imagebytes - ZIMAGE_HEADER;
	ptr += ZIMAGE_HEADER;
	bolt_zimage_setenv_end((unsigned int)bootcode+imagebytes);

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
