/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "lib_types.h"
#include "lib_printf.h"
#include "timer.h"
#include "bolt.h"
#include "psci.h"
#include "arch_ops.h"
#include "loader.h"
#include <devtree.h>
#include "tz.h"
#include "tz_priv.h"

void tz_smm_set_params(bolt_loadargs_t *la)
{
#ifdef STUB64_START
	unsigned long b0, r0, r1, r2, r3;
	void *fdt = NULL;
	bolt_devtree_params_t p;
	struct tz_info *t;

	r0 = OEM_SMM_SET_PARMS;
	r1 = la->la_address;
	r2 = 0;
	r3 = la->la_flags;
	if (la->la_flags & LOADFLG_SECURE) {
		t = tz_info();
		if (t)
			r2 = (unsigned int)t->dt_address;
	} else {
		bolt_devtree_getenvs(&p);
		if (p.dt_address)
			fdt = p.dt_address;
		r2 = (unsigned long) fdt;
	}
	xprintf("SMM: set params for %s...\n",
                (la->la_flags & LOADFLG_SECURE) ? "Astra" : "Linux");

	b0 = psci(r0, r1, r2, r3);
	if (PSCI_SUCCESS != b0) {
		err_msg("SMM: set params failed 0x%lx\n", b0);
		return;
	}
#endif /* STUB64_START */
}
