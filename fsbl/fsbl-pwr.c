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
#if CFG_BATTERY_BACKUP

/* Very 7145a0 specific at the moment */

#include <common.h>
#include <lib_types.h>

#include <bchp_common.h>
#include <bchp_aon_ctrl.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_cm_top_ctrl.h>
#include <bchp_mbox_cpuc.h>
#include <bchp_aon_pm_l2.h>
#include <bchp_aon_pm_bbm_l2.h>
#include <bchp_cpu_comm_regs_cpuc.h>

#include "fsbl.h"
#include "sysmailbox.h"

/* SWBOLT-143

 If we're woken up or rebooted & CFG_BATTERY_BACKUP
then check the state:

 1. Cold boot, a/c power on:
	boot as normal.

 2. Cold boot from battery ONLY :
	boot as normal (OS should enable minimal hardware).

 3. Woken from battery sleep (a/c restored after a a/c lost):
	Bypass AVS processing and SHMOOING then boot as normal.

*/

int fsbl_docsis_booted(uint32_t *aon_reset_history, uint32_t *pm_ctrl)
{
	static uint32_t arh = 0, pmc = 0;
	static int docsis_booted = 0, first = 1;

	if (first) {
		arh = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY);
		pmc = BDEV_RD(BCHP_AON_CTRL_PM_CTRL);

		docsis_booted = (
			!(arh & BCHP_AON_CTRL_RESET_HISTORY_cm_s3_wakeup_reset_MASK) &&
			(arh & BCHP_AON_CTRL_RESET_HISTORY_s3_wakeup_reset_MASK)
		);

		__puts("arh="); writehex(arh); puts("");
		__puts("pmc="); writehex(pmc); puts("");
		first = 0;
	}

	if (aon_reset_history)
		*aon_reset_history = arh;

	if (pm_ctrl)
		*pm_ctrl = pmc;

	return docsis_booted;
}


/* Return true if we are running on battery. */
int fsbl_booted_on_battery(void)
{
	/* read MBOx0 to check is system is running on AC or battery power
	do this read after AVS balance is executed (either by HostCPU or AVS)
	*/
	int mbox0 = BDEV_RD(BCHP_MBOX_CPUC_DATA0);
	__puts("mbox0="); writehex(mbox0); puts("");
	mbox0 = (mbox0 & BCHP_MBOX_CPUC_DATA0_POWER_STATE_MASK) >>
		BCHP_MBOX_CPUC_DATA0_POWER_STATE_SHIFT;
	return !(mbox0 == BCHP_MBOX_CPUC_DATA0_POWER_STATE_ac_is_present);
}


void inform_ecm(uint32_t rstate, uint32_t pstate)
{
	uint32_t mbox3 = BDEV_RD(BCHP_MBOX_CPUC_DATA3);

	mbox3 &= ~(BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_MASK |
				BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_MASK);

	mbox3 |= (rstate << BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_SHIFT) |
			 (pstate << BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_SHIFT);

	BDEV_WR(BCHP_MBOX_CPUC_DATA3, mbox3);

	__puts("mbox3="); writehex(BDEV_RD(BCHP_MBOX_CPUC_DATA3)); puts("");
}


/* This has to be done after AVS_Balance is achieved. */
int fsbl_C1_check(struct fsbl_info *info)
{
	int running_on_ac = 0, docsis_booted = 0;
	uint32_t val;

	running_on_ac = !fsbl_booted_on_battery();
	docsis_booted = fsbl_docsis_booted(0, 0);

	info->powerdet = (running_on_ac) ? POWER_DET_STD : POWER_DET_MINIMAL;

	if (running_on_ac) {
		__puts("AC");
	    /* clear AON_BBM_PM_L2.CPU_MASK_STATUS.bmu_intr_ap_restored */
		val = BDEV_RD(BCHP_AON_PM_L2_CPU_MASK_STATUS);
		val &= ~BCHP_AON_PM_L2_CPU_STATUS_BMU_INTR_AP_RESTORED_MASK;
	    BDEV_WR(BCHP_AON_PM_L2_CPU_MASK_STATUS, val);
	} else {
		__puts("BATT");
	}
	puts(" power");

	__puts("DOCSIS ");
	if (docsis_booted) { /* DOCSIS up, this is boot from BBM */
		puts("booted");
		if (running_on_ac) {
			/* ac present and docsis up - this is wakeup from BBM.
			 CPU_comm major delay is to be set to 100us
			allow time for pull-the-plug to work
			do not arm pull-the-plug to allow handdling of bmu_intr_bat_low
			or bus_oftware_intr later in linux */
			puts("comm delay");
			BDEV_WR(BCHP_CPU_COMM_REGS_CPUC_AP_LOST_MAJOR_DELAY, 100);
		} else	{
			puts("Dying Gasp!");
			/* ac is not present and docsis is running - hence dying gasp or
			head-end intr wakeup bmu_intr_batt_low and ubus_software_intr
			interrupts are serviced in Linux */
		}
	} else {
		puts("NOT booted!");
		if (running_on_ac) { /* cold boot on ac */
			__puts("STD");
			BDEV_WR(BCHP_SUN_TOP_CTRL_HW_RESET_EXTENSION, 50*27 *2); //0xa8c);
			BDEV_WR(BCHP_CM_TOP_CTRL_HW_RESET_EXTENSION, 50*108); //0x1518);

			/* extend STB & CM ph3_hw_reset_assertion period by 50 ms to allow
			voltage to reach default devles before de-assertion	time in SBT uses
			54 MHz clock, timer in CM side uses 108 MHz */
			val = BDEV_RD(BCHP_AON_CTRL_PM_CTRL);
			val |= (1 << BCHP_AON_CTRL_PM_CTRL_reboot_on_powerloss_SHIFT);
			BDEV_WR(BCHP_AON_CTRL_PM_CTRL, val);
		} else	{ /* cold boot on battery */
			__puts("LEAN");
			/* leave auto_battery_mode and force battery_mode at default value 0
			this is code boot-on-battery as we are booting for the first time
			We also want to boot a minimal Linux config ASAP to save power. */
		}
		puts(" BOOT");
	}
	return docsis_booted; /* don't do shmoo if eCM is up */
}

#endif /*CFG_BATTERY_BACKUP*/

