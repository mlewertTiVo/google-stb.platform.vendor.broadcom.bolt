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

#if CFG_ZLIB

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
#include <zlib-helpers.h>

#include "bolt.h"

#include "zlib.h"

/**********************************************************************
  *  ZLIBFS context
  **********************************************************************/

/*
 * File system context - describes overall file system info,
 * such as the handle to the underlying device.
 */

typedef struct zlibfs_fsctx_s {
	void *zlibfsctx_subfsctx;
	const fileio_dispatch_t *zlibfsctx_subops;
	int zlibfsctx_refcnt;
} zlibfs_fsctx_t;

/*
 * File context - describes an open file on the file system.
 * For raw devices, this is pretty meaningless, but we do
 * keep track of where we are.
 */

#define ZLIBFS_BUFSIZE	4096
typedef struct zlibfs_file_s {
	zlibfs_fsctx_t *zlibfs_fsctx;
	int zlibfs_fileoffset;
	void *zlibfs_subfile;
	z_stream zlibfs_stream;
	uint8_t *zlibfs_inbuf;
	uint8_t *zlibfs_outbuf;
	int zlibfs_outlen;
	uint8_t *zlibfs_outptr;
	int zlibfs_eofseen;
} zlibfs_file_t;

/**********************************************************************
  *  Prototypes
  **********************************************************************/

static int zlibfs_fileop_init(void **fsctx, void *ctx);
static int zlibfs_fileop_open(void **ref, void *fsctx, const char *filename,
			      int mode);
static int zlibfs_fileop_read(void *ref, uint8_t *buf, int len);
static int zlibfs_fileop_write(void *ref, uint8_t *buf, int len);
static int zlibfs_fileop_seek(void *ref, int offset, int how);
static void zlibfs_fileop_close(void *ref);
static void zlibfs_fileop_uninit(void *fsctx);

/**********************************************************************
  *  ZLIB fileio dispatch table
  **********************************************************************/

static uint8_t gz_magic[2] = { 0x1f, 0x8b };	/* gzip magic header */

/* gzip flag byte */
#define ASCII_FLAG   0x01	/* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02	/* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04	/* bit 2 set: extra field present */
#define ORIG_NAME    0x08	/* bit 3 set: original file name present */
#define COMMENT      0x10	/* bit 4 set: file comment present */
#define RESERVED     0xE0	/* bits 5..7: reserved */

const fileio_dispatch_t zlibfs_fileops = {
	.method    = "z",
	.loadflags = 0,
	.init      = zlibfs_fileop_init,
	.open      = zlibfs_fileop_open,
	.read      = zlibfs_fileop_read,
	.write     = zlibfs_fileop_write,
	.seek      = zlibfs_fileop_seek,
	.close     = zlibfs_fileop_close,
	.uninit    = zlibfs_fileop_uninit,
};

/*
 * Utility functions needed by the ZLIB routines.
 * These are prefixed so it's clear that they aren't the zlib versions which
 * are provided when !Z_SOLO.
 */
voidpf bolt_zcalloc(voidpf opaque, unsigned items, unsigned size)
{
	void *ptr;

	ptr = KMALLOC(items * size, 0);
	if (ptr)
		lib_memset(ptr, 0, items * size);
	return ptr;
}

void bolt_zcfree(voidpf opaque, voidpf ptr)
{
	KFREE(ptr);
}

/* TODO: temporary stub for linking, delete me once Z_SOLO is used */
voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
	return bolt_zcalloc(opaque, items, size);
}

/* TODO: temporary stub for linking, delete me once Z_SOLO is used */
void zcfree(voidpf opaque, voidpf ptr)
{
	bolt_zcfree(opaque, ptr);
}

static int zlibfs_fileop_init(void **newfsctx, void *curfsvoid)
{
	zlibfs_fsctx_t *fsctx;
	fileio_ctx_t *curfsctx = (fileio_ctx_t *) curfsvoid;

	*newfsctx = NULL;

	fsctx = KMALLOC(sizeof(zlibfs_fsctx_t), 0);
	if (!fsctx)
		return BOLT_ERR_NOMEM;

	fsctx->zlibfsctx_refcnt = 0;
	fsctx->zlibfsctx_subops = curfsctx->ops;
	fsctx->zlibfsctx_subfsctx = curfsctx->fsctx;

	*newfsctx = fsctx;

	return 0;
}

