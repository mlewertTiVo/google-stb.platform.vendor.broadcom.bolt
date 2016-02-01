/*
 * Copyright (c) 2013, Marc Carino
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mem_pool.h"
#include "os.h"

static mem_pool_t mem_pool;

void mem_pool_get_pool_info(bfr_desc_t *bd)
{
	if (bd == NULL)
		return;

	bd->pa = mem_pool.pa_base;
	bd->va = mem_pool.va_base;
	bd->sz = mem_pool.sz;
}

/*
 * API for a slow but simple memory allocator. If performance is a problem,
 * add another bitmap level or use a linked-list.
 */
int mem_pool_init(bfr_desc_t *bfr_desc)
{
	const uint32_t sz_pool  = 1 * 1024 * 1024;
	const uint32_t sz_words = (sz_pool / sizeof(uint32_t)) + 1;

	if (sz_words < 32)
		return ERR_MEMPOOL_INIT_INV_SZ;

	mem_pool.va_base = (uint32_t *)bfr_desc->va;
	mem_pool.pa_base = bfr_desc->pa;
	mem_pool.sz = sz_pool;

	dbg_printf("va: %p, pa: %llxh, sz: %xh szw: %xh\n",
		   mem_pool.va_base,
		   mem_pool.pa_base,
		   mem_pool.sz,
		   sz_words);

	/*
	 * Use a bitmap to track free-space. Every bit represents a 32-bit
	 * word
	 */
	mem_pool.bitmap_sz = sz_words / 32 * 4;
	mem_pool.bitmap = (uint32_t *)malloc(mem_pool.bitmap_sz);
	if (!mem_pool.bitmap) {
		err_printf("couldn't allocate mem_pool bitmap!\n");
		return ERR_MEMPOOL_INIT_FAIL;
	}

	memset(mem_pool.bitmap, 0, mem_pool.bitmap_sz);

	bfr_desc->sz -= sz_pool;
	bfr_desc->pa += sz_pool;
	bfr_desc->va += (uintptr_t)sz_pool / sizeof(uintptr_t);

	return 0;
}

int mem_pool_alloc(int size, uint32_t alignment, bfr_desc_t *bd)
{
	const unsigned int num_words = mem_pool.bitmap_sz / sizeof(uint32_t);

	unsigned int i;
	unsigned int j;
	int          start;
	int          end;

	/*
	 * We can only do an alignment that's a multiple of 4 since the
	 * bitmap tracks DWORDS
	 */
	if ((alignment & 0x3) || (size & 0x3))
		return ERR_MEMPOOL_ALIGNMENT;

	if (!bd || !mem_pool.bitmap)
		return ERR_MEMPOOL_NOT_INIT;

	/* Find the first cleared bit that meets the alignment criteria */
	start = -1;
	end = -1;

	for (i = 0; (i < num_words) && (end == -1); i++) {
		uint32_t *word = &mem_pool.bitmap[i];

		for (j = 0; (j < 32) && (end == -1); j++) {
			const int curr = 4 * (j + (32 * i));

			if (start == -1) {
				if (alignment && ((curr % alignment) != 0))
					continue;
				else if (*word & BIT(j))
					continue;
				else
					start = curr;
			} else if (end == -1) {
				if (*word & BIT(j)) {
					start = -1;
					continue;
				} else if (curr == (start + (size / 4)))
					end = curr;
			}
		}
	}

	if (end != -1) {
		/* Set the bits as allocated */
		i = start / 4;
		j = end / 4;

		while (i < j) {
			mem_pool.bitmap[i / 32] |= BIT(i % 32);
			i++;
		}

		bd->va = (uint32_t *)(((uint32_t)mem_pool.va_base) +
			(start * 4));
		bd->pa = mem_pool.pa_base + (start * 4);
		bd->sz = size;
		bd->next = NULL;

		return 0;
	}

	return ERR_MEMPOOL_NO_FREE;
}

void mem_pool_free(bfr_desc_t *bd)
{
	assert(bd->va != NULL);

	uint32_t i = ((uint32_t)bd->va) - ((uint32_t)mem_pool.va_base);
	const uint32_t j = i + bd->sz;

	while (i < j) {
		mem_pool.bitmap[i / 32] &= ~BIT(i % 32);
		i++;
	}
}
