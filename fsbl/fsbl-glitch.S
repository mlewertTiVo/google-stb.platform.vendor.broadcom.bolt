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
#ifndef CFG_EMULATION
#include "arm-macros.h"

#define ERR_GLITCH_TRACE_CHECK 15
#define S3_TRACE_VALUE	0x712A0006
	.section .init
	.align   4
	.arm

	.macro sram_memset foo, reg_beg, reg_end, reg_val
\foo\()_memset_loop:
	str		\reg_val, [\reg_beg], #4
	cmp		\reg_beg, \reg_end
	blo		\foo\()_memset_loop
	.endm

@  We cannot rely on the compiler to properly
@ order these functions, instead we use
@ assembly code and also do a runtime
@ verification before this sequence is
@ called.


@ glitch_addr += glitch_trace;
@ glitch_entry = (void *) (glitch_addr << 1);
@ if (glitch_entry != glitch_addr1) handle_boot_err()
@ glitch_entry((struct fsbl_info *)glitch_info);
@ handle_boot_err()
@
ENTRY_PROC(anti_glitch_a)
	ldr		r1,	=glitch_trace
	ldr		r5,	[r1]
	ldr		r1,	=glitch_addr
	ldr		r4,	[r1]
	add		r4, r5, r4
	str		r4,	[r1]

	mov		r4, r4, LSL #1
	ldr		r1,	=glitch_entry
	str		r4,	[r1]

	ldr		r1,	=glitch_addr1
	ldr		r2,	[r1]
	cmp		r4,	r2
	beq		entry_ok

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2

entry_ok:

	@ clear SRAM from start of sram to pc
	ldr		r1,	=SRAM_ADDR	@0xffe0_0000
	mov		r2, 	pc
	mov		r3, 	#0
	sram_memset	clear1, r1, r2, r3

	@ store entry and fsbl_info into r4 and r0
	ldr		r1,	=glitch_info
	ldr		r0,	[r1]
	ldr		r1,	=glitch_entry
	ldr		r4,	[r1]

	@ store pointer to handle_boot_err()
	ldr		r5,	=handle_boot_err

	@clear anti_glitch_b to end of SRAM
	ldr		r1, =anti_glitch_b
	ldr		r2, =SRAM_STACK		@0xffe1_0000
	mov  		r3, #0
	sram_memset	clear2, r1, r2, r3

	@ call ssbl_entry(*fsbl_info)
	blx		r4
	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	blx		r5
END_PROC(anti_glitch_a)

@ glitch_trace = 12345;
@ sec_verify_ssbl();
@ glitch_addr -= glitch_trace;
@ sec_mitch_check();
@ anti_glitch_a();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_b)
	movw		r0,	#12345
	ldr		r1,	=glitch_trace
	str		r0,	[r1]

	ldr		r2,	=sec_verify_ssbl
	blx		r2

	ldr		r1,	=glitch_trace
	ldr		r5,	[r1]
	ldr		r1,	=glitch_addr
	ldr		r4,	[r1]
	rsb		r4, r5, r4
	str		r4,	[r1]

	ldr		r2,	=sec_mitch_check
	blx		r2

	ldr		r2,	=anti_glitch_a
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_b)

@ glitch_addr = (uint32_t) copy_code();
@ glitch_addr1 = glitch_addr;
@ glitch_addr = glitch_addr >> 1;
@ mmu_disable();
@ i_cache_config(0);
@ anti_glitch_b();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_c)
	ldr		r2,	=copy_code
	blx		r2
	ldr		r1,	=glitch_addr1
	str		r0,	[r1]
	mov		r0, r0, LSR #1
	ldr		r1,	=glitch_addr
	str		r0,	[r1]

	ldr		r2,	=mmu_disable
	blx		r2

	ldr		r2,	=i_cache_config
	mov		r0,	#0
	blx		r2

	ldr		r2,	=anti_glitch_b
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_c)

@ glitch_entry = (void *) (glitch_addr << 1);
@ glitch_entry();
@ handle_boot_err()
@
ENTRY_PROC(anti_glitch_d)
	ldr		r1,	=glitch_addr
	ldr		r2,	[r1]
	mov		r2, r2, LSL #1
	ldr		r3,	=handle_boot_err

	@ call linux_entry()
	blx		r2
	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	blx		r3
END_PROC(anti_glitch_d)

#if CFG_PM_S3
@ store glitch_addr content to r0
@ anti_glitch_b();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_e)

@	ldr		r0,	=BCHP_PHYSICAL_OFFSET
@	ldr		r1,	=BCHP_HIF_TOP_CTRL_SCRATCH


	ldr		r1,	=glitch_trace
	ldr		r0,	[r1]	
	ldr		r2,	=S3_TRACE_VALUE

	cmp		r0,	r2
	beq		trace_ok

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2

trace_ok:
	ldr		r2,	=anti_glitch_d
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_e)
#endif

@ r1: addr of register to set
@ r4: value to set
@
ENTRY_PROC(return_from_check_return_val)
	str		r4,	[r1]
	pop		{r4-r6, pc}
END_PROC(return_from_check_return_val)

@ check return value and set trace bit
@ r0: val to check, 0: ok, other: failure
@ r1: addr of register to set
@ r2: trace bit to set
@ r3: error code
@
ENTRY_PROC(check_return_val)
	push		{r4-r6, lr}
	ldr		r4,	[r1]
	mov		r5, 	#1
	lsl		r6, 	r5, r2
	orr		r4, 	r4, r6
	cmp		r0, 	#0
	beq		return_from_check_return_val

	mov		r0,	r3
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(check_return_val)

.end

#endif /* !CFG_EMULATION */
