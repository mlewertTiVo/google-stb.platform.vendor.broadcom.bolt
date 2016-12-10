/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdbool.h>

#include <bchp_clkgen.h>
#include <bchp_hif_cpubiuctrl.h>
#include <bchp_sun_top_ctrl.h>

#include <arch_ops.h>
#include <arm-macros.h>
#include <board.h>
#include <board_init.h>
#include <boardcfg.h>
#include <bsp_config.h>
#include <chipid.h>
#include <common.h>
#include <error.h>
#include <ssbl-common.h>

/* Detailed handling against the given PTE is implementation specific.
 * And, the handling requires context information, page table entry type.
 */
typedef uint32_t (modify_page_table_entry)(int ptetype, uint32_t pte);

/* do_ranges_overlap -- checks if the given two ranges overlap
 *
 * NOTE: Protetion for zero size must be provided by the caller.
 *
 * Parameters:
 *  off_x  [in] starting address of range X
 *  size_x [in] size of range X
 *  off_y  [in] starting address of range Y
 *  size_y [in] size of range Y
 *
 * Returns
 *  true if they do overlap
 *  false otherwise
 */
static bool do_ranges_overlap(uint32_t off_x, uint32_t size_x,
	uint32_t off_y, uint32_t size_y)
{
	uint32_t x2 = off_x + size_x - 1;
	uint32_t y2 = off_y + size_y - 1;

	/*  .x1..x2... ..x1...x2. ...x1..x2... .x1......x2.
	 *  ....y1.y2. .y1..y2... .y1......y2. ...y1..y2...
	 */
	return (off_x <= y2) && (off_y <= x2);
}

uint64_t arch_get_cpu_freq_hz(void)
{
	int shift = BDEV_RD_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, CLK_RATIO);

	return arch_get_cpu_pll_hz() >> shift;
}

#if defined(BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG)
/* table of divisors for CPU/L2 clock */
static const unsigned int CPU_L2_DIV_TABLE[] = {
	 1, /* 0 - SEL_CLK_PATTERN  */
	 2, /* 1 - SEL_CLK_PATTERN */
	 3, /* 2 - SEL_CLK_PATTERN */
	 4, /* 3 - SEL_CLK_PATTERN */
	 8, /* 4 - SEL_CLK_PATTERN */
	16, /* 5 - SEL_CLK_PATTERN */
	32}; /* 6 - SEL_CLK_PATTERN */
#endif

/*
 * Source for the CPU PLL; can be divided dynamically with MDIV
 */
uint64_t arch_get_cpu_vco_hz(void)
{
	uint32_t regval;
	unsigned int pdiv, ndiv;
#if defined(BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG)
	unsigned int cpu_l2_div;
#endif

#if defined(BCHP_CLKGEN_PLL_CPU_PLL_DIV_4K)
	regval = BDEV_RD(BCHP_CLKGEN_PLL_CPU_DVFS_SM_CONTROL0);
	if (regval & BCHP_CLKGEN_PLL_CPU_DVFS_SM_CONTROL0_GOTO_IDLE_MASK) {
		/* DVFS NDIV state machine is NOT running */
		regval = BDEV_RD(BCHP_CLKGEN_PLL_CPU_PLL_DIV_4K);
		regval &= BCHP_CLKGEN_PLL_CPU_PLL_DIV_4K_NDIV_INT_MASK;
		ndiv = regval >> BCHP_CLKGEN_PLL_CPU_PLL_DIV_4K_NDIV_INT_SHIFT;
	} else {
		/* DVFS NDIV state machine is running, or has run */
		regval = BDEV_RD(BCHP_CLKGEN_PLL_CPU_DVFS_SM_STATUS);
		regval &= BCHP_CLKGEN_PLL_CPU_DVFS_SM_STATUS_NDIV_INT_MASK;
		ndiv = regval >>
			BCHP_CLKGEN_PLL_CPU_DVFS_SM_STATUS_NDIV_INT_SHIFT;
	}
	regval = BDEV_RD(BCHP_CLKGEN_PLL_CPU_PLL_DIV);
	pdiv = (regval & BCHP_CLKGEN_PLL_CPU_PLL_DIV_PDIV_MASK) >>
		 BCHP_CLKGEN_PLL_CPU_PLL_DIV_PDIV_SHIFT;

#elif defined(BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0)
	regval = BDEV_RD(BCHP_CLKGEN_PLL_CPU_PLL_DIV);
	pdiv = (regval & BCHP_CLKGEN_PLL_CPU_PLL_DIV_PDIV_MASK) >>
		BCHP_CLKGEN_PLL_CPU_PLL_DIV_PDIV_SHIFT;
	ndiv = (regval & BCHP_CLKGEN_PLL_CPU_PLL_DIV_NDIV_INT_MASK) >>
		BCHP_CLKGEN_PLL_CPU_PLL_DIV_NDIV_INT_SHIFT;
#else
#error CLKGEN info not found
#endif

#if !defined(BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG)
	return (uint64_t)CHIP_REFCLOCK_FREQUENCY / pdiv * ndiv;
#else
	regval = BDEV_RD_F(HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG,
		SEL_CLK_PATTERN);
	if (regval >= sizeof(CPU_L2_DIV_TABLE)/sizeof*(CPU_L2_DIV_TABLE)) {
		regval = BCHP_HIF_CPUBIUCTRL_CPU_CLUSTER0_CLOCK_CONTROL_REG_SEL_CLK_PATTERN_DEFAULT;
	}
	cpu_l2_div = CPU_L2_DIV_TABLE[regval];
	return (uint64_t)CHIP_REFCLOCK_FREQUENCY / pdiv * ndiv / cpu_l2_div;
#endif
}

