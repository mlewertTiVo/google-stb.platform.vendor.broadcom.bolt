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

#ifndef _BCMGPHY_H_
#define _BCMGPHY_H_

#include "lib_printf.h"
#include "net_mdio.h"

void bcm_gphy_workaround(mdio_info_t *mdio, int *phy, uint8_t cnt);

#endif /* _BCMGPHY_H_ */
