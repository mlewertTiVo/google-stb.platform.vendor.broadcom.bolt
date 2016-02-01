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

#ifndef __SSBL_SEC_H__
#define __SSBL_SEC_H__

#include "lib_types.h"

#define SEC_MAX_NUM_RANDOM_NUMBERS 10

const char *sec_get_bsp_version(void);
int sec_get_random_num(uint32_t *dest, int num);

#endif /* __SSBL_SEC_H__ */
