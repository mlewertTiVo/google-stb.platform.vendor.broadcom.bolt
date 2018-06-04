/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BOARDCFG_H__
#define __BOARDCFG_H__

#define BOOT_DEV_ERROR	(0)
#define BOOT_FROM_NAND	(1)
#define BOOT_FROM_NOR	(2)
#define BOOT_FROM_SPI	(3)
#define BOOT_FROM_EMMC	(4)

/* Can't rely on the RDB for these, and we force consistency.
*/
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_eMMC_MASK		0b11110
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_MASK		0b11000
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_RESERVED_MASK	0b11111
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_PNOR_MASK		0b11100

#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_NAND_VALUE		0b10111
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_VALUE		0b11011
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_PNOR_VALUE		0b11101
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_eMMC_VALUE		0b11110
/* On 7445d0+ */
#define SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_4B_VALUE	0b11111

unsigned int boardcfg_bootmode(void);

#endif /* __BOARDCFG_H__ */
