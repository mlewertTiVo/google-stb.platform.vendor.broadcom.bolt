/*****************************************************************************
* Copyright 2014 - 2015 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/

#ifndef _ANDROID_TYPES_H_
#define _ANDROID_TYPES_H_

#define BOOT_MAGIC        "ANDROID!"

#define BOOT_REASON_MASK       0x000000FF
#define BOOT_QUIESCENT_MASK    0x00000100
#define BOOT_DMVERITY_EIO_MASK 0x00000200

/* from arch/arm/include/asm/types.h */
typedef unsigned short umode_t;

/*
 *  * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 *   * header files exported to user space
 *    */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

/*
 *  * These aren't exported outside the kernel to avoid name space clashes
 *   */

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define BITS_PER_LONG 32

/* Dma addresses are 32-bits wide.  */

typedef u32 dma_addr_t;

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;



/*from u-boot include/linux/types.h*/

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__

typedef         __u8            u_int8_t;
typedef         __s8            int8_t;
typedef         __u16           u_int16_t;
typedef         __s16           int16_t;
typedef         __u32           u_int32_t;
typedef         __s32           int32_t;

#endif /* !(__BIT_TYPES_DEFINED__) */

typedef         __u8            uint8_t;
typedef         __u16           uint16_t;
typedef         __u32           uint32_t;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef         __u64           uint64_t;
typedef         __u64           u_int64_t;
typedef         __s64           int64_t;
#endif


/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */
#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
#if defined(__GNUC__)
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
#endif
typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

#endif /* _ANDROID_TYPES_H_ */
