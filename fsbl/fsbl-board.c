/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>
#include <lib_types.h>

#include "fsbl.h"
#include "chipid.h"

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>

#ifdef SECURE_BOOT
/* This must only be included in ONLY ONCE in FSBL. Do not
include it in SSBL.
*/
#include <board_types.h>
#endif


static struct board_type tmp_board;
static char __maybe_unused tmp_ddr_tag[MAX_DDR][DDR_TAGLEN];


/* Accessor */
struct board_type *get_tmp_board(void)
{
	return &tmp_board;
}


static void print_select(void)
{
	puts("");
	__puts("Select board type: ");
}


static void using_board(struct fsbl_info *info)
{
#ifndef SECURE_BOOT
	int i;
	char *dst, *src;
#endif
	puts("");
	if (info->n_boards == 1) {
			puts("single board");
	} else {
		__puts("using board #");
		putchar(board_idx_to_char(info->board_idx));
		puts("");
	}

	/* Cache current board so it survives shmoo as
	the rest of the boards will get overwritten
	as we load other things (i.e. MEMSYS, SHMOO.) */
	memcpy(&tmp_board, &(info->board_types[info->board_idx]),
		sizeof(struct board_type));

#ifndef SECURE_BOOT
	/* If we have pointers to tag strings in the BOARDS section
	of BOLT then we have to copy them out for our board before use.*/
	for (i = 0; i < tmp_board.nddr; i++) {
		if (tmp_board.ddr[i].tag) {
			/* Our temp stash of tags that goes
			away when FSBL ends. */
			dst = &(tmp_ddr_tag[i][0]);

			/* The tag pointer is an offset (from zero) into the
			BOARDS section. SRAM address info->board_types is offset
			by sizeof(struct boards_nvm_list) from where it was
			loaded so we subtract nvm list to get our original SRAM
			load address + tag offset = &string.*/
			src = (char *)((physaddr_t)info->board_types +
				(physaddr_t)tmp_board.ddr[i].tag -
				sizeof(struct boards_nvm_list));

			/* Don't care about DDR_TAGLEN > strlen(src) */
			memcpy(dst, src, DDR_TAGLEN);
			tmp_board.ddr[i].tag = dst;
#if (CFG_CMD_LEVEL >= 5)
			__puts("TAG: ");
			__puts(dst);
			puts("");
#endif
		}
	}
#endif /* !SECURE_BOOT */
}


struct board_nvm_info *nvm_load(void)
{
	struct board_nvm_info *s;

#ifdef SECURE_BOOT
	/* Note that we must now build for ONLY ONE BOARD, due to limited
	space for MCBs in the MEMSYS section and CFG_BOARDDEFAULT being
	hard wired, though customers can work around that by custom setting
	what board_idx is at runtime via a gpio pin strap or OTP bit etc.
	*/

	/* Fill the entire struct to prevent a silent memcpy on
	some toolchains */
	static const struct board_nvm_info nvm_info = {
		.magic		= BOARD_NVM_MAGIC,
		.board_idx	= CFG_BOARDDEFAULT-1,
		.romstart	= FSBL_TEXT_ADDR,
		.romoffset	= SHMOO_TEXT_ADDR, /* == &nvm_info */
		.ddr_override	= DDR_OVERRIDE_NO,
		.hardflags	= FSBL_HARDFLAG_DEFAULT,
		.shmoo_data	= (struct memsys_info *)NULL,
	};
	s = (struct board_nvm_info *)&nvm_info;
#else
	uint32_t *dst = (uint32_t *)SHMOO_SRAM_ADDR;

	if (load_from_flash(dst, SHMOO_TEXT_OFFS,
			sizeof(struct board_nvm_info)) < 0)
		die("nvm load");

	s = (struct board_nvm_info *)dst;
#endif
	if (s->magic != BOARD_NVM_MAGIC)
		die("nvm magic");

	return s;
}


/* Load a list o' boards */
void load_boards(struct fsbl_info *info, uint32_t dst)
{
#ifdef SECURE_BOOT
	/* For secure boot the board info is in FSBL instead of
	the loadable BOARDS section of BOLT. */
	info->n_boards = ARRAY_SIZE(board_types);
	info->board_types = (struct board_type *)board_types;
#else
	const struct boards_nvm_list *b = (const struct boards_nvm_list *)dst;

	if (load_from_flash((void *)dst, BOARDS_TEXT_OFFS, BOARDS_SIZE))
		die("brd list load");

	if (b->magic != BOARD_LIST_MAGIC)
		die("brd list magic");

	info->n_boards = b->n_boards;
	info->board_types = (struct board_type *)
			((physaddr_t)(dst) + (physaddr_t)b->board_types);
#endif
}


