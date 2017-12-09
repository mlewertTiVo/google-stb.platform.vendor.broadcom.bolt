/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdbool.h>

#include "lib_hexdump.h"
#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "error.h"
#include "dev_nandflash.h"
#include "byteorder.h"
#include "bitops.h"
#include "compiler.h"
#include "board_init.h"
#include "cache_ops.h"
#include "timer.h"

#include "bchp_nand.h"
#include "bchp_hif_intr2.h"
#include "common.h"
#include "fsbl-common.h"
#include "nand_chips.h"
#include "ssbl-common.h"
#include "nand-common.h"

#ifdef NAND_DEBUG
#define DBG(...)	printf(__VA_ARGS__)
#define DBG_HEXDUMP(buf, len) \
	lib_hexdump((buf), (len), 1, false, DUMP_PREFIX_ADDRESS)
#else
#define DBG(...)
#define DBG_HEXDUMP(buf, len)
#endif

/* TODO: Hard-code this here, for now; I will clean this up eventually, but
 * we definitely don't want it user configurable */
#define NAND_BAD_BLOCK_INDICATOR_MAP 0x00000301

enum {
	NAND_ERR		= -3,
	NAND_ERR_TIMEOUT	= -2,
	NAND_ERR_UNCORR		= -1,
	NAND_ERR_CORR		= 1,
};

/* Consecutive readable/writeable spare area registers, in bytes */
#if (BCHP_NAND_REVISION_MAJOR_DEFAULT >= 7 && \
	BCHP_NAND_REVISION_MINOR_DEFAULT == 2)
#define NAND_MAX_SPARE_AREA 128
#else
#define NAND_MAX_SPARE_AREA 64
#endif

#define REG_ACC_CONTROL(cs) (BCHP_NAND_ACC_CONTROL_CS0 + \
	(cs) * (BCHP_NAND_ACC_CONTROL_CS1 - BCHP_NAND_ACC_CONTROL_CS0))

#define WR_ACC_CONTROL(cs, field, val) do { \
	uint32_t reg = REG_ACC_CONTROL(cs), contents = BDEV_RD(reg); \
	contents &= ~(BCHP_NAND_ACC_CONTROL_CS1_##field##_MASK); \
	contents |= (val) << BCHP_NAND_ACC_CONTROL_CS1_##field##_SHIFT; \
	BDEV_WR(reg, contents); \
	} while (0)

#define RD_ACC_CONTROL(cs, field) \
	((BDEV_RD(REG_ACC_CONTROL(cs)) & \
	BCHP_NAND_ACC_CONTROL_CS1_##field##_MASK) \
		>> BCHP_NAND_ACC_CONTROL_CS1_##field##_SHIFT)

#define WR_CORR_THRESH(cs, val) do { \
	uint32_t contents = BDEV_RD(BCHP_NAND_CORR_STAT_THRESHOLD); \
	uint32_t shift = BCHP_NAND_CORR_STAT_THRESHOLD_CORR_STAT_THRESHOLD_CS1_SHIFT * (cs); \
	contents &= ~(BCHP_NAND_CORR_STAT_THRESHOLD_CORR_STAT_THRESHOLD_CS0_MASK \
			<< shift); \
	contents |= ((val) & BCHP_NAND_CORR_STAT_THRESHOLD_CORR_STAT_THRESHOLD_CS0_MASK) \
			<< shift; \
	BDEV_WR(BCHP_NAND_CORR_STAT_THRESHOLD, contents); \
	} while (0);

#define REG_TIMING(cs, idx) (BCHP_NAND_TIMING_1_CS0 + ((idx) - 1) * 4 + \
	(cs) * (BCHP_NAND_TIMING_1_CS1 - BCHP_NAND_TIMING_1_CS0))

#define WR_TIMING(cs, idx, val) BDEV_WR(REG_TIMING(cs, idx), val)

#define NAND_FC(x)	(BCHP_NAND_FLASH_CACHEi_ARRAY_BASE + ((x) << 2))

/* Get a single word from the flash cache, at word index i */
#define GET_NAND_FC_WORD(i) \
	(BDEV_RD(NAND_FC(i)))
/* Get a single byte from the flash cache, at byte position i */
#define GET_NAND_FC_BYTE(i) \
	((BDEV_RD(NAND_FC((i) >> 2)) >> (((i) & 0x3) * 8)) & 0xff)

#define NAND_CMD_PAGE_READ					0x1
#define NAND_CMD_SPARE_AREA_READ				0x2
#define NAND_CMD_STATUS_READ					0x3
#define NAND_CMD_PROGRAM_PAGE					0x4
#define NAND_CMD_PROGRAM_SPARE_AREA				0x5
#define NAND_CMD_COPY_BACK					0x6
#define NAND_CMD_DEVICE_ID_READ					0x7
#define NAND_CMD_BLOCK_ERASE					0x8
#define NAND_CMD_FLASH_RESET					0x9
#define NAND_CMD_BLOCKS_LOCK					0xa
#define NAND_CMD_BLOCKS_LOCK_DOWN				0xb
#define NAND_CMD_BLOCKS_UNLOCK					0xc
#define NAND_CMD_READ_BLOCKS_LOCK_STATUS			0xd
#define NAND_CMD_PARAMETER_READ					0xe
#define NAND_CMD_PARAMETER_CHANGE_COL				0xf
#define NAND_CMD_LOW_LEVEL_OP					0x10

enum {
	NAND_BLOCK_STATUS_GOOD		= 0,
	NAND_BLOCK_STATUS_BAD,
};

/* Low-level operation types: command, address, write, or read */
enum llop_type {
	LL_OP_CMD,
	LL_OP_ADDR,
	LL_OP_WR,
	LL_OP_RD,
};

enum {
	LLOP_RE				= BIT(16),
	LLOP_WE				= BIT(17),
	LLOP_ALE			= BIT(18),
	LLOP_CLE			= BIT(19),
	LLOP_RETURN_IDLE		= BIT(31),
	LLOP_DATA_MASK			= 0x0000ffff
};

enum nand_feature_cmd {
	NAND_GET_FEATURE_CMD = 0xee,
	NAND_SET_FEATURE_CMD = 0xef,
};

/* Flash cache size, in bytes */
#define NAND_FC_SHIFT						9
#define NAND_FC_SIZE						(1 << NAND_FC_SHIFT)

/* NAND INTFC status bits */
#define NAND_STATUS_FAIL	0x01

/*
 * ONFI CRC-16 defines
 * see: http://www.onfi.org/~/media/onfi/specs/onfi_2_0_gold.pdf?la=en
 */
#define ONFI_CRC16_INIT	0x4F4E
#define ONFI_CRC16_POLYNOM	0x8005
#define ONFI_CRC16_HIBIT	0x8000
#define ONFI_CRC16_LEN		254
#define ONFI_MAX_PARAM_PAGES	3

/***********************************************************************
 * Flash DMA
 ***********************************************************************/

/* Just assume everything is 64-bit, for simplicity */
typedef uint64_t dma_addr_t;

struct flash_dma_desc {
	uint32_t next_desc;
	uint32_t next_desc_ext;
	uint32_t cmd_irq;
	union {
		struct {
			uint32_t dram_addr;
			uint32_t dram_addr_ext;
			uint32_t tfr_len;
			uint32_t total_len;
			uint32_t flash_addr;
			uint32_t flash_addr_ext;
			uint32_t cs;
			uint32_t pad2[5];
		};
		struct {
			struct {
				uint32_t reg_addr;
				uint32_t reg_cmd;
				uint32_t reg_data;
			} regs[4];
		};
	};
	uint32_t status_valid;
} __packed;

/* Bitfields for flash_dma_desc::status_valid */
#define FLASH_DMA_ECC_ERROR	(1 << 8)
#define FLASH_DMA_CORR_ERROR	(1 << 9)

#if NAND_CONTROLLER_REVISION >= 0x703
enum flash_dma_reg {
	FLASH_DMA_REVISION		= 0x00,
	FLASH_DMA_FIRST_DESC		= 0x08,
	FLASH_DMA_FIRST_DESC_EXT	= 0x0,
	FLASH_DMA_CTRL			= 0x10,
	FLASH_DMA_MODE			= 0x14,
	FLASH_DMA_STATUS		= 0x18,
	FLASH_DMA_INTERRUPT_DESC	= 0x20,
	FLASH_DMA_INTERRUPT_DESC_EXT	= 0x0,
	FLASH_DMA_ERROR_STATUS		= 0x28,
	FLASH_DMA_CURRENT_DESC		= 0x30,
	FLASH_DMA_CURRENT_DESC_EXT	= 0x0,
};
#else
enum flash_dma_reg {
	FLASH_DMA_REVISION		= 0x00,
	FLASH_DMA_FIRST_DESC		= 0x04,
	FLASH_DMA_FIRST_DESC_EXT	= 0x08,
	FLASH_DMA_CTRL			= 0x0c,
	FLASH_DMA_MODE			= 0x10,
	FLASH_DMA_STATUS		= 0x14,
	FLASH_DMA_INTERRUPT_DESC	= 0x18,
	FLASH_DMA_INTERRUPT_DESC_EXT	= 0x1c,
	FLASH_DMA_ERROR_STATUS		= 0x20,
	FLASH_DMA_CURRENT_DESC		= 0x24,
	FLASH_DMA_CURRENT_DESC_EXT	= 0x28,
};
#endif

static inline bool has_flash_dma(struct nand_dev *nand)
{
	return CFG_FLASH_DMA && nand->flash_dma_base;
}

static inline bool flash_dma_buf_ok(const void *buf)
{
	return buf && IS_ALIGNED((uintptr_t)buf, 4);
}

static inline void flash_dma_writel(struct nand_dev *nand, uint32_t offs,
				    uint32_t val)
{
	DEV_WR((unsigned long)nand->flash_dma_base + offs, val);
}

static inline uint32_t flash_dma_readl(struct nand_dev *nand, uint32_t offs)
{
	return DEV_RD((unsigned long)nand->flash_dma_base + offs);
}

static inline void flash_dma_write64(struct nand_dev *nand, uint32_t offs,
				    uint64_t val)
{
	DEV_WR64((unsigned long)nand->flash_dma_base + offs, val);
}

static inline uint64_t flash_dma_read64(struct nand_dev *nand, uint32_t offs)
{
	return DEV_RD64((unsigned long)nand->flash_dma_base + offs);
}

static inline bool flash_dma_irq_done(void)
{
	return BDEV_RD_F(HIF_INTR2_CPU_STATUS, FLASH_DMA_DONE_INTR);
}

static inline void flash_dma_irq_clear(void)
{
	BDEV_WR(BCHP_HIF_INTR2_CPU_CLEAR,
		BCHP_HIF_INTR2_CPU_CLEAR_FLASH_DMA_DONE_INTR_MASK);
}

/* Command types for a Flash DMA Register Descriptor */
enum dma_reg_cmd {
	DMA_REG_READ		= 0, /* Read register */
	DMA_REG_WRITE		= 1, /* Write register */
	DMA_REG_WR_F_AND	= 2, /* Write last read & mask */
	DMA_REG_WR_F_OR		= 3, /* Write last read | mask */
	DMA_REG_POLL_0		= 4, /* Poll until read 0 */
	DMA_REG_POLL_1		= 5, /* Poll until read 1 */
	DMA_REG_POLL_0_RESTART	= 6, /* Poll for 0, else restart descriptor */
	DMA_REG_POLL_1_RESTART	= 7, /* Poll for 1, else restart descriptor */
};

