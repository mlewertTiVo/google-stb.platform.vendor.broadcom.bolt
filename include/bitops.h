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

#ifndef __BITOPS_H__
#define __BITOPS_H__

#include <stdint.h>

/* Find first set bit (like POSIX ffs()) */
static inline int ffs(int i)
{
	return __builtin_ffs(i);
}

/* Find last set bit; mimics ffs() */
static inline int fls(int i)
{
	return i ? 32 - __builtin_clz(i) : 0;
}

/* ffs() for long long */
static inline int ffsll(long long i)
{
	return __builtin_ffsll(i);
}

/* fls() for long long */
static inline int flsll(long long i)
{
	return i ? sizeof(long long) * 8 - __builtin_clzll(i) : 0;
}

/**
 * upper_32_bits - return bits 32-63 of a number
 * @n: the number we're accessing
 *
 * A basic shift-right of a 64- or 32-bit quantity.  Use this to suppress
 * the "right shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define upper_32_bits(n) ((uint32_t)(((n) >> 16) >> 16))

/**
 * lower_32_bits - return bits 0-31 of a number
 * @n: the number we're accessing
 */
#define lower_32_bits(n) ((uint32_t)(n))

/**
 * A basic shift-left of a 64- or 32-bit quantity.  Use this to suppress
 * the "left shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define shift_left_32(n) (((n) << 16) << 16)

#endif /* __BITOPS_H__ */
