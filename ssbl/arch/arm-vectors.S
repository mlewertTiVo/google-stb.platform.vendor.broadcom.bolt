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

	.align	4
	.arm

	.global vectors
vectors:
	b	reset_handler
	b	undef_handler
	b	svc_handler
	b	data_abort_handler
	b	prefetch_abort_handler
	b	hyp_handler
	b	irq_handler
	b	fiq_handler

reset_handler:
	b	ssbl_init

undef_handler:
	b	dump_regs_and_halt

svc_handler:
	b	dump_regs_and_halt

data_abort_handler:
	b	dump_regs_and_halt

prefetch_abort_handler:
	b	dump_regs_and_halt

hyp_handler:
	b	dump_regs_and_halt

irq_handler:
	b	dump_regs_and_halt

fiq_handler:
	b	dump_regs_and_halt

dump_regs_and_halt:
	sub	sp, #80

	/* fill out and push a 'struct arm_regs' instance on the stack */
	stmia	sp!, {r0-r12}		/* push r0 thru r12 */

	mrs	r0, CPSR
	mov	r1, r0			/* backup CPSR in r1 */
	bic	r0, #CPSR_MODE_MASK
	orr	r0, #MODE_SVC
	msr	CPSR_c, r0
	mov	r4, sp
	msr	CPSR_c, r1		/* go back to exception mode */
	mov	r5, lr
	stmia	sp!, {r4-r5}		/* push SP_svc, LR_<mode> */

	mov	r2, r1
	bic	r2, #~CPSR_MODE_MASK
	mov	r3, #MODE_ABT
	cmp	r2, r3			/* if ABT, calculate faulting PC */
	subeq	r2, r5, #8		/* if ABT, PC = LR - 8 */
	movne	r2, pc

	stmia	sp!, {r2}		/* push PC */
	and	r2, #CPSR_MODE_MASK

	stmia	sp!, {r0}		/* push copy of CPSR */

	mrc	p15, 0, r0, c5, c0, 0	/* read DFSR */
	mrc	p15, 0, r1, c6, c0, 0	/* read DFAR */
	mrc	p15, 0, r2, c6, c0, 2	/* read IFAR */
	stmia	sp!, {r0-r2}		/* push DFSR, DFAR, IFAR */

	sub	sp, #80
	mov	r0, sp
	bl	arch_dump_registers

1:
	wfi
	b	1b
