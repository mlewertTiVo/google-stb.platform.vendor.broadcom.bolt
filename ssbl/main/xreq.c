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
#include "lib_malloc.h"
#include "lib_queue.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "xiocb.h"
#if CFG_VENDOR_EXTENSIONS
#include "vendor_iocb.h"
#include "vendor_xiocb.h"
#endif
#include "error.h"
#include "device.h"
#include "timer.h"
#include "env_subr.h"
#include "bolt.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

/* enum values for various plist types */

#define PLBUF	1		/* iocb_buffer_t */
#define PLCPU	2		/* iocb_cpuctl_t */
#define PLMEM	3		/* iocb_meminfo_t */
#define PLENV	4		/* iocb_envbuf_t */
#define PLINP	5		/* iocb_inpstat_t */
#define PLTIM	6		/* iocb_time_t */
#define PLINF   7		/* iocb_fwinfo_t */
#define PLEXIT  8		/* iocb_exitstat_t */

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

struct bolt_xcmd_dispatch_s {
	unsigned int xplistsize;
	int iplistsize;
	int plisttype;
};

/*  *********************************************************************
    *  Command conversion table
    *  This table contains useful information for converting
    *  iocbs to xiocbs.
    ********************************************************************* */

static const struct bolt_xcmd_dispatch_s
		bolt_xcmd_dispatch_table[BOLT_CMD_MAX] = {
	{sizeof(xiocb_fwinfo_t), sizeof(iocb_fwinfo_t), PLINF}
	,			/* 0 : BOLT_CMD_FW_GETINFO */
	{sizeof(xiocb_exitstat_t), sizeof(iocb_exitstat_t), PLEXIT}
	,			/* 1 : BOLT_CMD_FW_RESTART */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 2 : BOLT_CMD_FW_BOOT */
	{sizeof(xiocb_cpuctl_t), sizeof(iocb_cpuctl_t), PLCPU}
	,			/* 3 : BOLT_CMD_FW_CPUCTL */
	{sizeof(xiocb_time_t), sizeof(iocb_time_t), PLTIM}
	,			/* 4 : BOLT_CMD_FW_GETTIME */
	{sizeof(xiocb_meminfo_t), sizeof(iocb_meminfo_t), PLMEM}
	,			/* 5 : BOLT_CMD_FW_MEMENUM */
	{0, 0, 0}
	,			/* 6 : BOLT_CMD_FW_FLUSHCACHE */
	{-1, 0, 0}
	,			/* 7 : */
	{-1, 0, 0}
	,			/* 8 : */
	{0, 0, 0}
	,			/* 9 : BOLT_CMD_DEV_GETHANDLE */
	{sizeof(xiocb_envbuf_t), sizeof(iocb_envbuf_t), PLENV}
	,			/* 10 : BOLT_CMD_DEV_ENUM */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 11 : BOLT_CMD_DEV_OPEN_ */
	{sizeof(xiocb_inpstat_t), sizeof(iocb_inpstat_t), PLINP}
	,			/* 12 : BOLT_CMD_DEV_INPSTAT */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 13 : BOLT_CMD_DEV_READ */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 14 : BOLT_CMD_DEV_WRITE */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 15 : BOLT_CMD_DEV_IOCTL */
	{0, 0, 0}
	,			/* 16 : BOLT_CMD_DEV_CLOSE */
	{sizeof(xiocb_buffer_t), sizeof(iocb_buffer_t), PLBUF}
	,			/* 17 : BOLT_CMD_DEV_GETINFO */
	{-1, 0, 0}
	,			/* 18 : */
	{-1, 0, 0}
	,			/* 19 : */
	{sizeof(xiocb_envbuf_t), sizeof(iocb_envbuf_t), PLENV}
	,			/* 20 : BOLT_CMD_ENV_ENUM */
	{-1, 0, 0}
	,			/* 21 : */
	{sizeof(xiocb_envbuf_t), sizeof(iocb_envbuf_t), PLENV}
	,			/* 22 : BOLT_CMD_ENV_GET */
	{sizeof(xiocb_envbuf_t), sizeof(iocb_envbuf_t), PLENV}
	,			/* 23 : BOLT_CMD_ENV_SET */
	{sizeof(xiocb_envbuf_t), sizeof(iocb_envbuf_t), PLENV}
	,			/* 24 : BOLT_CMD_ENV_DEL */
	{-1, 0, 0}
	,			/* 25 : */
	{-1, 0, 0}
	,			/* 26 : */
	{-1, 0, 0}
	,			/* 27 : */
	{-1, 0, 0}
	,			/* 28 : */
	{-1, 0, 0}
	,			/* 29 : */
	{-1, 0, 0}
	,			/* 30 : */
	{-1, 0, 0}		/* 31 : */
};

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

