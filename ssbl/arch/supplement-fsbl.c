/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* FSBL (First Stage BootLoader) and SSBL (Second Stage BootLoader) are
 * two main parts of BOLT. As the names imply, SSBL starts before FSBL
 * does. FSBL also sets up many critical data structures and passes them
 * to SSBL. If required, SSBL adjusts them before progressing further.
 *
 * Due to such dependency, FSBL and SSBL should be closely coupled. Any
 * change in FSBL should match what SSBL expects, and vice versa.
 *
 * Taking a BOLT release guarantees matching FSBL and SSBL. But, there
 * are cases that FSBL cannot change even when a newer BOLT release needs
 * be taken. The newer BOLT release will come with a newer SSBL, which
 * might be imcompatible with the fixed/frozen FSBL.
 *
 * Routines here will supplement fixed/frozen FSBL so that new features
 * of a newer BOLT release can be taken.
 */

#include "supplement-fsbl.h"

#include <arch-mmu.h>
#include <arch_ops.h>
#include <arm.h>
#include <arm-start.h>
#include <board.h>
#include <common.h>
#include <ddr.h>

#include <stddef.h>
#include <stdint.h>

/* construct_dram_pages -- builds up page table for installed DRAM
 *
 * Please note this should be called before setting up the guard page
 * because this routine overrides anything on the 1st level page table
 * with the size of 1MB, SECTION, if the corresponding area is DRAM.
 *
 * Parameter:
 *  pt1 [in] pointer to the 1st level page table
 */
static void construct_dram_pages(uint32_t *pt1)
{
	unsigned int i;
	struct board_type *b = board_thisboard();

	if (b == NULL)
		return;

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
}

/* assure_guard_page -- makes sure that the guard page is properly set up
 *
 * With an older FSBL, the guard page was mistakenly created and linked
 * to an incorrect location of 1st level page table, 0xC000_0000. This
 * routine checks the indication of such improperly constructed guard
 * page. If an improper guard page is detected, page table entries for
 * installed DRAM are re-constructed first, and then the guard page
 * gets fixed.
 *
 * Parameter:
 *  pt1 [in] pointer to the 1st level page table
 */
static void assure_guard_page(uint32_t *pt1)
{
	/* yes, 0xC0000000 is a magic number */
	const unsigned int magic_offset = 0xC0000000 >> SECTION_SHIFT;
	uint32_t pte;
	int ptetype;

	/* skip sanity check on pt1 as the caller should have done it */

	pte = pt1[0]; /* 1st entry is for guard page */
	ptetype = arch_pte_type(true, pte);
	if (ptetype == PTE_TABLE) {
		/* get the 2nd level page table */
		uint32_t *pt2 = (uint32_t *) (pte & PT_BASEADDR_MASK);

		if (pt2[0] == 0)
			/* guard page has already been properly set up */
			return;
	}

	if (ptetype != PTE_SECTION) {
		puts("Un-fixable guard page!");
		return;
	}

	/* An impromper/incorrect guard page is an indication of
	 * potetially insufficient DRAM page table construction.
	 * The insufficiency is 1GB limit per MEMC under 32bit
	 * address space. It was recently fixed, but might not be
	 * available with older FSBL.
	 */
	construct_dram_pages(pt1);

	__puts("g "); /* Mark fixup for guard page */

	set_guard_page(pt1);

	/* 0xC0000000 is where PCIe-0 mapping begins.
	 * Since PCIe is not supported in BOLT, accessing its mapped
	 * address is not expected and the whole mapping should be
	 * marked invalid in the page table. But, it is possible to have
	 * an incorrect page table entry for [0xC000_0000..0xC010_0000)
	 * if an old and unfixable FSBL is combined with SSBL.
	 *
	 * Since it is just one PTE whose final and correct value is
	 * an "invalid" PTE, simply mark it invalid without checking
	 * its current value.
	 */
	pt1[magic_offset] = 0;
}

/* assure_sram_pages -- makes sure that the SRAM pages are properly set up
 *
 * The whole SRAM reserved area, [0xFFE0_0000..0xFFF0_0000), was marked
 * valid with an older FSBL. Since the size of actually implemented SRAM
 * is 64KiB only (smaller than the reserved area), marking the whole 1MiB
 * of the SRAM reserved area is not accurate.
 *
 * This routine checks whether detailed SRAM pages are constructed, and
 * sets it up if not constructed previously.
 *
 * Parameter:
 *  pt1 [in] pointer to the 1st level page table
 */
static void assure_sram_pages(uint32_t *pt1)
{
	uint32_t pte;
	int ptetype;

	/* skip sanity check on pt1 as the caller should have done it */

	pte = pt1[SRAM_ADDR >> SECTION_SHIFT];
	ptetype = arch_pte_type(true, pte);
	if (ptetype == PTE_TABLE)
		/* SRAM pages have already been properly set up */
		return;

	if (ptetype == PTE_SECTION)
		set_sram_pages(pt1);
}

/* supplement_fsbl_pagetable -- supplements page table passed from FSBL
 *
 * FSBL sets up page table and passes it to SSBL even though MMU is
 * disable when jumping to SSBL. Then, SSBL adjusts PT in a way that
 * only necessary areas are marked excutable (via removing XN).
 *
 * Before such adjustment, this routine makes sure that PT passed from
 * FSBL is what SSBL assumes. This version of SSBL might be paired with
 * an older FSBL whose PT construction does not satisfy this SSBL.
 */
void supplement_fsbl_pagetable(uint32_t *pt1)
{
	assure_guard_page(pt1);
	assure_sram_pages(pt1);
}

