/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
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
#include <boot_defines.h>

#ifdef BCHP_CCL_REG_START
#include <bchp_aon_ctrl.h>
#include <bchp_aon_pin_ctrl.h>
#include <bchp_ccl.h>
#endif

#if defined(CONFIG_BCM7260A0)
#include <bchp_ccl.h>
#include <bchp_mpm_cpu_ctrl.h>
#include <bchp_mpm_cpu_data_mem.h>
#endif

#if defined(CONFIG_BCM7278)
#include <bchp_aon_ctrl.h>
#include <chipid.h>
#endif

#if defined(CONFIG_BCM7255)
void bcm7255_enable_qam(void)
{
	BDEV_WR_F(SUN_TOP_CTRL_IP_DISABLE_0, ufe_disable, 0);
}
#endif

#if defined(CONFIG_BCM7260A0)
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

#if defined(CONFIG_BCM7260B0)
void bcm7260b0_bp3_apply_otp(void)
{
	uint32_t bp3_enable, rval, data;

	rval = sec_read_otp_bit(OTP_IP_LICENSING_CHECK_ENABLE_BIT, &bp3_enable);
	if (rval)
		sys_die(DIE_OTP_READ_FAILED, "OTP read failed");

	/* SUN_TOP_CTRL_GENERAL_CTRL_0 should not be adjusted afterward */
	if (bp3_enable) {
		/* set GISB 15 to protect SUN_TOP_CTRL_GENERAL_CTRL_0 except SAGE/BSP */
		REG(BCHP_SUN_GISB_ARB_BP_HI_START_ADDR_15) = 0x0;
		REG(BCHP_SUN_GISB_ARB_BP_START_ADDR_15) =
			BPHYSADDR(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0);
		REG(BCHP_SUN_GISB_ARB_BP_HI_END_ADDR_15) = 0x0;
		REG(BCHP_SUN_GISB_ARB_BP_END_ADDR_15) =
			BPHYSADDR(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0+0x3);
		REG(BCHP_SUN_GISB_ARB_BP_READ_15) =
			~(BCHP_SUN_GISB_ARB_BP_READ_15_scpu_0_MASK |
			BCHP_SUN_GISB_ARB_BP_READ_15_bsp_0_MASK);
		REG(BCHP_SUN_GISB_ARB_BP_WRITE_15) =
			~(BCHP_SUN_GISB_ARB_BP_WRITE_15_scpu_0_MASK |
			BCHP_SUN_GISB_ARB_BP_WRITE_15_bsp_0_MASK);
		REG(BCHP_SUN_GISB_ARB_BP_ENABLE_15) =
			(BCHP_SUN_GISB_ARB_BP_ENABLE_15_block_MASK |
			BCHP_SUN_GISB_ARB_BP_ENABLE_15_address_MASK |
			BCHP_SUN_GISB_ARB_BP_ENABLE_15_access_MASK);

		/* set GISB 14 to block access to GISB 14/15 except SAGE/BSP  */
		REG(BCHP_SUN_GISB_ARB_BP_HI_START_ADDR_14) = 0x0;
		REG(BCHP_SUN_GISB_ARB_BP_START_ADDR_14) =
			BPHYSADDR(BCHP_SUN_GISB_ARB_BP_HI_START_ADDR_14);
		REG(BCHP_SUN_GISB_ARB_BP_HI_END_ADDR_14) = 0x0;
		REG(BCHP_SUN_GISB_ARB_BP_END_ADDR_14) =
			BPHYSADDR(BCHP_SUN_GISB_ARB_BP_ENABLE_15+0x3);
		REG(BCHP_SUN_GISB_ARB_BP_READ_14) =
			~(BCHP_SUN_GISB_ARB_BP_READ_14_scpu_0_MASK |
			BCHP_SUN_GISB_ARB_BP_READ_14_bsp_0_MASK);
		REG(BCHP_SUN_GISB_ARB_BP_WRITE_14) =
			~(BCHP_SUN_GISB_ARB_BP_WRITE_14_scpu_0_MASK |
			BCHP_SUN_GISB_ARB_BP_WRITE_14_bsp_0_MASK);
		REG(BCHP_SUN_GISB_ARB_BP_ENABLE_14) =
			(BCHP_SUN_GISB_ARB_BP_ENABLE_14_block_MASK |
			BCHP_SUN_GISB_ARB_BP_ENABLE_14_address_MASK |
			BCHP_SUN_GISB_ARB_BP_ENABLE_14_access_MASK);
	} else {
		data = REG(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0);
		data &= ~(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_hdcp22_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_dv_hdr_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_vmxwatermarking_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_hdcp_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_macrovision_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_cwmcwatermarking_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_tc_itm_disable_MASK |
			BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_tc_hdr_disable_MASK);
		REG(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0) = data;
	}
}
#endif

