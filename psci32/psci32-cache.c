/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bchp_sun_top_ctrl.h"
#include "bchp_hif_cpubiuctrl.h"

#include "common.h"
#include "arm.h"
#include "cache_ops.h"
#include "psci32.h"
#include "armv8.h"
#include "arm-macros.h"


void set_sys_ctl_reg(unsigned long sctlr)
{
	__asm__ __volatile__(
		"mcr " SCTLR("%0")
		: : "r" (sctlr)
	);
	BARRIER(); /* swbolt-797 */
}


unsigned long get_sys_ctl_reg(void)
{
	unsigned long sctlr;

	__asm__ __volatile__(
		"mrc " SCTLR("%0")
		: "=r" (sctlr)
	);
	BARRIER();

	return sctlr;
}



/* Disable before:
 * DC ISW
 * DC CSW
 * DC CISW
 * IC IALLUIS
 * IC IALLU
 */
uint32_t rac_disable_and_flush(void)
{
	uint32_t v, old;

	v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG);
	if (0 == v)
		return 0; /* Nothing to do, RAC is not currently on */

	old = v; /* save old state */

	/* Disable ALL cpus RAC */
	rdb_write(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG, 0);
	BARRIER();

	/* flush */
	rdb_write(BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG,
			BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG_FLUSH_RAC_MASK);
	BARRIER();

	do {
		dmb();
		v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG);
	} while (v & BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG_FLUSH_RAC_MASK);

	return old;
}


void rac_enable(uint32_t rac_cfg0)
{
	if (0 == rac_cfg0) {
		if (debug())
			msg_cpu("RAC_EN_ZERO", get_cpu_idx(get_mpidr()));
		return; /* Don't turn RAC off, use disable_and_flush instead */
	}

	rdb_write(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG, rac_cfg0);
	BARRIER();
}


int rac_is_enabled(void)
{
	uint32_t v;

	v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG);
	return (v != 0);
}


/*******************************************************************
 * Function: icache_enable
 * Description: enable/disable instruction cache
 * Arguments:  yes - 1: enable, 0: disable
 * Returns: none
 *******************************************************************/
void icache_enable(int yes)
{
	unsigned long sctlr = get_sys_ctl_reg();

	if (yes)
		sctlr |= SCTLR_ICACHE_ENABLE;
	else
		sctlr &= ~SCTLR_ICACHE_ENABLE;

	set_sys_ctl_reg(sctlr);
}


unsigned long disable_all_caches_and_mmu(void)
{
	unsigned long sctlr_new, sctlr = get_sys_ctl_reg();

	sctlr_new = sctlr & ~(SCTLR_MMU_ENABLE |
				SCTLR_DUCACHE_ENABLE |
				SCTLR_ICACHE_ENABLE);

	set_sys_ctl_reg(sctlr_new);

	return sctlr;
}



/*******************************************************************
 * Function: invalidate_icache
 * Description: invalidate all instruction lines in
 * L1 instruction cache.
 * Arguments: none
 * Returns: none
 *******************************************************************/
void invalidate_icache(void)
{
	uint32_t saved = rac_disable_and_flush();

	/* IC IALLUIS: Invalidate all instruction caches Inner Shareable
	 * to PoU. If branch predictors are architecturally
	 * visible, also flush branch predictors.
	 */
	__asm__ __volatile__("mcr p15, 0, %0, c7, c1, 0\n" : : "r" (0));
	BARRIER();

	/* BP IALLIS: invalidate branch predictor Inner Shareable
	 * since i-cache lines are invalidated.
	 */
	__asm__ __volatile__("mcr p15, 0, %0, c7, c1, 6\n" : : "r" (0));

	BARRIER();

	if (saved)
		rac_enable(saved);
}

/*******************************************************************
 * Function: flush_cache
 * Description: perform cache maintenance
 * Arguments: level - cache level to perform maintenance on
 *            do_clean - 1: clean+invalidate, 0: invalidate only
 * Returns: none
 *******************************************************************/
static inline void _flush_cache(int level, int do_clean)
{
	const uint32_t id = get_ccsidr(level, 0);
	const int linesz = CACHE_LINE_SIZE(id);
	const unsigned int ways = CACHE_NUM_WAYS(id);
	const unsigned int way_bits = fls(ways) - 1;
	const unsigned int sets = CACHE_NUM_SETS(id);

	unsigned int sl, way;

	for (way = 0; way < ways; way++) {
		const unsigned int max_sl = sets * linesz;

		for (sl = 0; sl < max_sl; sl += linesz) {
			const uint32_t arg = ((level - 1) << 1) | sl;

			if (do_clean)
				do_dccisw(arg, way, way_bits);
			else
				do_dcisw(arg, way, way_bits);
		}
	}

	BARRIER();
}

static inline void _dcache_op(int to_cache_level, int op)
{
	uint32_t saved = rac_disable_and_flush();

	dmb()
	_flush_cache(1, op); /* L1 d-cache */

	if (to_cache_level >= UPTO_POU)
		_flush_cache(2, op); /* L2 cache */

	if (saved)
		rac_enable(saved);
}

/*******************************************************************
 * Function: clean_invalidate_dcache
 * Description: clean then invalidate all data
 * cache lines, first on L1 D-cache, then optionally on L2 cache
 * Arguments: to_cache_level 0=L1 1=L1+L2
 * Returns: none
 *******************************************************************/
void clean_invalidate_dcache(int to_cache_level)
{
	_dcache_op(to_cache_level, 1);
}

/*******************************************************************
 * Function: invalidate_dcache
 * Description: invalidate all data
 * cache lines, first on L1 D-cache, then optionally on L2 cache
 * Arguments: to_cache_level 0=L1 1=L1+L2
 * Returns: none
 *******************************************************************/
void invalidate_dcache(int to_cache_level)
{
	_dcache_op(to_cache_level, 0);
}


void armv7_dump_registers(struct arm_regs *regs)
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
		"mpidr   : ",
	};

	__puts("\nPSCI exception: ");

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
	case MODE_USR:
		__puts("USR\n");
		break;
	case MODE_SYS:
		__puts("SYS\n");
		break;
	case MODE_MON:
		__puts("MON\n");
		break;
	case MODE_SVC:
		__puts("SVC\n");
		break;
	default:
		__puts("unknown\n");
		break;
	}

	__puts(" ");
	__puts( (cpsr & CPSR_FIQ) ? "F": "f");
	__puts( (cpsr & CPSR_IRQ) ? "I": "i");
	__puts( (cpsr & CPSR_ASYNC) ? "A": "a");
	__puts( (cpsr & CPSR_ENDIAN) ? "E" : "e");
	__puts( (cpsr & CPSR_Q) ? "Q" : "q");
	__puts( (cpsr & CPSR_V) ? "V" : "v");
	__puts( (cpsr & CPSR_C) ? "C" : "c");
	__puts( (cpsr & CPSR_Z) ? "Z" : "z");
	__puts( (cpsr & CPSR_N) ? "N" : "n");
	puts("");

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

	__puts(names[20]);
	writehex(get_mpidr());
	puts("\n");
}
