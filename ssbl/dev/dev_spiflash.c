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

#include <stdbool.h>
#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "timer.h"
#include "error.h"
#include "common.h"

#include "bchp_hif_mspi.h"

#include "bchp_bspi.h"
#include "bchp_ebi.h"
#include "dev_spiflash.h"
#include "flash.h"
#include <byteorder.h>
#include "cfi.h"

#define REG_U8(x)	(*(volatile uint8_t *)(x))
#define REG_U32(x)	(*(volatile uint32_t *)(x))

enum spi_ident_type {
	SPI_IDENT_NONE = 0,	/* Not identified */
	SPI_IDENT_ID,		/* Found in ID table */
	SPI_IDENT_CFI,		/* CFI flash */
	SPI_IDENT_SFDP,		/* SFDP flash */
};

struct spidev {
	spiflash_probe_t fd_probe;	/* probe information */
	struct flash_dev flash;		/* generic flash device */
	enum spi_ident_type type;

	uint8_t *fd_sectorbuffer;	/* sector copy buffer */
	int fd_ttlsect;		/* total sectors on one device */

	int fd_iptr;		/* flashop engine instructions */
	spiinstr_t *fd_inst;

	uint8_t mfr_id;
};

struct flash_write_s {
	unsigned int start_address;
	unsigned int start_offset;
	unsigned int end_address;
	unsigned int end_offset;
	unsigned int write_len;
};

/*  *********************************************************************
    *  Macros
    ********************************************************************* */

#define MSPI_CALC_TIMEOUT(bytes, baud)	((((bytes * 9000)/baud) * 110)/100 + 1)

/*
 * These are the default values to use in case we couldn't detect the flash type
 */
#define DEFAULT_SPI_FLASH_SIZE			(16*1024*1024)
#define DEFAULT_FLASH_SPI_SECTOR_SIZE		(64*1024)
#define DEFAULT_FLASH_SPI_MAX_PAGE_SIZE		(256)
#define DEFAULT_OPCODE_TABLE			spansion_opcodes

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static void spidrv_probe(bolt_driver_t *drv,
			 unsigned long probe_a, unsigned long probe_b,
			 void *probe_ptr);

static int spidrv_open(bolt_devctx_t *ctx);
static int spidrv_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int spidrv_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int spidrv_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int spidrv_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int spidrv_close(bolt_devctx_t *ctx);
static void calc_sector_offset(flash_sector_t *sector,
			       struct flash_write_s *range);
static void calc_write_range(flash_sector_t *sector,
			     struct flash_write_s *range);
static int mspi_read_flash_data(const void *tx, int wlen, void *buf, int len);
static int mspi_enable_four_byte_addressing(void);
static int mspi_disable_four_byte_addressing(void);

static int bspi_enabled = 1;
static int four_byte_enabled;

static void flash_spi_engine(struct spidev *softc);
static int mspi_sector_erase(unsigned int offset, int four_byte_enabled);
static int mspi_page_program(unsigned int offset, const void *buf, int len,
			     int four_byte_enabled);
static int mspi_enable_write(void);
static int mspi_disable_write(void);

static int bspi_busy_poll(void);
/*  *********************************************************************
    *  Device dispatch
    ********************************************************************* */

static const bolt_devdisp_t spidrv_dispatch = {
	spidrv_open,
	spidrv_read,
	spidrv_inpstat,
	spidrv_write,
	spidrv_ioctl,
	spidrv_close,
	NULL,
	NULL
};

const bolt_driver_t spiflashdrv = {
	"SPI flash",
	"flash",
	BOLT_DEV_FLASH,
	&spidrv_dispatch,
	spidrv_probe
};

/*  *********************************************************************
    *  flash opcodes
    ********************************************************************* */
static spi_flash_op_codes_t spansion_opcodes = {
	.sector_erase	= SPI_SE_CMD,
	.enter_4byte	= SPANSION_SPI_EN4B_CMD,
	.exit_4byte	= SPANSION_SPI_EX4B_CMD,
	.read_br	= SPANSION_SPI_BRRD_CMD,
	.write_br	= SPANSION_SPI_BRWR_CMD,
};

static spi_flash_op_codes_t macronix_opcodes = {
	.sector_erase	= SPI_SE_CMD, /* 64KB block erase in Macronix term */
	.enter_4byte	= MACRONIX_SPI_EN4B_CMD,
	.exit_4byte	= MACRONIX_SPI_EX4B_CMD,
};

static spi_flash_op_codes_t atmel_opcodes = {
	.sector_erase	= ATMEL_SPI_SE_CMD,
};

static spi_flash_op_codes_t winbond_opcodes = {
	.sector_erase	= SPI_SE_CMD, /* 64KB block erase in Winbond term */
	.enter_4byte	= WINBOND_SPI_EN4B_CMD,
	.exit_4byte	= WINBOND_SPI_EX4B_CMD,
};

static spi_flash_op_codes_t *spi_flash_op_tbl;

static spi_flash_device_lookup_t spi_id_table[] = {
	/* ID, ID len, total_size, sector_size, page_size */
	{ {0xc2, 0x20, 0x13},             3, _KB(512),  _KB(64), 256 },
	{ {0xc2, 0x20, 0x14},             3, _MB(1),    _KB(64), 256 },
	{ {0xc2, 0x20, 0x16},             3, _MB(4),    _KB(64), 256 },
	{ {0xc2, 0x20, 0x18},             3, _MB(16),   _KB(64), 256 },
	{ {0xc2, 0x20, 0x19},             3, _MB(32),   _KB(64), 256 },
	{ {0xc2, 0x25, 0x33},             3, _KB(512),  _KB(64), 256 },
	{ {0xc2, 0x25, 0x34},             3, _MB(1),    _KB(64), 256 },
	{ {0xc2, 0x9e, 0x16},             3, _MB(4),    _KB(64), 256 },

	{ {0x01, 0x02, 0x14},             3, _MB(2),    _KB(64), 256 },
	{ {0x01, 0x02, 0x15},             3, _MB(4),    _KB(64), 256 },
	{ {0x01, 0x02, 0x16},             3, _MB(8),    _KB(64), 256 },
	{ {0x01, 0x02, 0x19, 0x4d, 0x00}, 5, _MB(32),  _KB(256), 256 },
	{ {0x01, 0x02, 0x19},             3, _MB(32),   _KB(64), 256 },
	{ {0x01, 0x02, 0x18, 0x03, 0x00}, 5, _MB(16),  _KB(256), 256 },
	{ {0x01, 0x02, 0x18, 0x4d, 0x00}, 5, _MB(16),  _KB(256), 256 },
	{ {0x01, 0x20, 0x18},             3, _MB(16),   _KB(64), 256 },

	{ {0x1f, 0x63},	                  2, _KB(256),  _KB(64), 256 },
	{ {0x1f, 0x26},                   2, _MB(2),   _KB(128), 512 },
	{ {},                             0, 0,         0,       0 },
};

