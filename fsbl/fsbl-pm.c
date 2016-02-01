/***************************************************************************
 *	 Copyright (c) 2012-2015, Broadcom Corporation
 *	 All Rights Reserved
 *	 Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>
#include <aon_defs.h>
#include <stdbool.h>

#include "fsbl.h"
#include "fsbl-pm.h"
#include "../common/xpt_dma.h"
#include "boot_defines.h"
#include "bchp_hif_top_ctrl.h"

/* we're assuming the PA:VA mapping is a 1:1 identity function in FSBL */
static inline dma_addr_t va_to_pa(void *p)
{
	return (dma_addr_t)(uintptr_t)p;
}

static inline void *pa_to_va(dma_addr_t addr)
{
	return (void *)(uintptr_t)addr;
}

/* Reset history must contain only 's3_wakeup', with no other reset causes */
static bool reset_history_is_s3(void)
{
	uint32_t history = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY);
	return history == BCHP_AON_CTRL_RESET_HISTORY_s3_wakeup_reset_MASK;
}

#if CFG_PM_S3

/*
 * Linux may not always be equipped to run memory through the crypto engines;
 * allow (non-secure) BOLT to still perform warm S3 warm boot without verifying
 * memory
 */
#ifndef SECURE_BOOT
static inline bool fsbl_pm_mem_verify(uint32_t flags)
{
	return !(flags & S3_FLAG_NO_MEM_VERIFY);
}
#endif

/*
 * Check for warm boot:
 */
bool fsbl_ack_warm_boot(void)
{
	uint32_t magic = AON_REG(AON_REG_MAGIC_FLAGS);

	if (reset_history_is_s3() && (magic & BRCMSTB_S3_MAGIC_MASK) ==
			BRCMSTB_S3_MAGIC_SHORT) {
		return true;
	}
	return false;
}

/* Clear reset history
 *  - Reset history must contain only 's3_wakeup', with no other reset causes
 *  - AON SRAM must contain the magic S3 flag
 *
 * A caller is responsible for determining whether it is a warm boot.
 * If it is warm boot, then clear the history and flags, to avoid any
 * reboot/suspend loops.
 */
void fsbl_clear_reset_history(void)
{
	uint32_t magic = AON_REG(AON_REG_MAGIC_FLAGS);

	/*
	 * while we are forcing 0 -> 1 -> 0 transitions here because those
	 * registers are implemented as rising edge.
	 */
	BDEV_WR_RB(BCHP_AON_CTRL_RESET_CTRL, 0);
	BDEV_WR_RB(BCHP_AON_CTRL_RESET_CTRL, 1);
	BDEV_WR_RB(BCHP_AON_CTRL_RESET_CTRL, 0);

	AON_REG(AON_REG_MAGIC_FLAGS) = magic & ~BRCMSTB_S3_MAGIC_MASK;
}

enum bsp_initiate_command {
	BSP_CLOCK_STOP		= 0x00,
	BSP_GEN_RANDOM_KEY	= 0x4A,
	BSP_RESTORE_RANDOM_KEY	= 0x55,
	BSP_GEN_FIXED_KEY	= 0x63,
};

#define PM_INITIATE		0x01
#define PM_INITIATE_SUCCESS	0x00
#define PM_INITIATE_FAIL	0xfe

static int s3_key_load(enum bsp_initiate_command cmd)
{
	int ret;

	BDEV_WR_RB(BCHP_AON_CTRL_PM_INITIATE, 0);

	/* Go! */
	BDEV_WR(BCHP_AON_CTRL_PM_INITIATE, (cmd << 1) | PM_INITIATE);

	do {
		ret = BDEV_RD(BCHP_AON_CTRL_PM_INITIATE);
	} while (ret & PM_INITIATE);

	return (ret & 0xff) != PM_INITIATE_SUCCESS;

	return 0;
}

static int s3_compare_hash(uint32_t *orig, uint32_t *new)
{
	unsigned int i;
	for (i = 0; i < BRCMSTB_HASH_LEN / 4; i++)
		if (orig[i] != new[i])
			return -1;
	return 0;
}

static int verify_s3_params(struct brcmstb_s3_params *params, uint32_t flags)
{
	struct mcpb_dma_desc *desc;
	void *scratch;
	struct dma_region region;
	uint32_t hash[BRCMSTB_HASH_LEN / 4], orig_hash[BRCMSTB_HASH_LEN / 4];
	unsigned int i;
	enum bsp_initiate_command cmd;
	volatile int status;

	/* Scratch memory located at start */
	scratch = &params->scratch;

	/* Verify the rest */
	region.addr = (uintptr_t)&params->magic;
	region.len = AON_REG(AON_REG_CONTROL_HASH_LEN);

	__puts("Verifying S3 parameters @ ");
	writehex(region.addr);
	puts("");

	if (params->magic != BRCMSTB_S3_MAGIC)
		return -1;

#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		puts("WARNING: Skipping params verification");
		return 0;
	}
