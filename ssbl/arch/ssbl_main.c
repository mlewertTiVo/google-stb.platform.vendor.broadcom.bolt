/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "supplement-fsbl.h"

#include <arch.h>
#include <arch-mmu.h>
#include <stdbool.h>
#include <arm-start.h>
#include <board.h>
#include <bolt.h>
#include <bsp_config.h>
#include <common.h>
#include <ddr.h>
#include <hwuart.h>
#include <initdata.h>
#include <lib_types.h>
#include "mmap-dram.h" /* gen/${family}/ */
#include "ssbl-sec.h"

/* The stack and heap must be SECTION (1MiB) aligned and
sized in multiples of it for us to reliably mark *only*
those areas and therefore prevent any overlap with something else.
*/
#if defined(ARM_V7) && (CFG_ALIGN_SIZE != SECTION_SIZE)
#error Cannot mark heap & stack as XN due to ALIGN_SIZE != SECTION_SIZE
#endif

#if CFG_STACK_PROTECT_SSBL
uintptr_t __stack_chk_guard = SSBL_STACK_CHECK_VAL;

void __attribute__((noreturn)) __stack_chk_fail(void)
{
	unsigned long *sp = (unsigned long *)__getstack();

	puts("stack_chk_fail ");
	writehex(__stack_chk_guard);
	__puts(" STACK @ ");
	writehex((unsigned long)sp);
	__puts(" (");
	writehex(*(unsigned long *)sp);
	puts(")");
	while (1)
		;
}
#endif

struct fsbl_info *board_info(void)
{
	return _fsbl_info;
}

static int putchar(int c)
{
	if ((_fsbl_info) && (_fsbl_info->uart_base)) {
		while (!(DEV_RD(_fsbl_info->uart_base + LSR_REG) & LSR_THRE))
			;
		DEV_WR(_fsbl_info->uart_base + THR_REG, c);
	}
	return 0;
}


void __puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			putchar('\r');
		putchar(*(s++));
	}
}


int puts(const char *s)
{
	__puts(s);
	putchar('\r');
	putchar('\n');
	return 0;
}


void __noisy_flush_caches(void)
{
	__puts("CACHE FLUSH ");
	clear_all_d_cache();
	invalidate_all_i_cache();
	puts("OK");
}

void writehex(uint32_t val)
{
	unsigned int i, c;

	for (i = 0; i < 8; i++, val <<= 4) {
		c = (val >> 28) + '0';
		if (c > '9')
			c += 'a' - '9' - 1;
		putchar(c);
	}
}

/* get_pagetable_location -- returns the location of the 1st level PT
 *
 * It is not guaranteed that the location of 1st level page table is
 * identical to FSBL and SSBL if they use statically defined values.
 * The consequence of using static values in SSBL will be huge when
 * they are separately built and combined later.
 *
 * SSBL should use the location of the 1st level PT that FSBL passes.
 *
 * Returns:
 *  location of 1st level PT that FSBL did set and passes
 */
static uint32_t get_pagetable_location(void)
{
	struct fsbl_info *fsbl;
	uint32_t pt1; /* 1st level page table */

	fsbl = board_info();
	if (fsbl == NULL) {
		/* should not happen, just for safety */
		puts("board_info() failed!");
		return 0;
	}

	pt1 = fsbl->pte;
	if (pt1 == 0) {
		puts("Page Table cannot be at 0x0 (zero)!");
		/* should not happen, just for safety
		 *
		 * The reason for prohibiting PT at 0x0 is the guard page.
		 */
		return 0;
	}

	return pt1;
}

