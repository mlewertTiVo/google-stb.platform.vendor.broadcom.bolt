/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ADDR_MAPPING_H__
#define __ADDR_MAPPING_H__

#include <stdint.h>

/* mapping between different address spaces, e.g., between child and parent */
struct addr_mapping_entry {
	uint32_t which; /* index or address that can identify source device */
	uint32_t from_mb; /* in MB */
	uint32_t to_mb; /* in MB */
	uint32_t size_mb; /* in MB */
};

/* hard code v7-64 mapping, v7-32 mapping will be provded by family-*.cfg */
static const struct addr_mapping_entry dram_mapping_table_v7_64[] = {
	{
		.which   = 0,
		.from_mb = 0,
		.to_mb   = 1024,
		.size_mb = 3072, /* for something under 4GB boundary */
	},
	{
		.which   = 0,
		.from_mb = 3072,
		.to_mb   = 4096,
		.size_mb = 5120,
	},
	{
		.which   = 1,
		.from_mb = 0,
		.to_mb   = 12288,
		.size_mb = 8192,
	}
};

#endif /* __ADDR_MAPPING_H__ */
