/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "boot_defines.h"

#include <bchp_aon_ctrl.h>
#include <bchp_bsp_cmdbuf.h>
#include <bchp_bsp_glb_control.h>
#include <bchp_common.h>
#include <board.h>
#include <boardcfg.h>
#include <cache_ops.h>
#include <common.h>
#include <console.h>
#include <devfuncs.h>
#include <device.h>
#include <error.h>
#include <iocb.h>
#include <lib_printf.h>
#include <lib_types.h>
#include <timer.h>
#include <ssbl-sec.h>

#ifndef BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR
/* use BCHP_BSP_GLB_CONTROL_GLB_DWNLD_STATUS */
#define BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR \
	BCHP_BSP_GLB_CONTROL_GLB_DWNLD_STATUS
#define BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR_DISASTER_RECOVER_MASK \
	BCHP_BSP_GLB_CONTROL_GLB_DWNLD_STATUS_DISASTER_RECOVER_MASK
#endif

#define TEMP_BUF_SIZE 256
void     sec_print_bsp_version(void);

static int __maybe_unused sec_check_for_IRDY2(void)
{
	return BOLT_OK;
}

static int __maybe_unused sec_check_for_OLOAD2(void)
{
	return BOLT_OK;
}


static int __maybe_unused sec_do_bsp_cmd_prologue(void)
{
	return BOLT_OK;
}

static int __maybe_unused sec_do_bsp_cmd_epilogue(void)
{
	return BOLT_OK;
}

#define CMD_INBUFFER2		(2 * 384) /* 0x300 */
#define CMD_OUTBUFFER2		(3 * 384) /* 0x480 */
int sec_get_random_num(uint32_t *dest, int num)
{
	uint32_t status, *param = (uint32_t *)(BCHP_PHYSICAL_OFFSET +
		BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + CMD_OUTBUFFER2 + 24);

	if (!dest || (num > SEC_MAX_NUM_RANDOM_NUMBERS))
		return BOLT_ERR;

	if (sec_check_for_IRDY2())
		return BOLT_ERR;

	memset(dest, 0, sizeof(uint32_t) * num);

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2) = 0x10;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 +  4) = 0x22;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 +  8) = 0xABCDEF00;

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 + 12) = 0xF255AA0D;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 + 16) = 0x789A000C;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 + 20) = 0;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 + 24) = 0;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2 + 28) =
		sizeof(uint32_t) * num;

	if (sec_do_bsp_cmd_epilogue())
		return BOLT_ERR;

	BARRIER(); /* complete */

	status = REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + CMD_OUTBUFFER2 + 20);
	if (status)
		return BOLT_ERR;

	memcpy(dest, param, sizeof(uint32_t) * num);

	return BOLT_OK;
}

int select_image(image_info *info)
{
#ifdef SECURE_BOOT
	uint32_t __maybe_unused disaster;
#endif
	int res, sfd = 0;
	uint32_t boot_status;
	uint32_t __maybe_unused *image_ptr = 0;
	uint32_t __maybe_unused *part_ptr = 0;
	uint32_t __maybe_unused *ctrl_ptr = 0;
	uint32_t aon_value = 0;
	uint32_t boot_status_shift = 0;
	uint8_t  temp_buf[TEMP_BUF_SIZE];

	/* Use just one AON register to store the status.
	 * First byte has the status of BFW.
	 * Second byte has the status of AVS.
	 * Third byte has the status of MEMSYS.
	 */
	if (info->image_type != IMAGE_TYPE_BFW)
		sys_die(DIE_UNKNOWN_IMAGE_TYPE, "unknown image type");

	if (board_bootmode() == BOOT_FROM_EMMC)
		sfd = bolt_open("flash1");
	else
		sfd = bolt_open("flash0");
	if (sfd < 0) {
		xprintf("bfw:cannot open flash\n");
		return 1;
	}

	res = bolt_readblk(sfd, (bolt_offset_t)SEC_PARAM_OFFSET_FLASH,
		temp_buf, TEMP_BUF_SIZE);
	if (res != TEMP_BUF_SIZE) {
		xprintf("bfw:cannot read flash\n");
		bolt_close(sfd);
		return 1;
	}

	aon_value = AON_FW_TYPE_BFW;
	boot_status_shift = BOOT_BFW_STATUS_SHIFT;

	image_ptr = (uint32_t *)(temp_buf+PARAM_2ND_BFW_PART_OFFSET);
	part_ptr = (uint32_t *)(temp_buf+PARAM_2ND_BFW_PART);
	ctrl_ptr = (uint32_t *)(temp_buf+PARAM_2ND_BFW_CTRL);
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &= ~AON_FW_TYPE_MASK;
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |= aon_value;

	boot_status = REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH);
