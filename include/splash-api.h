/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __SPLASH_API_H__
 #define __SPLASH_API_H__

#include <stdint.h>
#include "splash_script_load.h"
/* API that faces toward BOLT
*/
struct splash_rgb {
       uint8_t r;
       uint8_t g;
       uint8_t b;
};

void splash_api_setenv(void);
void splash_api_info(void);
void splash_api_start(void);
int  splash_glue_getmem(uint32_t memc, uint32_t *memtop, uint32_t *memlowest);
int  splash_audio_script_run(unsigned int size, unsigned int address,
	uint32_t repeatcount, SplashData *splashData);

void *splash_api_get_imgbuff(int idx);
void splash_api_replace_bmp(uint8_t *bmp, struct splash_rgb *rgb);

void splash_feedback(int idx);
#endif /* __SPLASH_API_H__ */
