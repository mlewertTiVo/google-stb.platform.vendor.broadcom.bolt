/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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

#define BITS2MASK(bits) ((1<<(bits))-1)

static uint32_t	bmem_free[MAX_DDR];
static uint32_t	bmem_ptop[MAX_DDR];

/* pointers to struct bnode */
static struct bnode *bmem_in_use_list[MAX_DDR]; /* sorted by addr */
static struct bnode *bmem_free_list[MAX_DDR]; /* sorted by addr */

static void consolidate(struct bnode *head);

static uint32_t AlignUp(uint32_t addr, unsigned int align_bits)
{
	return (addr + BITS2MASK(align_bits)) & ~BITS2MASK(align_bits);
}

/* insert_bnode -- inserts bnode into list in the ascending order of 'addr'
 *
 * Parameters:
 *  head - list that new_node is to be inserted
 *  new_node - a new node to be inserted
 *
 * Returns:
 *  pointer to the head of the list after the new node is inserted
 */
static struct bnode *insert_bnode(struct bnode *head, struct bnode *new_node)
{
	struct bnode *curr;
	struct bnode *prev;

	if (new_node == NULL)
		return head;

	if (head == NULL) {
		new_node->next = NULL;
		return new_node;
	}

	prev = NULL;
	curr = head;
	while (curr != NULL) {
		if (curr->addr == new_node->addr) {
			xprintf("SPLASH-GLUE: allocating the same BMEM addr\n"
				"	base 0x%08x sizes 0x%08x 0x%08x\n",
				(unsigned int)curr->addr, curr->size,
				new_node->size);
			/* ignore the new BMEM allocation */
			return head;
		}
		if (curr->addr > new_node->addr) {
			if (prev != NULL) {
				prev->next = new_node;
			} else {
				new_node->next = curr;
				head = new_node;
			}
			break;
		}
		/* advance two pointers one step each */
		prev = curr;
		curr = prev->next;
	}

	if (curr == NULL) {
		/* append */
		prev->next = new_node;
	}

	return head;
}

/* remove_bnode -- removes bnode whose addr matches given addr from list
 *
 * Parameters:
 *  head - pointer to the pointer of list
 *  addr - address to be searched for from the list
 *
 * Returns:
 *  pointer to removed bnode,
 *  NULL if non matches the given addr
 */
static struct bnode *remove_bnode(struct bnode **head, unsigned char *addr)
{
	struct bnode *curr;
	struct bnode *prev;

	prev = NULL;
	curr = *head;
	while (curr != NULL) {
		if (curr->addr == addr) {
			if (prev == NULL) {
				*head = curr->next;
			} else {
				prev->next = curr->next;
			}
			curr->next = NULL;
			return curr;
		}
		prev = curr;
		curr = curr->next;
	}

	return NULL;
}

/* can_combine -- whether two adjacent bnode's can be combined
 *
 * Parameters:
 *  prev - bnode in front of curr
 *  curr - bnode
 *
 * Returns:
 *  true if addr of curr starts right after prev's range
 *  false otherwise
 */
static bool can_combine(struct bnode *former, struct bnode *latter)
{
	if (former == NULL || latter == NULL)
		return false;

	return ((former->addr + former->size) == latter->addr);
}

/* consolidate -- consolidates bnode's that can be combined in list
 *
 * Parameters:
 *  head - pointer to list of bnode
 */
static void consolidate(struct bnode *head)
{
	struct bnode *curr;
	struct bnode *prev;

	prev = NULL;
	curr = head;
	while (curr != NULL) {
		if (!can_combine(prev, curr)) {
			prev = curr;
			curr = curr->next;
		} else {
			prev->size += curr->size;
			prev->next = curr->next;
			curr->next = NULL;
			KFREE(curr);
			curr = prev->next;
		}
	}
}

/* alloc_from_free_list -- allocates BMEM from list of bnode's
 *
 * Parameters:
 *  head - pointer to the pointer to list of free bnode's
 *  bytes - number of bytes to allocate
 *  align_bits - how many bits to be aligned
 *
 * Returns:
 *  pointer to bnode
 */
