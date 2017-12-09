/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <board.h>
#include <board_init.h>
#include <ddr.h>
#include <env_subr.h>
#include <error.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_types.h>
#include <rts.h>
#include <ssbl-common.h>

#include <stdbool.h>

static void setup_rts(unsigned long base, uint32_t *tbl)
{
	uint32_t i, c;

	c = tbl[0];
	tbl++;

	for (i = 0; i < c; i++)
		BDEV_WR(base + (i<<2), tbl[i]);
}

/* current_rts -- returns data structure of currently selected BOX mode
 *
 * If an RTS data structure that corresponds to the currently selected
 * BOX mode is available, a pointer to the data structure is returned.
 * If not, a pointer to the first available data structure is returned.
 *
 * NOTE: With the introduction of BOX mode #0, BOLT is assumed to apply
 *       only the BOX mode #0. By being able to return a pointer to
 *       a non-zero BOX mode, such an assumption might be broken.
 *
 *       But, it should be okay. In reality, only the BOX mode #0 will
 *       be prepared and integrated into a BOLT build.
 *
 *       Returning a non-zero BOX mode becomes possible only when an
 *       extra BOX mode(s) is integrated and the BOX mode is selected
 *       via either the board configuratoin or the ENVSTR_BOXMODE
 *       environment variable.
 *
 *       The ability of returning a non-zero BOX mode remains here to
 *       handle special cases. For example, a customer may want to
 *       apply the targetted BOX mode before running splash if the BOX
 *       mode #0 does not satisfy their use case.
 *
 * Returns:
 *  pointer to an RTS coniguratoin data structure
 */
static struct rts *current_rts(void)
{
	int i;
	unsigned int boxmode;

	boxmode = board_init_rts_current_boxmode();
	for (i = 0; i < num_rts_cfgs; i++) {
		if (rts_cfgs[i]->rtsdefault == boxmode)
			return rts_cfgs[i];
	}

	return rts_cfgs[0];
}

/* board_init_rts_current_boxmode -- returns currently selected BOX mode
 *
 * If ENVSTR_BOXMODE is defined, its value will be returned unless it is
 * negative. If negative, 0 (zero) is returned. If ENVSTR_BOXMODE is not
 * defined, the board default BOX mode is returned. If the board default
 * is not available, 0 (zero) is returned.
 *
 * Returns:
 *  currently selected BOX mode number
 */
int board_init_rts_current_boxmode(void)
{
	char *str;
	int boxmode;
	const struct ssbl_board_params *p;

	/* environment variable first */
	str = env_getenv(ENVSTR_BOXMODE);
	if (str != NULL) {
		boxmode = atoi(str);
		if (boxmode < 0)
			return 0;

		return boxmode;
	}

	/* board default */
	p = board_current_params();
	if (p != NULL)
		return p->rtsdefault;

	return 0;
}

/* board_init_rts -- applies currenly selected RTS settings */
void board_init_rts(void)
{
	unsigned int i;
	struct rts *r;
	struct board_type *b;

	r = current_rts();
	if (r == NULL) {
		err_msg("RTS: not programed!\n");
		return;
	}

	b = board_thisboard();
	if (b == NULL) {
		err_msg("RTS: cannot get board information!");
		return;
	}

	for (i = 0; i < b->nddr; i++) {
		/* no need to apply RTS for MEMC that is not active */
		if (!ddr_is_populated(&(b->ddr[i])))
			continue;

		if (0 == rts_bases[i]) {
			/* "rtsbase n ..." (where n == i) in
			 * 'config/family-${FAMILY}.cfg' may be bad.
			 */
			info_msg("RTS: not programed for MEMC%d", i);
			continue;
		}

		/* The number of lists (of values) in
		 * 'config/family-${FAMILY}-box*.rts' file(s) should be
		 * equal to or bigger than the number of MEMC's that are
		 * active on your chip/board.
		 */
		if (i >= r->number) {
			warn_msg("RTS: missing for MEMC%d in file ID %d",
				i, r->rtsdefault);
			continue;
		}

		setup_rts(rts_bases[i], r->values[i]);
	}
}

/* board_init_rts_update -- updates BOX mode selection
 *
 * A new BOX mode selection is stored under the name of ENVSTR_BOXMODE
 * in a flash partition unless the new selection is -1. If it is -1,
 * the environment variable, ENVSTR_BOXMODE, is removed if exists.
 *
 * Parameter:
 *  rts_id [in] new BOX mode selection
 *
 * Returns:
 *  return value of env_delenv() if rts_id == -1,
 *  return value of env_setenv() otherwise
 */
int board_init_rts_update(int rts_id)
{
	char str[16]; /* should be enough for a decimal number */
	int retval;

	if (rts_id == -1) {
		retval = env_delenv(ENVSTR_BOXMODE);
	} else {
		xsprintf(str, "%d", rts_id);
		retval = env_setenv(ENVSTR_BOXMODE, str, ENV_FLG_NORMAL);
	}

	if (retval == BOLT_OK)
		env_save();

	return retval;
}
