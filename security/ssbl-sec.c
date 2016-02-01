/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "bchp_common.h"
#include "bchp_bsp_glb_control.h"
#include "bchp_bsp_cmdbuf.h"

#include "common.h"
#include "timer.h"

#include "../fsbl/fsbl.h"
#include "boot_defines.h"

#include "lib_types.h"
#include "lib_printf.h"
#include "lib_malloc.h"
#include "error.h"
#include "ssbl-sec.h"
#include "lib_string.h"

#define CMD_INBUFFER2		(2 * 384) /* 0x300 */
#define CMD_OUTBUFFER2		(3 * 384) /* 0x480 */
#define OTP_SECURE_BOOT_BIT	0x4a
#define BSP_VER_STRLEN		10


static char bsp[BSP_VER_STRLEN];
static uint32_t bsp_version;


static int sec_check_for_IRDY2(void)
{
	uint32_t ready;
	int to = 10;

	while (--to) {
		ready = BDEV_RD_F(BSP_GLB_CONTROL_GLB_IRDY, CMD_IDRY2);
		if (ready)
			break;
		bolt_msleep(1);
	}

	return (ready) ? BOLT_OK : BOLT_ERR_TIMEOUT;
}

static int sec_check_for_OLOAD2(void)
{
	uint32_t oload;
	int to = 10;

	while (--to) {
		oload = BDEV_RD_F(BSP_GLB_CONTROL_GLB_OLOAD2, CMD_OLOAD2);
		if (oload)
			break;
		bolt_msleep(1);
	}

	return (oload) ? BOLT_OK : BOLT_ERR_TIMEOUT;
}


static int sec_do_bsp_cmd_prologue(void)
{
	if (sec_check_for_IRDY2())
		return BOLT_ERR;

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2) = 0x10;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+4) = 0x22;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+8) = 0xABCDEF00;

	return BOLT_OK;
}

static int sec_do_bsp_cmd_epilogue(void)
{
	REG(BCHP_BSP_GLB_CONTROL_GLB_ILOAD2) = 0x1;

	if (sec_check_for_OLOAD2())
		return BOLT_ERR;

	REG(BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS) = 0x00;
	REG(BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) = 0x00;
	return BOLT_OK;
}


const char *sec_get_bsp_version(void)
{
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
