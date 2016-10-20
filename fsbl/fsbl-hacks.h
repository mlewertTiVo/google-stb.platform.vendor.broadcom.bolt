/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _FSBL_HACKS_H
#define _FSBL_HACKS_H

#include <common.h>
#include <lib_types.h>
#include "fsbl.h"

#if defined(CONFIG_BCM7260) || defined(CONFIG_BCM7268) || defined(CONFIG_BCM7271)
void orion_hack_early_bus_cfg(void);
#else
static inline void orion_hack_early_bus_cfg(void) { }
#endif

void hack_power_down_cpus(void);

#endif /* _FSBL_HACKS_H */
