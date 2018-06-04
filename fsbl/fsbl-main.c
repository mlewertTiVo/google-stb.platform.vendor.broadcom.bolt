/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <aon_defs.h>
#include <arch-mmu.h>
#include <avs.h>
#include <avs_temp_reset.h>
#include <bchp_aon_ctrl.h>
#include <bchp_common.h>
#include <bchp_sun_gisb_arb.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_timer.h>
#include <boot_defines.h>
#include <chipid.h>
#include <common.h>
#include <ddr.h>
#include <fsbl.h>
#include <fsbl-clock.h>
#include <fsbl-dtu.h>
#include <fsbl-hacks.h>
#include <fsbl-pm.h>
#include <lib_types.h>
#include <mmap-dram.h>

#include <stdbool.h>

#ifdef CFG_EMULATION
extern void jumpto(uint32_t address);
#endif

#ifndef CFG_EMULATION
static void set_other_info(struct fsbl_info *info);
static const struct ddr_info *find_ddr(uint32_t memc);
#endif


/* remove cores out of reset one by one to reduce power surge */
void late_release_resets(void)
{
	int i;

	for (i = 0; i < 32; i++) {
		BDEV_WR(BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, 1 << i);
		BARRIER();
		udelay(10);
		BDEV_WR(BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR, 1 << i);
		BARRIER();
		udelay(10);
	}
}

void fsbl_print_version(int vermax, int vermin)
{
	__puts("v");
	writeint(vermax);
	__puts(".");
	if (vermin < 10)
		__puts("0");
	writeint(vermin);
	puts("");
}

static void fsbl_banner(void)
{
	report_hex("BCM", BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID));
	report_hex("PRID", BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID));

	fsbl_print_version(BOLT_VER_MAJOR, BOLT_VER_MINOR);

	report_hex("RR:", BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY));

	if (BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY) &
	    BCHP_AON_CTRL_RESET_HISTORY_security_master_reset_MASK)
		sec_print_bsp_debug_info();

	sec_config_print();
}

#ifndef CFG_EMULATION
static void fsbl_setup_mmu(struct board_type *b)
{
	unsigned long size;

	mmu_initialize_pagetable();

	/* add PTE's from the base of MEMC#0 to the end of to-be-coped SSBL */
	size = SSBL_RAM_ADDR - _MB(dram_mapping_table[0].to_mb) + SSBL_SIZE;
	mmu_add_pages(_MB(dram_mapping_table[0].to_mb), size);
	/* TODO: What if 'size' is smaller than the installed memory on MEMC#0?
	 * The following code might be added for checking such a condition:
	 *      if (size > _MB(b->ddr[0].size_mb))
	 *             puts("Too small amount of memory on MEMC#0");
	 * But, such a check would not be needed because copying SSBL code
	 * to DRAM will fail anyway.
	 */

	/* Modify the very first table entry so we can catch
	 * such things as NULL pointer exceptions.
	 */
	set_guard_page((uint32_t *)SSBL_PAGE_TABLE);

	mmu_enable();
}
#endif

/* Decide whether to boot Rescue Loader or not */
int rescue_loader_needed(void)
{
	/* OEM must implement this function */
	return 0;
}

/* Change SSBL parameters to select Rescue Loader instead */
void select_rescue_loader(void)
{
	/* Overwrite SSBL parameters */
	*((uint32_t *)(SRAM_ADDR + PARAM_SSBL_PART_OFFSET)) = RESCUE_LOADER_PART_OFFSET;
	*((uint32_t *)(SRAM_ADDR + PARAM_SSBL_SIZE)) = RESCUE_LOADER_SIZE;
	*((uint32_t *)(SRAM_ADDR + PARAM_SSBL_CTRL)) = RESCUE_LOADER_CTRL;
	*((uint32_t *)(SRAM_ADDR + PARAM_SSBL_PART)) = RESCUE_LOADER_PART;

	/* OEM may want to set some status in AON to indicate that the
	Rescue Loader will be run */
}

