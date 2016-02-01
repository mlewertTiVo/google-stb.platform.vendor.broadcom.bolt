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

/* For secure boot i.e. one, or a very limited set of boards,
 the board info will be in FSBL.
*/
#ifndef SECURE_BOOT

#include <fsbl-common.h>
#include <board_types.h>

const struct boards_nvm_list nvm_boards
		__attribute__ ((section(".header"))) = {
	.magic = BOARD_LIST_MAGIC,
	.n_boards = ARRAY_SIZE(board_types),
	.board_types = board_types,
};

#endif

