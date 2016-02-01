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

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include "arm-macros.h"

	.equ SCTLR_I, (1 << 12)

	.macro icache_on
#if !CFG_UNCACHED
	mrc	p15, 0, r1, c1, c0, 0
	orr	r1, r1, #SCTLR_I	@ enable
	mcr	p15, 0, r1, c1, c0, 0
#endif
	.endm

	.macro icache_off
#if !CFG_UNCACHED
	mrc	p15, 0, r1, c1, c0, 0
	bic	r1, r1, #SCTLR_I	@ disable
	mcr	p15, 0, r1, c1, c0, 0
#endif
	.endm

	@ Starting with ZEUS 4.2, there is a "magic" region a little after
	@ the top of the FSBL image reserved for security. We have
	@ 64 bytes from the top of the FSBL image free for early-boot
	@ initialization code. The rest is filled with the FILLB pattern.
	@ For more details, see 'fsbl.lds.in'.

	.section .init
	.align   4
	.arm

	.global fsbl_b_start
fsbl_b_start:
#ifndef SECURE_BOOT
	@ see if we're running from SRAM (Boot 2.0) or flash (Boot 1.0)
	mov	r0, pc
	ldr	r3, =_fsbl_copy_start
	ldr	r2, =0xffff0000

	and	r0, r0, r2
	and	r1, r3, r2
	cmp	r0, r1
	beq	1f

	icache_on

	@ nope, need to copy.  r0=flash, r3=SRAM, r1=last SRAM address
	ldr	r1, =_fsbl_copy_end
2:	ldmia	r0!, {r2}
	stmia	r3!, {r2}
	cmp	r3, r1
	blt	2b

	icache_off

	@ use "ldr" instead of "adr" to force an absolute SRAM address
	ldr	r0, =1f
	bx	r0

1:
#endif
	@ zero out .bss
	ldr	r0, =_fsbl_zero_start
	ldr	r1, =_fsbl_zero_end
	mov	r2, #0
2:
	stmia	r0!, {r2}
	cmp	r0, r1
	blt	2b

	bl	setup_vector_table

	ldr	r0, =fsbl_main
	bx	r0

	.global early_cpu_init
early_cpu_init:
	@ A2 errata: Dummy write to L2CTLR to propagate modified default
	@ register settings to L2 consuming logic
	mrc	p15,1,r2,c9,c0,2
	mcr	p15,1,r2,c9,c0,2

	mov	pc, lr

	.global setup_exception_stacks
setup_exception_stacks:
	@ Initialize the stack pointers of all exception modes
	mrs		r0, CPSR
	mov		r1, r0			@ backup CPSR in r1
	ldr		r2, =SRAM_STACK		@ r2 = desired stack base
	set_mode_sp	r0, r2, MODE_FIQ
	set_mode_sp	r0, r2, MODE_IRQ
	set_mode_sp	r0, r2, MODE_ABT
	set_mode_sp	r0, r2, MODE_UND
	set_mode_sp	r0, r2, MODE_SYS
	set_mode_sp	r0, r2, MODE_SVC	@ return in SVC mode
	bx		lr

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

#ifdef CFG_EMULATION
	.align  4
	.global	jumpto
	.type	jumpto, %function
	.arm
jumpto:
	mov	r5, r0

	@ r0: should be 0 for Linux boot
	mov	r0, #0

	@ r1: should have machine ID corresponding to DT
	movw	r1, #0xffff
	movt	r1, #0xffff

	@ r2: address of DT
	mov	r2, #0x1000

	@ go!
	mov	pc, r5
#endif

	.section .init.rodata
	.align  4

.end