bool fsbl_set_srr(struct board_type *b, struct fsbl_info *info)
{
	static const uint32_t MAX_SRR_SIZE_MB = 255;
	uint32_t size_mb;
	uint32_t offset_mb = 0;

	if (b == NULL || info == NULL)
		return false;

	if (USER_SRR_SIZE_MB < 0)
	/* retrieve the required size of SRR */
		size_mb = *((uint32_t *)(SRAM_ADDR + PARAM_SRR_SIZE_MB));
	else
		size_mb = USER_SRR_SIZE_MB;

	if (size_mb > MAX_SRR_SIZE_MB) {
		size_mb = 0;
		offset_mb = 0;
	}

	/* find a suitable location */
	if (size_mb != 0) {
#if CFG_ZEUS4_2
		uint32_t dram0_size_mb =
			ddr_get_restricted_size_mb(&(b->ddr[0]));

		if (dram0_size_mb > size_mb) {
			offset_mb = dram0_size_mb - size_mb;
		} else {
			size_mb = 0;
			offset_mb = 0;
		}
#else
		size_mb = 0;
		offset_mb = 0;
#endif
	}

	/* set ARCH, otherwise SAGE will be unhappy */
	if (size_mb != 0) {
		if (sec_set_arch(_MB(offset_mb), _MB(size_mb))) {
			__puts("ARCH: ");
			writeint(size_mb);
			__puts("@");
			writeint(offset_mb);
			puts(" failed");
			info->srr_size_mb = 0;
			info->srr_offset_mb = 0;
			return false;
		}
	}

	/* pass the information to SSBL so that it is passed up to Linux */
	info->srr_size_mb = (uint16_t) size_mb;
	info->srr_offset_mb = (uint16_t) offset_mb;
	return true;
}

static void bp3_apply_otp(void)
{
#ifdef BCHP_SUN_TOP_CTRL_IP_DISABLE_UNLOCK
	uint32_t bp3_enable, rval;

	rval = sec_read_otp_bit(OTP_IP_LICENSING_CHECK_ENABLE_BIT, &bp3_enable);
	if (rval)
		sys_die(DIE_OTP_READ_FAILED, "OTP read failed");

	/* adjust IP_DISABLE_xxx based on read OTP bit */
	if (bp3_enable) {
		bcm7255_enable_qam();
		bcm7278_disable_hdcp();
		BDEV_WR(BCHP_SUN_TOP_CTRL_IP_DISABLE_UNLOCK, 0);
	} else {
		BDEV_WR(BCHP_SUN_TOP_CTRL_IP_DISABLE_0, 0);
		bcm7278_disable_hdcp();
	}
#endif
	bcm7260b0_bp3_apply_otp();
}

void fsbl_main(void)
{
#ifndef CFG_EMULATION
	int avs_err = AVS_DISABLED;
	struct fsbl_info info, *p = (struct fsbl_info *)SSBL_BOARDINFO;
	struct board_nvm_info *pnvm;
	struct board_type *b;
	physaddr_t b_ddr;
	int do_shmoo_menu, avs_hf, avs_en;
	unsigned int pmap_id = 0;
	uint32_t restore_val;
	int en_avs_thresh;
#ifndef SECURE_BOOT
	int bypass_avs;
#endif
	uint32_t mhl_power;
#endif
	int i;
	bool warm_boot;
	__maybe_unused bool resume_ddr_phys;
	bool is_srr_allocated = false;

	late_cpu_init();
	late_release_resets();
	uart_init(BCHP_UARTA_REG_START);
	hack_power_down_cpus();
#if !CFG_UNCACHED
	i_cache_config(1);
#endif

	/* Must resume PHYs even for cold boot */
	resume_ddr_phys = must_resume_ddr_phys();
	warm_boot = fsbl_ack_warm_boot();

	if (!warm_boot) {
		fsbl_banner();

		/* after power cycle, clean all AON registers */
		if (REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) == 0)
			for (i = 0; i <=
				BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_END; ++i)
				AON_REG(i) = 0;
		bcm7260a0_patch_mpm();
	}

	hack_lpddr_reg();
	orion_hack_early_bus_cfg();

