/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BOOTBLOCK_H__
#define __BOOTBLOCK_H__

#include <stdint.h>

/*
 * BOLT boot block, modeled loosely on Alpha.
 *
 * It consists of:
 *
 * 		BSD disk label
 *		<blank space>
 *		Boot block info (5 u_int_64s)
 *
 * The boot block portion looks like:
 *
 *
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *      |                        BOOT_MAGIC_NUMBER                      |
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *      | Flags |   Reserved    | Vers  |      Header Checksum          |
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *      |             Secondary Loader Location (bytes)                 |
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *      |     Loader Checksum           |     Size of loader (bytes)    |
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *      |          Reserved             |    Architecture Information   |
 *      +-------+-------+-------+-------+-------+-------+-------+-------+
 *
 * Boot block fields should always be read as 64-bit numbers.
 *
 */

struct boot_block {
	uint64_t bb_data[64];	/* data (disklabel, also as below) */
};
#define bb_magic	bb_data[59]	/* magic number */
#define bb_hdrinfo	bb_data[60]	/* header checksum, ver, flags */
#define bb_secstart	bb_data[61]	/* secondary start (bytes) */
#define bb_secsize	bb_data[62]	/* secondary size (bytes) */
#define bb_archinfo	bb_data[63]	/* architecture info */

#define	BOOT_BLOCK_OFFSET	0	/* offset of boot block. */
#define	BOOT_BLOCK_BLOCKSIZE	512	/* block size for sec. size/start,
					 * and for boot block itself
					 */
#define BOOT_BLOCK_SIZE		40	/* 5 64-bit words */

/* 
 * This is the highest block number that we look at when 
 * searching for a valid boot block
 */
#define BOOT_BLOCK_MAXLOC	16

/*
 * Fields within the boot block
 */
#define _U64(x) ((uint64_t) x)
#define BOOT_MAGIC_NUMBER	_U64(0x43465631424f4f54LL)
#define BOOT_HDR_CHECKSUM_MASK	_U64(0x00000000FFFFFFFFLL)
#define BOOT_HDR_VER_MASK       _U64(0x000000FF00000000LL)
#define BOOT_HDR_VER_SHIFT	32
#define BOOT_HDR_FLAGS_MASK	_U64(0xFF00000000000000LL)
#define BOOT_SECSIZE_MASK	_U64(0x00000000FFFFFFFFLL)
#define BOOT_DATA_CHECKSUM_MASK _U64(0xFFFFFFFF00000000LL)
#define BOOT_DATA_CHECKSUM_SHIFT 32
#define BOOT_ARCHINFO_MASK	_U64(0x00000000FFFFFFFFLL)

#define BOOT_HDR_VERSION	1

#define	CHECKSUM_BOOT_DATA(data,len,cksum)				\
	do {								\
		uint32_t *_ptr = (uint32_t *) (data);                 \
		uint32_t _cksum;					\
		unsigned int _i;					\
									\
		_cksum = 0;						\
		for (_i = 0;						\
		    _i < ((len) / sizeof (uint32_t));       		\
		    _i++)						\
			_cksum += _ptr[_i];				\
		*(cksum) =  _cksum;					\
	} while (0)

#endif /* __BOOTBLOCK_H__ */

