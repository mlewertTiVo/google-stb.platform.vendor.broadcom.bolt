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

#ifndef __IOCB_H__
#define __IOCB_H__

#include "iocb-defs.h"

#define unsigned signed		/* Kludge to get unsigned size-shaped type. */
typedef __SIZE_TYPE__ bolt_int_t;
#undef unsigned
typedef __SIZE_TYPE__ bolt_uint_t;
typedef unsigned long long bolt_offset_t;
typedef long long bolt_int64_t;
typedef unsigned char *bolt_ptr_t;


typedef struct iocb_buffer_s {
	bolt_offset_t buf_offset;	/* offset on device (bytes) */
	bolt_ptr_t buf_ptr;	/* pointer to a buffer */
	bolt_uint_t buf_length;	/* length of this buffer */
	bolt_uint_t buf_retlen;	/* returned length (for read ops) */
	bolt_uint_t buf_ioctlcmd;	/* IOCTL command (used only for IOCTLs) */
} iocb_buffer_t;

#define buf_devflags buf_ioctlcmd	/* returned device info flags */

typedef struct iocb_inpstat_s {
	bolt_uint_t inp_status;	/* 1 means input available */
} iocb_inpstat_t;

typedef struct iocb_envbuf_s {
	int enum_idx;		/* 0-based enumeration index */
	bolt_ptr_t name_ptr;	/* name string buffer */
	bolt_int_t name_length;	/* size of name buffer */
	bolt_ptr_t val_ptr;	/* value string buffer */
	bolt_int_t val_length;	/* size of value string buffer */
} iocb_envbuf_t;

typedef struct iocb_cpuctl_s {
	bolt_uint_t cpu_number;	/* cpu number to control */
	bolt_uint_t cpu_command;	/* command to issue to CPU */
	bolt_uint_t start_addr;	/* CPU start address */
	bolt_uint_t gp_val;	/* starting GP value */
	bolt_uint_t sp_val;	/* starting SP value */
	bolt_uint_t a1_val;	/* starting A1 value */
} iocb_cpuctl_t;

typedef struct iocb_time_s {
	long long ticks;	/* current time in ticks */
} iocb_time_t;

typedef struct iocb_exitstat_s {
	bolt_int_t status;
} iocb_exitstat_t;

typedef struct iocb_meminfo_s {
	bolt_int_t mi_idx;	/* 0-based enumeration index */
	bolt_int_t mi_type;	/* type of memory block */
	bolt_int64_t mi_addr;	/* physical start address */
	bolt_int64_t mi_size;	/* block size */
} iocb_meminfo_t;

typedef struct iocb_fwinfo_s {
	bolt_int64_t fwi_version;	/* major, minor, eco version */
	bolt_int64_t fwi_totalmem;	/* total installed mem */
	bolt_int64_t fwi_flags;	/* various flags */
	bolt_int64_t fwi_boardid;	/* board ID */
	bolt_int64_t fwi_bootarea_va;	/* VA of boot area */
	bolt_int64_t fwi_bootarea_pa;	/* PA of boot area */
	bolt_int64_t fwi_bootarea_size;	/* size of boot area */
	bolt_int64_t fwi_reserved1;
	bolt_int64_t fwi_reserved2;
	bolt_int64_t fwi_reserved3;
} iocb_fwinfo_t;

typedef struct bolt_iocb_s {
	bolt_uint_t iocb_fcode;	/* IOCB function code */
	bolt_int_t iocb_status;	/* return status */
	bolt_int_t iocb_handle;	/* file/device handle */
	bolt_uint_t iocb_flags;	/* flags for this IOCB */
	bolt_uint_t iocb_psize;	/* size of parameter list */
	union {
		iocb_buffer_t iocb_buffer;	/* buffer parameters */
		iocb_inpstat_t iocb_inpstat;	/* input status parameters */
		iocb_envbuf_t iocb_envbuf;	/* environment function parameters */
		iocb_cpuctl_t iocb_cpuctl;	/* CPU control parameters */
		iocb_time_t iocb_time;	/* timer parameters */
		iocb_meminfo_t iocb_meminfo;	/* memory arena info parameters */
		iocb_fwinfo_t iocb_fwinfo;	/* firmware information */
		iocb_exitstat_t iocb_exitstat;	/* Exit Status */
	} plist;
} bolt_iocb_t;

int bolt_iocb_dispatch(bolt_iocb_t *iocb);
void bolt_device_poll(void *);

#endif /* __IOCB_H__ */

