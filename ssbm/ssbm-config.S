/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include <arm-macros.h>

.text
.align	4
.arm

ENTRY_PROC(arch_config)
    push	{r0-r2, lr}

	/* Setup and start the timer
	*/
	mrc		p15, 0, r0, c14, c2, 1

	/* Disable timer interrupt output, enable timer
	*/
	orr		r0, #0x3
	mcr		p15, 0, r0, c14, c2, 1

	/* Enable branch prediction (SCTLR.Z)
	*/
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, #(1 << 11)
	mcr		p15, 0, r0, c1, c0, 0

    pop     {r0-r2, pc}
END_PROC(arch_config)
