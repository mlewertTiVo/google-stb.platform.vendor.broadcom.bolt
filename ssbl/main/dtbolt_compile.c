/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bolt.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "devtree.h"


static unsigned char to_nibble(char c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	else if ((c >= 'A') && (c <= 'F'))
		return 10 + c - 'A';
	else if ((c >= 'a') && (c <= 'f'))
		return 10 + c - 'a';
	return 0xff;
}


static inline const char *skip_ws(const char *pi)
{
	while (*pi == ' ' || *pi == '\t')
		pi++;
	return pi;
}


static inline const char *skip_nws(const char *pi)
{
	while (*pi != ' ' && *pi != '\t')
		pi++;
	return pi;
}


static inline const char *skip_comma(const char *pi)
{
	if (*pi == ',')
		pi++;
	return pi;
}


static const char *skip_hex_or_dec(const char *pi)
{
	if (pi[0] == '0' && (pi[1] == 'x' || pi[1] == 'X')) {
		pi += 2;
		while(to_nibble(*pi) < 16)
			pi++;
	} else {
		while (*pi >= '0' && *pi <= '9')
			pi++;
	}
	return pi;
}


static unsigned char *write_uint32(uint32_t u, unsigned char *po,
			    unsigned char *po_limit)
{
	if (po + 4 > po_limit) {
		xprintf("%s: exceeded output buffer space!\n", __func__);
		return NULL;
	}
	*po++ = u >> 24;
	*po++ = u >> 16;
	*po++ = u >> 8;
	*po++ = u >> 0;
	return po;
}


static inline unsigned char *write_uint8(uint8_t u, unsigned char *po,
					 unsigned char *po_limit)
{
	if (po + 1 > po_limit) {
		xprintf("%s: exceeded output buffer space!\n", __func__);
		return NULL;
	}
	*po++ = u;
	return po;
}


static const char *read_uint32(void *fdt, const char *pi, uint32_t *u)
{
	if (*pi == '&') {
		char buf[128];
		char *pbuf = &buf[0];
		char *pbuf_limit = &buf[sizeof(buf) - 1];
		int x;

		pi++;
		while (*pi && *pi != ' ' && *pi != '\t' && *pi != '>') {
			if (pbuf > pbuf_limit)
				break;
			*pbuf++ = *pi++;
		}
		*pbuf = 0;
		x = bolt_devtree_phandle_from_path(fdt, buf);
		if (x < 0) {
			xprintf("%s: failed to get phandle of '&%s'!\n",
				__func__, buf);
			return NULL;
		}
		*u = (uint32_t) x;
	} else {
		*u = atoi(pi);
		pi = skip_hex_or_dec(pi);
	}
	return pi;
}


int bolt_devtree_compile_prop_value(void *fdt, const char *in, unsigned char *out,
				    int out_size)
{
	const char *pi = in;
	unsigned char *po = out;
	unsigned char *po_limit = po + out_size;
	const char *old_pi;

	do {

		pi = skip_ws(pi);
		if (*pi == '<') {
			pi = skip_ws(++pi);
			while (*pi != '>') {
				uint32_t u;
				pi = read_uint32(fdt, pi, &u);
				if (pi == NULL)
					goto fail;
				po = write_uint32(u, po, po_limit);
				if (po == NULL)
					goto fail;
				pi = skip_ws(pi);
			}
			pi++;

		} else if (*pi == '[') {
			pi = skip_ws(++pi);
			while (*pi != ']') {
				uint8_t n1 = to_nibble(*pi++);
				uint8_t n0 = to_nibble(*pi++);
				if (n0 >= 16 || n1 >= 16) {
					xprintf("%s: bad byte sequence!\n",
						__func__);
					goto fail;
				}
				po = write_uint8((n1 << 4) | n0,
						 po, po_limit);
				if (po == NULL)
					goto fail;
				pi = skip_ws(pi);
			}
			pi++;

		} else if (*pi == '"') {
			pi++;
			while (*pi && *pi != '"') {
				po = write_uint8(*pi++, po, po_limit);
				if (po == NULL)
					goto fail;
			}
			pi++;
			po = write_uint8(0, po, po_limit);
			if (po == NULL)
				goto fail;

		}
		pi = skip_ws(pi);
		old_pi = pi;
		pi = skip_comma(pi);

	} while (pi > old_pi);
	pi = skip_ws(pi);
	if (*pi) {
		xprintf("%s: unparsed chars remain: '%s'!\n", __func__, pi);
		goto fail;
	}
	return po - out;
fail:
	xprintf("%s: failed to parse prop val '%s'!\n", __func__, in);
	return -1;
}