static unsigned int arch_get_cpu_mdiv(void)
{
#if defined(BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0_4K)
	/* 7271a0, 7268a0, 7260a0 */
	return BDEV_RD_F(CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0_4K, MDIV_CH0);
#else
	return BDEV_RD_F(CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0, MDIV_CH0);
#endif
}

/* arch_get_cpu_pll_hz -- returns CPU PLL speed in HZ
 *
 * It is the CPU PLL clock, the baseline clock to A15. It can be further
 * controlled (divided) by HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG.CLK_RATIO.
 * Hence, the user experience-able CPU speed is after the division.
 *
 * returns the speed of CPU PLL in HZ
 * see also: arch_get_cpu_freq_hz()
 */
uint64_t arch_get_cpu_pll_hz(void)
{
	return arch_get_cpu_vco_hz() / arch_get_cpu_mdiv();
}

/* arch_get_midr -- returns Main ID Register, ARM specific
 *
 * returns MIDR
 */
uint32_t arch_get_midr(void)
{
	uint32_t midr;

	__asm__ ("mrc   p15, 0, %0, c0, c0, 0" : "=r" (midr));

	return midr;
}

int arch_get_num_processors(void)
{
	uint32_t num_processors;

#if defined(CONFIG_BCM74371A0)
	return 1; /* L2CTLR.NUMCPU reports 2 while only one is available. */
	/* Do not report dead code after this. It is intentional. */
	/* Coverity[dead_error_line] */
#elif defined(CONFIG_BCM7445D0)
	struct board_type *b = board_thisboard();

	if (b) {
		const uint32_t prid_without_rev = b->prid & ~CHIPID_REV_MASK;

		switch (prid_without_rev) {
		case 0x72520000:
		case 0x74480000:
		case 0x74490000:
			return 2; /* SWBOLT-277 */
		default:
			/* nothing to do */
			break;
		}
	}
#endif

	/* L2CTLR */
	__asm__ ("mrc	p15, 1, %0, c9, c0, 2" : "=r" (num_processors));

	return (int)(((num_processors >> 24) & 0x3) + 1);
}

/* arch_get_scb_freq_hz -- returns SCB frequency in HZ
 *
 * returns the speed of SCB frequency in HZ
 */
