/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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
uint32_t	arch_get_midr(void);
int		arch_get_num_processors(void);
uint64_t	arch_get_scb_freq_hz(void);
uint64_t	arch_get_sysif_freq_hz(void);
uint32_t	arch_get_timer_freq_hz(void);
void		arch_mark_uncached(uintptr_t uc, unsigned int size);
int		arch_mark_executable(uintptr_t addr, unsigned int size,
			bool executable);
void		arch_config(void);
int		arch_pte_type(bool first_level, uint32_t pte);
void		arch_call_bolt_main(unsigned int new_stack_pointer);
const char	*arch_get_cpu_bootname(void);
int		arch_booted64(void);

/* common */
uint32_t	arch_getticks(void);
uint64_t	arch_getticks64(void);

enum {
	CPU_CLK_RATIO_ONE = 0,
	CPU_CLK_RATIO_HALF = 1,
	CPU_CLK_RATIO_QUARTER = 2,
	CPU_CLK_RATIO_EIGHTH = 3,
	CPU_CLK_RATIO_SIXTEENTH = 4,
};

void arch_set_cpu_clk_ratio(int ratio);

int memsys_ssbl_ddr_info(uint32_t idx, uint32_t *clock, uint32_t *phy_width,
			   uint32_t *dev_size, uint32_t *dev_width);

#endif /* __ASSEMBLER__ */
#endif /*__ARCH_OPS_H__  */
