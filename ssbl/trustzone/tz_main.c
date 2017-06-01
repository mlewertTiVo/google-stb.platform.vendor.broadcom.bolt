/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "tz.h"
#include "tz_priv.h"

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>
#include <board.h>
#include <bolt.h>
#include <chipid.h>
#include <error.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <mmap-dram.h>
#include <ssbl-common.h>

static struct tz_info s_tz_info;

struct tz_info *tz_info(void)
{
	/* This could be run-time allocated */
	return &s_tz_info;
}


static int tz_memory_init(void)
{
	struct tz_info *t;
	struct memory_area list;
	queue_t *qe;
	int count;
	int rc;
	struct board_type *b;
	unsigned int i;
	unsigned int total_mb, reserve_mb;
	int64_t retval;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	b = board_thisboard();
	if (b == NULL)
		return BOLT_ERR;

	total_mb = 0;
	for (i = 0; i < b->nddr; i++) {
		struct ddr_info *ddr = board_find_ddr(b, i);

		if (!ddr)
			continue;

		total_mb += ddr->size_mb;
	}

	/* - 32MB if total is over 1GB, 16MB otherwise
	 * - highest available under 4GB boundary
	 * - no memory controller preferrence
	 * - DT not to be created by dtbolt
	 * - DT to be created under /reserved-memory by tz_dt
	 */
	reserve_mb = (total_mb > 1024) ? 32 : 16;
	reserve_mb *= 1024 * 1024;
	retval = bolt_reserve_memory(reserve_mb, _KB(4), 0, "TZOS");
	if (retval < 0) {
		reserve_mb /= 1024 * 1024;
		err_msg("failed to reserve %d MB for TrustZone %lld\n",
			reserve_mb, retval);
	}
	t->mem_addr = (uint64_t) retval;

	count = bolt_reserve_memory_getlist(&list);
	if (count <= 0)
		return BOLT_ERR_NOMEM;

	rc = BOLT_OK;
	qe = q_getfirst((queue_t *)&list);
	for ( ; qe != (queue_t *)&list; qe = qe->q_next) {
		struct memory_area *m = (struct memory_area *)qe;

		if (t->mem_addr != m->offset)
			continue;

		/* match */
		t->mem_size = m->size;
		/* already done t->mem_addr */
		switch (m->options & BOLT_RESERVE_MEMORY_OPTION_MEMC_MASK) {
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_0:
			t->which = 0;
			break;
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_1:
			t->which = 1;
			break;
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_2:
			t->which = 2;
			break;
		default:
			rc = BOLT_ERR_NOMEM;
			goto out;
		}

		break;
	}

out:
	while (!q_isempty((queue_t *)&list)) {
		queue_t *q = q_getfirst((queue_t *)&list);

		q_dequeue(q);
		KFREE(q);
	}

	return rc;
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
	 * Decide memory layout
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

