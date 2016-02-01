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
#include "lib_malloc.h"

#include "error.h"
#include "fileops.h"

#include "bolt.h"

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

extern const fileio_dispatch_t raw_fileops;
#if CFG_NETWORK
extern const fileio_dispatch_t tftp_fileops;
#endif
#if CFG_FATFS
extern const fileio_dispatch_t fatfs_fileops;
extern const fileio_dispatch_t pfatfs_fileops;
#endif
#if CFG_ZLIB
extern const fileio_dispatch_t zlibfs_fileops;
#endif

/*  *********************************************************************
    *  File system list
    ********************************************************************* */

static const fileio_dispatch_t *const bolt_filesystems[] = {
	&raw_fileops,
#if CFG_NETWORK
	&tftp_fileops,
#endif
#if CFG_FATFS
	&fatfs_fileops,
	&pfatfs_fileops,
#endif
#if CFG_ZLIB
	&zlibfs_fileops,
#endif
	NULL
};

/*  *********************************************************************
    *  bolt_findfilesys(name)
    *
    *  Locate the dispatch record for a particular file system.
    *
    *  Input parameters:
    *      name - name of filesys to locate
    *
    *  Return value:
    *      pointer to dispatch table or NULL if not found
    ********************************************************************* */

const fileio_dispatch_t *bolt_findfilesys(const char *name)
{
	const fileio_dispatch_t *const *disp;

	disp = bolt_filesystems;

	while (*disp) {
		if (strcmp((*disp)->method, name) == 0)
			return *disp;
		disp++;
	}

	return NULL;
}

/*  *********************************************************************
    *  fs_init(name,fsctx,device)
    *
    *  Initialize a filesystem context
    *
    *  Input parameters:
    *      name - name of file system
    *      fsctx - returns a filesystem context
    *      device - device name or other info
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */
int fs_init(const char *fsname, fileio_ctx_t **fsctx, void *device)
{
	fileio_ctx_t *ctx;
	int res;
	const fileio_dispatch_t *ops;

	ops = bolt_findfilesys(fsname);
	if (!ops)
		return BOLT_ERR_FSNOTAVAIL;

	ctx = (fileio_ctx_t *) KMALLOC(sizeof(fileio_ctx_t), 0);
	if (!ctx)
		return BOLT_ERR_NOMEM;

	ctx->ops = ops;
	res = BDINIT(ops, &(ctx->fsctx), device);

	if (res != 0) {
		KFREE(ctx);
		*fsctx = NULL;
	} else {
		*fsctx = ctx;
	}

	return res;
}

/*  *********************************************************************
    *  fs_hook(fsctx,name)
    *
    *  "Hook" a filesystem to attach a filter, like a decompression
    *  or decryption engine.
    *
    *  Input parameters:
    *      fsctx - result from a previous fs_init
    *      name - name of fs to hook onto the beginning
    *
    *  Return value:
    *      0 if ok
    *      else error
    ********************************************************************* */

int fs_hook(fileio_ctx_t *fsctx, const char *fsname)
{
	void *hookfsctx;
	const fileio_dispatch_t *ops;
	int res;

	/*
	 * Find the hook filesystem (well, actually a filter)
	 */

	ops = bolt_findfilesys(fsname);
	if (!ops)
		return BOLT_ERR_FSNOTAVAIL;

	/*
	 * initialize our hook file filter.
	 */

	res = BDINIT(ops, &hookfsctx, fsctx);
	if (res != 0)
		return res;

	/*
	 * Now replace dispatch table for current filesystem
	 * with the hook's dispatch table.  When fs_read is called,
	 * we'll go to the hook, and the hook will call the original.
	 *
	 * When fs_uninit is called, the hook will call the original's
	 * uninit routine.
	 */

	fsctx->ops = ops;
	fsctx->fsctx = hookfsctx;

	return 0;

}

/*  *********************************************************************
    *  fs_uninit(fsctx)
    *
    *  Uninitialize a file system context.
    *
    *  Input parameters:
    *      fsctx - filesystem context to remove (from fs_init)
    *
    *  Return value:
    *      0 if ok
    *      else error
    ********************************************************************* */
int fs_uninit(fileio_ctx_t *fsctx)
{
	BDUNINIT(fsctx->ops, fsctx->fsctx);

	KFREE(fsctx);

	return 0;
}

/*  *********************************************************************
    *  fs_open(fsctx,ref,filename,mode)
    *
    *  Open a file on the file system
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      ref - returns file handle
    *      filename - name of file to open
    *      mode - file open mode
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */
int fs_open(fileio_ctx_t *fsctx, void **ref, const char *filename, int mode)
{
	return BDOPEN2(fsctx->ops, ref, fsctx->fsctx, filename, mode);
}

/*  *********************************************************************
    *  fs_close(fsctx,ref)
    *
    *  Close a file on the file system
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      ref - file handle (from fs_open)
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */
int fs_close(fileio_ctx_t *fsctx, void *ref)
{
	BDCLOSE(fsctx->ops, ref);

	return 0;
}

/*  *********************************************************************
    *  fs_read(fsctx,ref,buffer,len)
    *
    *  Read data from the device.
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      ref - file handle (from fs_open)
    *      buffer - buffer pointer
    *      len - length
    *
    *  Return value:
    *      number of bytes read
    *      0=eof
    *      <0 = error
    ********************************************************************* */

int fs_read(fileio_ctx_t *fsctx, void *ref, uint8_t *buffer, int len)
{
	return BDREAD(fsctx->ops, ref, buffer, len);
}

/*  *********************************************************************
    *  fs_write(fsctx,ref,buffer,len)
    *
    *  write data from the device.
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      ref - file handle (from fs_open)
    *      buffer - buffer pointer
    *      len - length
    *
    *  Return value:
    *      number of bytes written
    *      0=eof
    *      <0 = error
    ********************************************************************* */

int fs_write(fileio_ctx_t *fsctx, void *ref, uint8_t *buffer, int len)
{
	return BDWRITE(fsctx->ops, ref, buffer, len);
}

/*  *********************************************************************
    *  fs_dir(fsctx,name)
    *
    *  Scan and print the named directory
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      name - name of directory
    *
    *  Return value:
    *      1 if dir or file found
    *      0 if file not found
    *      <0 = error
    ********************************************************************* */
int fs_dir(fileio_ctx_t *fsctx, char *name)
{
	if (!fsctx->ops->dir)
		return BOLT_ERR_UNSUPPORTED;
	return fsctx->ops->dir(fsctx->fsctx, name);
}

/*  *********************************************************************
    *  fs_seek(fsctx,ref,offset,how)
    *
    *  move file pointer on the device
    *
    *  Input parameters:
    *      fsctx - filesystem context (from fs_init)
    *      ref - file handle (from fs_open)
    *      offset - distance to move
    *      how - origin (FILE_SEEK_xxx)
    *
    *  Return value:
    *      new offset
    *      <0 = error
    ********************************************************************* */

int fs_seek(fileio_ctx_t *fsctx, void *ref, int offset, int how)
{
	return BDSEEK(fsctx->ops, ref, offset, how);
}