struct sfdp_header {
	char sig[4];
	uint8_t minor_rev;
	uint8_t major_rev;
	uint8_t num_headers;
	uint8_t reserved0;

	/* parameter header */
	uint8_t param_id;
	uint8_t param_minor_rev;
	uint8_t param_major_rev;
	uint8_t param_len; /* In DWORDs */
	le32 param_ptr; /* Top byte is reserved */
} __packed;

struct sfdp_jedec_flash_param {
	le32 unused0;
	le32 size;
	le32 reserved0[5];
	struct {
		uint8_t size; /* log2 */
		uint8_t opcode;
	} __packed sector_types[4];
} __packed;

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int spi_sector_query(struct spidev *softc, flash_sector_t *sector);

/*  *********************************************************************
    *  spi_op_begin(softc)
    *
    *  Reset the pointer to the flash operations so that we can
    *  begin filling in new instructions to execute
    *
    *  Input parameters:
    *	   softc - our softc.
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

#define spi_op_begin(softc) do { (softc)->fd_iptr = 0; } while (0)

/*  *********************************************************************
    *  spi_OP_ADD(softc,op,dest,src,cnt)
    *
    *  Add an instruction to the flashop table
    *
    *  Input parameters:
    *	   softc - our flash
    *	   op,dest,src,cnt - data for the opcode
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void spi_op_add(struct spidev *softc, long base, long op, long dest,
		       long src, long cnt)
{
	spiinstr_t *fi = &(softc->fd_inst[softc->fd_iptr]);

	fi->fi_op = op;
	fi->fi_base = base;
	fi->fi_dest = dest;
	fi->fi_src = src;
	fi->fi_cnt = cnt;

	softc->fd_iptr++;
}

/*  *********************************************************************
    *  spi_op_execute(softc)
    *
    *  Execute the stored "flash operations"
    *
    *  Input parameters:
    *	   softc - our flash
    *
    *  Return value:
    *	   0 if ok, else # of failures (less than zero)
    ********************************************************************* */

static int spi_op_execute(struct spidev *softc)
{
	flash_spi_engine(softc);
	return 0;
}

static void bspi_set_4byte_addressing(bool enabled)
{
	BDEV_WR_F(BSPI_STRAP_OVERRIDE_CTRL, addr_4byte_n_3byte, !!enabled);
	BDEV_WR_F(BSPI_BITS_PER_PHASE, addr_bpp_select, !!enabled);
	BDEV_WR(BCHP_BSPI_FLEX_MODE_ENABLE, !!enabled);
}

/* 1 = success */
static int spi_set_4byte_addressing(bool enabled)
{
	int ret;
	if (enabled)
		ret = mspi_enable_four_byte_addressing();
	else
		ret = mspi_disable_four_byte_addressing();
	if (ret != 1)
		return ret;

	bspi_set_4byte_addressing(enabled);

	four_byte_enabled = enabled;

	return 1;
}

#if CFG_SPI_QUAD_MODE
static void bspi_set_quad_mode(void)
{
	/* set the BSPI STRAP_OVERRIDE to do quad io */
	BDEV_WR_F(BSPI_STRAP_OVERRIDE_CTRL, override, 1);
	BDEV_WR_F(BSPI_STRAP_OVERRIDE_CTRL, data_quad, 1);
}

static void mspi_set_spansion_quad_mode(void)
{
	uint8_t tx_buf[4];
	uint8_t cfg_reg, sts_reg;

	/* RCR */
	tx_buf[0] = SPI_RCR_CMD;
	mspi_read_flash_data(tx_buf, 1, &cfg_reg, 1);
	cfg_reg |= 0x2;
	/* WREN */
	tx_buf[0] = SPI_WREN_CMD;
	mspi_read_flash_data(tx_buf, 1, NULL, 0);
	/* WRR */
	tx_buf[0] = SPI_WRR_CMD;
	tx_buf[1] = 0; /* status register */
	tx_buf[2] = cfg_reg; /* configuration register */
	mspi_read_flash_data(tx_buf, 3, NULL, 0);
	/* wait till ready */
	do {
		tx_buf[0] = SPI_RDSR_CMD;
		mspi_read_flash_data(tx_buf, 1, &sts_reg, 1);
		bspi_busy_poll();
	} while (sts_reg & 1);
}

static void mspi_set_macronix_quad_mode(void)
{
	uint8_t tx_buf[4];
	uint8_t cfg_reg, sts_reg;

	/* RDSR */
	tx_buf[0] = SPI_RDSR_CMD;
	mspi_read_flash_data(tx_buf, 1, &cfg_reg, 1);
	cfg_reg |= 0x40;
	/* WREN */
	tx_buf[0] = SPI_WREN_CMD;
	mspi_read_flash_data(tx_buf, 1, NULL, 0);
	/* WRSR */
	tx_buf[0] = SPI_WRSR_CMD;
	tx_buf[1] = cfg_reg; /* status register */
	mspi_read_flash_data(tx_buf, 2, NULL, 0);
	/* wait till ready */
	do {
		tx_buf[0] = SPI_RDSR_CMD;
		mspi_read_flash_data(tx_buf, 1, &sts_reg, 1);
		bspi_busy_poll();
	} while (sts_reg & 1);
	/* RDSR */
	tx_buf[0] = SPI_RDSR_CMD;
	mspi_read_flash_data(tx_buf, 1, &cfg_reg, 1);
}

static int mspi_set_quad_mode(struct spidev *softc)
{

	switch (softc->mfr_id) {
	case SPANSION_ID:
		mspi_set_spansion_quad_mode();
		break;

	case MACRONIX_ID:
		mspi_set_macronix_quad_mode();
		break;

	/* known part ids not needing any custom cmds to set quad lane io */
	case MICRON_ID:
	case ATMEL_ID:
	case SST_ID:
	case NUMONYX_ID:
	case WINBOND_ID:
		break;

	default:
		xprintf("spi: quad mode io disabled\n");
		return -1;
	}

	return 0;
}

static void spi_set_quad_mode(struct spidev *softc)
{
	int ret;

	ret = mspi_set_quad_mode(softc);

	if (ret == 0)
		bspi_set_quad_mode();
}
#endif

/*  *********************************************************************
    *  spi_erase_range(softc,range)
    *
    *  Erase a range of sectors
    *
    *  Input parameters:
    *	   softc - our flash
    *	   range - range structure
    *
    *  Return value:
    *	   0 if ok
    *	   else error
    ********************************************************************* */

