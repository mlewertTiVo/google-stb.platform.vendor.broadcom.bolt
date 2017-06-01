/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __LIB_MALLOC_H__
#define __LIB_MALLOC_H__

#include <stddef.h>

typedef struct memstats_s {
	unsigned int mem_totalbytes;
	unsigned int mem_allocbytes;
	unsigned int mem_freebytes;
	unsigned int mem_allocnodes;
	unsigned int mem_freenodes;
	unsigned int mem_largest;
} memstats_t;

typedef struct mempool_s mempool_t;
void bmeminit(mempool_t * pool, unsigned char *buffer, int length);
void bfree(mempool_t * pool, void *ptr);

/* Note that bmalloc() will call cache invalidate on
newly allocated memory. */
void *bmalloc(mempool_t * pool, unsigned int size, unsigned int align);

int bmemchk(mempool_t * pool, int verbose);
extern mempool_t bmempool;
extern mempool_t bumempool;
void *bmempoolbase(mempool_t * pool);
int bmempoolsize(mempool_t * pool);
int bmemstats(mempool_t * pool, memstats_t * stats);
size_t bmalloc_usable_size(mempool_t *pool, void *ptr);

#define KMEMINIT(buffer,length) bmeminit(&bmempool,(buffer),(length))
#define KMEMPOOLBASE() bmempoolbase(&bmempool)
#define KMEMPOOLSIZE() bmempoolsize(&bmempool)
#define KMALLOC(size,align) bmalloc(&bmempool,(size),(align))
#define KFREE(ptr) bfree(&bmempool,(ptr))
#define KMEMSTATS(s) bmemstats(&bmempool,(s))
#define KMALLOC_USABLE_SIZE(s) bmalloc_usable_size(&bmempool, (s))

#define KUMALLOC(size,align) bmalloc(&bumempool,(size),(align))
#define KUFREE(ptr) bfree(&bumempool,(ptr))
#define KUMALLOC_USABLE_SIZE(s) bmalloc_usable_size(&bumempool, (s))

#endif /* __LIB_MALLOC_H__ */

