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
#include <bl_common.h>

/* SPSR modes */
#define SPSR_MODE_RW_SHIFT	0x4
#define SPSR_MODE_RW_64		0x0
#define SPSR_MODE_RW_32		0x1

/* From ld script */
extern void *_smm_zero_start[];
extern void *_smm_zero_end[];
extern void *__config_info[];
extern void *__per_cpu_info[];

static struct psci_cfg * const config = (struct psci_cfg *)__config_info;

image_info_t bl31_image_info;
entry_point_info_t bl32_ep_info;
image_info_t bl32_image_info;
entry_point_info_t bl33_ep_info;
image_info_t bl33_image_info;
bl31_params_t bl31_params;

void smm_set_loadparams(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
			uint64_t x4 /* current stack pointer */)
{
	static int uninited = 1;
	unsigned int la_flags = (unsigned int) x3;
	uint32_t spsr;

	/* TODO: static setup would be better */
	if (uninited) {
		/* Setup ep info records */
		bl32_ep_info.h.type = PARAM_EP;
		bl32_ep_info.h.version = VERSION_1;
		bl32_ep_info.h.size = 0;
		bl32_ep_info.h.attr = SECURE;
		bl33_ep_info.h.type = PARAM_EP;
		bl33_ep_info.h.version = VERSION_1;
		bl33_ep_info.h.size = 0;
		bl33_ep_info.h.attr = NON_SECURE;

		/* Setup image records */
		bl32_image_info.h.type = PARAM_IMAGE_BINARY;
		bl32_image_info.h.version = VERSION_1;
		bl32_image_info.h.size = 0;
		bl32_image_info.h.attr = SECURE;
		bl33_image_info.h.type = PARAM_IMAGE_BINARY;
		bl33_image_info.h.version = VERSION_1;
		bl33_image_info.h.size = 0;
		bl33_image_info.h.attr = NON_SECURE;

		/* Setup bl31 params */
		bl31_params.h.type = PARAM_BL31;
		bl31_params.h.version = VERSION_1;
		bl31_params.h.size = 0;
		bl31_params.h.attr = 0;
		bl31_params.bl31_image_info = &bl31_image_info;
		bl31_params.bl32_ep_info = &bl32_ep_info;
		bl31_params.bl32_image_info = &bl32_image_info;
		bl31_params.bl33_ep_info = &bl33_ep_info;
		bl31_params.bl33_image_info = &bl33_image_info;
		uninited = 0;
	}

	if (la_flags & LOADFLG_APP64)
		spsr = (SPSR_MODE_RW_64 << SPSR_MODE_RW_SHIFT);
	else
		spsr = (SPSR_MODE_RW_32 << SPSR_MODE_RW_SHIFT);

	if (la_flags & LOADFLG_SECURE) {
		bl32_ep_info.pc = x1;
		bl32_ep_info.args.arg0 = x2;
		bl32_ep_info.spsr = spsr;
	} else {
		bl33_ep_info.pc = x1;
		if (la_flags & LOADFLG_APP64)
			bl33_ep_info.args.arg0 = x2;
		else
			bl33_ep_info.args.arg2 = x2;
		bl33_ep_info.spsr = spsr;
	}
	eret64(PSCI_SUCCESS, x4);
}