/*
 * Represents a single element in a DMA linked-list; may be either
 *  - a scatter/gather operation: PROGRAM_PAGE or PAGE_READ; or
 *  - a register operation: read, write, RMW, etc. a register
 */
struct dma_transaction {
	bool is_register;
	union {
		/* Scatter-gather */
		struct {
			dma_addr_t addr;
			size_t len;
			uint8_t cmd;
		};
		/* Register */
		struct {
			struct {
				uint32_t reg_addr;
				uint32_t reg_data;
				enum dma_reg_cmd reg_cmd;
			} reg_op[4];
		};
	};
};

static int nanddrv_open(bolt_devctx_t * ctx);
static int nanddrv_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer);

static int nanddrv_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer);

static uint32_t do_nand_cmd(struct nand_probe_info *info, uint32_t cmd,
		uint64_t address);
static int nand_get_page_status(struct nand_probe_info *info,
		uint64_t page_addr, uint32_t bbi_byte_map);
static int nand_get_block_status(struct nand_dev *nand, uint64_t blk_addr);
static void nand_mark_page_bad(struct nand_dev *nand, uint64_t page_addr);
static int nanddrv_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat);
static int nanddrv_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int nanddrv_close(bolt_devctx_t * ctx);
static int32_t nand_get_range_intersection(uint64_t range_base,
					   uint32_t range_len,
					   uint32_t blk_size,
					   range_descriptor_t * rdt);

static void nanddrv_probe(bolt_driver_t * drv,
			  unsigned long probe_a,
			  unsigned long probe_b, void *probe_ptr);
static int32_t nand_erase_block(struct nand_dev *nand, uint64_t blk_addr);

static const bolt_devdisp_t nanddrv_dispatch = {
	.dev_open	= nanddrv_open,
	.dev_read	= nanddrv_read,
	.dev_inpstat	= nanddrv_inpstat,
	.dev_write	= nanddrv_write,
	.dev_ioctl	= nanddrv_ioctl,
	.dev_close	= nanddrv_close,
};

bolt_driver_t nandflashdrv = {
	.drv_description	= "NAND flash",
	.drv_bootname		= "flash",
	.drv_class		= BOLT_DEV_FLASH,
	.drv_dispatch		= &nanddrv_dispatch,
	.drv_probe		= nanddrv_probe,
};

/* See onfi_params::opt_cmds */
enum onfi_opt_cmds {
	ONFI_OPT_CMD_CACHE_PROGRAM		= BIT(0),
	ONFI_OPT_CMD_CACHE_READ			= BIT(1),
	ONFI_OPT_CMD_GET_SET_FEATURE		= BIT(2),
	ONFI_OPT_CMD_RD_STATUS_ENH		= BIT(3),
	ONFI_OPT_CMD_COPYBACK			= BIT(4),
	ONFI_OPT_CMD_RD_UNIQ_ID			= BIT(5),
	ONFI_OPT_CMD_CHG_COL_ENH		= BIT(6),
	ONFI_OPT_CMD_CHG_ROW			= BIT(7),
	ONFI_OPT_CMD_SMALL_DATA_MOV		= BIT(8),
	ONFI_OPT_CMD_RESET_LUN			= BIT(9),
	ONFI_OPT_CMD_VOL_SEL			= BIT(10),
	ONFI_OPT_CMD_ODT_CONFIG			= BIT(11),
};

struct onfi_params {
	/* Revision info and features block */
	char onfi[4]; /* 'O' 'N' 'F' 'I' */
	le16 revision;
	le16 features;
	le16 opt_cmds;
	uint8_t adv_cmd;
	uint8_t reserved0;
	le16 ext_param_page_len;
	uint8_t num_param_pages;
	uint8_t reserved[17];

	/* Manufacturer info block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id;
	le16 date_code;
	uint8_t reserved2[13];

	/* Memory organization block */
	le32 page_size;
	le16 oob_per_page;
	uint8_t reserved3[6];
	le32 pages_per_block;
	le32 blocks_per_lun;
	uint8_t num_lun;
	uint8_t address_cycles;
	uint8_t bits_per_cell;
	le16 max_bb_per_lun;
	le16 block_endurance;
	uint8_t guaranteed_good_blocks;
	le16 guaranteed_endurance;
	uint8_t programs_per_page;
	uint8_t obsolete;
	uint8_t ecc_bits;
	uint8_t plane_address_bits;
	uint8_t multi_plane_attrs;
	uint8_t ez_nand;
	uint8_t reserved4[12];

	/* Electrical parameters block */
	uint8_t pin_capacitance_max;
	le16 async_timing_mode;
	le16 obsolete2;
	le16 t_prog;
	le16 t_bers;
	le16 t_r;
	le16 t_ccs;
	uint8_t ddr_timing_mode;
	uint8_t ddr2_timing_mode;
	uint8_t ddr_features;
	le16 clk_pin_capacitance_typ;
	le16 io_pin_capacitance_typ;
	le16 input_pin_capacitance_typ;
	uint8_t input_pin_capacitance_max;
	uint8_t driver_strength;
	le16 t_r_multiplane;
	le16 t_adl;
	le16 t_r_eznand;
	uint8_t ddr2_features;
	uint8_t ddr2_warmup_cycles;
	uint8_t reserved5[4];

	/* Vendor block */
	uint8_t reserved6[90];

	le16 crc;
} __packed;

struct onfi_extended_params {
	le16 crc;
	char epps[4]; /* 'E' 'P' 'P' 'S' */
	uint8_t reserved[10];

	struct {
		uint8_t type;
		uint8_t length;
	} __packed section_types[8];

	/* Sections laid out sequentially according to section_types */
	uint8_t sections[];
} __packed;

enum onfi_section_type {
	ONFI_SECTION_NONE = 0,
	ONFI_SECTION_TYPE_LEN = 1,
	ONFI_SECTION_ECC = 2,
};

struct onfi_ecc_block {
	uint8_t ecc_bits;
	uint8_t codeword_size_shift;
	le16 max_bb_per_lun;
	le16 block_endurance;
	uint8_t reserved[2];
};

/*
 * ONFI CRC-16 function as per ONFI spec
 * see: http://www.onfi.org/~/media/onfi/specs/onfi_2_0_gold.pdf?la=en
 */
static uint16_t onfi_crc16(uint8_t const *buf, size_t len)
{
	int i;
	uint16_t crc = ONFI_CRC16_INIT;

	while (len--) {
		crc ^= *buf++ << 8;
		for (i = 0; i < 8; i++) {
			if (crc & ONFI_CRC16_HIBIT)
				crc = (crc << 1) ^ ONFI_CRC16_POLYNOM;
			else
				crc = (crc << 1) ^ 0;
		}
	}

	return crc;
}

/*
 * Read an ONFI parameter page (standard or extended) into a buffer
 * Return 0 for success
 */
static int onfi_parameter_read(struct nand_probe_info *info, uint32_t *buf, int column, int len)
{
	int offs, sect_1k;

	/*
	 * SWBOLT-154: workaround for CRNAND-6 quirk; the address for
	 * PARAMETER_CHANGE_COL does not compute properly with SECTOR_SIZE_1K=1
	 */
	sect_1k = RD_ACC_CONTROL(info->cs, SECTOR_SIZE_1K);
	if (sect_1k)
		WR_ACC_CONTROL(info->cs, SECTOR_SIZE_1K, 0);

	do_nand_cmd(info, NAND_CMD_PARAMETER_READ, 0);
	if (column)
		do_nand_cmd(info, NAND_CMD_PARAMETER_CHANGE_COL, column & ~(NAND_FC_SIZE - 1));
	for (offs = 0; offs < len; offs += 4) {
		int idx = ((column + offs) % NAND_FC_SIZE) >> 2;
		if (idx == 0)
			do_nand_cmd(info, NAND_CMD_PARAMETER_CHANGE_COL, column + offs);
		buf[offs >> 2] = swap32(GET_NAND_FC_WORD(idx));
	}

	/* SWBOLT-154: restore previous sector size */
	if (sect_1k)
		WR_ACC_CONTROL(info->cs, SECTOR_SIZE_1K, sect_1k);
	return 0;
}

/* Locate a particular ONFI extended parameter section type (e.g., ECC
 * information section). See ONFI v3.0, section 5.7.2. */
static int onfi_ext_params_section_offset(struct onfi_extended_params *params,
		enum onfi_section_type type, int *block_len)
{
	unsigned int i, offset = 0;
	for (i = 0; i < ARRAY_SIZE(params->section_types); i++) {
		if (!params->section_types[i].length)
			break; /* Section listing is complete */

		if (params->section_types[i].type == type) {
			*block_len = params->section_types[i].length;
			return offset;
		}
		offset += params->section_types[i].length * 16;
	}

	/* Section type not found */
	*block_len = 0;
	return 0;
}

/*
 * Get the minimum required ECC settings from ONFI (extended) parameter page
 * Return 0 for success
 */
static int onfi_get_required_ecc(struct nand_probe_info *info,
		struct onfi_params *params, int *ecc_level, int *sector_size)
{
	struct onfi_extended_params *ext_params;
	struct onfi_ecc_block *ecc_block;
	int page_len, section_len, section_offset;
	int ret = -1;

	if (params->ecc_bits != 0xff) {
		*ecc_level = params->ecc_bits;
		*sector_size = 512;
		return 0;
	}

	/* Else, look for extended ECC parameter */

	/* Extended parameter page len is multiple of 16 */
	page_len = le16_to_cpu(params->ext_param_page_len) * 16;
	ext_params = KMALLOC(page_len, 4);
	if (!ext_params)
		return -1;

	/* Read extended parameter page */
	onfi_parameter_read(info, (uint32_t *)ext_params,
			params->num_param_pages * sizeof(*params),
			page_len);
	if (strncmp(ext_params->epps, "EPPS", 4)) {
		err_msg("CS%d: NAND: ECC parameter page not found", info->cs);
		goto out;
	}

	/* Compute offset of ECC information block */
	section_offset = onfi_ext_params_section_offset(ext_params,
			ONFI_SECTION_ECC, &section_len);
	if (!section_len)
		goto out;

	ecc_block = (struct onfi_ecc_block *)&ext_params->sections[section_offset];
	*ecc_level = ecc_block->ecc_bits;
	*sector_size = 1 << ecc_block->codeword_size_shift;

	ret = 0;
out:
	KFREE(ext_params);
	return ret;
}

static void nand_format_ecc_string(char *str, int ecc_level, int sector_size_shift)
{
	str += xsprintf(str, "BCH-%u",
			ecc_level << sector_size_shift);
	if (sector_size_shift)
		xsprintf(str, " (%dKB sector)", 1 << (sector_size_shift - 1));
}

static void nand_print_config(struct nand_dev *nand)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	char ecc_str[40];

	nand_format_ecc_string(ecc_str, info->ecc_level, info->sector_size_1k);
	xprintf("CS%d: %sNAND, %lluMB, %lukB blocks, %luB page, %dB OOB, %s\n",
			info->cs,
			info->chip_idx == NAND_CHIP_ONFI ? "ONFI " : "",
			flash->size >> 20,
			(unsigned long)flash->blocksize >> 10,
			(unsigned long)flash->writesize,
			info->oob_sector,
			ecc_str);
}

#if (BCHP_NAND_REVISION_MAJOR_DEFAULT < 7) || \
    (BCHP_NAND_REVISION_MAJOR_DEFAULT >= 7 && BCHP_NAND_REVISION_MINOR_DEFAULT < 1)
