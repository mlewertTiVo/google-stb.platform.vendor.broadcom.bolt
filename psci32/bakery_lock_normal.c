/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <common.h>
#include "psci32.h"
#include "armv8.h"
#include "bakery_lock32.h"

/*
 * Functions in this file implement Bakery Algorithm for mutual exclusion with the
 * bakery lock data structures in cacheable and Normal memory.
 *
 * ARM architecture offers a family of exclusive access instructions to
 * efficiently implement mutual exclusion with hardware support. However, as
 * well as depending on external hardware, these instructions have defined
 * behavior only on certain memory types (cacheable and Normal memory in
 * particular; see ARMv8 Architecture Reference Manual section B2.10). Use cases
 * in trusted firmware are such that mutual exclusion implementation cannot
 * expect that accesses to the lock have the specific type required by the
 * architecture for these primitives to function (for example, not all
 * contenders may have address translation enabled).
 *
 * This implementation does not use mutual exclusion primitives. It expects
 * memory regions where the locks reside to be cacheable and Normal.
 *
 * Note that the ARM architecture guarantees single-copy atomicity for aligned
 * accesses regardless of status of address translation.
 */


#define write_cache_op(_addr, _cached)	\
	do {	\
		if (_cached)\
			set_dccvac((uint32_t)_addr); \
		else \
			set_dcivac((uint32_t)_addr);\
		dsbish();\
	} while (0)


#define read_cache_op(_addr, _cached) \
	do {	\
		if (_cached) { \
			set_dccivac((uint32_t)_addr); \
			BARRIER();\
		} \
	} while (0)


/* NOTE:
 * Verify that the platform defined value for the per-cpu space for bakery locks is
 * a multiple of the cache line size, to prevent multiple CPUs writing to the same
 * bakery lock cache line. For the current BOLT we operate with dcache off (no mmu
 * @ EL3.)
 */
static int cpu_dcache_on(void)
{
	unsigned long sctlr = get_sys_ctl_reg();

	return !!(sctlr & SCTLR_DUCACHE_ENABLE);
}


static unsigned int get_ticket(volatile uint32_t *lock,
						unsigned int me, int is_cached)
{
	struct psci_cfg *cfg = get_config();
	unsigned int my_ticket = 0, their_ticket, they;
	volatile uint32_t *their_lock;

	/*
	 * Prevent recursive acquisition.
	 * Since lock data is written to and cleaned by the owning cpu, it
	 * doesn't require any cache operations prior to reading the lock data.
	 */
	if (bakery_ticket_number(*lock)) {
		if (debug())
			puts("!BGT");
	}

	/*
	 * Tell other contenders that we are through the bakery doorway i.e.
	 * going to allocate a ticket for this cpu.
	 */
	*lock = make_bakery_data(CHOOSING_TICKET, my_ticket);

	write_cache_op(lock, is_cached);

	/*
	 * Iterate through the bakery information of each contender to allocate
	 * the highest ticket number for this cpu.
	 */
	for (they = 0; they < cfg->all_cpus; they++) {
		if (me == they)
			continue;

		/*
		 * Get a reference to the other contender's bakery info and
		 * ensure that a stale copy is not read.
		 */
		their_lock = &(cfg->cpu[they].lock);

		read_cache_op(their_lock, is_cached);

		/*
		 * Update this cpu's ticket number if a higher ticket number is
		 * seen
		 */
		their_ticket = bakery_ticket_number(*their_lock);

		if (their_ticket > my_ticket)
			my_ticket = their_ticket;
	}

	/*
	 * Compute ticket; then signal to other contenders waiting for us to
	 * finish calculating our ticket value that we're done
	 */
	++my_ticket;

	*lock = make_bakery_data(CHOSEN_TICKET, my_ticket);

	write_cache_op(lock, is_cached);

	return my_ticket;
}

void bakery_lock_get(unsigned int me, volatile uint32_t *lock)
{
	struct psci_cfg *cfg = get_config();
	unsigned int my_ticket, their_ticket, is_cached, they, my_prio;
	volatile uint32_t *their_lock;

	is_cached = cpu_dcache_on();

	/* Get a ticket */
	my_ticket = get_ticket(lock, me, is_cached);

	/*
	 * Now that we got our ticket, compute our priority value, then compare
	 * with that of others, and proceed to acquire the lock
	 */
	my_prio = PRIORITY(my_ticket, me);

	for (they = 0; they < cfg->all_cpus; they++) {
		if (me == they)
			continue;

		their_lock = &(cfg->cpu[they].lock);

		/* Wait for the contender to get their ticket */
		do {
			read_cache_op(their_lock, is_cached);

		} while (bakery_is_choosing(*their_lock));

		/*
		 * If the other party is a contender, they'll have non-zero
		 * (valid) ticket value. If they do, compare priorities
		 */
		their_ticket = bakery_ticket_number(*their_lock);

		if (their_ticket && (PRIORITY(their_ticket, they) < my_prio)) {
			/*
			 * They have higher priority (lower value). Wait for
			 * their ticket value to change (either release the lock
			 * to have it dropped to 0; or drop and probably content
			 * again for the same lock to have an even higher value)
			 */
			do {
				wfe();
				read_cache_op(their_lock, is_cached);

			} while (their_ticket ==
				bakery_ticket_number(*their_lock));
		}
	}
	/* Lock acquired */
}

int bakery_lock_status(volatile uint32_t *lock)
{
	return !!(bakery_ticket_number(*lock));
}

void bakery_lock_release(volatile uint32_t *lock)
{
	/* If we did not have a ticket number then we're
	 * possibly double releasing it which indicates
	 * a problem. Just report it for now.
	 */
	if (!bakery_ticket_number(*lock)) {
		if (debug())
			puts("!BLR");
	}

	*lock = 0;

	write_cache_op(lock, cpu_dcache_on());

	/* ARM DDI 0487A.a, G1-3462 "The Send Event instruction"
	 * ...ARM recommends that software includes a DSB
	 * instruction before an SEV instruction.
	 */
	dsb();
	sev();
}
