/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* API that faces toward BSEAV splash app
*/

#ifndef __SPLASH_GLUE_H__
 #define __SPLASH_GLUE_H__

#ifndef __SPLASH_MAGNUM_H__
 #error use splash_magnum.h instead, do not directly include this file!
#endif

#include "splash-media.h"
#include "splash_script_load.h"
#include "timer.h"
#include "bchp_rdc.h"

#include <stdbool.h>

/* Enable 64 bit register read and write if RDC registers are 64 bit wide*/
#ifdef BCHP_RDC_desc_0_addr_reserved0_SHIFT
#define RDC_IS_64BIT
#endif

#ifdef RDC_IS_64BIT
#define BREG_WriteAddr BREG_Write64
#else
#define BREG_WriteAddr BREG_Write32
#endif

#define SPLASH_HDR_SIZE 128 /* BMP file format, fixed. */
#define MAX_SPLASH_SIZE	(512*1024)	/* 512Kb */

#define BMEM_Heap_Handle BMEM_Handle

#define BKNI_Free(m) KFREE(m)
#define BKNI_Malloc(m) KMALLOC((m), 0)
#define BKNI_Memset(m, v, s) memset(m, v, s)
#define BKNI_Sleep(s) bolt_usleep(s)
#define BMEM_Alloc(heap, size) BMEM_AllocAligned(heap, size, 8, 0)
#define BMEM_Free(heap, p) BMEM_Heap_Free(heap, p)

#define BDBG_ASSERT(cond) BDBG_Assert((cond), #cond, __FILE__, __func__, __LINE__)

void BDBG_Assert(bool cond, char *name, char *path, const char *func, int line);

BERR_Code BMEM_ConvertAddressToOffset
(
	BMEM_Handle	heap,
	void		*addr,
	uint32_t	*offset
);

BERR_Code BMEM_Heap_ConvertAddressToCached
(
	BMEM_Heap_Handle  Heap,             /* Heap that contains the memory block. */
	void             *pvAddress,        /* Address of the memory block */
	void            **ppvCachedAddress  /* [out] Returned cache address. */
);

BERR_Code BMEM_Heap_FlushCache
(
	BMEM_Heap_Handle  Heap,             /* Heap containing the cached memory. */
	void             *pvCachedAddress,  /* Start address to flush */
	size_t            size              /* Size in bytes of the block to flush */
);

void *BMEM_AllocAligned
(
	BMEM_Handle       pheap,       /* Heap to allocate from */
	size_t            ulSize,      /* size in bytes of block to allocate */
	unsigned int      uiAlignBits, /* alignment for the block */
	unsigned int      Boundary     /* boundry restricting allocated value */
);

void BMEM_Heap_Free
(
	BMEM_Handle	pheap,	/* Heap to allocate from */
	void		*addr	/* size in bytes of block to allocate */
);

struct bnode {
	uint32_t size;
	uint8_t *addr;
	struct bnode *next;
};

int splash_glue_init(void);

#endif /* __SPLASH_GLUE_H__ */