uint64_t arch_get_scb_freq_hz(void)
{
	uint64_t scbfreq;
	unsigned int regval;
	unsigned int pdiv, ndiv, mdiv;

#if defined(BCHP_CLKGEN_PLL_SCB_PLL_CHANNEL_CTRL_CH_0)
	/* channel#0 of an SCB dedicated PLL */
	regval = BDEV_RD(BCHP_CLKGEN_PLL_SCB_PLL_DIV);
	pdiv = (regval & BCHP_CLKGEN_PLL_SCB_PLL_DIV_PDIV_MASK) >>
		BCHP_CLKGEN_PLL_SCB_PLL_DIV_PDIV_SHIFT;
	ndiv = (regval & BCHP_CLKGEN_PLL_SCB_PLL_DIV_NDIV_INT_MASK) >>
		BCHP_CLKGEN_PLL_SCB_PLL_DIV_NDIV_INT_SHIFT;
	mdiv = BDEV_RD_F(CLKGEN_PLL_SCB_PLL_CHANNEL_CTRL_CH_0, MDIV_CH0);
#else
	/* SCB shares SYS0 PLL with others */
#if defined(CONFIG_BCM7439B0)
	/* channel#1 */
	regval = BDEV_RD(BCHP_CLKGEN_PLL_SYS0_PLL_DIV);
	pdiv = (regval & BCHP_CLKGEN_PLL_SYS0_PLL_DIV_PDIV_MASK) >>
		BCHP_CLKGEN_PLL_SYS0_PLL_DIV_PDIV_SHIFT;
	ndiv = (regval & BCHP_CLKGEN_PLL_SYS0_PLL_DIV_NDIV_INT_MASK) >>
		BCHP_CLKGEN_PLL_SYS0_PLL_DIV_NDIV_INT_SHIFT;
	mdiv = BDEV_RD_F(CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1, MDIV_CH1);
#elif defined(CONFIG_BCM7250B0) || \
	defined(CONFIG_BCM7260A0) || \
	defined(CONFIG_BCM7364) || \
	defined(CONFIG_BCM7366) || \
	defined(CONFIG_BCM74371A0)|| \
	defined(CONFIG_BCM7271A0)|| \
	defined(CONFIG_BCM7268A0)
	/* channel#2 */
	regval = BDEV_RD(BCHP_CLKGEN_PLL_SYS0_PLL_DIV);
	pdiv = (regval & BCHP_CLKGEN_PLL_SYS0_PLL_DIV_PDIV_MASK) >>
		BCHP_CLKGEN_PLL_SYS0_PLL_DIV_PDIV_SHIFT;
	ndiv = (regval & BCHP_CLKGEN_PLL_SYS0_PLL_DIV_NDIV_INT_MASK) >>
		BCHP_CLKGEN_PLL_SYS0_PLL_DIV_NDIV_INT_SHIFT;
	mdiv = BDEV_RD_F(CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2, MDIV_CH2);
#else
	#error "Please check how and where SCB frequency is configured."
#endif
#endif
	scbfreq = CHIP_REFCLOCK_FREQUENCY / pdiv * ndiv / mdiv;

	return scbfreq;
}

/* arch_get_sysif_freq_hz -- returns sysIF frequency in HZ
 *
 * Returns
 *  sysIF frequency in HZ
 *  0 (zero) if not applicable
 */
uint64_t arch_get_sysif_freq_hz(void)
{
#if !defined(BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K)
	return 0;
#else
	return arch_get_cpu_vco_hz() /
		BDEV_RD_F(CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K, MDIV_CH1);
#endif
}

uint32_t arch_get_timer_freq_hz(void)
{
	uint32_t timer_freq_hz;

	/* CNTFRQ */
	__asm__ ("mrc	p15, 0, %0, c14, c0, 0" : "=r" (timer_freq_hz));

	return timer_freq_hz;
}


/* Minimum CPU clock frequency allowed without using SAFE_MODE */
uint64_t arch_get_cpu_min_safe_hz(void)
{
	const uint64_t freq_scb = arch_get_scb_freq_hz();

	switch (board_num_active_memc()) {
	case 0: /* NO active MEMC does not make sense, just for safety */
	case 1:
		/* CPU should be faster than x2 of SCB */
		return freq_scb * 2;
	case 2:
	case 3:
	default:
		/* CPU should be faster than x3 of SCB */
		return freq_scb * 3;
	}
}

/* arch_is_safemode_required -- whether SAFE_CLK_MODE should be on
 *
 * Depending on the ratio between CPU and SCB frequencies, a special
 * hardware feature (SAFE_CLK_MODE) needs be enabled. Otherwise, system
 * might lock up. The ratio varies based on the number of active memory
 * controllers.
 *
 * returns true if SAFE_CLK_MODE needs be enabled, false otherwise
 */