static void indicate_if_match(struct fsbl_info *info, int idx, uint32_t prid)
{
#if CFG_BOARD_ID
	/* Identify by BID & PRID if this is a recognised
	*  configuration for this board.
	*/
	if ((info->bid &&
		(info->board_types[idx].bid == info->bid) &&
		(info->board_types[idx].prid == prid)))
			info->runflags |= FSBL_RUNFLAG_BID_MATCH;
#endif
}

int board_select(struct fsbl_info *info, uint32_t dst)
{
	int do_shmoo_menu = 0, force_menu = 0;
	unsigned int i = 0;
#if CFG_BOARD_ID
	int idx = -1;
#endif
	/* Ignore the minor chip revision for the purpose of board detection */
	uint32_t prid =
		BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID) & ~CHIPID_MINOR_REV_MASK;
#if CFG_BOARD_ID
	info->bid = get_ext_board_id();
#endif
	info->board_idx = -1;

	/* Don't present boot menu for secure boot */
#ifndef SECURE_BOOT
	if (getc() == '1')
		force_menu = 1;
#endif
	load_boards(info, dst);

	if (info->n_boards == 1) {
		info->board_idx = 0;
		info->saved_board.product_id = prid;
		using_board(info);
		if (!force_menu)
			return 0;

	}

	if (info->saved_board.board_idx < info->n_boards) {
		info->board_idx = info->saved_board.board_idx;
		info->saved_board.product_id = prid;
		using_board(info);
		if (!force_menu)
			return 0;
	}

	puts("");
	puts("======");
	puts("BOARDS");
	puts("======");
	puts("");

	for (i = 0; i < info->n_boards; i++) {
		const struct board_type *b = &(info->board_types[i]);

		putchar(board_idx_to_char(i));
		__puts(") ");
		__puts(b->name);
#if CFG_BOARD_ID
		__puts("\t\t");
		writehex(b->prid);
		putchar(':');
		writehex((uint32_t)b->bid);
		/* nb: an id of 0.0 is never a valid value
		*/
		if (info->bid && (b->bid == info->bid) && (b->prid == prid)) {
			__puts(" <-");
			idx = i;
		}
#endif
		puts("");
	}
	puts("");

#if CFG_BOARD_ID
	if ((!force_menu) && (idx >= 0)) {
		indicate_if_match(info, idx, prid);
		info->board_idx = idx;
		info->saved_board.product_id = prid;
		using_board(info);
		return 0;
	}
#endif

	puts("RESET + 1 = this menu");
	puts("RESET + 8 = bypass AVS");
	puts("2)  shmoo menu after board menu");
	puts("");

	print_select();

	while (1) {
		i = getchar();

		if (i == '2') {
			do_shmoo_menu = 1;
			puts("[shmoo menu]");
			print_select();
			continue;
		}
		if (!is_board_char(i))
			continue;

		info->board_idx = board_char_to_idx(i);
		if (info->board_idx >= info->n_boards)
			continue;

		indicate_if_match(info, info->board_idx, prid);
		info->saved_board.product_id = prid;
		using_board(info);
		return do_shmoo_menu;
	}
}


void board_try_patch_ddr(struct board_nvm_info *nvm)
{
#ifndef SECURE_BOOT
	unsigned int i;
	struct ddr_info *ddr;
	struct board_type *b = &tmp_board;

	if (DDR_OVERRIDE_NO == nvm->ddr_override)
		return;

	ddr = shmoo_ddr_by_index(nvm->ddr_override);
	if (!ddr)
		return;

	for (i = 0; i < b->nddr; i++) {
		b->ddr[i].ddr_clock = ddr->ddr_clock;
		b->ddr[i].ddr_size  = ddr->ddr_size;
		b->ddr[i].ddr_width = ddr->ddr_width;
		b->ddr[i].phy_width = ddr->phy_width;
	}
	__puts("DDR patch #");
	putchar('a' + nvm->ddr_override);
	puts("");
#endif
}

