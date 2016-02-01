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

#include "board_init.h"
#include "ssbl-common.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_malloc.h"

#include "bchp_common.h"

#ifdef BCHP_AON_CTRL_REG_START
#include "bchp_aon_ctrl.h"
#ifdef BCHP_AON_CTRL_RESET_HISTORY
#include "aon_history.h"
#endif
#endif


/* SWBOLT-99
*/
static uint32_t g_aon_history;

uint32_t get_aon_reset_history(void)
{
	return g_aon_history;
}

static void __maybe_unused aon_append(char **dest, const char *src)
{
	char *ptr = *dest;

	while (*src)
		*ptr++ = *src++;

	*ptr++ = ',';
	*dest = ptr;
}

char *aon_reset_as_string(void)
{
#if defined(BCHP_AON_CTRL_REG_START) && \
	defined(BCHP_AON_CTRL_RESET_HISTORY)
	static char *b;
	char *s;
	int  i = 0;

	if (b) /* already done */
		return b;

	b = KMALLOC(AON_HISTORY_TOTAL_STRLENS, 0);
	if (!b)
		return b;

	memset(b, 0, AON_HISTORY_TOTAL_STRLENS);

	s = b;
	while (aon[i].mask != 0) {
		if (g_aon_history & aon[i].mask)
			aon_append(&s, aon[i].name);
		i++;
	}

	/* rm any trailing comma. */
	if ((s != b) && (s[-1] == ','))
		s[-1] = '\0';

	return b;
#else
	return NULL;
#endif
}


void aon_reset_history(void)
{
#if defined(BCHP_AON_CTRL_REG_START) && \
	defined(BCHP_AON_CTRL_RESET_HISTORY)
	int __maybe_unused count = 0, i = 0;

	g_aon_history = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY);

	/* Clear the reset history in case we reset again
	*/
	BDEV_WR(BCHP_AON_CTRL_RESET_CTRL, 1);
	BDEV_WR(BCHP_AON_CTRL_RESET_CTRL, 0);

#ifndef SECURE_BOOT
	xprintf("%#08x", g_aon_history);

	while (aon[i].mask != 0) {
		if (g_aon_history & aon[i].mask) {
			xprintf(" %s", aon[i].name);
			count++;
		}
		i++;
	}
	xprintf(" (%d of %d possible causes)\n", count, i);
#endif
#else
#ifndef SECURE_BOOT
	xprintf("unsupported on this chip\n");
#endif
	g_aon_history = 0;
#endif
}
