/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>
#include "psci32.h"
#include <bchp_common.h>
#include <bchp_cntcontrolbase.h>

void __puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			uart_putc('\r');
		uart_putc(*(s++));
	}
}


int puts(const char *s)
{
	__puts(s);
	uart_putc('\r');
	uart_putc('\n');
	return 0;
}


void udelay(uint32_t us)
{
	uint32_t now, f, duration, elapsed;

	f = rdb_read(BCHP_CNTControlBase_CNTFID0);
	now = rdb_read(BCHP_CNTControlBase_CNTCV_LO);
	duration = (f / 1000000) * us;

	do {
		elapsed = rdb_read(BCHP_CNTControlBase_CNTCV_LO) - now;
	} while (elapsed < duration);
}


void mdelay(uint32_t ms)
{
	uint32_t i;

	for (i = 0; i < ms; i++)
		udelay(1000);
}


void memcpy(void *dst, const void *src, size_t n)
{
	uint8_t *d = dst;
	const uint8_t *s = src;

	while (n--)
		*(d++) = *(s++);
}


void *memset(void *s, int c, size_t n)
{
	unsigned char *x = (unsigned char *)s;

	while (n--)
		*(x++) = c;
	return s;
}

void hexdump(uint32_t mem, int numitems)
{
	int a, b = 0;
	volatile uint32_t *v = (volatile uint32_t *)mem;

	puts("");
	for(a = 0; a < numitems; a++) {
		if (!b) {
			writehex((uint32_t)&v[a]);
			__puts(": ");
		}

		writehex((uint32_t)v[a]);
		__puts(" ");
		if (b >= 3) {
			puts("");
			b = 0;
		} else
			b++;
	}
	puts("");
}

void writehex(uint32_t val)
{
	unsigned int i, c;

	for (i = 0; i < 8; i++, val <<= 4) {
		c = (val >> 28) + '0';
		if (c > '9')
			c += 'a' - '9' - 1;
		uart_putc(c);
	}
}

void writehex64(uint64_t val)
{
	unsigned int i, c;

	for (i = 0; i < 16; i++, val <<= 4) {
		c = (val >> 60) + '0';
		if (c > '9')
			c += 'a' - '9' - 1;
		uart_putc(c);
	}
}


#define DIV10(a) ((a)/10)
#define MOD10(a) ((a)-(10*DIV10(a)))

static char *_itoa(int n, char s[])
{
	int _n = n;
	unsigned int d = 1;

	while ((_n = DIV10(_n)))
		d++;

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

void writeint(int n)
{
	static char str[11];

	_itoa(n, str);
	__puts(str);
}

void msg_cpu(const char *s, unsigned int cpu)
{
	__puts(s);
	__puts("-CPU");
	writeint(cpu); /* index (cpu+cluster) */
	__puts(" ");
}
