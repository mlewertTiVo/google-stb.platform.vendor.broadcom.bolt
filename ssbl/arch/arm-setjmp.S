/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_setjmp.h"
#include "arm-macros.h"

	.arm

ENTRY_PROC(lib_setjmp)
	/* According to APCS, r4 ~ r14 are callee-saved general
	purpose registers, f4 ~ f7 are callee-saved floating registers
	*/
	stmia	r0!, {r4 - r14}
	mrs		r1, cpsr
	str		r1, [r0]
	mov		r0, #0x0
	bx		lr
END_PROC(lib_setjmp)

ENTRY_PROC(lib_longjmp)
	ldmia	r0!, {r4 - r14}
	ldr		r0, [r0]
	msr		cpsr, r0
	movs	r0, r1
	moveq	r0, #1
	bx		lr
END_PROC(lib_longjmp)
