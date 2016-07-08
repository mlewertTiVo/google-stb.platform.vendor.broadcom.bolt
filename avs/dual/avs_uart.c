/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
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

	/* Chip specific local pinmuxing and/or SUN_TOP_CTRL routing
	 * goes here for avs. If not defined then it just won't set
	 * anything up.
	 */
}