static int spi_erase_range(struct spidev *softc, flash_range_t *range)
{
	flash_sector_t sector;
	int res;

	if (range->range_base + range->range_length >
	    softc->flash.size)
		return BOLT_ERR_INV_PARAM;

	res = 0;

	sector.flash_sector_idx = 0;

	for (;;) {
		res = spi_sector_query(softc, &sector);
		if (res != 0)
			break;
		if (sector.flash_sector_status == FLASH_SECTOR_INVALID)
			break;

		if ((sector.flash_sector_offset >= range->range_base) &&
		    (sector.flash_sector_offset <
		     (range->range_base + range->range_length - 1))) {

			spi_op_begin(softc);
			spi_op_add(softc, softc->fd_probe.flash_phys,
				   FEOP_SPI_ERASE16,
				   sector.flash_sector_offset, 0, 0);
			res = spi_op_execute(softc);

			if (res != 0)
				break;
		}
		sector.flash_sector_idx++;
	}

	return res;
}

/*  *********************************************************************
    *  FLASH_ERASE_ALL(softc)
    *
    *  Erase the entire flash device, except the NVRAM area,
    *  sector-by-sector.
    *
    *  Input parameters:
    *	   softc - our flash
    *
    *  Return value:
    *	   0 if ok
    *	   else error code
    ********************************************************************* */

static int spi_erase_all(struct spidev *softc)
{
	flash_range_t range;

	range.range_base = 0;
	range.range_length = softc->flash.size;

	return spi_erase_range(softc, &range);
}

/*  *********************************************************************
    *  spi_sector_query(softc,sector)
    *
    *  Query the sector information about a particular sector.  You can
    *  call this iteratively to find out about all of the sectors.
    *
    *  Input parameters:
    *	   softc - our flash info
    *	   sector - structure to receive sector information
    *
    *  Return value:
    *	   0 if ok
    *	   else error code
    ********************************************************************* */
static int spi_sector_query(struct spidev *softc, flash_sector_t *sector)
{
	unsigned int offset;
	int whichchip;
	int secidx;

	/* this function is based on the fact that nsectors = 1 */

	if (softc->flash.type != FLASH_TYPE_SPI)
		return BOLT_ERR_UNSUPPORTED;

	if (softc->fd_probe.flash_nsectors == 0)
		return BOLT_ERR_UNSUPPORTED;
	/* Figure out which chip */
	whichchip = sector->flash_sector_idx / softc->fd_ttlsect;
	if (whichchip >= 1) {
		sector->flash_sector_status = FLASH_SECTOR_INVALID;
		return 0;
	}
	/* Within that chip, get sector info */
	offset = softc->flash.size * whichchip;
	secidx = sector->flash_sector_idx % softc->fd_ttlsect;

	if (secidx < softc->fd_ttlsect) {
		sector->flash_sector_status = FLASH_SECTOR_OK;
		sector->flash_sector_offset =
			offset + (secidx * softc->flash.blocksize);
		sector->flash_sector_size = softc->flash.blocksize;
	} else
		sector->flash_sector_status = FLASH_SECTOR_INVALID;

	return 0;
}

/*  *********************************************************************
    *  spi_lookup_device_type()
    *
    *  This function looks up device information based on RDID data.
    *
    *  Input parameters:
    *		softc - SPI device structure
    *		table - pointer to flash manuf table
    *		id    - bytes of RDID data
    *
    *  Output:
    *      pOpcode_table - pointer to opcode table to use
    *
    *  Return value:
    *	   1 if device type is found, 0 if not found
    ********************************************************************* */
static int spi_lookup_device_type(struct spidev *softc,
				  spi_flash_device_lookup_t *table, uint8_t *id)
{
	int i;

	while (table->id_len != 0) {
		bool match = true;
		for (i = 0; match && i < table->id_len; i++)
			if (table->id[i] != id[i])
				match = false;
		if (match) {
			/* Found it */
			softc->flash.size = table->total_size;
			softc->flash.blocksize = table->sector_size;
			softc->fd_probe.flash_page_size = table->page_size;

			softc->type = SPI_IDENT_ID;

			return 1;
		}
		table++;
	}

	return 0;
}

static int spi_fill_addr_buff(uint8_t *buf, unsigned int addr,
			      bool addr_4byte);

static void sfdp_read(void *buf, uint32_t addr, size_t len)
{
	uint8_t cmd[5];
	/* one command byte */
	cmd[0] = SPI_RD_SFDP_CMD;
	/* three address bytes */
	spi_fill_addr_buff(&cmd[1], addr, false);
	/* one dummy byte */

	mspi_enable_write();
	mspi_read_flash_data(cmd, sizeof(cmd), buf, len);
}

static int spi_sfdp_probe(struct spidev *softc)
{
	struct sfdp_header sfdp;
	struct sfdp_jedec_flash_param sfdp_param;
	uint32_t param_offs;
	uint8_t tmp;
	unsigned int i;

	sfdp_read(&sfdp, 0, sizeof(sfdp));

	if (strncmp(sfdp.sig, "SFDP", 4))
		return -1;

	param_offs = le32_to_cpu(sfdp.param_ptr) & 0x00ffffff;
	sfdp_read(&sfdp_param, param_offs, sizeof(sfdp_param));

	softc->flash.size = le32_to_cpu(sfdp_param.size);
	/* Bit 31 distinguishes <= 2Gbit vs. >= 4Gbit */
	if (softc->flash.size & 0x80000000) {
		softc->flash.size &= ~0x80000000;
		softc->flash.size = 1 << softc->flash.size;
	} else
		softc->flash.size += 1;
	softc->flash.size >>= 3; /* Size was in bits */

	/* Get maximum supported sector size */
	tmp = 0;
	for (i = 0; i < ARRAY_SIZE(sfdp_param.sector_types); i++)
		tmp = max(sfdp_param.sector_types[i].size, tmp);
	softc->flash.blocksize = 1 << tmp;

	/* Assume page size is 256B - not specified in SFDP 1.0 */
	softc->fd_probe.flash_page_size = DEFAULT_FLASH_SPI_MAX_PAGE_SIZE;

	softc->type = SPI_IDENT_SFDP;

	return 0;
}

