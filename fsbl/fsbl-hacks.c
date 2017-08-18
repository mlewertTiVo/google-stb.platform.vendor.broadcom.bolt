/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "fsbl-hacks.h"

#include <addr-mapping.h>
#include <bchp_common.h>
#include <bchp_hif_cpubiuctrl.h>
#include <bchp_sun_gisb_arb.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_timer.h>

#if defined(CONFIG_BCM7260A0)
#include <bchp_ccl.h>
#include <bchp_mpm_cpu_ctrl.h>
#include <bchp_mpm_cpu_data_mem.h>
#endif

#if defined(CONFIG_BCM7278A0)
#include <bchp_aon_ctrl.h>
#include <bchp_clkgen.h>
#endif

#if defined(CONFIG_BCM7260A0)
static const uint32_t MPM_TIMEOUT = 3*27000000; /* 3 seconds */
static const uint32_t MPM_PATCH_DATA[] = { 0x0, 0xEA8, 0x0 };
static const uint32_t MPM_PATCH_OFFSET = 0x5D60;

static inline bool is_patch_done(void)
{
	return (0 != (BDEV_RD(BCHP_MPM_CPU_CTRL_PM_CTRL) &
		BCHP_MPM_CPU_CTRL_PM_CTRL_MPM_WAKEUP_SYSTEM_MASK));
}

void bcm7260a0_patch_mpm(void)
{
	uint32_t *dccm;
	uint32_t strap1;

	strap1 = BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_1);
	/* only when powered via USB-C */
	if (0 == (strap1 &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_1_strap_mpm_runmode_0_MASK))
		return;
	/* we should not have been here if booted in S3 mode */
	if (0 != (strap1 &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_1_strap_powerup_in_s3_MASK))
		return;

	/* has the patch been applied */
	if (is_patch_done())
		return;

	/* from now on, it is the "do or die" situation */

	/* take MPM out of reset */
	BDEV_WR(BCHP_MPM_CPU_CTRL_RESET_CTRL,
		(0 << BCHP_MPM_CPU_CTRL_RESET_CTRL_CPU_RESET_LEVEL_SHIFT));
	__puts("L");
	BARRIER();
	/* wait for MPM CPU to complete the reset before accessing it again */
	udelay(1);

	/* apply patch */
	dccm = (uint32_t *) BPHYSADDR(BCHP_MPM_CPU_DATA_MEM_WORDi_ARRAY_BASE +
		MPM_PATCH_OFFSET);
	memcpy(dccm, MPM_PATCH_DATA, sizeof(MPM_PATCH_DATA));
	__puts("P");
	BARRIER();

	/* mimic as if booted in S3 mode */
	BDEV_WR_F(MPM_CPU_CTRL_MISC_CTRL, SELECT_UART_PINS, 1);

	/* one-shot reset to MPM */
	BDEV_WR(BCHP_MPM_CPU_CTRL_RESET_CTRL,
		(1 << BCHP_MPM_CPU_CTRL_RESET_CTRL_CPU_RESET_ONE_SHOT_SHIFT));
	__puts("O");

	udelay(50000); /* 50 ms, otherwise new debounce may get overridden */
	BDEV_WR_F(CCL_TIMING_0, T_CC_DEBOUNCE, 100); /* 100 ms */
	__puts("D");

	/* wait for MPM to complete negotiation, FOREVER */
	while (!is_patch_done()) ;
	puts("W");
}
#endif

#if defined(CONFIG_BCM7260) || \
	defined(CONFIG_BCM7268) || \
	defined(CONFIG_BCM7271) || \
	defined(CONFIG_BCM7278A0)