static bool arch_is_safemode_required(void)
{
	return arch_get_cpu_freq_hz() < arch_get_cpu_min_safe_hz();
}

/* walk_page_table -- walks through page table for the given area, and
 *   updates page table entry(ies) based on 'action'
 *
 * Parameters:
 *  addr   [in] starting address of target area in bytes, 4KB aligned
 *  size   [in] size of target area in bytes, 4KB aligned
 *  action [in] pointer to a function that returns updated PTE
 *
 * Returns
 *  0 on success
 *  BOLT_ERR_INV_PARAM if addr or size do not meet assumptions (see inline)
 *  BOLT_ERR_NOMEM when finding unrecognizable PTE
 */
static int walk_page_table(uintptr_t addr, unsigned int size,
	modify_page_table_entry action)
{
	uint32_t *pte = (uint32_t *)board_info()->pte;
	uintptr_t off;

	/* 'addr' and 'size' should be page aligned. Since the size of
	 * a page can be either 4KB or 1MB, checking the alignment of
	 * a requested range would look complicated.
	 *
	 * BAD cases:
	 *   size == 0
	 *   (addr & 0xFFF) != 0, becuase of the minimum size of a page
	 *   (size & 0xFFF) != 0, becuase of the minimum size of a page
	 *   addr + size > 4GB, no wrap around
	 *
	 * accepted cases (example):
	 *   addr == 0, the guard page will not altered
	 */
	if (size == 0 || (addr % 4096) != 0 || (size % 4096) != 0)
		return BOLT_ERR_INV_PARAM;
	if ((addr/_KB(1) + size/_KB(1)) > _MB(4)) /* (addr + size) > 4GB ? */
		return BOLT_ERR_INV_PARAM;

	CACHE_INVAL_ALL();

	off = addr & SECTION_MASK;
	pte += addr >> SECTION_SHIFT;
	do {
		uint32_t page = *pte;

		if (page & SECTION) {
			*pte = action(PTE_SECTION, page);
		} else if (page & PT_TYPE) {
			/* 2nd-level page table with 'small' 4KiB pages */
			uint32_t *pt2 = (uint32_t *)(page & PT_BASEADDR_MASK);
			unsigned int i;

			for (i = 0; i < PT2_ENTRIES; i++) {
				uint32_t page2 = pt2[i];
				uintptr_t off2;

				if (!(page2 & SMALL_PG_TYPE))
					continue; /* large page or fault */

				off2 = off + i * SMALL_PG_SIZE;
				if (!do_ranges_overlap(addr, size,
					off2, SMALL_PG_SIZE))
					continue;

				pt2[i] = action(PTE_SMALL, page2);
			}
		} else {
			return BOLT_ERR_NOMEM; /* some other page type */
		}

		pte++;
		off += SECTION_SIZE;
	} while (do_ranges_overlap(addr, size, off, SECTION_SIZE));

	tlb_flush();
	CACHE_INVAL_ALL();
	return 0;
}

static uint32_t mark_pte_uncacheable(int ptetype, uint32_t pte)
{
	/*
	 * Change normal cached memory to  Device Memory (TEX=0, C=0, B=1)
	 * so the uncached accesses are never reordered.
	 */
	switch (ptetype) {
	case PTE_SECTION:
		pte &= ~(BUFFERABLE | CACHABLE | TEX_2_0(7));
		pte |= SECT_DEVICE;
		break;
	case PTE_SMALL:
		pte &= ~(SMALL_PG_BUFFERABLE | SMALL_PG_CACHABLE |
			SMALL_PG_TEX_2_0(7));
		pte |= SMALL_PG_DEVICE;
		break;
	default:
		/* do nothing */
		break;
	}

	return pte;
}

static uint32_t mark_pte_executable(int ptetype, uint32_t pte)
{
	switch (ptetype) {
	case PTE_SECTION:
		pte &= ~XN;
		break;
	case PTE_SMALL:
		pte &= ~SMALL_PG_XN;
		break;
	default:
		/* do nothing */
		break;
	}

	return pte;
}

static uint32_t mark_pte_nonexecutable(int ptetype, uint32_t pte)
{
	switch (ptetype) {
	case PTE_SECTION:
		pte |= XN;
		break;
	case PTE_SMALL:
		pte |= SMALL_PG_XN;
		break;
	default:
		/* do nothing */
		break;
	}

	return pte;
}

