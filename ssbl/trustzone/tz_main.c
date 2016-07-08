/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "board.h"
#include "mmap-dram.h"
#include "chipid.h"

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"

#include "tz.h"
#include "tz_priv.h"


static struct tz_info s_tz_info;

struct tz_info *tz_info(void)
{
	/* This could be run-time allocated */
	return &s_tz_info;
}


static int tz_memory_init(void)
{
	struct tz_info *t;
	struct board_type *b;
	uint32_t which;
	uint32_t total_mb;
	uint32_t top_mb;
	int i;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR;

	/* The algorithm is:
	 *    - use the top memory segment (from any MEMC) below 4G;
	 *    - carve out from the top of this memory segment;
	 *    - use 16MB or 32MB, based on total memory:
	 *        . if total memory < 1GB, use 16MB
	 *        . otherwise, use 32MB
	 */
	total_mb = 0;
	top_mb = 0;
	which = 0;
	for (i = 0; i < b->nddr; i++) {
		struct ddr_info *ddr;
		uint32_t dtop_mb;
		uint32_t dsize_mb;
		int j;

		ddr = board_find_ddr(b, i);
		if (!ddr)
			continue;

		if (NUM_DRAM_MAPPING_ENTRIES > 0) {
			/* Mapping table used, find top cpu addr of this DDR */
			dtop_mb = 0;
			dsize_mb = ddr->size_mb;
			for (j = 0; j < (int)NUM_DRAM_MAPPING_ENTRIES; j++) {
				const struct addr_mapping_entry *mapping =
					&dram_mapping_table[j];

				if (mapping->which != ddr->which)
					continue;

				if (mapping->to_mb >= 4096) /* over 4GB */
					break;

				if (dsize_mb > mapping->size_mb) {
					dtop_mb = mapping->to_mb +
						mapping->size_mb;
					dsize_mb -= mapping->size_mb;
				} else {
					dtop_mb = mapping->to_mb + dsize_mb;
					break;
				}
			}
		} else {
			/* No mapping table, use ddr->base_mb as cpu addr */
			dtop_mb = ddr->base_mb + ddr->size_mb;
		}

		total_mb += ddr->size_mb;

		if (top_mb < dtop_mb) {
			top_mb = dtop_mb;
			which = ddr->which;
		}
	}

	t->which = which;
	t->mem_size = (uint64_t)((total_mb > 1024) ? 32 : 16) * 0x100000;
	t->mem_addr = (uint64_t)top_mb * 0x100000 - t->mem_size;

	return 0;
}

static int tz_reg_group_init(void)
{
	struct tz_info *t;
	struct board_type *b;
	struct tz_reg_group *reg_group;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR;

	/* Use default map */
	/* If it compiles for the chip, it should work :-) */
	t->reg_groups = s_tz_reg_groups;

	/* Modify reg groups for MEMC tracelog and sentinel */
	if (t->which == 0)
		return 0;

	reg_group = t->reg_groups;

	for (reg_group = t->reg_groups;
	     reg_group->compatible;
	     reg_group++) {

		if (reg_group->start == BCHP_MEMC_TRACELOG_0_0_REG_START) {
#ifdef BCHP_MEMC_TRACELOG_0_1_REG_START
			if (t->which == 1) {
				reg_group->start =
					BCHP_MEMC_TRACELOG_0_1_REG_START;
				reg_group->end =
					BCHP_MEMC_TRACELOG_0_1_REG_END;
			} else
#endif
#ifdef BCHP_MEMC_TRACELOG_0_2_REG_START
			if (t->which == 2) {
				reg_group->start =
					BCHP_MEMC_TRACELOG_0_2_REG_START;
				reg_group->end =
					BCHP_MEMC_TRACELOG_0_2_REG_END;
			} else
#endif
			warn_msg("TZ tracelog not supported on MEMC%d",
				t->which);
			break;
		}
	}

	for (reg_group = t->reg_groups;
	     reg_group->compatible;
	     reg_group++) {

		if (reg_group->start == BCHP_MEMC_SENTINEL_0_0_REG_START) {
#ifdef BCHP_MEMC_SENTINEL_0_1_REG_START
			if (t->which == 1) {
				reg_group->start =
					BCHP_MEMC_SENTINEL_0_1_REG_START;
				reg_group->end =
					BCHP_MEMC_SENTINEL_0_1_REG_END;
			} else
#endif
#ifdef BCHP_MEMC_SENTINEL_0_2_REG_START
			if (t->which == 2) {
				reg_group->start =
					BCHP_MEMC_SENTINEL_0_2_REG_START;
				reg_group->end =
					BCHP_MEMC_SENTINEL_0_2_REG_END;
			} else
#endif
			warn_msg("TZ sentinel not supported on MEMC%d",
				t->which);
			break;
		}
	}

	return 0;
}

int tz_init(void)
{
	int rc;
	struct tz_info *t;
	struct board_type *b;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR;

	/*
	 * Init board specific config
	 */
	rc = tz_config_init();
	if (rc)
		xprintf("Failed to config TZ on bid 0x%x\n", b->bid);

	/*
	 * Set TZIOC IRQ
	 */
	t->tzioc_irq = 0xf;

	/*
	 * Scan for memory to use
	 */
	rc = tz_memory_init();
	if (rc)
		return rc;

	/*
	 * Decide memory laylout
	 */
	if (t->mem_size == 0x2000000) {
		/* 32MB */
		t->mem_layout = &s_tz_mem_layout_32MB;
	} else if (t->mem_size == 0x1000000) {
		/* 16MB */
		t->mem_layout = &s_tz_mem_layout_16MB;
	} else {
		xprintf("TZ memory size of 0x%x is not supported\n",
			(unsigned int)t->mem_size);

		return BOLT_ERR;
	}

	/*
	 * Decide reg group maps to use
	 */
	rc = tz_reg_group_init();
	if (rc)
		return rc;

	/*
	 * Init NWOS device tree with TZ/board info
	 */
	rc = tz_devtree_init_nwos();
	if (rc)
		return rc;

	/*
	 * Init TZ device tree
	 */
	rc = tz_devtree_init();
	if (rc)
		return rc;

	return 0;
}

/* EOF */

