/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: platformconfig.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/14/07 6:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/platformconfig.h $
 * 
 * 1   5/14/07 6:49p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 * 
 *
 ***************************************************************************/

#ifndef PLATFORMCONFIG_H__
#define PLATFORMCONFIG_H__
#include "bstd.h"
#include "breg_mem.h"

BERR_Code ConfigPlatform(BREG_Handle hRegister);

#endif
