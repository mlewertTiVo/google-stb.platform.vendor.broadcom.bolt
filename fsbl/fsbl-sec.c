/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bchp_aon_ctrl.h"
#include "bchp_bsp_glb_control.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_ctrl.h"
#include "bchp_cntcontrolbase.h"
#include "bchp_hif_top_ctrl.h"
#include "fsbl.h"
#include "fsbl-pm.h"
#include "error.h"

#include "boot_defines.h"

#ifndef CFG_EMULATION

uint32_t sec_get_aon_boot_status(int reg_idx_shift)
{
	uint32_t status;

	status = (REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) >> reg_idx_shift);
	return (status & BOOT_IMAGE_STATUS_MASK);
}

void sec_set_aon_boot_status(int reg_idx_shift, int success)
{
	uint32_t status;

	status = REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH);
	status &= ~(BOOT_STATUS_MASK << reg_idx_shift);
	if (success)
		status |= (BOOT_STATUS_SUCCESS << reg_idx_shift);
	else
		status |= (BOOT_STATUS_FAIL << reg_idx_shift);
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) = status;
}

void sec_handle_boot_status(int aon_reg_shift, int success)
{
	sec_set_aon_boot_status(aon_reg_shift, success);

	if (!success)
		handle_boot_err(ERR_BAD_IMAGE);
}

image_info g_info;

int get_image_info(image_info *info)
{
	if (info == NULL)
		return BOLT_ERR;

	memcpy(info, &g_info, sizeof(g_info));
	return BOLT_OK;
}

int set_image_info(image_info *info)
{
	if (info == NULL)
		return BOLT_ERR;

	memcpy(&g_info, info, sizeof(g_info));
	return BOLT_OK;
}

int select_image(image_info *info)
{
	uint32_t disaster;
	uint32_t boot_status;
	uint32_t __maybe_unused *image_ptr = 0;
	uint32_t __maybe_unused *part_ptr = 0;
	uint32_t __maybe_unused *ctrl_ptr = 0;
	uint32_t image_offset = 0;
	uint32_t part_offset = 0;
	uint32_t ctrl_offset = 0;
	uint32_t aon_value = 0;
	uint32_t boot_status_shift = 0;
	char     *image_name_ptr = NULL;

	/* Use just one AON register to store the status.
	 * First byte has the status of BFW.
	 * Second byte has the status of AVS.
	 * Third byte has the status of MEMSYS.
	 */
	switch (info->image_type) {
	case IMAGE_TYPE_BFW:
			image_name_ptr = "BFW:";
			image_offset = PARAM_1ST_BFW_PART_OFFSET;
			part_offset = PARAM_1ST_BFW_PART;
			ctrl_offset = PARAM_1ST_BFW_CTRL;
			aon_value = AON_FW_TYPE_BFW;
			boot_status_shift = BOOT_BFW_STATUS_SHIFT;
			break;
	case IMAGE_TYPE_AVS:
			image_name_ptr = "AVS:";
			image_offset = PARAM_1ST_AVS_PART_OFFSET;
			part_offset = PARAM_1ST_AVS_PART;
			ctrl_offset = PARAM_1ST_AVS_CTRL;
			aon_value = AON_FW_TYPE_AVS;
			boot_status_shift = BOOT_AVS_STATUS_SHIFT;
			break;
#if CFG_ZEUS4_2
	case IMAGE_TYPE_MEMSYS:
			image_name_ptr = "MEMSYS:";
			image_offset = PARAM_1ST_MEMSYS_PART_OFFSET;
			part_offset = PARAM_1ST_MEMSYS_PART;
			ctrl_offset = PARAM_1ST_MEMSYS_CTRL;
			aon_value = AON_FW_TYPE_MEMSYS;
			boot_status_shift = BOOT_MEMSYS_STATUS_SHIFT;
			break;
#endif
	default:
			die("unknown image type");
			break;
	}
	image_ptr = (uint32_t *)(SRAM_ADDR+image_offset);
	part_ptr = (uint32_t *)(SRAM_ADDR+part_offset);
	ctrl_ptr = (uint32_t *)(SRAM_ADDR+ctrl_offset);
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &= ~AON_FW_TYPE_MASK;
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |= aon_value;

#ifdef CFG_FULL_EMULATION
	/* no sec in generic emulation */
	disaster = 0;
#else
	/* if fail, set error in its aon register */
	disaster = REG(BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR);
#endif
	boot_status = REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH);
	if ((disaster &
		BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR_DISASTER_RECOVER_MASK) ||
		(boot_status & BOOT_BSP_RESET_MASK)) {
		puts("*** BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR is set.");
		/* reset by BSP, set error to failure causer */
		boot_status = sec_get_aon_boot_status(boot_status_shift);

		/* set bsp reset flag if BSP bootrom clear it */
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |= BOOT_BSP_RESET_FLAG;

		if ((boot_status & BOOT_STATUS_MASK) != BOOT_STATUS_SUCCESS) {
			/* clear status and update it with a new value */
			REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
				~(BOOT_STATUS_MASK << boot_status_shift);
			REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |=
				(BOOT_STATUS_FAIL << boot_status_shift);

#ifndef CFG_FULL_EMULATION
			/* clear disaster flag */
			disaster &= ~BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR_DISASTER_RECOVER_MASK;
			REG(BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR) = disaster;
#endif
			/* clear bsp reset flag */
			REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
						~BOOT_BSP_RESET_MASK;
		}
	}

	boot_status = sec_get_aon_boot_status(boot_status_shift);
	if  (boot_status == BOOT_STATUS_2_FAIL) {
		__puts(image_name_ptr);
		writehex(boot_status);
		puts("");
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
			~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
		die("Boot 2nd image failed");
	}

