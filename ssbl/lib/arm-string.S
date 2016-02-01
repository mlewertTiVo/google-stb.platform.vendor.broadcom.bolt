/***************************************************************************
 *	 Copyright (c) 2012-2013, Broadcom Corporation
 *	 All Rights Reserved
 *	 Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#if defined(LIBSTRING_ASM_MEMCPY)

#define __ARM_NEON__
	.arm
	.fpu neon
	.section .text

.global lib_memcpy
.type lib_memcpy,"function"
lib_memcpy:
	/* r0=dst r1=src r2=len */

	push	{r0, r4, lr}
	cmp		r2, #0x0	@ Short-circuit 0 length copies
	beq		done
	ands	r4, r0, #0x3	@ Check if dst is word aligned
	bne		copy_bytes
	ands	r4, r1, #0x3	@ Check if src is word aligned
	bne		copy_bytes
	cmp		r2, #0x4	@ Check if len is at least 1 word
	blt		copy_bytes
	ands	r4, r2, #0x3	@ Check if len is word aligned
	bne		copy_bytes
#if defined(__ARM_NEON__)
	cmp		r2, #0x40	@ Check if len is >= NEON stride length
	blt		copy_words
	ldr		r3, =0x40000000	@ Enable NEON
	fmxr	FPEXC, r3
copy_words_neon:
	pld		[r1, #0xC0]		@ Cache preload hint
	vldm	r1!, {d0-d7}	@ *r1 => NEON registers
	vstm	r0!, {d0-d7}	@ *r0 <= NEON registers
	sub		r2, #0x40
	cmp		r2, #0x40
	bge		copy_words_neon
	cmp		r2, #0
	beq		done
	ldr		r3, =0x0		@ Disable NEON
	fmrx	r3, FPEXC
#endif
copy_words:
	ldr		r3, [r1], #4
	str		r3, [r0], #4
	subs	r2, #4
	bne		copy_words
	b		done
copy_bytes:
	ldrb	r3, [r1], #1
	strb	r3, [r0], #1
	subs	r2, #1
	bne		copy_bytes
done:
	pop 	{r0, r4, pc}

	.end
#endif

