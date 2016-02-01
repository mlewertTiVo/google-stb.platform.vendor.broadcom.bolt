/***************************************************************************
 *     Copyright (c) 2016-, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _FSBL_EMMC_H
#define _FSBL_EMMC_H

#include <lib_types.h>

int emmc_read_bfw(uint32_t dma_addr,
		  uint32_t bfw_size,
		  uint32_t emmc_addr);

#endif /* _FSBL_EMMC_H */