#ifndef CFG_EMULATION
	/* arb_timer / 108x10^6 = timeout in seconds */
	BDEV_WR(BCHP_SUN_GISB_ARB_TIMER, 0x278d0); /* 1.5 ms */
	BDEV_WR(BCHP_SUN_GISB_ARB_REQ_MASK, 0);

	avs_init(); /* initialize the AVS hardware */

	/* Must be called before loading anything else from flash */
	get_flash_info(&info);

	sec_init(); /* send out memsys_ready_for_init and get boot parameter */

#if (CFG_ZEUS_VERSION <= 0x410)
	sec_bfw_load(warm_boot);
#endif

#ifndef SECURE_BOOT
	bypass_avs = ('8' == getc());
	if (!bypass_avs)
#endif
		avs_err = avs_load();

	/* Load the basic runtime configuration. Must be done
	before we load the boards as that checks this nvm data. */
	pnvm = nvm_load();

	/* Local copy, so memory @ pnvm can be discarded.
	NOTE: if you modify, check sizeof(*pnvm)%sizeof(unit32_t)
	*/
	memcpy4((uint32_t *)&(info.saved_board),
		(uint32_t *)pnvm, sizeof(*pnvm));

	/* This has to happen before board_select() and
	 * read_mhl_power_config() in case those set
	 * any 'runflags'
	 */
	set_other_info(&info);

#ifdef STUB64_START
	/* If we booted in A64 mode then the smc vector has already been
	 * installed by A64 stub code, else we're in 32 bit 'svc' mode
	 * rather than non-secure 'svc_ns' mode and have to set up the
	 * monitor mode smc vector here for 32 bit only operation.
	 */
	if (a53_bootmode())
		info.runflags |= FSBL_RUNFLAG_A64_BOOT;
	else
		(void)setup_mvbar(PSCI_BASE);
#endif
	/* If MHL is present then get the Amperage of any
	 * power negotiated by the MPM firmware.
	 * return value is one of FSBL_RUNFLAG_MHL_BOOT_
	 * enums, with 0 = no MHL boot.
	 */
	mhl_power = read_mhl_power_config(&info);

	/* We cannot access board specific info until this function
	is done. The board list	*WILL* go bye-bye if we load
	SHMOO MCBs! NB: Don't use MEMSYS_SRAM_ADDR as it could already
	have been populated. */
	do_shmoo_menu = board_select(&info, SHMOO_SRAM_ADDR);

#ifndef SECURE_BOOT
	pmap_id = FSBL_HARDFLAG_PMAP_ID(info.saved_board.hardflags);
	if (pmap_id == FSBL_HARDFLAG_PMAP_BOARD)
		/* board default PMap ID as it is overridden */
		pmap_id = AVS_PMAP_ID(info.board_types[info.board_idx].avs);
#else /* SECURE BOOT */
#if (CFG_ZEUS_VERSION >= 0x420)
	pmap_id = DEV_RD(SEC_PARAM_START_SRAM + PARAM_AVS_PARAM_1) &
			PARAM_AVS_PARAM_1_PMAP_MASK;
#endif
#endif

	/* CAUTION: load memsys *before* shmoo data. We may
	use FIXed MCB tables, so allow an override. */
	sec_verify_memsys();
	info.shmoo_ver = memsys_load();
	shmoo_load();

#ifdef SECURE_BOOT
	sec_set_memc(&info);