#if (CFG_ZEUS4_2 || CFG_ZEUS4_1)
	/* select 2nd image if boot 1st image failed or
	 * 1st image does not exist
	 */
	if ((boot_status == BOOT_STATUS_2_SUCCESS) ||
		(boot_status == BOOT_STATUS_1_FAIL) ||
		(((*ctrl_ptr) & PARAM_CTRL_WORD_ENABLE_MASK) == 0)) {
		boot_status = (boot_status & ~BOOT_IMAGE_MASK) | BOOT_2ND_IMAGE;
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
			~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |=
			(boot_status << boot_status_shift);

		if (info->image_type == IMAGE_TYPE_BFW) {
			image_ptr = (uint32_t *) (SRAM_ADDR+
				PARAM_2ND_BFW_PART_OFFSET);
			part_ptr = (uint32_t *) (SRAM_ADDR+PARAM_2ND_BFW_PART);
			ctrl_ptr = (uint32_t *) (SRAM_ADDR+PARAM_2ND_BFW_CTRL);
		} else if (info->image_type == IMAGE_TYPE_AVS) {
			image_ptr = (uint32_t *) (SRAM_ADDR+
				PARAM_2ND_AVS_PART_OFFSET);
			part_ptr = (uint32_t *) (SRAM_ADDR+PARAM_2ND_AVS_PART);
			ctrl_ptr = (uint32_t *) (SRAM_ADDR+PARAM_2ND_AVS_CTRL);
		}
#if CFG_ZEUS4_2
		else {
			image_ptr = (uint32_t *)(SRAM_ADDR+
				PARAM_2ND_MEMSYS_PART_OFFSET);
			part_ptr = (uint32_t *) (SRAM_ADDR+
				PARAM_2ND_MEMSYS_PART);
			ctrl_ptr = (uint32_t *) (SRAM_ADDR+
				PARAM_2ND_MEMSYS_CTRL);
		}
#endif
	}
	else {
		boot_status = (boot_status & ~BOOT_IMAGE_MASK) | BOOT_1ST_IMAGE;
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
			~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |=
			(boot_status << boot_status_shift);
	}

	info->addr_offset = *image_ptr;
	info->flash.part_offs = *part_ptr;
	/* partition size is in 1k size. Change it to byte */
	info->flash.part_size = ((*ctrl_ptr & PARAM_PART_SIZE_MASK)
		>> PARAM_PART_SIZE_SHIFT)*1024;
	info->flash.cs = (*ctrl_ptr & PARAM_IMAGE_CS_MASK)
		>> PARAM_IMAGE_CS_SHIFT;
	info->ctrl_word = *ctrl_ptr;
