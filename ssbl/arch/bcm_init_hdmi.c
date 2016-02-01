/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "board_init.h"
#include "board.h"

#if defined(CONFIG_BCM7250)
#include "bchp_hdmi_tx_phy.h"
#include "bchp_sun_top_ctrl.h"
#include "chipid.h"

/* hdmi_swap_channels -- swaps HDMI Tx channels 0 and 2
 */
static void hdmi_swap_channels(void)
{
	uint32_t channel_swap;

	/* The channels TX2 and TX0 of HDMI output are physically swapped
	 * on Dongle boards due to its physical layout. Swap their
	 * corresponding internal channels so that the final HDMI output
	 * does not have any swapped channels.
	 */
	channel_swap = BDEV_RD(BCHP_HDMI_TX_PHY_CHANNEL_SWAP);
	channel_swap &=
		~(BCHP_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_SEL_MASK |
			BCHP_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_SEL_MASK);
	channel_swap |=
		(0 << BCHP_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_SEL_SHIFT) |
		(2 << BCHP_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_SEL_SHIFT);
	BDEV_WR(BCHP_HDMI_TX_PHY_CHANNEL_SWAP, channel_swap);
}
#endif

/* board_init_hdmi -- initializes board specific HDMI configuration
 */
void board_init_hdmi()
{
#if defined(CONFIG_BCM7250)
	const uint32_t prid = BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID) &
		~CHIPID_REV_MASK;

	/* only BCM72502 is used for Dongle design */
	if (prid == 0x07250200)
		hdmi_swap_channels();
#endif
}

