/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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

#ifdef CONFIG_BCM7278A0
#include <bchp_aon_ctrl.h>
#endif

/* Limit to a 32-bit space---BOLT does not (currently) support ARM LPAE. */
uint32_t ddr_get_restricted_size_mb(const struct ddr_info *ddr)
{
	const struct addr_mapping_entry *am_entry = dram_mapping_table;
	uint32_t size_mb = 0;  /* 0 is error value */
	unsigned int i;
	unsigned int num_entries = NUM_DRAM_MAPPING_ENTRIES;

#ifdef CONFIG_BCM7278A0
	if (BDEV_RD(BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT) &
		BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK) {
		num_entries = sizeof(dram_mapping_table_v7_64) /
			sizeof*(dram_mapping_table_v7_64);
		am_entry = dram_mapping_table_v7_64;
	}
#endif

	for (i = 0; i < num_entries; ++i) {
		if (am_entry->to_mb == ddr->base_mb &&
				am_entry->to_mb + am_entry->size_mb <= 4096) {
			size_mb = min(ddr->size_mb, am_entry->size_mb);
			break;
		}
		++am_entry;
	}

	return size_mb;
}
