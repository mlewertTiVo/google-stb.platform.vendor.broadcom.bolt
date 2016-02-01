/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "env_subr.h"
#include "error.h"

#include "ssbl-common.h"
#include "board.h"
#include "rts.h"
#include <stdbool.h>


/* support ----------------------------------------------------------------- */

static void setup_rts(unsigned long base, uint32_t *tbl)
{
	uint32_t i, c;

	c = tbl[0];
	tbl++;

	xprintf("%d clients, ", c);

	for (i = 0; i < c; i++)
		BDEV_WR(base + (i<<2), tbl[i]);
}


/* return an index into the rts array from
 its (rtsdefault) ident.
*/
static int rts_idx_from_id(uint32_t id)
{
	int i;
	for (i = 0; i < num_rts_cfgs; i++) {
		if (rts_cfgs[i]->rtsdefault == id)
			return i;
	}
	return -1;
}

/*
 * If we only have one RTS set and its box mode #0 then the way RTS is done
 * is changed from the 'rtsdefault' (per board, scanned for in rts_cfgs[]),
 * or 'hardflags' (saved in FLASH, index into rts_cfgs[]) to just either
 * returning 'rtsdefault' (board default box mode) or 'hardflags' now returning
 * its value as the box mode (NOT being an index into rts_cfgs[] any more.)
 *
 *  This maintains backward compatibility with the current implementation.
 *
 *  NOTE: The box mode cannot be set/changed if *no* RTS (incl. box mode #0)
 * is specified for incorporation into the BOLT build; see 'rtsconfig' script
 * command.
 */
static bool is_box0(void)
{
	return ((1 == num_rts_cfgs) &&
		(0 == rts_cfgs[0]->rtsdefault));
}


/* accessors --------------------------------------------------------------- */

/* get the rts array index out of FLASH
*/
static int rts_saved_array_idx(void)
{
	struct fsbl_info *inf = board_info();
	if (!inf)
		return -1;

	/*
	 * As the RTS index/boxmode range has expanded check we're not getting
	 * an older one from an older FSBL.
	 */
	if (inf->saved_board.hardflags & FSBL_HARDFLAG_RTS_V2)
		return (inf->saved_board.hardflags >> FSBL_HARDFLAG_RTS_SHIFT)
			& FSBL_HARDFLAG_RTS_MASK;

	/* Older box mode (0..15) */
	return (inf->saved_board.hardflags >> FSBL_HARDFLAG_RTS_V1_SHIFT)
		& FSBL_HARDFLAG_RTS_V1_MASK;
}


/* get the CURRENT BOARDs default rts array index
*/
static int rts_board_array_idx(void)
{
	const struct ssbl_board_params *p = board_current_params();

	if (p)
		return rts_idx_from_id(p->rtsdefault);

	return -1;
}


static struct rts *current_rts(void)
{
	int i;

	/*
	 * For box #0 'hardflags' will ony hold the
	 * box mode and _not_ an rts_cfgs[] array index
	 * that may very well be out of bounds.
	 */
	if (is_box0())
		return rts_cfgs[0];

	/*
	 * saved into flash has priority
	 * over the board default
	 */
	i = rts_saved_array_idx();
	if ((i >= 0) && (i < num_rts_cfgs)) /* SWBOLT-586 */
		return rts_cfgs[i];

	i = rts_board_array_idx();
	if ((i >= 0) && (i < num_rts_cfgs)) /* SWBOLT-586 */
		return rts_cfgs[i];

	return NULL;
}


static void print_rts(int i, int saved, int board)
{
	if ((i >= 0) && (i < num_rts_cfgs)) /* SWBOLT-587 */
		xprintf("rts %02d [%s]\t%s\t%s\n",
			rts_cfgs[i]->rtsdefault,
			rts_cfgs[i]->name,
			(i == saved) ? "*used" : "",
			(i == board) ? "*default" : "");
}

/* api --------------------------------------------------------------------- */

