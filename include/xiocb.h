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

#ifndef __XIOCB_H__
#define __XIOCB_H__

#include "iocb-defs.h"

typedef unsigned long long bolt_xuint_t;
typedef long long bolt_xint_t;
typedef long long bolt_xptr_t;


typedef struct xiocb_buffer_s {
	bolt_xuint_t buf_offset;	/* offset on device (bytes) */
	bolt_xptr_t buf_ptr;	/* pointer to a buffer */
	bolt_xuint_t buf_length;	/* length of this buffer */
	bolt_xuint_t buf_retlen;	/* returned length (for read ops) */
	bolt_xuint_t buf_ioctlcmd;	/* IOCTL command (used only for IOCTLs) */
} xiocb_buffer_t;

#define buf_devflags buf_ioctlcmd	/* returned device info flags */

typedef struct xiocb_inpstat_s {
	bolt_xuint_t inp_status;	/* 1 means input available */
} xiocb_inpstat_t;

typedef struct xiocb_envbuf_s {
	bolt_xint_t enum_idx;	/* 0-based enumeration index */
	bolt_xptr_t name_ptr;	/* name string buffer */
	bolt_xint_t name_length;	/* size of name buffer */
	bolt_xptr_t val_ptr;	/* value string buffer */
	bolt_xint_t val_length;	/* size of value string buffer */
} xiocb_envbuf_t;

typedef struct xiocb_cpuctl_s {
	bolt_xuint_t cpu_number;	/* cpu number to control */
	bolt_xuint_t cpu_command;	/* command to issue to CPU */
	bolt_xuint_t start_addr;	/* CPU start address */
	bolt_xuint_t gp_val;	/* starting GP value */
	bolt_xuint_t sp_val;	/* starting SP value */
	bolt_xuint_t a1_val;	/* starting A1 value */
} xiocb_cpuctl_t;

typedef struct xiocb_time_s {
	bolt_xint_t ticks;	/* current time in ticks */
} xiocb_time_t;

typedef struct xiocb_exitstat_s {
	bolt_xint_t status;
} xiocb_exitstat_t;

typedef struct xiocb_meminfo_s {
	bolt_xint_t mi_idx;	/* 0-based enumeration index */
	bolt_xint_t mi_type;	/* type of memory block */
	bolt_xuint_t mi_addr;	/* physical start address */
	bolt_xuint_t mi_size;	/* block size */
} xiocb_meminfo_t;

typedef struct xiocb_fwinfo_s {
	bolt_xint_t fwi_version;	/* major, minor, eco version */
	bolt_xint_t fwi_totalmem;	/* total installed mem */
	bolt_xint_t fwi_flags;	/* various flags */
	bolt_xint_t fwi_boardid;	/* board ID */
	bolt_xint_t fwi_bootarea_va;	/* VA of boot area */
	bolt_xint_t fwi_bootarea_pa;	/* PA of boot area */
	bolt_xint_t fwi_bootarea_size;	/* size of boot area */
	bolt_xint_t fwi_reserved1;
	bolt_xint_t fwi_reserved2;
	bolt_xint_t fwi_reserved3;
} xiocb_fwinfo_t;

typedef struct bolt_xiocb_s {
	bolt_xuint_t xiocb_fcode;	/* IOCB function code */
	bolt_xint_t xiocb_status;	/* return status */
	bolt_xint_t xiocb_handle;	/* file/device handle */
	bolt_xuint_t xiocb_flags;	/* flags for this IOCB */
	bolt_xuint_t xiocb_psize;	/* size of parameter list */
	union {
		xiocb_buffer_t xiocb_buffer;	/* buffer parameters */
		xiocb_inpstat_t xiocb_inpstat;	/* input status parameters */
		xiocb_envbuf_t xiocb_envbuf;	/* environment function parameters */
		xiocb_cpuctl_t xiocb_cpuctl;	/* CPU control parameters */
		xiocb_time_t xiocb_time;	/* timer parameters */
		xiocb_meminfo_t xiocb_meminfo;	/* memory arena info parameters */
		xiocb_fwinfo_t xiocb_fwinfo;	/* firmware information */
		xiocb_exitstat_t xiocb_exitstat;	/* Exit Status */
	} plist;
} bolt_xiocb_t;

#endif /* __XIOCB_H__ */

