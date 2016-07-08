/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "../fsbl/fsbl.h"
#include "../fsbl/fsbl-pm.h"

#include "error.h"
#include "boot_defines.h"

/* Fake BSL */

void sec_print_version(void)
{
	puts("seclib-fake");
}

void sec_print_bsp_version(void)
{
	puts("0.0");
}

uint32_t sec_memsys_ready(void)
{
	return 0;
}

uint32_t sec_read_otp_bit(uint32_t field, uint32_t *otp_value)
{
	*otp_value = 0;
	return 0;
}

uint32_t sec_scramble_sdram_impl(bool warm_boot)
{
	return 0;
}

uint32_t sec_disable_MICH(void)
{
	return 0;
}

#if (CFG_ZEUS4_2 || CFG_ZEUS4_1)
uint32_t sec_bfw_load_impl(image_info *info, uint32_t pagelist)
#else
uint32_t bseck_reload(void)
#endif
{
	return 0;
}

void sec_print_bsp_debug_info(void)
{
}

uint32_t sec_get_bootparam(uint32_t *param)
{
	*param = BOOT_PARAMETER_OFFSET + SRAM_ADDR;
	return 0;
}


/* END */
