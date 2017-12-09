/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "dev_bcmgphy.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "timer.h"
#include "net_mdio.h"
#include "compiler.h"

static void __maybe_unused mii_write_misc(mdio_info_t *mdio,
	int phy_id, uint16_t reg, uint16_t chl, uint16_t value)
{
	int tmp;

	mdio_write(mdio, phy_id, 0x18, 0x7);
	tmp = mdio_read(mdio, phy_id, 0x18);
	tmp = tmp | 0x800;
	mdio_write(mdio, phy_id, 0x18, tmp);

	tmp = (chl * 0x2000) | reg;
	mdio_write(mdio, phy_id, 0x17, tmp);

	mdio_write(mdio, phy_id, 0x15, value);
}

static void __maybe_unused mii_write_exp(mdio_info_t *mdio,
	int phy_id, uint16_t reg, uint16_t value)
{
	mdio_write(mdio, phy_id, 0x17, 0xf00 | reg);
	mdio_write(mdio, phy_id, 0x15, value);
}

static void __maybe_unused r_rc_cal_reset(mdio_info_t *mdio, int phy_id)
{
	/* Reset R_CAL/RC_CAL Engine */
	mii_write_exp(mdio, phy_id, 0x00b0, 0x0010);
	/* Disable Reset R_AL/RC_CAL Engine */
	mii_write_exp(mdio, phy_id, 0x00b0, 0x0000);
}

/* Workaround based on the recommendations from HW7445-1461, HW7445-1744 and
 * HW7445-1804 for GPHY rev. D0 */
#if defined(CONFIG_BCM7445D0)
void bcm_gphy_workaround(mdio_info_t *mdio, int *phy, uint8_t cnt)
{
	int i;

	for (i = 0; i < cnt; i++) {
		/* AFE_RXCONFIG_0 */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0000, 0xeb15);

		/* AFE_RXCONFIG_1 */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0001, 0x9b2f);

		/* AFE_RXCONFIG_2 */
		/* set rCal offset for HT=0 code and LT=-2 code */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0002, 0x2003);

		/* AFE_RX_LP_COUNTER, set RX bandwidth to maximum */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0003, 0x7fc0);

		/* AFE_TX_CONFIG */
		/* set 100BT Cfeed=011 to improve rise/fall time */
		mii_write_misc(mdio, phy[i], 0x0039, 0x0000, 0x0431);

		/* AFE_VDCA_ICTRL_0 */
		/* set Iq=1101 instead of 0111 for AB symmetry */
		mii_write_misc(mdio, phy[i], 0x0039, 0x0001, 0xa7da);

		/* AFE_VDAC_OTHERS_0, set 1000BT Cidac=010 for all ports */
		mii_write_misc(mdio, phy[i], 0x0039, 0x0003, 0xa020);

		/* AFE_HPF_TRIM_OTHERS, set 100Tx/10BT to -4.5% swing and set
		 * rCal offset for HT=0 code
		 */
		mii_write_misc(mdio, phy[i], 0x003a, 0x0000, 0x00e3);

		/* CORE_BASE1E */
		/* force trim to overwrite and set I_ext trim to 0000 */
		mdio_write(mdio, phy[i], 0x001e, 0x0010);

		/* DSP_TAP10, adjust bias current trim (+0% swing, +0 tick) */
		mii_write_misc(mdio, phy[i], 0x000a, 0x0000, 0x011b);

		/* Reset R_CAL/RC_CAL engine */
		r_rc_cal_reset(mdio, phy[i]);
	}
}

/* Workaround based on HW7364-116, HW7250-113, HW74371-72, HW74371-72,
 * HW7439-663 for GPHY revs. E0, F0 and G0 */
#elif defined(CONFIG_BCM7364) || defined(CONFIG_BCM7250B0) || \
	defined(CONFIG_BCM74371A0) || defined(CONFIG_BCM7439B0) || \
	defined(CONFIG_BCM7366C0) || defined(CONFIG_BCM7445E0)