#endif
	b = get_tmp_board();

	__puts("AVS: overtemp mon ");
	if (BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY) &
		BCHP_AON_CTRL_RESET_HISTORY_overtemp_reset_MASK) {
		/* if one of reset causes is overtemp, do not re-enable
		 * at this point so that the overtemp condition does not
		 * reset the chip over and over again.
		 *
		 * As long as OVERTEMP is enabled, it gets re-enabled
		 * in SSBL after SSBL performs temperature parking and
		 * the chip cools down.
		 */
		en_avs_thresh = 0;
	}
	else {
		en_avs_thresh = (info.saved_board.hardflags &
			FSBL_HARDFLAG_OTPARK_MASK) ? 0 : 1;
	}
	avs_set_temp_threshold(STB_DEVICE, en_avs_thresh);
	if (en_avs_thresh)
		puts("ON");
	else
		puts("OFF");

	if (do_shmoo_menu)
		shmoo_menu(&(info.saved_board));

	board_try_patch_ddr(&(info.saved_board));

	/* must be called before shmoo because SCB has to be full frequency */
	adjust_clocks(b, mhl_power);

	avs_hf = (info.saved_board.hardflags >> FSBL_HARDFLAG_AVS_SHIFT) &
		FSBL_HARDFLAG_AVS_MASK;
	avs_en = ((avs_hf == FSBL_HARDFLAG_AVS_BOARD) && AVS_ENABLE(b->avs)) ||
		(avs_hf == FSBL_HARDFLAG_AVS_ON);
#ifndef SECURE_BOOT
	if (!bypass_avs)
#endif
		if (avs_err == AVS_LOADED)
			avs_err = avs_start(avs_en, pmap_id);

	info.avs_err = avs_err;

	if (warm_boot)
		memsys_warm_restart(b->nddr);
	else if (resume_ddr_phys)
		fsbl_resume_ddr_phys();

	for (i = 0; i < (int)b->nddr; i++) {
		const struct ddr_info *ddr;

		__puts("DDR");
		putchar('0' + i);
		ddr = find_ddr(i);
		__puts(": ");
		if (ddr) {
			shmoo_set(ddr, warm_boot);
			puts("OK");
		} else {
			puts("-");
		}
	}

	sec_memsys_region_disable();

	bp3_apply_otp();

	is_srr_allocated = fsbl_set_srr(b, &info);

	sec_scramble_sdram(warm_boot);

	dtu_init(b);

#if (CFG_ZEUS_VERSION >= 0x510)
	dtu_load(b, warm_boot);
#endif

	/*
	 * Only 16 bit of AON_SYSTEM_DATA[0] is used to check s3 warm boot in
	 * BOLT but in ZEUS 4.2 BFW still uses 32 BRCMSTB_S3_MAGIC to to check
	 * S3 warm boot
	 */
	restore_val = AON_REG(AON_REG_MAGIC_FLAGS);

#if (CFG_ZEUS_VERSION >= 0x420)
	if (warm_boot)
		AON_REG(AON_REG_MAGIC_FLAGS) = BRCMSTB_S3_MAGIC;

#if (CFG_ZEUS_VERSION < 0x510)
	sec_bfw_load(warm_boot);
#endif
#endif
	if (warm_boot) {
		fsbl_init_warm_boot(restore_val);
#if (CFG_ZEUS_VERSION >= 0x510)
		dtu_verify(b->nddr);
#endif
	}

#if (CFG_ZEUS_VERSION >= 0x420)
	dtu_load(b, warm_boot);
#endif

	/* if previous set_srr fail, do it again after loading BFW */
	if (!is_srr_allocated)
		fsbl_set_srr(b, &info);

	for (i = 0; i < (int)b->nddr; i++) {
		const struct ddr_info *ddr;

		ddr = find_ddr(i);
		if (ddr)
			sec_lock_memc(i);
	}

#if (CFG_ZEUS4_2 || CFG_ZEUS5_1)
	sec_check_rpmb_access();
#endif

	if (warm_boot)
		fsbl_finish_warm_boot(restore_val);

	fsbl_setup_mmu(b);

	/* Set target address of the board list in DDR
	that we want to copy out to, hopping over the
	'info' struct so we append above it. */
	b_ddr = (physaddr_t)p + sizeof(info);

	/* Reload board types for SSBL (SECURE_BOOT=n)
	 NOTE: The address we load includes the
	struct boards_nvm_list header so the actual
	offset of the struct board_type array is
	given by info.board_types that this function
	calculates as an offset from the load
	address and is *NOT* this b_ddr load address.
	*/
	load_boards(&info, b_ddr);

	report_hex("INFO v", FSBLINFO_VERSION(info.n_boards));

	/* copy into the loaded list of boards our tmp
	board in case its been runtime modified e.g. by
	board_try_patch_ddr() or shmoo_set() */
	memcpy(&(info.board_types[info.board_idx]),
		b, sizeof(struct board_type));

