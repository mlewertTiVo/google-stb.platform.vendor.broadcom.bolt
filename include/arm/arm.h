/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARM_H__
#define __ARM_H__

#include <bitops.h>

/*
 * CRn == c0
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define MIDR(reg)                       "p15,    0,    " reg ",      c0,     c0,     0"	// Main ID Register
#define MIDR_IMPLEMENTER_SHIFT          24
#define MIDR_IMPLEMENTER_MASK           (0xFF << MIDR_IMPLEMENTER_SHIFT)
#define MIDR_VARIANT_SHIFT              20
#define MIDR_VARIANT_MASK               (0xF << MIDR_VARIANT_SHIFT)
#define MIDR_ARCH_SHIFT                 16
#define MIDR_ARCH_MASK                  (0xF << MIDR_ARCH_SHIFT)
#define MIDR_PARTNUM_SHIFT              4
#define MIDR_PARTNUM_MASK               (0xFFF << MIDR_PARTNUM_SHIFT)
#define MIDR_REVISION_SHIFT             0
#define MIDR_REVISION_MASK              (0xF << MIDR_REVISION_SHIFT)

#define CTR(reg)                        "p15,    0,    " reg ",      c0,     c0,     1"	// Cache Type Register

#define CCSIDR(reg)                     "p15,    1,    " reg ",      c0,     c0,     0"	// Cache Size ID Register, RO
#define CCSIDR_NumSets_MASK             0x0FFFE000	// CCSIDR.bit[27:13] - the (number of sets in cache) - 1
#define CCSIDR_NumSets_SHIFT            13
#define CCSIDR_Associativity_MASK       0x00001FF8	// CCSIDR.bit[12:3] - the (associativity of cache) - 1
#define CCSIDR_Associativity_SHIFT      3
#define CCSIDR_LineSize_MASK            0x00000007	// CCSIDR.bit[3:0] - the the (log2 (number of words in cache line)) - 2

#define CSSELR(reg)                     "p15,    2,    " reg ",      c0,     c0,     0"	// Cache Size Selection Register, RW
#define CSSELR_InD_MASK                 0x00000001	// CSSELR.bit[0] - Instruction, Data or unified cache
#define CSSELR_InD_DataOrUnifiedCache   0x00000000	// Data or unified cache
#define CSSELR_InD_InstructionCache     0x00000001	// Instruction cache
#define CSSELR_Level_MASK               0x0000000e	// CSSELR.bit[3:1] - Cache level of required cache
#define CSSELR_Level_1                  0x00000000	// Level 1
#define CSSELR_Level_2                  0x00000002	// Level 2

#define MPIDR(reg)                      "p15,    0,    " reg ",      c0,     c0,     5"	// Multiprocessor Affinity Register, RO
#define MPIDR_Aff2_MASK                 0x00ff0000
#define MPIDR_Aff1_MASK                 0x0000ff00
#define MPIDR_Aff0_MASK                 0x000000ff

#define ID_PFR0(reg)                    "p15,    0,    " reg ",      c0,     c1,     0"	// Processor Feature Register 0
#define ID_PFR1(reg)                    "p15,    0,    " reg ",      c0,     c1,     1"	// Processor Feature Register 1

#define ID_MMFR0(reg)                   "p15,    0,    " reg ",      c0,     c1,     4"	// Memory Model Feature Register 0

#define ID_ISAR0(reg)                   "p15,    0,    " reg ",      c0,     c2,     0"	// Instruction Set Attribute Register 0

#define CLIDR(reg)                      "p15,    1,    " reg ",      c0,     c0,     1"	// Cache Level ID Register, RO

#define CTR(reg)                        "p15,    0,    " reg ",      c0,     c0,     1"	// Cache Type Register, RO

/*
 * CRn == c1
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define SCTLR(reg)                      "p15,    0,      "reg",        c1,     c0,     0"	// System Control Register
#define SCTLR_ICACHE_CTRL_MASK          0x00001000
#define SCTLR_ICACHE_ENABLE             0x00001000	// SCTLR.bit[12] - Instruction caches enabled
#define SCTLR_ICACHE_DISABLE            0xFFFFEFFF	// SCTLR.bit[12] - Instruction caches disabled (reset value)

#define SCTLR_BRANCH_PRED_CTRL_MASK     0x00000800
#define SCTLR_BRANCH_PRED_ENABLE        0x00000800	// SCTLR.bit[11] - Branch prediction enable
#define SCTLR_BRANCH_PRED_DISABLE       0xFFFFF7FF	// SCTLR.bit[11] - Branch prediction disable (reset value)

#define SCTLR_DUCACHE_CTRL_MASK         0x00000004
#define SCTLR_DUCACHE_ENABLE            0x00000004	// SCTLR.bit[2] - Data and unified caches enabled
#define SCTLR_DUCACHE_DISABLE           0xFFFFFFFB	// SCTLR.bit[2] - Data and unified caches disabled (reset value)

#define SCTLR_MMU_CTRL_MASK             0x00000001
#define SCTLR_MMU_ENABLE                0x00000001	// SCTLR.bit[0] - PL1 and PL0 stage 1 MMU enabled
#define SCTLR_MMU_DISABLE               0xFFFFFFFE	// SCTLR.bit[0] - PL1 and PL0 stage 1 MMU disabled (reset value)

#define HCR(reg)                        "p15,    4,    " reg ",      c1,     c1,     0"	// Hyp Configuration Register, Virtualization Extensions
#define HCR_VM_CTRL_MASK                0x00000001
#define HCR_VM_ENABLE                   0x00000001	// HCR.bit[0] - PL1 and PL0 stage 2 MMU enabled
#define HCR_VM_DISABLE                  0xFFFFFFFE	// HCR.bit[0] - PL1 and PL0 stage 2 MMU disabled (reset value)

#define HSCTLR(reg)                     "p15,    4,    " reg ",      c1,     c0,     0"	// Hyp System Control Register, Virtualization Extensions

/* CPACR - Co-processor Access Control Register, VMSA */
#define CPACR(reg)                      "p15,    0,    " reg ",      c1,     c0,     2"
#define CPACR_CP(x,setting)             (((setting) & 0x3) << ((x) * 2))
#define CPACR_PL0                       (0x1)
#define CPACR_PL1                       (0x2)

