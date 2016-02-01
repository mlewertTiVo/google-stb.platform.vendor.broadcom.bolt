/*****************************************************************************
*
* Copyright 2014 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*
*****************************************************************************/

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include "arm-macros.h"

	.arm

.section .init
.align 4

ENTRY_PROC(android_bsu_init)
	push		{r0}
	ldr		r0, =android_bsu_entry
	blx		r0

	pop		{r0, lr}
	bx		lr
END_PROC(android_bsu_init)

.end

