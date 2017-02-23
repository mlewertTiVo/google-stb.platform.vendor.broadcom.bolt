/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef __FSBL_SHMOO_H__
#define __FSBL_SHMOO_H__

#include <fsbl-common.h>
#include <memsys.h>
#include "memsys-if.h"

#define SHMOO_OPTION_DISABLE_CONSOLE	(1 << 0)
#define SHMOO_OPTION_WARMBOOT		(1 << 1)
#define SHMOO_OPTION_PHY_ISLPE0		(1 << 3)
#define SHMOO_OPTION_SKIP_SHMOO		(1 << 4)
#define SHMOO_OPTION_SKIP_RTS		(1 << 5)
#define SHMOO_OPTION_SKIP_MEMCINIT	(1 << 6)
#define SHMOO_OPTION_SKIP_PHYINIT	(1 << 7)
#define SHMOO_OPTION_SKIP_PHYDRAMINIT	(1 << 8)
#define SHMOO_OPTION_SKIP_PHYPLLINIT	(1 << 9)
#define SHMOO_OPTION_SAVE_PHYSTATE	(1 << 10)
#define SHMOO_OPTION_PREP_PHYSTANDBY	(1 << 11)
#define SHMOO_OPTION_PHY_LOWPOWER	(1 << 12)
#define SHMOO_OPTION_NUMRANKS_MASK	0xF
#define SHMOO_OPTION_NUMRANKS(n) \
	((((n)-1) & SHMOO_OPTION_NUMRANKS_MASK) << 13)

void do_shmoo(const struct memsys_interface *gmemsys,
	const struct ddr_info *ddr, uint32_t *mcb, unsigned int options);
void print_shmoo_error(memsys_error_t *e);

#endif /* __FSBL_SHMOO_H__ */