#else
	if (info->image_type == IMAGE_TYPE_BFW)
		info->addr_offset = BFW2_IMAGE_FLASH_OFFSET;
	else if (info->image_type == IMAGE_TYPE_AVS)
		info->addr_offset = AVS2_IMAGE_FLASH_OFFSET;
	else
		info->addr_offset = MEMSYS2_IMAGE_FLASH_OFFSET;

	info->flash.part_size = 0;
	info->flash.part_offs = 0;
	info->flash.cs = 0;
#endif

	/* set aon status to booting */
	boot_status = (boot_status & ~BOOT_STATUS_MASK) | BOOT_STATUS_BOOTING;
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
		~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH)
		|= (boot_status << boot_status_shift);

	__puts("select_image: addr_offset: ");
	writehex((unsigned long) info->addr_offset);
	__puts(" part_offset: ");
	writehex((unsigned long) info->flash.part_offs);

	__puts(" bootStatus: ");
	writehex(REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH));
	puts("");

	return 0;
}

void sec_config_print(void)
{
#ifdef BFW_LOAD
	puts("BFW");
#endif
#ifdef SECURE_BOOT
	puts("SEC");
#endif
	sec_print_version();

	__puts("BBL v");
	sec_print_bsp_version();
}

void sec_init(void)
{
	uint32_t otp_val, rval;
	uint32_t  *dst;
	int __maybe_unused ret;

#if CFG_ZEUS4_2 && !defined(CFG_FULL_EMULATION)
	if ((REG(BCHP_BSP_GLB_CONTROL_FW_FLAGS)
		& 0x0f000000) == 0x07000000) {
		if (sec_memsys_ready())
			die("!MR");
	}
#endif

	rval = sec_read_otp_bit(OTP_BSECK_ENABLE_BIT, &otp_val);
	if (rval)
		die("OTP read failed");

	dst = (uint32_t *) (BOOT_PARAMETER_OFFSET+SRAM_ADDR);
	/* if bseck is enabled, then get the boot params from BSP */
	if (CFG_ZEUS4_2 && otp_val) {
		if (sec_get_bootparam(dst)) /* get boot params from BSP */
			die("Get boot param failed");
		return;
	}

	ret = load_from_flash(dst, BOOT_PARAMETER_OFFSET, BOOT_PARAMETER_SIZE);
	if (ret < 0)
		die("boot parameter read failure");

#if CFG_ZEUS4_1
	dst = (uint32_t *) (PARAM_SSBL_SIZE+SRAM_ADDR);
	ret = load_from_flash(dst, PARAM_SSBL_SIZE, 8);
	if (ret < 0)
		die("boot parameter read failure");
#endif
}

