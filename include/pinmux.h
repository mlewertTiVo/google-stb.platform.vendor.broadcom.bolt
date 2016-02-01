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

#ifndef __PINMUX_H__
#define __PINMUX_H__

#include <bchp_common.h>
#include "common.h"

#ifdef BCHP_SUN_TOP_CTRL_REG_START
#  include <bchp_sun_top_ctrl.h>
#endif
#ifdef BCHP_AON_PIN_CTRL_REG_START
#  include <bchp_aon_pin_ctrl.h>
#endif
#ifdef BCHP_CM_TOP_CTRL_REG_START
#  include <bchp_cm_top_ctrl.h>
#endif
#ifdef BCHP_GPIO_PER_REG_START
#  include <bchp_gpio_per.h>
#endif
#define PMUX(type, reg, field, val) \
	BDEV_WR(BCHP_##type##_CTRL_PIN_MUX_CTRL_##reg, \
		(BDEV_RD(BCHP_##type##_CTRL_PIN_MUX_CTRL_##reg) & \
		 ~BCHP_##type##_CTRL_PIN_MUX_CTRL_##reg##_##field##_MASK) | \
		((BCHP_##type##_CTRL_PIN_MUX_CTRL_##reg##_##field##_##val) << \
		 BCHP_##type##_CTRL_PIN_MUX_CTRL_##reg##_##field##_SHIFT))
#define PMUX_PADC(type, reg, field, val) \
	BDEV_WR(BCHP_##type##_CTRL_PIN_MUX_PAD_CTRL_##reg, \
		(BDEV_RD(BCHP_##type##_CTRL_PIN_MUX_PAD_CTRL_##reg) & \
		 ~BCHP_##type##_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_MASK) | \
		((BCHP_##type##_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_##val) << \
		 BCHP_##type##_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_SHIFT))

#endif /* __PINMUX_H__ */

