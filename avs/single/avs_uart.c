/***************************************************************************
 *     Copyright (c) 2013-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides the hardware setup to route the AVS UART to a port on
 *  the reference board.  Note that this port may be used by other code
 *  so should not be enabled by default.  Instead, use the BBS equivalent
 *  script that does this.  This is here to demonstrate how it can be
 *  done and for developer convience.
 *
***************************************************************************/

#include <common.h>
#include <pinmux.h>

#include "avs.h"
#include "bchp_sun_top_ctrl.h"

#ifndef ENABLE_AVS_UART
#define ENABLE_AVS_UART		0
#endif

/*
** NOTICE: This is used for debug purposes to route the AVS UART to a port on
** the reference board.  The ports on the reference board are not allocated
** to any specific part so this should NOT be enabled by default.
** This needs to be included in the build if its being called.
*/

/* The UART for the AVS CPU needs to be routed to a port on the reference board.
 * This is used for debugging only and should not be enabled in normal
 * processing. */
/* Note: a BBS script should be used for this.  This is here for convience. */
void setup_chip_for_avs_uart(void)
{
	if (!ENABLE_AVS_UART)
		return;

#if defined(CONFIG_BCM7445D0)
	PMUX(SUN_TOP, 9, gpio_010, TP_IN_22);
	PMUX(SUN_TOP, 9, gpio_011, TP_OUT_23);
	BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_2_cpu_sel, 15);
	BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16);
#else
	/* If not defined then it just won't set anything up */
#endif
}