/* Returns the RTS_VER string we scraped from the original *.rts file */
char *board_init_current_rts(void)
{
	struct rts *r = current_rts();

	if (is_box0()) {
		static char boxmode[40];

		/* NEXUS can look at /proc/device-tree/bolt/rts
		 * instead of /proc/device-tree/bolt/box so
		 * we fake out the RTS_VER string so it picks
		 * up the right boxmode whatever the case, though
		 * we do lose date & chip version which is currently
		 * not looked at in NEXUS anyway.
		 *                20140402215718_7445_box1
		 */
		sprintf(boxmode, "00000000000000_0000_box%d",
				board_init_current_rts_boxmode());
		return boxmode;
	}

	return (r && r->name) ? r->name : NULL;
}


/* The 'rtsdefault' (box mode) element of struct rts
has to now be passed to Linux. */
int board_init_current_rts_boxmode(void)
{
	struct rts *r = current_rts();
	const struct ssbl_board_params *p;
	int i;

	/*
	 * If box #0 then check if the saved or	board default
	 * 'box mode' will be used, as of course being box #0,
	 * it will be 0 and we don't have other choices.
	 */
	if (is_box0()) {
		/* Check for any saved override first, */
		i = rts_saved_array_idx();
		if ((i >= 0) && (i < FSBL_HARDFLAG_RTS_MASK))
			return i;

		/* ...and if not then just use the board default. */
		p = board_current_params();
		if (p && (p->rtsdefault < FSBL_HARDFLAG_RTS_MASK))
			return p->rtsdefault;
	}

	return (r) ? (int)r->rtsdefault : 0;
}


void board_init_rts_show(int show_only_selected)
{
	int i = 0;
	int saved = rts_saved_array_idx();
	int board = rts_board_array_idx();

	if (show_only_selected) {
		print_rts(saved, saved, -1);
		print_rts(board, -1, board);
	} else {
		for (i = 0; i < num_rts_cfgs; i++)
			print_rts(i, saved, board);
	}
	/* Always print out the box mode now.*/
	xprintf("BOX MODE: %d\n",
		board_init_current_rts_boxmode());
}


void board_init_rts(void)
{
	uint32_t i;
	struct rts *r = current_rts();

	if (!r) {
		xprintf("RTS: not programed!\n");
		return;
	}

	for (i = 0; i < rts_base_size; i++) {
		xprintf("RTS%d: %#08x, ", i, rts_bases[i]);
		/*   The ("rtsbase n MEMC_ARB_[m]...") client
			is not to be programmed. See your family *.cfg
			script file if you think this is in error.
		*/
		if (0 == rts_bases[i]) {
			xprintf("not programed\n");
			continue;
		}
	/*	Don't poke random values if the rts client to
		be programed does not have a corresponding
		'values' entry. This may be by accident or design
		as you may not want to program fewer memc/clients for some
		Linux app cases, but have 'rtsbase' defined for them in
		the script for other cases.
		See your *.rts file(s) if you think this is in error.
	*/
		if (i >= r->number) {
			xprintf(
				"missing client list(s) in rts file id:#%d? (%d >= %d)\n",
				r->rtsdefault, r->number, i);
			continue;
		}
		setup_rts(rts_bases[i], r->values[i]);
		xprintf("ok\n");
	}
}

int board_init_rts_update(int rts_id)
{
	int i;
	uint32_t hardflags, oldhard;
	struct fsbl_info *inf = board_info();

	if (!inf)
		return BOLT_ERR_BADADDR;

	if (rts_id < 0) { /* < 0 means clear & use board defaults */
		i = FSBL_HARDFLAG_RTS_BOARD;
	} else if (!is_box0()) {
		i = rts_idx_from_id(rts_id);
		if (i < 0)
			return BOLT_ERR_CANNOTSET;
	} else /* Allow anything (within limits) for box #0 */
		i = rts_id & FSBL_HARDFLAG_RTS_MASK;

	oldhard = inf->saved_board.hardflags;

	hardflags = oldhard & ~(FSBL_HARDFLAG_RTS_MASK
		<< FSBL_HARDFLAG_RTS_SHIFT);
	hardflags |= (i << FSBL_HARDFLAG_RTS_SHIFT);

	/*
	 * Always set this bit in case FSBL provided an older field
	 * range in 'oldhard' (where it is not set) as this bit then
	 * gets preserved as we update to the expanded V2 range,
	 * but only for SECURE_BOOT=n builds.
	 */
	hardflags |= FSBL_HARDFLAG_RTS_V2;

	inf->saved_board.hardflags = hardflags;

	return (oldhard != hardflags);
}
