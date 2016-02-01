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
#ifndef __SPLASH_MEDIA_H__
#define __SPLASH_MEDIA_H__

/*
	splashFile format:

	BRCM | VERSION | TOTAL_SIZE |
	BMPX/PCMX | MEDIA_FILE_SIZE | OFFSET |
	BMPX/PCMX | MEDIA_FILE_SIZE | OFFSET |
	.
	.
	.
	BMPX/PCMX | MEDIA_FILE_SIZE | OFFSET |
	DATAX |
	DATAX |
	.
	.
	.
	DATAX |
*/

#define SPLASH_FILE_MAJ_VER 1

#define SPLASH_FILE_MIN_VER 0

#define SPLASH_FILE_VER (SPLASH_FILE_MAJ_VER<<16 | SPLASH_FILE_MIN_VER)

/* two bmps and two pcm files. Refer to porting_bolt_to_customer_platforms.txt
 * in /docs for more information.
 */
#define SPLASH_MAX_MEDIA 4

/* Identifier, size and offset */
#define SPLASH_MEDIA_INFO_TYPES 3

/* 4 bytes each for identifier, size and offset. */
#define SPLASH_MEDIA_INFO_BYTES 4

#define SPLASH_MEDIA_INFO_SIZE (SPLASH_MEDIA_INFO_TYPES *\
				SPLASH_MEDIA_INFO_BYTES)

/* 4 bytes each for BRCM, VERSION */
#define SPLASH_IDENTIFIER_WIDTH 4

/* 4 bytes to show the total file size of the concatenated splashFile. */
#define SPLASH_TOTAL_SIZE_WIDTH 4

/* 4 bytes aligned. */
#define SPLASH_ALIGNMENT_WIDTH 4

/* Audio needs to be aligned at 32-byte boundary for the FMM. 32 bytes will be
mandatorily added to the pcm files while creating the combined splash file.
The pcm data will then be shifted to align it to 32 bytes while bolt reads pcm
data into DDR. */
#define SPLASH_AUDIO_ALIGNMENT_WIDTH 32

#define SPLASH_FILE_HDR_SIZE ((SPLASH_IDENTIFIER_WIDTH*2) +\
			SPLASH_TOTAL_SIZE_WIDTH + \
			(SPLASH_MEDIA_INFO_SIZE * SPLASH_MAX_MEDIA))

/* basic info of a media file */
enum SplashMediaType {
	SplashMediaType_eBoot,
	SplashMediaType_eOverTemp,
	SplashMediaType_eMax,
} SplashMediaType;

enum SplashMediaFormat {
	SplashMediaFormat_eBmp,
	SplashMediaFormat_ePcm,
	SplashMediaFormat_eMax,
} SplashMediaFormat;

struct SplashMediaInfo {
	uint32_t	size;
	uint32_t	offset;
	unsigned char	*buf;
};

int splash_load_media(char *filename);

struct SplashMediaInfo *splash_open_media(enum SplashMediaType type,
				enum SplashMediaFormat format, void *phMem);

#endif /* __SPLASH_MEDIA_H__ */

