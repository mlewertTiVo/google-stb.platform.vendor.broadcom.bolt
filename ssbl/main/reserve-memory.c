/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/

#include <addr-mapping.h>
#include <bchp_aon_ctrl.h>
#include <board.h>
#include <bolt.h>
#include <error.h>
#include <initdata.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <mmap-dram.h>
#include <ssbl-common.h>

#include <stdint.h>

/* define DEBUG_RESERVE_MEMORY for debug messages */
#undef DEBUG_RESERVE_MEMORY
#if CFG_CMD_LEVEL >= 5 /* or, "cmdlevel 5" activates debug messages */
#define DEBUG_RESERVE_MEMORY
#endif

#ifdef DEBUG_RESERVE_MEMORY
#define DEBUGMSG(x) printf x
#else
#define DEBUGMSG(x) do {} while (0)
#endif

/* four possible scenarios that a requested reservation can be subset
 *
 *    base             top
 * (1)  +---------------+ : requested
 *      +---------------+ : superset
 *
 * (2)  +-----------+     : requested
 *      +---------------+ : superset
 *                  | b |   extra at top
 *
 * (3)      +-----------+ : requested
 *      +---------------+ : superset
 *      | a |               extra at bottom
 *
 * (4)      +-------+     : requested
 *      +---------------+ : superset
 *      | a |       | b |   extra at both bottom and top
 */
#define SUBSET_NOT 0
#define SUBSET_IDENTICAL 1
#define SUBSET_EXTRA_TOP 2
#define SUBSET_EXTRA_BOTTOM 3
#define SUBSET_EXTRA_BOTH 4

static const uint64_t FOUR_GB = 4ULL * 1024 * 1024 * 1024;

struct board_memory_chunk {
	unsigned int memc;
	unsigned int offset_mb;
	unsigned int size_mb; /* 0 (zero) means invalid */
};

static void debug_print_board_memory(
	unsigned int num_entries, struct board_memory_chunk *bm);
static void debug_print_memory_areas(
	struct memory_area *table_ma, const char *name, bool areas_only);

static int build_board_memory_map(void);
static int build_available_memory_map(struct memory_area *amm,
	unsigned int max_entries_bmm, struct board_memory_chunk *bmm);
static unsigned int combine_chunks(unsigned int offset_mb,
	unsigned int num_entries, struct board_memory_chunk *bmm);
static struct memory_area *create_memory_area_node(unsigned int memc,
	uint64_t offset, unsigned int size, const char *tag);
static bool is_memc_eligible(unsigned int memc, unsigned int memc_selection);
static bool is_power_of_two(uint64_t n);
static void print_memory_areas(struct memory_area *table_ma, const char *name,
	bool areas_only);
static int try_fulfill_request(const uint64_t current_offset,
	const uint64_t current_top,
	const uint64_t requested_offset,
	const uint64_t requested_top);
static uint32_t update_memc_option(uint32_t options, unsigned int memc);

static struct memory_area table_am; /* map for available memory areas */
static struct memory_area table_rm; /* map for reserved memory areas */
static struct board_memory_chunk *table_bm; /* board configured memory map */
static unsigned int num_entries_table_bm;

/* bolt_reserve_memory -- reserves memory area
 *
 * Parameters:
 *  amount    [in] desired amount of memory to reserve in bytes
 *  alignment [in] alignment requirement or absolute offset in bytes
 *  options   [in] preferred MEMC, okay to be over 4GB boundary, etc
 *  tag       [in] optional string to be associated with reservation
 *
 * Returns:
 *  offset of a reserved memory area on success,
 *  otherwise, BOLT error code (negative value)
 */
