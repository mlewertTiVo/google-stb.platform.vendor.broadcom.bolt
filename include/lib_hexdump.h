/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __LIB_HEXDUMP_H__
#define __LIB_HEXDUMP_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_OFFSET,
	DUMP_PREFIX_ADDRESS,
};

void lib_hexdump(const void *p, size_t len, int wlen, bool ascii, int prefix);

static inline void hexdump(const void *p, size_t len)
{
	lib_hexdump(p, len, 1, false, DUMP_PREFIX_OFFSET);
}

#endif /* __LIB_HEXDUMP_H__ */