static struct bnode *alloc_from_free_list(struct bnode **head, size_t bytes,
	unsigned int align_bits)
{
	struct bnode *curr;
	struct bnode *prev;
	struct bnode *second_best = NULL;
	struct bnode *second_best_prev = NULL;
	bool second_best_is_aligned = false;
	unsigned int second_best_size = 0;
	unsigned int second_best_frag = 0;

	prev = NULL;
	curr = *head;
	while (curr != NULL) {
		uint32_t addr = AlignUp((uint32_t)curr->addr, align_bits);
		uint32_t bytes_frag = addr - (uint32_t)curr->addr;

		bytes += bytes_frag;
		if (curr->size == bytes) {
			/* look no further, enough to say perfect */
			if (bytes_frag == 0) {
				/* this is the most perfect fit */
				if (prev == NULL) {
					*head = curr->next;
				} else {
					prev->next = curr->next;
				}
			} else {
				/* split it up */
				struct bnode *frag;

				/* fragmented memory due to alignment remains */
				frag = (struct bnode *)
					KMALLOC(sizeof(struct bnode), 0);
				if (frag == NULL) {
					err_msg("%s: failed to alloc struct "
						"bnode\n", __func__);
					return NULL;
				}
				frag->addr = curr->addr;
				frag->size = bytes_frag;
				frag->next = curr->next;
				prev->next = frag;
			}
			curr->addr += bytes_frag;
			curr->size -= bytes_frag;
			curr->next = NULL;
			return curr;
		} else if (curr->size > bytes) {
			if (bytes_frag != 0) {
				/* aligned bnode has priority */
				if (!second_best_is_aligned) {
					if (second_best_size >= curr->size) {
						second_best = curr;
						second_best_prev = prev;
						second_best_size = curr->size;
						second_best_frag = bytes_frag;
					}
				}
			} else {
				if (second_best_is_aligned) {
					/* smallest fit first */
					if (second_best_size >= curr->size) {
						second_best = curr;
						second_best_prev = prev;
						second_best_size = curr->size;
					}
				} else {
					/* aligned bnode has priority */
					second_best = curr;
					second_best_prev = prev;
					second_best_size = curr->size;
					second_best_is_aligned = true;
				}
			}
		}

		prev = curr;
		curr = curr->next;
	}

	if (second_best != NULL) {
		if (second_best_is_aligned) {
			if (second_best_prev == NULL) {
				*head = curr->next;
			} else {
				second_best_prev->next = second_best->next;
			}
		} else {
			/* split it up */
			struct bnode *frag;

			/* fragmented memory due to alignment remains in list */
			frag = (struct bnode *)KMALLOC(sizeof(struct bnode), 0);
			if (frag == NULL) {
				err_msg("%s: failed to alloc struct "
					"bnode\n", __func__);
				return NULL;
			}
			frag->addr = second_best->addr;
			frag->size = second_best_frag;
			frag->next = second_best->next;
			second_best_prev->next = frag;
			second_best->addr += second_best_frag;
			second_best->size -= second_best_frag;
			second_best->next = NULL;
		}
		return second_best;
	}

	return NULL;
}

/* splash_glue_init_bmem -- initializes BMEM counterpart for given memc
 *
 * Parameters:
 *  memc - memory controller index
 *  b - pointer to board type
 *
 * Returns:
 *  0 if successful or no memory deemed installed on memc
 *  1 if the board type is not detected
 *  BOLT_ERR if nothing can be reserved for a memory installed memc
 */
static int splash_glue_init_bmem(uint32_t memc, struct board_type *b)
{
	struct ddr_info *ddr;
	uint32_t top, size_mb;
	/* skip sanity check, the caller should make sure */

	if (!b) {
		b = board_thisboard();
		if (!b)
			return 1;
	}

	ddr = board_find_ddr(b, memc);
	if (!ddr) {
		/* it is possible that nothing is installed on 'memc' */
		return 0;
	}

	/*
	 * FSBL MMU code may have restrictions on what can be mapped.
	 * (e.g. BOLT does not support LPAE on ARM)
	 */
	size_mb = ddr_get_restricted_size_mb(ddr);
	if (!size_mb) {
		err_msg("%s: Bad size for DDR %d.  Check your memory map.",
			__func__, memc);
		return BOLT_ERR;
	}

	/* heap grows down */
	top = _MB(ddr->base_mb + size_mb);

	bmem_free[memc] = top;
	bmem_ptop[memc] = top;

#if (CFG_CMD_LEVEL >= 3)
	xprintf("SPLASH BMEM init @ %x\n", bmem_ptop[memc]);
#endif
	return 0;
}

void BDBG_Assert(bool cond, char *name, char *path, const char *func, int line)
{
	if (cond)
		return;

	xprintf("#### ERROR %s is not true at line# %d in %s@%s\n",
		name, line, func, path);
}

BERR_Code BMEM_ConvertAddressToOffset(BMEM_Handle heap,
	void *addr, uint32_t *offset)
{
	*offset = (uint32_t)addr;
	return 0;
}

BERR_Code BMEM_Heap_ConvertAddressToCached(BMEM_Heap_Handle heap,
	void *addr, void **addr_cached)
{
	*addr_cached = addr;
	return 0;
}

BERR_Code BMEM_Heap_FlushCache(BMEM_Heap_Handle heap,
	void *addr_cached, size_t bytes)
{
	CACHE_FLUSH_ALL();
	return 0;
}