void sec_bfw_load(bool warm_boot)
{
#ifdef BFW_LOAD
	uint32_t ret;
#if (CFG_ZEUS4_2 || CFG_ZEUS4_1)
	uint32_t *page_table = 0;
	uint32_t bfw_buffer_addr;
#endif

	g_info.image_type = IMAGE_TYPE_BFW;
	if (select_image(&g_info))
		die("no BFW image");

#if (CFG_ZEUS4_2 || CFG_ZEUS4_1)
	if (g_info.flash.cs == 1 || BFW_USE_EMMC_DATA_PART == 1) {
		__puts("using page list, ");
#if CFG_PM_S3
		if (warm_boot) {
			bfw_buffer_addr = BFW_WARMBOOT_BUFFER_ADDR;
			puts("warm boot");
		}
		else
#endif
		{
			bfw_buffer_addr = BFW_COLDBOOT_BUFFER_ADDR;
			puts("cold boot");
		}
		page_table = (uint32_t *)bfw_buffer_addr;
		*page_table = 192;
		*(page_table+1) = 0;
		*(page_table+2) = 0;
		*(page_table+3) = bfw_buffer_addr + BFW_PAGELIST_BUFFER_SIZE;
	}

	ret = sec_bfw_load_impl(&g_info, (uint32_t)page_table);
#else
	ret = bseck_reload();
#endif
	sec_handle_boot_status(BOOT_BFW_STATUS_SHIFT, !ret);
#endif

	__puts("BFW v");
	sec_print_bsp_version();
}


#ifdef SECURE_BOOT
void sec_set_memc(struct fsbl_info *info)
{
#if !CFG_ZEUS4_2
	return;
#else
	uint32_t memsys_ctrl;
	struct board_type *b;
	struct ddr_info *ddr;
	int i;

	if ((sec_get_aon_boot_status(BOOT_MEMSYS_STATUS_SHIFT) &
			    BOOT_IMAGE_MASK) == BOOT_1ST_IMAGE)
		memsys_ctrl = DEV_RD(SRAM_ADDR + PARAM_1ST_MEMSYS_CTRL) >>
			PARAM_CTRL_MEMSYS_DISABLE_SHIFT;
	else
		memsys_ctrl = DEV_RD(SRAM_ADDR + PARAM_2ND_MEMSYS_CTRL) >>
			PARAM_CTRL_MEMSYS_DISABLE_SHIFT;

	b = get_tmp_board();

	for (i = 1; i < MAX_DDR; ++i, memsys_ctrl >>= 1) {
		if (memsys_ctrl & 0x01) {
			ddr = &(b->ddr[i]);
			ddr->size_mb = 0;
		}
	}
#endif
}
#endif

#ifndef SECURE_BOOT
int sec_verify_avs(void)
{
	return 0;
}

void sec_verify_memsys(void)
{
#if CFG_ZEUS4_1
	return;
#else
	uint32_t *dst = (uint32_t *)MEMSYS_SRAM_ADDR;
	uint32_t flash_offs = MEMSYS_TEXT_OFFS;
	size_t len = MEMSYS_SIZE;
	struct board_type *b = get_tmp_board();
#if CFG_ZEUS4_2
	image_info info;

	info.image_type = IMAGE_TYPE_MEMSYS;
	if (select_image(&info))
		die("no MemsysFW image");
#endif

	__puts("MEMSYS-");
	if (b->memsys) {
#ifdef MEMC_ALT
		flash_offs = MEMSYS_ALT_TEXT_OFFS;
		len = MEMSYS_ALT_SIZE;
		puts("ALT");
#else
		die("no ALT!");
#endif
	} else
		puts("STD");

	if (load_from_flash(dst, flash_offs, len) < 0)
		die("memsys load failure");
#endif
}

void sec_memsys_region_disable()
{
}

void sec_verify_ssbl(void)
{
}

void sec_lock_memc(int memc)
{
}
#endif

uint32_t sec_avs_select_image(void)
{
	g_info.image_type = IMAGE_TYPE_AVS;
	if (select_image(&g_info))
		die("no AVS image");

	return g_info.addr_offset;
}

/* This function should be called after AVS started */
void sec_avs_set_status(int success)
{
	sec_handle_boot_status(BOOT_AVS_STATUS_SHIFT, success);
}


/* This function should be called after memsys_init */
void sec_memsys_set_status(int success)
{
	sec_handle_boot_status(BOOT_MEMSYS_STATUS_SHIFT, success);
}

void sec_scramble_sdram(bool warm_boot)
{
#ifdef BFW_LOAD
	if (sec_scramble_sdram_impl(warm_boot))
		puts("Sdram Scramble failed!");
#endif
}

