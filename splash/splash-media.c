/***************************************************************************
 *	 Copyright (c) 2015, Broadcom Corporation
 *	 All Rights Reserved
 *	 Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "error.h"
#include "devfuncs.h"
#if CFG_ZLIB
#include "zlib.h"
#endif

#include "bsp_config.h"
#include "board.h"
#include "board_init.h"

#include "splash_magnum.h"
#include "splash_script_load.h"
#include "splash-media.h"
#include "splash-uncompress.h"


#define IS_NEWSPLASH_SIGNATURE(p) \
	((p[0] == 'B' && p[1] == 'R' && p[2] == 'C' && p[3] == 'M'))

#define IS_BMP_SIGNATURE(p) \
	((p[0] == 'B' && p[1] == 'M'))

#define IS_ZB_SIGNATURE(p) \
	((p[0] == 'G' && p[1] == 'Z' && p[2] == 'B' && p[3] == 'R'))


static unsigned char *g_splashFile;
static struct SplashMediaInfo
	g_SplashMedia[SplashMediaType_eMax][SplashMediaFormat_eMax];

struct zbhdr {
	uint32_t signature;
	uint32_t ucsize;
	uint32_t csize;
	uint32_t uc_crc;
};

static int splash_copy_media_to_cma(struct SplashMediaInfo *mediaInfo,
						void *phMem)
{
	int rc = 0;
	void *audioBuf;
	unsigned char *source = NULL;
	BMEM_Handle *memHandle = (BMEM_Handle *)phMem;

	audioBuf = BMEM_Alloc(*(memHandle), mediaInfo->size);
	if (!audioBuf) {
		rc = BOLT_ERR_NOMEM;
		goto done;
	}

	source = g_splashFile+mediaInfo->offset;

	memcpy(audioBuf, source, mediaInfo->size);
	mediaInfo->buf = audioBuf;

	/* flush cached addr */
	BMEM_Heap_FlushCache(*(memHandle+0), audioBuf, mediaInfo->size);
done:
	return rc;
}


int splash_load_media(char *bfile)
{
	int n, h;
#if CFG_ZLIB
	struct zbhdr *z;
	uint8_t *saved, *ucbuff = NULL;
	int rc = 0;
	unsigned int crc;
#endif
	h = bolt_open(bfile);
	if (h < 0) {
		err_msg("SPLASH: bad file '%s'", bfile);
		return h;
	}

	/* TBD: get from partition size? */
	g_splashFile = KMALLOC(MAX_SPLASH_SIZE, 0);
	if (!g_splashFile) {
		err_msg("SPLASH: heap alloc failed");
		return BOLT_ERR_NOMEM;
	}

	n = bolt_read(h, g_splashFile, MAX_SPLASH_SIZE);
	if (n != MAX_SPLASH_SIZE) {
		err_msg("SPLASH: file read failed");
		KFREE(g_splashFile);
		return BOLT_ERR_IOERR;
	}

	bolt_close(h);
#if CFG_ZLIB
	saved = g_splashFile;

	if (!IS_ZB_SIGNATURE(g_splashFile))
		goto done;

	z = (struct zbhdr *)g_splashFile;
	if (!z->ucsize || !z->csize) {
		err_msg("SPLASH: bad compressed file");
		rc = BOLT_ERR;
		goto err;
	}

	ucbuff = KMALLOC(z->ucsize, 0);
	if (!ucbuff) {
		err_msg("SPLASH: cannot alloc");
		rc = BOLT_ERR_NOMEM;
		goto err;
	}

	g_splashFile += sizeof(struct zbhdr);

	rc = splash_uncompress(ucbuff, &z->ucsize, g_splashFile, z->csize);
	if (rc != Z_OK) {
		err_msg("SPLASH: uncompress failed");
		rc = BOLT_ERR;
		goto err;
	}

	crc = ~lib_crc32(ucbuff, z->ucsize);
	if (crc != z->uc_crc) {
		err_msg("SPLASH: crc mismatch %x != %x", crc, z->uc_crc);
		rc = BOLT_ERR;
		goto err;
	}

	goto ok;
err:
	KFREE(saved);
	KFREE(ucbuff);
	return rc;
ok:
	KFREE(saved);
	g_splashFile = ucbuff;
done:
#endif
	CACHE_FLUSH_ALL();
	return 0;
}