#ifdef SECURE_BOOT
	memcpy((struct board_type *)b_ddr, info.board_types,
		FSBLINFO_N_BOARDS(info.n_boards) * sizeof(struct board_type));

	/* Reset info to point to DDR now. Note that this address
	will be different from the SECURE_BOOT=n case as we've no
	struct boards_nvm_list header to take into account. */
	info.board_types = (struct board_type *)b_ddr;
#endif
	/* finally overwite the loaded 'info' with
	our local, possibly modified, copy. */
	memcpy(p, &info, sizeof(info));

	/* Boot Rescue Loader or not */
	if (rescue_loader_needed())
		select_rescue_loader();

	/*
	 * This function executes the FSBL epilogue and jumps to the SSBL.
	 * Do not add code after this function call!
	 */
	fsbl_end(p);

#else	/* --- CFG_EMULATION --- */
	__puts("JMP ");
	writehex(CFG_EMULATION_JUMP_TARGET);
	puts("");

	jumpto(CFG_EMULATION_JUMP_TARGET);
#endif	/* !CFG_EMULATION */
}


#ifndef CFG_EMULATION
void *copy_code(void)
{
#if CFG_SSBM
	void *dst = (void *)SSBM_RAM_ADDR;
#else
	void *dst = (void *)SSBL_RAM_ADDR;
#endif
#if SSBL_SOFTLOAD
	mmu_disable();
	__puts("softload SSBL @ ");
	writehex(SSBL_RAM_ADDR);
	__puts(", then press any key twice... ");
	getchar();
	getchar();
#else
#if (CFG_ZEUS_VERSION >= 0x410)
	image_info  info;

	info.image_type = IMAGE_TYPE_SSBL;
	select_image(&info);

	__puts("COPY CODE... ");
	if (load_from_flash_ext(dst, info.addr_offset, info.image_size, &(info.flash)) < 0)
		sys_die(DIE_FLASH_READ_FAILURE, "flash read failure");
#else
	__puts("COPY CODE... ");
	if (load_from_flash(dst, SSBL_TEXT_OFFS, SSBL_SIZE) < 0)
		sys_die(DIE_FLASH_READ_FAILURE, "flash read failure");
#endif
#ifdef M64BIN_RAM_ADDR
	if (a53_bootmode()) {
		puts("DONE");

		/* load M64BIN separately in case alternative copy is used */
		__puts("COPY M64BIN... ");
		if (load_from_flash((void *)M64BIN_RAM_ADDR,
			M64BIN_TEXT_OFFS, M64BIN_SIZE) < 0)
			sys_die(DIE_FLASH_READ_FAILURE, "flash read failure");
	}
#endif
#endif
	puts("DONE");

	return dst;
}


static void set_other_info(struct fsbl_info *info)
{
	info->uart_base = (CFG_SSBL_CONSOLE) ? (uint32_t)get_uart_base() : 0;
	info->pte = (uint32_t)SSBL_PAGE_TABLE;
	info->runflags = 0;
#ifdef STUB64_START
	info->psci_base = PSCI_BASE;
#endif
}


static const struct ddr_info *find_ddr(uint32_t memc)
{
	/* Only reference board_types *AFTER*
	 set_other_info() & then board_type_init()!
	 */
	struct board_type *b = get_tmp_board();
	struct ddr_info *ddr;
	unsigned int i;

	if (!b)
		return NULL;

	for (i = 0; i < b->nddr; i++) {
		ddr = &(b->ddr[i]);
		if (!ddr_is_populated(ddr)) /* an unused ddr? */
			continue;
		if (ddr->which == memc)
			return ddr;
	}
	return NULL;
}

#endif /* !CFG_EMULATION */
