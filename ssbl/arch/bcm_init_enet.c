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

#include <bchp_sun_top_ctrl.h>

#include "board_init.h"
#include "board.h"
#include "pinmux.h"

#if CFG_ENET
/* Set the correct (RG)MII pad voltage based on the configured GENET/SYSTEMPORT
 * PHY interface mode.
 *
 * GENET_0 can use the RGMII_0 or the RGMII_1 pad based on mii_genet_mac_select
 * GENET_1 can only use the RGMII_1 pad
 * GENET_2 can only use the RGMII_0 pad
 *
 * SYSTEMPORT has two RGMII pads which have the same restrictions.
 *
 * hint is used to skip over the adapter which is not relevant for the
 * configuration (e.g: 1 for GENET_1 and 0 for SYSTEMPORT Port 0).
 */
static void board_mii_cfg(unsigned int hint)
{
	unsigned int uses_rgmii0_pad = 0, uses_rgmii1_pad = 0;
	unsigned int __maybe_unused rgmii0_pad_voltage = 0;
	unsigned int __maybe_unused rgmii1_pad_voltage = 0;
	const enet_params *e;
	unsigned int i, count = 0;
	unsigned int pad_sel_val;

	/* Every other chip uses 14 mA -> 6 */
	pad_sel_val = 6;

	for (i = 0; i < board_num_enet(); i++) {
		e = board_enet(i);
		if (!e)
			continue;

		if (strstr(e->phy_type, "MII")) {
			if (i != hint)
				uses_rgmii0_pad = 1;
			if (count)
				uses_rgmii1_pad = 1;
			count++;
		}

		/* Resolve the RGMII_<N> pad voltage, 0 -> 2.5V, 1 -> 3.3V */
		if (!strncmp(e->phy_type, "MII", 3) && uses_rgmii0_pad)
			rgmii0_pad_voltage = 1;
		if (!strncmp(e->phy_type, "MII", 3) && uses_rgmii1_pad)
			rgmii1_pad_voltage = 1;
	}

#ifdef CONFIG_BCM7445
	/* set RGMII_0 lines to 2.5V */
	if (uses_rgmii0_pad)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_0_pad_sel_gmii, rgmii0_pad_voltage);
	if (uses_rgmii1_pad)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_1_pad_sel_gmii, rgmii1_pad_voltage);
#endif

	/* Set the correct pad drive strength */
#ifdef BCHP_GPIO_PER_PAD_CTRL_rgmii_0_pad_sel_MASK
	if (uses_rgmii0_pad)
		BDEV_WR_F(GPIO_PER_PAD_CTRL,
			  rgmii_0_pad_sel, pad_sel_val);
#endif
#ifdef BCHP_SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0_rgmii_0_pad_sel_MASK
	if (uses_rgmii0_pad)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_0_pad_sel, pad_sel_val);
#endif
#ifdef BCHP_SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0_rgmii_1_pad_sel_MASK
	if (uses_rgmii1_pad)
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_1_pad_sel, pad_sel_val);
#endif

#ifdef CONFIG_BCM3390A0 /* SWBOLT-1684 */
	{
		struct fsbl_info *inf = board_info();

		/* For BCM93390VMS52S only. */
		if (inf && (inf->bid == 0x50)) {
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_1_pad_modehv, 0);
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_1_pad_amp_en, 1);
		}
	}
#endif /* CONFIG_BCM3390A0 */
}
#endif /* CFG_ENET */

#if CFG_GENET

/* for genet_get_phyintf() */
#include "../dev/dev_genet.h"

static void board_init_genet(void)
{
	unsigned int i;

	board_mii_cfg(1);

	/* Add the ethernet driver.
	*/
	for (i = 0; i < board_num_enet(); i++)
		bolt_add_device(&genetdrv, 0, 0, (int *)i);
}

static void board_exit_genet(void)
{
	/* soft reset enet
	*/
	const enet_params *e = board_enet(0);
	const char *phyintf;

	if (NULL == e)
		return;
	phyintf = genet_get_phyintf(e->base);
	if (phyintf && (strcmp(phyintf, "INT") == 0)) {
#if defined(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet0_sw_init_MASK)
		REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET) =
BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet0_sw_init_MASK;
#endif
#if defined(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet1_sw_init_MASK)
		REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET) =
BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet1_sw_init_MASK;
#endif
		bolt_msleep(10);
#if defined(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet0_sw_init_MASK)
		REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR) =
BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet0_sw_init_MASK;
#endif
#if defined(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet1_sw_init_MASK)
		REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR) =
BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_genet1_sw_init_MASK;
#endif
	}
}

#define board_ll_init_enet board_init_genet
#define board_ll_exit_enet board_exit_genet
#endif /* CFG_GENET */

#if CFG_SYSTEMPORT && CFG_ENET
static void board_init_systemport(void)
{
	board_mii_cfg(0);

	bolt_add_device(&sysportdrv, 0, 0, (int *)0);
}

static void board_exit_systemport(void)
{
#if defined(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_systemport_sw_init_MASK)
	REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET) =
		BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_systemport_sw_init_MASK;
	bolt_msleep(10);
	REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR) =
		BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_systemport_sw_init_MASK;
#endif
}

#define board_ll_init_enet board_init_systemport
#define board_ll_exit_enet board_exit_systemport
#endif /* CFG_SYSTEMPORT */

#if CFG_RUNNER && CFG_ENET
static void board_init_runner(void)
{
	board_mii_cfg(0);

	bolt_add_device(&runnerdrv, 0, 0, (int *)0);
}

static void board_exit_runner(void)
{
}

#define board_ll_init_enet board_init_runner
#define board_ll_exit_enet board_exit_runner

#endif

#if CFG_ENET
void board_init_enet(void)
{
	board_ll_init_enet();
}

void enet_exit(void)
{
	board_ll_exit_enet();
}
#endif /* CFG_ENET */
