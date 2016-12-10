/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <avs_bsu.h>
#include <board.h>
#include <lib_printf.h>

void board_init_avs(void)
{
	const struct dvfs_params *dvfs;
	unsigned int pmap_id;
#ifndef SECURE_BOOT
	struct fsbl_info *inf;
#endif

	dvfs = board_dvfs();
	if (dvfs == NULL) {
		xprintf("AVS: cannot get board DVFS params!\n");
		return;
	}

#ifdef SECURE_BOOT
	pmap_id = dvfs->pmap;
#else
	inf = board_info();
	if (inf == NULL) {
		xprintf("AVS: cannot get board info!\n");
		return;
	}

	pmap_id = FSBL_HARDFLAG_PMAP_ID(inf->saved_board.hardflags);
	if (pmap_id == FSBL_HARDFLAG_PMAP_BOARD) {
		/* respect the board default PMap configuration */
		pmap_id = dvfs->pmap;
	} else {
		if (pmap_id != dvfs->pmap) {
			xprintf("AVS: PMap# %d (board default %d)\n",
				pmap_id, dvfs->pmap);
			/* The user selected PMap has already been applied in
			 * FSBL. It is just the pstate of the PMap that is
			 * being applied at this moment, which should not
			 * cause any crash or instability.
			 */
		}
	}
#endif

	avs_ssbl_init(pmap_id);
}