/* set_ssbl_page -- marks only the text section of SSBL executable
 *
 * A memory area can be either executable or non-executable at the unit
 * of a page size (4KiB or 1MiB with the current implementation) via
 * clearing/marking XN (eXecute Never) from/in corresponding page(s).
 *
 * Once SSBL starts, only the text section of SSBL should be executable.
 * It can be achieved by marking all other areas XN in page table.
 *
 * From linker script:
 *                 +-----------------------+ _end (end of SSBL)
 *                 |        ARM.exidx      |
 *                 +-----------------------+ _ebss (8B aligned)
 *                 |   bss and ssbl.bss    |
 *                 +-----------------------+ _edata/_fbss (8B aligned)
 *                 |         data          |
 *     0x????_?000 +-----------------------+ _fdata (4KiB aligned)
 *                 {   for 4KiB alignment  }
 *                 +-----------------------+ _etext
 *                 |    text (SSBL code)   |
 *                 +-----------------------+ _ftext (8B aligned)
 *                 |         vectors       |
 *     0x0700_8000 +-----------------------+ SSBL_RAM_ADDR (start of SSBL)
 *                 | 2nd level page tables |
 *                 | (guard, sram, ... )   |
 *     0x0700_4000 +-----------------------+ SSBL_PAGE_TABLE + 0x4000
 *                 | 1st level page table  |
 *     0x0700_0000 +-----------------------+ SSBL_PAGE_TABLE
 *                 |        board info     |
 *     0x06FF_C000 +-----------------------+ SSBL_BOARDINFO
 *                 |                       |
 *                 {         kernel        }
 *                 | (or, other loaded app)|
 *                 |                       |
 *     0x0000_8000 +-----------------------+
 *
 * (_fdata - (SSBL_RAM_ADDR & 0xfff0_0000)) should be 1MB or smaller because:
 * - marking excutable/non-executable is done at 4KiB granularity by
 *   constructing a 2nd level page table
 * - code will get too complicated for the very low chance of exceeding
 *   the current limit if 'big' SSBL text is allowed
 *
 * With the current memory layout, the limit for the SSBL text size is
 * (1MiB - 32KiB - vectors), almost 1MiB. The size of the SSBL text size
 * is currently approximately 200KiB. And, Before hitting the limit,
 * the 'total' size of SSBL has to increase by changing the memory layout.
 *
 * Parameter:
 *  pt_1st [in] pointer to the 1st level page table
 *  _fdata [in] end of SSBL text section in bytes, also start of data section
 */
static void set_ssbl_page(uint32_t *pt_1st, uint32_t _fdata)
{
	unsigned int i;
	uint32_t *pt_ssbl;
	uint32_t pa = SSBL_RAM_ADDR & ~(_MB(1) - 1); /* 1MiB alignment */

	if ((_fdata - pa) > _MB(1)) {
		writehex(_fdata - pa);
		puts(" Too big SSBL");
		/* early exit will eventually halt the system as soon as
		 * MMU gets enabled because all memory areas are marked XN
		 */
		return;
	}

	/*       |                       |
	 *       +-----------------------+ PT2B + 0x400
	 *       | 2nd level PT for SSBL |
	 *  PT2B +-----------------------+ PT2S + 0x400
	 *       | 2nd level PT for SRAM |
	 *  PT2S +-----------------------+ PT2G + 0x400
	 *       | 2nd level PT for guard|
	 *  PT2G +-----------------------+ PT1 + 0x4000
	 *       | 1st level page table  |
	 *  PT1  +-----------------------+ PT1
	 */
	pt_ssbl = (uint32_t *)(2 * SSBL_PAGE_TABLE_2_SIZE +
		SSBL_PAGE_TABLE_SIZE + (uint32_t)pt_1st);

	for (i = 0; i < PT2_ENTRIES; i++) {
		/* by default, non-executable */
#if !CFG_UNCACHED
		const uint32_t pte = pa | SMALL_PG_MEM_CACHED_WB | SMALL_PG_XN;
#else
		const uint32_t pte = pa | SMALL_PG_MEM_NONCACHED | SMALL_PG_XN;
#endif

		pt_ssbl[i] = pte;
		pa += SMALL_PG_SIZE;
	}

	/* set 1st-level pte to a page-table descriptor */
	pt_1st += SSBL_RAM_ADDR >> SECTION_SHIFT;
	*pt_1st = (uintptr_t)pt_ssbl | PT_TYPE;

	/* remove XN from code area */
	arch_mark_executable(SSBL_RAM_ADDR, _fdata-SSBL_RAM_ADDR, true);
}

/* construct_dram_pages -- builds up page table for installed DRAM
 *
 * FSBL sets up page table entries for memory mapped registers, SRAM,
 * EBI and small portion of DRAM (enough to copy SSBL into DRAM).
 *
 * Complete page table for DRAM so that it covers all of the installed
 * DRAM on the board.
 *
 * Parameter:
 *  pt1 [in] pointer to the 1st level page table
 */
static void construct_dram_pages(uint32_t *pt1)
{
	unsigned int i;
	struct board_type *b = board_thisboard();

	/* skip sanity check on pt1 as the caller should have done it */

	if (b == NULL) {
		puts("INVALID board info");
		return;
	}

	/* construct DRAM portion of PT, must be before setting guard page */
	for (i = 0; i < b->nddr; i++) {
		unsigned long size;
		const struct ddr_info *ddr = &(b->ddr[i]);

		size = ddr_get_restricted_size(ddr);
		if (size == 0)
			continue;

		set_pte_range(pt1, _MB(ddr->base_mb), _MB(ddr->base_mb) + size,
			/* make sure marking XN (eXecute Never) */
#if !CFG_UNCACHED
			SECT_MEM_CACHED_WB | XN
#else
			SECT_MEM_NONCACHED | XN
#endif
		);
	}

	/* after constructing PTE's for DRAM */
	set_guard_page(pt1);
}

