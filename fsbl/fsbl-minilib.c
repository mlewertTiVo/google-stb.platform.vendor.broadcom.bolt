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

#include <common.h>
#include <lib_types.h>
#include "fsbl.h"


void memcpy4(uint32_t *dst, const uint32_t *src, size_t bytes)
{
	uint32_t i;
	for (i = 0; i < bytes; i += 4)
		*(dst++) = *(src++);
}


#define FASTCOPY_STRIDE 32
#define FASTCOPY_STRIDE_WORDS (FASTCOPY_STRIDE / sizeof(uint32_t))
#define FASTCOPY_STRIDE_MASK (FASTCOPY_STRIDE - 1)

typedef struct {
	uint32_t a[FASTCOPY_STRIDE_WORDS];
} fastcopy_t;

void fastcopy(uint32_t *dst, const uint32_t *src, size_t bytes)
{
	if (!((uintptr_t)dst & FASTCOPY_STRIDE_MASK) &&
	    !((uintptr_t)src & FASTCOPY_STRIDE_MASK) &&
	    ((bytes > FASTCOPY_STRIDE) && !(bytes & FASTCOPY_STRIDE_MASK))) {
		do {
			*((fastcopy_t *)dst) = *((const fastcopy_t *)src);
			dst += FASTCOPY_STRIDE_WORDS;
			src += FASTCOPY_STRIDE_WORDS;
			bytes -= FASTCOPY_STRIDE;
		} while (bytes != 0);
	} else
		memcpy(dst, src, bytes);
}


void memcpy(void *dst, const void *src, size_t n)
{
	/* Only use memcpy4() if all inputs are 32-bit aligned */
	if (IS_ALIGNED((uintptr_t)dst, sizeof(uint32_t)) &&
	    IS_ALIGNED((uintptr_t)src, sizeof(uint32_t)) &&
	    IS_ALIGNED(n, sizeof(uint32_t)))
		memcpy4(dst, src, n);
	else {
		uint8_t *d = dst;
		const uint8_t *s = src;

		while (n--)
			*(d++) = *(s++);
	}
}


void *memset(void *s, int c, size_t n)
{
	unsigned char *x = (unsigned char *)s;
	while (n--)
		*(x++) = c;
	return s;
}


void __puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			putchar('\r');
		putchar(*(s++));
	}
}

void crlf(void)
{
	putchar('\r');
	putchar('\n');
}

int puts(const char *s)
{
	__puts(s);
	crlf();
	return 0;
}


void writehex(uint32_t val)
{
	unsigned int i, c;

	for (i = 0; i < 8; i++, val <<= 4) {
		c = (val >> 28) + '0';
		if (c > '9')
			c += 'a' - '9' - 1;
		putchar(c);
	}
}


/* this is more normal definition of itoa */
static char *_itoa(int n, char s[])
{
	int _n = n;
	unsigned int d = 1;

	while ((_n = DIV10(_n))) d++;
	if (n < 0) {
		s[0] = '-';
		n = -n;
		d++;
	}
	s[d--] = '\0';
	do {
		s[d--] = MOD10(n) + '0';
		n = DIV10(n);
	} while (n);
	return s;
}


void report_hex(const char *s, uint32_t h)
{
	if (s[0] == '@')
		__puts(&s[1]);
	else
		__puts(s);

	writehex(h);

	if (s[0] != '@')
		crlf();
}


char *itoa(int n)
{
	static char ret[11];
	_itoa(n, ret);
	return ret;
}


#if CFG_STACK_PROTECT_FSBL
uintptr_t __stack_chk_guard = FSBL_STACK_CHECK_VAL;

void __noreturn __stack_chk_fail(void)
{
	sys_die(DIE_STACK_CHECK_FAIL, "stack check");
}
#endif
