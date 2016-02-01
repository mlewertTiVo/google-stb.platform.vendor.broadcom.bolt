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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <compiler.h>

#include "arch.h"
#include "bchp_common.h"
#include "lib_physio.h"

#define min(a, b) \
	({ __typeof__(a) _a = (a); \
	   __typeof__(b) _b = (b); \
	   _a < _b ? _a : _b; })

#define max(a, b) \
	({ __typeof__(a) _a = (a); \
	   __typeof__(b) _b = (b); \
	   _a > _b ? _a : _b; })

#define abs(a) \
	({ __typeof__(a) _a = (a); \
	_a < 0 ? -_a : _a; })

#define IS_ALIGNED(a, b) (((a) & ((__typeof__(a))(b) - 1)) == 0)

#define BIT(x)			(1 << (x))
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))

#define _KB(k) ((k)*1024)
#define _MB(m) (_KB((m))*1024)
#define _GB(g) (_MB((g))*1024)

#define ALIGN_TO(x, align)	((x) & ~((__typeof__(x))(align) - 1))
#define ALIGN_UP_TO(x, align)	(((x) + (__typeof__(x))(align) - 1)\
				& ~((__typeof__(x))(align) - 1))
#define PTR_ALIGN(p, align)	((typeof(p))ALIGN_TO((unsigned long)(p), (align)))

#define __stringify(s) stringify(s)
#define stringify(s) #s

#endif /* __COMMON_H__ */
