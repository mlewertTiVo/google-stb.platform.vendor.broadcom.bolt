/***************************************************************************
 *	Copyright (c) 2012-2015, Broadcom Corporation
 *	All Rights Reserved
 *	Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#include "splash_bmp.h"
#include "splash_file.h"
#include "splash_script_load.h"
#include "compiler.h"
#include "cache_ops.h"
#include "splash-api.h"


/* Replace SPLASH image or and/or set background color.
 * Derived directly from splash_script_run().
 */
void splash_api_replace_bmp(uint8_t *bmp, struct splash_rgb *rgb)
{
       BMP_HEADER_INFO bmp_info;
       SplashSurfaceInfo *surf_info;
       int  x, y;
       unsigned int i;

       for (i = 0; i < g_ulNumSurface; i++) {

	       void *pimg = splash_api_get_imgbuff(i);

	       if (!pimg)
		       continue;

	       surf_info = &g_SplashSurfaceInfo[i];

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
}

