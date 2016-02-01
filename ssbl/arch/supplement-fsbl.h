/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __SUPPLEMENT_FSBL_H__
#define __SUPPLEMENT_FSBL_H__

#include <stdint.h>

#ifdef CFG_EMULATION
static void supplement_fsbl_pagetable(uint32_t *pt_1st) {}
#else
void supplement_fsbl_pagetable(uint32_t *pt_1st);
#endif

#endif /* __SUPPLEMENT_FSBL_H__ */

