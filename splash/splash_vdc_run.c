/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* This file is a copy of BSEAV/app/splash/splashrun/splash_vdc_run.c
 * that has been modified to run under BOLT. If you update BSEAV/app/splash
 * be sure to check any changes in that original file.
 */

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

int splash_script_run(BREG_Handle hReg, BMEM_Handle *phMem,
	SplashData *pSplashData)
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
	/* hReg can be NULL, it is unused */
	if (!phMem || !pSplashData)
	{
		xprintf("ERROR: invalid parameters to %s(0x%x, 0x%x, 0x%x)\n",
			__func__, (unsigned int)hReg, (unsigned int)phMem,
			(unsigned int)pSplashData);
		return 1;
	}

	BKNI_Memset((void *) &splashBuf, 0, sizeof(SplashBufInfo));

	splashBuf.hRulMem = *(phMem + pSplashData->iRulMemIdx);
	
	for (ii=0; ii<pSplashData->iNumSurface; ii++)
	{
		BMEM_Handle heap;
		unsigned int bufSize;

		pSurInfo = pSplashData->pSurfInfo + ii;
		heap = *(phMem+pSurInfo->ihMemIdx);
		bufSize = BSPLASH_SURFACE_BUF_SIZE(pSurInfo);

		/* skip invalid surface */
		if (bufSize == 0)
			continue;

		apvBuf[ii] = BMEM_Alloc(heap, bufSize);
		if (!apvBuf[ii])
		{
			xprintf("ERROR: allocating %d bytes from heap %u "
				"failed\n", bufSize, (unsigned int)heap);
			return 1;
		}
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
		splash_fillbuffer(pvBuf, 0x00, 0x00, 0x00);

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

	splash_bvn_init(hReg, &splashBuf, pSplashData);
	
	return 0;
}