static int get_byte(zlibfs_file_t *file, uint8_t *ch)
{
	int res;

	res = BDREAD(file->zlibfs_fsctx->zlibfsctx_subops,
		     file->zlibfs_subfile, ch, 1);

	return res;
}

static int check_header(zlibfs_file_t *file)
{
	int method;		/* method byte */
	int flags;		/* flags byte */
	uInt len;
	uint8_t c;
	int res;

	/* Check the gzip magic header */
	for (len = 0; len < 2; len++) {
		res = get_byte(file, &c);
		if ((res != 1) || c != gz_magic[len])
			return -1;
	}

	get_byte(file, &c);
	method = (int)c;
	get_byte(file, &c);
	flags = (int)c;

	if (method != Z_DEFLATED || (flags & RESERVED) != 0)
		return -1;

	/* Discard time, xflags and OS code: */
	for (len = 0; len < 6; len++)
		(void)get_byte(file, &c);

	if ((flags & EXTRA_FIELD) != 0) {	/* skip the extra field */
		get_byte(file, &c);
		len = (uInt) c;
		get_byte(file, &c);
		len += ((uInt) c) << 8;

		/* len is garbage if EOF but the
		 loop below will quit anyway */
		while ((len-- != 0) && (get_byte(file, &c) == 1))
			;
	}

	if ((flags & ORIG_NAME) != 0) {	/* skip the original file name */
		while ((get_byte(file, &c) == 1) && (c != 0))
			;
	}

	if ((flags & COMMENT) != 0) {	/* skip the .gz file comment */
		while ((get_byte(file, &c) == 1) && (c != 0))
			;
	}

	if ((flags & HEAD_CRC) != 0) {	/* skip the header crc */
		for (len = 0; len < 2; len++)
			(void)get_byte(file, &c);
	}

	return 0;
}

static int zlibfs_fileop_open(void **ref, void *fsctx_arg, const char *filename,
			      int mode)
{
	zlibfs_fsctx_t *fsctx;
	zlibfs_file_t *file;
	int err;

	if (mode != FILE_MODE_READ)
		return BOLT_ERR_UNSUPPORTED;

	fsctx = (zlibfs_fsctx_t *) fsctx_arg;

	file = KMALLOC(sizeof(zlibfs_file_t), 0);
	if (!file)
		return BOLT_ERR_NOMEM;

	file->zlibfs_fileoffset = 0;
	file->zlibfs_fsctx = fsctx;
	file->zlibfs_inbuf = NULL;
	file->zlibfs_outbuf = NULL;
	file->zlibfs_eofseen = 0;

	err = BDOPEN(fsctx->zlibfsctx_subops, &(file->zlibfs_subfile),
		     fsctx->zlibfsctx_subfsctx, filename);

	if (err != 0) {
		goto error2;
		return err;
	}

	/* Open the zstream */

	file->zlibfs_inbuf = KMALLOC(ZLIBFS_BUFSIZE, 0);

	/* TODO: this is a hack. skip  mono image header. */

	file->zlibfs_outbuf = KMALLOC(ZLIBFS_BUFSIZE, 0);

	if (!file->zlibfs_inbuf || !file->zlibfs_outbuf) {
		err = BOLT_ERR_NOMEM;
		goto error;
	}

	file->zlibfs_stream.next_in = NULL;
	file->zlibfs_stream.avail_in = 0;
	file->zlibfs_stream.next_out = file->zlibfs_outbuf;
	file->zlibfs_stream.avail_out = ZLIBFS_BUFSIZE;
	file->zlibfs_stream.zalloc = bolt_zcalloc;
	file->zlibfs_stream.zfree = bolt_zcfree;

	file->zlibfs_outlen = 0;
	file->zlibfs_outptr = file->zlibfs_outbuf;

	err = inflateInit2(&(file->zlibfs_stream), -15);
	if (err != Z_OK) {
		err = BOLT_ERR;
		goto error;
	}

	check_header(file);

	fsctx->zlibfsctx_refcnt++;

	*ref = file;
	return 0;

error:
	BDCLOSE(file->zlibfs_fsctx->zlibfsctx_subops, file->zlibfs_subfile);
error2:
	if (file->zlibfs_inbuf)
		KFREE(file->zlibfs_inbuf);
	if (file->zlibfs_outbuf)
		KFREE(file->zlibfs_outbuf);
	KFREE(file);
	return err;
}

