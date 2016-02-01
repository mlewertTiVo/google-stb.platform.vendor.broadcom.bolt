/***************************************************************************
 *     Copyright (c) 2003-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_vdc_run.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 4/12/13 2:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/splash_vdc_run.c $
 * 
 * 17   4/12/13 2:06p syang
 * SW7445-213: change to use cached addr for surface and RULs
 * 
 * 16   3/29/13 11:41a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 * 
 * 15   10/31/12 7:03p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 13   9/10/12 4:41p jessem
 * SW7425-3872: Removed hMem1 use with 7425.
 * 
 * 12   9/4/12 4:40p jessem
 * SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
 * GFD1 to MEMC0.
 *
 * 11   4/19/12 4:41p mward
 * SW7435-114:  Add support for 7435.  Uses MEMC0 for graphics surface in
 * 4 transcode RTS use-case, MEMC1 for others.
 *
 * 10   4/12/12 5:05p randyjew
 * SWCFE-769: Memc0 only support
 *
 * 9   4/10/12 2:15p jessem
 * SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
 * correct heap to use.
 *
 * 8   8/23/11 10:01a jrubio
 * SW7340-277: add Support for 7340
 *
 * 7   8/22/11 5:14p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 *
 * 6   8/8/11 11:54p nickh
 * SW7425-878: Add 7425 splash support
 *
 * 5   9/21/09 5:38p nickh
 * SW7420-351: Pass in the appropriate heaps for 7420
 *
 * 4   5/14/09 5:42p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 3   10/8/07 7:45p shyam
 * PR 30741 : Simplify the bitmap filename
 *
 * 2   6/28/07 5:07p shyam
 * PR 30741 : Syncup with cfe portability
 *
 * 1   5/14/07 7:17p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 * Hydra_Software_Devel/2   2/6/06 7:36a dkaufman
 * PR3481: Moved surface update code to new file
 *
 * Hydra_Software_Devel/1   11/17/05 4:49p dkaufman
 * PR3481: Added file
 *
 *
 ***************************************************************************/

/* This file is a copy of BSEAV/app/splash/splashrun/splash_vdc_run.c
 that has been modified to run under BOLT. If you update BSEAV/app/splash
 be sure to check any changes in that original file.
*/

#if 0 /* original */
#include <stdio.h>
#include <string.h>

#include "bstd.h"
#include "bmem.h"
#include "bkni.h"
#include "breg_mem.h"
#endif

#include "splash_bmp.h"
#include "splash_file.h"
#include "splash-media.h"
#include "splash_script_load.h"


BDBG_MODULE(splashrun);

/* As splash runs, save the surface address so we
 * can replace it later on with another bitmap.
 */
void *splash_apvBuf[SPLASH_MAX_SURFACE];

/* accessor for the saved gfx surfaces. */
void *splash_api_get_imgbuff(int idx)
{
	return (idx < SPLASH_MAX_SURFACE) ? splash_apvBuf[idx] : NULL;
}


int splash_script_run(BREG_Handle hReg, BMEM_Handle *phMem)
{
	SplashBufInfo  splashBuf;
	uint8_t  *bmpBuf = NULL;
	BMP_HEADER_INFO  myBmpInfo;
	SplashSurfaceInfo  *pSurInfo;
	void *apvBuf[SPLASH_MAX_SURFACE];
	void *pvBuf;
	int  x,y;
	int  ii;
	struct SplashMediaInfo *mediaInfo = NULL;
	
#if (CFG_CMD_LEVEL >= 5)
	printf("******************* Splash BVN Script Loader !!!! *************************\n");
#endif
	BKNI_Memset((void *) &splashBuf, 0, sizeof(SplashBufInfo));

	splashBuf.hRulMem = *(phMem + g_iRulMemIdx);
	
	for (ii=0; ii<(int)g_ulNumSurface; ii++)
	{
		pSurInfo = &g_SplashSurfaceInfo[ii];
		apvBuf[ii] = BMEM_Alloc(*(phMem+pSurInfo->ihMemIdx), BSPLASH_SURFACE_BUF_SIZE(pSurInfo));
		BMEM_Heap_ConvertAddressToCached(*(phMem+pSurInfo->ihMemIdx), apvBuf[ii], &pvBuf);
		BMEM_ConvertAddressToOffset(
			*(phMem+pSurInfo->ihMemIdx), apvBuf[ii], &splashBuf.aulSurfaceBufOffset[ii]);
		splash_apvBuf[ii] = apvBuf[ii]; /* BOLT addition: save buffer address */
#if (CFG_CMD_LEVEL >= 5)
		printf("** Allocating Surface %d Memory done **\n", ii);
#endif

		if (!mediaInfo)
		{
			mediaInfo = splash_open_media(SplashMediaType_eBoot,
						SplashMediaFormat_eBmp, phMem);
			if (mediaInfo)
			{
				bmpBuf = (uint8_t *)mediaInfo->buf;
				splash_bmp_getinfo(bmpBuf, &myBmpInfo);
#if (CFG_CMD_LEVEL >= 3)
				xprintf("Loaded BMP: W=%d H=%d\n",
				 myBmpInfo.info.width, myBmpInfo.info.height);
#endif
			}
#if (CFG_CMD_LEVEL >= 5)
			else
				BDBG_ERR(("Missing BMP file in splashFile\n"));
#endif
		}

		splash_set_surf_params(pSurInfo->ePxlFmt, pSurInfo->ulPitch, pSurInfo->ulWidth, pSurInfo->ulHeight) ;

		/* splash_fillbuffer(splashBuf.apvSurfaceBufAddr[ii] , 0xF8, 0xE0, 0) ; */
		splash_fillbuffer(pvBuf, 0x00, 0x00, 0x80);

		if(bmpBuf)
		{
#if (CFG_CMD_LEVEL >= 5)
			BDBG_MSG(("*******************************"));
			BDBG_MSG(("splash.bmp: Width = %d Height = %d",
					  myBmpInfo.info.width, myBmpInfo.info.height));
			BDBG_MSG(("*******************************"));
#endif
			x = (pSurInfo->ulWidth-myBmpInfo.info.width)/2 ;
			y = (pSurInfo->ulHeight-myBmpInfo.info.height)/2 ;
			splash_render_bmp_into_surface(x, y, bmpBuf, pvBuf);
#if (CFG_CMD_LEVEL >= 5)
			printf("** copy bmp into Surface %d done **\n", ii);
#endif
		}

		/* flush cached addr */
		BMEM_Heap_FlushCache(*(phMem+pSurInfo->ihMemIdx), pvBuf, BSPLASH_SURFACE_BUF_SIZE(pSurInfo));
	}

	splash_bvn_init(hReg, &splashBuf);
#if 0 /* original */
	printf("** Splash display done **\n");
#if 0 /* either mem leak error or GFD access unallocted mem error */
	printf("Press any key to continue ...");
	getchar();

	for (ii=0; ii<(int)g_ulNumSurface; ii++)
	{
		pSurInfo = &g_SplashSurfaceInfo[ii];
		BMEM_Free(*(phMem + pSurInfo->ihMemIdx), apvBuf[ii]);
	}
#endif
#endif /* original */
	
	return 0;
}