/* NAND controller 7.0 or lower */
static void nand_set_block_size_index(int cs, unsigned int i)
{
	const uint32_t reg = BCHP_NAND_CONFIG_CS0 +
		cs * (BCHP_NAND_CONFIG_CS1 - BCHP_NAND_CONFIG_CS0);
	uint32_t val;

	val = BDEV_RD(reg);
	val &= ~BCHP_NAND_CONFIG_CS0_BLOCK_SIZE_MASK;
	val |= i << BCHP_NAND_CONFIG_CS0_BLOCK_SIZE_SHIFT;
	BDEV_WR(reg, val);
}

static void nand_set_page_size_index(int cs, unsigned int i)
{
	const uint32_t reg = BCHP_NAND_CONFIG_CS0 +
		cs * (BCHP_NAND_CONFIG_CS1 - BCHP_NAND_CONFIG_CS0);
	uint32_t val;

	val = BDEV_RD(reg);
	val &= ~BCHP_NAND_CONFIG_CS0_PAGE_SIZE_MASK;
	val |= i << BCHP_NAND_CONFIG_CS0_PAGE_SIZE_SHIFT;
	BDEV_WR(reg, val);
}

#else
/* NAND controller 7.1 or higher */
static void nand_set_block_size_index(int cs, unsigned int i)
{
	const uint32_t reg = BCHP_NAND_CONFIG_EXT_CS0 +
		cs * (BCHP_NAND_CONFIG_EXT_CS1 - BCHP_NAND_CONFIG_EXT_CS0);
	uint32_t val;

	val = BDEV_RD(reg);
	val &= ~BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_MASK;
	val |= i << BCHP_NAND_CONFIG_EXT_CS0_BLOCK_SIZE_SHIFT;
	BDEV_WR(reg, val);
}

static void nand_set_page_size_index(int cs, unsigned int i)
{
	const uint32_t reg = BCHP_NAND_CONFIG_EXT_CS0 +
		cs * (BCHP_NAND_CONFIG_EXT_CS1 - BCHP_NAND_CONFIG_EXT_CS0);
	uint32_t val;

	val = BDEV_RD(reg);
	val &= ~BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_MASK;
	val |= i << BCHP_NAND_CONFIG_EXT_CS0_PAGE_SIZE_SHIFT;
	BDEV_WR(reg, val);
}
#endif
static void nand_set_device_size_index(int cs, unsigned int i)
{
	const uint32_t reg = BCHP_NAND_CONFIG_CS0 +
		cs * (BCHP_NAND_CONFIG_CS1 - BCHP_NAND_CONFIG_CS0);
	uint32_t val;

	val = BDEV_RD(reg);
	val &= ~BCHP_NAND_CONFIG_CS0_DEVICE_SIZE_MASK;
	val |= i << BCHP_NAND_CONFIG_CS0_DEVICE_SIZE_SHIFT;
	BDEV_WR(reg, val);
}

static void nand_set_write_protect(int enable)
{
#ifdef BCHP_NAND_CS_NAND_SELECT_NAND_WP_MASK
	BDEV_WR_F(NAND_CS_NAND_SELECT, NAND_WP, enable);
#endif
}

/*
 * enable/disable ECC
 */
static inline void nand_enable_read_ecc(struct nand_dev *nand, bool on)
{
	struct nand_probe_info *info = &nand->info;

	if (on) {
		WR_ACC_CONTROL(info->cs, RD_ECC_EN, 1);
		WR_ACC_CONTROL(info->cs, ECC_LEVEL, info->ecc_level);
	} else {
		WR_ACC_CONTROL(info->cs, RD_ECC_EN, 0);
		WR_ACC_CONTROL(info->cs, ECC_LEVEL, 0);
	}
}

/*
 * nand_check_read_status:
 * returns 0 if no error detected
 * returns 1 if correctable error detected
 * returns -1 if uncorrectable error detected
 */
static int nand_check_read_status(void)
{
	int ret = 0;

	/* check the interrupt status */
	if (BDEV_RD_F(HIF_INTR2_CPU_STATUS, NAND_CORR_INTR)) {
		BDEV_WR_F(HIF_INTR2_CPU_CLEAR, NAND_CORR_INTR, 1);
		ret = NAND_ERR_CORR;
	}

	if (BDEV_RD_F(HIF_INTR2_CPU_STATUS, NAND_UNC_INTR)) {
		BDEV_WR_F(HIF_INTR2_CPU_CLEAR, NAND_UNC_INTR, 1);
		ret = NAND_ERR_UNCORR;
	}

	return ret;
}

static int nand_create_block_status_array(struct flash_partition *part)
{
	uint32_t i;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;
	uint32_t num_blocks = flash->size / flash->blocksize;

	/* find the bad blocks */
	for (i = 0; i < num_blocks; i++) {
		uint64_t addr = (uint64_t)i * flash->blocksize;
		if (nand_get_block_status(softc, addr)) {
			softc->blk_status[i] = NAND_BLOCK_STATUS_BAD;
			xprintf("NAND: block %d is bad at address %#llx\n",
					i, addr);
		} else {
			softc->blk_status[i] = NAND_BLOCK_STATUS_GOOD;
		}
	}

	/* Clear past ECC errors */
	BDEV_WR_F(HIF_INTR2_CPU_CLEAR, NAND_UNC_INTR, 1);
	BDEV_WR_F(HIF_INTR2_CPU_CLEAR, NAND_CORR_INTR, 1);

	return 0;
}

static int nand_get_bad_blk_cnt(struct flash_partition *part, uint32_t index1,
				uint32_t index2, uint32_t num_blocks_in_dev)
{
	struct nand_dev *softc = flashpart_get_drvdata(part);
	uint32_t i;
	int bad_blk_cnt = 0;

	if (index1 > (num_blocks_in_dev - 1)
	    || (index2 > num_blocks_in_dev - 1)) {
		return -1;
	}

	for (i = index1; i <= index2; i++)
		if (softc->blk_status[i] == NAND_BLOCK_STATUS_BAD)
			bad_blk_cnt++;

	return bad_blk_cnt;
}

static uint64_t nand_get_good_block(struct flash_partition *part, uint64_t address)
{
	uint32_t num_blocks_in_dev;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;
	uint64_t part_base_address = part->offset;
	uint32_t total_bad_blk_num = 0;
	int bad_blk_num = 0;
	uint32_t index1, index2;

	num_blocks_in_dev = flash->size / flash->blocksize;

	index1 = part_base_address / flash->blocksize;
	index2 = address / flash->blocksize;

	while (1) {
		bad_blk_num = nand_get_bad_blk_cnt(part, index1, index2,
					 num_blocks_in_dev);

		if (bad_blk_num == 0) {
			break;
		} else if (bad_blk_num > 0) {
			total_bad_blk_num += bad_blk_num;
			index1 = index2 + 1;
			index2 = index2 + bad_blk_num;
		} else {
			/* we are out of blocks in partition */
			return -1;
		}
	}

	return (address + total_bad_blk_num * flash->blocksize);
}

/* Return non-zero for a bad address; otherwise 0 */
static int check_block_address(struct flash_partition *part, uint64_t address)
{
	if (address == (uint64_t)-1)
		return 1;
	return address > part->offset + part->size;
}

/******************************************************************************
 *
 * Function Name: do_nand_cmd
 * Description: Returns the flash status; this byte value is updated with a
 *     status read command from the Flash device after every Program Page,
 *     Block Erase, or Copy Back command operation.
 *     Reset value is 0x0
 * Arguments: address should be block aligned
 * Returns: the flash status: This byte value is updated with a status read
 *     command from the Flash device
 *
 *****************************************************************************/
static uint32_t do_nand_cmd(struct nand_probe_info *info, uint32_t cmd,
		uint64_t addr)
{
	int t = 100 * 1000; /* 100ms */

	DBG("NAND cmd: %#04x @ %llx\n", cmd, addr);

#ifdef BCHP_NAND_CMD_ADDRESS_CS_SEL_SHIFT
	/* 64 bit register */
	addr &= BCHP_NAND_CMD_ADDRESS_ADDRESS_MASK;
	addr |= ((uint64_t)info->cs) << BCHP_NAND_CMD_ADDRESS_CS_SEL_SHIFT;
	BDEV_WR64(BCHP_NAND_CMD_ADDRESS, addr);
#else
	BDEV_WR(BCHP_NAND_CMD_EXT_ADDRESS,
			(info->cs << 16) | ((addr >> 32) & 0xffff));
	BDEV_WR(BCHP_NAND_CMD_ADDRESS, addr & 0xffffffff);
#endif
	BDEV_WR_F(NAND_CMD_START, OPCODE, cmd);

	while (!BDEV_RD_F(NAND_INTFC_STATUS, CTLR_READY)) {
		if (t <= 0)
			break;
		bolt_usleep(1);
		t -= 1;
	}
	if (!BDEV_RD_F(NAND_INTFC_STATUS, CTLR_READY))
		err_msg("NAND: timeout waiting for command (%#04x @ %llx)",
			cmd, addr);

	return BDEV_RD(BCHP_NAND_INTFC_STATUS) & NAND_STATUS_FAIL;
}

/******************************************************************************
 *
 * Function Name: do_nand_low_level_op
 * Description: function perfroms low level ops for doing vendor specific
 *              commands
 * Arguments: low level op code, data to write
 * Returns:
 *
 *****************************************************************************/
static void do_nand_low_level_op(struct nand_dev *nand, enum llop_type type,
				 uint32_t data, bool last_op)

{
	struct nand_probe_info *info = &nand->info;
	uint32_t tmp;

	tmp = data & LLOP_DATA_MASK;
	switch (type) {
	case LL_OP_CMD:
		tmp |= LLOP_WE | LLOP_CLE;
		break;
	case LL_OP_ADDR:
		/* WE | ALE */
		tmp |= LLOP_WE | LLOP_ALE;
		break;
	case LL_OP_WR:
		/* WE */
		tmp |= LLOP_WE;
		break;
	case LL_OP_RD:
		/* RE */
		tmp |= LLOP_RE;
		break;
	}

	if (last_op)
		tmp |= LLOP_RETURN_IDLE;

	BDEV_WR(BCHP_NAND_LL_OP, tmp);
	tmp = BDEV_RD(BCHP_NAND_LL_OP);
	DBG("NAND_LL_OP %x\n", tmp);

	do_nand_cmd(info, NAND_CMD_LOW_LEVEL_OP, 0);
}

/******************************************************************************
 *
 * Function Name: nand_set_get_feature
 * Description: function set or gets vendor specific features
 * Arguments: Feature command, sub feature param data to write
 * Returns : buffer with data as part of the buf argument on reads
 *
 *****************************************************************************/
static void nand_set_get_feature(struct nand_dev *nand,
				 enum nand_feature_cmd cmd, uint32_t addr,
				 uint8_t *buf, int len)
{
	int i;

	if (!buf || !len)
		return;

	do_nand_low_level_op(nand, LL_OP_CMD, cmd, false);
	do_nand_low_level_op(nand, LL_OP_ADDR, addr, false);

	switch (cmd) {
	case NAND_SET_FEATURE_CMD:
		for (i = 0; i < len; i++)
			do_nand_low_level_op(nand, LL_OP_WR, buf[i],
					     (i + 1) == len);
		break;

	case NAND_GET_FEATURE_CMD:
		for (i = 0; i < len; i++) {
			do_nand_low_level_op(nand, LL_OP_RD, 0, (i + 1) == len);
			buf[i] = BDEV_RD(BCHP_NAND_LL_RDDATA) & 0xff;
		}

		break;
	}
}

