/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "splash-api.h"
#include "splash_bmp.h"
#include "splash_file.h"
#include "splash_magnum.h"
#include "splash-media.h"
#include "splash_script_load.h"

#include "cache_ops.h"
#include "compiler.h"
#include "env_subr.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_queue.h"
#include "lib_string.h"
#include "lib_types.h"

#if !CFG_SPLASH

void splash_api_setenv(void)
{
	char buffer[20];

	xsprintf(buffer, "%s", "DISABLE");
	env_setenv("SPLASH", buffer, ENV_FLG_BUILTIN | ENV_FLG_READONLY);
}

void splash_api_info(void)
{
}

void splash_api_start(void)
{
}

/* the real one is in splash-glue.c
*/
int  splash_glue_getmem(uint32_t memc, uint32_t *memtop, uint32_t *memlowest)
{
	return 1; /* error */
}

#else /* CFG_SPLASH -------------------------------------------------------- */

/* 'splashgen' generated files make sense only when SPLASH is activated */
#include "splash_vdc_reg.h"
#ifndef SPLASH_VERSION_2
#include "splash_vdc_rul.h"
#endif

#ifdef SPLASH_VERSION_2
SplashData *GetSplashData(void);
#else
static SplashData *GetSplashData(void)
{
	SplashData *pSplashData;

	pSplashData = KMALLOC(sizeof(SplashData), 0);
	if (pSplashData == NULL)
		return NULL;

	pSplashData->ulNumMem = BSPLASH_NUM_MEM;
	pSplashData->iRulMemIdx = g_iRulMemIdx;
	pSplashData->iNumSurface = BSPLASH_NUM_SURFACE;
	pSplashData->pSurfInfo = g_SplashSurfaceInfo;
	pSplashData->iNumDisplay = BSPLASH_NUM_DISPLAY;
	pSplashData->pDispInfo = g_SplashDisplayInfo;
	pSplashData->iNumTrigMap =
		sizeof(g_aTriggerMap)/sizeof(g_aTriggerMap[0]);
	pSplashData->pTrigMap = (SplashTriggerMap *)g_aTriggerMap;
	pSplashData->nAulReg = sizeof(g_aulReg)/(2*sizeof(g_aulReg[0]));
	pSplashData->pAulReg = g_aulReg;

	return pSplashData;
}
#endif

static void dumpSplashData(SplashData *pSplashData)
{
#if (CFG_CMD_LEVEL >= 5)
	xprintf("SPLASH:\n"
		"\tulNumMem:\t%u (0x%08x)\n"
		"\tiRulMemIdx:\t%d (0x%08x)\n"
		"\tiNumSurface:\t%d (0x%08x)\n"
		"\tpSurfInfo: 0x%08x\n"
		"\tiNumDisplay:\t%d (0x%08x)\n"
		"\tpDispInfo: 0x%08x\n"
		"\tiNumTrigMap:\t%d (0x%08x)\n"
		"\tpTrigMap: 0x%08x\n"
		"\tnAulReg:\t%d (0x%08x)\n"
		"\tpAulReg: 0x%08x\n",
		(unsigned int) pSplashData->ulNumMem,
		(unsigned int) pSplashData->ulNumMem,
		pSplashData->iRulMemIdx, pSplashData->iRulMemIdx,
		pSplashData->iNumSurface,
		(unsigned int) pSplashData->iNumSurface,
		(unsigned int)pSplashData->pSurfInfo,
		pSplashData->iNumDisplay, pSplashData->iNumDisplay,
		(unsigned int)pSplashData->pDispInfo,
		pSplashData->iNumTrigMap, pSplashData->iNumTrigMap,
		(unsigned int)pSplashData->pTrigMap,
		pSplashData->nAulReg, pSplashData->nAulReg,
		(unsigned int)pSplashData->pAulReg);
#endif
}

int splash_script_run(BREG_Handle hReg, BMEM_Handle *phMem,
	SplashData *pSplashData);