#ifdef STUB64_START /* aarch64/smm(smc) & PSCI32 support */

/* Use the pre-processor to stringify the name
*/
#define _SMM_VAR(odir, modname) _binary_ ## odir ## _smm_64_bin_ ## modname
#define SMM_VAR(odir, modname) _SMM_VAR(odir, modname)

extern unsigned char SMM_VAR(ODIR, start);
extern unsigned char SMM_VAR(ODIR, size);

#define _PSCI32_VAR(odir, modname) _binary_ ## odir ## _psci32_bin_ ## modname
#define PSCI32_VAR(odir, modname) _PSCI32_VAR(odir, modname)

extern unsigned char PSCI32_VAR(ODIR, start);
extern unsigned char PSCI32_VAR(ODIR, size);

static void install_smm64(void)
{
	__puts("smm64@");

	memcpy((uint8_t *)PSCI_BASE,
		(uint8_t *)&SMM_VAR(ODIR, start),
		(uint32_t)&SMM_VAR(ODIR, size));
}

static void install_smm32(void)
{
	__puts("psci32@");

	memcpy((uint8_t *)PSCI_BASE,
		(uint8_t *)&PSCI32_VAR(ODIR, start),
		(uint32_t)&PSCI32_VAR(ODIR, size));
}

/*
 * SSBL code for Power on/cold boot.
 * If the VBAR_EL3/MVBAR value set by FSBL is different
 * to that which SSBL was made with then we have to
 * reprogram it to the SSBL value.
 *
 * This is for the case of an FSBL being
 * supplied in binary only form and it is not
 * upgradable. The only caveat is that nothing
 * (SSBL wise) must be at the old vector if the
 * strap was set for 64 bit boot and restore is
 * false.
 */
static void reprogram_psci_base(uint32_t oldvec, uint32_t newvec,
		bool is64boot, bool restore)
{
	__puts("MVBAR reprogram: ");
	writehex(oldvec);
	__puts(" -> ");

	if (is64boot) {
		const uint32_t a64_revec[4] = {
			0x92407c00,	/* and	x0, x0, #0xffffffff */
			0xd51ec000,	/* msr	vbar_el3, x0 */
			0xd5033fdf,	/* isb */
			0xd69f03e0	/* eret */
		};
		uint32_t saved[4];

		/* Offset for lower EL using aarch32 'Synchronous' entry */
		oldvec += 0x600;

		if (restore) {
			memcpy(saved, (uint32_t *)oldvec, sizeof(a64_revec));
			dsb();
		}

		/* Copy over our A64 code to the old vector */
		memcpy((uint32_t *)oldvec, a64_revec, sizeof(a64_revec));
		dsb();

		/* Call it with the new vector in x0 */
		(void)psci(newvec, 0, 0, 0);

		if (restore)
			memcpy((uint32_t *)oldvec, saved, sizeof(a64_revec));

	} else {
		/*
		 * Write MVBAR. EL1(S), EL3(NS) or EL3(S)
		 * Setup is valid only if EL3 is AArch32.
		 */
		__asm__ __volatile__(
			"mcr	p15, 0, %0, c12, c0, 1\n"
			"isb\n"
			: /* no outputs */
			: "r" (newvec)
			: "memory");
	}

	writehex(newvec);
	puts("");
}
#endif /* STUB64_START */


/* ssbl_main -- C entry point of SSBL, called from ssbl_init
 *
 * Parameters:
 *  _end   [in] end of SSBL in bytes
 *  _fbss  [in] start of BSS in bytes
 *  _ebss  [in] end of BSS in bytes
 *  _fdata [in] start of data section in bytes, and end of text section
 */
