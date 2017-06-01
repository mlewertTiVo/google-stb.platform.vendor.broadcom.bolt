/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides the methods used to pass DVFS parameters to the AVS firmware
 *
***************************************************************************/

#ifndef _DVFS_H_
#define _DVFS_H_

#include <stdint.h>

enum avs_mode {
	avs_mode_e = 0,
		/* To be used in boards that do not support DVFS.
		** AVS firmware does not adjust CPU's frequency or its voltage.
		** It maintains core VDDC at V0 (AVS voltage).
		** Only PMAP commands are entertained to cover Boost mode
		** scenarios (i.e., V0 is boosted to support 1.7 GHz)
		** All P-state commands are ignored by AVS firmware as DVFS
		** is not supported. */
	dfs_mode_e,
		/* To be used in boards that support frequency change only.
		** AVS firmware adjusts CPU's frequency to target P-state.
		** CPU_VDDC held at V0 (AVS) voltage. */
	dvfs_mode_e = 3,
		/* To be used in boards that support DVFS. */
	max_mode_e
		/* Max enum for bounds checking. */
} avs_mode;


/* This defines the parameters that make up a pmap.
** This defines the parameters used to set up the ARM CPU poser states.
*/
struct pmap_parameters {
	uint16_t ndiv_int;
	uint8_t pdiv;
	uint8_t mdiv_p0;
	uint8_t mdiv_p1;
	uint8_t mdiv_p2;
	uint8_t mdiv_p3;
	uint8_t mdiv_p4;
} pmap_parameters;

enum AVS_ERROR {
	AVS_NO_ERROR = 0,
	AVS_SUCCESS = 0,
	/* results 1-9 reserved to match firmware response errors */
	AVS_BAD_MODE = 10,  /* supplied a bad AVS mode */
	AVS_BAD_STATE,      /* supplied a bad AVS state */
	AVS_BAD_PARAMETER,  /* supplied a bad ??? */
} AVS_ERROR;

#ifdef __cplusplus
extern "C" {
#endif

/* Set the AVS mode and PMap to be used by the AVS firmware */
/* Note: this should ONLY be used on a resume from S3 as this was initially
set by SSBL at boot */
enum AVS_ERROR avs_set_pmap(enum avs_mode mode, struct pmap_parameters *map,
				unsigned initial_state);

/* Get the current AVS mode and PMap being used by the AVS firmware */
enum AVS_ERROR avs_get_pmap(enum avs_mode *mode, struct pmap_parameters *map);

/* Use this to change the state (0-4) */
enum AVS_ERROR avs_set_state(uint32_t state);

/* Use this to get the gurrent state (0-4) */
enum AVS_ERROR avs_get_state(uint32_t *state);

void dvfs_init_board_pmap(int pmap_id);

#ifdef __cplusplus
}
#endif

#endif/*_DVFS_H_*/

