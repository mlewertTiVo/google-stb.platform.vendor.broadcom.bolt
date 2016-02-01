/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __DEV_NANDFLASH_H__
#define __DEV_NANDFLASH_H__

#include "device.h"
#include "iocb.h"
#include "lib_types.h"
#include "flash.h"

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

/*
 * Probe structure - this is used when we want to describe to the flash
 * driver the layout of our flash, particularly when you want to
 * manually describe the sectors.
 */
struct nand_probe_info {
	int cs;			/* Chip select; passed into driver probe */
	int chip_idx;		/* index in known_nand_chips table */

	unsigned long bbi_map;	/* bad block indicator map */
	int ecc_level;		/* correction per 512 bytes */
	int sector_size_1k;	/* 1: 1KB ECC sector; 0: 512B ECC sector */
	unsigned int oob_sector;/* OOB bytes per 512B page area */

	int supports_cache_read;/* Supports Read Cache command */
};

/*  *********************************************************************
    *  PRIVATE STRUCTURES
    *
    *  These structures are actually the "property" of the
    *  flash driver.  
    *	
    ********************************************************************* */

struct nand_dev {
	struct nand_probe_info info;	/* probe information */
	struct flash_dev flash;		/* generic flash device */

	void /* __iomem */ *flash_dma_base;

	uint8_t *fd_sectorbuffer;	/* sector copy buffer */
	void *page_buf;			/* intermediate page buffer */
	struct flash_dma_desc *dma_desc;

	int fp_blk_status_valid;
	unsigned int *blk_status;
};

typedef struct {
	uint64_t new_range_base;
	uint32_t first_block_data_offset;
	uint32_t last_block_data_offset;
	uint32_t num_block_access;
} range_descriptor_t;

#endif /* __DEV_NANDFLASH_H__ */
