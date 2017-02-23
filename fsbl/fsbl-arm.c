/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>
#include <ddr.h>
#include <lib_types.h>

#include "fsbl.h"
#include "arch-mmu.h"
#include "arm.h"
#include "cache_ops.h"
#include "config.h"

#include "bchp_clkgen.h"
#include "bchp_cntcontrolbase.h"
#include "bchp_hif_cpubiuctrl.h"

static void neon_access_enable(void)
{
	unsigned int tmp;

	/* CPACR - Co-processor Access Control Register */
	__asm__ __volatile__("mrc p15, 0, %0, c1, c0, 2" : "=r" (tmp));

	tmp &= ~(CPACR_CP(10, 0x3) | CPACR_CP(11, 0x3));
	tmp |= CPACR_CP(10, CPACR_PL1 | CPACR_PL0);
	tmp |= CPACR_CP(11, CPACR_PL1 | CPACR_PL0);

	__asm__ __volatile__("mcr p15, 0, %0, c1, c0, 2" : : "r" (tmp));
}

void late_cpu_init(void)
{
	/* SWBOLT-1037: Revert RAC aggressive prefetch as
	it may cause system instability due to synchronization
	issues. Data on the bus may get out of sync as there
	is no way to check a RAC flush completed, or is synced
	with the system cache flush and invalidate operations.

	WARNING: Changing the RAC settings is absolutely NOT
	RECOMMENDED nor supported by Broadcom. Broadcom
	cannot be held responsible for non-default RAC
	settings that may result in inexplicable corruptions
	involving peripherals capable of DMA, or other side
	effects that degrade overall system stability.
	*/
	const uint32_t rac_inst_cfg =
		RAC_SETTING(RAC_DISABLED, RAC_NO_PREF);
	const uint32_t rac_data_cfg =
		RAC_SETTING(RAC_DISABLED, RAC_NO_PREF);

	uint32_t __maybe_unused cntfreq;
#if !defined(CFG_EMULATION) && !defined(CFG_FULL_EMULATION)
	uint32_t regval;
#endif

	/* SCB of 216 MHz for 7268b0, 7271b0 and 7278a0
	 * The POR value of SCB is 38.88 MHz for reducing power spike
	 * when getting out of reset, but too slow. For now, boost it to
	 * 216 MHz. It needs be boosted even higher before Shmoo.
	 */
#if defined(CONFIG_BCM7268B0) || defined(CONFIG_BCM7271B0)
	BDEV_WR(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2,
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_POST_DIVIDER_HOLD_CH2_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_POST_DIVIDER_HOLD_CH2_SHIFT ) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_PHASE_OFFSET_CH2_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_PHASE_OFFSET_CH2_SHIFT) |
		(18 << BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_SHIFT) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_CLOCK_DIS_CH2_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2_CLOCK_DIS_CH2_SHIFT));
	dsb();
#elif defined(CONFIG_BCM7278A0)
	BDEV_WR(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1,
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_POST_DIVIDER_HOLD_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_POST_DIVIDER_HOLD_CH1_SHIFT ) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_PHASE_OFFSET_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_PHASE_OFFSET_CH1_SHIFT) |
		(18 << BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_SHIFT) |
		(BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_CLOCK_DIS_CH1_DEFAULT <<
			BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1_CLOCK_DIS_CH1_SHIFT));
	dsb();
#endif

#if defined(CFG_EMULATION) || defined(CFG_FULL_EMULATION)
	/* The A15 comes up in div-by-2 mode. Set to div-by-4 mode
	 (375MHz for 1.5GHz parts.)
	*/
	BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, CLK_RATIO, 2);
#else
	/* Full throttle CPU speed. arch_set_cpu_clk_ratio(CPU_CLK_RATIO_ONE)
	should be used for non-emulation targets i.e. older
	chips with HW7366-552 or HW7445-1480. */
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG 
	/* CPU/L2 clock control */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG, 0);
#endif
#ifdef BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K 
	BDEV_WR(BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K, 4); /* sysIF */
	dsb();
#endif

	/* Let's minimize read, modify and writes from/to the same register. */
	regval = BDEV_RD(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG);
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CCI_CLK_RATIO_MASK
	regval &= ~BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CCI_CLK_RATIO_MASK;
	/* divide by 2 when CCI_CLK_RATIO == 1*/
	regval |= 1 << BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CCI_CLK_RATIO_SHIFT;
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, regval);
	dsb();
#endif
	/* divide by 1 when CLK_RATIO == 0 */
	regval &= ~BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CLK_RATIO_MASK;
	/* skipping "regval |= 0 << CLK_RATIO_SHIFT;" as it is not required */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, regval);
	dsb();
	/* now turn off SAFE_CLK_MODE */
	regval &= ~BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_SAFE_CLK_MODE_MASK;
	/* skipping "regval |= SAFE_CLK_MODE;" as it is not required */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, regval);
	dsb();
#endif

	/* enable RAC */
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG,
		RAC_CFG(0, rac_inst_cfg, rac_data_cfg) |
		RAC_CFG(1, rac_inst_cfg, rac_data_cfg) |
		RAC_CFG(2, rac_inst_cfg, rac_data_cfg) |
		RAC_CFG(3, rac_inst_cfg, rac_data_cfg));
	BDEV_WR(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG1_REG, 0);

/* SWBOLT-1715: ALLOW_MCP_WRITE_PAIRING is set by a config
 * script whitelist in BcmUtils.pm::mcp_wr_pairing_allowed()
 */
