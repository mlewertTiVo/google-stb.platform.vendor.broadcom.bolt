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
#include <psci64.h>
#include <psci.h>

#include "bchp_sun_top_ctrl.h"
#include "bchp_hif_cpubiuctrl.h"


/* Log2(Number of bytes in cache line) - 4 */
#define CACHE_LINE_SIZE(_c)	(4 << (((_c) & CCSIDR_EL1_LineSize) + 2))

/* Associativity of cache - 1 */
#define CACHE_NUM_WAYS(_c)	((((_c) & CCSIDR_EL1_Associativity) >> \
					CCSIDR_EL1_Associativity_SHIFT) + 1)
/* Number of sets in cache - 1 */
#define CACHE_NUM_SETS(_c)	((((_c) & CCSIDR_EL1_NumSets) >> \
					CCSIDR_EL1_NumSets_SHIFT) + 1)

#define MAX_CLIDR_CTYPES (fls(CLIDR_EL1_CTYPES) / CLIDR_EL1_CTYPES_SHIFT)


/* Disable -> flush -> enable RAC before:
 * DC ISW
 * DC CSW
 * DC CISW
 * IC IALLUIS
 * IC IALLU
 */
static void __rac_flush(void)
{
	uint32_t v;

	/* tell the RAC to flush */
	rdb_write(BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG,
			BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG_FLUSH_RAC_MASK);
	BARRIER64();

	/* wait for the flag to clear (flush done) */
	do {
		dmb64();
		v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG);
	} while (v & BCHP_HIF_CPUBIUCTRL_RAC_FLUSH_REG_FLUSH_RAC_MASK);
}

static void __rac_enable(uint32_t rac_cfg0)
{
	rdb_write(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG, rac_cfg0);
	BARRIER64();
}


void rac_flush(void)
{
	if (rac_master() && rac_is_enabled())
		__rac_flush();
}


int rac_enable(uint32_t rac_cfg0)
{
	if (!rac_master())
		return PSCI_ERR_DENIED;

	__rac_enable(rac_cfg0);

	return PSCI_SUCCESS;
}


uint32_t rac_disable_and_flush(void)
{
	uint32_t v;

	if (!rac_master())
		return 0;

	v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG);
	if (v) {
		__rac_enable(0);
		__rac_flush();
	}

	return v;
}


int rac_is_enabled(void)
{
	uint32_t v;

	dmb64();
	v = rdb_read(BCHP_HIF_CPUBIUCTRL_RAC_CONFIG0_REG);
	return (v != 0);
}


void icache_enable(int yes)
{
	uint64_t sctlr;

	get_sctlr_el3(sctlr);
	isb64();

	if (yes)
		sctlr |= SCTLR_EL2_I;
	else
		sctlr &= ~(SCTLR_EL2_I);

	set_sctlr_el3(sctlr);
}

unsigned long disable_all_caches_and_mmu(void)
{
	unsigned long sctlr_new, sctlr;
	
	get_sctlr_el3(sctlr);
	isb64();

	sctlr_new = sctlr & ~(SCTLR_EL2_M | SCTLR_EL2_C | SCTLR_EL2_I);

	set_sctlr_el3(sctlr_new);

	return sctlr;
}


void invalidate_icache(void)
{
	dsb64();
	rac_flush();
	ic_ialluis(); /* Instruction cache inval to PoU, Inner Sharable */
	BARRIER64();
}


/* level: 0 = L1, 1 = L2 etc. */
static inline uint32_t get_ccsidr(unsigned int level, int icache)
{
	uint32_t csselr = 0, ccsidr = 0;

	csselr = (level << CSSELR_EL1_Level_SHIFT) |
		((icache) ? CSSELR_EL1_InD_ICACHE : CSSELR_EL1_InD_DCACHE);

	set_csselr_el1(csselr);
	isb64();
	get_ccsidr_el1(ccsidr);

	return ccsidr;
}

/* Set & Way (associativity) format for cache ops.
 *
 * DC op: 31[WAY(31:32-A) SET(B-1:L]) LEVEL(3:1) RES0(0)]0
 *
 * A = Log2(ASSOCIATIVITY)
 * L = Log2(LINELEN)
 * S = Log2(NSETS)
 * B = (L + S)
 *
 * LEVEL: 0=L1, 1=L2 etc.
 *
 * E.g. For L1
 *  ccsidr: 700fe01a
 *  linesz: 00000040   64
 *    ways: 00000004    4
 *    sets: 00000080  128
 *
 *   wbits: 00000002    2
 *   lbits: 00000006    6
 *
 * A = Log2(4)   = 2
 * L = Log2(64)  = 6
 * S = Log2(128) = 7
 * B = (L + S)   = 13
 *
 * DC op: 31[ WAY(31:30) SET(12:6) LEVEL(3:1) (0)]0
 *
 * DC op: WW______.________.___sssss.ss__LLL0
 *
 * W = 2 bits  = 0..3 (4 values)
 * S = 7 bits  = 0..127 (128 values)
 *
 */
static inline void flush_cache(int level, int clean)
{
	const uint32_t id = get_ccsidr(level, 0);
	const unsigned int linesz = CACHE_LINE_SIZE(id);
	const unsigned int ways = CACHE_NUM_WAYS(id);
	const unsigned int sets = CACHE_NUM_SETS(id);
	const unsigned int wbits = fls(ways) - 1;
	const unsigned int lbits = fls(linesz) - 1;
	uint64_t set, way;

	for (way = 0; way < ways; way++) {
		const uint64_t shway = way << (32 - wbits);

		for (set = 0; set < sets; set++) {
			const uint64_t shset = (set << lbits);
			const uint64_t wsl = shway | shset | (level << 1);
			if (clean)
				set_dccisw(wsl); /* Clean & Invalidate */
			else
				set_dcisw(wsl); /* Invalidate */
		}
	}
	BARRIER64();
}


/* to_cache_level: 1 = L1, 2 = L2 etc. */
static inline void _dcache_op_all(int to_cache_level, int clean)
{
	uint64_t clidr, n;
	int level;

	/* Nothing less than L1 cache */
	if (1 > to_cache_level)
		return;

	 /* Hard limit CtypeN from 1 to n (n=7 on A53) */
	if (to_cache_level > MAX_CLIDR_CTYPES)
		to_cache_level = MAX_CLIDR_CTYPES;

	get_clidr_el1(clidr);
	isb64();

	/* level: 0 = L1, 1 = L2 etc. */
	for (level = 0; level < to_cache_level; level++) {

		n = (clidr >> (level * CLIDR_EL1_CTYPES_SHIFT))
				& CLIDR_EL1_CTYPES_MASK;
		switch (n) {
		case CLIDR_EL1_CTYPE_NOCACHE: /* End of cache levels. */
			return;

		case CLIDR_EL1_CTYPE_DC_ONLY:
		case CLIDR_EL1_CTYPE_SEPARATE_ID:
		case CLIDR_EL1_CTYPE_UINFIED_ID:
			flush_cache(level, clean);
			break;

		default: /* Instruction cache only or reserved */
			break;
		}
	}
}


void clean_invalidate_dcache(int to_cache_level)
{
	dmb64();
	rac_flush();
	_dcache_op_all(to_cache_level, 1 /* clean + invalidate */);
}


void invalidate_dcache(int to_cache_level)
{
	dmb64();
	rac_flush();
	_dcache_op_all(to_cache_level, 0);
}