static int spi_cfi_probe(struct spidev *softc)
{
	uint8_t cmd;
	uint8_t buf[MAX_CFIDATA];
	struct cfi_ident *cfi;
	int i;

	cmd = SPI_RDID_CMD;
	mspi_read_flash_data(&cmd, 1, buf, sizeof(buf));
	cfi = (struct cfi_ident *)&buf[CFI_OFFSET];

	if (strncmp(cfi->qry, "QRY", 3))
		return -1;

	softc->flash.size = 1 << cfi->dev_size;
	softc->fd_probe.flash_page_size = 1 << le16_to_cpu(cfi->max_buf_write);

	for (i = 0; i < cfi->num_erase_regions; i++) {
		struct cfi_erase_region *region = &cfi->erase_regions[i];
		int sector_size = le16_to_cpu(region->density) * 256;

		/* Is this the "right" sector size? */
		if (cfi->num_erase_regions == 1 || sector_size > 4096) {
			/* Does the sector size divide evenly? */
			if (sector_size == 0 || softc->flash.size % sector_size)
				return -1;
			softc->flash.blocksize = sector_size;

			softc->type = SPI_IDENT_CFI;

			return 0;
		}
	}
	return -1;
}

/*
 * Handle CFI/SFDP for probing certain SPI flash
 *
 * @softc: SPI device structure
 *
 * Return 0 on success, negative on error/failure to detect
 */
static int spi_auto_detect(struct spidev *softc)
{
	int sts;

	/*
	 * As per Spansion datasheet :
	 * Spansion SFDP parameter points to the ID-CFI address space.
	 * SFDP and ID-CFI information can be accessed by either the RSFDP or
	 * RDID commands, we use CFI to avoid SFDP indirection which does not
	 * seem to work anyways
	 */
	if (softc->mfr_id == SPANSION_ID)
		sts = spi_cfi_probe(softc);
	else
		sts = spi_sfdp_probe(softc);

	return sts;
}

/*  *********************************************************************
    *  spi_probe_device(softc)
    *
    *  Input parameters:
    *	   softc - out flash
    *
    *  Return value:
    *	   0 if successful, <0 if error
    ********************************************************************* */
static int spi_probe_device(struct spidev *softc)
{
	uint8_t cmd;
	uint8_t id[SPI_MAX_RDID_LEN];
	const char *ident_string;

	cmd = SPI_RDID_CMD;
	mspi_read_flash_data(&cmd, 1, id, sizeof(id));

	softc->mfr_id = id[0];

	if (spi_auto_detect(softc)) {
		if (spi_lookup_device_type(softc, spi_id_table, id) == 0) {
			warn_msg("Flash type not found, using defaults!");
			softc->flash.size = DEFAULT_SPI_FLASH_SIZE;
			softc->flash.blocksize = DEFAULT_FLASH_SPI_SECTOR_SIZE;
			softc->fd_probe.flash_page_size =
				DEFAULT_FLASH_SPI_MAX_PAGE_SIZE;
		}
	}

	/* Assign command set */
	switch (softc->mfr_id) {
	case SPANSION_ID:
		spi_flash_op_tbl = &spansion_opcodes;
		break;
	case MACRONIX_ID:
	case MICRON_ID:
		spi_flash_op_tbl = &macronix_opcodes;
		break;
	case ATMEL_ID:
		spi_flash_op_tbl = &atmel_opcodes;
		break;
	case WINBOND_ID:
		spi_flash_op_tbl = &winbond_opcodes;
		break;
	default:
		spi_flash_op_tbl = &DEFAULT_OPCODE_TABLE;
		break;
	}

#if CFG_SPI_QUAD_MODE
	spi_set_quad_mode(softc);
#endif

	if (softc->flash.size > 0x01000000) {
		/* Size >= 32MB requires 32-bit addressing */
		spi_set_4byte_addressing(true);
	}

	switch (softc->type) {
	case SPI_IDENT_CFI:
		ident_string = " CFI";
		break;
	case SPI_IDENT_SFDP:
		ident_string = " SFDP";
		break;
	case SPI_IDENT_ID:
	default:
		ident_string = "";
		break;
	}

	printf("CS0:%s SPI, %dMB, %dkB blocks, %uB pages\n",
	       ident_string,
	       (int)softc->flash.size / (1024 * 1024),
	       softc->flash.blocksize / 1024,
	       softc->fd_probe.flash_page_size);

	softc->fd_probe.flash_nsectors = 1;
	softc->flash.type = FLASH_TYPE_SPI;
	softc->fd_probe.flash_phys = BDEV_RD(BCHP_EBI_CS_BASE_0) &
				     BCHP_EBI_CS_BASE_0_base_addr_MASK;
#if CFG_ARCH_ARM
	softc->fd_probe.flash_phys = 0xe0000000;
#else
	softc->fd_probe.flash_phys = 0x20000000 - softc->flash.size;
#endif

	softc->fd_ttlsect = softc->flash.size / softc->flash.blocksize;

	return 0;
}

/*  *********************************************************************
    *  flashdrv_probe(drv,probe_a,probe_b,probe_ptr)
    *
    *  Device probe routine.  Attach the flash device to
    *  BOLT's device table.
    *
    *  Input parameters:
    *	   drv - driver descriptor
    *	   probe_a - physical address of flash
    *	   probe_b - size of flash (bytes)
    *	   probe_ptr - unused
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
static void spidrv_probe(bolt_driver_t *drv, unsigned long probe_a,
			 unsigned long probe_b, void *probe_ptr)
{
	struct spidev *softc;
	static int flashidx;

	softc = KMALLOC(sizeof(*softc), 0);
	if (!softc) {
		printf("SPI: out of memory\n");
		return;
	}

	memset(softc, 0, sizeof(*softc));

	softc->flash.cs = flashidx; /* Only support CS 0? */
	softc->flash.type = FLASH_TYPE_SPI;
	softc->flash.writesize = 1;

	/* Store driver data */
	softc->flash.priv = softc;

	BDEV_WR(BCHP_HIF_MSPI_SPCR0_LSB, SPI_SYSTEM_CLK / (2 * MAX_SPI_BAUD));
	BDEV_WR(BCHP_HIF_MSPI_SPCR0_MSB,
		BCHP_HIF_MSPI_SPCR0_MSB_CPOL_MASK |
		BCHP_HIF_MSPI_SPCR0_MSB_CPHA_MASK |
#ifdef BCHP_HIF_MSPI_SPCR0_MSB_MSTR_MASK
		BCHP_HIF_MSPI_SPCR0_MSB_MSTR_MASK |
#endif
		(0x8 << BCHP_HIF_MSPI_SPCR0_MSB_data_reg_32_BITS_SHIFT));

	if (spi_probe_device(softc)) {
		KFREE(softc);
		return;
	}

	/* 64KB is the size of the block */
	softc->fd_sectorbuffer =
		(unsigned char *)KMALLOC(softc->flash.blocksize, 4);

	/* Register this flash with the generic flash system */
	flash_register_device(drv, &softc->flash);

	flashidx++;		/* indicates number of flash devices */
}

/*  *********************************************************************
    *  spidrv_open(ctx)
    *
    *  Called when the flash device is opened.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0 if ok else error code
    ********************************************************************* */

