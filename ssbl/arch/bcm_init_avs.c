/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <avs_bsu.h>
#include <avs_dvfs.h>
#include <board.h>
#include <lib_printf.h>

void board_init_avs(void)
{
#ifndef DVFS_SUPPORT
	avs_ssbl_init(0);
#else
	const struct dvfs_params *dvfs;
	unsigned int pmap_id;

	dvfs = board_dvfs();
	if (dvfs == NULL) {
		xprintf("AVS: cannot get board DVFS params!\n");
		return;
	} else if (is_pmap_valid(dvfs->pmap)) {
		xprintf("AVS: Invalid PMap in DVFS params!\n");
		return;
	}

	/* AVS could have overridden the requested PMAP based on
	 * the product ID. Notify the user if the current PMAP set
	 * is different than the board PMAP.
	 */
	pmap_id = avs_get_current_pmap();
	if (pmap_id != dvfs->pmap) {
		xprintf("AVS: PMap# %d (board default %d)\n",
				pmap_id, dvfs->pmap);
	}

	avs_ssbl_init(pmap_id);
#endif /* DVFS_SUPPORT */
}