/*
 * CRn == c2
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define TTBR0(reg)                      "p15,    0,   " reg ",      c2,     c0,     0"	// Translation Table Base Register 0
#define TTBR1(reg)                      "p15,    0,   " reg ",      c2,     c0,     1"	// Translation Table Base Register 1

#define TTBCR(reg)                      "p15,    0,   " reg ",      c2,     c0,     2"	// TTBCR, Translation Table Base Control Register, VMSA
#define TTBCR_EAE_MASK                  0x80000000
#define TTBCR_ExtendedAddressDisable    0x7FFFFFFF	// TTBCR.EAE = 0, use 32-bit translation system, with the Short-descriptor translation table format
#define TTBCR_ExtendedAddressEnable     0x80000000	// TTBCR.EAE = 1, use 40-bit translation system, with the Long-descriptor translation table format
#define TTBCR_PD1_MASK                  0x00000020
#define TTBCR_PD0_MASK                  0x00000010

/*
 * CRn == c3
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define DACR(reg)                       "p15,    0,    " reg ",      c3,     c0,     0"	// Domain Access Control Register

/*
 * CRn == c7
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define ICIALLUIS(reg)                  "p15,    0,    " reg ",      c7,     c1,     0"	// Invalidate all instruction caches to PoU Inner Shareable
#define BPIALLIS(reg)                   "p15,    0,    " reg ",      c7,     c1,     6"	// Invalidate all branch predictors Inner Shareable

#define ICIALLU(reg)                    "p15,    0,    " reg ",      c7,     c5,     0"	// Invalidate all instruction caches to PoU
#define ICIMVAU(reg)                    "p15,    0,    " reg ",      c7,     c5,     1"	// Invalidate instruction caches by MVA to PoU
#define CP15ISB(reg)                    "p15,    0,    " reg ",      c7,     c5,     4"	// Instruction Synchronization Barrier operation
#define BPIALL(reg)                     "p15,    0,    " reg ",      c7,     c5,     6"	// Invalidate all branch predictors
#define BPIMVA(reg)                     "p15,    0,    " reg ",      c7,     c5,     7"	// Invalidate MVA from branch predictors

/*
 * Data cache and unified cache operations
 */
/* invalidate */
#define DCISW(reg)                      "p15,    0,    " reg ",      c7,     c6,     2"	// Invalidate data cache line by set/way

/* invalidate to PoC*/
#define DCIMVAC(reg)                    "p15,    0,    " reg ",      c7,     c6,     1"	// Invalidate data cache lines of D-cache and unified caache by MVA to PoC

/* clean */
//#define   DCCSW(reg)

/* clean to PoC */
#define DCCMVAC(reg)                    "p15,    0,    " reg ",      c7,     c10,    1"

/* clean to PoU */
#define DCCMVAU(reg)                    "p15,    0,    " reg ",      c7,     c11,    1"

/* clean and invalidate to PoC */
#define DCCIMVAC(reg)                   "p15,    0,    " reg ",      c7,     c14,    1"	// Clean and invalidate data cache line by MVA to PoC

#define DCCISW(reg)                     "p15,    0,    " reg ",      c7,     c14,    2"	// Clean and invalidate data cache line by set/way