void arch_mark_uncached(uintptr_t addr, unsigned int size)
{
	(void) walk_page_table(addr, size, mark_pte_uncacheable);
}

int arch_mark_executable(uintptr_t addr, unsigned int size, bool executable)
{
	if (executable)
		return walk_page_table(addr, size, mark_pte_executable);
	return walk_page_table(addr, size, mark_pte_nonexecutable);
}

/* arch_pte_type -- returns the type of a given page table entry
 *
 * Parameters:
 *  first_level [in] whether pte is from 1st level page table
 *  pte         [in] Page Table Entry whose type is to be returned
 *
 * Returns
 *  PTE_TABLE   if pointer to 2nd level table
 *  PTE_SECTION if 1MiB in 1st level
 *  PTE_SS      if super section, 16MB in 1st level
 *  PTE_LARGE   if 64KB in 2nd level
 *  PTE_SMALL   if 4KB in 2nd level
 *  PTE_FAULT   otherwise
 *
 *                31             16  15              0
 *                +---+---+---+---   +---+---+---+---+
 * FAULT:         ................   ...............00
 * page table:    ................   ...............01
 * Section:       .............0..   ...............1.
 * Super Section: .............1..   ...............1.
 * Reserved:      ................   ...............11
 *
 *                31             16  15              0
 *                +---+---+---+---   +---+---+---+---+
 * FAULT:         ................   ...............00
 * Large page:    ................   ...............01
 * Small page:    ................   ...............1.
 *
 */
int arch_pte_type(bool first_level, uint32_t pte)
{
	const uint32_t last_two_bits = pte & 0x3;

	if (last_two_bits == 0x0)
		return PTE_FAULT;

	if (first_level) {
		/* entry in 1st level page table */
		if (last_two_bits == 0x1)
			return PTE_TABLE;
		if ((pte & BIT(18)) == 0 && (pte & BIT(1)) == BIT(1))
			return PTE_SECTION;
		if ((pte & BIT(18)) == BIT(18) && (pte & BIT(1)) == BIT(1))
			return PTE_SS;

		return PTE_FAULT;
	}

	/* entry in 2nd level page table */
	if (last_two_bits == 0x1)
		return PTE_LARGE;
	else
		return PTE_SMALL;
}

void arch_dump_registers(struct arm_regs *regs)
{
	int i;
	uint32_t cpsr;

	static const char * const names[] = {
		" r0     : ",
		" r1     : ",
		" r2     : ",
		" r3     : ",
		" r4     : ",
		" r5     : ",
		" r6     : ",
		" r7     : ",
		" r8     : ",
		" r9     : ",
		"r10     : ",
		"r11     : ",
		"r12     : ",
		"r13 (sp): ",
		"r14 (lr): ",
		"r15 (pc): ",
		"cpsr    : ",
		"dfsr    : ",
		"dfar    : ",
		"ifar    : ",
	};

	__puts("\n\nCPU exception: ");

	__asm__("mrs %0, CPSR" : "=r" (cpsr));
	switch (cpsr & CPSR_MODE_MASK) {
	case MODE_FIQ:
		__puts("FIQ\n");
		break;
	case MODE_IRQ:
		__puts("IRQ\n");
		break;
	case MODE_ABT:
		__puts("ABT\n");
		break;
	case MODE_UND:
		__puts("UND\n");
		break;
	default:
		__puts("unknown\n");
		break;
	}

	for (i = 0; i < 16; i++) {
		__puts(names[i]);
		writehex(regs->gprs.r[i]);
		__puts("\n");
	}

	__puts(names[16]);
	writehex(regs->cpsr);
	__puts("\n");

	__puts(names[17]);
	writehex(regs->dfsr);
	__puts("\n");

	__puts(names[18]);
	writehex(regs->dfar);
	__puts("\n");

	__puts(names[19]);
	writehex(regs->ifar);
	__puts("\n");
}


/* HW7439-655. Verify against e.g. HW7366-587
before adding newer chips listed there. */
#if	defined(CONFIG_BCM7260A0) || \
	defined(CONFIG_BCM7268A0) || \
	defined(CONFIG_BCM7271A0) || \
	defined(CONFIG_BCM7366C0) || \
	defined(CONFIG_BCM7439B0)
