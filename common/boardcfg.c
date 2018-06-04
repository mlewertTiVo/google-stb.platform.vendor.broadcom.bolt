/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>
#include <boardcfg.h>
#include <lib_physio.h>

#include <stdint.h>

unsigned int boardcfg_bootmode(void)
{
	uint32_t boot_strap_val;

	boot_strap_val = BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0);

	boot_strap_val =
		(boot_strap_val &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK) >>
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

	if (boot_strap_val <=
	    SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_NAND_VALUE)
		return BOOT_FROM_NAND;
	else if (boot_strap_val <=
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_VALUE)
		return BOOT_FROM_SPI;
	else if (boot_strap_val <=
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_PNOR_VALUE)
		return BOOT_FROM_NOR;
	else if (boot_strap_val ==
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_eMMC_VALUE)
		return BOOT_FROM_EMMC;
	else if (boot_strap_val ==
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_4B_VALUE)
		return BOOT_FROM_SPI;
	else
		return BOOT_DEV_ERROR;
}

