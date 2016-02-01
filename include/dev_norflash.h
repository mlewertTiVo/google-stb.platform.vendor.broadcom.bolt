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

#ifndef  __DEV_NORFLASH_H__
#define  __DEV_NORFLASH_H__

#include "device.h"
#include "iocb.h"
#include "flash.h"

/*  *********************************************************************
    *  Flash magic numbers
    ********************************************************************* */

/*
 * AMD Flash commands and magic offsets
 */

#if defined (NOR_FLASH_8_BIT)
#define	AMD_FLASH_MAGIC_ADDR_1	0xAAA	/* AAA for 16-bit devices in 8-bit mode */
#define	AMD_FLASH_MAGIC_ADDR_2	0x555	/* 554 for 16-bit devices in 8-bit mode */
#define FLASH_CFI_QUERY_ADDR	0xAA
#else
#define	AMD_FLASH_MAGIC_ADDR_1	0x555	/* AAA for 16-bit devices in 8-bit mode */
#define	AMD_FLASH_MAGIC_ADDR_2	0x2AA	/* 554 for 16-bit devices in 8-bit mode */
#define FLASH_CFI_QUERY_ADDR	0x55
#endif

#define	AMD_FLASH_RESET		0xF0
#define	AMD_FLASH_MAGIC_1	0xAA
#define	AMD_FLASH_MAGIC_2	0x55
#define	AMD_FLASH_AUTOSEL	0x90
#define	AMD_FLASH_PROGRAM	0xA0
#define	AMD_FLASH_UNLOCK_BYPASS	0x20
#define	AMD_FLASH_ERASE_3	0x80
#define	AMD_FLASH_ERASE_4	0xAA
#define	AMD_FLASH_ERASE_5	0x55
#define	AMD_FLASH_ERASE_ALL_6	0x10
#define	AMD_FLASH_ERASE_SEC_6	0x30
#define AMD_CMD_LOCKREG_ENTRY   0x40
#define AMD_CMD_DYNAMIC_LOCKBIT_ENTRY  0xE0
#define AMD_CMD_DYB_CMD    0xA0
#define AMD_CMD_DYB_CLEAR  0x01
#define AMD_CMD_SECTOR_PROTECT_EXIT0        0x90
#define AMD_CMD_SECTOR_PROTECT_EXIT1        0x00

/*
 * INTEL Flash commands and magic offsets
 */

#define INTEL_FLASH_READ_MODE 0xFF
#define INTEL_FLASH_ERASE_BLOCK	0x20
#define INTEL_FLASH_ERASE_CONFIRM 0xD0
#define INTEL_FLASH_PROGRAM	0x40

/* INTEL Flash commands for 16-bit mode */
#define INTEL_FLASH_16BIT_READ_MODE 		0xFF00
#define INTEL_FLASH_16BIT_ERASE_BLOCK 		0x2000
#define INTEL_FLASH_16BIT_ERASE_CONFIRM 	0xD000
#define INTEL_FLASH_16BIT_PROGRAM 		0x4000
#define INTEL_FLASH_8BIT			0
#define INTEL_FLASH_16BIT			1

/*
 * Common Flash Interface (CFI) commands and offsets
 */

#define FLASH_CFI_QUERY_MODE	0x98
#define FLASH_CFI_QUERY_EXIT	0xFF

/*
 * JEDEC offsets
 */

#define FLASH_JEDEC_OFFSET_MFR	0
#define FLASH_JEDEC_OFFSET_DEV	1

/* Vendor-specific flash identifiers */

#define FLASH_MFR_HYUNDAI       0xAD

/*  *********************************************************************
    *  Macros for defining custom sector tables
    ********************************************************************* */

#define FLASH_SECTOR_RANGE(nblks,size) (((nblks)-1) << 16) + ((size)/256)
#define FLASH_SECTOR_NBLKS(x) (((x) >> 16)+1)
#define FLASH_SECTOR_SIZE(x) (((x) & 0xFFFF)*256)
#define FLASH_MAXSECTORS  16

/*  *********************************************************************
    *  Flashop engine constants and structures
    *  The flashop engine interprets a little table of commands
    *  to manipulate flash parts - we do this so we can operate
    *  on the flash that we're currently running from.
    ********************************************************************* */

/*
 *    opcode
 *    flash base
 *    destination
 *    source
 *    length
 *    result
 */
typedef struct flashinstr_s {
	long fi_op;
	long fi_base;
	long fi_dest;
	long fi_src;
	long fi_cnt;
	long fi_result;
} flashinstr_t;

/*
 * Flash opcodes
 */