#define HW7445_1480 0
#else
#define HW7445_1480 1
#endif

/* SWBOLT-876 --------------------------------------------------------------

 The following functionality is required when moving to 1:1 cpu clk ratio
on existing chips.
*/

#define TICKS_COUNT 0x200ULL

 /* NB: isb()'s added due to co-processor reads that can occur
speculatively and out of order relative to other instructions
executed on the same processor, on page B8-1960 of ARM Architectural
Reference Manual (ARMv7-A and ARMv7-R). */


/* PMCNTENSET: Performance Monitors Count Enable SET register (B6.1.74) */
static void perf_cnt_enable_set(uint32_t v)
{
	isb();
	__asm__ ("mcr     p15, 0, %0, c9, c12,1"
		: : "r" (v));
}


/* PMCNTENCLR: Performance Monitors Count Enable CLEAR register (B6.1.73) */
static void perf_cnt_disable_set(uint32_t v)
{
	isb();
	__asm__ ("mcr     p15, 0, %0, c9, c12,2"
		: : "r" (v));
}


/* PMCCNTR: Performance Monitors Cycle Count Register (B6.1.71)
Get and set processor clock cycle counter. */
static uint32_t get_perf_cpu_cycle_counter(void)
{
	uint32_t count;

	isb();
	__asm__ ("mrc     p15, 0, %0, c9, c13,0"
		: "=r" (count) : : "memory");
	return count;
}

static void set_perf_cpu_cycle_counter(uint32_t count)
{
	isb();
	__asm__ ("mcr     p15, 0, %0, c9, c13,0"
		: : "r" (count));
}


/* PMCR: Performance Monitors Control Register (B4.1.117) */
static uint32_t get_perf_ctl_reg(void)
{
	uint32_t v;

	isb();
	__asm__ ("mrc     p15, 0, %0, c9, c12,0"
		: "=r" (v) : : "memory");
	return v;
}

static void set_perf_ctl_reg(uint32_t v)
{
	isb();
	__asm__ ("mcr     p15, 0, %0, c9, c12,0"
		: : "r" (v));
}


/* Enable or disable the CPU performance monitoring
within the ARM PMU (C12: The Performance Monitors Extension)
*/
static void enable_perf_monitor(void)
{
	uint32_t val;

	/* C, bit[31] 1 = Cycle counter enabled */
	perf_cnt_enable_set(0x1 << 31);

	val = get_perf_ctl_reg();
	/*	E, bit[0] 1 = All counters are enabled
		D, bit[3] = 0 = PMCCNTR counts every clock cycle.
	*/
	set_perf_ctl_reg(val | 0x1);
}

static void disable_perf_monitor(void)
{
	uint32_t val;

	/* C, bit[31] 0 = Cycle counter disabled. */
	perf_cnt_disable_set(0x1 << 31);

	val = get_perf_ctl_reg();
	/* E, bit[0] 0 = All counters (including PMCCNTR) are disabled */
	set_perf_ctl_reg(val & (~0x1));
}


static int is_badclock(int e1, int e2, int r1, int r2)
{
	int ratio, e_ratio;

	if (r2 > r1) {
		ratio = r2 / r1;
		e_ratio = (e1 + e2 / 2) / e2;
	} else {
		ratio = r1 / r2;
		e_ratio = (e2 + e1 / 2) / e1;
	}
	return(ratio != e_ratio);
}

static void nop_loop(void)
{
	  int i;

	  for (i = 0; i <= 2; i++) {
		__asm__ ("nop; nop; nop; nop; nop;");
		__asm__ ("nop; nop; nop; nop; nop;");
	  }
}

/* The do..while loop will never exit if we don't
use BARRIER to make sure we get completion. Other things
like __optimize_O0 on the function or volatile for all
the variables involved can mask this and make it appear
to work OK.
*/
static int timed_loop(uint64_t count)
{
	uint64_t tick1, tick2, diff, cyclecount;

	set_perf_cpu_cycle_counter(0);

	/* As long as we don't spend days in the FSBL
	menu this counter should not wrap.*/
	tick1 = arch_getticks64();
	BARRIER(); /* fully complete */

	do {
		nop_loop();
		tick2 = arch_getticks64();
		BARRIER();

		diff = tick2 - tick1;
	} while (diff <= count);

	cyclecount = (uint64_t)get_perf_cpu_cycle_counter();

	return (int)(cyclecount / diff);
}


