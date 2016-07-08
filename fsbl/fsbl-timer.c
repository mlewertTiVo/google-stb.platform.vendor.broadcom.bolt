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
#include "fsbl.h"
#include "bchp_cntcontrolbase.h"

#if (BCHP_CNTControlBase_CNTFID0_CNTFID_DEFAULT != 27000000)
#error "Timer API's have to be reviewed since 27MHz assumption is invalid."
#endif

static const uint32_t TicksPerMicrosecond =
	BCHP_CNTControlBase_CNTFID0_CNTFID_DEFAULT / 1000000;

void udelay(uint32_t us)
{
	uint32_t now, duration, elapsed;

	now = arch_getticks();
	duration = (TicksPerMicrosecond * us) + 1;

	do {
		elapsed = arch_getticks() - now;
	} while (elapsed < duration);
}

void sleep_ms(uint32_t ms)
{
	udelay(1000 * ms);
}

/* return time in micro-seconds. Counter overflows after 4294.97 seconds.
 *  4294967295 == maximum number of an unsigned 32 bit integer, (2^32 - 1)
 *  4,294,967,295 micro-seconds ~= 4,294.97 seconds (71 minutes 34.97 seconds)
 */
uint32_t get_time_us(void)
{
	uint32_t frac, q = 16;
	uint64_t time_us;

	frac = (1 << q) / TicksPerMicrosecond;
	time_us = (arch_getticks64() * frac) >> q;

	return time_us & 0x00000000ffffffffULL;
}

uint32_t get_time_diff(uint32_t was)
{
	return get_time_us() - was;
}
