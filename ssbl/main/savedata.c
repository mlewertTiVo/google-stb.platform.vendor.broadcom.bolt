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

#include "iocb.h"
#include "device.h"
#include "error.h"
#include "devfuncs.h"

#include "bolt.h"
#include "fileops.h"

#include "loader.h"

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

/*  *********************************************************************
    *  bolt_savedata(fsname,filename,start,end)
    *
    *  Write memory contents to the specified device
    *
    *  Input parameters:
    *      fsname - name of file system
    *      filename - name of file within file system
    *      start - starting address (pointer)
    *      end - ending address (pointer)
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */
int bolt_savedata(char *fsname, char *devname, char *filename, uint8_t *start,
		 uint8_t *end)
{
	int res;
	fileio_ctx_t *fsctx;
	void *ref;

	/*
	 * Create a file system context
	 */

	res = fs_init(fsname, &fsctx, devname);
	if (res != 0)
		return res;

	/*
	 * Open the device
	 */

	res = fs_open(fsctx, &ref, filename, FILE_MODE_WRITE);
	if (res != 0) {
		fs_uninit(fsctx);
		return res;
	}

	/*
	 * Write the data
	 */

	res = fs_write(fsctx, ref, start, end - start);

	/*
	 * Close
	 */

	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	return res;

}
