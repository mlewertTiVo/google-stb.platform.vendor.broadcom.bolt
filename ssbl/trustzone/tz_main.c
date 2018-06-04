/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
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

#if CFG_TRUSTZONE_MON
static int tz_mon_init(void)
{
	struct tz_info *t;
	struct memory_area list;
	queue_t *qe;
	int count;
	int rc;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	count = bolt_reserve_memory_getlist(&list);
	if (count <= 0)
		return BOLT_ERR_NOMEM;

	rc = BOLT_ERR_NOMEM;
	qe = q_getfirst((queue_t *)&list);
	for ( ; qe != (queue_t *)&list; qe = qe->q_next) {
		struct memory_area *m = (struct memory_area *)qe;

		if (m->tag == NULL || strcmp(m->tag, "MON"))
			continue;

		/* match */
		rc = BOLT_OK;
		t->mon_addr = m->offset;
		t->mon_size = m->size;
		break;
	}

	while (!q_isempty((queue_t *)&list)) {
		queue_t *q = q_getfirst((queue_t *)&list);

		q_dequeue(q);
		KFREE(q);
	}

	return rc;
}
#endif

static int tz_memory_init(void)
{
	struct tz_info *t;
	struct memory_area list;
	int count;
	bool found;
	int64_t retval;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

#if CFG_MON64
	/* Reserve memory for mon64 to TZ mailbox */
	retval = bolt_reserve_memory(_KB(4), _KB(4),
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW, "TZMBOX");
	if (retval < 0) {
		err_msg("failed to reserve 4 KB for TZMBOX %lld\n", retval);
		return BOLT_ERR_NOMEM;
	}
#endif

	/* Reserve 64MB of memory for TrustZone by default */
	retval = bolt_reserve_memory(64 * 1024 * 1024, _KB(4),
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW, "TZOS");
	if (retval < 0) {
		err_msg("failed to reserve 64 MB for TZOS %lld\n", retval);
		return BOLT_ERR_NOMEM;
	}

	t->mem_addr = (uint64_t)retval;

	count = bolt_reserve_memory_getlist(&list);
	if (count <= 0)
		return BOLT_ERR_NOMEM;

	found = false;
	while (!q_isempty((queue_t *)&list)) {
		queue_t *q = q_getfirst((queue_t *)&list);
		struct memory_area *m = (struct memory_area *)q;

		if (found || t->mem_addr != m->offset) {
			q_dequeue(q);
			KFREE(q);
			continue;
		}

		found = true;
		/* Already set mem_addr */
		t->mem_size = m->size;

		switch (m->options & BOLT_RESERVE_MEMORY_OPTION_MEMC_MASK) {
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_0:
		default:
			t->which = 0;
			break;
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_1:
			t->which = 1;
			break;
		case BOLT_RESERVE_MEMORY_OPTION_MEMC_2:
			t->which = 2;
			break;
		}
	}

	return BOLT_OK;
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

#if CFG_TRUSTZONE_MON
	/*
	 * Scan for monitor memory
	 */
	rc = tz_mon_init();
	if (rc)
		return rc;
#endif

	/*
	 * Reserve memory
	 */
	rc = tz_memory_init();
	if (rc)
		return rc;

	/*
	 * Decide memory layout
	 */
	t->mem_layout = &s_tz_mem_layout_64MB;

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
