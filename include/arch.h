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

/* bundle your arch specific includes here
*/
#ifndef __BOLT_ARCH_H
 #define __BOLT_ARCH_H

#ifdef CFG_ARCH_ARM
#include "arm/arm.h"
#include "arm-start.h"

#elif defined(CFG_ARCH_MIPS)
#include "mips/bmips.h"
#include "mips/hazard.h"

#else
#error "unknown architecture"
#endif

#endif /* __BOLT_ARCH_H */
