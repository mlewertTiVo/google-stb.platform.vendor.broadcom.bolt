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

	.section .fsbl.vectors
	.align 4
	.arm

@ WARNING: For Zeus 4.2 do not add any more code or data here
@ as we are one word away from the 64 byte/16 ins limit, or
@ right at the limit WRT 3390a0 & 3390b0.

	.global vectors
vectors:
	b	reset_handler
	b	default_handler		@ undefined instruction
	b	default_handler		@ software interrupt
	b	default_handler		@ data abort
	b	default_handler		@ prefetch (instruction) abort
	b	default_handler		@ hypercall
	b	default_handler		@ interrupt
	b	default_handler		@ fast interrupt

	.global reset_handler
reset_handler:
	ldr	sp, =SRAM_STACK
	bl	early_cpu_init
	bl	setup_exception_stacks
	b	fsbl_b_start
default_handler:
	b	default_handler

#if defined(CONFIG_BCM3390A0) || defined(CONFIG_BCM3390B0)
	@ word offset to the bootrom data area
	. = vectors + 0x38
	.word 0x140 / 4
#endif
