/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#if ((defined(__BIG_ENDIAN)+defined(__LITTLE_ENDIAN)) != 1)
#error "Either __BIG_ENDIAN or __LITTLE_ENDIAN must be defined!"
#endif

#include "lib_types.h"

/* Endian types - they are not type-checked; just used for self-documentation */
typedef uint64_t le64;
typedef uint32_t le32;
typedef uint16_t le16;
typedef uint64_t be64;
typedef uint32_t be32;
typedef uint16_t be16;

#if CFG_ARCH_ARM

static inline uint32_t __fswap16(uint16_t x) {
	__asm__ ("rev16 %0, %1" : "=r" (x) : "r" (x));
	return x;
}

static inline uint32_t __fswap32(uint32_t x) {
	__asm__ ("rev %0, %1" : "=r" (x) : "r" (x));
	return x;
}

static inline uint64_t __fswap64(uint64_t x) {
	return ((uint64_t)__fswap32((uint32_t)x) << 32) |
		(__fswap32((uint32_t)(x >> 32)));
}

#else

/* TODO: implement __fswap{16,32,64} for MIPS */

static inline uint64_t __fswap16(uint16_t x) {
	return __const_swap16(x);
}

static inline uint32_t __fswap32(uint32_t x) {
	return __const_swap32(x);
}

static inline uint64_t __fswap64(uint64_t x) {
	return __const_swap64(x);
}

#endif /* CFG_ARCH_ARM */

#define __const_swap16(x) \
	(uint16_t)((((x) & 0x0000ff00U) >> 8) | \
		   (((x) & 0x000000ffU) << 8))

#define __const_swap32(x) \
	(uint32_t)((((x) & 0xff000000UL) >> 24) | \
		   (((x) & 0x00ff0000UL) >>  8) | \
		   (((x) & 0x0000ff00UL) <<  8) | \
		   (((x) & 0x000000ffUL) << 24))

#define __const_swap64(x) \
	(uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
		   (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
		   (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
		   (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
		   (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
		   (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
		   (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
		   (((uint64_t)(x) & 0x00000000000000ffULL) << 56))

#define __swap16(x) \
	(__builtin_constant_p(x) ? __const_swap16(x) : __fswap16(x))
#define __swap32(x) \
	(__builtin_constant_p(x) ? __const_swap32(x) : __fswap32(x))
#define __swap64(x) \
	(__builtin_constant_p(x) ? __const_swap64(x) : __fswap64(x))

#define swap16(x) __swap16(x)
#define swap32(x) __swap32(x)
#define swap64(x) __swap64(x)

#ifdef __BIG_ENDIAN

#define le64_to_cpu(x) __swap64(x)
#define le32_to_cpu(x) __swap32(x)
#define le16_to_cpu(x) __swap16(x)

#define be64_to_cpu(x) (x)
#define be32_to_cpu(x) (x)
#define be16_to_cpu(x) (x)

#else /* little endian */

#define le64_to_cpu(x) (x)
#define le32_to_cpu(x) (x)
#define le16_to_cpu(x) (x)

#define be64_to_cpu(x) __swap64(x)
#define be32_to_cpu(x) __swap32(x)
#define be16_to_cpu(x) __swap16(x)

#endif /* little endian */

/* cpu_to_* is just the reverse of *_to_cpu */
#define cpu_to_be64 be64_to_cpu
#define cpu_to_be32 be32_to_cpu
#define cpu_to_be16 be16_to_cpu
#define cpu_to_le64 le64_to_cpu
#define cpu_to_le32 le32_to_cpu
#define cpu_to_le16 le16_to_cpu

#endif /* __BYTEORDER_H__ */

