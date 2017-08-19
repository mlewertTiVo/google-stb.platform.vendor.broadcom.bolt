/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef _FSBL_PM_H
#define _FSBL_PM_H

#include <stdbool.h>
#include "fsbl.h"

#if CFG_PM_S3

bool fsbl_ack_warm_boot(void);
void fsbl_finish_warm_boot(uint32_t restore_val);
void fsbl_init_warm_boot(uint32_t flags);

#else

static inline bool fsbl_ack_warm_boot(void) { return false; }

static inline void fsbl_finish_warm_boot(uint32_t t)
{
	sys_die(DIE_PM_NO_S3_SUPPORT, "no S3 support");
}

static inline void fsbl_init_warm_boot(uint32_t flags)
{
	sys_die(DIE_PM_NO_S3_SUPPORT, "no S3 support");
}

#endif /* CFG_PM_S3 */

bool must_resume_ddr_phys(void);
void fsbl_resume_ddr_phys(void);

unsigned long psci(unsigned long r0, unsigned long r1,
		unsigned long r2, unsigned long r3);

#endif /* _FSBL_PM_H */
