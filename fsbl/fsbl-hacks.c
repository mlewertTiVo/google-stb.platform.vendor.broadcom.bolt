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


#if defined(CONFIG_BCM3390A0) || defined(CONFIG_BCM3390B0)
#include <bchp_g2u_regs.h>
#include <bchp_rg_top_ctrl.h>
#include <bchp_mbox_cpuc.h>
#endif

/* ------------------------------------------------------------------------- */
#if defined (CONFIG_BCM3390A0) || defined (CONFIG_BCM3390B0)
void bcm3390_hack_early_bus_cfg(void)
{
	/* enable UBUS clock */
	BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, UBUS_CLK_EN, 1);

	/* limit0: RBUS FFD0_0000 - FFFF_FFFF MPCore, Boot SRAM/ROM */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0, 0x00FFD000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT0, 0x00FFFFF2);

	/* limit1: RBUS E000_0000 - F1FF_FFFF EBI and GISB registers */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT1, 0x00E00000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT1, 0x00F1FFF2);

	/* limit2: RBUS 04_0000_0000 - 0B_FFFF_FFFF EBI and PCIe ext */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT2, 0x04000000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT2, 0x0BFFFFF2);

#if defined (CONFIG_BCM3390A0)
	/* limit3: MCP0 0000_0000 - 7FFF_FFFF DRAM-0 */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT3, 0x00000000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3, 0x007FFFF4);
#elif defined (CONFIG_BCM3390B0)
	/* limit3: MCP0 0000_0000 - BFFF_FFFF DRAM-0 */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT3, 0x00000000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT3, 0x00BFFFF4);
#endif

	/* limit4: Not used */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT4, 0x0);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4, 0x0);

	/* limit5: UBUS RDP D500_0000 - D6FF_FFFF UBUS registers endian swap */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT5, 0x00D50000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5, 0x00D6FFF1);

#if defined (CONFIG_BCM3390A0)
	/* limit6: MCP1 03_0000_0000 - 03_3FFF_FFFF DRAM-1 ext */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT6, 0x03000000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6, 0x033FFFF5);
#elif defined (CONFIG_BCM3390B0)
	/* limit6: MCP0 01_0000_0000 - 01_3FFF_FFFF DRAM-0 ext */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT6, 0x01000000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT6, 0x013FFFF4);
#endif

#if defined (CONFIG_BCM3390A0)
	/* limit7: MCP1 8000_0000 - BFFF_FFFF DRAM-1 */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7, 0x00800000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7, 0x00BFFFF5);
#elif defined (CONFIG_BCM3390B0)
	/* limit 7: No MCP1 for 3390B0 */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT7, 0x0);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT7, 0x0);
#endif

	/* limit8: UBUS D000_0000 - D4FF_FFFF UBUS registers  */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT8, 0x00D00001);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT8, 0x00D4FFF1);

	/* limit9: RBUS C000_0000 - CFFF_FFFF PCIe */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT9, 0x00C00000);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT9, 0x00CFFFF2);

	/* limit10: UBUS D700_0000 - DFFF_FFFF UBUS registers */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT10, 0x00D70001);
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT10, 0x00DFFFF1);
}

void bcm3390_hack_late_bus_cfg(void)
{
	BDEV_WR(BCHP_RG_TOP_CTRL_SW_INIT_0_CLEAR, 0x1F8000A3);

	sleep_ms(1);

	BDEV_WR(BCHP_G2U_REGS_G2U_WINDOW_START,   0xD0000000);
	BDEV_WR(BCHP_G2U_REGS_G2U_WINDOW_END,	  0xD7FFFFFF);
	BDEV_WR(BCHP_G2U_REGS_G2U_TRANSLATE_MASK, 0xFFFFFFFF);

	/* --- Attempt to read from CM UBUS Space --- */

	/* MBOX15: 'U' 'B' 'U' 'S' */
	BDEV_WR(BCHP_MBOX_CPUC_DATA15, 0x55425553);
	if (BDEV_RD(BCHP_MBOX_CPUC_DATA15) != 0x55425553)
		__puts("Warning! UBUS Access Failed!");
}
#endif /* defined(CONFIG_BCM3390A0) */

#if defined(CONFIG_BCM7268A0) || defined(CONFIG_BCM7271A0)
void orion_hack_early_bus_cfg(void)
{
	/* limit4: MCP0 0x1_0000_0000..0x1_3FFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4,
		(0x13FFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT4_BUSNUM_SHIFT));
	/* limit5: MCP0 0x0_4000_0000..0x0_BFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5,
		(0xBFFFF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_ULIMIT_SHIFT) |
		(4 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT5_BUSNUM_SHIFT));
}
#endif /* defined(CONFIG_BCM7268A0) || defined(CONFIG_BCM7271A0) */

#if defined(CONFIG_BCM7366B0) || defined(CONFIG_BCM7366C0)
void bcm7366b0_mii_rx_err_cfg(void)
{
	unsigned int i;

	/* Fixed in 7366c0 */
	if ((BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID)
			& CHIPID_MAJOR_REV_MASK) == 0x20)
		return;

	/* workaround for HW7366-517, MII_A_RX_ERR is multiplexed to both
	 * GPIO_126 and GPIO_093. We need to create some sort of clock
	 * using GPIO_079 to avoid this.
	 */
	for (i = 0; i < 5; i++) {
		PMUX_PADC(SUN_TOP, 8, gpio_079_pad_ctrl, PULL_UP);
		udelay(10);

		PMUX_PADC(SUN_TOP, 8, gpio_079_pad_ctrl, PULL_DOWN);
		udelay(10);
	}
}
#endif /* defined(CONFIG_BCM7366B0) */

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