static int spidrv_open(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	struct spidev *softc = flashpart_get_drvdata(part);
	int num_instr;

	/*
	 * Calculate number of flashop instructions we'll need at most.
	 * This will be two for each sector plus two more for the first
	 * and last sectors, plus two extra
	 */
	num_instr = (softc->fd_ttlsect * 2) + 6;

	/*
	 * Allocate memory for instructions.
	 */

	softc->fd_inst = KMALLOC(num_instr * sizeof(spiinstr_t), 0);
	if (!softc->fd_inst)
		return BOLT_ERR_NOMEM;

	return 0;
}

/*  *********************************************************************
    *  spidrv_read(ctx,buffer)
    *
    *  Read data from the flash device.    The flash device is
    *  considered to be like a disk (you need to specify the offset).
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - buffer descriptor
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */

static int spidrv_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct spidev *softc = flashpart_get_drvdata(part);
	unsigned int blen;
	unsigned int offset;

	blen = buffer->buf_length;
	offset = (long)buffer->buf_offset;

	if ((offset + blen) > part->size)
		blen = part->size - offset;

	offset += part->offset;

	if (blen > 0) {
		spi_op_begin(softc);
		spi_op_add(softc, softc->fd_probe.flash_phys,
			   FEOP_SPI_READ16, (long)buffer->buf_ptr, offset,
			   blen);
		spi_op_execute(softc);
	}

	buffer->buf_retlen = blen;

	return 0;
}

/*  *********************************************************************
    *  spidrv_inpstat(ctx,inpstat)
    *
    *  Return "input status".  For flash devices, we always return true.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   inpstat - input status structure
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */

static int spidrv_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	inpstat->inp_status = 1;
	return 0;
}

/*  *********************************************************************
    *  spidrv_write(ctx,buffer)
    *
    *  Write data to the flash device.    The flash device is
    *  considered to be like a disk (you need to specify the offset).
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - buffer descriptor
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */
static int spidrv_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{

	struct flash_partition *part = ctx->dev_softc;
	struct spidev *softc = flashpart_get_drvdata(part);
	uint8_t *sector_buffer;
	struct flash_write_s range;
	flash_sector_t sector;
	int res;
	unsigned int blen;
	unsigned int offset;
	long chipbase;

	blen = buffer->buf_length;
	offset = (long)buffer->buf_offset;

	chipbase = softc->fd_probe.flash_phys;
	sector.flash_sector_size = softc->flash.blocksize;
	sector_buffer = softc->fd_sectorbuffer;

	/* Compute the range within the physical flash */
	if (offset + blen > part->size)
		blen = part->size - offset;

	range.start_address = offset + part->offset;
	range.end_address = range.start_address + blen;

	buffer->buf_retlen = blen;

	/* Handle the case of writing nothing */
	if (blen == 0)
		return (buffer->buf_length == blen) ? 0 : BOLT_ERR_IOERR;

	/* Compute the first sector_offset */
	calc_sector_offset(&sector, &range);

	spi_op_begin(softc);

	while (blen > 0) {
		/* Compute the temporary start of the programming */
		calc_write_range(&sector, &range);

		if (range.write_len > sector.flash_sector_size) {
			printf("write_len was not calculated correctly\n");
			return -1;
		}

		if (range.write_len == sector.flash_sector_size) {
			/* Erase and reprogram an entire sector */
			spi_op_add(softc, chipbase,
				   FEOP_SPI_ERASE16,
				   sector.flash_sector_offset, 0, 0);
			spi_op_add(softc, chipbase, FEOP_SPI_PGM16,
				   sector.flash_sector_offset,
				   ((long)buffer->buf_ptr) +
				   /* plus how much has already been
				    * written
				    */
				   (range.start_offset -
				    range.start_address),
				   range.write_len);
		} else {	/* Erase and reprogram a partial sector */
			/* Save old sector */
			spi_op_add(softc, chipbase,
				   FEOP_SPI_READ16,
				   (long)sector_buffer,
				   sector.flash_sector_offset,
				   sector.flash_sector_size);
			/* Copy in new stuff */
			spi_op_add(softc, chipbase, FEOP_SPI_MEMCPY,
				   ((long)sector_buffer) +
				   /* skipping the offset at the beginning
				    * of the first sector to be written to
				    */
				   (range.start_offset -
				    sector.flash_sector_offset),
				   ((long)buffer->buf_ptr) +
				   /* plus how much has already been
				    * written
				    */
				   (range.start_offset -
				    range.start_address),
				   range.write_len);
			/* Erase sector */
			spi_op_add(softc, chipbase,
				   FEOP_SPI_ERASE16,
				   sector.flash_sector_offset, 0, 0);
			/* Program sector */
			spi_op_add(softc, chipbase,
				   FEOP_SPI_PGM16,
				   sector.flash_sector_offset,
				   (long)sector_buffer,
				   sector.flash_sector_size);
		}

		blen -= range.write_len;
		sector.flash_sector_offset += sector.flash_sector_size;
	}

	res = spi_op_execute(softc);

	return (res == 0) ? 0 : BOLT_ERR_IOERR;
}

/*  *********************************************************************
    *  flashdrv_ioctl(ctx,buffer)
    *
    *  Handle special IOCTL functions for the flash.  Flash devices
    *  support NVRAM information, sector and chip erase, and a
    *  special IOCTL for updating the running copy of BOLT.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - descriptor for IOCTL parameters
    *
    *  Return value:
    *	   0 if ok else error
    ********************************************************************* */
