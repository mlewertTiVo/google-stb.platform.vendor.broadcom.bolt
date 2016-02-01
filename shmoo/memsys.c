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
#ifndef CFG_NOSHMOO
 #error !CFG_NOSHMOO - why are you building with this then?
#endif

#include <lib_types.h>
#include <fsbl-common.h>

/* not in include path
*/
#include <../../fsbl/fsbl.h>


/* In shmoo/memsys-$(FAMILY).c
  - for (full) emulation it can be a dummy function.
  - for bringup it can be hard wired.
  - no shmooing or memsysinit for FSBL only emulation.
*/
extern void memsys_init_all(int x);


void memsys_load(void)
{
	puts("NOMEMSYS");
}

void set_loud_shmoo(void)
{
}


/* replacements for real shmoo functions. Keep this fn is sync with
 with the one in fsbl-shmoo.c
*/
void shmoo_load(void) {
	/* struct board_nvm_info is now loaded
	by nvm_load() in fsbl-board.c */
}

struct ddr_info *shmoo_ddr_by_index(uint32_t d) {
	return NULL;
}

void shmoo_set(const struct ddr_info *ddr, bool warm_boot) {
	if (warm_boot)
		die("warm boot not supported");
	__puts("MEMSYS #");
	putchar('0' + ddr->which);
	memsys_init_all(ddr->which);
	puts(" DONE");
}

void shmoo_menu(struct board_nvm_info *nvm) {
}

void print_shmoo_version(void) {
	puts("NOSHMOO");
}