int64_t bolt_reserve_memory(unsigned int amount, uint64_t alignment,
	uint32_t options, const char *tag)
{
	const bool okay_to_exceed_4gb_boundary =
		options & BOLT_RESERVE_MEMORY_OPTION_OVER4GB;
	const uint32_t memc_selection =
		options & BOLT_RESERVE_MEMORY_OPTION_MEMC_MASK;
	const bool is_alignment_absolute_offset =
		options & BOLT_RESERVE_MEMORY_OPTION_ABS;
	const bool is_dryrun = options & BOLT_RESERVE_MEMORY_OPTION_DRYRUN;
	int retval;
	queue_t *qe;

	if (amount == 0)
		return BOLT_ERR_INV_PARAM;

	if (!is_alignment_absolute_offset) {
		/* unless absolute offset, alignment has to be power of 2 */
		if (!is_power_of_two(alignment))
			return BOLT_ERR_INV_PARAM;
	}

	/* TODO: granularity on amount and offset/alignment
	 *       should it be forced in this API, or let the caller take care?
	 */

	/* construct a memory map that is specific to board first,
	 * then create one more for available memory, excluding
	 * areas that should not be reserved
	 */
	if (table_bm == NULL) {
		retval = build_board_memory_map();
		if (retval < 0)
			return retval;
		/* table_bm has been constructed and,
		 * retval is the max #entries in table_bm
		 */

		num_entries_table_bm = 0;
		while (num_entries_table_bm < (unsigned int) retval) {
			if (table_bm[num_entries_table_bm].size_mb == 0)
				break;
			++num_entries_table_bm;
		}
		debug_print_board_memory(num_entries_table_bm, table_bm);

		q_init((queue_t *)&table_am);
		q_init((queue_t *)&table_rm);

		retval = build_available_memory_map(&table_am,
			num_entries_table_bm, table_bm);
		if (retval != BOLT_OK) {
			KFREE(table_bm);
			table_bm = NULL;
			err_msg("failed to create available memory map\n");
			return retval;
		}
		debug_print_memory_areas(&table_am, "avalable memory areas",
			true);
	}
	/* table_am now contains combined (contiguous if possible) memory
	 * chunks sorted in the ascending order of 'offset'.
	 */

	/* scan thourgh the available memory areas from top to bottom
	 * to find a slot for the requested reservation.
	 */
	qe = q_getlast((queue_t *)&table_am);
	for ( ; qe != (queue_t *)&table_am; qe = qe->q_prev) {
		struct memory_area *m = (struct memory_area *) qe;
		struct memory_area *b;
		const unsigned int memc = m->memc;
		const uint64_t offset = m->offset;
		const uint64_t top = m->offset + m->size;
		uint64_t req_offset;
		uint64_t req_top;
		int case_number;

		if (!is_memc_eligible(memc, memc_selection))
			continue;

		if (is_alignment_absolute_offset) {
			req_offset = alignment; /* alignment is offset */
		} else {
			req_offset = offset + m->size;
			req_offset -= amount;
			req_offset &= ~(alignment - 1);
		}
		req_top = req_offset + amount;

		DEBUGMSG(("considering %d %010llx %08x for %010llx %08x\n",
			m->memc, m->offset, m->size, req_offset, amount));

		if (!okay_to_exceed_4gb_boundary) {
			if (offset >= FOUR_GB)
				continue; /* impossible to go under 4GB */

			if (req_top > FOUR_GB) {
				uint64_t diff;

				/* needs to shift */
				if (is_alignment_absolute_offset)
					return BOLT_ERR_NOMEM;

				/*            4GB
				 *  +----------+--------+ : m
				 *                  |-r-| : requested
				 *         |-r-|--diff--| : requested, shifted
				 */
				diff = req_top - FOUR_GB;
				req_offset -= diff;
				req_top -= diff;
				if (req_offset < offset)
					continue; /* went over due to shift */

				DEBUGMSG(("trying to reserve memory in the "
					"middle of an available memory area "
					"due to 4GB boundary limit\n"));
			}
		}

		case_number = try_fulfill_request(offset, top,
			req_offset, req_top);
		if (is_dryrun) {
			/* just for checking, no actual reservation */
			if (case_number != SUBSET_NOT)
				return req_offset;
		}

		switch (case_number) {
		case SUBSET_IDENTICAL:
			/* (1)  +---------------+ : requested
			 *      +---------------+ : m
			 */
			/* de-queue from table_am */
			q_dequeue((queue_t *)m);
			/* then, put (enqueue) in table_rm */
			m->options = update_memc_option(options, memc);
			if (tag != NULL)
				m->tag = strdup(tag);
			q_enqueue((queue_t *)&table_rm, (queue_t *)m);
			break;
		case SUBSET_EXTRA_TOP:
			/* (2)  +-----------+     : requested
			 *      +---------------+ : m
			 *                  | b |
			 */
			/* shift/reduce current offset/size */
			m->offset = req_top;
			m->size = top - req_top;
			DEBUGMSG(("case 2: reduced to %010llx %08x\n",
				m->offset, m->size));
			/* and, add the reservation to table_rm */
			m = create_memory_area_node(memc, offset, amount, tag);
			if (m == NULL)
				return BOLT_ERR_NOMEM;
			m->options = update_memc_option(options, memc);
			q_enqueue((queue_t *)&table_rm, (queue_t *)m);
			DEBUGMSG(("case 2: reserved %010llx %08x\n",
				offset, amount));
			break;
		case SUBSET_EXTRA_BOTTOM:
			/* (3)      +-----------+ : requested
			 *      +---------------+ : m
			 *      | a |
			 */
			/* reduce the current size */
			m->size = req_offset - offset;
			DEBUGMSG(("case 3: reduced to %010llx %08x\n",
				m->offset, m->size));
			/* and, add the reservation to table_rm */
			m = create_memory_area_node(memc, req_offset,
				amount, tag);
			m->options = update_memc_option(options, memc);
			q_enqueue((queue_t *)&table_rm, (queue_t *)m);
			DEBUGMSG(("case 3: reserved %010llx %08x\n",
				req_offset, amount));
			break;
		case SUBSET_EXTRA_BOTH:
			/* (4)      +-------+     : requested
			 *      +---------------+ : m
			 *      | a |       | b |
			 */
			/* 'a' to current 'm' */
			m->size = req_offset - offset;
			DEBUGMSG(("case 4: split to %010llx %08x",
				m->offset, m->size));
			/* new one for 'b' */
			b = create_memory_area_node(memc, req_top,
				top - req_top, NULL);
			if (b == NULL) {
				m->size = top - offset;
				return BOLT_ERR_NOMEM;
			}
			q_enqnext((queue_t *)m, (queue_t *)b);
			DEBUGMSG((" and %010llx %08x\n",
				req_top, (unsigned int)(top - req_top)));
			/* and, add the reservation to table_rm */
			m = create_memory_area_node(memc, req_offset,
				amount, tag);
			if (m == NULL)
				return BOLT_ERR_NOMEM;
			m->options = update_memc_option(options, memc);
			q_enqueue((queue_t *)&table_rm, (queue_t *)m);
			DEBUGMSG(("case 4: reserved %010llx %08x\n",
				req_offset, amount));
			break;
		default:
			/* do nothing, no match */
			break;
		}

		if (case_number > 0) /* was there a match? */ {
			DEBUGMSG(("match case %d allowed memory reservation\n",
				case_number));
			return req_offset;
		}
	}

	return BOLT_ERR_NOMEM;
}

