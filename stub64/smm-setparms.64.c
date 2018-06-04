/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 * Initial SM calls to setup EL3 firmware and NS world image parameters.
 *
 **************************************************************************/
#include <aarch64/armv8-regs.h>
#include <aarch64/armv8-cop.h>
#include <psci64.h>
#include <psci.h>
#include <loader.h>
#include "mon_params.h"
#include "brcmstb_params.h"

/* SPSR modes */
#define SPSR_MODE_RW_SHIFT	0x4
#define SPSR_MODE_RW_64		0x0
#define SPSR_MODE_RW_32		0x1

/* From ld script */
extern void *_smm_zero_start[];
extern void *_smm_zero_end[];
extern void *__config_info[];
extern void *__per_cpu_info[];

mon_params_t mon_params = {
	.hdr = {
		.type = MONITOR_PARAMS,
		.version = MONITOR_PARAMS_VERSION
	}
};

brcmstb_params_t brcmstb_params = {
	.hdr = {
		.type = BRCMSTB_PARAMS,
		.version = BRCMSTB_PARAMS_VERSION
	},
	.bootsram_secure = {
#if defined(BCHP_BOOTSRAM_SECURE_REG_START)
		.base = BCHP_PHYSICAL_OFFSET
		      + BCHP_BOOTSRAM_SECURE_REG_START,
		.size = BCHP_BOOTSRAM_SECURE_REG_END
		      - BCHP_BOOTSRAM_SECURE_REG_START + 4
#elif defined(BCHP_HIF_BOOTSRAM_SECURE_REG_START)
		.base = BCHP_PHYSICAL_OFFSET
		      + BCHP_HIF_BOOTSRAM_SECURE_REG_START,
		.size = BCHP_HIF_BOOTSRAM_SECURE_REG_END
		      - BCHP_HIF_BOOTSRAM_SECURE_REG_START + 4
#else
#error "Secure boot SRAM is not define for this chip"
#endif
	},
	.sun_top_ctrl = {
		.base = BCHP_PHYSICAL_OFFSET
		      + BCHP_SUN_TOP_CTRL_REG_START,
		.size = BCHP_SUN_TOP_CTRL_REG_END
		      - BCHP_SUN_TOP_CTRL_REG_START + 4
	},
	.hif_cpubiuctrl = {
		.base = BCHP_PHYSICAL_OFFSET
		      + BCHP_HIF_CPUBIUCTRL_REG_START,
		.size = BCHP_HIF_CPUBIUCTRL_REG_END
		      - BCHP_HIF_CPUBIUCTRL_REG_START + 4
	},
	.hif_continuation = {
		.base = BCHP_PHYSICAL_OFFSET
		      + BCHP_HIF_CONTINUATION_REG_START,
		.size = BCHP_HIF_CONTINUATION_REG_END
		      - BCHP_HIF_CONTINUATION_REG_START + 4
	}
};

void smm_set_loadparams(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
			uint64_t x4 /* current stack pointer */)
{
	unsigned int la_flags = (unsigned int) x3;

	if (la_flags & LOADFLG_SECURE) {
		mon_params.tz_img_info.flags |= (la_flags & LOADFLG_APP64) ?
			IMG_AARCH64 : IMG_AARCH32;

		mon_params.tz_entry_pt = x1;
		mon_params.tz_dev_tree = x2;
	} else {
		mon_params.nw_img_info.flags |= (la_flags & LOADFLG_APP64) ?
			IMG_AARCH64 : IMG_AARCH32;

		mon_params.nw_entry_pt = x1;
		mon_params.nw_dev_tree = x2;
	}
	eret64(PSCI_SUCCESS, x4);
}

