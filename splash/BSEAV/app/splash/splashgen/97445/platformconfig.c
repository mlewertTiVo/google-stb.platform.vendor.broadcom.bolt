/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: platformconfig.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/31/12 2:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97445/platformconfig.c $
 * 
 * 1   8/31/12 2:50p katrep
 * SW7445-1:add spalsh
 * 
 * 1   8/8/11 11:07p nickh
 * SW7425-878: Add 7425
 * 
 * 1   6/27/11 11:34a katrep
 * SW7231-220:add support for 7231 &newer 40nm chips
 * 
 * 1   9/21/09 5:08p nickh
 * SW7420-351: Add 7420 Splash screen support
 * 
 * 1   5/14/07 7:01p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 * 
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