static int zlibfs_fileop_read(void *ref, uint8_t *buf, int len)
{
	zlibfs_file_t *file = (zlibfs_file_t *) ref;
	int res = 0;
	int err;
	int amtcopy;
	int ttlcopy = 0;

	if (len == 0)
		return 0;

	while (len) {

		/* Figure the amount to copy.  This is the min of what we
		   have left to do and what is available. */
		amtcopy = len;

		if (amtcopy > file->zlibfs_outlen)
			amtcopy = file->zlibfs_outlen;

		/* Copy the data. */

		if (buf) {
			memcpy(buf, file->zlibfs_outptr, amtcopy);
			buf += amtcopy;
		}

		/* Update the pointers. */
		file->zlibfs_outptr += amtcopy;
		file->zlibfs_outlen -= amtcopy;
		len -= amtcopy;
		ttlcopy += amtcopy;

		/* If we've eaten all of the output, reset and call inflate
		   again. */

		if (file->zlibfs_outlen == 0) {

			/* If no input data to decompress,
			get some more if we can. */
			if (file->zlibfs_eofseen)
				break;

			if (file->zlibfs_stream.avail_in == 0) {
				res =
				    BDREAD(file->zlibfs_fsctx->zlibfsctx_subops,
					   file->zlibfs_subfile,
					   file->zlibfs_inbuf, ZLIBFS_BUFSIZE);
				/* If at EOF or error, get out. */
				if (res <= 0)
					break;
				file->zlibfs_stream.next_in =
				    file->zlibfs_inbuf;
				file->zlibfs_stream.avail_in = res;
			}

			/* inflate the input data. */
			file->zlibfs_stream.next_out = file->zlibfs_outbuf;
			file->zlibfs_stream.avail_out = ZLIBFS_BUFSIZE;
			file->zlibfs_outptr = file->zlibfs_outbuf;

			err = inflate(&(file->zlibfs_stream), Z_SYNC_FLUSH);

			if (err == Z_STREAM_END) {
				/* We can get a partial buffer fill here. */
				file->zlibfs_eofseen = 1;
			} else if (err != Z_OK) {
				res = BOLT_ERR;
				break;
			}

			file->zlibfs_outlen = file->zlibfs_stream.next_out -
			    file->zlibfs_outptr;
		}

	}

	file->zlibfs_fileoffset += ttlcopy;

	return (res < 0) ? res : ttlcopy;
}

static int zlibfs_fileop_write(void *ref, uint8_t *buf, int len)
{
	return BOLT_ERR_UNSUPPORTED;
}

static int zlibfs_fileop_seek(void *ref, int offset, int how)
{
	zlibfs_file_t *file = (zlibfs_file_t *) ref;
	int res;
	int delta;

	switch (how) {
	case FILE_SEEK_BEGINNING:
		delta = offset - file->zlibfs_fileoffset;
		break;
	case FILE_SEEK_CURRENT:
		delta = offset;
		break;
	default:
		return BOLT_ERR_UNSUPPORTED;
		break;
	}

	/* backward seeking not allowed on compressed streams */
	if (delta < 0)
		return BOLT_ERR_UNSUPPORTED;

	res = zlibfs_fileop_read(ref, NULL, delta);

	if (res < 0)
		return res;

	return file->zlibfs_fileoffset;
}

static void zlibfs_fileop_close(void *ref)
{
	zlibfs_file_t *file = (zlibfs_file_t *) ref;

	file->zlibfs_fsctx->zlibfsctx_refcnt--;

	inflateEnd(&(file->zlibfs_stream));

	BDCLOSE(file->zlibfs_fsctx->zlibfsctx_subops, file->zlibfs_subfile);

	KFREE(file);
}

static void zlibfs_fileop_uninit(void *fsctx_arg)
{
	zlibfs_fsctx_t *fsctx = (zlibfs_fsctx_t *) fsctx_arg;

	if (fsctx->zlibfsctx_refcnt)
		xprintf("zlibfs_fileop_uninit: warning: refcnt not zero\n");

	BDUNINIT(fsctx->zlibfsctx_subops, fsctx->zlibfsctx_subfsctx);

	KFREE(fsctx);
}

#endif
