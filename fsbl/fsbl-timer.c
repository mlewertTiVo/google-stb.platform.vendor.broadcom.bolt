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

#include <common.h>
#include "fsbl.h"
#include "bchp_cntcontrolbase.h"


void udelay(uint32_t us)
{
	uint32_t now, f, duration, elapsed;

	f = BDEV_RD(BCHP_CNTControlBase_CNTFID0);
	now = BDEV_RD(BCHP_CNTControlBase_CNTCV_LO);
	duration = (f / 1000000) * us;

	do {
		elapsed = BDEV_RD(BCHP_CNTControlBase_CNTCV_LO) - now;
	} while (elapsed < duration);
}

void sleep_ms(uint32_t ms)
{
	udelay(1000 * ms);
}

/* return 64-bit counter value (27MHz counter) */
uint64_t get_syscount(void)
{
	uint64_t count_hi1, count_hi2, count_lo, count;

	count_hi1 = BDEV_RD(BCHP_CNTControlBase_CNTCV_HI);
	count_lo = BDEV_RD(BCHP_CNTControlBase_CNTCV_LO) & 0xFFFFFFFF;
	count_hi2 = BDEV_RD(BCHP_CNTControlBase_CNTCV_HI);
	if ((count_hi1 != count_hi2)  &&  (count_lo & 0x80000000))
		count = (count_hi1 << 32) | count_lo;
	else
		count = (count_hi2 << 32) | count_lo;

	return count;
}

/* Return time in micro-seconds.  Counter overflows after 4 months */
uint64_t get_systime(void)
{
	uint32_t f, frac, q = 16;
	uint64_t time_us;

	f = BDEV_RD(BCHP_CNTControlBase_CNTFID0);
	f = f / 1000000;
	if (f == 0)
		f = 1;
	frac = (1 << q) / f;
	time_us = (get_syscount() * frac) >> q;

	return time_us;
}


uint64_t get_time_diff(uint64_t was)
{

	return get_systime() - was;
}
