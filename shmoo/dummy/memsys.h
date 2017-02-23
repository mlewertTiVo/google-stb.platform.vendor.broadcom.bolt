/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/
#ifndef MEMSYS_H
#define MEMSYS_H

/* TODO: remove dependency on the following from bsu.c
 *  - EDIS_NPHY
 *  - EDIS_OFFS
 *  - shmoo_params
 */
#define EDIS_NPHY	0
#define EDIS_OFFS	0
static const struct memsys_params __maybe_unused shmoo_params[] = {
	{0, 0, 0, 0},
};

#endif /* MEMSYS_H */