struct SplashMediaInfo *splash_open_media(
	enum SplashMediaType type, enum SplashMediaFormat format, void *phMem)
{
	int rc = 0;
	unsigned char *ptr = g_splashFile;
	char buf[4];
	uint32_t *tmpBuf = NULL, *wordPtr = (uint32_t *)ptr, totalSize = 0;
	struct SplashMediaInfo *mediaInfo = NULL;
	BMEM_Handle *memHandle = (BMEM_Handle *)phMem;

	if (!g_splashFile)
		goto error;

	mediaInfo = &g_SplashMedia[type][format];

	/* We ignore the BSEAV defined name in g_SplashSurfaceInfo[] and use
	one from BOLT instead. For a demo, flash the splashFile created by
	splash_create_flash_file tool in /scripts/
	*/
	switch (format) {
	case SplashMediaFormat_eBmp:
		buf[0] = 'b';
		buf[1] = 'm';
		buf[2] = 'p';
		break;
	case SplashMediaFormat_ePcm:
		buf[0] = 'p';
		buf[1] = 'c';
		buf[2] = 'm';
		break;
	case SplashMediaFormat_eMax:
	default:
		rc = BOLT_ERR_IOERR;
		goto error;
	}

	switch (type) {
	case SplashMediaType_eBoot:
		buf[3] = '0';
		break;
	case SplashMediaType_eOverTemp:
		buf[3] = '1';
		break;
	case SplashMediaType_eMax:
	default:
		rc = BOLT_ERR_IOERR;
		goto error;
	}

	if (IS_NEWSPLASH_SIGNATURE(ptr)	&& (wordPtr[1] == SPLASH_FILE_VER)) {
		/* Skip over the signature and version
		 * to now point to the total file size
		 * entry in the file.
		 */
		wordPtr += 2;
		goto new_format;
	} else if (IS_BMP_SIGNATURE(ptr)) {

		/* no audio support for the old bitmap only splash */
		if (format == SplashMediaFormat_ePcm) {
			mediaInfo->buf = NULL;
			mediaInfo->offset = 0;
			mediaInfo->size = 0;
			goto done;
		}

		xprintf("SPLASH: Old format file in flash. ");
		xprintf("Use splash_create_flash_file ");
		xprintf("to create newer format.\n");

		/* skip over two bytes for BM. */
		ptr += 2;
		/* get size (embedded in the BMP file) */
		wordPtr = (uint32_t *)ptr;
		totalSize = *wordPtr;

		mediaInfo->buf = g_splashFile;
		mediaInfo->offset = 0;
		mediaInfo->size = totalSize;
		goto done;

	} else {
		rc = BOLT_ERR;
		warn_msg("SPLASH: Invalid format, or unprogrammed");
		goto error;
	}

new_format:
	tmpBuf = (uint32_t *)buf;

	/* skip over four bytes for BRCM and four for version info. */
	ptr += (SPLASH_IDENTIFIER_WIDTH*2);

	totalSize = *wordPtr;
	wordPtr++;

	for (; wordPtr < (uint32_t *)(g_splashFile+SPLASH_FILE_HDR_SIZE);
				wordPtr += SPLASH_MEDIA_INFO_TYPES) {
		if (*wordPtr == *tmpBuf)
			goto found;
	}
	goto error; /* media format & type not found */

found:
	wordPtr++;
	mediaInfo->size = *wordPtr++;
	mediaInfo->offset = *wordPtr;

	if (format == SplashMediaFormat_ePcm) {
		rc = splash_copy_media_to_cma(mediaInfo, memHandle);
		if (rc)
			goto error;
	} else
		mediaInfo->buf = g_splashFile + mediaInfo->offset;
done:
	return mediaInfo;

error:
	return NULL;
}