void splash_api_setenv(void)
{
	char buffer[20];

	xsprintf(buffer, "%s", "ENABLE");
	env_setenv("SPLASH", buffer, ENV_FLG_BUILTIN | ENV_FLG_READONLY);
}

void splash_api_info(void)
{
	 xprintf("SPLASH:  %u; %s\n", MAX_SPLASH_SIZE,
		 CFG_SPLASH_FILE);
}

void splash_api_start(void)
{
	int i;
	BMEM_Handle mem[MAX_DDR];
	struct SplashMediaInfo *mediaInfo;
	SplashData *pSplashData;
	int retval;

	if (env_getenv("NO_SPLASH") || strcmp(env_getenv("SPLASH"), "ENABLE"))
		return;

	xprintf("SPLASH: starting\n");

	pSplashData = GetSplashData();
	if (pSplashData == NULL) {
		err_msg("SPLASH: getting SplashData failed");
		return;
	}
	dumpSplashData(pSplashData);

	if (splash_glue_init()) {
		KFREE(pSplashData);
		err_msg("SPLASH: glue failed");
		return;
	}

	if (splash_load_media(CFG_SPLASH_FILE)) {
		KFREE(pSplashData);
		err_msg("SPLASH: load failed");
		return;
	}

	/* Without hacking our custom copy of splash_vdc_run.c too
	 * much to make maintenance a pain, we need to get an
	 * index into which memc the RUL is made for in
	 * splash_vdc_rul_def.h => ihMemIdx.
	 */
	for (i = 0; i < MAX_DDR; i++)
		mem[i] = (BMEM_Handle)i;

	/* BREG_Handle is not used */
	if (splash_script_run(NULL, mem, pSplashData)) {
		KFREE(pSplashData);
		err_msg("SPLASH: running script failed");
		return;
	}

	mediaInfo = splash_open_media(SplashMediaType_eBoot,
			SplashMediaFormat_ePcm, mem);
	if (mediaInfo == NULL || mediaInfo->buf == NULL) {
		KFREE(pSplashData);
		xprintf("SPLASH: audio not present\n");
		return;
	}
	xprintf("SPLASH: start audio\n");
	retval = splash_audio_script_run(mediaInfo->size,
			(unsigned int)mediaInfo->buf,
			SPLASH_AUDIO_REPEAT_COUNT, pSplashData);
	KFREE(pSplashData); /* release as soon as not needed anymore */
	if (retval != 0) {
		err_msg("SPLASH: failed starting audio %d", retval);
		return;
	}
}

/* Replace SPLASH image or and/or set background color.
 * Derived directly from splash_script_run().
 */
void splash_api_replace_bmp(uint8_t *bmp, struct splash_rgb *rgb)
{
	BMP_HEADER_INFO bmp_info;
	SplashSurfaceInfo *surf_info;
	int  x, y;
	int i;
	SplashData *pSplashData;

	pSplashData = GetSplashData();
	if (pSplashData == NULL) {
		err_msg("SPLASH: failed to get data");
		return;
	}

	for (i = 0; i < pSplashData->iNumSurface; i++) {

		void *pimg = splash_api_get_imgbuff(i);

		if (!pimg)
			continue;

		surf_info = pSplashData->pSurfInfo + i;

		splash_bmp_getinfo(bmp, &bmp_info);

		splash_set_surf_params(surf_info->ePxlFmt,
			surf_info->ulPitch,
			surf_info->ulWidth, surf_info->ulHeight);

		x = (surf_info->ulWidth-bmp_info.info.width) / 2;
		y = (surf_info->ulHeight-bmp_info.info.height) / 2;

		if (rgb)
			splash_fillbuffer(pimg, rgb->r, rgb->g, rgb->b);

		if (bmp)
			splash_render_bmp_into_surface(x, y, bmp, pimg);

		CACHE_FLUSH_RANGE(pimg, BSPLASH_SURFACE_BUF_SIZE(surf_info));
	}

	KFREE(pSplashData);
}

#endif /* CFG_SPLASH */

