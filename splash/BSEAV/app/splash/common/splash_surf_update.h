/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_surf_update.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/14/07 7:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/common/splash_surf_update.h $
 * 
 * 1   5/14/07 7:23p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   2/6/06 8:13a dkaufman
 * PR3481: new file
 * 
 *
 ***************************************************************************/


#include "bstd.h"

void ShowColorsOnSurface(void* splashAddress, uint32_t splashPitch, uint32_t splashWidth, uint32_t splashHeight);