#if defined(CONFIG_BCM7278B0)
void bcm7278_disable_hdcp(void)
{
	const uint32_t family_id = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);

	if ((family_id & CHIPID_REV_MASK) == 0x10)
		BDEV_WR_F(SUN_TOP_CTRL_IP_DISABLE_0, hdcp_disable, 1);
}
#endif

#if defined(CONFIG_BCM7255) || \
	defined(CONFIG_BCM7260) || \
	defined(CONFIG_BCM7268) || \
	defined(CONFIG_BCM7271) || \
	defined(CONFIG_BCM7278)

#if defined(CONFIG_BCM7278)
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
#if defined(CONFIG_BCM7260A0)
	/* limit0: RBUS 0x0_FFD0_0000..0x0_FFFF_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0,
		(0xFFD00 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_LLIMIT_SHIFT) |
		(0 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0_UBUSCDBIT_SHIFT));
#endif

#if defined(CONFIG_BCM7255A0)
	/* limit1: RBUS 0x0_C000_0000..0x0_F23F_FFFF */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT1,
		(0xF23FF << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT1_ULIMIT_SHIFT) |
		(2 << BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_ULIMT1_BUSNUM_SHIFT));
#endif

#if defined(CONFIG_BCM7255) || \
	defined(CONFIG_BCM7260) || \
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

#if defined(CONFIG_BCM7278)
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

/* hack_lpddr_reg -- uses the on-chip voltage sensor
 *
 *  Instead of using a dedicated power management component for sensing
 *  loss of power and perform power down sequencing, chips with USB-C
 *  may be able to detect power fail as well as brown out conditions
 *  through on-chip 5V voltage comparators without extra BOM's.
 */
void hack_lpddr_reg(void)
{
#ifdef CFG_EMULATION
	return; /* does not make sense in emulation */
#endif

	/* The existence of CCL does not guarantee LPDDR4_REG_EN. But,
	 * the existence of LPDDR4_REG_EN guarantees the existence of
	 * CCL, and its location is
	 * AON_PIN_CTRL_PIN_MUX_CTRL_3[aon_gpio_25].
	 */
#ifdef BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_gpio_25_LPDDR4_REG_EN
	/* enable sensor */
	BDEV_WR(BCHP_CCL_VBUS_SENSE_CFG_0, 0xA000);
	/* choose brownout signal, and force output to 1 */
	BDEV_WR(BCHP_AON_CTRL_GENERAL_CTRL_0, 7);
	/* internal pullup for aon_gpio_25 */
	BDEV_WR_F(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_2, aon_gpio_25_pad_ctrl, 2);
	/* LPDDR4_REG_EN */
	BDEV_WR_F(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_gpio_25, 3);
	BARRIER();

	sleep_ms(4); /* 4 milliseconds for the above sequence to stabilize */

	/* allow brownout signal to propagate */
	BDEV_WR(BCHP_AON_CTRL_GENERAL_CTRL_0, 5);
	BARRIER();
#endif
}