/* bolt_reserve_memory_getlist -- returns the list of reserved memory areas
 *
 * Parameter:
 *  list [in] pointer to where reserved memory areas are to be stored
 *
 * Returns:
 *  on success, number of reserved memory areas (0, zero is possible)
 *  otherwise, BOLT error code
 */
int bolt_reserve_memory_getlist(struct memory_area *list)
{
	int count;
	queue_t *qe;

	if (list == NULL)
		return BOLT_ERR_INV_PARAM;

	q_init((queue_t *)list); /* caller might not have done it */

	qe = q_getfirst((queue_t *)&table_rm);
	if (qe == NULL)
		return 0; /* nothing is in */

	count = 0;
	for ( ; qe != (queue_t *)&table_rm; qe = qe->q_next) {
		struct memory_area *m = (struct memory_area *) qe;
		struct memory_area *r;

		r = KMALLOC(sizeof(*r), 0);
		if (r == NULL) {
			err_msg("failed to allocate a memory_area node");
			goto error_exit;
		}

		memcpy(r, m, sizeof(*r));
		/* NOTE: tag is *NOT* string duplicated */
		q_enqueue((queue_t *)list, (queue_t *)r);
		++count;
	}

	return count;

error_exit:
	while (!q_isempty((queue_t *)list)) {
		queue_t *q = q_getlast((queue_t *)list);

		q_dequeue(q);
		KFREE(q);
	}

	return 0;
}

/* bolt_reserve_memory_getstatus -- prints available and reserved memory areas
 */
void bolt_reserve_memory_getstatus(void)
{
	print_memory_areas(&table_rm, "Reserved memory areas", false);
	print_memory_areas(&table_am, "Available memory areas", true);
}

