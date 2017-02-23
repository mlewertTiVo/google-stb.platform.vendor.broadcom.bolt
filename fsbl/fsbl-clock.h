/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _FSBL_CLOCK_H
#define _FSBL_CLOCK_H

#include <common.h>
#include "fsbl.h"

unsigned int get_cpu_freq_mhz(void);
void adjust_clocks(struct board_type *b, uint32_t mhl_power);

#ifdef DVFS_SUPPORT
void apply_dvfs_clocks(struct clock_divisors cpu_clks,
	struct clock_divisors scb_clks, uint8_t sysif_mdiv);
#else
static inline void apply_dvfs_clocks(struct clock_divisors cpu_clks,
	struct clock_divisors scb_clks, uint8_t sysif_mdiv) {}
#endif

#endif /* _FSBL_CLOCK_H */