static int nand_get_block_status(struct nand_dev *nand, uint64_t blk_addr)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	uint64_t page_addr;
	uint32_t bbi_page_map;
	uint32_t bbi_byte_map;
	int rval;
	uint32_t i;

	/* align the address to blk boundary */
	blk_addr &= ~((uint64_t)flash->blocksize - 1);

	/* set page_addr to first page in block */
	page_addr = blk_addr;

	/* get BBI map for pages offset from beginning of block (lower 16 bits of NAND_BAD_BLOCK_INDICATOR_MAP) */
	bbi_page_map = (info->bbi_map >> 8) & 0xFF;
	bbi_byte_map = info->bbi_map & 0xFF;

	/* if bad block indicator is within the first 8 pages of the block, check them */
	if (bbi_page_map) {
		for (i = 0; i < 8; i++) {
			/* start checking from the first page to the 8th page */
			if (bbi_page_map & 0x1) {
				rval =
				    nand_get_page_status(info, page_addr +
							 (i * flash->writesize),
							 bbi_byte_map);
				if (rval == NAND_BLOCK_STATUS_BAD)
					return NAND_BLOCK_STATUS_BAD;
			}
			bbi_page_map >>= 1;
		}
	}

	/* set page_addr to last page in block */
	page_addr = flash->blocksize + flash->blocksize - flash->writesize;

	/* get BBI map for pages offset from end of block (upper 16 bits of NAND_BAD_BLOCK_INDICATOR_MAP) */
	bbi_page_map = (info->bbi_map >> 24) & 0xFF;
	bbi_byte_map = (info->bbi_map >> 16) & 0xFF;

	/* if bad block indicator is within the last 8 pages of the block, check them  */
	if (bbi_page_map) {
		/* start checking from (last page) to (8th to the last page) */
		for (i = 0; i < 8; i++) {
			if (bbi_page_map & 0x1) {
				rval =
				    nand_get_page_status(info, page_addr -
							 (i * flash->writesize),
							 bbi_byte_map);
				if (rval == NAND_BLOCK_STATUS_BAD)
					return NAND_BLOCK_STATUS_BAD;
			}
			bbi_page_map >>= 1;
		}
	}

	return NAND_BLOCK_STATUS_GOOD;
}

/*
 * nand_get_page_status: checks the status of the present blk, if it is good returns the same address,
 * else returns the address of the next good block
 */
static int nand_get_page_status(struct nand_probe_info *info,
		uint64_t page_addr, uint32_t bbi_byte_map)
{
	uint32_t indicator_byte;
	uint32_t byte_position_mask;
	uint32_t ofs_data;
	int i;
	bool prefetch;
	int ret = NAND_BLOCK_STATUS_GOOD;

	/* check page */
	if (!bbi_byte_map)
		return ret;

	/*
	 * Disable prefetch for bad block scan, since we don't do full-page
	 * reads. See JIRA CRNAND-58.
	 */
	prefetch = RD_ACC_CONTROL(info->cs, PREFETCH_EN);
	if (prefetch)
		WR_ACC_CONTROL(info->cs, PREFETCH_EN, 0);

	/* read the page */
	do_nand_cmd(info, NAND_CMD_PAGE_READ, page_addr);
	nand_check_read_status();

	/* read the first 4 OFS bytes */
	ofs_data = REG(BCHP_NAND_SPARE_AREA_READ_OFS_0);

	/* set mask for OFS3 */
	byte_position_mask = 0x8;

	/* check OFS bytes 3, 2, 1, & 0 (in that order) */
	for (i = 0; i < 4; i++) {
		if (bbi_byte_map & byte_position_mask) {
			indicator_byte = ofs_data & 0xFF;

			if (indicator_byte != 0xFF) {
				ret = NAND_BLOCK_STATUS_BAD;
				goto out;
			}
		}
		byte_position_mask >>= 1;
		ofs_data >>= 8;
	}

	/* read the next 4 OFS bytes */
	ofs_data = REG(BCHP_NAND_SPARE_AREA_READ_OFS_4);

	/* set mask for OFS7 */
	byte_position_mask = 0x80;

	/* check OFS bytes 3, 2, 1, & 0 (in that order) */
	for (i = 0; i < 4; i++) {
		if (bbi_byte_map & byte_position_mask) {
			indicator_byte = ofs_data & 0xFF;

			if (indicator_byte != 0xFF) {
				ret = NAND_BLOCK_STATUS_BAD;
				goto out;
			}
		}
		byte_position_mask = byte_position_mask >> 1;
		ofs_data = ofs_data >> 8;
	}

out:
	/* Re-enable prefetch */
	if (prefetch)
		WR_ACC_CONTROL(info->cs, PREFETCH_EN, 1);

	return ret;
}

static void nand_mark_block_bad(struct nand_dev *nand, uint64_t blk_addr)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	uint64_t page_addr;
	uint32_t bbi_page_map;
	uint32_t i;
	uint32_t blk_idx;

	/* align the address to blk boundary */
	blk_addr &= ~(flash->blocksize - 1);

	/* set page_addr to first page in block */
	page_addr = blk_addr;

	/* calculate block number */
	blk_idx = blk_addr / flash->blocksize;

	/* get BBI map for pages offset from beginning of block (lower 16 bits of NAND_BAD_BLOCK_INDICATOR_MAP) */
	bbi_page_map = (info->bbi_map >> 8) & 0xFF;

	/* if bad block indicator is within the first 8 pages of the block, check them */
	if (bbi_page_map) {
		for (i = 0; i < 8; i++) {
			/* start checking from the first page to the 8th page */
			if (bbi_page_map & 0x1) {
				nand_mark_page_bad(nand, page_addr +
						(i * flash->writesize));
			}
			bbi_page_map = bbi_page_map >> 1;
		}
	}

	/* set page_addr to last page in block */
	page_addr = blk_addr + flash->blocksize - flash->writesize;

	/* get BBI map for pages offset from end of block (upper 16 bits of NAND_BAD_BLOCK_INDICATOR_MAP) */
	bbi_page_map = (info->bbi_map >> 24) & 0xFF;

	/* if bad block indicator is within the last 8 pages of the block, check them  */
	if (bbi_page_map) {
		/* start checking from (last page) to (8th to the last page) */
		for (i = 0; i < 8; i++) {
			if (bbi_page_map & 0x1) {
				nand_mark_page_bad(nand, page_addr -
						(i * flash->writesize));
			}
			bbi_page_map = bbi_page_map >> 1;
		}
	}

	/* Update block status array */
	nand->blk_status[blk_idx] = NAND_BLOCK_STATUS_BAD;
}

static void nand_mark_page_bad(struct nand_dev *nand, uint64_t page_addr)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	unsigned int i;

	/* Mark all bad block indicators */
	BDEV_WR(BCHP_NAND_SPARE_AREA_WRITE_OFS_0, 0);
	BDEV_WR(BCHP_NAND_SPARE_AREA_WRITE_OFS_4, 0);
	for (i = 8; i < NAND_MAX_SPARE_AREA; i += 4)
		BDEV_WR(BCHP_NAND_SPARE_AREA_WRITE_OFS_0 + i, 0xffffffff);

	/* Write the bad block mark to every 512 byte sector. */
	/* The block is bad so we don't care if the data or   */
	/* ECC bytes get corrupted.                           */
	for (i = 0; i < flash->writesize; i += NAND_FC_SIZE)
		do_nand_cmd(info, NAND_CMD_PROGRAM_PAGE, page_addr + i);
}

/* nand_write_page:
 * addr: should be page aligned
 * returns 0 if write successful else 1
 */
static int nand_write_page(struct nand_dev *nand,
		uint64_t addr, uint32_t *buf)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	unsigned int i, j;
	int res;

	/* Fill spare area with 0xff */
	for (i = 0; i < NAND_MAX_SPARE_AREA; i += 4)
		BDEV_WR(BCHP_NAND_SPARE_AREA_WRITE_OFS_0 + i, 0xffffffff);

	for (i = 0; i < flash->writesize; i += NAND_FC_SIZE) {
		/*
		 * need to update subpage address in NAND_CMD_ADDRESS
		 * register before filling flash cache, even though
		 * we call do_nand_cmd() where we do the same for each
		 * subpage
		 */
#ifdef BCHP_NAND_CMD_ADDRESS_CS_SEL_SHIFT
		/* 64 bit register */
		addr &= BCHP_NAND_CMD_ADDRESS_ADDRESS_MASK;
		addr |= ((uint64_t)info->cs) <<
			BCHP_NAND_CMD_ADDRESS_CS_SEL_SHIFT;
		BDEV_WR64(BCHP_NAND_CMD_ADDRESS, addr + i);
#else
		BDEV_WR(BCHP_NAND_CMD_ADDRESS, addr + i);
#endif
		for (j = 0; j < NAND_FC_SIZE / 4; j++, buf++)
			BDEV_WR(NAND_FC(j), *buf);

		res = do_nand_cmd(info, NAND_CMD_PROGRAM_PAGE, addr + i);

		if (res)
			return res;
	}

	return 0;
}

static int nand_handle_partition_read_disturb(bolt_devctx_t * ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;
	int rval;
	uint64_t offs;
	uint8_t *read_buffer;
	iocb_buffer_t buffer;

	rval = nand_check_read_status();

	/* correctable error occured */
	if (rval == NAND_ERR_CORR) {
		/* check bolt partition */
		read_buffer = KMALLOC(flash->writesize, 4);
		if (read_buffer) {
			buffer.buf_length = flash->writesize;
			buffer.buf_offset = 0;
			buffer.buf_ptr = (bolt_ptr_t) read_buffer;

			for (offs = 0; offs < part->size; offs += flash->writesize)
				nanddrv_read(ctx, &buffer);
			KFREE(read_buffer);
		} else {
			err_msg("malloc failed: cannot handle read disturbance");
		}
	}

	return rval;
}

/**
 * Read (up to) one NAND flash sector (NAND flash cache)
 *
 * @info: NAND info
 * @address: flash address to read from
 * @data: read data into this buffer
 */
static int32_t nand_read_sector(struct nand_probe_info *info, uint64_t address,
				uint32_t *data)
{
	unsigned int i;

	do_nand_cmd(info, NAND_CMD_PAGE_READ, address);

	for (i = 0; i < NAND_FC_SIZE >> 2; i++)
		data[i] = GET_NAND_FC_WORD(i);

	return nand_check_read_status();
}

/* Read an entire page */
static int nand_read_page(struct nand_dev *nand, uint64_t addr, void *buf)
{
	struct nand_probe_info *info = &nand->info;
	struct flash_dev *flash = &nand->flash;
	unsigned int i;
	int ret = 0, res;

	for (i = 0; i < flash->writesize; i += NAND_FC_SIZE) {
		res = nand_read_sector(info, addr + i, buf + i);
		/* Report strongest error */
		if (res && ret != NAND_ERR_UNCORR)
			ret = res;
	}

	return ret;
}

/*
 * Verify that a page is actually erased, or if is really uncorrectable. See
 * CRNAND-57.
 */
