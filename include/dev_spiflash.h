/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEV_SPIFLASH_H__
#define __DEV_SPIFLASH_H__

#include "device.h"
#include "iocb.h"
#include "bsp_config.h"
#include "flash.h"

/*  *********************************************************************
    *  Flash magic numbers
    ********************************************************************* */

/*
 * Common Flash Interface (CFI) commands and offsets
 */
#define SPANSION_ID			0x01
#define MACRONIX_ID			0xc2
#define ATMEL_ID			0x1f
#define MICRON_ID			0x20
#define SST_ID				0xbf
#define NUMONYX_ID			0x89
#define WINBOND_ID			0xef

/* Common SPI flash command set */
#define SPI_WRSR_CMD			0x01
#define SPI_WRR_CMD			0x01
#define SPI_WREN_CMD			0x06
#define SPI_WRDI_CMD			0x04
#define SPI_RDSR_CMD			0x05
#define SPI_READ_CMD			0x03
#define SPI_QREAD_CMD			0x6B
#define SPI_PP_CMD			0x02
#define SPI_RCR_CMD			0x35
#define SPI_SE_CMD			0xd8
#define SPI_SE_4K_CMD			0x20
#define SPI_SE_32K_CMD			0x52
#define SPI_RDID_CMD			0x9f
#define SPI_RD_SFDP_CMD			0x5a

/*
 * Spansion flash command set
 */
#define SPANSION_SPI_EN4B_CMD		0xb7
#define SPANSION_SPI_EX4B_CMD		0xe9
#define SPANSION_SPI_BRRD_CMD		0x16
#define SPANSION_SPI_BRWR_CMD		0x17
 #define SPI_BR_EXTADD			BIT(7)

/*
 * Macronix flash command set
 */
#define MACRONIX_SPI_EN4B_CMD		0xb7
#define MACRONIX_SPI_EX4B_CMD		0xe9

/*
 * Winbond flash command set (from W25Q256FV Revision H, Feb 11, 2015)
 */
#define WINBOND_SPI_EN4B_CMD		0xb7	/* same as Macronix, Spansion */
#define WINBOND_SPI_EX4B_CMD		0xe9	/* same as Macronix, Spansion */

#define SPI_POLLING_INTERVAL		10	/* in usecs */
#define SPI_CDRAM_CONT			0x80

#define SPI_CDRAM_PCS_DSCK		0x10
#define SPI_CDRAM_PCS			0xe0	/* unused for spi-nor devices */
#define SPI_CDRAM_BITSE			0x40

#define SPI_SYSTEM_CLK			27000000	/* 27 MHz */
#define MAX_SPI_BAUD			1687500	/* SPBR = 8, 27MHZ */

/*  *********************************************************************
    *  Flashop engine constants and structures
    *  The flashop engine interprets a little table of commands
    *  to manipulate flash parts - we do this so we can operate
    *  on the flash that we're currently running from.
    ********************************************************************* */

/*
 * Flash opcodes
 */

#define FEOP_SPI_PGM16			2	/* Program, 16 bits at a time */
#define FEOP_SPI_ERASE16		3	/* Erase 16 bits at a time */
#define FEOP_SPI_READ16			4	/* Read 16 bits at a time */
#define FEOP_SPI_MEMCPY			5	/* generic memcpy */

/*
 *    opcode
 *    flash base
 *    destination
 *    source
 *    length
 *    result
 */
typedef struct spiinstr_s {
	long fi_op;
	long fi_base;
	long fi_dest;
	long fi_src;
	long fi_cnt;
	long fi_result;
} spiinstr_t;

/*
 * Probe structure - this is used when we want to describe to the flash
 * driver the layout of our flash, particularly when you want to
 * manually describe the sectors.
 */

typedef struct spiflash_probe_t {
	unsigned long flash_phys;	/* physical address of flash */
	int flash_page_size;	/* page size for page programming */
	/* The following are used when manually sectoring */
	int flash_nsectors;	/* number of ranges */
	uint32_t flash_max_blocksize;
} spiflash_probe_t;

/*  *********************************************************************
    *  PRIVATE STRUCTURES
    *
    *  These structures are actually the "property" of the
    *  flash driver.  The only reason a board package might
    *  want to dig around in here is if it implements a hook
    *  or overrides functions to support special, weird flash parts.
    ********************************************************************* */

typedef struct {
	uint8_t erase_cmd;
	uint8_t enter_4byte;
	uint8_t exit_4byte;
	uint8_t read_br;
	uint8_t write_br;
} spi_flash_op_codes_t;

#define SPI_MAX_RDID_LEN	6

typedef struct {
	uint8_t id[SPI_MAX_RDID_LEN];
	int id_len;
	uint32_t total_size;
	uint32_t sector_size;
	uint32_t page_size;
	uint32_t erase_block_size;
} spi_flash_device_lookup_t;

#endif /* __DEV_SPIFLASH_H__ */
