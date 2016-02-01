/***************************************************************************
 *	 Copyright (c) 2012-2015, Broadcom Corporation
 *	 All Rights Reserved
 *	 Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "error.h"
#include "devfuncs.h"
#include "ddr.h"

#include "bsp_config.h"
#include "board.h"
#include "board_init.h"

#include "splash_magnum.h"
#include "splash_bmp.h"


/* ------------------------------------------------------------------------- */

#define BITS2MASK(bits) ((1<<(bits))-1)


/* ------------------------------------------------------------------------- */

static uint32_t	bmem_free[MAX_DDR];
static uint32_t	bmem_ptop[MAX_DDR];


/* ------------------------------------------------------------------------- */

BERR_Code BMEM_ConvertAddressToOffset(BMEM_Handle heap,
	void *addr, uint32_t *offset) {
	*offset = (uint32_t)addr;
	return 0;
}


/* ------------------------------------------------------------------------- */

BERR_Code BMEM_Heap_ConvertAddressToCached(BMEM_Heap_Handle Heap,
	void *pvAddress, void **ppvCachedAddress) {
	*ppvCachedAddress = pvAddress;
	return 0;
}


/* ------------------------------------------------------------------------- */

BERR_Code BMEM_Heap_FlushCache(BMEM_Heap_Handle Heap,
	void *pvCachedAddress, size_t size) {
	CACHE_FLUSH_ALL();
	return 0;
}


/* ------------------------------------------------------------------------- */

uint32_t AlignUp(uint32_t ui32Address, unsigned int uiAlignBits)
{
	return (ui32Address + BITS2MASK(uiAlignBits)) & ~BITS2MASK(uiAlignBits);
}


/* ------------------------------------------------------------------------- */

void *BMEM_AllocAligned(
	BMEM_Handle	pheap,	      /* Heap to allocate from */
	size_t		ulSize,	      /* size in bytes of block to allocate */
	unsigned int	uiAlignBits,  /* alignment for the block */
	unsigned int	Boundary      /* boundry restricting allocated value */
)
{
	uint32_t pAllocMem, adjSize, adjMem, heap;

	/*
	  splash_api_start() passes BMEM_Handle[] that maps to a DDR/MEMC number
	 to splash_script_run(), then that splash app forwards this to us here,
	 which is nice. The error check is a "should not happen" case but add
	 in case users make a new RUL list and its a bad one.
	*/
	heap = (uint32_t)pheap;
	if (heap >= MAX_DDR) {
		xprintf("%s() bad heap! %u >= %u\n",
						__func__, heap, MAX_DDR);
		return NULL;
	}

	/* Add alignment value in case AlignUp() adjusts us.
	 */
	adjSize = ulSize + BITS2MASK(uiAlignBits);

	/* Grow down
	 */
	adjMem = bmem_free[heap] - adjSize;

	pAllocMem = AlignUp(adjMem, uiAlignBits);

	bmem_free[heap] = adjMem;

#if (CFG_CMD_LEVEL >= 5)
	xprintf("BMEM_Alloc(%d) base:0x%08x ulSize:0x%08x adjSize:0x%08x ",
				heap, bmem_ptop[heap], ulSize, adjSize);
	xprintf("[adjMem:0x%08x] pAllocMem:0x%08x uiAlignBits:0x%08x / 0x%08x\n",
		adjMem, pAllocMem, uiAlignBits, BITS2MASK(uiAlignBits));
#endif
	return (void *)pAllocMem;
}


/* ------------------------------------------------------------------------- */

int splash_glue_init(void)
{
	unsigned int i;
	uint32_t top, size;
	struct board_type *b;
	struct ddr_info *ddr;

	b = board_thisboard();
	if (!b)
		return 1;

	for (i = 0; i < b->nddr; i++) {
		ddr = board_find_ddr(b, i);
		if (!ddr)
			continue;

		/*
		 * FSBL MMU code may have restrictions on what can be mapped.
		 * (e.g. BOLT does not support LPAE on ARM)
		 */
		size = ddr_get_restricted_size_mb(ddr);
		if (!size) {
			err_msg("%s: Bad size for DDR %d.  Check your memory map.",
					__func__, i);
			return BOLT_ERR;
		}

		/* heap grows down
		*/
		top = ddr->base_mb + size;

		/* check we are within mmu bounds
		*/
		if (top > MMU_MEMTOP)
			top = MMU_MEMTOP;

		top = _MB(top);

		bmem_free[ddr->which] = top;
		bmem_ptop[ddr->which] = top;

#if (CFG_CMD_LEVEL >= 3)
		xprintf("SPLASH BMEM init @ %x\n", bmem_ptop[ddr->which]);
#endif
	}
	return 0;
}


/* ------------------------------------------------------------------------- */
/* Get values to plug into devicetree, exposed in splash-api.h
*/
int  splash_glue_getmem(uint32_t memc, uint32_t *memtop, uint32_t *memlowest)
{
	if (memc >= MAX_DDR) { /* memc == heap == ddr->which */
#if (CFG_CMD_LEVEL >= 5)
		xprintf("%s() bad memc! %u >= %u\n",
						__func__, memc, MAX_DDR);
#endif
		return 1;
	}
	*memtop	   = bmem_ptop[memc];
	*memlowest = bmem_free[memc];
	return 0;
}

