/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __NAND_COMMON_H__
#define __NAND_COMMON_H__

#include <bchp_nand.h>

#define NAND_CONTROLLER_REVISION (BCHP_NAND_REVISION_MAJOR_DEFAULT * 0x100 + \
				  BCHP_NAND_REVISION_MINOR_DEFAULT)

#if NAND_CONTROLLER_REVISION < 0x701
/* NAND controller 7.0 or lower */

/*
 * The NAND_CONFIG_CSx block and page size fields take a non-uniform set of
 * values. With luck, this won't have too many permutations over future NAND
 * controller revisions.
 */

/* From NAND_CONFIG_CSx BLOCK_SIZE field */
static const unsigned int __maybe_unused nand_block_sizes[] = {
	8 * 1024,
	16 * 1024,
	128 * 1024,
	256 * 1024,
	512 * 1024,
	1 * 1024 * 1024,
	2 * 1024 * 1024,
};

/* From NAND_CONFIG_CSx PAGE_SIZE field */
static const unsigned int __maybe_unused nand_page_sizes[] = {
	512,
	2048,
	4096,
	8192,
};

#else
/* NAND controller 7.1 or higher */

static const unsigned int __maybe_unused nand_block_sizes[] = {
	8 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_8KB */
	16 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_16KB */
	32 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_32KB */
	64 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_64KB */
	128 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_128KB */
	256 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_256KB */
	512 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_512KB */
	1024 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_1024KB */
	2048 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_2048KB */
	4096 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_4096KB */
	8192 * 1024  /* BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_BK_SIZE_8192KB */
};

static const unsigned int __maybe_unused nand_page_sizes[] = {
	512, /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_512 */
	1024, /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_1KB */
	2 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_2KB */
	4 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_4KB */
	8 * 1024, /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_8KB */
	16 * 1024 /* BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_PG_SIZE_16KB  */
};

#endif /* BCHP_NAND_REVISION_MAJOR && BCHP_NAND_REVISION_MINOR */


#endif /* __NAND_COMMON_H__ */

