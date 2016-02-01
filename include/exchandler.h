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

#ifndef __EXCHANDLER_H__
#define __EXCHANDLER_H__

#include "lib_queue.h"
#include "lib_setjmp.h"

#define MEM_BYTE     1
#define MEM_HALFWORD 2
#define MEM_WORD     3
#define MEM_QUADWORD 4

#define EXC_NORMAL_RETURN 0
#define EXC_CHAIN_EXC     1

typedef struct jmpbuf_s {
	queue_t stack;
	jmp_buf jmpbuf;
} jmpbuf_t;

typedef struct exc_handler_s {
	int catch_exc;
	int chain_exc;
	queue_t jmpbuf_stack;
} exc_handler_t;

extern exc_handler_t exc_handler;

jmpbuf_t *exc_initialize_block(void);
void exc_cleanup_block(jmpbuf_t *);
void exc_cleanup_handler(jmpbuf_t *, int);
void exc_longjmp_handler(void);
int mem_peek(void *, long, int);
int mem_poke(long, uint64_t, int);

#define exc_try(jb) (lib_setjmp(((jb)->jmpbuf)))

#endif /* __EXCHANDLER_H__ */

