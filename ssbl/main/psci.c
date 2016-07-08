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
#include "board.h"


void bolt_psci_init(void)
{
#ifdef STUB64_START
	unsigned long b0, r0, r1;

	/* TBD: get nr_* from the RDB e.g. HIF_CONTINUATION or
	 * HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG. We can then use it
	 * for the EL3 stack calcs in 'calc_per_cpu_stack'
	 */
	unsigned int nr_clusters = 1;

	r0 = OEM_PSCI_INIT;
	r1 = (nr_clusters << OEM_PSCI_INIT_NR_CLUST_SHIFT) |
		(arch_get_num_processors() << OEM_PSCI_INIT_NR_CPU_SHIFT);

	r1 |= OEM_PSCI_INIT_DEBUG_MASK | OEM_PSCI_INIT_LOCK_MASK;
 
	xprintf("PSCI: Init...\n");
	b0 = psci(r0, r1, 0, 0);
	if (PSCI_SUCCESS != b0) {
		err_msg("PSCI: Init fail 0x%lx\n", b0);
		return;
	}

	xprintf("PSCI: v");
	r0 = PSCI_VERSION;
	b0 = psci(r0, 0, 0, 0);
	xprintf("%ld.%ld\n", PSCI_VERSION_MAX(b0), PSCI_VERSION_MIN(b0));

#endif /* STUB64_START */
}