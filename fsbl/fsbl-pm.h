/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
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

#else

static inline bool fsbl_ack_warm_boot(void) { return false; }
static inline void fsbl_finish_warm_boot(uint32_t t) { die("no S3 support"); }

#endif /* CFG_PM_S3 */

bool must_resume_ddr_phys(void);
void fsbl_resume_ddr_phys(void);

unsigned long psci(unsigned long r0, unsigned long r1,
		unsigned long r2, unsigned long r3);

#endif /* _FSBL_PM_H */