extern int bolt_iocb_dispatch(bolt_iocb_t *iocb);
bolt_int_t bolt_doxreq(bolt_xiocb_t *xiocb);
#if CFG_VENDOR_EXTENSIONS
extern bolt_int_t bolt_vendor_doxreq(bolt_vendor_xiocb_t *xiocb);
#endif

/*  *********************************************************************
    *  bolt_doxreq(xiocb)
    *
    *  Process an xiocb request.  This routine converts an xiocb
    *  into an iocb, calls the IOCB dispatcher, converts the results
    *  back into the xiocb, and returns.
    *
    *  Input parameters:
    *	xiocb - pointer to user xiocb
    *
    *  Return value:
    *	command status, <0 if error occured
    ********************************************************************* */

bolt_int_t bolt_doxreq(bolt_xiocb_t *xiocb)
{
	const struct bolt_xcmd_dispatch_s *disp;
	bolt_iocb_t iiocb;
	bolt_int_t res;

	/*
	 * Call out to customer-specific IOCBs.  Customers may choose
	 * to implement their own XIOCBs directly, or go through their own
	 * translation layer (xiocb->iocb like BOLT does) to insulate
	 * themselves from IOCB changes in the future.
	 */
	if (xiocb->xiocb_fcode >= BOLT_CMD_VENDOR_USE) {
#if CFG_VENDOR_EXTENSIONS
		return bolt_vendor_doxreq((bolt_vendor_xiocb_t *) xiocb);
#else
		return BOLT_ERR_INV_COMMAND;
#endif
	}

	/*
	 * Check for commands codes out of range
	 */

	if (xiocb->xiocb_fcode >= BOLT_CMD_MAX) {
		xiocb->xiocb_status = BOLT_ERR_INV_COMMAND;
		return xiocb->xiocb_status;
	}

	/*
	 * Check for command codes in range but invalid
	 */

	disp = &bolt_xcmd_dispatch_table[xiocb->xiocb_fcode];

	/*
	 * Check for invalid parameter list size
	 */

	if (disp->xplistsize != xiocb->xiocb_psize) {
		xiocb->xiocb_status = BOLT_ERR_INV_PARAM;
		return xiocb->xiocb_status;
	}

	/*
	 * Okay, copy parameters into the internal IOCB.
	 * First, the fixed header.
	 */

	iiocb.iocb_fcode = (unsigned int)xiocb->xiocb_fcode;
	iiocb.iocb_status = (int)xiocb->xiocb_status;
	iiocb.iocb_handle = (int)xiocb->xiocb_handle;
	iiocb.iocb_flags = (unsigned int)xiocb->xiocb_flags;
	iiocb.iocb_psize = (unsigned int)disp->iplistsize;

	/*
	 * Now the parameter list
	 */

	switch (disp->plisttype) {
	case PLBUF:
		iiocb.plist.iocb_buffer.buf_offset =
		    (bolt_offset_t) xiocb->plist.xiocb_buffer.buf_offset;
		iiocb.plist.iocb_buffer.buf_ptr =
		    (unsigned char *)(uintptr_t) xiocb->plist.
		    xiocb_buffer.buf_ptr;
		iiocb.plist.iocb_buffer.buf_length =
		    (unsigned int)xiocb->plist.xiocb_buffer.buf_length;
		iiocb.plist.iocb_buffer.buf_retlen =
		    (unsigned int)xiocb->plist.xiocb_buffer.buf_retlen;
		iiocb.plist.iocb_buffer.buf_ioctlcmd =
		    (unsigned int)xiocb->plist.xiocb_buffer.buf_ioctlcmd;
		break;
	case PLCPU:
		iiocb.plist.iocb_cpuctl.cpu_number =
		    (unsigned int)xiocb->plist.xiocb_cpuctl.cpu_number;
		iiocb.plist.iocb_cpuctl.cpu_command =
		    (unsigned int)xiocb->plist.xiocb_cpuctl.cpu_command;
		iiocb.plist.iocb_cpuctl.start_addr =
		    (unsigned long)xiocb->plist.xiocb_cpuctl.start_addr;
		iiocb.plist.iocb_cpuctl.gp_val =
		    (unsigned long)xiocb->plist.xiocb_cpuctl.gp_val;
		iiocb.plist.iocb_cpuctl.sp_val =
		    (unsigned long)xiocb->plist.xiocb_cpuctl.sp_val;
		iiocb.plist.iocb_cpuctl.a1_val =
		    (unsigned long)xiocb->plist.xiocb_cpuctl.a1_val;
		break;
	case PLMEM:
		iiocb.plist.iocb_meminfo.mi_idx =
		    (int)xiocb->plist.xiocb_meminfo.mi_idx;
		iiocb.plist.iocb_meminfo.mi_type =
		    (int)xiocb->plist.xiocb_meminfo.mi_type;
		iiocb.plist.iocb_meminfo.mi_addr =
		    (unsigned long long)xiocb->plist.xiocb_meminfo.mi_addr;
		iiocb.plist.iocb_meminfo.mi_size =
		    (unsigned long long)xiocb->plist.xiocb_meminfo.mi_size;
		break;
	case PLENV:
		iiocb.plist.iocb_envbuf.enum_idx =
		    (int)xiocb->plist.xiocb_envbuf.enum_idx;
		iiocb.plist.iocb_envbuf.name_ptr =
		    (unsigned char *)(uintptr_t) xiocb->plist.
		    xiocb_envbuf.name_ptr;
		iiocb.plist.iocb_envbuf.name_length =
		    (int)xiocb->plist.xiocb_envbuf.name_length;
		iiocb.plist.iocb_envbuf.val_ptr =
		    (unsigned char *)(uintptr_t) xiocb->plist.
		    xiocb_envbuf.val_ptr;
		iiocb.plist.iocb_envbuf.val_length =
		    (int)xiocb->plist.xiocb_envbuf.val_length;
		break;
	case PLINP:
		iiocb.plist.iocb_inpstat.inp_status =
		    (int)xiocb->plist.xiocb_inpstat.inp_status;
		break;
	case PLTIM:
		iiocb.plist.iocb_time.ticks =
		    (long long)xiocb->plist.xiocb_time.ticks;
		break;
	case PLINF:
		break;
	case PLEXIT:
		iiocb.plist.iocb_exitstat.status =
		    (long long)xiocb->plist.xiocb_exitstat.status;
		break;
	}

	/*
	 * Do the internal function dispatch
	 */

	res = (bolt_int_t) bolt_iocb_dispatch(&iiocb);
	if (res)
		return res;

	/*
	 * Now convert the parameter list members back
	 */

	switch (disp->plisttype) {
	case PLBUF:
		xiocb->plist.xiocb_buffer.buf_offset =
		    (bolt_uint_t) iiocb.plist.iocb_buffer.buf_offset;
		xiocb->plist.xiocb_buffer.buf_ptr =
		    (bolt_xptr_t) (uintptr_t) iiocb.plist.iocb_buffer.buf_ptr;
		xiocb->plist.xiocb_buffer.buf_length =
		    (bolt_uint_t) iiocb.plist.iocb_buffer.buf_length;
		xiocb->plist.xiocb_buffer.buf_retlen =
		    (bolt_uint_t) iiocb.plist.iocb_buffer.buf_retlen;
		xiocb->plist.xiocb_buffer.buf_ioctlcmd =
		    (bolt_uint_t) iiocb.plist.iocb_buffer.buf_ioctlcmd;
		break;
	case PLCPU:
		xiocb->plist.xiocb_cpuctl.cpu_number =
		    (bolt_uint_t) iiocb.plist.iocb_cpuctl.cpu_number;
		xiocb->plist.xiocb_cpuctl.cpu_command =
		    (bolt_uint_t) iiocb.plist.iocb_cpuctl.cpu_command;
		xiocb->plist.xiocb_cpuctl.start_addr =
		    (bolt_uint_t) iiocb.plist.iocb_cpuctl.start_addr;
		break;
	case PLMEM:
		xiocb->plist.xiocb_meminfo.mi_idx =
		    (bolt_int_t) iiocb.plist.iocb_meminfo.mi_idx;
		xiocb->plist.xiocb_meminfo.mi_type =
		    (bolt_int_t) iiocb.plist.iocb_meminfo.mi_type;
		xiocb->plist.xiocb_meminfo.mi_addr =
		    (bolt_int64_t) iiocb.plist.iocb_meminfo.mi_addr;
		xiocb->plist.xiocb_meminfo.mi_size =
		    (bolt_int64_t) iiocb.plist.iocb_meminfo.mi_size;
		break;
	case PLENV:
		xiocb->plist.xiocb_envbuf.enum_idx =
		    (bolt_int_t) iiocb.plist.iocb_envbuf.enum_idx;
		xiocb->plist.xiocb_envbuf.name_ptr =
		    (bolt_xptr_t) (uintptr_t) iiocb.plist.iocb_envbuf.name_ptr;
		xiocb->plist.xiocb_envbuf.name_length =
		    (bolt_int_t) iiocb.plist.iocb_envbuf.name_length;
		xiocb->plist.xiocb_envbuf.val_ptr =
		    (bolt_xptr_t) (uintptr_t) iiocb.plist.iocb_envbuf.val_ptr;
		xiocb->plist.xiocb_envbuf.val_length =
		    (bolt_int_t) iiocb.plist.iocb_envbuf.val_length;
		break;
	case PLINP:
		xiocb->plist.xiocb_inpstat.inp_status =
		    (bolt_int_t) iiocb.plist.iocb_inpstat.inp_status;
		break;
	case PLTIM:
		xiocb->plist.xiocb_time.ticks =
		    (bolt_int_t) iiocb.plist.iocb_time.ticks;
		break;
	case PLINF:
		xiocb->plist.xiocb_fwinfo.fwi_version =
		    iiocb.plist.iocb_fwinfo.fwi_version;
		xiocb->plist.xiocb_fwinfo.fwi_totalmem =
		    iiocb.plist.iocb_fwinfo.fwi_totalmem;
		xiocb->plist.xiocb_fwinfo.fwi_flags =
		    iiocb.plist.iocb_fwinfo.fwi_flags;
		xiocb->plist.xiocb_fwinfo.fwi_boardid =
		    iiocb.plist.iocb_fwinfo.fwi_boardid;
		xiocb->plist.xiocb_fwinfo.fwi_bootarea_va =
		    iiocb.plist.iocb_fwinfo.fwi_bootarea_va;
		xiocb->plist.xiocb_fwinfo.fwi_bootarea_pa =
		    iiocb.plist.iocb_fwinfo.fwi_bootarea_pa;
		xiocb->plist.xiocb_fwinfo.fwi_bootarea_size =
		    iiocb.plist.iocb_fwinfo.fwi_bootarea_size;
		xiocb->plist.xiocb_fwinfo.fwi_reserved1 =
		    iiocb.plist.iocb_fwinfo.fwi_reserved1;
		xiocb->plist.xiocb_fwinfo.fwi_reserved2 =
		    iiocb.plist.iocb_fwinfo.fwi_reserved2;
		xiocb->plist.xiocb_fwinfo.fwi_reserved3 =
		    iiocb.plist.iocb_fwinfo.fwi_reserved3;
		break;
	case PLEXIT:
		xiocb->plist.xiocb_exitstat.status =
		    (bolt_int_t) iiocb.plist.iocb_exitstat.status;
		break;
	}

	/*
	 * And the fixed header
	 */

	xiocb->xiocb_status = (bolt_int_t) iiocb.iocb_status;
	xiocb->xiocb_handle = (bolt_int_t) iiocb.iocb_handle;
	xiocb->xiocb_flags = (bolt_uint_t) iiocb.iocb_flags;

	return xiocb->xiocb_status;
}
