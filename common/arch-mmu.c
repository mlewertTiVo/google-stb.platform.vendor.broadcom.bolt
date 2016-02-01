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

#include <arch-mmu.h>
#include <arm.h>
#include <common.h>
#include <lib_types.h>

#include <stdint.h>

#ifndef CFG_EMULATION

/*
 * Sets the short-descriptor first-level PT entry.
 *
 * Note: The section type descriptor entries map memory with 1MB granularity.
 */
void set_pte_range(uint32_t *tbl,
	unsigned long va_start, unsigned long va_end, uint32_t attr)
{
	unsigned long pa = va_start;
	unsigned long aligned_va_end = ALIGN_UP_TO(va_end, SECTION_SIZE);

#if (CFG_CMD_LEVEL >= 5) /* show PTE range config */
	writehex(va_start);__puts(" : "); 
	writehex(aligned_va_end);__puts(" [");
	writehex(attr);puts("]");
#endif
	tbl += va_start >> SECTION_SHIFT;
	do {
		*(tbl++) = pa | attr;
		va_start += SECTION_SIZE;
		pa += SECTION_SIZE;
	} while (va_start != aligned_va_end);
}

/*
 *       |                       |
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

/*
 * The guard page is a 4K page that maps VA:0x0 -> VA:0xFFF as
 * invalid. Any accesses to the guard page results in a translation
 * fault, which causes a data abort.
 */
void set_guard_page(uint32_t *tbl)
{
	uint32_t *pt_baseaddr = (uint32_t *)
		(SSBL_PAGE_TABLE_SIZE + (uint32_t)tbl);
	const uint32_t *pt_endaddr = (uint32_t *)
		(SSBL_PAGE_TABLE_2_SIZE + (uint32_t)pt_baseaddr);
	uint32_t pa;
	uint32_t n;

	/* Change the 1st-level section mapping to a page-table descriptor */
	n = (uintptr_t)pt_baseaddr | PT_TYPE;
	set_pte_range(tbl, 0, _MB(1), n);

	/*
	 * First, set all entries in the 2nd-level page table to the identity
	 * mapping.
	 */
	pa = 0;
	while (pt_baseaddr != pt_endaddr) {
#if !CFG_UNCACHED
		n = SMALL_PG_MEM_CACHED_WB | SMALL_PG_XN;
#else
		n = SMALL_PG_MEM_NONCACHED | SMALL_PG_XN;
#endif
		*pt_baseaddr++ = pa | n;
		pa += SMALL_PG_SIZE;
	}

	/* The 1st-entry in the 2nd-level page table maps the guard page */
	pt_baseaddr = (uint32_t *)(SSBL_PAGE_TABLE_SIZE + (uint32_t)tbl);
	*pt_baseaddr = 0;
}

void set_sram_pages(uint32_t *tbl)
{
	uint32_t *pt_baseaddr = (uint32_t *)(SSBL_PAGE_TABLE_2_SIZE +
		SSBL_PAGE_TABLE_SIZE + (uint32_t)tbl);
	const uint32_t *pt_endaddr_sram =
		pt_baseaddr + SRAM_LEN / SMALL_PG_SIZE;
	const uint32_t *pt_endaddr = (uint32_t *)
		(SSBL_PAGE_TABLE_2_SIZE + (uint32_t)pt_baseaddr);
	uint32_t pa;

	/* Set 1st-level pte to a page-table descriptor */
	tbl += SRAM_ADDR >> SECTION_SHIFT;
	*tbl = (uintptr_t)pt_baseaddr | PT_TYPE;

	/* Set all the 2nd-level pte mappings */
	pa = SRAM_ADDR;
	while (pt_baseaddr != pt_endaddr_sram) {
		*pt_baseaddr++ = pa |
#if !CFG_UNCACHED
			SMALL_PG_MEM_CACHED_WB;
#else
			SMALL_PG_MEM_NONCACHED;
#endif
		pa += SMALL_PG_SIZE;
	}

	/* Clear the remaining entries to mark them invalid */
	while (pt_baseaddr != pt_endaddr)
		*pt_baseaddr++ = 0;
}

#endif /* !CFG_EMULATION */