static int spidrv_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct spidev *softc = flashpart_get_drvdata(part);
	nvram_info_t *nvinfo;
	struct flash_info *info;
	flash_range_t range;

	switch (buffer->buf_ioctlcmd) {
	case IOCTL_NVRAM_GETINFO:
		/*
		 * We only support NVRAM on flashes that have been partitioned
		 * into at least two partitions.  Every partition supports
		 * being an NVRAM in that case, but we'll only attach one
		 * of them to the environment subsystem.
		 */
		if (softc->flash.nparts <= 1)
			return BOLT_ERR_UNSUPPORTED;
		nvinfo = (nvram_info_t *)buffer->buf_ptr;
		if (buffer->buf_length != sizeof(*nvinfo))
			return BOLT_ERR_INV_PARAM;

		nvinfo->nvram_offset = 0;
		nvinfo->nvram_offs_part = part->offset;
		nvinfo->nvram_size = part->size;
		nvinfo->nvram_eraseflg = 1;
		buffer->buf_retlen = sizeof(*nvinfo);
		return 0;
		break;

	case IOCTL_FLASH_ERASE_SECTOR:
	case IOCTL_FLASH_ERASE_BLOCK:
		spi_op_begin(softc);
		spi_op_add(softc, softc->fd_probe.flash_phys,
			   FEOP_SPI_ERASE16, (long)buffer->buf_offset, 0, 0);

		spi_op_execute(softc);
		return 0;	/* returned one */

	case IOCTL_FLASH_ERASE_ALL:
		if (buffer->buf_offset)
			return -1;
		spi_erase_all(softc);
		return 0;

	case IOCTL_FLASH_GETINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = softc->fd_probe.flash_phys;
		info->flash_size = softc->flash.size;
		info->type = softc->flash.type;
		info->flags = FLASH_FLAG_NOERASE;
		return 0;

	case IOCTL_FLASH_GETPARTINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = part->offset;
		info->flash_size = part->size;
		info->flags = FLASH_FLAG_NOERASE;
		return 0;
	case IOCTL_FLASH_GETSECTORS:
		return spi_sector_query(softc,
					(flash_sector_t *) buffer->buf_ptr);

	case IOCTL_FLASH_ERASE_RANGE:
		memcpy(&range, buffer->buf_ptr, sizeof(flash_range_t));
		range.range_base += part->offset;

		if (range.range_length > part->size)
			range.range_length = part->size;
		return spi_erase_range(softc, &range);

	default:
		return -1;
	}

	return -1;
}

/*  *********************************************************************
    *  flashdrv_close(ctx)
    *
    *  Close the flash device.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0
    ********************************************************************* */
static int spidrv_close(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	struct spidev *softc = flashpart_get_drvdata(part);

	if (softc->fd_inst)
		KFREE(softc->fd_inst);

	softc->fd_inst = NULL;

	/* XXX Invalidate the cache ?!?! */

	return 0;
}

static int bspi_busy_poll(void)
{
	int i;

	/* This should finish within 10us */
	for (i = 0; i < 1000; i++) {
		if (BDEV_RD_F(BSPI_BUSY_STATUS, busy) == 0)
			return 0;
		bolt_usleep(1);
	}

	if (BDEV_RD_F(BSPI_BUSY_STATUS, busy) == 0)
		return 0;

	xprintf("BSPI: timeout while waiting for !busy_status\n");
	return -1;
}

static void mspi_disable_bspi(void)
{
	if (bspi_enabled == 0)
		return;

	if (BDEV_RD_F(BSPI_MAST_N_BOOT_CTRL, mast_n_boot) == 1)
		return;

	/* Wait for BSPI to finish loading */
	bspi_busy_poll();

	BDEV_WR(BCHP_BSPI_MAST_N_BOOT_CTRL, 0x1);

	/* 0 -> driven by BSPI, 1 -> driven by MSPI */
	while (BDEV_RD_F(BSPI_MAST_N_BOOT_CTRL, mast_n_boot) == 0)
		;

	/* the BSPI is disabled */
	bspi_enabled = 0;
}

static void mspi_enable_bspi(void)
{
	if (bspi_enabled == 1)
		return;

	if (BDEV_RD_F(BSPI_MAST_N_BOOT_CTRL, mast_n_boot) == 0)
		return;

	BDEV_WR(BCHP_BSPI_MAST_N_BOOT_CTRL, 0x0);

	bspi_enabled = 1;
	return;
}

static void bspi_flush_prefetch_buffers(void)
{
	/* avoid flushing if B1 prefetch is still active */
	bspi_busy_poll();

	/* force rising edge to flush BSPI buffers */
	BDEV_WR(BCHP_BSPI_B0_CTRL, 0x0);
	BDEV_WR(BCHP_BSPI_B0_CTRL, 0x1);
	BDEV_WR(BCHP_BSPI_B1_CTRL, 0x0);
	BDEV_WR(BCHP_BSPI_B1_CTRL, 0x1);
}

static void bspi_read(struct spidev *softc, void *buf, uint32_t addr,
		      uint32_t len)
{
	uint8_t *rx8 = buf;
	uint32_t *rx32 = NULL;
	int align_sz = sizeof(uint32_t);
	uint32_t upper_addr, reg_base;
	unsigned int i;

	mspi_enable_bspi();

	upper_addr = addr & 0xff000000;
	BDEV_WR(BCHP_BSPI_BSPI_FLASH_UPPER_ADDR_BYTE, upper_addr);

	bspi_flush_prefetch_buffers();

#if CFG_ARCH_ARM /* for ARM, this is not needed */
	reg_base = 0xe0000000;
#else
#error MIPS support not implemented
#endif

	if (IS_ALIGNED(len, align_sz) &&
	    IS_ALIGNED((uintptr_t)buf, align_sz)
	    && IS_ALIGNED(addr, align_sz))
		rx32 = buf;

	for (i = 0; i < len;) {
		/* check if we wrap to a new addr[31:24] */
		if ((addr & 0xff000000) != upper_addr) {
			upper_addr = addr & 0xff000000;
			BDEV_WR(BCHP_BSPI_BSPI_FLASH_UPPER_ADDR_BYTE,
					upper_addr);
		}

		if (rx32) {
			/* aligned copy */
			*rx32++ = REG_U32(reg_base | addr);
			addr += align_sz;
			i += align_sz;
		} else {
			/* bytewise copy! */
			*(rx8++) = REG_U8(reg_base | addr);
			addr++;
			i++;
		}
	}
}

static int spi_execute_one(struct spidev *softc, spiinstr_t *instr)
{
	int result;
	unsigned long reg_src, reg_dest, reg_cnt, i, number_byte_to_write;

	reg_dest = instr->fi_dest;
	reg_src = instr->fi_src;
	reg_cnt = instr->fi_cnt;

	switch (instr->fi_op) {
	case FEOP_SPI_READ16:
		bspi_read(softc, (void *)reg_dest, reg_src, reg_cnt);
		break;

	/* copies the original data into the sector buffer at the sector
	 * buffer adddress (this takes place for the entire sector - 64KB)
	 */
	case FEOP_SPI_MEMCPY:
		mspi_enable_bspi();

		for (i = 0; i < reg_cnt; i += 1) {
			REG_U8(reg_dest) = REG_U8(reg_src);

			reg_src += 1;
			reg_dest += 1;
		}

		break;
	/* copies the new data into the sector buffer at the sector buffer
	 * address (for the length of the new data so that the rest of the
	 * sector data is preserved)
	 */

	case FEOP_SPI_ERASE16:
		mspi_disable_bspi();

		result = mspi_sector_erase(reg_dest, four_byte_enabled);

		if (result == 0)
			printf("erase fail \n");

		mspi_enable_bspi();
		break;

	case FEOP_SPI_PGM16:
		mspi_disable_bspi();

		for (i = 0; i < reg_cnt; i += number_byte_to_write) {
			if (i + softc->fd_probe.flash_page_size >= reg_cnt) {
				/* reg_cnt always a sector size */
				number_byte_to_write = reg_cnt - i;
			} else
				number_byte_to_write =
					softc->fd_probe.flash_page_size;

			/* to prevent address cross the page boundary of 256
			 * bytes.
			 */

			number_byte_to_write = number_byte_to_write <=
					(256 - (i & 0xFF)) ?
					 number_byte_to_write :
					 (256 - (i & 0xFF));

			result = mspi_page_program(reg_dest + i,
						(unsigned char *)reg_src,
						number_byte_to_write,
						four_byte_enabled);
			reg_src += number_byte_to_write;
			if (result == 0)
				printf("program fail\n");

		}
		mspi_disable_write();

		mspi_enable_bspi();
		break;

	default:
		break;

	}
	return 0;
}

