/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "bchp_common.h"
#include "bchp_bsp_glb_control.h"
#include "bchp_bsp_cmdbuf.h"

#include "arch_ops.h"

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
#define BSP_VER_STRLEN		10
#define BSP_TIMEOUT_MS		10 /* milliseconds */


#define SEC_CHECK_FOR(reg, field)\
		wait_rdb_reg_bits_set(BCHP_##reg,\
			BCHP_##reg##_##field##_MASK, BSP_TIMEOUT_MS)

#define sec_check_for_IRDY2()\
	SEC_CHECK_FOR(BSP_GLB_CONTROL_GLB_IRDY, CMD_IDRY2)

#define sec_check_for_OLOAD2()\
	SEC_CHECK_FOR(BSP_GLB_CONTROL_GLB_OLOAD2, CMD_OLOAD2)

#define sec_check_for_IRDY1()\
	SEC_CHECK_FOR(BSP_GLB_CONTROL_GLB_IRDY, CMD_IDRY1)

#define sec_check_for_OLOAD1()\
	SEC_CHECK_FOR(BSP_GLB_CONTROL_GLB_OLOAD1, CMD_OLOAD1)


static uint32_t bsp_version;


/*
 * return the number of timer ticks that occur
 * in a given number of milliseconds.
 */
static uint64_t __maybe_unused ticks_per_ms(unsigned int milliseconds)
{
	return (uint64_t)((arch_get_timer_freq_hz() / 1000) * milliseconds);
}


/*
 * Note: The timeout is intended for use very early on in SSBL before
 * the timer infrastructure is up and running. It can be used after that
 * as well, but should be used sparingly as it does not do POLL() or
 * keep the current time value refreshed.
 */
static int __maybe_unused wait_rdb_reg_bits_set(uint32_t reg,
				uint32_t bitmask, unsigned int timeout_ms)
{
	uint32_t val;
	uint64_t baseline, duration, elapsed;

	val = BDEV_RD(reg) & bitmask;

	duration = ticks_per_ms(timeout_ms);
	baseline = arch_getticks64();

	while (0 == val) {
		elapsed = arch_getticks64() - baseline;
		if (elapsed > duration)
			break;
		dmb();
		val = BDEV_RD(reg) & bitmask;
	}

	return (val) ? BOLT_OK : BOLT_ERR_TIMEOUT;
}

static int __maybe_unused sec_do_bsp_cmd_prologue(void)
{
	if (sec_check_for_IRDY2())
		return BOLT_ERR;

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2) = 0x10;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+4) = 0x22;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+CMD_INBUFFER2+8) = 0xABCDEF00;

	return BOLT_OK;
}

static int __maybe_unused sec_do_bsp_cmd_epilogue(void)
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
#ifndef CFG_FULL_EMULATION
	static char bsp[BSP_VER_STRLEN];

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
#else
	return NULL;
#endif
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

uint32_t sec_enable_debug_ports()
{
	uint32_t status;
	uint32_t bfw_major_version;
	const char *s = sec_get_bsp_version();

	if (!s) {
		puts("SEC: Can't get BFW version!");
		return BOLT_ERR;
	}

	bfw_major_version = (bsp_version >>
		BSP_S_FW_MAJOR_VERSION_SHIFT) & 0xf;

	/* Enable debug is supported with BFW 4.x or later */
	if (bfw_major_version < 4)
		return BOLT_ERR;

	sec_check_for_IRDY1();

	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE) = 0x10;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+4) = 0xdb;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+8) = 0xabcdef00;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+12) = 0xBE55AA41;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+16) = 0x789A0004;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+20) = 0x1057A675;
	REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+24) = 0x0;  /* maybe not needed */

	REG(BCHP_BSP_GLB_CONTROL_GLB_ILOAD1) = 0x1;

	sec_check_for_OLOAD1();

	REG(BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS) = 0x00;
	REG(BCHP_BSP_GLB_CONTROL_GLB_OLOAD1) = 0x00;

	__puts("SEC: jtag debug status: ");
	status = REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x194);
	writehex(status);
	puts("");

	return (status);
}
