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

#ifndef __ADDR_MAPPING_H__
#define __ADDR_MAPPING_H__

#include <stdint.h>

/* mapping between different address spaces, e.g., between child and parent */
struct addr_mapping_entry {
	uint32_t which; /* index or address that can identify source device */
	uint32_t from_mb; /* in MB */
	uint32_t to_mb; /* in MB */
	uint32_t size_mb; /* in MB */
};

#endif /* __ADDR_MAPPING_H__ */