static int nand_verify_erased_page(struct nand_dev *nand, uint64_t addr,
				   void *buf)
{
	struct nand_probe_info *info = &nand->info;
	struct flash_dev *flash = &nand->flash;
	unsigned int i, j;
	int ret = 0;
	int max_bitflips = 0, bitflips = 0;
	unsigned int sector_size = flash->ecc_step;

#if NAND_CONTROLLER_REVISION >= 0x702
	/*
	 * read erased page is protected from ecc error generated
	 * skip software check.
	 */
	return NAND_ERR_UNCORR;
#endif
	nand_enable_read_ecc(nand, false);
	for (i = 0; i < flash->writesize; i += NAND_FC_SIZE) {
		nand_read_sector(info, addr + i, buf + i);
		for (j = 0; j < NAND_MAX_SPARE_AREA; j += 4)
			bitflips += 32 - bcnt(BDEV_RD(BCHP_NAND_SPARE_AREA_READ_OFS_0 + j));

		for (j = 0; j < NAND_FC_SIZE; j += 4)
			bitflips += 32 - bcnt(((uint32_t *)buf)[(i + j) / 4]);

		/* 1K sector we need to read 2 times NAND_FC_SIZE */
		if (((sector_size - 1) & (addr + i)) != 0)
			continue;

		DBG("NAND: %d bitflips @ sector 0x%llx\n", bitflips, addr + i);
		if (bitflips > flash->ecc_strength) {
			ret = NAND_ERR_UNCORR;
			break;
		} else {
			max_bitflips = max(max_bitflips, bitflips);
			bitflips = 0;
		}
	}
	nand_enable_read_ecc(nand, true);

	if (ret != NAND_ERR_UNCORR) {
		/* fill return buffer with all ffs */
		memset(buf, 0xff, flash->writesize);
	}

	DBG("NAND: verified %s page @ %llx with %d bitflips\n",
	    ret == 0 ? "erased" : "corrupt", addr, max_bitflips);

	return ret;
}

/**
 * Flash DMA helpers
 */

/*
 * Prepare a list of DMA transactions for reading page(s)
 *
 * Return: transaction information (by reference) and number of descriptors
 * (return value); caller should free *trans when done
 *
 * On error, return zero or negative
 */
static int nand_prepare_dma_read_trans(struct nand_dev *nand, dma_addr_t buf,
				       size_t len, uint64_t addr,
				       struct dma_transaction **trans)
{
	struct nand_probe_info *info = &nand->info;
	struct flash_dev *flash = &nand->flash;
	struct dma_transaction *descs;
	int npages, ndescs, i, j;
	bool toggle_cache;

	if (!IS_ALIGNED(addr, flash->writesize) ||
	    !IS_ALIGNED(len, flash->writesize))
		return NAND_ERR;

	npages = len / flash->writesize;

	if (npages > 1 && info->supports_cache_read) {
		toggle_cache = true;
		ndescs = npages + 2;
	} else {
		toggle_cache = false;
		ndescs = npages;
	}

	descs = KMALLOC(ndescs * sizeof(*descs), 0);
	if (!descs)
		return NAND_ERR;

	memset(descs, 0, ndescs * sizeof(*descs));

	i = 0;

	/* Enable cache mode */
	if (toggle_cache) {
		/* Read register */
		descs[i].reg_op[0].reg_addr = BVIRTADDR(REG_ACC_CONTROL(info->cs));
		descs[i].reg_op[0].reg_cmd = DMA_REG_READ;

		/* Modify/write register */
		descs[i].reg_op[1].reg_addr = BVIRTADDR(REG_ACC_CONTROL(info->cs));
		descs[i].reg_op[1].reg_cmd = DMA_REG_WR_F_OR;
		descs[i].reg_op[1].reg_data = BCHP_NAND_ACC_CONTROL_CS0_CACHE_MODE_EN_MASK;

		/* Remaining 2 ops are no-op reads */

		descs[i].is_register = true;
		i++;
	}

	/* N-1 pages */
	for (j = 0; j < npages - 1; j++) {
		descs[i].addr = buf;
		descs[i].len = flash->writesize;
		descs[i].cmd = NAND_CMD_PAGE_READ;
		i++;
		buf += flash->writesize;
	}

	/* Disable cache mode */
	if (toggle_cache) {
		/* Read register */
		descs[i].reg_op[0].reg_addr = BVIRTADDR(REG_ACC_CONTROL(info->cs));
		descs[i].reg_op[0].reg_cmd = DMA_REG_READ;

		/* Modify/write register */
		descs[i].reg_op[1].reg_addr = BVIRTADDR(REG_ACC_CONTROL(info->cs));
		descs[i].reg_op[1].reg_data = ~BCHP_NAND_ACC_CONTROL_CS0_CACHE_MODE_EN_MASK;
		descs[i].reg_op[1].reg_cmd = DMA_REG_WR_F_AND;

		/* Remaining 2 ops are no-op reads */

		descs[i].is_register = true;
		i++;
	}

	/* Nth page */
	descs[i].addr = buf;
	descs[i].len = flash->writesize;
	descs[i].cmd = NAND_CMD_PAGE_READ;
	i++;
	buf += flash->writesize;

	*trans = descs;
	return i; /* i == ndescs */
}

/* Convert one DMA transaction to a Flash DMA descriptor */
static void nand_fill_dma_desc(struct nand_dev *nand,
			       struct flash_dma_desc *desc, uint64_t addr,
			       struct dma_transaction *reg, bool last)
{
	struct nand_probe_info *info = &nand->info;
	uint32_t cmd_irq = 0;
	dma_addr_t next;
	int i;

	if (last) {
		cmd_irq |= 0x03 << 8; /* IRQ | STOP */
		next = 0;
	} else {
		next = (dma_addr_t)(uintptr_t)desc + sizeof(*desc);
	}

	memset(desc, 0, sizeof(*desc));
	/* Descriptors are written in native byte order (wordwise) */
	desc->next_desc = lower_32_bits(next);
	desc->next_desc_ext = upper_32_bits(next);
	if (!reg->is_register) {
#if defined(__BIG_ENDIAN)
		cmd_irq |= 0x01 << 12;
#endif
		cmd_irq |= reg->cmd << 24;
		cmd_irq |= 0x03; /* head | tail */

		desc->cmd_irq = cmd_irq;
		desc->dram_addr = lower_32_bits(reg->addr);
		desc->dram_addr_ext = upper_32_bits(reg->addr);
		desc->tfr_len = reg->len;
		desc->total_len = reg->len;
		desc->flash_addr = lower_32_bits(addr);
		desc->flash_addr_ext = upper_32_bits(addr);
		desc->cs = info->cs;

		addr += reg->len;
	} else {
		cmd_irq |= 0x04; /* register descriptor */

		desc->cmd_irq = cmd_irq;
		for (i = 0; i < (int)ARRAY_SIZE(desc->regs); i++) {
			desc->regs[i].reg_addr = reg->reg_op[i].reg_addr;
			desc->regs[i].reg_cmd = reg->reg_op[i].reg_cmd << 28;
			desc->regs[i].reg_data = reg->reg_op[i].reg_data;
		}
	}
	desc->status_valid = 0x01;
}

/*
 * Convert an array of transactions into an array of Flash DMA descriptors
 *
 * @nand: NAND device
 * @descs: array of DMA descriptors
 * @ndescs: number of elements in @descs and @regs
 * @addr: flash address
 * @regs: array of DMA transaction descriptions
 */
static void nand_fill_dma_descs(struct nand_dev *nand,
				struct flash_dma_desc *descs, int ndescs,
				uint64_t addr, struct dma_transaction *regs)
{
	int i;

	for (i = 0; i < ndescs; i++) {
		struct flash_dma_desc *desc = &descs[i];
		struct dma_transaction *reg = &regs[i];
		bool last = i == ndescs - 1;

		nand_fill_dma_desc(nand, desc, addr, reg, last);
		if (!reg->is_register)
			addr += reg->len;

		DBG("Descriptor %d:\n", i);
		DBG_HEXDUMP(desc, sizeof(*desc));
	}
}

static int nand_dma_run(struct nand_dev *nand, dma_addr_t desc)
{
	int us = 1000 * 1000, delay = 50;
	int ret = 0;

	flash_dma_irq_clear();

#if NAND_CONTROLLER_REVISION >= 0x703
	flash_dma_write64(nand, FLASH_DMA_FIRST_DESC, desc);
	(void) flash_dma_read64(nand, FLASH_DMA_FIRST_DESC);
#else
	flash_dma_writel(nand, FLASH_DMA_FIRST_DESC, lower_32_bits(desc));
	flash_dma_writel(nand, FLASH_DMA_FIRST_DESC_EXT, upper_32_bits(desc));
#endif

	/* Start FLASH_DMA engine */
	BARRIER();
	flash_dma_writel(nand, FLASH_DMA_CTRL, 0x03); /* wake | run */

	while (!flash_dma_irq_done()) {
		if (us <= 0)
			break;

		bolt_usleep(delay);
		us -= delay;
	}
	if (!flash_dma_irq_done()) {
		err_msg("NAND: timeout waiting for DMA; status %#x, error status %#x",
				flash_dma_readl(nand, FLASH_DMA_STATUS),
				flash_dma_readl(nand, FLASH_DMA_ERROR_STATUS));
		flash_dma_writel(nand, FLASH_DMA_ERROR_STATUS, 0);
		ret = NAND_ERR_TIMEOUT;
	}
	flash_dma_irq_clear();
	flash_dma_writel(nand, FLASH_DMA_CTRL, 0); /* force stop */

	return ret;
}

static int nand_dma_read(struct nand_dev *nand, uint64_t addr, void *buf,
			 size_t len)
{
	struct flash_dma_desc *descs;
	struct dma_transaction *trans;
	int i, ret, ndescs;
	bool retry = true;

	DBG("DMA read: len %x @ %llx\n", len, addr);

try_dmaread:
	ndescs = nand_prepare_dma_read_trans(nand, (uintptr_t)buf, len, addr, &trans);
	if (ndescs <= 0) {
		/* coverity[leaked_storage] */
		return NAND_ERR;
	}

	descs = KMALLOC(ndescs * sizeof(*descs), 32);
	if (!descs) {
		ret = NAND_ERR;
		goto out;
	}

	nand_fill_dma_descs(nand, descs, ndescs, addr, trans);

	CACHE_FLUSH_RANGE(buf, len);
	CACHE_FLUSH_RANGE(descs, ndescs * sizeof(*descs));

	ret = nand_dma_run(nand, (uintptr_t)descs);

	CACHE_INVAL_RANGE(buf, len);
	CACHE_INVAL_RANGE(descs, ndescs * sizeof(*descs));

	for (i = 0; i < ndescs; i++) {
		struct flash_dma_desc *desc = &descs[i];

		if (desc->status_valid & FLASH_DMA_ECC_ERROR) {
			uint32_t offs = desc->flash_addr - lower_32_bits(addr);

			/*
			 * CRNAND-32 : if we are doing a DMA read after a prior
			 * PIO read that reported uncorrectable error, the DMA
			 * engine captures this error in the first DMA read
			 * after this PIO read, cleared only on subsequent DMA
			 * read, so just retry once to clear a possible false
			 * error reported for current DMA read
			 */
			if (retry) {
				KFREE(trans);
				KFREE(descs);
				retry = false;
				goto try_dmaread;
			}

			/* Re-check all uncorrectable errors, CRNAND-57 */
			ret = nand_verify_erased_page(nand, addr + offs, buf + offs);
		} else if (!ret && desc->status_valid & FLASH_DMA_CORR_ERROR) {
			ret = NAND_ERR_CORR;
		}
	}

out:
	KFREE(trans);
	KFREE(descs);
	return ret;
}