#if !ALLOW_MCP_WRITE_PAIRING
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_CREDIT_REG_MCP0_WR_PAIRING_EN_MASK
	/*
	 * HW7445-1920: Disable MCP write pairing to improve stability
	 * on long term stress test.
	 *
	 * N.B. This assumes that if we have MCP0 write pairing then we
	 * also have it on MCP1 and MCP2.  This is true for all current and known
	 * future chips.
	 */
	const int wr_pairing_mask =
		BCHP_HIF_CPUBIUCTRL_CPU_CREDIT_REG_MCP2_WR_PAIRING_EN_MASK |
		BCHP_HIF_CPUBIUCTRL_CPU_CREDIT_REG_MCP1_WR_PAIRING_EN_MASK |
		BCHP_HIF_CPUBIUCTRL_CPU_CREDIT_REG_MCP0_WR_PAIRING_EN_MASK;
	BDEV_UNSET(BCHP_HIF_CPUBIUCTRL_CPU_CREDIT_REG, wr_pairing_mask);
#endif
#endif /* !ALLOW_MCP_WRITE_PAIRING */

	/* Stop CPU System Counter, and clear counter value
	*/
	BDEV_WR(BCHP_CNTControlBase_CNTCR, 0);
	BDEV_WR(BCHP_CNTControlBase_CNTCV_LO, 0);
	BDEV_WR(BCHP_CNTControlBase_CNTCV_HI, 0);

#ifdef STUB64_START
	/* If we booted via A64 @EL3 then we now don't have permission
	 * to write to cntfrq, but STUB64 has set it up for us anyway.
	 */
	if (0 == a53_bootmode()) {
#endif
		/* Set the ARM generic timer frequency using the CPU system
		 * counter. system counter clock @ BCHP_CNTControlBase_CNTFID0
		 * (27MHz - default)
		 */
		cntfreq = BDEV_RD(BCHP_CNTControlBase_CNTFID0);
		__asm__ __volatile__(
		"	mcr	p15, 0, %0, c14, c0, 0\n"
		: /* no outputs */
		: "r" (cntfreq)
		: "memory");
#ifdef STUB64_START
	}
#endif
	BARRIER();

	/* Start CPU System Counter with the 1st entry of the frequency modes
	*/
	BDEV_WR(BCHP_CNTControlBase_CNTCR,
			((1 << 0) << BCHP_CNTControlBase_CNTCR_FCREQ_SHIFT) |
			BCHP_CNTControlBase_CNTCR_EN_MASK);

	neon_access_enable();
}

#ifndef CFG_EMULATION

void mmu_initialize_pagetable(void)
{
	uint32_t *tbl = (uint32_t *)SSBL_PAGE_TABLE;

	/* mark everything as invalid */
	memset(tbl, 0, PT1_SIZE);

	/* flash */
	set_pte_range(tbl, FSBL_TEXT_ADDR, FSBL_TEXT_ADDR + _MB(256),
		SECT_DEVICE | XN);

	/* registers */
	set_pte_range(tbl, BCHP_PHYSICAL_OFFSET,
		BCHP_PHYSICAL_OFFSET + BCHP_REGISTER_END, SECT_DEVICE | XN);

	/* sram */
	set_sram_pages(tbl);
}

void mmu_add_pages(unsigned long base, unsigned long size)
{
	uint32_t *tbl = (uint32_t *)SSBL_PAGE_TABLE;

	/* sanity check on 'size' is done by the caller */
	set_pte_range(tbl, base, base + size,
#if !CFG_UNCACHED
		SECT_MEM_CACHED_WB | XN);
#else
		SECT_MEM_NONCACHED | XN);
#endif
}


void mmu_enable(void)
{
	uint32_t *tbl = (uint32_t *)SSBL_PAGE_TABLE;
	unsigned long tmp0;
	uint32_t sctlr_flags = SCTLR_MMU_ENABLE | SCTLR_DUCACHE_ENABLE;

	BARRIER();

	clear_all_d_cache();

	__asm__ __volatile__(

	/* translation table base control register (TTBCR) */
	"	mov	%0, #0\n"
	"	mcr	p15, 0, %0, c2, c0, 2\n"

	/* translation table base register 0 (TTBR0) */
	"	mcr	p15, 0, %2, c2, c0, 0\n"

	/* domain access control register (DACR) (Domain 0 as client) */
	"	mov	%0, #0xfffffffd\n"
	"	mcr	p15, 0, %0, c3, c0, 0\n"

	/* system control register (SCTLR) - enable MMU and caches */
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	orr	%0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"

	: "=&r" (tmp0)
	: "r" (sctlr_flags), "r" (tbl)
	: "memory");

	BARRIER();
}


void mmu_disable(void)
{
	unsigned long tmp0;
	uint32_t sctlr_flags = SCTLR_MMU_ENABLE | SCTLR_DUCACHE_ENABLE;

	clear_all_d_cache();

	__asm__ __volatile__(

	/* system control register (SCTLR) - disable MMU and caches */
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	and	%0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	"	isb\n"

	: "=&r" (tmp0)
	: "r" (~sctlr_flags)
	: "memory");

	clear_all_d_cache();

	tlb_flush();
}
#endif /* !CFG_EMULATION */

/*
 * Borrowed from SSBL's arch_get_num_processors(), except don't worry about
 * exceptions for bad chips. Thus, this is an upper bound on the number of
 * CPUs.
 */
int get_num_cpus(void)
{
	uint32_t n;

	/* L2CTLR */
	__asm__ ("mrc   p15, 1, %0, c9, c0, 2" : "=r" (n));

	return (int)(((n >> 24) & 0x3) + 1);
}

/* support libgcc dependency
*/
int raise(int sig)
{
	return 0;
}
