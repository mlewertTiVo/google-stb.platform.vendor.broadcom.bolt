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

void board_init_sata(void)
{
#if CFG_SATA
	int port;
#ifdef OTP_OPTION_SATA_DISABLE
	if (!OTP_OPTION_SATA_DISABLE())
#endif
		for (port = 0; port < NUM_SATA; port++)
			bolt_add_device(&satadrv, port, 0, 0);
#endif
}