/**
 * nand_block_read
 *
 * @nand: NAND device structure
 * @address: flash address to read from
 * @len: length of data to read
 * @data: buffer in which data should be stored
 */
static int nand_block_read(struct nand_dev *nand, uint64_t address,
				size_t len, void *data)
{
	struct nand_probe_info *info = &nand->info;
	struct flash_dev *flash = &nand->flash;
	unsigned int i;
	int ret = 0;
	range_descriptor_t pdt;
	uint64_t page_address;
	uint32_t copy_len;
	uint32_t offset;
	uint32_t step = flash->writesize;
	bool toggle_cache_mode = false;

	nand_get_range_intersection(address, len, step, &pdt);

	page_address = pdt.new_range_base;

	if (has_flash_dma(nand) && flash_dma_buf_ok(data) &&
			IS_ALIGNED(len, flash->writesize) &&
			IS_ALIGNED(address, flash->writesize)) {
		ret = nand_dma_read(nand, address, data, len);
		return ret;
	}

	if (pdt.num_block_access > 1 && info->supports_cache_read) {
		/* More than 1 sequential page; let's use cache mode */
		toggle_cache_mode = true;
		WR_ACC_CONTROL(info->cs, CACHE_MODE_EN, 1);
	}

	for (i = 0; i < pdt.num_block_access; i++) {
		bool bounce = false;
		void *buf;
		int res;

		/* Disable cache mode for last page */
		if ((i == pdt.num_block_access - 1) && toggle_cache_mode)
			WR_ACC_CONTROL(info->cs, CACHE_MODE_EN, 0);

		/* copy data from internal page buffer to data buffer */

		if (i == 0 && pdt.first_block_data_offset > 0) {
			offset = pdt.first_block_data_offset;
			copy_len = min(len, step - offset);
			bounce = true;
		} else if ((i == pdt.num_block_access - 1)
			   && (pdt.last_block_data_offset != 0)) {
			offset = 0;
			copy_len = pdt.last_block_data_offset;
			bounce = true;
		} else {
			offset = 0;
			copy_len = step;
		}

		if (bounce)
			buf = nand->page_buf;
		else
			buf = data;

		if (has_flash_dma(nand) && flash_dma_buf_ok(buf)) {
			res = nand_dma_read(nand, page_address, buf, step);
		} else {
			res = nand_read_page(nand, page_address, buf);

			/* Double-check ECC error reports for all-0xff */
			if (res == NAND_ERR_UNCORR)
				res = nand_verify_erased_page(nand, page_address, buf);
		}

		if (bounce)
			memcpy(data, buf + offset, copy_len);

		/* Keep the strongest error code */
		if (res > 0)
			ret = NAND_ERR_CORR;
		else
			ret = res;

		data += copy_len;

		page_address += step;
	}

	return ret;
}

/*
 * nand_block_write:
 * This will write the data from the data buffer to the block specified by the
 * address. The address always need to be block aligned.
 * There can be four types of block writes:
 *		- Program the whole block
 *		- Program the bottom part of the block with the new data in the
 *		  buffer, but preserve the old data in the top part.
 *		- Program the top part of the block but preserve the bottom
 *		  part.
 *		- Program only a portion of a block.
 *
 * @nand: NAND device structure
 * @address: block-aligned address
 * @offset: offset within the block
 * @len: length of data to program
 * @data: data to write
 * @read_buffer: buffer to hold intermediate read data; must fit 1 eraseblock
 *     of data
 */
static int nand_block_write(struct nand_dev *nand,
			  uint64_t address,
			  uint32_t offset,
			  size_t len,
			  void *data,
			  void *read_buffer)
{
	struct flash_dev *flash = &nand->flash;
	int ret;
	uint32_t size;
	void *temp_data;

	if (len < flash->blocksize || !IS_ALIGNED(address, flash->blocksize)) {
		/* Need to read/modify/write */
		ret = nand_block_read(nand, address, flash->blocksize,
				      read_buffer);
		if (ret)
			/* FIXME: should we fail harder here? */
			warn_msg("Warning: read failure during in %s (%d)",
				 __func__, ret);

		memcpy(read_buffer + offset, data, len);

		temp_data = read_buffer;
	} else {
		/*
		 * Some callers expect the complete block to be available in
		 * the read buffer, so that it can be reprogrammed to the next
		 * block in the case of a write failure
		 */
		memcpy(read_buffer, data, flash->blocksize);

		/* Just program the original buffer directly */
		temp_data = data;
	}

	/* Erase the block */
	ret = nand_erase_block(nand, address);
	if (ret)
		goto handle_error;

	/* now write the block page by page */
	for (size = flash->blocksize; size != 0; size -= flash->writesize) {
		ret = nand_write_page(nand, address, temp_data);
		if (ret)
			goto handle_error;

		address += flash->writesize;
		temp_data += flash->writesize;
	}

handle_error:
	return ret;
}

/*  *********************************************************************
    *  nand_erase_block(info,blk_addr)
    *
    *  Erase a block
    *
    *  Return value:
    *	   0 if ok
    *	   else error
    *
    **********************************************************************/
static int nand_erase_block(struct nand_dev *nand, uint64_t blk_addr)
{
	struct flash_dev *flash = &nand->flash;
	struct nand_probe_info *info = &nand->info;
	int rval;

	blk_addr = ALIGN_TO(blk_addr, flash->blocksize);

	rval = nand_get_block_status(nand, blk_addr);

	if (rval == NAND_BLOCK_STATUS_GOOD) {
		rval = do_nand_cmd(info, NAND_CMD_BLOCK_ERASE, blk_addr);
		if (rval)
			printf("block_erase_failed\n");
	}

	return rval;
}

/*  *********************************************************************
    *  nand_erase_range(softc,range)
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
    *
    **********************************************************************/
static int nand_erase_range(struct nand_dev * softc, flash_range_t * range)
{
	struct flash_dev *flash = &softc->flash;
	int blockstatus;
	uint64_t blockbase;

	for (blockbase = range->range_base; blockbase < range->range_base +
	      range->range_length; blockbase += flash->blocksize) {
		blockstatus = nand_erase_block(softc, blockbase);
		if (blockstatus) {
			err_msg("Erase address %#llx failed, bad block?",
				blockbase);
			/*
			 * seems ok to return 0; bad block will be marked
			 * during write
			 */
		}
	}
	return 0;
}

/*  *********************************************************************
    *  nand_erase_all(softc)
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
    **********************************************************************/
static int nand_erase_all(struct nand_dev * softc)
{
	return 0;
}

static int32_t nand_get_range_intersection(uint64_t range_base,
					   uint32_t range_len,
					   uint32_t blk_size,
					   range_descriptor_t * rdt)
{
	uint32_t size;

	rdt->first_block_data_offset = 0;
	rdt->last_block_data_offset = 0;
	rdt->new_range_base = range_base;
	rdt->num_block_access = 0;

	size = range_len;

	/* first align the address to blk boundary */
	rdt->new_range_base = ALIGN_TO(range_base, blk_size);
	rdt->first_block_data_offset = range_base - rdt->new_range_base;

	if (rdt->first_block_data_offset > 0) {
		rdt->num_block_access = 1;

		if (range_len <= blk_size - rdt->first_block_data_offset) {
			/* the data is less than 1 blk */

			goto end_fn;
		}
		size -= (blk_size - rdt->first_block_data_offset);
	}

	/* the last blk may not cover a full block */
	if ((size % blk_size) != 0)
	{
		rdt->last_block_data_offset = (size % blk_size);

		size -= rdt->last_block_data_offset;
		rdt->num_block_access++;
	}

	/* now the data should be blk aligned */

	rdt->num_block_access += size / blk_size;

end_fn:

	return 0;
}

/*  *********************************************************************
    *  nand_block_query(softc,sector)
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
    **********************************************************************/
static int nand_block_query(struct nand_dev * softc, flash_sector_t * sector)
{
	sector->flash_sector_size = softc->flash.blocksize;
	return 0;
}

static int nand_check_id(struct nand_probe_info *info)
{
	uint32_t id0, id1;
	int i;

	/* Disable auto-configure after first probe */
	BDEV_WR_F(NAND_CS_NAND_SELECT, AUTO_DEVICE_ID_CONFIG, 1);
	do_nand_cmd(info, NAND_CMD_DEVICE_ID_READ, 0x00);
	BDEV_WR_F(NAND_CS_NAND_SELECT, AUTO_DEVICE_ID_CONFIG, 0);

	id0 = BDEV_RD(BCHP_NAND_FLASH_DEVICE_ID);
	id1 = BDEV_RD(BCHP_NAND_FLASH_DEVICE_ID_EXT);
	for (i = 0; i < (int)ARRAY_SIZE(known_nand_chips); i++) {
		const struct nand_chip *chip;
		chip = &known_nand_chips[i];
		if (((id0 & chip->id_mask[0]) == chip->id_val[0]) &&
				((id1 & chip->id_mask[1]) == chip->id_val[1]))
			return i;
	}

	if (BDEV_RD_F(NAND_ONFI_STATUS, ONFI_detected))
		return NAND_CHIP_ONFI;

	err_msg("CS%d: NAND: unrecognized ID: %#x %#x", info->cs, id0, id1);
	return NAND_CHIP_NONE;
}

