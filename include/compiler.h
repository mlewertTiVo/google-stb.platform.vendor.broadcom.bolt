/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __COMPILER_H__
#define __COMPILER_H__

#define __maybe_unused		__attribute__((unused))
#define __optimize_O0		__attribute__((optimize("O0")))
#define __weak			__attribute__((weak))
#define __noreturn		__attribute__((noreturn))
#define __packed		__attribute__((packed))

#endif /* __COMPILER_H__ */
