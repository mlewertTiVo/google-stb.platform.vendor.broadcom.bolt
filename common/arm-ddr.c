/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <addr-mapping.h>
#include <fsbl-common.h>
#include <mmap-dram.h>
#include <stdint.h>

/* Limit to a 32-bit space---BOLT does not (currently) support ARM LPAE. */
uint32_t ddr_get_restricted_size_mb(const struct ddr_info *ddr)
{
	const struct addr_mapping_entry *am_entry = NULL;
	uint32_t size_mb = 0;  /* 0 is error value */
	unsigned int i;

	for (i = 0; i < NUM_DRAM_MAPPING_ENTRIES; ++i) {
		am_entry = &dram_mapping_table[i];
		if (am_entry->to_mb == ddr->base_mb &&
				am_entry->to_mb + am_entry->size_mb <= 4096) {
			size_mb = min(ddr->size_mb, am_entry->size_mb);
			break;
		}
	}

	return size_mb;
}