void bcm_gphy_workaround(mdio_info_t *mdio, int *phy, uint8_t cnt)
{
	int i;

	for (i = 0; i < cnt; i++) {
		/* AFE_RXCONFIG_1 */
		/* provide more margin for INL/DNL measurement on ATE */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0001, 0x9b2f);

		/* AFE_TX_CONFIG */
		/* set 100BT Cfeed=011 to improve rise/fall time */
		mii_write_misc(mdio, phy[i], 0x0039, 0x0000, 0x0431);

		/* AFE_VDCA_ICTRL_0 */
		/* set Iq=1101 instead of 0111 for AB symmetry */
		mii_write_misc(mdio, phy[i], 0x0039, 0x0001, 0xa7da);

		/* AFE_HPF_TRIM_OTHERS
		 * set 100Tx/10BT to -4.5% swing and set rCal
		 * offset for HT=0 code
		 */
		mii_write_misc(mdio, phy[i], 0x003a, 0x0000, 0x00e3);

		/* CORE_BASE1E */
		/* force trim to overwrite and set I_ext trim to 0000 */
		mdio_write(mdio, phy[i], 0x001e, 0x0010);

		/* DSP_TAP10, adjust bias current trim (+0% swing, +0 tick) */
		mii_write_misc(mdio, phy[i], 0x000a, 0x0000, 0x011b);

		/* Reset R_CAL/RC_CAL engine */
		r_rc_cal_reset(mdio, phy[i]);
	}
}
#elif defined(CONFIG_BCM7260A0) || \
	defined(CONFIG_BCM7268) || \
	defined(CONFIG_BCM7271)
/* Workaround based on HWBCM7268-85 and HW7271-242 */
void bcm_gphy_workaround(mdio_info_t *mdio, int *phy, uint8_t cnt)
{
	int i;

	for (i = 0; i < cnt; i++) {
		/* set shadow mode 2 */
		mdio_set_clr_bits(mdio, phy[i], 0x1f, 0x0004, 0x0000);

		/* Set current trim values INT_trim = -1, Ext_trim =0 */
		mdio_write(mdio, phy[i], 0x1A, 0x3BE0);

		/* Cal reset */
		mdio_write(mdio, phy[i], 0xE, 0x23);
		mdio_set_clr_bits(mdio, phy[i], 0xF, 0x0006, 0x0000);

		/* Cal reset disable */
		mdio_write(mdio, phy[i], 0xE, 0x23);
		mdio_set_clr_bits(mdio, phy[i], 0xF, 0x0000, 0x0006);

		/* reset shadow mode 2 */
		mdio_set_clr_bits(mdio, phy[i], 0x1f, 0x0000, 0x0004);
	}
}
#elif defined(CONFIG_BCM7278)
/* Workaround based on HW7278-359 */
void bcm_gphy_workaround(mdio_info_t *mdio, int *phy, uint8_t cnt)
{
	int i;

	for (i = 0; i < cnt; i++) {
		/* AFE_RXCONFIG_2
		 * +1 RCAL codes for RL centering for both LT/HT conditions
		 */
		mii_write_misc(mdio, phy[i], 0x0038, 0x0002, 0xd003);

		/* DSP_TAP10
		 * Cut master bias current by 2% to compensate for RCAL offset
		 */
		mii_write_misc(mdio, phy[i], 0x000a, 0x0000, 0x791b);

		/* AFE_HPF_TRIM_OTHERS
		 * Improve hybrid leakage
		 */
		mii_write_misc(mdio, phy[i], 0x003a, 0x0000, 0x10e3);

		/* rx_on_tune 8 -> 0xf */
		mii_write_misc(mdio, phy[i], 0x0021, 0x0002, 0x87f6);

		/* 100BaseTx EEE bandiwdth */
		mii_write_misc(mdio, phy[i], 0x0022, 0x0002, 0x017d);

		/* Enable ffe zero detection for Vitesse interoperability */
		mii_write_misc(mdio, phy[i], 0x0026, 0x0002, 0x0015);

		/* Reset R_CAL/RC_CAL engine */
		r_rc_cal_reset(mdio, phy[i]);
	}
}
#endif