static int nand_do_probe(struct nand_dev *softc)
{
	struct flash_dev *flash = &softc->flash;
	struct nand_probe_info *info = &softc->info;
	unsigned int i;
	int min_ecc_bits = 0, ecc_codeword = 0, codeword_shift;
	uint16_t crc;

	/* Disable direct addressing + XOR for all NAND CS */
	BDEV_UNSET(BCHP_NAND_CS_NAND_SELECT, 0xff);
	BDEV_UNSET(BCHP_NAND_CS_NAND_XOR, 0xff);

	info->chip_idx = nand_check_id(info);
	if (info->chip_idx == NAND_CHIP_NONE)
		return -1;

	/* ONFI already auto-configured */
	if (info->chip_idx == NAND_CHIP_ONFI) {
		struct onfi_params params;

		/* Read ONFI parameter page */
		for (i = 0; i < ONFI_MAX_PARAM_PAGES; i++) {
			onfi_parameter_read(info, (uint32_t *)&params,
					   i * sizeof(params), sizeof(params));
			crc = onfi_crc16((uint8_t *)&params, ONFI_CRC16_LEN);
			if (crc == le16_to_cpu(params.crc))
				break;
		}

		if (i == ONFI_MAX_PARAM_PAGES) {
			err_msg("CS%d: NAND: ONFI parameter page bad CRC",
				info->cs);
			return -1;
		}

		if (strncmp(params.onfi, "ONFI", 4)) {
			err_msg("CS%d: NAND: invalid ONFI parameter page", info->cs);
			return -1;
		}

		flash->writesize = le32_to_cpu(params.page_size);

		flash->blocksize = flash->writesize;
		/*
		 * pages_per_block and blocks_per_lun may not be a power-of-2
		 * size (don't ask me who thought of this...). We assume that
		 * these dimensions will be power-of-2, so just truncate the
		 * remaining area.
		 */
		flash->blocksize <<= fls(le32_to_cpu(params.pages_per_block)) - 1;

		flash->size = (uint64_t)flash->blocksize * params.num_lun;
		flash->size <<= fls(le32_to_cpu(params.blocks_per_lun)) - 1;

		info->oob_sector = params.oob_per_page * NAND_FC_SIZE / flash->writesize;

		if (onfi_get_required_ecc(info, &params, &min_ecc_bits, &ecc_codeword))
			return -1;;

		info->supports_cache_read = le16_to_cpu(params.opt_cmds &
						ONFI_OPT_CMD_CACHE_READ);
	} else {
		const struct nand_chip *chip = &known_nand_chips[info->chip_idx];
		flash->writesize = chip->page_size;
		flash->blocksize = chip->block_size;
		flash->size = chip->size;
		info->oob_sector = chip->oob_sector;

		min_ecc_bits = chip->min_ecc_bits;
		ecc_codeword = chip->ecc_codeword;
	}

	/* Cap the spare area size at the controller maximum */
	if (info->oob_sector > NAND_MAX_SPARE_AREA)
		info->oob_sector = NAND_MAX_SPARE_AREA;

	if (info->cs) {
		info->sector_size_1k = !!(ecc_codeword > 512);
		info->ecc_level = min_ecc_bits >> info->sector_size_1k;
		/* Default to a minimum of BCH-4 */
		if (info->ecc_level < 4)
			info->ecc_level = 4;
	} else {
		uint32_t hw_spare = RD_ACC_CONTROL(info->cs, SPARE_AREA_SIZE);

		/* Inherit CS0 strap */
		info->ecc_level = RD_ACC_CONTROL(info->cs, ECC_LEVEL);
		info->sector_size_1k = RD_ACC_CONTROL(info->cs, SECTOR_SIZE_1K);
		/*
		 * Complain if spare area size is too large, or if the strapped
		 * value is way too small
		 */
		if (hw_spare > info->oob_sector ||
				(info->oob_sector >= 27 && hw_spare == 16)) {
			err_msg("CS%d: NAND: invalid spare area\n"
				"      Detected: %uB\n"
				"      Required: %dB",
					info->cs,
					RD_ACC_CONTROL(info->cs, SPARE_AREA_SIZE),
					info->oob_sector);
			return -1;
		}
		info->oob_sector = RD_ACC_CONTROL(info->cs, SPARE_AREA_SIZE);
	}

	/* TODO: cap max ECC level based on NAND controller support */

	/* Required codeword size, shifted for 512B minimum */
	codeword_shift = ffs(ecc_codeword >> NAND_FC_SHIFT) - 1;

	/*
	 * Does the given configuration meet the requirements?
	 *
	 * If our configuration corrects A bits per B bytes and the minimum
	 * required correction level is X bits per Y bytes, then we must ensure
	 * both of the following are true:
	 *
	 * (1) A / B >= X / Y
	 * (2) A >= X
	 *
	 * Requirement (1) ensures we can correct for the required bitflip
	 * density.
	 * Requirement (2) ensures we can correct even when all bitflips are
	 * clumped in the same sector.
	 *
	 * Note: (1) can be rewritten as: A / (B / Y) >= X
	 */
	if ( /* (1) */ ((info->ecc_level << codeword_shift) < min_ecc_bits)
			||
	     /* (2) */ (info->ecc_level << info->sector_size_1k) < min_ecc_bits
	   ) {
		char buf[40];
		nand_format_ecc_string(buf, info->ecc_level, info->sector_size_1k);
		err_msg("CS%d: NAND: invalid ECC setting", info->cs);
		err_msg("      Detected: %s", buf);
		nand_format_ecc_string(buf, min_ecc_bits >> codeword_shift, codeword_shift);
		err_msg("      Required: %s", buf);
		return -1;
	}

	WR_ACC_CONTROL(info->cs, ECC_LEVEL, info->ecc_level);
	WR_ACC_CONTROL(info->cs, SPARE_AREA_SIZE, info->oob_sector);
	WR_ACC_CONTROL(info->cs, SECTOR_SIZE_1K, info->sector_size_1k);

	flash->ecc_strength = info->ecc_level << info->sector_size_1k;
	flash->ecc_step = 512 << info->sector_size_1k;
	flash->oob_sector_size = info->oob_sector;

	for (i = 0; i < ARRAY_SIZE(nand_page_sizes); i++)
		if (nand_page_sizes[i] == flash->writesize) {
			nand_set_page_size_index(info->cs, i);
			break;
		}
	if (i >= ARRAY_SIZE(nand_page_sizes))
		return -1;

	for (i = 0; i < ARRAY_SIZE(nand_block_sizes); i++)
		if (nand_block_sizes[i] == flash->blocksize) {
			nand_set_block_size_index(info->cs, i);
			break;
		}
	if (i >= ARRAY_SIZE(nand_block_sizes))
		return -1;

	nand_set_device_size_index(info->cs, fls(flash->size >> 23));

	/* threshold = ceil(BCH-level * 0.75) */
	WR_CORR_THRESH(info->cs, ((info->ecc_level << info->sector_size_1k) * 3 + 2) / 4);

	info->bbi_map = (unsigned long)NAND_BAD_BLOCK_INDICATOR_MAP;

	/* Mark CSx "uses NAND" for SW */
	BDEV_SET(BCHP_NAND_CS_NAND_SELECT, 1 <<
		(BCHP_NAND_CS_NAND_SELECT_EBI_CS_0_USES_NAND_SHIFT + info->cs));

	WR_ACC_CONTROL(info->cs, PARTIAL_PAGE_EN, 0);
	/* Enable prefetch; see JIRA CRNAND-58 for usage notes */
	WR_ACC_CONTROL(info->cs, PREFETCH_EN, 1);

	return 0;
}

/*  *********************************************************************
    *  nanddrv_probe(drv,probe_a,probe_b,probe_ptr)
    *
    *  Device probe routine.  Attach the flash device to
    *  BOLT's device table.
    *
    *  Input parameters:
    *	   drv - driver descriptor
    *	   cs - chip select to use
    *	   probe_b - unused
    *	   probe_ptr - unused
    *
    *  Return value:
    *	   nothing
    **********************************************************************/
static void nanddrv_probe(bolt_driver_t * drv,
			  unsigned long cs,
			  unsigned long probe_b, void *probe_ptr)
{
	struct nand_dev *softc;
	struct flash_dev *flash;

	/* prefer not free this memory */
	softc = (struct nand_dev *)KMALLOC(sizeof(struct nand_dev), 0);
	if (!softc)
		return;

	memset(softc, 0, sizeof(struct nand_dev));
	flash = &softc->flash;

	softc->flash.cs = softc->info.cs = cs;
	softc->flash.type = FLASH_TYPE_NAND;

	/* Store driver data */
	softc->flash.priv = softc;

	/* Do automatic probing */
	if (nand_do_probe(softc))
		return;

	/* Disable NAND write-protect pin, if supported */
	nand_set_write_protect(0);

	/* Flash DMA? */
	if (CFG_FLASH_DMA) {
		struct nand_dev *nand = softc;

		nand->flash_dma_base = (void *)BVIRTADDR(BCHP_FLASH_DMA_REG_START);

		/* CRNAND-61: descriptors require 32-byte alignment */
		nand->dma_desc = KMALLOC(sizeof(*nand->dma_desc), 32);
		if (!nand->dma_desc) {
			printf("Out of memory\n");
			goto err;
		}

		flash_dma_irq_clear();

		flash_dma_writel(nand, FLASH_DMA_MODE, 1); /* linked-list */
		flash_dma_writel(nand, FLASH_DMA_ERROR_STATUS, 0);
	}

	softc->fd_sectorbuffer = KMALLOC(flash->blocksize, 4);
	softc->blk_status = KMALLOC((sizeof(*softc->blk_status) *
				    (flash->size / flash->blocksize)), 4);
	softc->page_buf = KMALLOC(flash->writesize, 4);
	if (!softc->fd_sectorbuffer || !softc->blk_status ||
			!softc->page_buf) {
		printf("Error: no memory\n");
		goto err;
	}

	/* Register this flash with the generic flash system */
	flash_register_device(drv, &softc->flash);

	nand_print_config(softc);

	/* Registered successfully */
	return;

err:
	KFREE(softc->page_buf);
	KFREE(softc->blk_status);
	KFREE(softc->fd_sectorbuffer);
	KFREE(softc->dma_desc);
}

/*  *********************************************************************
    *  nanddrv_open(ctx)
    *
    *  Called when the flash device is opened.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0 if ok else error code
    **********************************************************************/
static int nanddrv_open(bolt_devctx_t * ctx)
{
	return 0;
}

/*  *********************************************************************
    *  nanddrv_inpstat(ctx,inpstat)
    *
    *  Return "input status".  For flash devices, we always return true.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   inpstat - input status structure
    *
    *  Return value:
    *	   0 if ok, else error code
    **********************************************************************/
static int nanddrv_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat)
{
	inpstat->inp_status = 1;
	return 0;
}

/*  *********************************************************************
    *  nanddrv_close(ctx)
    *
    *  Close the flash device.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0
    **********************************************************************/
static int nanddrv_close(bolt_devctx_t * ctx)
{
	return 0;
}

/*  *********************************************************************
    *  nand_handle_write_error(part, bad_block_address, data)
    *
    *  This function uses the skip-block algorithm to manage bad blocks
    *  within a partition.
    *
    *  Input parameters:
    *	   part - partition structure pointer
    *	   bad_block_address - address of the bad block
    *	   data - block of data to be written
    *
    *  Return value:
    *	   0
    **********************************************************************/
static int32_t nand_handle_write_error(struct flash_partition *part,
				       uint64_t bad_block_address,
				       uint8_t * data)
{
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;

	uint64_t part_size = part->size;
	uint64_t part_spare_size = part->sparesize;
	uint8_t *buffer2;
	int rval;
	uint64_t part_base = part->offset;
	uint32_t num_spare_blocks = part_spare_size / flash->blocksize;
	uint32_t num_bad_blocks;
	uint32_t part_first_index = part_base / flash->blocksize;
	uint32_t part_last_index =
	    (part_base + part_size - 1) / flash->blocksize;
	uint32_t num_blocks_in_dev = flash->size / flash->blocksize;
	uint64_t temp_block_address;
	uint64_t src_addr;
	uint64_t dest_addr;
	uint64_t temp_src_addr;
	uint64_t temp_dest_addr;
	uint8_t *copy_buffer;

	/* check if we're out of spare blocks */
	num_bad_blocks =
	    nand_get_bad_blk_cnt(part, part_first_index, part_last_index,
				 num_blocks_in_dev);
	if (num_bad_blocks > num_spare_blocks) {
		err_msg("NAND: write failed: no spare blocks available\n"
				"      Please Re-Partition flash");
		return 1;
	}

	/* add bad block to block status array */
	temp_block_address = nand_get_good_block(part, bad_block_address);
	nand_mark_block_bad(softc, temp_block_address);

	/* allocate a 2nd buffer */
	buffer2 = KMALLOC(flash->blocksize, 4);

	/* initialize source and destination addresses to point to the end of the partition (excluding spare blocks) */
	dest_addr = part_base + part_size - part_spare_size - flash->blocksize;
	src_addr = dest_addr - flash->blocksize;

	/* get adjusted addresses (after skipping) */
	temp_dest_addr = nand_get_good_block(part, dest_addr);
	temp_src_addr = nand_get_good_block(part, src_addr);

	/* copy data starting from the last block in the partition (excluding spare blocks) */
	while (dest_addr >= bad_block_address) {
		if (dest_addr == bad_block_address)
			copy_buffer = data;	/* use previously buffered data */
		else {
			/* read block from current block address */
			copy_buffer = buffer2;
			rval = nand_block_read(softc, temp_src_addr, flash->blocksize,
					copy_buffer);
			if (rval)
				/* FIXME: fail harder here? */
				warn_msg("Warning: read failure in %s (%d)",
					 __func__, rval);
		}

		/* write next block with new data */
		rval = nand_block_write(softc, temp_dest_addr, 0,
					flash->blocksize, copy_buffer,
					copy_buffer);

		/* if we get another write error */
		if (rval != 0) {
			/* record bad block info */
			nand_mark_block_bad(softc, temp_dest_addr);

			/* check if we ran out of spare blocks */
			num_bad_blocks =
			    nand_get_bad_blk_cnt(part, part_first_index,
						 part_last_index,
						 num_blocks_in_dev);
			if (num_bad_blocks > num_spare_blocks) {
				err_msg("NAND: write failed: no spare blocks available\n"
					"      Please Re-Partition flash");
				KFREE(buffer2);
				return 2;
			}

			/* reset current and next pointers to the end of partition */
			dest_addr =
			    part_base + part_size - part_spare_size -
			    flash->blocksize;
			src_addr = dest_addr - flash->blocksize;
			temp_dest_addr = nand_get_good_block(part, dest_addr);
			temp_src_addr = nand_get_good_block(part, src_addr);
		} else {
			/* decrment cur and next pointers */
			dest_addr -= flash->blocksize;
			src_addr -= flash->blocksize;
			temp_dest_addr = nand_get_good_block(part, dest_addr);
			temp_src_addr = nand_get_good_block(part, src_addr);
		}
	}

	KFREE(buffer2);
	return 0;
}

