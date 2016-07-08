/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARMV8_MACROS_H__
#define __ARMV8_MACROS_H__

#ifdef __ASSEMBLER__

#include <config.h>

.macro barrier
	isb;
	dsb sy;
.endm


.macro xpush2 xa, xb
	stp	\xa, \xb, [sp, #-16]!
.endm


.macro xpop2 xa, xb
	ldp	\xa, \xb, [sp], #16
.endm


.macro	drop_el3_to_elx current_spsr lower_el_startaddr
	/* when we eret, x30 of the lower EL 
	is picked up as the rtn addr.
	*/
	msr	elr_el3, \lower_el_startaddr

	/* do the mode change m[3..0]
	*/
	msr	spsr_el3, \current_spsr
	eret
.endm


/* EL aware i-cache enable & disable (op=1=enable)
 * Can use only once per assembly module!
 */
.macro icache_op op
#if !CFG_UNCACHED
	mrs	x9, CurrentEL
	and	x9, x9, #CURRENT_EL_MASK

	cmp	x9, #CURRENT_E3_MASK
	b.ne	ion_not_el3_\op
	mrs	x1, sctlr_el3
.ifgt \op
	orr	x1, x1, #SCTLR_I
.else
	bic	x1, x1, #SCTLR_I
.endif
	msr	sctlr_el3, x1
	b	ion_end_\op

ion_not_el3_\op:
	cmp	x9, #CURRENT_E2_MASK
	b.ne	ion_not_el2_\op
	mrs	x1, sctlr_el2
.ifgt \op
	orr	x1, x1, #SCTLR_I
.else
	bic	x1, x1, #SCTLR_I
.endif
	msr	sctlr_el2, x1
	b	ion_end_\op

ion_not_el2_\op:
	mrs	x1, sctlr_el1
.ifgt \op
	orr	x1, x1, #SCTLR_I
.else
	bic	x1, x1, #SCTLR_I
.endif
	msr	sctlr_el1, x1
ion_end_\op:
	isb
#endif /* !CFG_UNCACHED */
.endm


.macro putc char
	mov	w0, \char
	bl	uart_putc
.endm


.macro check_is_orion midr tmp brfail
	mrs     \midr, midr_el1
	isb
	/* Mask off variable fields */
	ldr     \tmp, =MIDR_MAJOR_PART_MASK
	and     \midr, \midr, \tmp
	ldr     \tmp, =MIDR_PartOrionB53
	cmp     \midr, \tmp
	b.ne    \brfail
.endm


#define ENTRY_PROC(name) \
	.global name; \
	.type name, % function; \
name:

#define END_PROC(name) \
	.size name, . -name; \
	.ltorg;


#endif /* __ASSEMBLER__ */

#endif /* __ARMV8_MACROS_H__ */

