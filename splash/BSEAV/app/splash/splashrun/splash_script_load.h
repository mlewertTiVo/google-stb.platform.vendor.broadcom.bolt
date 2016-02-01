/***************************************************************************
 *     Copyright (c) 2002-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_script_load.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 3/29/13 10:43p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/splash_script_load.h $
 * 
 * 11   3/29/13 10:43p syang
 * SW7435-676: add script loader version def
 * 
 * 10   3/29/13 11:41a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 * 
 * 9   10/31/12 7:03p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 7   9/10/12 4:40p jessem
 * SW7425-3872: Removed hMem1 use with 7425.
 * 
 * 6   4/12/12 5:05p randyjew
 * SWCFE-769: Memc0 only support
 * 
 * 5   8/22/11 5:14p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 * 
 * 4   11/30/09 3:12p jrubio
 * SW7335-627: fix for non-7420 chips
 * 
 * 3   9/24/09 11:47p nickh
 * SW7420-351: Modify load script to add MEMC1 for 7420 support
 * 
 * 2   6/28/07 5:06p shyam
 * PR 30741 : syncup with cfe version
 * 
 * 1   5/14/07 7:16p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:49p dkaufman
 * PR3481: Added file
 * 
 *
 ***************************************************************************/

#ifndef SPLASH_VDC_SETUP_H__
#define SPLASH_VDC_SETUP_H__

#include "splash_magnum.h"

/* for CFE to know the script loader version in compile time */
#define SPLASH_SCRIPT_LOADER_VER        2

/* basic info of a surface */
typedef struct
{
	int                    ihMemIdx;        /* this surface is allocated from *(phMem+ihMemIdx) */
	BPXL_Format            ePxlFmt;         /* pixel format */
    uint32_t               ulWidth;
    uint32_t               ulHeight;
    uint32_t               ulPitch;
	char                   bmpFile[256];    /* bmp file in splashgen */
} SplashSurfaceInfo;

/* basic info of a display */
typedef struct
{
	int                    iSurIdx;        /* uses surface buffer from *(phHeap+ihMemIdx) */
	BFMT_VideoFmt          eDspFmt;        /* display video format */
	uint32_t               ulRdcScratchReg0;
	uint32_t               ulRdcScratchReg1;  
} SplashDisplayInfo;

/* -----------------------------------------------------------------
 * note to CFE developer:
 * 1). each surface buffer must be located in the section controlled
 *     by the right memory controler. Otherwise you see black screen
 * 2). the order in the aulSurfaceBufOffset array must match
 *     g_SplashSurface in splash_vdc_rul_def.h
 */
/* PORT POINT: if we have more than 4 mem heaps or surfaces */
#define SPLASH_MAX_SURFACE     4
typedef struct
{
	BMEM_Handle  hRulMem; /* heap used by VDC for RULs, CFE can pass NULL */
	uint32_t     aulSurfaceBufOffset[SPLASH_MAX_SURFACE];
} SplashBufInfo;

#define BSPLASH_SURFACE_BUF_SIZE(pSur)   ((pSur)->ulHeight * (pSur)->ulPitch)
void splash_bvn_init(BREG_Handle hRegister, SplashBufInfo *pSplashBufInfo);

/* those globals are defined in splash_script_load.c */
extern uint32_t  g_ulNumMem;
extern uint32_t  g_ulNumSurface;
extern uint32_t  g_ulNumDisplay;

/* those globals are defined in splash_vdc_rul_def.h, it is included in splash_script_load.c */
extern int g_iRulMemIdx;
extern SplashSurfaceInfo g_SplashSurfaceInfo[];
extern SplashDisplayInfo g_SpalshDisplayInfo[];

#endif
