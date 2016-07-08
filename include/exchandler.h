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

#include "lib_types.h"

#define MEM_BYTE     1
#define MEM_HALFWORD 2
#define MEM_WORD     3
#define MEM_QUADWORD 4

int mem_peek(void *d, long addr, int type);
int mem_poke(long addr, uint64_t val, int type);

#endif /* __EXCHANDLER_H__ */

