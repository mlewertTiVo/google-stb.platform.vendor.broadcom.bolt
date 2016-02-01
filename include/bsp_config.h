/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include "arch.h"
#include "boardcfg.h"
#include "config.h"

#if CFG_GENET
#define INT_PHY_ADDR 1
#endif

#define __HEAP_SIZE  ALIGN_UP_TO(CFG_HEAP_SIZE,  CFG_ALIGN_SIZE)
#define __STACK_SIZE ALIGN_UP_TO(CFG_STACK_SIZE, CFG_ALIGN_SIZE)

/* Defensively align DMA to the RAC 4KiB granularity, though this is
reportedly an obsolete technique. */
#define DMA_BUF_ALIGN	_KB(4) /* SWBOLT-1027 */

/* part of the bsp */
#include "cache_ops.h"
#include "arch_ops.h"

#endif /* __BSP_CONFIG_H__ */
