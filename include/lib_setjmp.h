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

#ifndef __LIB_SETJMP_H__
#define __LIB_SETJMP_H__

/*
 * Note that while lib_setjmp() and lib_longjmp() behave like setjmp()
 * and longjmp() normally do, gcc 3.1.x (and later) assumes things about
 * how setjmp() and longjmp() should work (even with -fno-builtins).  We
 * don't want it to do those, so these functions must be named differently.
 */

#ifdef __ASSEMBLER__
#define _JBIDX(x)	(8*(x))
#else
#define _JBIDX(x)	(x)
#endif

#define JMPB_S0		_JBIDX(0)
#define JMPB_S1		_JBIDX(1)
#define JMPB_S2		_JBIDX(2)
#define JMPB_S3		_JBIDX(3)
#define JMPB_S4		_JBIDX(4)
#define JMPB_S5		_JBIDX(5)
#define JMPB_S6		_JBIDX(6)
#define JMPB_S7		_JBIDX(7)
#define JMPB_FP         _JBIDX(8)
#define JMPB_SP         _JBIDX(9)
#define JMPB_RA         _JBIDX(10)

#define JMPB_SIZE       _JBIDX(11)

#ifndef __ASSEMBLER__
typedef long long jmp_buf[JMPB_SIZE];
extern int lib_setjmp(jmp_buf);
extern void lib_longjmp(jmp_buf, int val);
#endif

#endif /* __LIB_SETJMP_H__ */

