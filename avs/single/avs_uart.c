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

#ifndef BUILD_FOR_AVS_SV_BOARD
#define BUILD_FOR_AVS_SV_BOARD	0
#endif

/*
** NOTICE: This is used for debug purposes to route the AVS UART to a port on
** the reference board.  The ports on the reference board are not allocated
** to any specific part so this should NOT be enabled by default.
** This needs to be included in the build if its being called.
*/

/* The UART for the AVS CPU needs to be routed to a port on the reference board.
 * This is used for debugging only and should not be enabled in normal
 * processing. Enabling this will only provide output until Bolt resets these
 * registers (after convergence).  To keep these on, comment out pin definitions
 * in appropriate configuration files. */
/* Note: a BBS script should be used for this.  This is here for convience. */
void setup_chip_for_avs_uart(void)
{
	if (!ENABLE_AVS_UART)
		return;

#if defined(CONFIG_BCM7445D0)
	/* SVMB board: requires board mods: (UART at J3202)
	 *   1) installed 0ohm at R2694&R2695
	 *   2) swapped R3203&R3204 to B-C position
	 */
	PMUX(SUN_TOP, 9, gpio_010, TP_IN_22);
	PMUX(SUN_TOP, 9, gpio_011, TP_OUT_23);
	BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_2_cpu_sel, 15);
	BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16);
#elif defined(CONFIG_BCM7271) || defined(CONFIG_BCM7268)
	if (BUILD_FOR_AVS_SV_BOARD) {
		/* SV boards (UARD on J3202): */
		/* Requires board mods (remove R2505&R2506, */
		/* add 0ohm at R2560&R2561) */
		PMUX(SUN_TOP, 1, gpio_005, ALT_TP_IN_04);
		PMUX(SUN_TOP, 1, gpio_006, ALT_TP_OUT_04);
		BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_2_cpu_sel, 11); /*AVS_TOP*/
		BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16); /*SYS*/
	} else {
		/* DV boards (UART1 on J3202): */
		/* 7271: no board mods required */
		/* 7268: Rev 03: move R3204&R3203 to B->C position */
		PMUX(AON_PIN, 1, aon_gpio_12, TP_IN_05);
		PMUX(AON_PIN, 1, aon_gpio_13, TP_OUT_05);
		BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_3_cpu_sel, 11); /*AVS_TOP*/
		BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16); /*SYS*/
	}
#elif defined(CONFIG_BCM7260A0)
	/* DV boards (UART1 on J3202): */
	/* 7260: no board mods required */
	PMUX(AON_PIN, 1, aon_gpio_12, ALT_TP_IN_00);
	PMUX(AON_PIN, 1, aon_gpio_13, ALT_TP_OUT_00);
	BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_0_cpu_sel, 11); /*AVS_TOP*/
	BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16); /*SYS*/
#elif defined(CONFIG_BCM7278A0)
	/* SV boards (UART1 on J3202): */
	/* 7278: Requires board mods (add 0ohm resistors at R2539&R2541) */
	PMUX(SUN_TOP, 2, gpio_006, TP_IN_18);
	PMUX(SUN_TOP, 2, gpio_007, TP_OUT_19);
	BDEV_WR_F(SUN_TOP_CTRL_UART_ROUTER_SEL_0, port_1_cpu_sel, 11); /*AVS_TOP*/
	BDEV_WR_F(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, 16); /*SYS*/
#else
	/* If not defined then it just won't set anything up */
#endif
}
