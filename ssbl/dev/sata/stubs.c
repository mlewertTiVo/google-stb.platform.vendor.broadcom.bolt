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

#include "os.h"

#define INT_MAX ((int)(~0U>>1))

FILE *stderr;

void _start(void)
{
	return;
}

WEAK uint32_t __aeabi_uidivmod(uint32_t x, uint32_t y)
{
	x = x;
	y = y;
	return 0;
}

WEAK int printf(const char *fmt, ...)
{
	fmt = fmt;

	return 0;
}

WEAK int fprintf(FILE *fp, const char *templat, ...)
{
#if defined(_BOLT_)
	va_list marker;
	int count;

	va_start(marker, templat);
	count = xvprintf(templat, marker);
	va_end(marker);

	return count;
#else
	fp = fp;
	fmt = fmt;

	return 0;
#endif
}

#if !defined(_BOLT_)
WEAK void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;

	while (n--)
		*p++ = (unsigned char)c;

	return s;
}
#endif

WEAK void *malloc(size_t n)
{
#ifdef KMALLOC
	return KMALLOC(n, 4);
#else
	n = n;
	return NULL;
#endif
}

WEAK void usleep(unsigned long t)
{
#if defined(_BOLT_)
	if (t > INT_MAX)
		t = INT_MAX;
	bolt_usleep((int)t);
#else
	t = t;
#endif
}

WEAK void msleep(unsigned long t)
{
#if defined(_BOLT_)
	if (t > INT_MAX)
		t = INT_MAX;
	bolt_msleep((int)t);
#else
	t = t;
#endif
}

WEAK void __assert(int x)
{
	x = x;
	return;
}

WEAK int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	clk_id = clk_id;
	tp = tp;
	return 0;
}

WEAK int rand()
{
	return 0;
}

WEAK void srand(unsigned int seed)
{
	seed = seed;
}

static inline void brcm_sata3_mdio_wr_reg(uint32_t port, uint32_t bank,
					  uint32_t ofs, uint32_t msk,
					  uint32_t value, uint32_t regbase)
{
	uint32_t       tmp;
	const uint32_t base = SATA_MDIO_BASE(regbase, port);

	*((volatile uint32_t *)(base + SATA_MDIO_BANK_OFFSET)) = bank;
	tmp = *((volatile uint32_t *)(base + SATA_MDIO_REG_OFFSET(ofs)));
	tmp = (tmp & msk) | value;
	*((volatile uint32_t *)(base + SATA_MDIO_REG_OFFSET(ofs))) = tmp;
}

WEAK void init_phy(int port, uint32_t base)
{
	/* Locate any MDIO workarounds here. */
	return;
}