static int nand_handle_read_disturb(struct flash_partition *part,
					uint64_t block_address)
{
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;

	uint8_t *rbuf1 = softc->fd_sectorbuffer;
	int rval;
	uint64_t temp_block_address;

	temp_block_address = nand_get_good_block(part, block_address);

	rval = nand_block_read(softc, temp_block_address, flash->blocksize,
			       rbuf1);
	/* FIXME: fail harder here? */
	if (rval)
		warn_msg("Warning: read failure in %s (%d)", __func__, rval);

	/*
	 * No need to check read status here as we are going to rewrite this
	 * block
	 */
	rval = nand_block_write(softc, temp_block_address, 0, flash->blocksize,
				rbuf1, rbuf1);

	if (rval != 0) {
		err_msg("NAND: write failed after a read disturbance");
		return nand_handle_write_error(part, block_address, rbuf1);
	}

	return 0;
}

static int nanddrv_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;

	uint8_t *data = buffer->buf_ptr;
	uint32_t buffer_len = buffer->buf_length;
	uint64_t buffer_offset = buffer->buf_offset + part->offset;

	uint32_t i;
	uint64_t block_address, temp_block_address;

	int ret;
	uint8_t *read_buffer = softc->fd_sectorbuffer;
	range_descriptor_t range_descrptr;

	buffer->buf_retlen = buffer_len;

	if (buffer_len == 0) {
		buffer->buf_retlen = buffer_len;
		return (buffer->buf_length == buffer_len) ? 0 : BOLT_ERR_IOERR;
	}

	/* update the blk status array */
	if (softc->fp_blk_status_valid == 0) {
		nand_create_block_status_array(part);
		softc->fp_blk_status_valid = 1;
	}

	nand_get_range_intersection(buffer_offset, buffer_len, flash->blocksize,
				    &range_descrptr);

	block_address = range_descrptr.new_range_base;

	for (i = 0; i < (range_descrptr.num_block_access); i++) {
		uint32_t block_offs = 0; /* offset within the block */
		size_t len;

		if (i == 0 && range_descrptr.first_block_data_offset > 0) {
			block_offs = range_descrptr.first_block_data_offset;
			len =
			    flash->blocksize - range_descrptr.first_block_data_offset;

			if (buffer_len <=
			    (flash->blocksize -
			     range_descrptr.first_block_data_offset)) {
				len = buffer_len;
			}
		} else if ((i == range_descrptr.num_block_access - 1)
			   && (range_descrptr.last_block_data_offset != 0)) {
			len = range_descrptr.last_block_data_offset;
		} else {
			len = flash->blocksize;
		}

		temp_block_address = nand_get_good_block(part, block_address);

		/* check whether the block is in partition or not */

		if (check_block_address(part, temp_block_address)) {
			err_msg("NAND: write failure: cannot find good blocks in partition");
			return -1;
		}

		do {
			ret = nand_block_write(softc, temp_block_address,
					       block_offs, len, data,
					       read_buffer);

			if (ret) {
				/*
				 * Call write error handler to perform
				 * skip-block algorithm on the partition.
				 *
				 * Note: read_buffer contains the block of data
				 * which still needs to be written
				 */
				ret = nand_handle_write_error(part, block_address,
							    read_buffer);

				if (ret)
					return -1;
			}
		} while (ret);

		data += len;
		block_address += flash->blocksize;
	}

	return 0;
}

static int nanddrv_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	struct flash_dev *flash = &softc->flash;

	uint8_t *data_buf = buffer->buf_ptr;
	uint32_t buffer_len = buffer->buf_length;
	uint64_t buffer_offset = buffer->buf_offset + part->offset;

	bool nand_read_disturb = false;
	uint64_t nand_read_disturb_address = 0;

	uint64_t block_address, temp_block_address;

	int ret = 0;

	range_descriptor_t range_descrptr;
	uint32_t i;

	buffer->buf_retlen = buffer->buf_length;

	/* update the blk status array */
	if (softc->fp_blk_status_valid == 0) {
		nand_create_block_status_array(part);
		softc->fp_blk_status_valid = 1;
	}

	nand_get_range_intersection(buffer_offset, buffer_len, flash->blocksize,
				    &range_descrptr);

	block_address = range_descrptr.new_range_base;

	for (i = 0; i < (range_descrptr.num_block_access); i++) {
		size_t len;
		uint64_t addr;
		int res;

		temp_block_address = nand_get_good_block(part, block_address);
		if (check_block_address(part, temp_block_address)) {
			err_msg("NAND: read failure: cannot find good blocks in partition");
			return -1;
		}

		addr = temp_block_address;

		if (i == 0 && range_descrptr.first_block_data_offset > 0) {
			addr += range_descrptr.first_block_data_offset;
			len = flash->blocksize - range_descrptr.first_block_data_offset;
			len = min(len, buffer_len);
		} else if ((i == range_descrptr.num_block_access - 1)
			   && (range_descrptr.last_block_data_offset != 0)) {
			len = range_descrptr.last_block_data_offset;
		} else {
			len = flash->blocksize;
		}

		res = nand_block_read(softc, addr, len, data_buf);
		if (res == NAND_ERR_UNCORR) {
			err_msg("NAND: uncorrectable read error at %#llx",
					(unsigned long long)temp_block_address);
			ret = BOLT_ERR_IOERR;
		} else if (res == NAND_ERR_CORR) {
			nand_read_disturb = true;
			nand_read_disturb_address = block_address;
		} else if (res && !ret) {
			/* Unknown error? */
			ret = BOLT_ERR;
		}

		data_buf += len;
		block_address += flash->blocksize;
	}

	if (!ret && nand_read_disturb) {
		return nand_handle_read_disturb(part,
						nand_read_disturb_address);
	}

	return ret;
}

/**
 * Ensure that the bad block array is valid
 */
static void nand_ensure_block_array_valid(struct flash_partition *part)
{
	struct nand_dev *softc = flashpart_get_drvdata(part);
	/* update the blk status array */
	if (softc->fp_blk_status_valid == 0) {
		nand_create_block_status_array(part);
		softc->fp_blk_status_valid = 1;
	}
}

/*  *********************************************************************
    *  nanddrv_ioctl(ctx,buffer)
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
    **********************************************************************/
static int nanddrv_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	struct nand_dev *softc = flashpart_get_drvdata(part);
	nvram_info_t *nvinfo;
	struct flash_info *info;
	flash_range_t range;
	struct flash_feature_cmd_info *nand_feature_cmd_info;
	int offset;
	int ioctl_cmd;

	/* Informationaly ioctl's don't need to scan for bad blocks */
	ioctl_cmd = (int)buffer->buf_ioctlcmd;
	switch (ioctl_cmd) {
	case IOCTL_NVRAM_GETINFO:
	case IOCTL_FLASH_GETINFO:
	case IOCTL_FLASH_GETPARTINFO:
	case IOCTL_FLASH_GETSECTORS:
		break;
	default:
		nand_ensure_block_array_valid(part);
	}

	switch (ioctl_cmd) {
	case IOCTL_NVRAM_GETINFO:
		/*
		 * We only support NVRAM on flashes that have been partitioned
		 * into at least two partitions.  Every partition supports
		 * being an NVRAM in that case, but we'll only attach one
		 * of them to the environment subsystem.
		 */
		if (softc->flash.nparts <= 1) {
			return BOLT_ERR_UNSUPPORTED;
		}
		nvinfo = (nvram_info_t *) buffer->buf_ptr;
		if (buffer->buf_length != sizeof(nvram_info_t))
			return BOLT_ERR_INV_PARAM;

		nvinfo->nvram_offs_part = part->offset;
		nvinfo->nvram_offset = 0;
		nvinfo->nvram_size = part->size;
		nvinfo->nvram_eraseflg = 1;
		buffer->buf_retlen = sizeof(nvram_info_t);
		return 0;
		break;

	case IOCTL_FLASH_ERASE_SECTOR:
	case IOCTL_FLASH_ERASE_BLOCK:
		return nand_erase_block(softc, buffer->buf_offset);

	case IOCTL_FLASH_ERASE_ALL:
		offset = (int)buffer->buf_offset;
		if (offset != 0)
			return -1;
		nand_erase_all(softc);
		return 0;

	case IOCTL_FLASH_GETINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = 0;
		info->flash_size = softc->flash.size;
		info->type = softc->flash.type;
		info->flags = FLASH_FLAG_NOERASE;
		info->page_size = softc->flash.writesize;
		return 0;

	case IOCTL_FLASH_GETPARTINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = part->offset;
		info->flash_size = part->size;
		info->type = softc->flash.type;
		info->flags = FLASH_FLAG_NOERASE;
		return 0;

	case IOCTL_FLASH_GETSECTORS:
		return nand_block_query(softc,
					(flash_sector_t *) buffer->buf_ptr);

	case IOCTL_FLASH_ERASE_RANGE:
		memcpy(&range, buffer->buf_ptr, sizeof(flash_range_t));
		range.range_base += part->offset;
		if (range.range_length > part->size) {
			range.range_length = part->size;
		}
		return nand_erase_range(softc, &range);

	case IOCTL_FLASH_HANDLE_PARTITION_READ_DISTURB:
		nand_handle_partition_read_disturb(ctx);
		return 0;

	case IOCTL_FLASH_SET_FEATURE :
	case IOCTL_FLASH_GET_FEATURE :
		nand_feature_cmd_info = (struct flash_feature_cmd_info *)buffer->buf_ptr;
		nand_set_get_feature(softc,
				     ioctl_cmd == IOCTL_FLASH_SET_FEATURE ?
				     NAND_SET_FEATURE_CMD :
				     NAND_GET_FEATURE_CMD,
				     nand_feature_cmd_info->feature_addr,
				     nand_feature_cmd_info->data_buf,
				     nand_feature_cmd_info->data_buf_len);
		return 0;

	default:
		return -1;
	}

	return -1;
}
