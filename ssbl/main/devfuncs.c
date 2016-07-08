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

#include "bchp_sun_top_ctrl.h"

#include "lib_types.h"
#include "lib_string.h"
#include "iocb.h"
#include "devfuncs.h"

int bolt_open(char *name)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_OPEN;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = 0;
	iocb.plist.iocb_buffer.buf_ptr = (unsigned char *)name;
	iocb.plist.iocb_buffer.buf_length = strlen(name);

	bolt_iocb_dispatch(&iocb);

	return (iocb.iocb_status < 0) ? iocb.iocb_status : iocb.iocb_handle;
}

int bolt_gethandle(char *name)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_GETHANDLE;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = 0;
	iocb.plist.iocb_buffer.buf_ptr = (unsigned char *)name;
	iocb.plist.iocb_buffer.buf_length = strlen(name);

	bolt_iocb_dispatch(&iocb);

	return (iocb.iocb_status < 0) ? iocb.iocb_status : iocb.iocb_handle;
}

int bolt_close(int handle)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_CLOSE;
	iocb.iocb_status = 0;
	iocb.iocb_handle = handle;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = 0;

	bolt_iocb_dispatch(&iocb);

	return iocb.iocb_status;

}

int bolt_readblk(int handle, bolt_offset_t offset, unsigned char *buffer,
		int length)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_READ;
	iocb.iocb_status = 0;
	iocb.iocb_handle = handle;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = offset;
	iocb.plist.iocb_buffer.buf_ptr = buffer;
	iocb.plist.iocb_buffer.buf_length = length;
	bolt_iocb_dispatch(&iocb);
	return (iocb.iocb_status <
		0) ? iocb.iocb_status : (int)iocb.plist.iocb_buffer.buf_retlen;
}

int bolt_read(int handle, unsigned char *buffer, int length)
{
	return bolt_readblk(handle, 0, buffer, length);
}

int bolt_writeblk(int handle, bolt_offset_t offset, unsigned char *buffer,
		 int length)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_WRITE;
	iocb.iocb_status = 0;
	iocb.iocb_handle = handle;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = offset;
	iocb.plist.iocb_buffer.buf_ptr = buffer;
	iocb.plist.iocb_buffer.buf_length = length;

	bolt_iocb_dispatch(&iocb);

	return (iocb.iocb_status <
		0) ? iocb.iocb_status : (int)iocb.plist.iocb_buffer.buf_retlen;
}

int bolt_write(int handle, unsigned char *buffer, int length)
{
	return bolt_writeblk(handle, 0, buffer, length);
}

int bolt_ioctl(int handle, unsigned int ioctlnum,
	      void *buffer, size_t length, size_t *retlen,
	      bolt_offset_t offset)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_IOCTL;
	iocb.iocb_status = 0;
	iocb.iocb_handle = handle;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = offset;
	iocb.plist.iocb_buffer.buf_ioctlcmd = (bolt_offset_t) ioctlnum;
	iocb.plist.iocb_buffer.buf_ptr = buffer;
	iocb.plist.iocb_buffer.buf_length = length;

	bolt_iocb_dispatch(&iocb);

	if (retlen)
		*retlen = iocb.plist.iocb_buffer.buf_retlen;
	return iocb.iocb_status;
}

int bolt_inpstat(int handle)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_INPSTAT;
	iocb.iocb_status = 0;
	iocb.iocb_handle = handle;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_inpstat_t);
	iocb.plist.iocb_inpstat.inp_status = 0;

	bolt_iocb_dispatch(&iocb);

	if (iocb.iocb_status < 0)
		return iocb.iocb_status;

	return iocb.plist.iocb_inpstat.inp_status;

}

long long bolt_getticks(void)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_FW_GETTIME;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_time_t);
	iocb.plist.iocb_time.ticks = 0;

	bolt_iocb_dispatch(&iocb);

	return iocb.plist.iocb_time.ticks;

}

int bolt_getenv(char *name, char *dest, int destlen)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	*dest = '\0';

	iocb.iocb_fcode = BOLT_CMD_ENV_GET;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_envbuf_t);
	iocb.plist.iocb_envbuf.enum_idx = 0;
	iocb.plist.iocb_envbuf.name_ptr = (unsigned char *)name;
	iocb.plist.iocb_envbuf.name_length = strlen(name) + 1;
	iocb.plist.iocb_envbuf.val_ptr = (unsigned char *)dest;
	iocb.plist.iocb_envbuf.val_length = destlen;

	bolt_iocb_dispatch(&iocb);

	return iocb.iocb_status;
}

int bolt_exit(int warm, int code)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_FW_RESTART;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = warm ? BOLT_FLG_WARMSTART : 0;
	iocb.iocb_psize = sizeof(iocb_exitstat_t);
	iocb.plist.iocb_exitstat.status = code;

	bolt_iocb_dispatch(&iocb);

	return iocb.iocb_status;

}

int bolt_flushcache(int flg)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_FW_FLUSHCACHE;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = flg;
	iocb.iocb_psize = 0;

	bolt_iocb_dispatch(&iocb);

	return iocb.iocb_status;
}

int bolt_getdevinfo(char *name)
{
	bolt_iocb_t iocb;

	memset(&iocb, 0, sizeof(iocb));

	iocb.iocb_fcode = BOLT_CMD_DEV_GETINFO;
	iocb.iocb_status = 0;
	iocb.iocb_handle = 0;
	iocb.iocb_flags = 0;
	iocb.iocb_psize = sizeof(iocb_buffer_t);
	iocb.plist.iocb_buffer.buf_offset = 0;
	iocb.plist.iocb_buffer.buf_ptr = (unsigned char *)name;
	iocb.plist.iocb_buffer.buf_length = strlen(name);

	bolt_iocb_dispatch(&iocb);

	return (iocb.iocb_status <
		0) ? iocb.iocb_status : (int)iocb.plist.
	    iocb_buffer.buf_devflags;
}
