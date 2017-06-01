/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* For secure boot i.e. one, or a very limited set of boards,
 the board info will be in FSBL.
*/
#ifndef SECURE_BOOT

#include <fsbl-common.h>
#include <board_types.h>
#ifdef DVFS_SUPPORT
#include <pmap.h>
#endif

const struct boards_nvm_list nvm_boards
		__attribute__ ((section(".header"))) = {
	.magic = BOARD_LIST_MAGIC,
	.n_boards = ARRAY_SIZE(board_types) |
		(FSBLINFO_CURRENT_VERSION << FSBLINFO_VERSION_SHIFT),
	.board_types = board_types,
#if defined(DVFS_SUPPORT)
	.n_pmaps = ARRAY_SIZE(pmapTable),
#else
	.n_pmaps = 0,
#endif
};

#endif

