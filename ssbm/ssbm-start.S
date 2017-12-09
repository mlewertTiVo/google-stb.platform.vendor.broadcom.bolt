/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include <arm-macros.h>
#include <psci.h>

	.arm

/* make sure _fsbl_info is not in bss */
.section .data
DECLARE_LONG(_fsbl_info)


.section .text
.align 4

ENTRY_PROC(ssbm_init)
	ldr		r1, =_fsbl_info
	str		r0, [r1]

	bl		setup_vector_table


	ldr		r0, =_end
	ldr		r1, =_fbss
	ldr		r2, =_ebss
	ldr		r3, =_fdata


	b		ssbm_main
END_PROC(ssbm_init)

ENTRY_PROC(setup_vector_table)
	@ Check ID_PFR1 if security extensions are supported, which implies
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
END_PROC(setup_vector_table)

ENTRY_PROC(__setstack)
	@ Enforce ARM-EABI stack requirements on entry
	bic		r0, r0, #7
	mov		sp, r0

	@ Initialize the stack pointers of all exception modes
	mrs		r1, CPSR
	set_mode_sp	r1, r0, MODE_FIQ
	set_mode_sp	r1, r0, MODE_IRQ
	set_mode_sp	r1, r0, MODE_ABT
	set_mode_sp	r1, r0, MODE_UND
	set_mode_sp	r1, r0, MODE_SYS
	set_mode_sp	r1, r0, MODE_SVC	@ return in SVC mode

	bx 		lr
END_PROC(__setstack)


ENTRY_PROC(arch_call_bfw_main)
	bl	__setstack
	mov	r0, #0
	mov	r1, #0
	ldr	r2, =bfw_main
	bx	r2
END_PROC(arch_call_bfw_main)


ENTRY_PROC(__getstack)
	mov		r0, sp
	bx 		lr
END_PROC(__getstack)

.end
