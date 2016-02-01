/***************************************************************************
 *     Copyright (c) 2005-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: platformconfig.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/8/13 1:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97366/platformconfig.c $
 * 
 * 1   10/8/13 1:31p pntruong
 * SW7439-28: Initial version.
 * 
 ***************************************************************************/

#include "platformconfig.h"

#include "bchp_gio.h" /* For platform config */
#include "bchp_sun_top_ctrl.h"


BERR_Code ConfigPlatform(BREG_Handle hRegister)
{
	BSTD_UNUSED(hRegister);
	return BERR_SUCCESS;
}