#define	L1_DCache_DCCISW_Level			1	// 1 for L1 D-cache
#define	L2_UCache_DCCISW_Level			2	// 2 for L2 cache

#define DCCISW_LEVEL_SHIFT              (1)

#define isb()	{ __asm__ __volatile__ ("isb" : : : "memory"); }
#define dsb()	{ __asm__ __volatile__ ("dsb" : : : "memory"); }
#define dmb()	{ __asm__ __volatile__ ("dmb" : : : "memory"); }

/*
 * Get Cache Size ID Register for a given cache level (L1, L2, ...)
 * NOTE: only available on ARMv7 or higher
 */
static inline uint32_t get_ccsidr(unsigned int level, int icache)
{
	unsigned long id, cache = ((level - 1) << 1) | (icache ? 0x1 : 0x0);

	__asm__ __volatile__(
			"	mcr p15, 2, %1, c0, c0, 0\n" /* CSSELR */
			"	isb\n"
			"	mrc p15, 1, %0, c0, c0, 0\n" /* CSSIDR */
			: "=r" (id)
			: "r" (cache));
	return id;
}
#define CACHE_LINE_SIZE(ccsidr)	(4 << (((ccsidr) & 0x7) + 2))
#define CACHE_NUM_WAYS(ccsidr)	((((ccsidr) >> 3) & 0x3f) + 1)
#define CACHE_NUM_SETS(ccsidr)	((((ccsidr) >> 13) & 0x7fff) + 1)

#define get_l1_dcache_line_size()	CACHE_LINE_SIZE(get_ccsidr(1, 0))
#define get_l2_cache_line_size()	CACHE_LINE_SIZE(get_ccsidr(2, 0))

static inline void do_dcisw(uint32_t arg, unsigned int way, int way_bits)
{
	arg |= way << (32 - way_bits);

	__asm__ __volatile__(
	"	mcr	p15, 0, %0, c7, c6, 2\n"
	: /* no outputs */
	: "r" (arg)
	: "memory");
}

static inline void do_dccisw(uint32_t arg, unsigned int way, int way_bits)
{
	arg |= way << (32 - way_bits);

	__asm__ __volatile__(
	"	mcr	p15, 0, %0, c7, c14, 2\n"
	: /* no outputs */
	: "r" (arg)
	: "memory");
}

static inline void tlb_flush(void)
{
	const int tmp = 0;

	__asm__ __volatile__(
		"	mcr	p15, 0, %0, c8, c7, 0\n" /* TLBIALL */
		"	mcr	p15, 0, %0, c8, c5, 0\n" /* ITLBIALL */
		"	mcr	p15, 0, %0, c7, c5, 6\n" /* BPIALL */
		"	dsb\n"
		"	isb\n"
			: : "r" (tmp));
}

/*
 * CRn == c9
 */
//                                      p15,    op1,    dest reg,   CRn,    CRm,    op2
#define L2CTLR(reg)			"p15,    1,    " reg ",      c9,     c0,     2"	// L2 Control Register, RO


/*
 * MMU (with only Short-descriptor translation table format)
 */

enum {
	PTE_FAULT = 0,
	PTE_TABLE, /* pointer to 2nd level table */
	PTE_SECTION, /* 1MB in 1st level */
	PTE_SS, /* super section, 16MB in 1st level */
	PTE_LARGE, /* 64KB in 2nd level */
	PTE_SMALL, /* 4KB in 2nd level */
};

#define PT_ENTRY_SIZE		4
#define PT1_ENTRIES		4096
#define PT1_SIZE		(PT1_ENTRIES * PT_ENTRY_SIZE)
#define PT2_ENTRIES		((_MB(1) / SMALL_PG_SIZE))
#define PT2_SIZE		(PT2_ENTRIES * PT_ENTRY_SIZE)

/*
 * Section and Supersection
 */
#define SECTION_SHIFT		20
#define SECTION_SIZE		(1U << SECTION_SHIFT)
#define SECTION_MASK		(~(SECTION_SIZE-1))

#define SECTION 		BIT(1)
#define SUPER_SECTION 	(BIT(1) | BIT(18))
#define BUFFERABLE		BIT(2)
#define CACHABLE		BIT(3)
#define SHARABLE		BIT(16)
#define PRIV_RW			BIT(10)
#define TEX_2_0(n)		(((n) & 0x7) << 12)
#define XN			BIT(4)

/*
 * Small page
 */
#define SMALL_PG_SIZE		(4096)
#define SMALL_PG_TYPE		BIT(1)
#define SMALL_PG_PA_SHIFT	12
#define SMALL_PG_PA_MASK	((~0) << SMALL_PG_PA_SHIFT)
#define SMALL_PG_BUFFERABLE	BIT(2)
#define SMALL_PG_CACHABLE	BIT(3)
#define SMALL_PG_SHARABLE	BIT(10)
#define SMALL_PG_PRIV_RW	BIT(4)
#define SMALL_PG_TEX_2_0(n)	(((n) & 0x7) << 6)
#define SMALL_PG_XN		BIT(0)

