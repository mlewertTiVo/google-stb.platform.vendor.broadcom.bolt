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
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "fileops.h"
#include "iocb.h"
#include "devfuncs.h"
#include "console.h"
#include "timer.h"

#include "bolt.h"

/*  *********************************************************************
    *  RAW context
    ********************************************************************* */

/*
 * File system context - describes overall file system info,
 * such as the handle to the underlying device.
 */

typedef struct raw_fsctx_s {
	int raw_dev;
	int raw_isconsole;
	int raw_refcnt;
} raw_fsctx_t;

/*
 * File context - describes an open file on the file system.
 * For raw devices, this is pretty meaningless, but we do
 * keep track of where we are.
 *
 * Note: we use signed 64-bit integer instead of unsigned
 * 64-bit integer because there are math operations that
 * check against negative values. This should be OK as signed
 * 64-bit value has enough address space for devices that we
 * typically access on our target platform.
 */

typedef struct raw_file_s {
	raw_fsctx_t *raw_fsctx;
	int64_t raw_fileoffset;
	int64_t raw_baseoffset; /* starting offset of raw "file" */
	int64_t raw_length;     /* length of file, -1 for whole device */
} raw_file_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

static int raw_fileop_init(void **fsctx, void *devicename);
static int raw_fileop_open(void **ref, void *fsctx,
				const char *filename, int mode);
static int raw_fileop_read(void *ref, uint8_t *buf, int len);
static int raw_fileop_write(void *ref, uint8_t *buf, int len);
static int raw_fileop_seek(void *ref, int offset, int how);
static void raw_fileop_close(void *ref);
static void raw_fileop_uninit(void *fsctx);

/*  *********************************************************************
    *  RAW fileio dispatch table
    ********************************************************************* */

const fileio_dispatch_t raw_fileops = {
	.method    = "raw",
	.loadflags = 0,
	.init      = raw_fileop_init,
	.open      = raw_fileop_open,
	.read      = raw_fileop_read,
	.write     = raw_fileop_write,
	.seek      = raw_fileop_seek,
	.close     = raw_fileop_close,
	.uninit    = raw_fileop_uninit,
};

static int raw_fileop_init(void **newfsctx, void *dev)
{
	raw_fsctx_t *fsctx;
	char *devicename = (char *)dev;

	*newfsctx = NULL;

	fsctx = KMALLOC(sizeof(raw_fsctx_t), 0);
	if (!fsctx)
		return BOLT_ERR_NOMEM;

	if (console_name && !strcmp(devicename, console_name)) {
		fsctx->raw_dev = console_handle;
		fsctx->raw_isconsole = TRUE;
	} else {
		fsctx->raw_dev = bolt_open(devicename);
		fsctx->raw_isconsole = FALSE;
	}

	fsctx->raw_refcnt = 0;

	if (fsctx->raw_dev >= 0) {
		*newfsctx = fsctx;
		return 0;
	}

	KFREE(fsctx);

	return BOLT_ERR_FILENOTFOUND;
}

static int raw_fileop_open(void **ref, void *fsctx_arg, const char *filename,
			   int mode)
{
	raw_fsctx_t *fsctx;
	raw_file_t *file;
	char temp[100];
	char *len;

	if (mode != FILE_MODE_READ)
		return BOLT_ERR_UNSUPPORTED;

	fsctx = (raw_fsctx_t *) fsctx_arg;

	file = KMALLOC(sizeof(raw_file_t), 0);
	if (!file)
		return BOLT_ERR_NOMEM;

	file->raw_fileoffset = 0;
	file->raw_fsctx = fsctx;

	/* Assume the whole device. */
	file->raw_baseoffset = 0;
	file->raw_length = -1;

	/*
	 * If a filename was specified, it will be in the form
	 * offset,length - for example, 0x10000,0x200
	 * Parse this into two pieces and set up our internal
	 * file extent information.  you can use either decimal
	 * or "0x" notation.
	 */
	if (filename) {
		strncpy(temp, filename, sizeof(temp));
		len = strchr(temp, ',');
		if (len)
			*len++ = '\0';

		if (temp[0])
			file->raw_baseoffset = atoq(temp);

		if (len)
			file->raw_length = atoq(len);
	}

	fsctx->raw_refcnt++;

	*ref = file;
	return 0;
}

/* Limit number of bytes per read as imposed by underlying device 
 * E.g. if we are reading from USB, the limit is imposed by the
 * descriptor pool allocated. 
 * The current limit is based on USB descriptor pool size.
 * Other file system imposes limit based on its use-case.
 * E.g. FATFS limits it to 4K per-read. */
#define MAX_TRANSFER_SIZE (128*512)

static int raw_fileop_read(void *ref, uint8_t *buf, int len)
{
	raw_file_t *file = (raw_file_t *) ref;
	int res;
	int totalamt;
	unsigned char *curr_write_ptr;

	/*
	 * Bound the length based on our "file length" if one
	 * was specified.
	 */

	if (file->raw_length >= 0) {
		if ((file->raw_length - file->raw_fileoffset) < len)
			len = file->raw_length - file->raw_fileoffset;
	}

	if (len == 0)
		return 0;

	/*
	 * Read the data, adding in the base address.
	 */
	curr_write_ptr = buf;
	totalamt = 0;
	while (len >= MAX_TRANSFER_SIZE)
	{
		res = bolt_readblk(file->raw_fsctx->raw_dev,
			  file->raw_baseoffset + file->raw_fileoffset,
			  curr_write_ptr, MAX_TRANSFER_SIZE);
		if (res != MAX_TRANSFER_SIZE) {
			xprintf("I/O error. remaining bytes:%d\n", len);
			res = BOLT_ERR_IOERR;
			goto exit;
		}
		file->raw_fileoffset += res;
		curr_write_ptr += res;
		len -= res;
		totalamt += res;
		POLL();
	}

	if (len) {
		res = bolt_readblk(file->raw_fsctx->raw_dev,
			  file->raw_baseoffset + file->raw_fileoffset,
			  curr_write_ptr, len);
		if (res > 0) {
			file->raw_fileoffset += res;
			totalamt += res;
		}
	}

exit:
	return totalamt;
}

static int raw_fileop_write(void *ref, uint8_t *buf, int len)
{
	return BOLT_ERR_UNSUPPORTED;
}

static int raw_fileop_seek(void *ref, int offset, int how)
{
	raw_file_t *file = (raw_file_t *) ref;

	switch (how) {
	case FILE_SEEK_BEGINNING:
		file->raw_fileoffset = offset;
		break;
	case FILE_SEEK_CURRENT:
		file->raw_fileoffset += offset;
		if (file->raw_fileoffset < 0)
			file->raw_fileoffset = 0;
		break;
	default:
		break;
	}

	/*
	 * Make sure we don't attempt to seek past the end of the file.
	 */

	if (file->raw_length >= 0) {
		if (file->raw_fileoffset > file->raw_length)
			file->raw_fileoffset = file->raw_length;
	}

	return file->raw_fileoffset;
}

static void raw_fileop_close(void *ref)
{
	raw_file_t *file = (raw_file_t *) ref;

	file->raw_fsctx->raw_refcnt--;

	KFREE(file);
}

static void raw_fileop_uninit(void *fsctx_arg)
{
	raw_fsctx_t *fsctx = (raw_fsctx_t *) fsctx_arg;

	if (fsctx->raw_refcnt)
		xprintf("raw_fileop_uninit: warning: refcnt not zero\n");

	if (fsctx->raw_isconsole == FALSE)
		bolt_close(fsctx->raw_dev);

	KFREE(fsctx);
}
