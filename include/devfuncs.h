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

#ifndef __DEVFUNCS_H__
#define __DEVFUNCS_H__

#ifndef __ASSEMBLER__
#include "lib_types.h"
#include "iocb.h"

int bolt_open(char *name);
int bolt_close(int handle);
int bolt_readblk(int handle, bolt_offset_t offset, unsigned char *buffer,
		int length);
int bolt_read(int handle, unsigned char *buffer, int length);
int bolt_writeblk(int handle, bolt_offset_t offset, unsigned char *buffer,
		 int length);
int bolt_write(int handle, unsigned char *buffer, int length);
int bolt_ioctl(int handle, unsigned int ioctlnum, void *buffer,
	      size_t length, size_t *retlen, bolt_offset_t offset);
int bolt_inpstat(int handle);
int bolt_getenv(char *name, char *dest, int destlen);
long long bolt_getticks(void);
int bolt_exit(int warm, int code);
int bolt_flushcache(int flg);
int bolt_getdevinfo(char *name);
int bolt_flushcache(int);
#endif

#endif /* __DEVFUNCS_H__ */

