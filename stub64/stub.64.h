/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __STUB64_H__
#define __STUB64_H__

#include <stddef.h>
#include <bitops.h>
#include <bchp_common.h>

static inline uint32_t rdb_read(uint64_t reg)
{
	return *(volatile uint32_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET);
}

static inline void rdb_write(uint64_t reg, uint32_t value)
{
	*(volatile uint32_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET) = value;
}

static inline uint64_t rdb_read64(uint64_t reg)
{
	return *(volatile uint64_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET);
}

static inline void rdb_write64(uint64_t reg, uint64_t value)
{
	*(volatile uint64_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET) = value;
}

#endif /* __STUB64_H__ */
