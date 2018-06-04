/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifdef TZ_SECURE_BOOT
#include "sec_boot.h"
#endif

#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"

#include "error.h"
#include "bolt.h"
#include "board.h"
#include "aon_defs.h"
#include "arch_ops.h"
#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_hif_continuation.h"
#include "mon_params.h"
#include "brcmstb_params.h"
#include "mon64.h"

#define PARAMS_HDR(_type, _version) \
{ \
	.type = _type, \
	.version = _version \
}

#define RGROUP_DESC(_base, _size) \
{ \
	.base = _base, \
	.size = _size \
}

#define RGROUP_DESC_REV(_base, _size, _rev) \
{ \
	.base = _base, \
	.size = _size, \
	.rev = _rev, \
}

#define RGROUP_BCHP_DESC(_name) \
	RGROUP_DESC(BCHP_PHYSICAL_OFFSET + BCHP_##_name##_REG_START, \
		BCHP_##_name##_REG_END - BCHP_##_name##_REG_START + 4)

#define RGROUP_BCHP_DESC_REV(_name, _rev) \
	RGROUP_DESC_REV(BCHP_PHYSICAL_OFFSET + BCHP_##_name##_REG_START, \
		BCHP_##_name##_REG_END - BCHP_##_name##_REG_START + 4, \
		_rev)

mon_params_t mon_params = {
	.hdr = PARAMS_HDR(MONITOR_PARAMS, MONITOR_PARAMS_VERSION),
};

brcmstb_params_t brcmstb_params = {
	.hdr = PARAMS_HDR(BRCMSTB_PARAMS, BRCMSTB_PARAMS_VERSION),
	.rgroups = {
		/* BRCMSTB_RGROUP_BOOTSRAM */
		RGROUP_DESC(SRAM_ADDR, SRAM_LEN),

		/* BRCMSTB_RGROUP_SUN_TOP_CTRL */
		RGROUP_BCHP_DESC(SUN_TOP_CTRL),

		/* BRCMSTB_RGROUP_HIF_CPUBIUCTRL */
		RGROUP_BCHP_DESC(HIF_CPUBIUCTRL),

		/* BRCMSTB_RGROUP_HIF_CONTINUATION */
#ifndef BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0
		/* 64-bit registers */
		RGROUP_BCHP_DESC_REV(HIF_CONTINUATION, 1),
#else
		RGROUP_BCHP_DESC(HIF_CONTINUATION),
#endif
		/* BRCMSTB_RGROUP_HIF_CPU_INTR1 */
		RGROUP_BCHP_DESC(HIF_CPU_INTR1),

		/* BRCMSTB_RGROUP_AVS_CPU_DATA_MEM */
		RGROUP_BCHP_DESC(AVS_CPU_DATA_MEM),

		/* BRCMSTB_RGROUP_AVS_HOST_L2 */
		RGROUP_BCHP_DESC(AVS_HOST_L2),

		/* BRCMSTB_RGROUP_AVS_CPU_L2 */
		RGROUP_BCHP_DESC(AVS_CPU_L2),

		/* BRCMSTB_RGROUP_SCPU_GLOBALRAM */
		RGROUP_BCHP_DESC(SCPU_GLOBALRAM),

		/* BRCMSTB_RGROUP_SCPU_HOST_INTR2 */
		RGROUP_BCHP_DESC(SCPU_HOST_INTR2),

		/* BRCMSTB_RGROUP_CPU_IPI_INTR2 */
		RGROUP_BCHP_DESC(CPU_IPI_INTR2),

		/* BRCMSTB_RGROUP_AON_CTRL */
		RGROUP_BCHP_DESC(AON_CTRL),
	},
	.intrs = {
		/* BRCMSTB_INTR_AVS_CPU */
		BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVS_CPU_INTR_SHIFT,

		/* BRCMSTB_INTR_SCPU_CPU */
		BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_SCPU_CPU_INTR_SHIFT,
	}
};

void mon64_init(void)
{
	int64_t retval;

	/* Set general monitor parameters */
	mon_params.num_clusters = 1;
	mon_params.num_cpus = arch_get_num_processors();

	mon_params.tz_cpus_mask = (1 << mon_params.num_cpus) - 1;
	mon_params.nw_cpus_mask = (1 << mon_params.num_cpus) - 1;

	/* Reserve memory for mon64 to NW mailbox */
	retval = bolt_reserve_memory(_KB(4), _KB(4),
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW, "NWMBOX");
	if (retval < 0)
		err_msg("failed to reserve 4 KB for NWMBOX %lld\n", retval);

	mon_params.nw_mbox_info.addr = retval;
	mon_params.nw_mbox_info.size = _KB(4);
	mon_params.nw_mbox_info.sgi  = MON64_NW_MBOX_SGI;
}

void mon64_install(long load_address, int load_size, long *install_address)
{
	long dest_address;
	int dest_size;
	int rc;

	dest_address = MON64_BASE;
	dest_size = MON64_SIZE;

	rc = sec_verify_mon64((void *)load_address, load_size,
			      (void *)dest_address, dest_size);
	if (rc)
		err_msg("failed to verify mon64 binary %d\n", rc);

	if (install_address)
		*install_address = dest_address;
}

#ifndef TZ_SECURE_BOOT
int sec_verify_mon64(void *load_address, int load_size,
		    void *dest_address, int dest_size)
{
	/* Copy mon64 from load address directly */
	memcpy((void *)dest_address, (void *)load_address, load_size);
	return BOLT_OK;
}
#endif

int mon64_boot(unsigned int la_flags, long la_entrypt, void *dt_address)
{
	if (la_flags & LOADFLG_SECURE) {
		struct memory_area list;
		int count;
		bool found;

		xprintf("MON64: setting params for Astra @ %#lx (DTB @ %p)\n",
			la_entrypt, dt_address);

		mon_params.tz_img_info.flags |= (la_flags & LOADFLG_APP64) ?
			IMG_AARCH64 : IMG_AARCH32;

		mon_params.tz_entry_point = (uintptr_t)la_entrypt;
		mon_params.tz_dev_tree = (uintptr_t)dt_address;

		/* Retieve memory for mon64 to TZ mailbox */
		count = bolt_reserve_memory_getlist(&list);
		if (count <= 0)
			return BOLT_ERR_NOMEM;

		found = false;
		while (!q_isempty((queue_t *)&list)) {
			queue_t *q = q_getfirst((queue_t *)&list);
			struct memory_area *m = (struct memory_area *)q;

			if (found || !m->tag || strcmp(m->tag, "TZMBOX")) {
				q_dequeue(q);
				KFREE(q);
				continue;
			}

			found = true;
			mon_params.tz_mbox_info.addr = m->offset;
			mon_params.tz_mbox_info.size = m->size;
			mon_params.tz_mbox_info.sgi  = MON64_TZ_MBOX_SGI;
		}
	} else {
		xprintf("MON64: setting params for Linux @ %#lx (DTB @ %p)\n",
			la_entrypt, dt_address);

		mon_params.nw_img_info.flags |= (la_flags & LOADFLG_APP64) ?
			IMG_AARCH64 : IMG_AARCH32;

		mon_params.nw_entry_point = (uintptr_t)la_entrypt;
		mon_params.nw_dev_tree = (uintptr_t)dt_address;
	}

	if (la_flags & LOADFLG_EXECUTE) {
		struct fsbl_info *info = board_info();

		xprintf("MON64: starting mon64 at %#lx (params @ %p %p)\n",
			(long)MON64_BASE, &mon_params, &brcmstb_params);

		bolt_set_aon_bootmode(S3_FLAG_PSCI_BOOT | S3_FLAG_BOOTED64);

		bolt_start_mon64(
			(unsigned long)&mon_params,
			(unsigned long)&brcmstb_params,
			(unsigned long)((info) ? info->uart_base : 0));
	}

	return BOLT_OK;
}