void ssbl_main(uint32_t _end, uint32_t _fbss, uint32_t _ebss, uint32_t _fdata)
{
	unsigned long sp;
	uint32_t *pt_1st;
	bool was_guard_page_correct;
#if (CFG_CMD_LEVEL >= 5)
	struct board_nvm_info *s = &(_fsbl_info->saved_board);
#endif

#ifdef SECURE_BOOT
	ssbl_main_sec();
#endif

	puts("SSBL");

	if (CFG_ZEUS4_2)
		sec_enable_debug_ports();

	puts("PINMUX");
	board_pinmux();

	__noisy_flush_caches();

	pt_1st = (uint32_t *)get_pagetable_location();
	/* check whether patch work is needed for older frozen FSBL
	 * before constructing PT for DRAM because such indication
	 * gets overridden once PT for DRAM is correctly constructed.
	 */
	was_guard_page_correct = supplement_fsbl_checkguardpage(pt_1st);
	construct_dram_pages(pt_1st);
	supplement_fsbl_pagetable(pt_1st, was_guard_page_correct);
	/* The whole DDR area has been marked XN */
#ifdef STUB64_START
	__puts("CLR PSCI MEM @ ");
	writehex(PSCI_BASE);
	memset((void *)PSCI_BASE, 0, PSCI_SIZE);
	puts(" OK");

	__puts("INSTALL ");
	if (arch_booted64())
		install_smm64();
	else
		install_smm32();

	writehex(PSCI_BASE);

	/* TBD: 2nd level 4KiB Table */
#ifndef SSBM_RAM_ADDR
	/* With SSBM, page table for PSCI is set after SSBL page is set up */
	arch_mark_executable(PSCI_BASE, SECTION_SIZE, true);
	arch_mark_uncached(PSCI_BASE,  SECTION_SIZE);
#endif
	puts(" OK");

	dsb(); /* make sure its there before we call it */

	/*
	 * Check what (a possibly older) FSBL passed in to us.
	 * No need to save the DDR used for this (if 64 bit bootstrap)
	 * as it is a new boot and not an e.g. S3 wakeup.
	 */
	if (_fsbl_info->psci_base != PSCI_BASE)
		reprogram_psci_base(_fsbl_info->psci_base, PSCI_BASE,
			arch_booted64(), false);
#endif
	arch_mark_executable(SRAM_ADDR, SRAM_LEN, false);
	set_ssbl_page(pt_1st, _fdata);

	__puts("MMU ");
	enable_caches();
	puts("ON");

	__noisy_flush_caches();

	mem_bottomofmem = (unsigned long)_fsbl_info;
	__puts("_fbss  "); writehex(_fbss); puts("");
	__puts("_ebss  "); writehex(_ebss); puts("");
	__puts("_end   "); writehex(_end);  puts("");

	__puts("HEAP @ ");
	mem_heapstart = ALIGN_UP_TO(_end, CFG_ALIGN_SIZE);
	writehex(mem_heapstart);
	puts("");

	__puts("STACK @ ");
	sp = mem_heapstart + (__STACK_SIZE + __HEAP_SIZE);
#if CFG_STACK_PROTECT_SSBL
	/* prime a fake previous guard value for bolt_main() in
	case -fstack-protector-all is used. */
	*((uint32_t *)(sp + sizeof(uint32_t))) = SSBL_STACK_CHECK_VAL;
#endif
	mem_topofmem = sp;
	writehex(sp);
	puts("");

	__puts("ARCH: CONFIG");
	arch_config();
	puts(" OK");

	__puts("CLR BSS ");
	writehex(_fbss);
	__puts(" to ");
	writehex(_ebss);
	memset((void *)_fbss, 0, _ebss - _fbss);
	puts(" OK");

	__puts("CLR SRAM ");
	memset((void *)SRAM_ADDR, 0, SRAM_LEN);
	puts(" OK");

#if (CFG_CMD_LEVEL >= 5)
	if (s)	{
		__puts("board_info  @ ");
		writehex((uint32_t)_fsbl_info); puts("");

		__puts("board_nvm   @ ");
		writehex((uint32_t)s); puts("");

		__puts("board_types @ ");
		writehex((uint32_t)&(_fsbl_info->board_types)); puts("");

		__puts("       magic "); writehex(s->magic); puts("");
		__puts("   board_idx "); writehex(s->board_idx); puts("");
		__puts("  product_id "); writehex(s->product_id); puts("");
		__puts("    romstart "); writehex(s->romstart); puts("");
		__puts("   romoffset "); writehex(s->romoffset); puts("");
		__puts("ddr_override "); writehex(s->ddr_override); puts("");
		__puts("   hardflags "); writehex(s->hardflags); puts("");
	} else {
		puts(" NO board_nvm_info!  magic ");
	}
#endif

	__noisy_flush_caches();

	/* We may be operating at another (lower, via FSBL) speed
	so set up the default. NB: If this is done after runtime
	devicetree poulation then it will confuse Linux because
	the DT will say one thing (as it read the clkrate at a
	specific moment) and we are doing another thing (going
	faster)	later on. */
	__puts("CPU CLKSET");
	arch_set_cpu_clk_ratio(CPU_CLK_RATIO_ONE);
	puts(" OK");

	puts("GO!");
	
	/* SWBOLT-1757: The compiler may choose to put its
	 * parameters on the stack so we set SP last, just
	 * before we call ssbl_main() or else _fbss, _ebss
	 * etc. disappear.
	 */
	arch_call_bolt_main(sp);
}
