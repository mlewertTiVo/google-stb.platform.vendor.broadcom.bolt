/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __ZIMAGE_H__
#define __ZIMAGE_H__

#include <byteorder.h>

#define DT_ZIMAGE_SIGNATURE  cpu_to_le32(0x016f2818U)

#define ZIMAGE_HEADER	0x40 /* 0x2C */

#define ZIMG_OF_SIG 	0x09 /* u32 offsets, not byte offsets! */
#define ZIMG_OF_START	0x0A
#define ZIMG_OF_END	0x0B

#define IMAGE_HEADER_MAGIC	0x644d5241

typedef struct bolt_zimage_s 
{
	unsigned int reserved[9];
	unsigned int magic;
	unsigned int astart;
	unsigned int aend;
}
bolt_zimage_t;

struct bolt_image_header {
	uint32_t code0;
	uint32_t code1;
	uint64_t text_offset;
	uint64_t res0;
	uint64_t res1;
	uint64_t res2;
	uint64_t res3;
	uint64_t res4;
	uint32_t magic;
	uint32_t res5;
} __packed;

int bolt_zimage_setenv_end(unsigned int address);

char *bolt_zimage_getenv_end(void);

#endif /* __ZIMAGE_H__ */