#ifdef SECURE_BOOT
#ifdef CFG_FULL_EMULATION
	/* no sec in generic emulation */
	disaster = 0;
#else
	/* if fail, set error in its aon register */
	disaster = REG(BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR);
#endif

	if ((disaster &
		BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR_DISASTER_RECOVER_MASK) ||
		(boot_status & BOOT_BSP_RESET_MASK)) {
		xprintf("*** BCHP_BSP_GLB_CONTROL_GLB_DWNLD_ERR is set.");
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
		xprintf("bfw: %x", boot_status);
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
			~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
		sys_die(DIE_BOOT_2ND_IMAGE_FAILED, "Boot 2nd image failed");
	}

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

		image_ptr = (uint32_t *) (temp_buf+PARAM_2ND_BFW_PART_OFFSET);
		part_ptr = (uint32_t *) (temp_buf+PARAM_2ND_BFW_PART);
		ctrl_ptr = (uint32_t *) (temp_buf+PARAM_2ND_BFW_CTRL);
	}
	else {
		boot_status = (boot_status & ~BOOT_IMAGE_MASK) | BOOT_1ST_IMAGE;
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
			~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
		REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) |=
			(boot_status << boot_status_shift);
		image_ptr = (uint32_t *) (temp_buf+PARAM_1ST_BFW_PART_OFFSET);
		part_ptr = (uint32_t *) (temp_buf+PARAM_1ST_BFW_PART);
		ctrl_ptr = (uint32_t *) (temp_buf+PARAM_1ST_BFW_CTRL);
	}
#else
	boot_status = (boot_status & ~BOOT_IMAGE_MASK) | BOOT_2ND_IMAGE;
#endif

	info->addr_offset = *image_ptr;
	info->flash.part_offs = *part_ptr;
	/* partition size is in 1k size. Change it to byte */
	info->flash.part_size = ((*ctrl_ptr & PARAM_PART_SIZE_MASK)
		>> PARAM_PART_SIZE_SHIFT)*1024;
	info->flash.cs = (*ctrl_ptr & PARAM_IMAGE_CS_MASK)
		>> PARAM_IMAGE_CS_SHIFT;
	info->ctrl_word = *ctrl_ptr;

	/* set aon status to booting */
	boot_status = (boot_status & ~BOOT_STATUS_MASK) | BOOT_STATUS_BOOTING;
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) &=
		~(BOOT_IMAGE_STATUS_MASK << boot_status_shift);
	REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH)
		|= (boot_status << boot_status_shift);

	xprintf("BFW: addr_offset: %x", (unsigned int) info->addr_offset);
	xprintf(" part_offset: %x", (unsigned int) info->flash.part_offs);
	xprintf(" bootStatus: %x\n", REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH));

	bolt_close(sfd);
	return 0;
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

uint32_t sec_get_aon_boot_status(int reg_idx_shift)
{
	uint32_t status;

	status = (REG(BCHP_AON_CTRL_UNCLEARED_SCRATCH) >> reg_idx_shift);
	return (status & BOOT_IMAGE_STATUS_MASK);
}


void sec_handle_boot_status(int aon_reg_shift, int success)
{
	sec_set_aon_boot_status(aon_reg_shift, success);

	if (!success)
		sys_die(DIE_NO_BFW_IMAGE, "bad BFW image");
}
void __noreturn sys_die(const uint16_t die_code, const char *die_string);

#ifndef SECURE_BOOT
void __noreturn sys_die(const uint16_t die_code, const char *die_string)
{
	xprintf("FATAL ERROR: %x;%s", die_code, die_string);
	while (1)
		;
}
#endif

/* bfw_main -- entry point */
void bfw_main(void)
{
	uint32_t ret;
	image_info img_info;

	img_info.image_type = IMAGE_TYPE_BFW;
	if (select_image(&img_info))
		sys_die(DIE_NO_BFW_IMAGE, "no BFW image");

	ret = sec_bfw_load_impl(&img_info, BFW_RAM_BASE);
	if (ret)
		sys_die(DIE_NO_BFW_IMAGE, "no BFW image");

	clear_all_d_cache();

	ret = sec_bfw_verify(BFW_RAM_BASE);
	sec_handle_boot_status(BOOT_BFW_STATUS_SHIFT, !ret);

	xprintf("BFW: ");
	sec_print_bsp_version();
}
