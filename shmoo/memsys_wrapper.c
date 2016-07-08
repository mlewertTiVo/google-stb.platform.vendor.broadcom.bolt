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

#if defined(CFG_NOSHMOO) || defined(CFG_EMULATION)
/* don't compile anything in this file
*/
#else

#include "../fsbl/fsbl.h"
#include "../fsbl/memsys-if.h" /* For struct memsys_interface */

/* mcb tables, if any */
extern uint32_t _f_mcb_table[];
extern uint32_t _e_mcb_table[];


static void memsys_setup(void);

struct memsys_interface mif INITSEG_RODATA = {
	.signature   = BOARD_MSYS_MAGIC, /* 'msy1' */
	.setup       = memsys_setup,
	.get_version = memsys_get_version,
	.init        = memsys_top,
	.shmoo_data  = NULL,
};

static void memsys_setup(void)
{
	if (_e_mcb_table > _f_mcb_table)
		mif.shmoo_data = (struct memsys_info *)_f_mcb_table;
}

/* support libgcc dependency
*/
int raise(int sig)
{
	return 0;
}

#endif /* defined(CFG_NOSHMOO) || defined(CFG_EMULATION) */
