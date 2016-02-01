/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
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

/* ------------------------------------------------------------------------- */

#define SPLASH_HDR_SIZE 128 /* BMP file format, fixed. */

#define MAX_SPLASH_SIZE	(512*1024)	/* 512Kb */

/* ------------------------------------------------------------------------- */

#define BMEM_Heap_Handle BMEM_Handle

#define BKNI_Memset(m, v, s) memset(m, v, s)
#define BKNI_Free(m) KFREE(m)
#define BMEM_Alloc(heap, size) BMEM_AllocAligned(heap, size, 8, 0)


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


/* ------------------------------------------------------------------------- */

int splash_glue_init(void);

/* ------------------------------------------------------------------------- */

/* from BSEAV
*/
extern int splash_script_run(BREG_Handle hReg, BMEM_Handle *phMem);


#endif /* __SPLASH_GLUE_H__ */
