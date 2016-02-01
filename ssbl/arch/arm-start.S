/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include "arm-macros.h"

	.arm

/* make sure _fsbl_info is not in bss
*/
.section .data
DECLARE_LONG(_fsbl_info)


.section .init
.align 4

ENTRY_PROC(ssbl_init)
	ldr		r1, =_fsbl_info
	str		r0, [r1]

	bl		setup_vector_table

	ldr		r0, =_end
	ldr		r1, =_fbss
	ldr		r2, =_ebss
	ldr		r3, =_fdata

	b		ssbl_main;
END_PROC(ssbl_init)

setup_vector_table:
	@ Check ID_PFR if security extensions are supported, which implies
	@ support for VBAR
	mrc	p15, 0, r0, c0, c1, 1
	ands	r0, #(1 << 4)
	beq	1f

	ldr	r0, =vectors
	mcr	p15, 0, r0, c12, c0, 0

	@ remap all exceptions to VBAR by setting SCTLR.V = 1
	mrc	p15, 0, r0, c1, c0, 0
	and	r0, #~(1 << 13)
	mcr	p15, 0, r0, c1, c0, 0
1:
	bx	lr

.section .text
.align 4

ENTRY_PROC(__setstack)
	mov		sp, r0

	@ Enforce ARM-EABI stack requirements on entry
	bic		sp, sp, #7

	@ Initialize the stack pointers of all exception modes
	mrs		r1, CPSR
	set_mode_sp	r1, r0, MODE_FIQ
	set_mode_sp	r1, r0, MODE_IRQ
	set_mode_sp	r1, r0, MODE_ABT
	set_mode_sp	r1, r0, MODE_UND
	set_mode_sp	r1, r0, MODE_SYS
	set_mode_sp	r1, r0, MODE_SVC	@ return in SVC mode

	mov		r0, #0
	bx 		lr
END_PROC(__setstack)


ENTRY_PROC(__getstack)
	mov		r0, sp
	bx 		lr
END_PROC(__getstack)

.end
