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


#ifdef MEMSYS_TOP_API_H__

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


#else /* --------------------  original version (V1) -------------------- */


static void memsys_setup(struct memsys_setup_params *p);

/* --- Our API to FSBL --- */
struct memsys_interface mif INITSEG_RODATA = {
	.signature   = BOARD_MSYS_MAGIC, /* 'msy1' */
	.setup       = memsys_setup,
	.get_version = memsys_init_get_version,
	.init        = memsys_init,
	.shmoo_data  = NULL,
};


static inline int _write_none(int c)
{
	return 0;
}

/* stash function pointer here as 
 memsys_setup_params may only live for
 the life of the caller (stack var.)
*/
static putchar_t fsbl_uout = _write_none;
static udelay_t fsbl_udelay = NULL;
static get_syscount_t fsbl_get_syscount = NULL;

static void memsys_setup(struct memsys_setup_params *p)
{
	if (p->first) {
		fsbl_udelay = p->udt;
		fsbl_get_syscount = p->gst;

		if (_e_mcb_table > _f_mcb_table)
			mif.shmoo_data = (struct memsys_info *)_f_mcb_table;
	}	
	fsbl_uout = (p->console) ? p->console : _write_none;
}


/* --- Our APIs to memsysinitlib --- */


void _writeasm(char c)
{
	(void)fsbl_uout(c);
}

void _udelay(uint32_t us)
{
	fsbl_udelay(us);
}

uint32_t _get_syscount_32(void)
{
	return (uint32_t)(fsbl_get_syscount() & 0x00000000ffffffffULL);
}

uint64_t _get_syscount(void)
{
	return fsbl_get_syscount();
}

#endif /* -------------------- __MEMSYS_API_V? -------------------- */


/* support libgcc dependency
*/
int raise(int sig)
{
	return 0;
}

#endif /* defined(CFG_NOSHMOO) || defined(CFG_EMULATION) */
