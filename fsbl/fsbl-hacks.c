/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "fsbl-hacks.h"
#include "pinmux.h"
#include "chipid.h"

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_sun_gisb_arb.h>
#include <bchp_timer.h>
#include <bchp_hif_cpubiuctrl.h>


#if defined(CONFIG_BCM7260) || defined(CONFIG_BCM7268) || defined(CONFIG_BCM7271)
void orion_hack_early_bus_cfg(void)
{
#if defined(CONFIG_BCM7260A0)
	/* limit0: RBUS 0x0_FFD0_0000..0x0_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0,
		(0xFFD00 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_UBUSCDBIT_SHIFT));
#endif

	/* limit4: MCP0 0x1_0000_0000..0x2_3FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4,
		(0x23FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_BUSNUM_SHIFT));
	/* limit5: MCP0 0x0_4000_0000..0x0_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5,
		(0xBFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_BUSNUM_SHIFT));
}
#endif


/* Macro works as long as the h/w ip blocks have the same offsets. Either
 next to each other, or as part of a larger block. Have to revisit and use
 an array if hardware locates them at non-identical offsets from each other.
*/
#define R_PZONE(x) (BPHYSADDR(BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG + \
			((x) * (BCHP_HIF_CPUBIUCTRL_CPU1_PWR_ZONE_CNTRL_REG - \
				BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG))))
#define F_PZONE(field) \
		(BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG_##field##_MASK)


#define BPCM_TIMEOUT_US 50000 /* 5 mS */
#define BPCM_WAIT_US 10


static void reg_bit_set(uintptr_t reg, uint32_t mask)
{
	DEV_WR(reg, DEV_RD(reg) | mask);
	BARRIER();
}

static void reg_bit_clr(uintptr_t reg, uint32_t mask)
{
	DEV_WR(reg, DEV_RD(reg) & (~mask));
	BARRIER();
}

static int wait_bit_clr(uintptr_t reg, uint32_t mask)
{
	int to = BPCM_TIMEOUT_US / BPCM_WAIT_US;

	while (to--) {
		if (!(DEV_RD(reg) & mask))
			return 0;
		udelay(BPCM_WAIT_US);
	}
	return 1;
}

/* cpu #0 is up and we want to make sure all other cpus (1..n)
are powered down until its Linux boot time.
*/
void hack_power_down_cpus(void) /* Ref:  HW7445-1743 */
{
	int i, rc, rd, ncpus = get_num_cpus();

	__puts("CPU 0");

	for (i = 1; i < ncpus; i++) {

		/* Its important to do ZONE_MAN_ISO_CNTL *first* */
		reg_bit_set(R_PZONE(i), F_PZONE(ZONE_MAN_ISO_CNTL));

		reg_bit_set(R_PZONE(i), F_PZONE(ZONE_MANUAL_CONTROL));
		reg_bit_clr(R_PZONE(i), F_PZONE(ZONE_MAN_RESET_CNTL));
		reg_bit_clr(R_PZONE(i), F_PZONE(ZONE_MAN_CLKEN));
		reg_bit_clr(R_PZONE(i), F_PZONE(ZONE_MAN_MEM_PWR));

		rc = wait_bit_clr(R_PZONE(i), F_PZONE(ZONE_MEM_PWR_STATE));

		/* This is actually DPG. */
		reg_bit_clr(R_PZONE(i), F_PZONE(reserved1));

		rd = wait_bit_clr(R_PZONE(i), F_PZONE(ZONE_DPG_PWR_STATE));

		/* For non existent cores we'll get 'X' or 'x'.
		If the pwr zone IP block goes wrong then
		it can read zero, so we catch that. NB: For
		production boxes we may be better off rebooting
		to a known cpu state if pwr zone is zero. */
		if (rc)
			__puts("X");
		else if (rd)
			__puts("x");
		else if (0 == DEV_RD(R_PZONE(i)))
			__puts("?");
		else
			writeint(i);
	}
	puts("");
}