static void flash_spi_engine(struct spidev *softc)
{
	int i;
	for (i = 0; i < softc->fd_iptr; i++)
		spi_execute_one(softc, &softc->fd_inst[i]);
}

/*
 * Check to make sure the MSPI has finished executing the
 * command queue by checking the SPIF in the MSPI_STATUS
 * register.  Returns 0 on success, other on failure.
 */
static int mspi_wait(unsigned int timeout_ms)
{
	/* Hopefully no one ever needs a timeout long enough to overflow */
	unsigned int timeout = timeout_ms * 10;

	while (timeout--) {
		/* ~300 ns per read */
		if (BDEV_RD_F(HIF_MSPI_MSPI_STATUS, SPIF)) {
			/* Clear the SPIF bit */
			BDEV_WR(BCHP_HIF_MSPI_MSPI_STATUS, 0);
			return 0;
		}
		bolt_usleep(100);
	}

	printf("Error: The SPIF bit was never set in mspi_wait\n");
	return -1;
}

static int __mspi_writeread(const void *w_buf, unsigned int wlen,
			  void *r_buf, unsigned int rlen, bool cont)
{
	unsigned int lval;
	unsigned char i, len;
	uint8_t *rx = r_buf;
	const uint8_t *tx = w_buf;

	len = wlen + rlen;
	BDEV_WR_F(HIF_MSPI_SPCR0_MSB, data_reg_32_BITS, 8);
	for (i = 0; i < len; i++) {
		if (i < wlen)
			BDEV_WR(BCHP_HIF_MSPI_TXRAM00 + (i * 8), tx[i]);
		lval =
			SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL |
			SPI_CDRAM_PCS_DSCK;
		lval &= ~SPI_CDRAM_PCS_PCS0;
		BDEV_WR((BCHP_HIF_MSPI_CDRAM00 + (i * 4)), lval);
	}

	if (!cont) {
		lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_PCS_DSCK;
		lval &= ~SPI_CDRAM_PCS_PCS0;
		BDEV_WR((BCHP_HIF_MSPI_CDRAM00 + ((len - 1) * 4)), lval);
	}

	/* Set queue pointers */
	BDEV_WR(BCHP_HIF_MSPI_NEWQP, 0);
	BDEV_WR(BCHP_HIF_MSPI_ENDQP, len - 1);

	BDEV_WR(BCHP_HIF_MSPI_WRITE_LOCK, 1);
	if (cont) {
		/* Start SPI transfer */
		lval = BDEV_RD(BCHP_HIF_MSPI_SPCR2);
		lval |= BCHP_HIF_MSPI_SPCR2_cont_after_cmd_MASK |
			BCHP_HIF_MSPI_SPCR2_spe_MASK;
		BDEV_WR(BCHP_HIF_MSPI_SPCR2, lval);
	} else {
		/* Start SPI transfer */
		BDEV_WR_F(HIF_MSPI_SPCR2, spe, 1);
	}

	/* Wait for SPI to finish */
	if (mspi_wait(MSPI_CALC_TIMEOUT(len, MAX_SPI_BAUD)) != 0)
		return 0;

	if (!cont)
		BDEV_WR(BCHP_HIF_MSPI_WRITE_LOCK, 0);

	for (i = wlen; i < len; ++i)
		rx[i - wlen] = BDEV_RD(BCHP_HIF_MSPI_RXRAM01 + (i * 8));

	return 1;
}

static int mspi_writeread(const void *w_buf, unsigned int wlen,
			  void *r_buf, unsigned int rlen)
{
	return __mspi_writeread(w_buf, wlen, r_buf, rlen, false);
}

static int mspi_writeread_continue(const void *w_buf, unsigned int wlen,
				   void *r_buf, unsigned int rlen)
{
	return __mspi_writeread(w_buf, wlen, r_buf, rlen, true);
}

static int mspi_enable_write(void)
{
	uint8_t cmd = SPI_WREN_CMD;
	return mspi_writeread(&cmd, 1, NULL, 0);
}

static int mspi_disable_write(void)
{
	uint8_t cmd = SPI_WRDI_CMD;
	return mspi_writeread(&cmd, 1, NULL, 0);
}

static int mspi_write32_continue(const void *w_buf, unsigned int len)
{
	unsigned int lval, i;
	const uint8_t *tx = w_buf;

	BDEV_WR_F(HIF_MSPI_SPCR0_MSB, data_reg_32_BITS, 0);

	for (i = 0; i < len; i++) {
		BDEV_WR((BCHP_HIF_MSPI_TXRAM00 + (i * 4)), tx[i]);
		BDEV_WR((BCHP_HIF_MSPI_TXRAM00 + (i * 4) + 4), tx[i + 1]);
		lval =
			SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL |
			SPI_CDRAM_PCS_DSCK | SPI_CDRAM_BITSE;
		lval &= ~SPI_CDRAM_PCS_PCS0;
		BDEV_WR((BCHP_HIF_MSPI_CDRAM00 + ((i / 2) * 4)), lval);
	}

	/* Set queue pointers */
	BDEV_WR(BCHP_HIF_MSPI_NEWQP, 0);
	BDEV_WR(BCHP_HIF_MSPI_ENDQP, (len / 2) - 1);

	BDEV_WR(BCHP_HIF_MSPI_WRITE_LOCK, 1);

	/* Start SPI transfer */
	lval = BDEV_RD(BCHP_HIF_MSPI_SPCR2);
	lval |= BCHP_HIF_MSPI_SPCR2_cont_after_cmd_MASK |
		BCHP_HIF_MSPI_SPCR2_spe_MASK;
	BDEV_WR(BCHP_HIF_MSPI_SPCR2, lval);

	/* Wait for SPI to finish */
	if (mspi_wait(MSPI_CALC_TIMEOUT(len, MAX_SPI_BAUD)) != 0)
		return 0;

	return 1;
}

