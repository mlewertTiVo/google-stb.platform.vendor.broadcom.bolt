/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bchp_common.h>
#include <board.h>
#include <board_init.h>
#include <common.h>
#include <lib_malloc.h>
#include <reset_history.h>
#include <ssbl-common.h>

#include <stdbool.h>

#ifdef BCHP_AON_CTRL_REG_START
#include <bchp_aon_ctrl.h>
#endif

static const unsigned int NUM_POSSIBLE_RESET_REASONS =
	ARRAY_SIZE(reset_reasons);
static uint32_t g_reset_history;

static bool latch_reset_history(void)
{
	static bool has_been_latched = false;

	if (has_been_latched)
		return true;

#ifdef BCHP_AON_CTRL_RESET_HISTORY
	g_reset_history = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY);

	/* Clear the reset history in case we reset again */
	BDEV_WR(BCHP_AON_CTRL_RESET_CTRL, 1);
	BDEV_WR(BCHP_AON_CTRL_RESET_CTRL, 0);

	has_been_latched = true;
	return true;
#endif

	return false;
}

uint32_t board_init_reset_history_value(void)
{
	latch_reset_history();

	return g_reset_history;
}

static void append_reason(char **dest, const char *src)
{
	char *ptr = *dest;

	while (*src)
		*ptr++ = *src++;

	*ptr++ = ',';
	*dest = ptr;
}

char *board_init_reset_history_string(void)
{
	static char *b;
	char *s;
	unsigned int i;

	if (!latch_reset_history())
		return NULL;

	if (b) /* already done */
		return b;

	b = KMALLOC(RESET_HISTORY_TOTAL_STRLENS, 0);
	if (!b)
		return b;

	memset(b, 0, RESET_HISTORY_TOTAL_STRLENS);

	s = b;
	for (i = 0; i < NUM_POSSIBLE_RESET_REASONS; ++i)
		if (g_reset_history & reset_reasons[i].mask)
			append_reason(&s, reset_reasons[i].name);

	/* rm any trailing comma. */
	if ((s != b) && (s[-1] == ','))
		s[-1] = '\0';

	return b;
}

static void print_reset_history(void)
{
#ifdef SECURE_BOOT
	/* do nothing */
	return;
#else
	unsigned int count, i;

	xprintf("0x%08x", g_reset_history);

	count = 0;
	for (i = 0; i < NUM_POSSIBLE_RESET_REASONS; ++i) {
		if (g_reset_history & reset_reasons[i].mask) {
			xprintf(" %s", reset_reasons[i].name);
			count++;
		}
	}

	xprintf(" (%d of %d possible causes)\n",
		count, NUM_POSSIBLE_RESET_REASONS);
#endif
}

void board_init_reset_history(void)
{
	if (!latch_reset_history()) {
#ifndef SECURE_BOOT
		xprintf("unsupported on this chip\n");
#endif
		return;
	}

	print_reset_history();
}
