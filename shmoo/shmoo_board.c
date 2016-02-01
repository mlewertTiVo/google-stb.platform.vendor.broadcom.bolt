
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

#include <fsbl-common.h>

#ifndef CFG_EMULATION
extern const struct memsys_info __maybe_unused shmoo_data[] 
					__attribute__ ((section(".mcbtable")));
#endif


const struct board_nvm_info nvm_info /* aka saved_board */
					__attribute__ ((section(".board"))) = {
	.magic		= BOARD_NVM_MAGIC,
	.board_idx	= CFG_BOARDDEFAULT-1,
	.romstart	= FSBL_TEXT_ADDR,
	.romoffset	= SHMOO_TEXT_ADDR, /* == &nvm_info */
	.ddr_override	= DDR_OVERRIDE_NO,
	.hardflags	= FSBL_HARDFLAG_DEFAULT,
#ifndef CFG_EMULATION
	.shmoo_data	= (struct memsys_info *)shmoo_data
#endif
};
