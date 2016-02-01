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

#include "os.h"
#if defined(_BOLT_)
#include "board.h"
#include "cache_ops.h"
#endif

WEAK uint32_t _getticks(void)
{
#if defined(__NONOS) && defined(_BOLT_)
	return arch_getticks() / get_clocks_per_usec();
#else
	struct timespec ts;
	uint64_t        us;
	int             ret = clock_gettime(CLOCK_REALTIME, &ts);

	assert(ret == 0);
	us = ts.tv_sec * 1000000;
	us += ts.tv_nsec / 1000;

	return (uint32_t)(us & 0xFFFFFFFF);
#endif
}

void cache_flush(void *addr, uint32_t len)
{
#if defined(__NONOS) && defined(_BOLT_)
	CACHE_FLUSH_RANGE(addr, len);
#endif
}

void cache_inval(void *addr, uint32_t len)
{
#if defined(__NONOS) && defined(_BOLT_)
	CACHE_INVAL_RANGE(addr, len);
#endif
}