void *BMEM_AllocAligned(
	BMEM_Handle	pheap,	      /* heap to allocate from */
	size_t		bytes,	      /* number of bytes to allocate */
	unsigned int	align_bits,   /* alignment for the block */
	unsigned int	boundary      /* boundry restricting allocated value */
)
{
	uint32_t pAllocMem, adjSize, adjMem, heap;
	struct bnode *bnode_new;
	uint32_t tag_free_top;

	/* splash_api_start() passes BMEM_Handle[] that maps to
	 * a DDR/MEMC number to splash_script_run(), then that splash app
	 * forwards this to us here, which is nice. The error check is
	 * a "should not happen" case but add in case users make a new RUL
	 * list and its a bad one.
	 */
	heap = (uint32_t)pheap;
	if (heap >= MAX_DDR) {
		xprintf("%s() bad heap! %u >= %u\n",
						__func__, heap, MAX_DDR);
		return NULL;
	}

	if (bmem_ptop[heap] == 0) {
		/* Even though it is within MAX_DDR, it is possible that
		 * no DRAM is installed on a specific MEMC. But, allocating
		 * memory out of such MEMC should fail.
		 */
		xprintf("%s() MEMC#%u does NOT have installed memory.\n",
			__func__, heap);
		return NULL;
	}

	if (bytes == 0) {
		xprintf("%s() zero allocation from heap %d with %d align_bits "
			"0x%08x boundary\n",
			__func__, heap, align_bits, boundary);
		return NULL;
	}

	bnode_new =
		alloc_from_free_list(&bmem_free_list[heap], bytes, align_bits);
	if (bnode_new != NULL) {
		bmem_in_use_list[heap] =
			insert_bnode(bmem_in_use_list[heap], bnode_new);
#if (CFG_CMD_LEVEL >= 5)
		xprintf("BMEM_Alloc(%d) 0x%08x @ 0x%08x\n",
			heap, bnode_new->size, (unsigned int)bnode_new->addr);
#endif
		return (void *)bnode_new->addr;
	}

	tag_free_top = bmem_free[heap];

	/* Add alignment value in case AlignUp() adjusts us. */
	adjSize = bytes + BITS2MASK(align_bits);

	/* Grow down */
	adjMem = bmem_free[heap] - adjSize;

	pAllocMem = AlignUp(adjMem, align_bits);

	bmem_free[heap] = pAllocMem;

#if (CFG_CMD_LEVEL >= 5)
	xprintf("BMEM_Alloc(%d) base:0x%08x bytes:0x%08x adjSize:0x%08x ",
				heap, bmem_ptop[heap], bytes, adjSize);
	xprintf("[adjMem:0x%08x] pAllocMem:0x%08x align_bits:0x%08x / 0x%08x\n",
		adjMem, pAllocMem, align_bits, BITS2MASK(align_bits));
#endif
	bnode_new = (struct bnode *) KMALLOC(sizeof(struct bnode), 0);
	if (bnode_new == NULL) {
		xprintf("SPLASH-GLUE: failed to alloc struct bnode\n");
	} else {
		bnode_new->size = tag_free_top - bmem_free[heap];
		bnode_new->addr = (uint8_t *)pAllocMem;
		bnode_new->next = NULL;
		bmem_in_use_list[heap] =
			insert_bnode(bmem_in_use_list[heap], bnode_new);
	}

	return (void *)pAllocMem;
}

void BMEM_Heap_Free(
	BMEM_Handle	pheap,	/* heap from which the block was alocated */
	void		*addr	/* allocated block address */
)
{
	uint32_t heap;
	struct bnode *p;

	heap = (uint32_t)pheap;
	if (heap >= MAX_DDR) {
		xprintf("%s() bad heap! %u >= %u\n",
						__func__, heap, MAX_DDR);
		return;
	}

	p = remove_bnode(&bmem_in_use_list[heap], addr);
	if (p == NULL) {
		xprintf("%s() invalid free 0x%08x to heap %u\n",
			__func__, (unsigned int)addr, heap);
		return;
	}

	if (bmem_in_use_list[heap] == NULL) {
		/* everything has been returned */
		struct bnode *curr = bmem_free_list[heap];

		KFREE(p);
		while (curr != NULL) {
			struct bnode *next = curr->next;
			KFREE(curr);
			curr = next;
		}
		bmem_free_list[heap] = NULL;
		(void)splash_glue_init_bmem(heap, NULL); /* re-initialize */
		return;
	}

	bmem_free_list[heap] = insert_bnode(bmem_free_list[heap], p);
	consolidate(bmem_free_list[heap]);
}

/* splash_glue_init -- initializes glue logic for splash between URSR and BOLT
 *
 * Returns:
 *  0 if successful
 *  1 if detect board type fails
 *  BOLT_ERR if initializing BMEM counterpart fails
 */
int splash_glue_init(void)
{
	unsigned int i;
	struct board_type *b;

	b = board_thisboard();
	if (!b)
		return 1;

	for (i = 0; i < b->nddr; i++) {
		if (splash_glue_init_bmem(i, b) != 0)
			return BOLT_ERR;
	}

	return 0;
}


/* Get values to plug into devicetree, exposed in splash-api.h */
int splash_glue_getmem(uint32_t memc, uint32_t *memtop, uint32_t *memlowest)
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

