/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "supplement-fsbl.h"

#include <arch.h>
#include <arch-mmu.h>
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
#if (CFG_CMD_LEVEL >= 5)
	struct board_nvm_info *s = &(_fsbl_info->saved_board);
#endif
	puts("SSBL");

	puts("PINMUX");
	board_pinmux();

	__noisy_flush_caches();

	__puts("MMU ");
	pt_1st = (uint32_t *)get_pagetable_location();
	supplement_fsbl_pagetable(pt_1st);
	/* The whole DDR area was marked XN by FSBL, or assured by
	 * supplement_fsbl_pagetable().
	 */
	arch_mark_executable(SRAM_ADDR, SRAM_LEN, false);
	set_ssbl_page(pt_1st, _fdata);
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
	__setstack(sp);
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
	bolt_main(0, 0);
}
