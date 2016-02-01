/***************************************************************************
 *     Copyright (c) 2012-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
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

#if defined(CONFIG_BCM3390A0) || defined (CONFIG_BCM3390B0)
void bcm3390_hack_early_bus_cfg(void);
void bcm3390_hack_late_bus_cfg(void);
#else
static inline void bcm3390_hack_early_bus_cfg(void) { }
static inline void bcm3390_hack_late_bus_cfg(void) { }
#endif

#if defined(CONFIG_BCM7145B0)
void bcm7145_hack_early_bus_cfg(void);
void bcm7145_hack_late_bus_cfg(void);
#else
static inline void bcm7145_hack_early_bus_cfg(void) { }
static inline void bcm7145_hack_late_bus_cfg(void) { }
#endif

#if defined(CONFIG_BCM7366B0) || defined(CONFIG_BCM7366C0)
void bcm7366b0_mii_rx_err_cfg(void);
#else
static inline void bcm7366b0_mii_rx_err_cfg(void) { }
#endif

void hack_power_down_cpus(void);

#endif /* _FSBL_HACKS_H */
