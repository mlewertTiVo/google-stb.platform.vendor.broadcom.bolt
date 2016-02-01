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

#ifndef __ARCH_OPS_H__
#define __ARCH_OPS_H__
#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>

/* architecture specific */

uint64_t	arch_get_cpu_pll_hz(void);
uint64_t	arch_get_cpu_vco_hz(void);
uint64_t	arch_get_cpu_freq_hz(void);
uint64_t	arch_get_cpu_min_safe_hz(void);
int		arch_get_num_processors(void);
uint64_t	arch_get_scb_freq_hz(void);
uint32_t	arch_get_timer_freq_hz(void);
uint32_t	arch_getticks(void);
uint64_t	arch_getticks64(void);
void		arch_mark_uncached(uintptr_t uc, unsigned int size);
int		arch_mark_executable(uintptr_t addr, unsigned int size,
			bool executable);
void		arch_config(void);
int		arch_pte_type(bool first_level, uint32_t pte);

enum {
	CPU_CLK_RATIO_ONE = 0,
	CPU_CLK_RATIO_HALF = 1,
	CPU_CLK_RATIO_QUARTER = 2,
	CPU_CLK_RATIO_EIGHTH = 3,
	CPU_CLK_RATIO_SIXTEENTH = 4,
};

void arch_set_cpu_clk_ratio(int ratio);

#endif /* __ASSEMBLER__ */
#endif /*__ARCH_OPS_H__  */
