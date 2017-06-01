/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
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
#include "fsbl-dtu.h"
#include "fsbl-pm.h"
#include "../common/xpt_dma.h"
#include "boot_defines.h"
#include "bchp_hif_top_ctrl.h"
#include "psci.h"

#define MCPB_DW2_LAST_DESC		(1 << 0)

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
	AON_REG(AON_REG_MAGIC_FLAGS) = magic & ~BRCMSTB_S3_MAGIC_MASK;
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

static bool entry_in_mcpb(struct mcpb_dma_desc *head, unsigned int max_descs,
			uintptr_t entry_addr)
{
	struct mcpb_dma_desc *curr = head;
	uintptr_t dma_start_addr;
	unsigned int count = 0;

	if (!curr)
		return false;

	for (count = 0; count < max_descs; count++, curr++) {
		dma_start_addr = curr->buf_lo;
		dma_start_addr |= shift_left_32((uintptr_t)curr->buf_hi);
		if ((entry_addr >= dma_start_addr) &&
			(entry_addr <= (dma_start_addr + curr->size)))
			return true;

		if (curr->next_offs == MCPB_DW2_LAST_DESC)
			break;
	}

	return false;
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

	report_hex("PM: verify params @ ", region.addr);

	if (params->magic != BRCMSTB_S3_MAGIC)
		return -1;

#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		puts("PM: WARNING: skip params verify");
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
		puts("PM: verify params hash mismatch");
		return -1;
	}

	check_return_val((uint32_t)status, BPHYSADDR(BCHP_HIF_TOP_CTRL_SCRATCH),
			 TRACE_VERIFY_WARM_0_BIT, ERR_S3_PARAM_HASH_FAILED);

	puts("PM: verify OK");

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
	unsigned int maxdescs = IMAGE_DESCRIPTORS_BUFSIZE /
					sizeof(struct mcpb_dma_desc);
	bool entry_found = false;

#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		puts("PM: WARNING: skip memory verify");
		return 0;
	}
#endif

	if (dual_channel)
		desc2 = desc1 + params->desc_offset_2;
	else
		desc2 = 0;

	entry_found = entry_in_mcpb((struct mcpb_dma_desc *)pa_to_va(desc1),
					maxdescs, params->reentry);
	entry_found |= entry_in_mcpb((struct mcpb_dma_desc *)pa_to_va(desc2),
					 maxdescs, params->reentry);

	if (!entry_found) {
		puts("PM: re-entry not covered by hash");
		return -1;
	}

	len = mcpb_get_dma_chain_len(
		(struct mcpb_dma_desc *)pa_to_va(desc1), maxdescs);
	if (dual_channel)
		len += mcpb_get_dma_chain_len(
			(struct mcpb_dma_desc *)pa_to_va(desc2), maxdescs);

	report_hex("PM: verify main memory, len ", len);

	if (memdma_run(desc1, desc2, dual_channel))
		return -1;

	get_hash(hash, dual_channel);

	status = 1234; /* security anti-glitch trace status */
	status = s3_compare_hash(params->hash, hash);
	if (status) {
		puts("PM: verify memory mismatch");
		return -1;
	}

	check_return_val(status, BPHYSADDR(BCHP_HIF_TOP_CTRL_SCRATCH),
			 TRACE_VERIFY_WARM_1_BIT, ERR_S3_DDR_HASH_FAILED);

	puts("PM: verify OK");

	return 0;
}

void fsbl_finish_warm_boot(uint32_t restore_val, unsigned int nddr)
{
	struct brcmstb_s3_params *params;
	uintptr_t addr;
	uint32_t flags;
	extern uint32_t glitch_addr, glitch_trace;
	const struct memdma_initparams e = {sys_die, udelay, memset, NULL};
#ifdef STUB64_START
	uint32_t psci_base = (uint32_t)sys_die;
#endif

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
	if (verify_s3_params(params, flags)) {
		puts("S3 params verification failed");
		handle_boot_err(ERR_S3_PARAM_HASH_FAILED);
	}

	dtu_enable(nddr);

#ifdef STUB64_START
       /*
	* After params verify, check PSCI base is valid.
	*/
	if (flags & S3_FLAG_PSCI_BOOT) {
		psci_base = AON_REG(AON_REG_PSCI_BASE);
		if (0 == psci_base) /* Should have been setup in SSBL */
			handle_boot_err(ERR_S3_BAD_PSCI_BASE);

		if (PSCI_BASE != psci_base) {
			report_hex("@MVBAR: ", PSCI_BASE);
				if (!a53_bootmode())
					setup_mvbar(psci_base);
			report_hex(" -> ", psci_base);
		}
	}
#endif
	if (verify_s3_memory(params, flags)) {
		puts("S3 memory verification failed");
		handle_boot_err(ERR_S3_DDR_HASH_FAILED);
	}

	sec_mitch_check();

	AON_REG(AON_REG_MAGIC_FLAGS) = restore_val;

	fsbl_clear_reset_history();

	__puts("OS reentry @ ");
#ifndef SECURE_BOOT
	if (!fsbl_pm_mem_verify(flags)) {
		void (*reentry)();

		/*
		 * Without memory verification, we skip the anti-glitch
		 * checks and jump (or jump via a psci call) straight to
		 * the OS re-entry point.
		 */
#ifdef STUB64_START
		if (flags & S3_FLAG_PSCI_BOOT)
			reentry = (void (*)())(uintptr_t)psci_base;
		else
#endif
			reentry = (void (*)())(uintptr_t)params->reentry;

		writehex((uint32_t)reentry);
		puts(" !verif");

		(*reentry)();
		/* Unexpected return. */
		sys_die(DIE_PM_RET_FROM_S3, "RET!"); /* Unexpected return. */
	}
#endif
	if ((uintptr_t)anti_glitch_e > (uintptr_t)anti_glitch_d) {
#ifdef STUB64_START
		if (flags & S3_FLAG_PSCI_BOOT)
			glitch_addr = psci_base / 2;
		else
#endif
			glitch_addr = (uint32_t)params->reentry / 2;

		writehex(glitch_addr * 2);
		puts("");

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
