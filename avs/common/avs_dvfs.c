/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides the methods used to pass DVFS parameters to the AVS firmware
 *
***************************************************************************/

#include <stdint.h>
#include "common.h"

#include "lib_printf.h"

#include "bchp_avs_cpu_data_mem.h"
#include "bchp_avs_cpu_l2.h"
#include "bchp_avs_host_l2.h"
#include "avs_fw_interface.h"
#include "avs_dvfs.h"
#include "board.h"
#include "pmap.h"
#include "avs_bsu.h"


/* Use this to signal AVS CPU that a command is read for processing */
static void send_command(uint32_t command,
	uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
	struct at_runtime *issue =
		(struct at_runtime *)BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE;

	BDEV_WR(&issue->status, 0);
	BDEV_WR(&issue->command_p0, p0);
	BDEV_WR(&issue->command_p1, p1);
	BDEV_WR(&issue->command_p2, p2);
	BDEV_WR(&issue->command_p3, p3);
	BDEV_WR(&issue->command, command);

	/* tell firmware to wake-up */
	BDEV_WR(BCHP_AVS_CPU_L2_SET0, BCHP_AVS_CPU_L2_SET0_SW_INTR_MASK);
}

/* Use this to wait for response from AVS. Parameters p0-p3 are optional and,
if set, will be filled in by firmware parameters */
static enum AVS_ERROR wait_for_response(
	uint32_t *p0, uint32_t *p1, uint32_t *p2, uint32_t *p3)
{
	struct at_runtime *receive =
		(struct at_runtime *)BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE;
	uint32_t status;

	/* status will be cleared by firmware on completion */
	do {
		status = BDEV_RD(&receive->status);
	} while ((!status) || (status > 0xFF));

	if (p0)
		*p0 = BDEV_RD(&receive->command_p0);
	if (p1)
		*p1 = BDEV_RD(&receive->command_p1);
	if (p2)
		*p2 = BDEV_RD(&receive->command_p2);
	if (p3)
		*p3 = BDEV_RD(&receive->command_p3);
	BDEV_WR(&receive->status, 0);

	return (enum AVS_ERROR)status;
}

/* Set the PMap to be used by the AVS firmware */
/* Note: this should ONLY be used on a resume from S3
as this was initially set by SSBL at boot */
enum AVS_ERROR avs_set_pmap(enum avs_mode mode, struct pmap_parameters *map,
				unsigned initial_state)
{
	uint32_t command_p1, command_p2, command_p3;

	switch (mode) {
	case avs_mode_e:
	case dfs_mode_e:
	case dvfs_mode_e:
		break;
	default:
		return AVS_BAD_MODE; /* illegal mode */
	}

/* ndiv_int = 0Xa7  pdiv = 0X3  mdiv_p0 = 0X2
mdiv_p1 = 0X3  mdiv_p2 = 0X4  mdiv_p3 = 0X6  mdiv_p4 = 0Xa */

	command_p1 =
		((map->ndiv_int & NDIV_INT_MASK) << NDIV_INT_SHIFT) |
		((map->pdiv & PDIV_MASK) << PDIV_SHIFT) |
		((map->mdiv_p0 & MDIV_P0_MASK) << MDIV_P0_SHIFT);

	command_p2 =
		((map->mdiv_p1 & MDIV_P1_MASK) << MDIV_P1_SHIFT) |
		((map->mdiv_p2 & MDIV_P2_MASK) << MDIV_P2_SHIFT) |
		((map->mdiv_p3 & MDIV_P3_MASK) << MDIV_P3_SHIFT) |
		((map->mdiv_p4 & MDIV_P4_MASK) << MDIV_P4_SHIFT);

	command_p3 = initial_state;

	send_command(CMD_SET_P_MAP, mode, command_p1, command_p2, command_p3);

	return wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_get_pmap(enum avs_mode *mode, struct pmap_parameters *map)
{
	uint32_t command_p1, command_p2;
	enum AVS_ERROR status;

	send_command(CMD_GET_P_MAP, 0, 0, 0, 0);
	status = wait_for_response(mode, &command_p1, &command_p2, 0);
	if (status)
		return status;

	if (map) {
		/* Note that 'mode' was already set by the 'wait' call */
		map->ndiv_int = (command_p1 >> NDIV_INT_SHIFT) & NDIV_INT_MASK;
		map->pdiv     = (command_p1 >> PDIV_SHIFT) & PDIV_MASK;
		map->mdiv_p0  = (command_p1 >> MDIV_P0_SHIFT) & MDIV_P0_MASK;
		map->mdiv_p1  = (command_p2 >> MDIV_P1_SHIFT) & MDIV_P1_MASK;
		map->mdiv_p2  = (command_p2 >> MDIV_P2_SHIFT) & MDIV_P2_MASK;
		map->mdiv_p3  = (command_p2 >> MDIV_P3_SHIFT) & MDIV_P3_MASK;
		map->mdiv_p4  = (command_p2 >> MDIV_P4_SHIFT) & MDIV_P4_MASK;
	}

	return AVS_SUCCESS;
}

/* Use this to change the state (0-4) */
enum AVS_ERROR avs_set_state(uint32_t state)
{
	/* Don't send an illegal state */
	if (state <= P_STATE_4) {
		send_command(CMD_SET_P_STATE, state, 0, 0, 0);
		return wait_for_response(0, 0, 0, 0);
	}
	return AVS_BAD_STATE; /* illegal state */
}

enum AVS_ERROR avs_get_state(uint32_t *state)
{
	send_command(CMD_GET_P_STATE, 0, 0, 0, 0);
	return wait_for_response(state, 0, 0, 0);
}


void dvfs_init_board_pmap(int pmap_id)
{
	const struct dvfs_params *dvfs = board_dvfs();
	bool firmware_running;

	avs_get_data(0, 0, &firmware_running);
	if (!firmware_running){
		xprintf("Error: AVS firmware not running\n");
		return;
	}

	if (pmap_id >= PMapMax) {
		xprintf("DVFS: PMap%d is not supported\n",pmap_id);
		return;
	}

	if ((dvfs->mode == dfs_mode_e) || (dvfs->mode == dvfs_mode_e)) {
		struct pmap_parameters pmap = {
			.ndiv_int	= pmapTable[pmap_id].ndiv_int,
			.pdiv		= pmapTable[pmap_id].pdiv,
			.mdiv_p0	= pmapTable[pmap_id].mdiv_p0,
			.mdiv_p1	= pmapTable[pmap_id].mdiv_p1,
			.mdiv_p2	= pmapTable[pmap_id].mdiv_p2,
			.mdiv_p3	= pmapTable[pmap_id].mdiv_p3,
			.mdiv_p4	= pmapTable[pmap_id].mdiv_p4
		};

		avs_set_pmap(dvfs->mode, &pmap, dvfs->pstate);
	}
}