#if defined(CONFIG_BCM7278A0)
static void fixup_v7_32(void)
{
	/* Fixup v7-32 memory map (power on reset values in parenthesis):
	 *      BUSNUM    START                         END
	 *---------------------------------------------------------------------
	 * RL#0  2    0x0_0000_0000                0x0_0bff_ffff
	 * RL#1  2    0x0_2000_0000                0x0_3fff_ffff
	 * RL#2  2    0x0_1000_0000                0x0_1fff_ffff
	 * RL#3  4    0x0_4000_0000                0x0_bfff_ffff(0x0_7fff_ffff)
	 * RL#4  4(0) 0x1_0000_0000                0x1_bfff_ffff
	 * RL#5  5(0) 0x0_c000_0000(0x0_4000_0000) 0x0_ffff_ffff(0x0_7fff_ffff)
	 * RL#6  5(0) 0x3_0000_0000                0x4_bfff_ffff(0x3_bfff_ffff)
	 * RL#7  4(0) 0x1_c000_0000(0x0_8000_0000) 0x2_7fff_ffff(0x0_b7ff_ffff)
	 * RL#8  0    0xc_0000_0000                0xc_bfff_ffff
	 * RL#9  0    0xd_0000_0000                0xd_ffff_ffff
	 * RL#10 0    0xe_0000_0000                0xe_ffff_ffff
	 */

	/* limit3: MCP0 0x0_4000_0000..0x0_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3,
		(0xBFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3_BUSNUM_SHIFT));
	/* limit4: MCP0 0x1_0000_0000..0x1_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4,
		(0x1BFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_BUSNUM_SHIFT));
	/* limit5: MCP1 0x0_C000_0000..0x0_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5,
		(0xFFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_ULIMIT_SHIFT) |
		(5 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5,
		(0xC0000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5_UBUSCDBIT_SHIFT));
	/* limit6: MCP1 0x3_0000_0000..0x4_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6,
		(0x4BFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6_ULIMIT_SHIFT) |
		(5 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6_BUSNUM_SHIFT));
	/* limit7: MCP0 0x1_C000_0000..0x2_7FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7,
		(0x27FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7,
		(0x1C0000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7_UBUSCDBIT_SHIFT));
}

static void fixup_v7_64(void)
{
	/* Fixup v7-64 memory map (power on reset values in parenthesis):
	 *      BUSNUM    START                         END
	 *---------------------------------------------------------------------
	 * RL#0  2    0x0_0000_0000                0x0_0bff_ffff
	 * RL#1  2    0x0_2000_0000                0x0_3fff_ffff
	 * RL#2  2    0x0_1000_0000                0x0_1fff_ffff
	 * RL#3  4    0x0_4000_0000                0x0_bfff_ffff(0x0_7fff_ffff)
	 * RL#4  2(0) 0x5_0000_0000(0x1_0000_0000) 0x8_ffff_ffff(0x1_bfff_ffff)
	 * RL#5  2(0) 0x9_0000_0000(0x0_4000_0000) 0xc_ffff_ffff(0x0_7fff_ffff)
	 * RL#6  5(0) 0x3_0000_0000                0x4_ffff_ffff(0x3_bfff_ffff)
	 * RL#7  4(0) 0x0_8000_0000                0x2_3fff_ffff(0x0_b7ff_ffff)
	 * RL#8  0    0xf_0000_0000(0xc_0000_0000) 0xf_ffff_ffff(0xc_bfff_ffff)
	 * RL#9  0    0xd_0000_0000                0xd_ffff_ffff
	 * RL#10 0    0xe_0000_0000                0xe_ffff_ffff
	 */

	/* limit3: MCP0 0x0_4000_0000..0x0_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3,
		(0xBFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3_BUSNUM_SHIFT));
	/* limit4: RBUS 0x5_0000_0000..0x8_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4,
		(0x8FFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_ULIMIT_SHIFT) |
		(2 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT4,
		(0x500000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT4_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT4_UBUSCDBIT_SHIFT));
	/* limit5: RBUS 0x0_C000_0000..0x0_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5,
		(0xCFFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_ULIMIT_SHIFT) |
		(2 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5,
		(0x900000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5_UBUSCDBIT_SHIFT));
	/* limit6: MCP1 0x3_0000_0000..0x4_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6,
		(0x4FFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6_ULIMIT_SHIFT) |
		(5 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT6,
		(0x300000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT6_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT6_UBUSCDBIT_SHIFT));
	/* limit7: MCP0 0x0_8000_0000..0x2_3FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7,
		(0x23FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7,
		(0x80000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7_UBUSCDBIT_SHIFT));
	/* limit8: NOBUS 0xF_0000_0000..0xF_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT8,
		(0xFFFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT8_ULIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT8_BUSNUM_SHIFT));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT8,
		(0xF00000 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT8_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT8_UBUSCDBIT_SHIFT));
}
#endif

void orion_hack_early_bus_cfg(void)
{
#if defined(CONFIG_BCM7278A0)
	/* Hard code SCB to 486 MHz until PMap's are defined,
	 * then AVS will take care of it.
	 */
	BDEV_WR(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1,
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_POST_DIVIDER_HOLD_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_POST_DIVIDER_HOLD_CH1_SHIFT ) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_PHASE_OFFSET_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_PHASE_OFFSET_CH1_SHIFT) |
		(8 << BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_SHIFT) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_CLOCK_DIS_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_CLOCK_DIS_CH1_SHIFT));

	/* TODO: 432 MHz of VICE, should not hard code as not always used */
	BDEV_WR(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3,
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_POST_DIVIDER_HOLD_CH3_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_POST_DIVIDER_HOLD_CH3_SHIFT ) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_PHASE_OFFSET_CH3_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_PHASE_OFFSET_CH3_SHIFT) |
		(9 << BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_SHIFT) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_CLOCK_DIS_CH3_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3_CLOCK_DIS_CH3_SHIFT));
#endif

#if defined(CONFIG_BCM7260A0)
	/* limit0: RBUS 0x0_FFD0_0000..0x0_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0,
		(0xFFD00 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_UBUSCDBIT_SHIFT));
#endif

#if defined(CONFIG_BCM7260) || \
	defined(CONFIG_BCM7268) || \
	defined(CONFIG_BCM7271)
	/* limit4: MCP0 0x1_0000_0000..0x2_3FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4,
		(0x23FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_BUSNUM_SHIFT));
	/* limit5: MCP0 0x0_4000_0000..0x0_7FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5,
		(0x7FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_BUSNUM_SHIFT));
	/* limit7: MCP0 0x0_8000_0000..0x0_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7,
		(0xBFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7_BUSNUM_SHIFT));
#endif

#if defined(CONFIG_BCM7278A0)
	/* fixup range limits depending on v7-32 or v7-64 */
	if (is_mmap_v7_64())
		fixup_v7_64();
	else
		fixup_v7_32();
#endif
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

