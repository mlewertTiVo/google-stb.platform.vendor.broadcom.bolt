/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <arch_ops.h>
#include <common.h>

/* arch_getticks -- returns timer tick value
 *
 * The lower 32 bits of the phycal count register (CNTPCT) of ARM System
 * Counter is returned. The speed of the counter should be referred to
 * the CNTFRQ register.
 *
 * returns the lower 32 bits of the CNTPCT register
 * see also: arch_get_timer_freq_hz()
 */
uint32_t arch_getticks(void)
{
	uint32_t count_low;
	uint32_t count_high;

	/* CNTPCT : 64 bits */
	isb(); /* Reads of CNTPCT can occur speculatively and out of order
		* relative to other instructions executed on the same
		* processor, on page B8-1960 of ARM Architectural Reference
		* Manual (ARMv7-A and ARMv7-R).
		*/
	__asm__ ("mrrc     p15, 0, %0, %1, c14"
		: "=r" (count_low), "=r" (count_high) : : "memory");

	return count_low;
}


uint64_t arch_getticks64(void)
{
	uint32_t count_low;
	uint32_t count_high;

	/* CNTPCT : 64 bits */
	isb(); /* Reads of CNTPCT can occur speculatively and out of order
		* relative to other instructions executed on the same
		* processor, on page B8-1960 of ARM Architectural Reference
		* Manual (ARMv7-A and ARMv7-R).
		*/
	__asm__ ("mrrc	  p15, 0, %0, %1, c14"
		: "=r" (count_low), "=r" (count_high) : : "memory");

	return (((uint64_t)count_high) << 32) | (uint64_t)count_low;
}