#define FEOP_RETURN       0	/* return to BOLT */
#define FEOP_READ8	  2	/* read, 8 bits at a time */
#define FEOP_READ16	  3	/* read, 16 bits at a time */
#define FEOP_CFIQUERY8	  4	/* CFI Query 8-bit */
#define FEOP_CFIQUERY16	  5	/* CFI Query 16-bit */
#define FEOP_CFIQUERY16B  6	/* CFI Query 16-bit */
#define FEOP_MEMCPY	  7	/* generic memcpy */
#define FEOP_AMD_ERASE8	  8	/* AMD-style 8-bit erase-sector */
#define FEOP_AMD_ERASE16  9	/* AMD-style 16-bit erase-sector */
#define FEOP_AMD_ERASE16B 10	/* AMD-style 16-bit erase-sector */
#define FEOP_AMD_PGM8	  11	/* AMD-style 8-bit program */
#define FEOP_AMD_PGM16	  12	/* AMD-style 16-bit program */
#define FEOP_AMD_PGM16B   13	/* AMD-style 16-bit program */
#define FEOP_INTEL_ERASE8 14	/* Intel-style 8-bit erase-sector */
#define FEOP_INTEL_ERASE16 15	/* Intel-style 16-bit erase-sector */
#define FEOP_INTEL_PGM8	  16	/* Intel-style 8-bit program */
#define FEOP_INTEL_PGM16  17	/* Intel-style 16-bit program */
#define FEOP_AMD_RESET8     18
#define FEOP_AMD_RESET16    19
#define FEOP_AMD_RESET16_2B 20
#define FEOP_AMD_ERASE16_2B 21
#define FEOP_AMD_PGM16_2B   22

/*
 * Flashop result values.
 */

#define FERES_OK	  0
#define FERES_ERROR	  -1

/*
 * This clearly belongs somewhere else.
 * The flash sector buffer is a chunk of unallocated memory
 * used to do sector merging when writing arbitrary byte
 * ranges.  We don't want this in the heap, to keep
 * heaps small.

 * the fixed-address FLASH_SECTOR_BUFFER (0x800e0000) is causing memory corruption
 * when partitial flash partition merge is needed. As an example, the data is read in
 * FLASH_STAGING_BUFFER (0x80040000), if data size > 0xa0000, and partial merge is
 * needed, the memory corruption happens. Use kmalloc instead. It seems
 * safe to asusme max flash sector size is 128k.
 * 
 */

/* #define FLASH_SECTOR_BUFFER	UNCACADDR(1*1024*1024-128*1024) */
#define MAX_FLASH_SECTOR_SIZE (128 * 1024)

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

/*
 * Flags.  If you instantiate the driver with probe_a = physical
 * address and probe_b = size, you should also OR in the
 * bus and device width below.
 */

#define FLASH_FLG_AUTOSIZE 0x00000002	/* resize to actual device size */
#define FLASH_FLG_BUS8	   0
#define FLASH_FLG_BUS16	   0x00000004	/* ROM is connected to 16-bit bus */
#define FLASH_FLG_DEV8	   0
#define FLASH_FLG_DEV16	   0x00000010	/* ROM has 16-bit data width */
#define FLASH_FLG_MANUAL   0x00000008	/* Not CFI, manual sectoring */

#define FLASH_FLG_MASK	   0x000000FF	/* mask of probe bits for flags */
#define FLASH_SIZE_MASK    0xFFFFFF00	/* mask of probe bits for size */
/* you don't have to shift the size, we assume it's in multiples of 256bytes */

/*
 * Probe structure - this is used when we want to describe to the flash
 * driver the layout of our flash, particularly when you want to
 * manually describe the sectors.
 */

typedef struct norflash_probe_t {
	unsigned long flash_phys;	/* physical address of flash */
	int flash_flags;	/* flags (FLASH_FLG_xxx) */
	/* The following are used when manually sectoring */
	int flash_nsectors;	/* number of ranges */
	int flash_sectors[FLASH_MAXSECTORS];
} norflash_probe_t;

/*  *********************************************************************
    *  PRIVATE STRUCTURES
    *
    *  These structures are actually the "property" of the
    *  flash driver.  The only reason a board package might
    *  want to dig around in here is if it implements a hook
    *  or overrides functions to support special, weird flash parts.
    ********************************************************************* */

typedef struct flashdev_s flashdev_t;

struct flashdev_s {
	norflash_probe_t fd_probe;	/* probe information */
	struct flash_dev flash;		/* generic flash device */

	uint8_t fd_erasefunc;	/* Erase routine to use */
	uint8_t fd_pgmfunc;	/* program routine to use */
	uint8_t fd_readfunc;	/* Read routine to use */

	uint8_t *fd_sectorbuffer;	/* sector copy buffer */
	int fd_ttlsect;		/* total sectors on one device */

	int fd_iptr;		/* flashop engine instructions */
	flashinstr_t *fd_inst;
};

#endif /* __DEV_NORFLASH_H__ */
