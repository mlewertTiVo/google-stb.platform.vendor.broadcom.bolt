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

#ifndef __INITDATA_H__
#define __INITDATA_H__

#if defined(__ASSEMBLER__)
#define DECLARE_INITVAR(x) \
       .globl x ; \
x:     _LONG_   0
#else
#define DECLARE_INITVAR(x) \
       extern unsigned long x;
#endif

#if !defined(__ASSEMBLER__)
DECLARE_INITVAR(mem_textreloc)
    DECLARE_INITVAR(mem_textbase)
    DECLARE_INITVAR(mem_textsize)
    DECLARE_INITVAR(mem_totalsize)
    DECLARE_INITVAR(mem_topofmem)
    DECLARE_INITVAR(mem_heapstart)
    DECLARE_INITVAR(mem_bottomofmem)
    DECLARE_INITVAR(mem_datareloc)
    DECLARE_INITVAR(cpu_prid)
#endif

#endif /* __INITDATA_H__ */