void sec_mitch_check(void)
{
#if !defined(S_UNITTEST) && !defined(CFG_FULL_EMULATION)

	__puts("MICH: ");
	/* if blank chip, skip disable_MICH */
	if ((REG(BCHP_BSP_GLB_CONTROL_FW_FLAGS) & 0x0f000000) != 0x0a000000) {
#ifdef BFW_LOAD
		__puts("disable ");
		if (sec_disable_MICH())
			puts("failed");
		else
			puts("success");
#else
		puts("!BFW_LOAD");
#endif
	} else
		puts("security disable");
#endif /* S_UNITTEST */
}


/* ------------------------------------------------------------------------- */
/* Anti-glitch

 Hackers may try to bypass security checks by 'glitching' the code
via power spikes or other methods to bump the program counter
past a security check or to corrupt a (status) register.

 The example code below shows that if the glitch happens
at line #2 then the security check may be bypassed if the
instruction pointer is 'bumped' over that line.

1: result = security_check() <== ENTRYPOINT
2: if(result==bad) goto fail;
3: run_code()

 To counter this, the code always calls to a lower
instruction pointer value (calls backwards.)
 Any glitch to try to bypass a check will
end up at a fail point.

0: run_code()
1: fail() // glitch this!

2: result = security_check_2()
3: if(result==good) goto line 0;
4: fail()

5: result = security_check_1() <== ENTRYPOINT
6: if(result==good) goto line 2;
7: fail()

*/


uint32_t glitch_trace = (uint32_t)die;
uint32_t glitch_addr  = (uint32_t)die; /* obfuscated glitch_entry */
uint32_t glitch_addr1 = (uint32_t)die;
uint32_t glitch_info  = (uint32_t)die;
uint32_t glitch_entry = (uint32_t)die; /*void ()(struct fsbl_info *) */
uint32_t glitch_psci_bootmode = (uint32_t)die;


INITSEG void __noreturn handle_boot_err(uint32_t err_code)
{
	uint32_t value;

	/* log error code into MS 16 bit, and trace value in LS 16 bit */
	value = REG(BCHP_SUN_TOP_CTRL_UNCLEARED_SCRATCH);
	value &= 0x0000FFFF;
	value |= ((uint32_t)(err_code << 16));
	REG(BCHP_SUN_TOP_CTRL_UNCLEARED_SCRATCH) = value;

	if ((err_code <= ERR_WARM_BOOT_FAILED) ||
		(err_code == ERR_S3_PARAM_HASH_FAILED) ||
		(err_code == ERR_S3_DDR_HASH_FAILED)) {
		/* TBD: need to clear warm boot indicator in AON */

		/* reset for ERR_BAD_IMAGE, ERR_WARM_BOOT_FAILED */
		REG(BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE) =
			BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE_sw_master_reset_enable_MASK;

		REG(BCHP_SUN_TOP_CTRL_SW_MASTER_RESET) =
			BCHP_SUN_TOP_CTRL_SW_MASTER_RESET_chip_master_reset_MASK;

		while (1)
			;
	}
	__puts("Boot failed: ");
	writehex(err_code);
	puts("");
	loop_forever();
}


INITSEG void __noreturn fsbl_end(struct fsbl_info *info)
{
	/* Runtime check that the anti-glitch function ordering
	is correct,	before we call into it.
	*/
	if (((uint32_t)anti_glitch_b > (uint32_t)anti_glitch_a) &&
		((uint32_t)anti_glitch_c > (uint32_t)anti_glitch_b) &&
		((uint32_t)handle_boot_err > (uint32_t)anti_glitch_c) &&
		((uint32_t)fsbl_end      > (uint32_t)handle_boot_err)) {
		glitch_info  = (uint32_t)info;
		glitch_trace = 10;

		anti_glitch_c();
	}
	handle_boot_err(ERR_GLITCH_TRACE_CHECK);
}

#endif /* !CFG_EMULATION */
