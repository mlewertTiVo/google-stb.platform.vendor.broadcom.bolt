/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _FSBL_DTU_H
#define _FSBL_DTU_H

#include <fsbl-common.h>
#include <stdbool.h>

#define DTU_PAGE_SIZE_MB	2
#define DTU_TIMEOUT		10000  /* total of 10 msec */
#define DTU_DELAY		1

#ifdef BCHP_MEMC_DTU_CONFIG_0_REG_START

void dtu_load(struct board_type *b, bool warm_boot);
void dtu_verify(unsigned int num_memc);
void dtu_init(struct board_type *b);

#else

static inline void dtu_load(struct board_type *b, bool warm_boot) { }
static inline void dtu_verify(unsigned int num_memc) { }
static inline void dtu_init(struct board_type *b) { }

#endif

#endif /* _FSBL_DTU_H */