static int build_board_memory_map(void)
{
	const struct addr_mapping_entry *am_entry = dram_mapping_table;
	unsigned int num_entries = NUM_DRAM_MAPPING_ENTRIES;
	unsigned int i;
	struct board_type *b;
	struct board_memory_chunk *bmm;
	unsigned int memc;

#ifdef BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK
	if (is_mmap_v7_64()) {
		num_entries = sizeof(dram_mapping_table_v7_64) /
			sizeof(*dram_mapping_table_v7_64);
		am_entry = dram_mapping_table_v7_64;
	}
#endif

	if (table_bm != NULL)
		return num_entries; /* has already built */

	DEBUGMSG(("building board configured memory map from %d entries\n",
		num_entries));

	b = board_thisboard();
	if (b == NULL)
		return BOLT_ERR_DEVNOTFOUND;

	bmm = (struct board_memory_chunk *)
		KMALLOC(num_entries * sizeof(*bmm), 0);
	if (bmm == NULL) {
		err_msg("failed to allocate %d bytes (board memory table)\n",
			num_entries * sizeof(*bmm));
		return BOLT_ERR_NOMEM;
	}
	memset(bmm, 0, num_entries * sizeof(*bmm));

	/* construct table of available memory based on board configuration */
	for (memc = 0; memc < MAX_DDR; ++memc) {
		struct ddr_info *ddrcfg;
		unsigned int size_left_mb;

		ddrcfg = board_find_ddr(b, memc);
		if (ddrcfg == NULL || ddrcfg->size_mb == 0)
			continue;
		size_left_mb = ddrcfg->size_mb;

		for (i = 0; i < num_entries; ++i) {
			unsigned int index;

			if (memc != am_entry[i].which)
				continue;

			index = combine_chunks(am_entry[i].to_mb,
				num_entries, bmm);
			if (index >= num_entries) {
				err_msg("failed to combine memory chunks\n");
				KFREE(bmm);
				return BOLT_ERR_NOMEM;
			}

			bmm[index].memc = memc;
			/* .offset_mb was filled in by combint_chunks() */
			bmm[index].size_mb =
				min(size_left_mb, am_entry[i].size_mb);
			size_left_mb -= bmm[index].size_mb;
			if (size_left_mb == 0)
				break;
		}
	}

	table_bm = bmm; /* now, no need to construct again */
	return num_entries;
}

static int build_available_memory_map(struct memory_area *amm,
	unsigned int max_entries_bmm, struct board_memory_chunk *bmm)
{
	unsigned int i;
	int retval;

	for (i = 0; i < max_entries_bmm; ++i) {
		struct memory_area *m;
		uint64_t offset;
		unsigned int size;

		if (bmm[i].size_mb == 0)
			break;

		offset = 1024ULL * 1024 * bmm[i].offset_mb; /* ULL for 64-bit */
		size = 1024 * 1024 * bmm[i].size_mb;

		m = create_memory_area_node(bmm[i].memc, offset, size, NULL);
		if (m == NULL) {
			retval = BOLT_ERR_NOMEM;
			goto error_exit;
		}

		q_enqueue((queue_t *)amm, (queue_t *)m);
	}

	return BOLT_OK;

error_exit:
	while (!q_isempty((queue_t *)amm)) {
		queue_t *q = q_getlast((queue_t *)amm);

		q_dequeue(q);
		KFREE(q);
	}

	return retval;
}

/* combine_chunks -- adds a memory chunk to the sorted memory chunk table
 *
 *  This file scope function should be called only within the context of
 *  processing the 'mmap' (or 'mmap64') BOLT configuration commands combined
 *  with a board specific memory configuration (via 'ddr' command).
 *
 *  Memory areas (chunks) described by 'mmap' do not and must not overlap.
 *
 *  The memory chunks should be kept in the ascending order of 'offset_mb'.
 */
static unsigned int combine_chunks(unsigned int offset_mb,
	unsigned int num_entries, struct board_memory_chunk *bmm)
{
	unsigned int i;

	/* ASSUMPTION: no overlap, should be okay due to mmap cfg cmds */
	for (i = 0; i < num_entries; ++i) {
		if (bmm[i].size_mb == 0) {
			/* bmm is empty, or the offset of the new chunk is
			 * higher than any of existing ones
			 */
			bmm[i].offset_mb = offset_mb;
			return i;
		}

		if (bmm[i].offset_mb > offset_mb) {
			/* insert the new chunk, and push existing ones */
			unsigned int top;
			void *dest;

			for (top = i + 1; top < num_entries; ++top) {
				if (bmm[top].size_mb == 0)
					break;
			}
			if (top >= num_entries) {
				/* It should not happen because:
				 * - 'num_entries' is number of 'mmap' cfg cmd
				 * - the new chunk is one of 'mmap' cfg cmd
				 * but for safety...
				 */
				 return top;
			}

			dest = memmove(&bmm[i+1], &bmm[i],
				(top - i) * sizeof(*bmm));
			if (dest == NULL) {
				err_msg("failed memmove() %d bytes to %p\n",
					(top - i) * sizeof(*bmm), &bmm[i+1]);
				return num_entries;
			}

			bmm[i].offset_mb = offset_mb;
			return i;
		}
	}

	/* error condition */
	return i;
}

