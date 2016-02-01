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

#if !defined(__STUBS_H__)
#define __STUBS_H__

#if defined(_BOLT_)
#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "error.h"
#include "common.h"
#include "bchp_common.h"
#include "bchp_port0_sata3_pcb.h"
#include "timer.h"
#endif

#define assert(x)

#if !defined(_BOLT_)
#define NULL 0
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned int uintptr_t;
typedef unsigned int size_t;
#endif
typedef int FILE;
typedef int clockid_t;
struct timespec { int x; };

extern int printf(const char *fmt, ...);
extern int fprintf(FILE *fp, const char *fmt, ...);
extern void *malloc(size_t);
#if defined(_BOLT_)
extern void *memset(void *, int, size_t);
#endif
extern void usleep(unsigned long);
extern FILE *stderr;

#define SATA_MDIO_REG_SPACE_SIZE	  0x1000
#define SATA_MDIO_BASE(base, port)	 ((base) + \
					 ((port) * SATA_MDIO_REG_SPACE_SIZE) + \
					 0x100)
#define SATA_MDIO_BANK_OFFSET		  (BCHP_PORT0_SATA3_PCB_BLOCK_ADDR - \
					  BCHP_PORT0_SATA3_PCB_START)
#define SATA_MDIO_REG_OFFSET(ofs)	  ((ofs) * 4)
#define SATA3_TXPMD_REG_BANK		  0x1C0
#if !defined(REG)
#define REG(x) *((uint32_t *)(x))
#endif

#endif /* __STUBS_H__ */
