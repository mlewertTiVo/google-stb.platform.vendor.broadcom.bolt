/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <aarch64/armv8-regs.h>
#include <aarch64/armv8-cop.h>
#include <aon_defs.h>
#include "stub.64.h"

/*
 * On S3 wake reprogram VBAR_EL3 (PSCI vector table base)
 * and let FSBL later on decide if this is valid or not,
 * rebooting us if anything is dodgy.
 */
void pm_check_wokeup(void)
{
	uint32_t history = rdb_read(BCHP_AON_CTRL_RESET_HISTORY);
	uint64_t vbar_el3;

	if (history == BCHP_AON_CTRL_RESET_HISTORY_s3_wakeup_reset_MASK)
		vbar_el3 = (uint64_t)AON_REG(AON_REG_PSCI_BASE);
	else
		vbar_el3 = (uint64_t)PSCI_BASE;

	set_vbar_el3(vbar_el3);
	isb64();
}
