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

#ifndef __ARM_MACROS_H__
#define __ARM_MACROS_H__

#define MODE_USR		0x10
#define MODE_FIQ		0x11
#define MODE_IRQ		0x12
#define MODE_SVC		0x13
#define MODE_ABT		0x17
#define MODE_UND		0x1b
#define MODE_SYS		0x1f
#define CPSR_MODE_MASK	0x1f

/*  *********************************************************************
    *  32/64-bit macros
    ********************************************************************* */

#ifdef __long64
#define _VECT_	.dword
#define _LONG_	.dword
#define REGSIZE	8
#define BPWSIZE 3		/* bits per word size */
#else
#define _VECT_	.word
#define _LONG_	.word
#endif


/*  *********************************************************************
    *  Declare variables
    ********************************************************************* */

#define DECLARE_LONG(x) \
	.global x ; \
x:	_LONG_  0

#ifdef __ASSEMBLER__
.macro barrier
	dsb;
	isb;
.endm

.macro set_mode_sp tmp_reg, sp_reg, mode
	bic	\tmp_reg, #CPSR_MODE_MASK
	orr	\tmp_reg, #\mode
	msr	CPSR_c, \tmp_reg
	mov	sp, \sp_reg
.endm
#endif

#define ENTRY_PROC(name) \
	.global name; \
	.type name, % function; \
name:

#define END_PROC(name) \
	.size name, . -name; \
	.ltorg;

#endif /* __ARM_MACROS_H__ */

