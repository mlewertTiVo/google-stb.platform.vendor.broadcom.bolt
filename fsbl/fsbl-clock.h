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

#endif /* _FSBL_CLOCK_H */

