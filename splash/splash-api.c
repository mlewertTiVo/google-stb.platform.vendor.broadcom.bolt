/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "env_subr.h"

#include "splash-api.h"
#include "splash-media.h"


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


#include "splash_magnum.h"
#include "splash_script_load.h"

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
	struct SplashMediaInfo *mediaInfo = NULL;

	if (!env_getenv("NO_SPLASH") &&
		!strcmp(env_getenv("SPLASH"), "ENABLE")) {

		xprintf("SPLASH: starting\n");

		if (splash_glue_init()) {
			err_msg("SPLASH: glue failed");
			return;
		}

		if (splash_load_media(CFG_SPLASH_FILE)) {
			err_msg("SPLASH: load failed");
			return;
		}

		/*
		Without hacking our custom copy of splash_vdc_run.c too
		much to make maintenance a pain, we need to get an
		index into which memc the RUL is made for in
		splash_vdc_rul_def.h => ihMemIdx.
		*/
		for (i = 0; i < MAX_DDR; i++)
			mem[i] = (BMEM_Handle)i;

		(void)splash_script_run(
			NULL /* unused BREG_Handle */, mem);

		mediaInfo = splash_open_media(SplashMediaType_eBoot,
					SplashMediaFormat_ePcm, mem);
		if ((mediaInfo)  && (mediaInfo->buf)) {
			xprintf("SPLASH: start audio\n");
			splash_audio_script_run(mediaInfo->size,
				   (unsigned int)mediaInfo->buf,
					SPLASH_AUDIO_REPEAT_COUNT);
		} else
			xprintf("SPLASH: audio not present\n");
	}
}

#endif /* CFG_SPLASH */
