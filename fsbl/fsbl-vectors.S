/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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


/* armv8 branch instruction.
 *  ARM Architecture Reference Manual
 *  ARMv8, for ARMv8-A architecture profile
 *  ARM DDI 0487A.f (ID032515)
 *  Section C6.6.20
 *
 *  Unconditional branch to offset from the address
 * of this instruction, in the range +/-128MB, 
 * encoded as "imm26" times 4
 *
 */
#define AARCH64_B 0x14000000


	.global vectors
vectors:
#ifdef STUB64_START
	/* Instruction 0xea000006 will decode as 'b 0xe0000020'
	 * (i.e. 'b reset_handler') for armv7-a and as
	 * 'ands x6, x0, x0' for armv8. If we get to pc +=0x04
	 * then we are running armv8 and have to jump to the
	 * stub loader. For armv7-a the UNDEF jump gets nuked
	 * until we put it back later on in 'setup_vector_table'.
	 */
	b	reset_handler
	.word	AARCH64_B | ((STUB64_START - SRAM_ADDR - 4) >> 2)
#else
	b	reset_handler
	b	default_handler		@ undefined instruction
#endif
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