static struct memory_area *create_memory_area_node(unsigned int memc,
	uint64_t offset, unsigned int size, const char *tag)
{
	struct memory_area *p;

	p = KMALLOC(sizeof(*p), 0);
	if (p == NULL) {
		err_msg("failed to allocate a memory_area node");
		if (tag == NULL)
			err_msg("\n");
		else
			err_msg(" for \"%s\"\n", tag);

		return NULL;
	}

	memset(p, 0, sizeof(*p));
	p->memc = memc;
	p->offset = offset;
	p->size = size;
	if (tag != NULL)
		p->tag = strdup(tag);

	return p;
}

static bool is_memc_eligible(unsigned int memc, unsigned int memc_selection)
{
	if (memc_selection == 0)
		return true;

	if ((1 << memc) & memc_selection)
		return true;

	return false;
}

static bool is_power_of_two(uint64_t n)
{
	if (n == 0)
		return true;
	if (0 == (n & (n - 1)))
		return true;

	return false;
}

/* try_fulfill_request -- tries within given/current/available area
 *
 * four possible scenarios that the requested reservation can be
 * fulfilled within the current memory area
 *
 *   offset            top
 * (1)  +---------------+ : requested - identical
 *      +---------------+ : current
 *
 * (2)  +-----------+     : requested
 *      +---------------+ : current
 *                  | b |   extra at top
 *
 * (3)      +-----------+ : requested
 *      +---------------+ : current
 *      | a |               extra at bottom
 *
 * (4)      +-------+     : requested
 *      +---------------+ : current
 *      | a |       | b |   extra at both bottom and top
 *
 * Parameters:
 *  current_offset   [in] bottom offset of an available memory area
 *  current_top      [in] top offset of the available memory area
 *  requested_offset [in] bottom offset of the requested memory area
 *  requested_top    [in] top offset of the request memory area
 *
 * Returns:
 *  SUBSET_IDENTICAL if the case (1)
 *  SUBSET_EXTRA_TOP if the case (2)
 *  SUBSET_EXTRA_BOTTOM if the case (3)
 *  SUBSET_EXTRA_BOTH if the case (4)
 *  SUBSET_NOT otherwise
 */
static int try_fulfill_request(const uint64_t current_offset,
	const uint64_t current_top,
	const uint64_t requested_offset,
	const uint64_t requested_top)
{
	if (current_offset == requested_offset) {
		if (current_top == requested_top)
			return SUBSET_IDENTICAL;
		if (current_top > requested_top)
			return SUBSET_EXTRA_TOP;
	}

	if (current_offset < requested_offset) {
		if (current_top == requested_top)
			return SUBSET_EXTRA_BOTTOM;
		if (current_top > requested_top)
			return SUBSET_EXTRA_BOTH;
	}

	return SUBSET_NOT; /* no match */
}

static uint32_t update_memc_option(uint32_t options, unsigned int memc)
{
	options &= ~BOLT_RESERVE_MEMORY_OPTION_MEMC_MASK;
	options |= 1 << memc;

	return options;
}

static void print_memory_areas(struct memory_area *table_ma, const char *name,
	bool areas_only)
{
	queue_t *qe;

	if (table_ma == NULL)
		return;

	if (name != NULL)
		printf("%s in bytes:\n", name);

	qe = q_getfirst((queue_t *) table_ma);
	if (qe == NULL)
		return; /* nothing is in */

	for ( ; qe != (queue_t *) table_ma; qe = qe->q_next) {
		struct memory_area *m = (struct memory_area *) qe;

		printf("  %d %010llx %08x", m->memc, m->offset, m->size);
		if (!areas_only) {
			/* options and tag if exists */
			printf(" %08x", m->options);
			if (m->tag)
				printf(" \"%s\"", m->tag);
		}
		printf("\n");
	}
}

static void debug_print_board_memory(
	unsigned int num_entries, struct board_memory_chunk *bm)
{
#ifdef DEBUG_RESERVE_MEMORY
	unsigned int i;

	if (num_entries == 0 || bm == NULL)
		return;

	printf("Board Memory in mb:\n");
	for (i = 0; i < num_entries; ++i) {
		printf("  [%u] %u %u %u\n", i,
			bm->memc, bm->offset_mb, bm->size_mb);
		++bm;
	}
#endif
}

static void debug_print_memory_areas(
	struct memory_area *table_ma, const char *name, bool areas_only)
{
#ifdef DEBUG_RESERVE_MEMORY
	print_memory_areas(table_ma, name, areas_only);
#endif
}
