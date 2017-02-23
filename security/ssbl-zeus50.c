/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "boot_defines.h"

#include <bchp_common.h>
#include <bchp_bsp_cmdbuf.h>
#include <bchp_bsp_glb_control.h>
#include <common.h>
#include <error.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_types.h>
#include <timer.h>

#include "ssbl-sec.h"
#include "lib_string.h"

#define CMD_INBUFFER2		(2 * 384) /* 0x300 */
#define CMD_OUTBUFFER2		(3 * 384) /* 0x480 */
#define OTP_SECURE_BOOT_BIT	0x4a
#define BSP_VER_STRLEN		10


static char bsp[BSP_VER_STRLEN];
static uint32_t bsp_version;

/* TODO: 7278a0
 *  Deleted BSP_GLB_CONTROL_GLB_IRDY
 *  Deleted BSP_GLB_CONTROL_GLB_ORDY
 *  Deleted BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS
 *  Deleted BSP_GLB_CONTROL_GLB_HOST_INTR_EN
 *  Deleted BSP_GLB_CONTROL_GLB_RAAGA_INTR_STATUS
 *  Deleted BSP_GLB_CONTROL_GLB_OLOAD2
 *  Deleted BSP_GLB_CONTROL_GLB_OLOAD1
 *  Deleted BSP_GLB_CONTROL_GLB_JTAG_DEBUG_CTRL
 *  Deleted BSP_GLB_CONTROL_GLB_DWNLD_ERR
 *  Deleted BSP_GLB_CONTROL_GLB_OLOAD_CR
 *
 *  Added BSP_GLB_CONTROL_GLB_ORDY2
 *  Added BSP_GLB_CONTROL_GLB_IRDY1
 *  Added BSP_GLB_CONTROL_GLB_IRDY_CR
 *  Added BSP_GLB_CONTROL_GLB_ORDY_CR
 */

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


const char *sec_get_bsp_version(void)
{
#ifndef CFG_FULL_EMULATION
	if (bsp_version)
		return bsp;

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_OUTBUFFER2) = 0;
	BARRIER();

	if (sec_do_bsp_cmd_prologue())
		return NULL;

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+12) = 0xE455AA1B;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+16) = 0x789A0004;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+20) =
		OTP_SECURE_BOOT_BIT;

	if (sec_do_bsp_cmd_epilogue())
		return NULL;

	/* First word of output buffer contains the version number */
	bsp_version = REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_OUTBUFFER2);

	xsprintf(bsp, "%d.%d.%d",
		(bsp_version >> BSP_S_FW_MAJOR_VERSION_SHIFT) & 0x3F,
		(bsp_version >> BSP_S_FW_MINOR_VERSION_SHIFT) & 0x3F,
		(bsp_version >> BSP_S_FW_SUB_MINOR_VERSION_SHIFT) & 0x0F);
#endif
	return (bsp_version) ? bsp : NULL;
}

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

uint32_t sec_enable_debug_ports(void)
{
	return BOLT_OK;
}
