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

#ifndef __IOCB_CONSTS_H__
 #define __IOCB_CONSTS_H__

#define BOLT_CMD_FW_GETINFO 	0
#define BOLT_CMD_FW_RESTART 	1
#define BOLT_CMD_FW_BOOT		2
#define BOLT_CMD_FW_CPUCTL		3
#define BOLT_CMD_FW_GETTIME 	4
#define BOLT_CMD_FW_MEMENUM 	5
#define BOLT_CMD_FW_FLUSHCACHE	6

#define BOLT_CMD_DEV_GETHANDLE	9
#define BOLT_CMD_DEV_ENUM		10
#define BOLT_CMD_DEV_OPEN		11
#define BOLT_CMD_DEV_INPSTAT	12
#define BOLT_CMD_DEV_READ		13
#define BOLT_CMD_DEV_WRITE		14
#define BOLT_CMD_DEV_IOCTL		15
#define BOLT_CMD_DEV_CLOSE		16
#define BOLT_CMD_DEV_GETINFO	17

#define BOLT_CMD_ENV_ENUM		20
#define BOLT_CMD_ENV_GET		22
#define BOLT_CMD_ENV_SET		23
#define BOLT_CMD_ENV_DEL		24

#define BOLT_CMD_MAX			32
#define BOLT_CMD_VENDOR_USE	0x8000 /* codes above this are for customer use */

#define BOLT_MI_RESERVED	0	/* memory is reserved, do not use */
#define BOLT_MI_AVAILABLE	1	/* memory is available */

#define BOLT_FLG_WARMSTART		0x00000001
#define BOLT_FLG_FULL_ARENA 	0x00000001
#define BOLT_FLG_ENV_PERMANENT	0x00000001

#define BOLT_CPU_CMD_START	1
#define BOLT_CPU_CMD_STOP	0

#define BOLT_DEV_NETWORK	1
#define BOLT_DEV_DISK		2
#define BOLT_DEV_FLASH		3
#define BOLT_DEV_SERIAL 	4
#define BOLT_DEV_CPU		5
#define BOLT_DEV_NVRAM		6
#define BOLT_DEV_CLOCK		7
#define BOLT_DEV_OTHER		8
#define BOLT_DEV_MEM		0x0A
#define BOLT_DEV_MASK		0x0F


/* Cache operation codes.
*/
#define BOLT_CACHE_FLUSH_D		1
#define BOLT_CACHE_INVAL_I		2
#define BOLT_CACHE_INVAL_D		4
#define BOLT_CACHE_INVAL_L2 	8
#define BOLT_CACHE_FLUSH_L2 	16
#define BOLT_CACHE_INVAL_RANGE	32
#define BOLT_CACHE_FLUSH_RANGE	64


/* Firmware
*/
#define BOLT_FWI_64BIT		0x00000001
#define BOLT_FWI_32BIT		0x00000002
#define BOLT_FWI_RELOC		0x00000004
#define BOLT_FWI_UNCACHED	0x00000008
#define BOLT_FWI_MULTICPU	0x00000010
#define BOLT_FWI_FUNCSIM	0x00000020
#define BOLT_FWI_RTLSIM 	0x00000040

#endif /* __IOCB_CONSTS_H__ */