static inline void wr_clk_config_safemode(uint32_t ratio)
{
	uint32_t v;

	v = BDEV_RD(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG) &
		(~BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CLK_RATIO_MASK);

	v |= BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_SAFE_CLK_MODE_MASK |
		ratio;

	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, v);
	BARRIER(); /* make sure it completes before we move on */
}


static void set_cpu_ratio(int new_ratio)
{
	int current_ratio, current_delta, new_delta;
	int c_current_ratio, c_new_ratio;
#if (CFG_CMD_LEVEL >= 5) /* for debug peek via BBS */
	int t = 0;

	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_SCRATCH_REG, t);
	dsb();
#endif

	current_ratio =
		BDEV_RD_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, CLK_RATIO);

	c_current_ratio = 1 << current_ratio;
	c_new_ratio = 1 << new_ratio;

	enable_perf_monitor();

	do {
		/* make sure safemode is on while
		re-setting the current clock ratio
		*/
		wr_clk_config_safemode(current_ratio);
		current_delta = timed_loop(TICKS_COUNT);

		/* New clock ratio (with safemode still on) */
		wr_clk_config_safemode(new_ratio);
		new_delta = timed_loop(TICKS_COUNT);

	} while (is_badclock(current_delta, new_delta,
				c_current_ratio, c_new_ratio));
	disable_perf_monitor();

	/* Turning off safemode, if OK */
	if (!arch_is_safemode_required()) {
		BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG,
			SAFE_CLK_MODE, 0);
		BARRIER();
	}

#if (CFG_CMD_LEVEL >= 5)
	t =  (current_ratio << 24) |
		((current_delta & 0xff) << 16) |
		(new_ratio << 8) |
		((new_delta & 0xff));

	BDEV_WR(BCHP_HIF_CPUBIUCTRL_CPU_SCRATCH_REG, t);
#endif
}


/**********************************************************************
  * arch_set_cpu_clk_ratio
  *
  *  The cpu PLL sets the base speed, but before it enters the cpu
  * complex it can be further (binary) divided down by this function.
  *
  * Input parameters:
  *     ratio: One of the CPU_CLK_RATIO_* enumerations that represent
  *           selectable ratios of the final divider from the cpu
  *           PLL into the cpu.
  *
  *  Return value:
  *     nothing
  **********************************************************************/
void arch_set_cpu_clk_ratio(int ratio)
{
	if (HW7445_1480) {
		set_cpu_ratio(ratio);
		return;
	}

#if defined(BCHP_HIF_CPUBIUCTRL_SYSIF_BPCM_ID)
	BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, CLK_RATIO, ratio);
	/* With sysIF, turning on/off SAFE_MODE is not required when
	 * changing CPU clock ratio.
	 */
#else
	/* else with HW7445-1480 fixed. */
	BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, SAFE_CLK_MODE, 1);
	BARRIER();

	BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG, CLK_RATIO, ratio);
	BARRIER();

	if (!arch_is_safemode_required()) {
		BDEV_WR_F(HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG,
				SAFE_CLK_MODE, 0);
		BARRIER();
	}
#endif
}


int arch_booted64(void)
{
#ifdef STUB64_START
	struct fsbl_info *info = board_info();

	if (!info) /* default */
		return 0;

	if (info->runflags & FSBL_RUNFLAG_A64_BOOT)
		return 1;
#endif
	return 0;
}

/**********************************************************************
  * arch_get_cpu_bootname
  *
  *  Get the bootup type (64 or 32 bit) that FSBL (possibly via STUB64)
  * reports. This helps if FSBL is set to no UART output.
  *
  * Input parameters:
  *     none
  *
  *  Return value:
  *     name of the bootup mode as a string
  **********************************************************************/
const char *arch_get_cpu_bootname(void)
{
#ifdef STUB64_START
	return (arch_booted64()) ? "B53 (64-bit)" : "B53 (32-bit)";
#endif
	return "B15";
}
