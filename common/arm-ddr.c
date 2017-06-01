/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <addr-mapping.h>
#include <bchp_aon_ctrl.h>
#include <fsbl-common.h>
#include <mmap-dram.h>
#include <stdint.h>

/* Limit to a 32-bit space---BOLT does not (currently) support ARM LPAE. */
uint32_t ddr_get_restricted_size_mb(const struct ddr_info *ddr)
{
	const struct addr_mapping_entry *am_entry = dram_mapping_table;
	uint32_t size_mb = 0;  /* 0 is error value */
	unsigned int i;
	unsigned int num_entries = NUM_DRAM_MAPPING_ENTRIES;

	if (ddr == NULL || ddr->base_mb >= 4096)
		return 0;

#ifdef BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK
	if (is_mmap_v7_64()) {
		num_entries = sizeof(dram_mapping_table_v7_64) /
			sizeof*(dram_mapping_table_v7_64);
		am_entry = dram_mapping_table_v7_64;
	}
#endif

	for (i = 0; i < num_entries; ++i) {
		if (am_entry->to_mb == ddr->base_mb) {
			size_mb = min(ddr->size_mb, am_entry->size_mb);
			if (am_entry->to_mb + size_mb > 4096)
				size_mb = 4096 - am_entry->to_mb;
			break;
		}
		++am_entry;
	}

	return size_mb;
}

#ifdef BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK
bool is_mmap_v7_64(void)
{
	if (BDEV_RD(BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT) &
		BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK)
		return true;

	return false;
}
#endif
