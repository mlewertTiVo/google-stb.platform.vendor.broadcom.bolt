/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* FSBL (First Stage BootLoader) and SSBL (Second Stage BootLoader) are
 * two main parts of BOLT. As the names imply, FSBL starts before SSBL
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

#include <arch-mmu.h>
#include <arch_ops.h>
#include <arm.h>
#include <arm-start.h>
#include <board.h>
#include <common.h>
#include <ddr.h>
#include <mmap-dram.h>
#include <supplement-fsbl.h>

#include <stddef.h>
#include <stdint.h>

#ifdef CFG_EMULATION
bool supplement_fsbl_checkguardpage(uint32_t *pt1)
{
	return true;
}

void supplement_fsbl_pagetable(uint32_t *pt1)
{
	/* do nothing */
}

uint32_t supplement_fsbl_shmoover(void)
{
	return 0;
}
#else
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

/* supplement_fsbl_checkguardpage -- checks if guard page was correctly set
 *
 * Parameter:
 *  pt1 [in] pointer to the 1st level page table
 */
bool supplement_fsbl_checkguardpage(uint32_t *pt1)
{
	uint32_t pte;
	int ptetype;

	pte = pt1[0]; /* 1st entry is for guard page */
	ptetype = arch_pte_type(true, pte);
	if (ptetype == PTE_FAULT) {
		return true;
	} else if (ptetype == PTE_TABLE) {
		/* get the 2nd level page table */
		uint32_t *pt2 = (uint32_t *) (pte & PT_BASEADDR_MASK);

		if (pt2[0] == 0)
			/* guard page has already been properly set up */
			return true;
	}

	return false;
}

/* supplement_fsbl_pagetable -- supplements page table passed from FSBL
 *
 * With an older FSBL, the guard page was mistakenly created and linked
 * to an incorrect location of 1st level page table, 0xC000_0000.
 *
 * Checking the indication of such improperly constructed guard page is
 * done and page table entries for installed DRAM are constructued
 * including a correct guard page before calling this function.
 *
 * Since 0xC000_0000 is a valid DRAM address on a newer chip design,
 * fixing the location is done in this function along with PTE's for
 * SRAM, which was too coarse in older FSBL.
 *
 * Parameters:
 *  pt1                    [in] pointer to the 1st level page table
 *  guard_page_was_correct [in] whether guard page was correctly set by FSBL
 */
void supplement_fsbl_pagetable(uint32_t *pt1, bool guard_page_was_correct)
{
#ifndef dram_mapping_table_v7_64
	/* yes, 0xC0000000 is a magic number */
	const unsigned int magic_offset = 0xC0000000 >> SECTION_SHIFT;

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
	if (!guard_page_was_correct)
		pt1[magic_offset] = 0;
#endif

	assure_sram_pages(pt1);
}

/* supplement_fsbl_shmoover -- retrieves Shmoo (memsysinitlib) version
 *
 * Requires: fsbl_info version 2 or higher
 *
 * Shmoo runs in FSBL, and becomes unavailable by the time the control
 * moves to SSBL. Because figuring out the version information of
 * Shmoo that ran in FSBL is extremely difficult (if not impossible),
 * FSBL passes the version information via 'struct fsbl_info'.
 *
 * Returns:
 *  0 if Shmoo version is not available
 *  Shmoo version in 32-bit unsigned integer otherwise
 */
uint32_t supplement_fsbl_shmoover(void)
{
	struct fsbl_info *info;

	info = board_info();
	if (info == NULL) /* should never happen though */
		return 0;

	/* available only 'fsbl_info' version 2 or higher */
	if (2 > FSBLINFO_VERSION(info->n_boards))
		return 0;

	return info->shmoo_ver;
}
#endif