/*
 * from the 1st level page table to a 2nd level page table that maps 1MB of VA
 */
#define PT_BASEADDR_SHIFT	10
#define PT_BASEADDR_MASK	((~0) << PT_BASEADDR_SHIFT)
#define PT_TYPE		BIT(0)

/*
 * - For an explanation behind the following bits, see ARM ARM B3.8.2
 * (Short-descriptor format memory region attributes without TEX remap)
 *
 * - Cortex-A15 errata, rev. 3, #801819 (fixed in r3p3):
 * This erratum manifests itself as an L1 D-cache stall during an L2 cache
 * snoop in SMP. The issue occurs with the Write-Back No-Allocate memory
 * region attribute (TEX=0, C=1, B=1). The errata also suggests that Write-Back
 * No-Allocate is rarely used. Although the issue will not occur since BOLT
 * runs on a single core, we have intentionally avoided defining the affected
 * attribute below.
 */
#define SECT_STRONGLY_ORDERED	(SECTION | PRIV_RW | SHARABLE)
#define SECT_DEVICE		(SECTION | PRIV_RW | SHARABLE | BUFFERABLE)
#define SECT_MEM_NONCACHED	(SECTION | PRIV_RW | SHARABLE | TEX_2_0(1))
#define SECT_MEM_CACHED_WT	(SECTION | PRIV_RW | SHARABLE | CACHABLE)
#define SECT_MEM_CACHED_WB	\
	(SECTION | PRIV_RW | SHARABLE | TEX_2_0(1) | CACHABLE | BUFFERABLE)

#define SMALL_PG_CMN		(SMALL_PG_TYPE | SMALL_PG_PRIV_RW | \
				 SMALL_PG_SHARABLE)
#define SMALL_PG_STRONGLY_ORDERED	(SMALL_PG_CMN)
#define SMALL_PG_DEVICE		(SMALL_PG_CMN | SMALL_PG_BUFFERABLE)
#define SMALL_PG_MEM_NONCACHED	(SMALL_PG_CMN | SMALL_PG_TEX_2_0(1))
#define SMALL_PG_MEM_CACHED_WT	(SMALL_PG_CMN | SMALL_PG_CACHABLE)
#define SMALL_PG_MEM_CACHED_WB	(SMALL_PG_CMN | SMALL_PG_TEX_2_0(1) | \
				 SMALL_PG_CACHABLE | SMALL_PG_BUFFERABLE)

/* for uncached malloc, if enabled.
*/
#define UC_SIZE 		(SECTION_SIZE*4)
#define UC_ALIGN		SECTION_SIZE

#ifdef ARM_V7
 #define REG_ADDR(x)	BPHYSADDR(x)
 #define PHYSADDR(x)	(x)
 #define CACADDR(x) 	(x)
 #define UNCADDR(x) 	(x)
 #define BVIRTADDR(x)	BPHYSADDR(x)
#else
 #error missing ARM arch version!
#endif /* ARM_V7 */

#define BARRIER()	do { dsb(); isb(); } while (0)

struct arm_regs {
	union {
		uint32_t r[16];
		struct {
			uint32_t r0;
			uint32_t r1;
			uint32_t r2;
			uint32_t r3;
			uint32_t r4;
			uint32_t r5;
			uint32_t r6;
			uint32_t r7;
			uint32_t r8;
			uint32_t r9;
			uint32_t r10;
			uint32_t r11;
			uint32_t r12;
			uint32_t r13_sp;
			uint32_t r14_lr;
			uint32_t r15_pc;
		} reg;
	} gprs;
	uint32_t cpsr;
	uint32_t dfsr;
	uint32_t dfar;
	uint32_t ifar;
};

/*
 * Read-ahead Cache
 */
enum {
	RAC_INST_SHIFT = 0,
	RAC_DATA_SHIFT = 4,
	RAC_DISABLED = 0,
	RAC_ACC_64B_INITIAL = 2,
	RAC_ACC_256B_INITIAL = 3,
	RAC_NO_PREF = 0,
	RAC_PREF_256B_AFTER_2 = 1,
	RAC_PREF_256B_AFTER_3 = 2,
	RAC_PREF_256B_AFTER_4 = 3,
};

#define RAC_CFG(cpu, insn, data) \
	((((data) << 4) | (insn)) << ((cpu) * 8))

#define RAC_SETTING(ena, pref) \
	(((ena) << 2) | (pref))

/*
 * Externs
 */
extern void i_cache_config(int i_cache_en);
extern void flush_cache(int level, int do_clean);
#endif	/* __ARM_H__ */
