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
#ifndef __DDR_H__
#define __DDR_H__

#include <stdbool.h>
#include "fsbl-common.h"

static inline bool ddr_is_populated(struct ddr_info *ddr)
{
	return ddr && ddr->size_mb;
}

/**
 * ddr_get_restricted_size_mb - Get largest permitted mapped DDR size in MB
 * @ddr - DDR info struct to read
 *
 * Basically ddr_get_restricted_size with a different return value.
 *
 * Return: The possibly-trimmed size in MB, or 0 on error.
 */
uint32_t ddr_get_restricted_size_mb(const struct ddr_info *ddr);

/**
 * ddr_get_restricted_size - Get largest permitted mapped DDR size
 * @ddr - DDR info struct to read
 *
 * This gets the largest permitted DDR size that can be mapped in BOLT.  On
 * ARM, for instance, BOLT does not support LPAE and so all MMU entries are
 * limited to 32-bit space: the returned size will be trimmed such that the
 * mapping will not go over 0xffffffff.
 *
 * Return: The possibly-trimmed size in bytes, or 0 on error.
 */
static inline unsigned long ddr_get_restricted_size(const struct ddr_info *ddr)
{
	return _MB((unsigned long)ddr_get_restricted_size_mb(ddr));
}

#endif  /* __DDR_H__ */
