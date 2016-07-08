/*
 * Copyright (c) 2013-2015, ARM Limited and Contributors. All rights reserved.
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

#ifndef __BAKERY_LOCK32_H__
#define __BAKERY_LOCK32_H__


/*****************************************************************************
 * Internal helper macros used by the bakery lock implementation.
 ****************************************************************************/
/* Convert a ticket to priority */
#define PRIORITY(t, pos)	(((t) << 8) | (pos))

#define CHOOSING_TICKET		0x1
#define CHOSEN_TICKET		0x0

/*
 * The lock_data is a bit-field of 2 members:
 * Bit[0]       : choosing. This field is set when the CPU is
 *                choosing its bakery number.
 * Bits[1 - 15] : number. This is the bakery number allocated.
 */
#define bakery_is_choosing(info)	(info & CHOOSING_TICKET)

#define bakery_ticket_number(info)	((info >> 1) & 0x7FFF)

#define make_bakery_data(choosing, number) \
		(((choosing & 0x1) | (number << 1)) & 0xFFFF)


/*****************************************************************************
 * External bakery lock interface.
 ****************************************************************************/
/*
 * Bakery locks are stored in normal memory
 * For BOLT PSCI, EL3 MMU is off.
 */

void bakery_lock_get(unsigned int me, volatile uint32_t *lock);
void bakery_lock_release(volatile uint32_t *lock);
int bakery_lock_status(volatile uint32_t *lock);

#endif /* __BAKERY_LOCK32_H__ */
