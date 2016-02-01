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

#ifndef __FILEOPS_H__
#define __FILEOPS_H__

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define FILE_SEEK_BEGINNING	0
#define FILE_SEEK_CURRENT	1

#define FILE_MODE_READ	1
#define FILE_MODE_WRITE	2

/*  *********************************************************************
    *  Macros
    ********************************************************************* */

#define BDINIT(ops,fsctx,name) (ops)->init((fsctx),(name))
#define BDOPEN(ops,ref,fsctx,name) (ops)->open((ref),(fsctx),(name),FILE_MODE_READ)
#define BDOPEN2(ops,ref,fsctx,name,mode) (ops)->open((ref),(fsctx),(name),(mode))
#define BDOPEN_WR(ops,ref,fsctx,name) (ops)->open((ref),(fsctx),(name),FILE_MODE_WRITE)
#define BDREAD(ops,ref,buf,len) (ops)->read((ref),(buf),(len))
#define BDWRITE(ops,ref,buf,len) (ops)->write((ref),(buf),(len))
#define BDCLOSE(ops,ref) (ops)->close((ref))
#define BDUNINIT(ops,ref) (ops)->uninit((ref))
#define BDSEEK(ops,ref,offset,how) (ops)->seek((ref),(offset),(how))

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct fileio_dispatch_s {
	const char *method;
	unsigned int loadflags;
	int (*init) (void **fsctx, void *device);
	int (*open) (void **ref, void *fsctx, const char *filename, int mode);
	int (*read) (void *ref, uint8_t * buf, int len);
	int (*write) (void *ref, uint8_t * buf, int len);
	int (*seek) (void *ref, int offset, int how);
	int (*dir) (void *fsctx, char *name);
	void (*close) (void *ref);
	void (*uninit) (void *devctx);
} fileio_dispatch_t;

typedef struct fileio_ctx_s {
	const fileio_dispatch_t *ops;
	void *fsctx;
} fileio_ctx_t;

const fileio_dispatch_t *bolt_findfilesys(const char *name);

int fs_init(const char *fsname, fileio_ctx_t ** fsctx, void *device);
int fs_uninit(fileio_ctx_t *);
int fs_open(fileio_ctx_t *, void **ref, const char *filename, int mode);
int fs_close(fileio_ctx_t *, void *ref);
int fs_read(fileio_ctx_t *, void *ref, uint8_t * buffer, int len);
int fs_write(fileio_ctx_t *, void *ref, uint8_t * buffer, int len);
int fs_seek(fileio_ctx_t *, void *ref, int offset, int how);
int fs_dir(fileio_ctx_t *fsctx, char *name);
int fs_hook(fileio_ctx_t * fsctx, const char *fsname);

#endif /* __FILEOPS_H__ */

