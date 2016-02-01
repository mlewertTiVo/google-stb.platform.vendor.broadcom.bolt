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

/*
 * The below copyright notice is from the original version in zlib-1.2.8.  The
 * uncompress implementation here is almost entirely based on the zlib version,
 * and has been minimally adjusted to use the BOLT allocation routines.
 */

/* Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950
  (zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).
*/

#include "splash-uncompress.h"

#include <lib_malloc.h>
#include <lib_string.h>
#include <zlib-helpers.h>

#include <stddef.h>
#include <stdint.h>
#include <zlib.h>

int splash_uncompress(uint8_t *dst, uint32_t *dst_len,
		const uint8_t *src, uint32_t src_len)
{
	z_stream stream;
	int err;

	stream.next_in = (z_const Bytef *)src;
	stream.avail_in = src_len;
	stream.next_out = dst;
	stream.avail_out = *dst_len;

	stream.zalloc = bolt_zcalloc;
	stream.zfree = bolt_zcfree;

	err = inflateInit(&stream);
	if (err != Z_OK)
		return err;

	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		inflateEnd(&stream);
		if (err == Z_NEED_DICT ||
		    (err == Z_BUF_ERROR && stream.avail_in == 0))
			return Z_DATA_ERROR;
		return err;
	}
	*dst_len = stream.total_out;

	err = inflateEnd(&stream);
	return err;
}
