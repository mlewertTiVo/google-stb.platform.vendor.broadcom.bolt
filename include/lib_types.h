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

#ifndef __LIB_TYPES_H__
#define __LIB_TYPES_H__

#include <stdint.h>
#include <stddef.h>

#ifdef CFG_ARCH_MIPS
#if ((defined(__BIG_ENDIAN)+defined(__LITTLE_ENDIAN)) != 1)
#error "Either __BIG_ENDIAN or __LITTLE_ENDIAN must be defined!"
#endif
#endif

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct cons_s {
	char *str;
	int num;
} cons_t;

#endif /* __LIB_TYPES_H__ */

