/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARCH_MMU_H__
#define __ARCH_MMU_H__

#include <stdint.h>

#ifndef CFG_EMULATION
void set_pte_range(uint32_t *tbl,
	unsigned long va_start, unsigned long va_end, uint32_t attr);
void set_guard_page(uint32_t *tbl);
void set_sram_pages(uint32_t *tbl);
#endif /* !CFG_EMULATION */

#endif /* __ARCH_MMU_H__ */

