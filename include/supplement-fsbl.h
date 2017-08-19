/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __SUPPLEMENT_FSBL_H__
#define __SUPPLEMENT_FSBL_H__

#include <stdbool.h>
#include <stdint.h>

bool supplement_fsbl_checkguardpage(uint32_t *pt_1st);
void supplement_fsbl_pagetable(uint32_t *pt_1st, bool guard_page_was_correct);
uint32_t supplement_fsbl_shmoover(void);

#endif /* __SUPPLEMENT_FSBL_H__ */