#endif

	/* Load random / fixed key */
	if (flags & S3_FLAG_LOAD_RANDKEY)
		cmd = BSP_RESTORE_RANDOM_KEY;
	else
		cmd = BSP_GEN_FIXED_KEY;
	if (s3_key_load(cmd))
		return -1;

	/* Use scratch memory for descriptor */
	desc = scratch;
	memdma_prepare_descs(desc, va_to_pa(desc), &region, 1, true);

	if (memdma_run(va_to_pa(desc), 0, false))
		return -1;

	get_hash(hash, false);

	for (i = 0; i < ARRAY_SIZE(orig_hash); i++)
		orig_hash[i] = AON_REG(AON_REG_S3_HASH + i);

	status = 1234; /* security anti-glitch trace status */
	status = s3_compare_hash(orig_hash, hash);
	if (status) {
		puts("S3 parameter verification hash mismatch");
		return -1;
	}

	check_return_val((uint32_t)status, BPHYSADDR(BCHP_HIF_TOP_CTRL_SCRATCH),
			 TRACE_VERIFY_WARM_0_BIT, ERR_S3_PARAM_HASH_FAILED);

	puts("Verification complete");

	return 0;
}

static int verify_s3_memory(struct brcmstb_s3_params *params, uint32_t flags)
{
	dma_addr_t desc1 = va_to_pa(&params->descriptors);
	dma_addr_t desc2;
	bool dual_channel = params->desc_offset_2;
	uint32_t hash[BRCMSTB_HASH_LEN / 4];
	volatile int status;
	size_t len;
	int maxdescs = IMAGE_DESCRIPTORS_BUFSIZE / sizeof(struct mcpb_dma_desc);

#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		puts("WARNING: Skipping memory verification");
		return 0;
	}
#endif

	if (dual_channel)
		desc2 = desc1 + params->desc_offset_2;
	else
		desc2 = 0;

	len = mcpb_get_dma_chain_len(
		(struct mcpb_dma_desc *)pa_to_va(desc1), maxdescs);
	if (dual_channel)
		len += mcpb_get_dma_chain_len(
			(struct mcpb_dma_desc *)pa_to_va(desc2), maxdescs);

	__puts("Verifying main memory, len ");
	writehex(len);
	puts("");

	if (memdma_run(desc1, desc2, dual_channel))
		return -1;

	get_hash(hash, dual_channel);

	status = 1234; /* security anti-glitch trace status */
	status = s3_compare_hash(params->hash, hash);
	if (status) {
		puts("Memory verification hash mismatch");
		return -1;
	}

	check_return_val(status, BPHYSADDR(BCHP_HIF_TOP_CTRL_SCRATCH),
			 TRACE_VERIFY_WARM_1_BIT, ERR_S3_DDR_HASH_FAILED);

	puts("Verification complete");

	return 0;
}

void fsbl_finish_warm_boot(uint32_t restore_val)
{
	struct brcmstb_s3_params *params;
	uintptr_t addr;
	uint32_t flags;
	extern uint32_t glitch_addr, glitch_trace;
	const struct memdma_initparams e = {die, udelay, memset};

	/*
	 * FIXME: once BFW stops making assumptions about AON_REG(0), we can
	 * pull 'flags' directly from AON, e.g.:
	 *
	 *     flags = AON_REG(AON_REG_MAGIC_FLAGS) & ~BRCMSTB_S3_MAGIC_MASK;
	 */
	flags = restore_val & ~BRCMSTB_S3_MAGIC_MASK;

	addr = AON_REG(AON_REG_CONTROL_LOW);
	addr |= shift_left_32((uintptr_t)AON_REG(AON_REG_CONTROL_HIGH));

	/* init trace register value */
	BDEV_WR(BCHP_HIF_TOP_CTRL_SCRATCH, TRACE_VALID_MARKER);

	memdma_init(&e);

	params = (struct brcmstb_s3_params *)addr;
	if (verify_s3_params(params, flags))
		die("S3 params verification failed");

	if (verify_s3_memory(params, flags))
		die("S3 memory verification failed");

	sec_mitch_check();

	AON_REG(AON_REG_MAGIC_FLAGS) = restore_val;

	fsbl_clear_reset_history();

	__puts("OS reentry @ ");
	writehex(params->reentry);
	puts("");

#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		/*
		 * Without memory verification, we also skip the anti-glitch
		 * checks and jump straight to the OS re-entry point
		 */
		void (*reentry)();

		reentry = (void (*)())(uintptr_t)params->reentry;
		(*reentry)();
		/* Should not return */
	}
#endif

	if ((uintptr_t)anti_glitch_e > (uintptr_t)anti_glitch_d) {
		glitch_addr = (uint32_t)params->reentry / 2;
		glitch_trace = BDEV_RD(BCHP_HIF_TOP_CTRL_SCRATCH);

		anti_glitch_e();
	}
	handle_boot_err(ERR_GLITCH_TRACE_CHECK);
}
#endif /* CFG_PM_S3 */

/* Need to clear the DDR PHY standby signal for both cold and warm boot */
bool must_resume_ddr_phys(void)
{
	return reset_history_is_s3();
}

void fsbl_resume_ddr_phys(void)
{
#ifdef BCHP_AON_CTRL_PM_CTRL_pm_dphy_standby_clear_SHIFT
	/* Bring DDR PHYs out of deep standby */
	BDEV_WR_F(AON_CTRL_PM_CTRL, pm_dphy_standby_clear, 1);
#endif
}