/* return number of written bytes */
static int spi_fill_addr_buff(uint8_t *buf, unsigned int addr,
			      bool addr_4byte)
{
	int i, bytes = 3 + !!addr_4byte;
	for (i = bytes - 1; i >= 0; i--)
		*(buf++) = (addr >> (i * 8)) & 0xff;
	return bytes;
}

static int mspi_sector_erase(unsigned int offset, int fourb_enabled)
{
	int result = 0;
	unsigned char cmd[5];
	unsigned char data;
	int cmd_len = 0;

	if (mspi_enable_write() != 1)
		goto done;

	cmd[cmd_len++] = spi_flash_op_tbl->sector_erase;

	cmd_len += spi_fill_addr_buff((uint8_t *)cmd + cmd_len, offset,
				      fourb_enabled);

	result = mspi_writeread(cmd, cmd_len, NULL, 0);
	if (result != 1)
		goto done;

	do {
		cmd[0] = SPI_RDSR_CMD;
		result = mspi_writeread(cmd, 1, &data, 1);
		if (result != 1)
			goto done;
	} while (data & 0x01 /* busy */);

	/* disable the write */
	if (mspi_disable_write() != 1)
		goto done;

done:
	return result;
}

static int mspi_page_program(unsigned int offset, const void *buf, int len,
			     int fourb_enabled)
{
	int result = 0;
	int i, len_total;
	static uint8_t cmd[DEFAULT_FLASH_SPI_MAX_PAGE_SIZE + 5];
	unsigned char data;
	int cmd_len = 0;

	/* Max bytes per transaction */
	if (len > DEFAULT_FLASH_SPI_MAX_PAGE_SIZE)
		goto done;

	if (mspi_enable_write() != 1)
		goto done;
	cmd[cmd_len++] = SPI_PP_CMD;

	cmd_len += spi_fill_addr_buff(cmd + cmd_len, offset, fourb_enabled);

	memcpy(cmd + cmd_len, buf, len);

	i = 0;
	len_total = len + cmd_len;

	while (len_total > 32) {
		result = mspi_write32_continue(cmd + i, 32);
		if (result != 1)
			goto done;
		i += 32;
		len_total -= 32;
	}
	while (len_total > 16) {
		result = mspi_writeread_continue(cmd + i, 16, NULL, 0);
		if (result != 1)
			goto done;
		i += 16;
		len_total -= 16;
	}
	if (len_total <= 16 && len_total > 0) {
		result = mspi_writeread(cmd + i, len_total, NULL, 0);
		if (result != 1)
			goto done;
	}

	do {
		cmd[0] = SPI_RDSR_CMD;
		result = mspi_writeread(cmd, 1, &data, 1);
		if (result != 1)
			goto done;
	} while (data & 0x01 /* busy */);

done:
	return result;
}

static void calc_sector_offset(flash_sector_t *sector,
				struct flash_write_s *range)
{
	sector->flash_sector_offset = 0;
	while (range->start_address > sector->flash_sector_offset)
		sector->flash_sector_offset += sector->flash_sector_size;
	if (range->start_address < sector->flash_sector_offset)
		sector->flash_sector_offset -= sector->flash_sector_size;
}

static void calc_write_range(flash_sector_t *sector,
				struct flash_write_s *range)
{
	/* Compute the start */
	if (range->start_address > sector->flash_sector_offset)
		range->start_offset = range->start_address;
	else
		range->start_offset = sector->flash_sector_offset;

	/* Compute the end */
	if (range->end_address <
	    (sector->flash_sector_offset + sector->flash_sector_size))
		range->end_offset = range->end_address;
	else
		range->end_offset =
			sector->flash_sector_offset + sector->flash_sector_size;

	range->write_len = (range->end_offset - range->start_offset);
}

/*
 * Write (e.g., command, address), then read
 */
static int mspi_read_flash_data(const void *tx, int wlen, void *buf, int len)
{
	int total = wlen + len;
	int wcount = 0, rcount = 0;
	int i, res = 0;

	mspi_disable_bspi();

	for (i = 0; i < total; i += 16) {
		int w = min(16, wlen - wcount);
		int r = min(16 - w, len - rcount);
		bool cont = (i + 16 < total);

		res = __mspi_writeread(tx + wcount, w, buf + rcount, r, cont);
		if (!res)
			break;

		wcount += w;
		rcount += r;
	}

	mspi_enable_bspi();
	return res;		/* 1 -> successful */
}

static int mspi_enable_four_byte_addressing(void)
{
	int result;
	unsigned char cmd[2];

	mspi_disable_bspi();

	if (spi_flash_op_tbl->write_br) {
		/* BRWR: bank address register write command - does NOT need a
		 * WREN to precede it
		 */
		cmd[0] = spi_flash_op_tbl->write_br;
		/* Enable EXTADDR mode */
		cmd[1] = SPI_BR_EXTADD;
		result = mspi_writeread(cmd, 2, NULL, 0);
		if (result != 1) {
			printf("Could not read flash information bits!\n");
			result = 0;
		}
	} else if (spi_flash_op_tbl->enter_4byte) {
		if (mspi_enable_write() != 1) {
			mspi_enable_bspi();
			return 0;
		}
		cmd[0] = spi_flash_op_tbl->enter_4byte;
		result = mspi_writeread(cmd, 1, NULL, 0);
		if (result != 1) {
			printf("Unable to enter 4-byte address mode\n");
			result = 0;
		}
	} else {
		printf("4-byte addressing not supported\n");
		result = 0;
	}
	mspi_enable_bspi();
	return result;		/* 1 -> successful */
}

static int mspi_disable_four_byte_addressing(void)
{
	int result;
	unsigned char cmd[2];

	mspi_disable_bspi();

	if (spi_flash_op_tbl->write_br) {
		/* BRWR: bank address register write command - does NOT need a
		 * WREN to precede it
		 */
		cmd[0] = spi_flash_op_tbl->write_br;
		/* Disable EXTADDR mode */
		cmd[1] = 0;
		result = mspi_writeread(cmd, 2, NULL, 0);
		if (result != 1) {
			printf("Could not read flash information bits!\n");
			result = 0;
		}
	} else if (spi_flash_op_tbl->exit_4byte) {
		if (mspi_enable_write() != 1) {
			mspi_enable_bspi();
			return 0;
		}
		cmd[0] = spi_flash_op_tbl->exit_4byte;
		result = mspi_writeread(cmd, 1, NULL, 0);
		if (result != 1) {
			printf("Unable to enter 4-byte address mode\n");
			result = 0;
		}
	} else {
		printf("4-byte addressing not supported\n");
		result = 0;
	}

	mspi_enable_bspi();
	return result;		/* 1 -> successful */
}
