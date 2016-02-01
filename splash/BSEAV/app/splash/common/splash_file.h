/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_file.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 3/29/13 11:28a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/common/splash_file.h $
 * 
 * 1   3/29/13 11:28a syang
 * SW7435-676: shared bmp file open is put here
 * 
 ***************************************************************************/

#ifndef __SPLASH_FILE_H
#define __SPLASH_FILE_H

#include "splash_magnum.h"
//#include <stdio.h>

/* this is not used in cfe */
uint8_t *splash_open_bmp(char *name);


#endif  /* #ifndef __SPLASH_FILE_H */

/* End of File */

