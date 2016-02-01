/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_magnum.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/22/11 5:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/os/cfe/splash_magnum.c $
 * 
 * 6   8/22/11 5:02p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 *
 * 5   10/14/10 5:20p jkim
 * SWCFE-386: remove compiler warning.
 *
 * 4   9/25/09 11:45a nickh
 * SW7420-351:  Add MEMC1 support for 7420
 *
 * 2   8/9/07 3:50p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 1   7/24/07 6:46p shyam
 * PR 30741 : Add magnum portability layer for CFE environment
 *
 ***************************************************************************/
#include "splash_magnum.h"
#include "lib_printf.h"

BERR_Code BMEM_ConvertAddressToOffset(BMEM_Handle heap, void* addr, uint32_t* offset)
{
	if ( (uint32_t)addr >= 0xe0000000 )
	{
#if BCHP_CHIP == 7420
		*offset = ((uint32_t)addr - 0xe0000000)+0x60000000 ;
#else /*BCHP_CHIP == 7425*/
		*offset = ((uint32_t)addr - 0xe0000000)+0x90000000 ;
#endif
	}
	else
	{
		*offset = K1_TO_PHYS(((uint32_t)addr));
	}

	BDBG_MSG(("BMEM : Virtual %p Physical %08x\n", addr, *offset));
	return 0 ;
}

#define MEMORY_BASE				((void *)UNCADDR(0x5800000))

static uint8_t *ui32CurrentStaticMemoryPointer = MEMORY_BASE ;
static uint8_t *ui32CurrentStaticMemoryBase = MEMORY_BASE ;
static uint32_t ui32StaticMemorySize = 8*1024*1024 ;

/* BMVD_P_AlignAddress : Aligns the address to the specified bit position
 */
uint32_t AlignAddress(
		uint32_t	ui32Address,	/* [in] size in bytes of block to allocate */
		unsigned int uiAlignBits	/* [in] alignment for the block */
		)
{
	return (ui32Address+((1<<uiAlignBits)-1)) & ~((1<<uiAlignBits)-1) ;
}


void *BMEM_AllocAligned
(
	BMEM_Handle       pheap,       /* Heap to allocate from */
	size_t            ulSize,      /* size in bytes of block to allocate */
	unsigned int      uiAlignBits, /* alignment for the block */
	unsigned int      Boundary     /* boundry restricting allocated value */
)
{
	uint32_t pAllocMem ;
	uint32_t ui32_adjSize ;

	ui32_adjSize = ulSize + (1<<uiAlignBits)-1 ;

	/* The simple static memory allocator works as follows :
	 * The running pointer always points to the starting address
	 * of the remaining and free portion of the static memory block.
	 */
	pAllocMem = (uint32_t)ui32CurrentStaticMemoryPointer ;

	/* Point the running pointer to the end+1 of the current buffer *
	 * but first check if the static memory block is not exhausted */
	if( (ui32CurrentStaticMemoryPointer+ui32_adjSize) >
		(ui32CurrentStaticMemoryBase+ui32StaticMemorySize) )
	{
		BDBG_ERR(("BMEM_AllocAligned : No more Memory available"));
		return NULL;
	}

	/* Align the address to the specified Bit position */
	pAllocMem = AlignAddress(pAllocMem, uiAlignBits) ;

	ui32CurrentStaticMemoryPointer += ui32_adjSize;

	BDBG_MSG(("Allocated Memory : %08lx, size %08lx, prealigned = %08lx\n", pAllocMem, ulSize, ui32CurrentStaticMemoryPointer ));
	return (void *)pAllocMem ;
}

/* End of File */
