/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARM_MACROS_H__
#define __ARM_MACROS_H__

#define MODE_USR	0x10
#define MODE_FIQ	0x11
#define MODE_IRQ	0x12
#define MODE_SVC	0x13
#define MODE_MON	0x16
#define MODE_ABT	0x17
#define MODE_HYP	0x1a
#define MODE_UND	0x1b
#define MODE_SYS	0x1f
#define CPSR_MODE_MASK	0x1f

#define CPSR_T		(0x01 << 5)
#define CPSR_FIQ	(0x01 << 6)
#define CPSR_IRQ	(0x01 << 7)
#define CPSR_ASYNC	(0x01 << 8)
#define CPSR_ENDIAN	(0x01 << 9)
#define CPSR_IT0 	(0x3f << 10)
#define CPSR_GE 	(0x0f << 16)
#define CPSR_IT1 	(0x02 << 25)
#define CPSR_Q 		(0x01 << 27)
#define CPSR_V 		(0x01 << 28)
#define CPSR_C 		(0x01 << 29)
#define CPSR_Z 		(0x01 << 30)
#define CPSR_N 		(0x01 << 31)

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

.macro set_cpsr_mode tmp_reg, mode
	bic	\tmp_reg, #CPSR_MODE_MASK
	orr	\tmp_reg, #\mode
	msr	CPSR_c, \tmp_reg
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

