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

#include "board_init.h"
#include "otp_status.h"
#include "bchp_sun_top_ctrl.h"
#include "lib_physio.h"
#include "timer.h"

#if CFG_SATA
void board_init_sata(void)
{
	int port;

#ifdef OTP_OPTION_SATA_DISABLE
	if (!OTP_OPTION_SATA_DISABLE())
#endif
		for (port = 0; port < NUM_SATA; port++)
			bolt_add_device(&satadrv, port, 0, 0);
}

void sata_exit(void)
{
	/* Fully H/W reset the SATA IP block.
	 * Done so the Linux SATA driver can start afresh.
	 */
	BDEV_WR_F(SUN_TOP_CTRL_SW_INIT_0_SET, sata_sw_init, 1);
	bolt_usleep(10);
	BARRIER();

	BDEV_WR_F(SUN_TOP_CTRL_SW_INIT_0_CLEAR, sata_sw_init, 1);
}

#endif
