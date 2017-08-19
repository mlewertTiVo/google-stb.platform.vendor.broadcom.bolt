/***************************************************************************
 *     Copyright (c) 2015-2016, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "dev_emmc_core_os.h"
#include "dev_emmc_core.h"
#include "dev_emmc_debug.h"
#include "dev_emmc_core_defs.h"
#include "sdhc.h"
#include "emmc_cst.h"

/*
 * Register access macros
 */
#define HOST_GET_FIELD(reg, field) \
	((reg & field##_MASK) >> field##_SHIFT)
#define HOST_SET_FIELD(reg, field, val) \
	(reg = ((reg & ~field##_MASK) | \
		((val << field##_SHIFT) & field##_MASK)))

/* op-code issue related timeout values */
#define POLL_DELAY_USECS 1
#define CMD_TIMEOUT_USECS 200000
#define CMD_SLEEP_CNT (CMD_TIMEOUT_USECS / POLL_DELAY_USECS)
#define DATA_TIMEOUT_USECS 1000000
#define DATA_SLEEP_CNT (DATA_TIMEOUT_USECS / POLL_DELAY_USECS)

#define SOFTWARE_RESET_TIMEOUT_USECS 1000000	/* 1 second */


#define SDHCI_CMD_RESP_R1 0
#define SDHCI_CMD_RESP_R2 1
#define SDHCI_CMD_RESP_R3 2
#define SDHCI_CMD_RESP_R4 3
#define SDHCI_CMD_RESP_R5 4
#define SDHCI_CMD_RESP_R6 5
#define SDHCI_CMD_RESP_R7 6
#define SDHCI_CMD_RESP_R1B 7
#define SDHCI_CMD_RESP_R5B 8

struct cmd_info {
	uint8_t type;			/* Cmd type from MMC spec */
#define CMDINFO_TYPE_BC 0
#define CMDINFO_TYPE_BCR 1
#define CMDINFO_TYPE_AC 2
#define CMDINFO_TYPE_ADTC 3
	uint8_t resp;			/* Cmd response type from MMC spec */
#define CMDINFO_RESP_NONE 0
#define CMDINFO_RESP_R1 1
#define CMDINFO_RESP_R1B 2
#define CMDINFO_RESP_R2 3
#define CMDINFO_RESP_R3 4
#define CMDINFO_RESP_R4 5
#define CMDINFO_RESP_R5 6
#define CMDINFO_RESP_R6 7
#define CMDINFO_RESP_R7 8
	uint8_t data_xfer;		/* Transfer direction (Host based) */
#define CMDINFO_DXFER_NONE 0
#define CMDINFO_DXFER_READ 1
#define CMDINFO_DXFER_READ_MULTI 2
#define CMDINFO_DXFER_WRITE 3
#define CMDINFO_DXFER_WRITE_MULTI 4
	char *desc;
};


struct mmc_cmd {
	int index;
	uint32_t arg;
};

struct mmc_data {
	uint32_t block_size;
	uint32_t block_count;
	uint32_t dma_address;
};

/*
 * Command Implementation
 * ----------------------
 * Respons Type Information
 *  R1 (Index check  On, CRC check  On, 48bit)
 *    : CMD 3,7,8,11,12,13,14,19,16,17,18,20,23,24,25,26,27,30,31,35,36,42,55,56
 *  R1b(Index check  On, CRC check  On, 48bit/busy_check)
 *    : CMD 5,6,7,12,28.29,38
 *  R2 (Index check Off, CRC check  On, 136bit) : CMD 2,9,10
 *  R3 (Index check Off, CRC check Off, 48bit)	: CMD 1
 *  R4 (Index check Off, CRC check Off, 48bit)	: CMD 39
 *  R5 (Index check  On, CRC check  On, 48bit)	: CMD 40
 * Clase Information v4.41
 *  class 0 : basic : CMD 0,1,2,3,4,5,6,7,8,9,10,12,13,14,15,19
 *  class 1 : stream read (Obsolete on v4.5) : CMD 11
 *  class 2 : block read	  : CMD 16,17,18,21(v4.5)
 *  class 3 : stream write (Obsolete on v4.5) : CMD 20
 *  class 4 : block write	  : CMD 23,24,25,26,27,49(v4.5)
 *  class 5 : erase		  : CMD 35,36,38
 *  class 6 : write protection	  : CMD 28,29,30,31
 *  class 7 : lock card		  : CMD 42
 *  class 8 : applicationspecific : CMD 55,56
 *  class 9 : I/O mode		  : CMD 39,40
 *  class 10–11 : reserved
 * -------------------------------------------------
 */
static const struct cmd_info emmc_cmd_table[] = {
	{
		CMDINFO_TYPE_BC, CMDINFO_RESP_NONE, CMDINFO_DXFER_NONE,	/* 0 */
		"GO_IDLE_STATE"
	},
	{
		CMDINFO_TYPE_BCR, CMDINFO_RESP_R3, CMDINFO_DXFER_NONE,	/* 1 */
		"SEND_OP_COND"
	},
	{
		CMDINFO_TYPE_BCR, CMDINFO_RESP_R2, CMDINFO_DXFER_NONE,	/* 2 */
		"ALL_SEND_CID"
	},
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 3 */
		"SET_RELATIVE_ADDR"
	},
	{ 0, 0, 0, NULL },						/* 4 */
	{ 0, 0, 0, NULL },						/* 5 */
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1B, CMDINFO_DXFER_NONE,	/* 6 */
		"SWITCH"
	},
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 7 */
		"SELECT_CARD"
	},
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_READ,	/* 8 */
		"SEND_EXT_CSD"
	},
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R2, CMDINFO_DXFER_NONE,	/* 9 */
		"SEND_CSD"
	},
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R2, CMDINFO_DXFER_NONE,	/* 10 */
		"SEND_CID"
	},
	{ 0, 0, 0, NULL  },						/* 11 */
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 12 */
		"STOP_TRANSMITTION"
	},
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 13 */
		"SEND_STATUS"
	},
	{ 0, 0, 0, NULL  },						/* 14 */
	{ 0, 0, 0, NULL  },						/* 15 */
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 16 */
		"SET_BLOCKLEN"
	},
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_READ,	/* 17 */
		"READ_SINGLE_BLOCK"
	},
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_READ_MULTI,	/* 18 */
		"READ_MULTIPLE_BLOCK"
	},
	{ 0, 0, 0, NULL  },						/* 19 */
	{ 0, 0, 0, NULL  },						/* 20 */
	{ 0, 0, 0, NULL  },						/* 21 */
	{ 0, 0, 0, NULL  },						/* 22 */
	{
		CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,	/* 23 */
		"SET_BLOCK_COUNT"
	},
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_WRITE,/* 24 */
		"WRITE_SINGLE_BLOCK"
	},
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_WRITE_MULTI,/* 25 */
		"WRITE_MULTIPLE_BLOCK"
	},
	{ 0, 0, 0, NULL  },						/* 26 */
	{
		CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1, CMDINFO_DXFER_WRITE,/* 27 */
		"PROGRAM_CSD"
	}
};

static const unsigned startup_speeds[] = { 400000, 300000, 200000, 100000 };

/*
 * Forward declarations
 */
static int emmc_cmd13_send_status(struct emmc_chip *chip, uint32_t *status);

/*
 * Functions
 */

/*
 * Enable interrupts
 */
static void emmc_enable_int(struct emmc_chip *chip, uint32_t mask)
{
	HOST_REG_WR(&chip->regs, INTR_STAT_ENABLE,  mask);
}

/*
 * Clear specified interrupt bits
 */
static void emmc_clear_int_status(struct emmc_chip *chip, uint32_t mask)
{
	HOST_REG_WR(&chip->regs, INTR_STAT, mask);
}

/*
 * Return a bit field within an array of ints. The max length field is
 * 32 bits amd the field can cross int boundries.
 */
static uint32_t array_to_bits(uint32_t *array, int start_bit, int length)
{
	uint32_t val;
	int index = start_bit >> 5;
	int shift = start_bit & 31;
	uint32_t mask;
	int rem;

	if (length == 32)
		mask = 0xffffffff;
	else
		mask = (1 << length) - 1;
	val = (array[index] >> shift);
	rem = ((length + shift) - 32);
	if (rem > 0)
		val |= (array[index + 1] << (length - rem));
	return val & mask;
}

static void emmc_decode_extcsd(struct emmc_chip *chip, uint8_t *ext_csd)
{
	unsigned int i;

	/* Properties Segment */
	chip->extcsd.S_CMD_SET = ext_csd[504];
	chip->extcsd.HPI_FEATURES = ext_csd[503];
	chip->extcsd.BKOPS_SUPPORT = ext_csd[502];
	chip->extcsd.MAX_PACKED_READS = ext_csd[501];
	chip->extcsd.MAX_PACKED_WRITES = ext_csd[500];
	chip->extcsd.DATA_TAG_SUPPORT = ext_csd[499];
	chip->extcsd.TAG_UNIT_SIZE = ext_csd[498];
	chip->extcsd.TAG_RES_SIZE = ext_csd[497];
	chip->extcsd.CONTEXT_CAPABILITIES = ext_csd[496];
	chip->extcsd.LARGE_UNIT_SIZE_M1 = ext_csd[495];
	chip->extcsd.EXT_SUPPORT = ext_csd[494];
	chip->extcsd.CACHE_SIZE =
		(ext_csd[252] << 24) + (ext_csd[251] << 16) +
		(ext_csd[250] << 8) + ext_csd[249];
	chip->extcsd.GENERIC_CMD6_TIME = ext_csd[248];
	chip->extcsd.POWER_OFF_LONG_TIME = ext_csd[247];
	chip->extcsd.BKOPS_STATUS = ext_csd[246];
	chip->extcsd.CORRECTLY_PRG_SECTORS_NUM =
		(ext_csd[245] << 24) + (ext_csd[244] << 16) +
		(ext_csd[243] << 8) + ext_csd[242];
	chip->extcsd.INI_TIMEOUT_AP = ext_csd[241];
	chip->extcsd.PWR_CL_DDR_52_360 = ext_csd[239];
	chip->extcsd.PWR_CL_DDR_52_195 = ext_csd[238];
	chip->extcsd.PWR_CL_200_360 = ext_csd[237];
	chip->extcsd.PWR_CL_200_195 = ext_csd[236];
	chip->extcsd.MIN_PERF_DDR_W_8_52 = ext_csd[235];
	chip->extcsd.MIN_PERF_DDR_R_8_52 = ext_csd[234];
	chip->extcsd.TRIM_MULT = ext_csd[232];
	chip->extcsd.SEC_FEATURE_SUPPORT = ext_csd[231];
	chip->extcsd.BOOT_INFO = ext_csd[228];
	chip->extcsd.BOOT_SIZE_MULT = ext_csd[226];
	chip->extcsd.ACC_SIZE = ext_csd[225];
	chip->extcsd.HC_ERASE_GRP_SIZE = ext_csd[224];
	chip->extcsd.ERASE_TIMEOUT_MULT = ext_csd[223];
	chip->extcsd.REL_WR_SEC_C = ext_csd[222];
	chip->extcsd.HC_WP_GRP_SIZE = ext_csd[221];
	chip->extcsd.S_C_VCC = ext_csd[220];
	chip->extcsd.S_C_VCCQ = ext_csd[219];
	chip->extcsd.S_A_TIMEOUT = ext_csd[217];
	chip->extcsd.SEC_COUNT =
		(ext_csd[215] << 24) + (ext_csd[214] << 16) +
		(ext_csd[213] << 8) + ext_csd[212];
	chip->extcsd.MIN_PERF_W_8_52 = ext_csd[210];
	chip->extcsd.MIN_PERF_R_8_52 = ext_csd[209];
	chip->extcsd.MIN_PERF_W_8_26_4_52 = ext_csd[208];
	chip->extcsd.MIN_PERF_R_8_26_4_52 = ext_csd[207];
	chip->extcsd.MIN_PERF_W_4_26 = ext_csd[206];
	chip->extcsd.MIN_PERF_R_4_26 = ext_csd[205];
	chip->extcsd.PWR_CL_26_360 = ext_csd[203];
	chip->extcsd.PWR_CL_52_360 = ext_csd[202];
	chip->extcsd.PWR_CL_26_195 = ext_csd[201];
	chip->extcsd.PWR_CL_52_195 = ext_csd[200];
	chip->extcsd.PARTITION_SWITCH_TIME = ext_csd[199];
	chip->extcsd.OUT_OF_INTERRUPT_TIME = ext_csd[198];
	chip->extcsd.DRIVER_STRENGTH = ext_csd[197];
	chip->extcsd.DEVICE_TYPE = ext_csd[196];
	chip->extcsd.CSD_STRUCTURE = ext_csd[194];
	/* Modes Segment */
	chip->extcsd.EXT_CSD_REV = ext_csd[192];
	chip->extcsd.CMD_SET = ext_csd[191];
	chip->extcsd.CMD_SET_REV = ext_csd[189];
	chip->extcsd.POWER_CLASS = ext_csd[187];
	chip->extcsd.HS_TIMING = ext_csd[185];
	chip->extcsd.BUS_WIDTH = ext_csd[183];
	chip->extcsd.ERASED_MEM_CONT = ext_csd[181];
	chip->extcsd.PARTITION_CONFIG = ext_csd[179];
	chip->extcsd.BOOT_CONFIG_PROT = ext_csd[178];
	chip->extcsd.BOOT_BUS_CONDITIONS = ext_csd[177];
	chip->extcsd.ERASE_GROUP_DEF = ext_csd[175];
	chip->extcsd.BOOT_WP_STATUS = ext_csd[174];
	chip->extcsd.BOOT_WP = ext_csd[173];
	chip->extcsd.USER_WP = ext_csd[171];
	chip->extcsd.FW_CONFIG = ext_csd[169];
	chip->extcsd.RPMB_SIZE_MULT = ext_csd[168];
	chip->extcsd.WR_REL_SET = ext_csd[167];
	chip->extcsd.WR_REL_PARAM = ext_csd[166];
	chip->extcsd.SANITIZE_START = ext_csd[165];
	chip->extcsd.BKOPS_START = ext_csd[164];
	chip->extcsd.BKOPS_EN = ext_csd[163];
	chip->extcsd.RST_n_FUNCTION = ext_csd[162];
	chip->extcsd.HPI_MGMT = ext_csd[161];
	chip->extcsd.PARTITIONING_SUPPORT = ext_csd[160];
	chip->extcsd.MAX_ENH_SIZE_MULT =
		(ext_csd[159] << 16) + (ext_csd[158] << 8) + ext_csd[157];
	chip->extcsd.PARTITIONS_ATTRIBUTE = ext_csd[156];
	chip->extcsd.PARTITION_SETTING_COMPLETED = ext_csd[155];
	for (i = 0; i < 4; i++)
		chip->extcsd.GP_SIZE_MULT[i] =
			(ext_csd[143+3*i+2]<<16) +
			(ext_csd[143+3*i+1]<<8) + ext_csd[143+3*i];
	chip->extcsd.ENH_SIZE_MULT =
		(ext_csd[142] << 16) + (ext_csd[141] << 8) + ext_csd[140];
	chip->extcsd.ENH_START_ADDR =
		(ext_csd[139] << 24) + (ext_csd[138] << 16) +
		(ext_csd[137] << 8) + ext_csd[136];
	chip->extcsd.SEC_BAD_BLK_MGMNT = ext_csd[134];

	if (chip->extcsd.EXT_CSD_REV < JESD84_V45) {	/* JESD84-B441 */
		chip->extcsd.SEC_ERASE_MULT = ext_csd[230];
		chip->extcsd.SEC_TRIM_MULT = ext_csd[229];
	}

	if (chip->extcsd.EXT_CSD_REV > JESD84_V441) { /* JESD84-B45 */
		chip->extcsd.TCASE_SUPPORT = ext_csd[132];
		chip->extcsd.PERIODIC_WAKEUP = ext_csd[131];
		chip->extcsd.PROGRAM_CID_CSD_DDR_SUPPORT = ext_csd[130];
		memcpy(chip->extcsd.VENDOR_SPECIFIC_FIELD, &ext_csd[64], 64);
		chip->extcsd.NATIVE_SECTOR_SIZE = ext_csd[63];
		chip->extcsd.USE_NATIVE_SECTOR = ext_csd[62];
		chip->extcsd.DATA_SECTOR_SIZE = ext_csd[61];
		chip->extcsd.INI_TIMEOUT_EMU = ext_csd[60];
		chip->extcsd.CLASS_6_CTRL = ext_csd[59];
		chip->extcsd.DYNCAP_NEEDED = ext_csd[58];
		chip->extcsd.EXCEPTION_EVENTS_CTRL =
			(ext_csd[57] << 8) + ext_csd[56];
		chip->extcsd.EXCEPTION_EVENTS_STATUS =
			(ext_csd[55] << 8) + ext_csd[54];
		chip->extcsd.EXT_PARTITIONS_ATTRIBUTE =
			(ext_csd[53] << 8) + ext_csd[52];
		memcpy(chip->extcsd.CONTEXT_CONF, &ext_csd[37], 15);
		chip->extcsd.PACKED_COMMAND_STATUS = ext_csd[36];
		chip->extcsd.PACKED_FAILURE_INDEX = ext_csd[35];
		chip->extcsd.POWER_OFF_NOTIFICATION = ext_csd[34];
		chip->extcsd.CACHE_CTRL = ext_csd[33];
		chip->extcsd.FLUSH_CACHE = ext_csd[32];
	}

}

#if DEBUG_EMMC_DRV
static void emmc_print_extcsd(struct emmc_chip *chip)
{
	uint8_t i;

	/*  Properties Segment */
	os_printf("ExtCSD.S_CMD_SET             [504] = 0x%02X (%d)\n",
		chip->extcsd.S_CMD_SET, chip->extcsd.S_CMD_SET);
	os_printf("ExtCSD.HPI_FEATURES          [503] = 0x%02X (%d)\n",
		chip->extcsd.HPI_FEATURES, chip->extcsd.HPI_FEATURES);
	os_printf("ExtCSD.BKOPS_SUPPORT         [502] = 0x%02X (%d)\n",
		chip->extcsd.BKOPS_SUPPORT, chip->extcsd.BKOPS_SUPPORT);
	os_printf("ExtCSD.MAX_PACKED_READS      [501] = 0x%02X (%d)\n",
		chip->extcsd.MAX_PACKED_READS,
		chip->extcsd.MAX_PACKED_READS);
	os_printf("ExtCSD.MAX_PACKED_WRITES     [500] = 0x%02X (%d)\n",
		chip->extcsd.MAX_PACKED_WRITES,
		chip->extcsd.MAX_PACKED_WRITES);
	os_printf("ExtCSD.DATA_TAG_SUPPORT      [499] = 0x%02X (%d)\n",
		chip->extcsd.DATA_TAG_SUPPORT,
		chip->extcsd.DATA_TAG_SUPPORT);
	os_printf("ExtCSD.TAG_UNIT_SIZE         [498] = 0x%02X (%d)\n",
		chip->extcsd.TAG_UNIT_SIZE, chip->extcsd.TAG_UNIT_SIZE);
	os_printf("ExtCSD.TAG_RES_SIZE          [497] = 0x%02X (%d)\n",
		chip->extcsd.TAG_RES_SIZE, chip->extcsd.TAG_RES_SIZE);
	os_printf("ExtCSD.CONTEXT_CAPABILITIES  [496] = 0x%02X (%d)\n",
		chip->extcsd.CONTEXT_CAPABILITIES,
		chip->extcsd.CONTEXT_CAPABILITIES);
	os_printf("ExtCSD.LARGE_UNIT_SIZE_M1    [495] = 0x%02X (%d)\n",
		chip->extcsd.LARGE_UNIT_SIZE_M1,
		chip->extcsd.LARGE_UNIT_SIZE_M1);
	os_printf("ExtCSD.EXT_SUPPORT           [494] = 0x%02X (%d)\n",
		chip->extcsd.EXT_SUPPORT, chip->extcsd.EXT_SUPPORT);
	os_printf("ExtCSD.CACHE_SIZE            [249] = 0x%08X (%d)\n",
		chip->extcsd.CACHE_SIZE, chip->extcsd.CACHE_SIZE);
	os_printf("ExtCSD.GENERIC_CMD6_TIME     [248] = 0x%02X (%d)\n",
		chip->extcsd.GENERIC_CMD6_TIME,
		chip->extcsd.GENERIC_CMD6_TIME);
	os_printf("ExtCSD.POWER_OFF_LONG_TIME   [247] = 0x%02X (%d)\n",
		chip->extcsd.POWER_OFF_LONG_TIME,
		chip->extcsd.POWER_OFF_LONG_TIME);
	os_printf("ExtCSD.BKOPS_STATUS          [246] = 0x%02X (%d)\n",
		chip->extcsd.BKOPS_STATUS, chip->extcsd.BKOPS_STATUS);
	os_printf("ExtCSD.CORRECTLY_PRG_SECTORS_NUM[242] = 0x%08X (%d)\n",
		chip->extcsd.CORRECTLY_PRG_SECTORS_NUM,
		chip->extcsd.CORRECTLY_PRG_SECTORS_NUM);
	os_printf("ExtCSD.INI_TIMEOUT_AP        [241] = 0x%02X (%d)\n",
		chip->extcsd.INI_TIMEOUT_AP, chip->extcsd.INI_TIMEOUT_AP);
	os_printf("ExtCSD.PWR_CL_DDR_52_360     [239] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_DDR_52_360,
		chip->extcsd.PWR_CL_DDR_52_360);
	os_printf("ExtCSD.PWR_CL_DDR_52_195     [238] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_DDR_52_195,
		chip->extcsd.PWR_CL_DDR_52_195);
	os_printf("ExtCSD.PWR_CL_200_360        [237] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_200_360, chip->extcsd.PWR_CL_200_360);
	os_printf("ExtCSD.PWR_CL_200_195        [236] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_200_195, chip->extcsd.PWR_CL_200_195);
	os_printf("ExtCSD.MIN_PERF_DDR_W_8_52   [235] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_DDR_W_8_52,
		chip->extcsd.MIN_PERF_DDR_W_8_52);
	os_printf("ExtCSD.MIN_PERF_DDR_R_8_52   [234] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_DDR_R_8_52,
		chip->extcsd.MIN_PERF_DDR_R_8_52);
	os_printf("ExtCSD.TRIM_MULT             [232] = 0x%02X (%d)\n",
		chip->extcsd.TRIM_MULT, chip->extcsd.TRIM_MULT);
	os_printf("ExtCSD.SEC_FEATURE_SUPPORT   [231] = 0x%02X (%d)\n",
		chip->extcsd.SEC_FEATURE_SUPPORT,
		chip->extcsd.SEC_FEATURE_SUPPORT);

	if (chip->extcsd.EXT_CSD_REV < JESD84_V45) { /* JESD84-B441 */
		os_printf("ExtCSD.ExtCSD_SEC_ERASE_MULT [232] = 0x%02X (%d)\n",
			chip->extcsd.SEC_ERASE_MULT,
			chip->extcsd.SEC_ERASE_MULT);
		os_printf("ExtCSD.ExtCSD_SEC_TRIM_MULT  [232] = 0x%02X (%d)\n",
			chip->extcsd.SEC_TRIM_MULT,
			chip->extcsd.SEC_TRIM_MULT);
	}

	os_printf("ExtCSD.BOOT_INFO             [228] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_INFO, chip->extcsd.BOOT_INFO);
	os_printf("ExtCSD.BOOT_SIZE_MULT        [226] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_SIZE_MULT, chip->extcsd.BOOT_SIZE_MULT);
	os_printf("ExtCSD.ACC_SIZE              [225] = 0x%02X (%d)\n",
		chip->extcsd.ACC_SIZE, chip->extcsd.ACC_SIZE);
	os_printf("ExtCSD.HC_ERASE_GRP_SIZE     [224] = 0x%02X (%d)\n",
		chip->extcsd.HC_ERASE_GRP_SIZE,
		chip->extcsd.HC_ERASE_GRP_SIZE);
	os_printf("ExtCSD.ERASE_TIMEOUT_MULT    [223] = 0x%02X (%d)\n",
		chip->extcsd.ERASE_TIMEOUT_MULT,
		chip->extcsd.ERASE_TIMEOUT_MULT);
	os_printf("ExtCSD.REL_WR_SEC_C          [222] = 0x%02X (%d)\n",
		chip->extcsd.REL_WR_SEC_C, chip->extcsd.REL_WR_SEC_C);
	os_printf("ExtCSD.HC_WP_GRP_SIZE        [221] = 0x%02X (%d)\n",
		chip->extcsd.HC_WP_GRP_SIZE, chip->extcsd.HC_WP_GRP_SIZE);
	os_printf("ExtCSD.S_C_VCC               [220] = 0x%02X (%d)\n",
		chip->extcsd.S_C_VCC, chip->extcsd.S_C_VCC);
	os_printf("ExtCSD.S_C_VCCQ              [219] = 0x%02X (%d)\n",
		chip->extcsd.S_C_VCCQ, chip->extcsd.S_C_VCCQ);
	os_printf("ExtCSD.S_A_TIMEOUT           [217] = 0x%02X (%d)\n",
		chip->extcsd.S_A_TIMEOUT, chip->extcsd.S_A_TIMEOUT);
	os_printf("ExtCSD.SEC_COUNT             [212] = 0x%08X (%d)\n",
		chip->extcsd.SEC_COUNT, chip->extcsd.SEC_COUNT);
	os_printf("ExtCSD.MIN_PERF_W_8_52       [210] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_W_8_52,
		chip->extcsd.MIN_PERF_W_8_52);
	os_printf("ExtCSD.MIN_PERF_R_8_52       [209] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_R_8_52,
		chip->extcsd.MIN_PERF_R_8_52);
	os_printf("ExtCSD.MIN_PERF_W_8_26_4_52  [208] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_W_8_26_4_52,
		chip->extcsd.MIN_PERF_W_8_26_4_52);
	os_printf("ExtCSD.MIN_PERF_R_8_26_4_52  [207] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_R_8_26_4_52,
		chip->extcsd.MIN_PERF_R_8_26_4_52);
	os_printf("ExtCSD.MIN_PERF_W_4_26       [206] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_W_4_26,
		chip->extcsd.MIN_PERF_W_4_26);
	os_printf("ExtCSD.MIN_PERF_R_4_26       [205] = 0x%02X (%d)\n",
		chip->extcsd.MIN_PERF_R_4_26,
		chip->extcsd.MIN_PERF_R_4_26);
	os_printf("ExtCSD.PWR_CL_26_360         [203] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_26_360, chip->extcsd.PWR_CL_26_360);
	os_printf("ExtCSD.PWR_CL_52_360         [202] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_52_360, chip->extcsd.PWR_CL_52_360);
	os_printf("ExtCSD.PWR_CL_26_195         [201] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_26_195, chip->extcsd.PWR_CL_26_195);
	os_printf("ExtCSD.PWR_CL_52_195         [200] = 0x%02X (%d)\n",
		chip->extcsd.PWR_CL_52_195, chip->extcsd.PWR_CL_52_195);
	os_printf("ExtCSD.PARTITION_SWITCH_TIME [199] = 0x%02X (%d)\n",
		chip->extcsd.PARTITION_SWITCH_TIME,
		chip->extcsd.PARTITION_SWITCH_TIME);
	os_printf("ExtCSD.OUT_OF_INTERRUPT_TIME [198] = 0x%02X (%d)\n",
		chip->extcsd.OUT_OF_INTERRUPT_TIME,
		chip->extcsd.OUT_OF_INTERRUPT_TIME);
	os_printf("ExtCSD.DRIVER_STRENGTH       [197] = 0x%02X (%d)\n",
		chip->extcsd.DRIVER_STRENGTH,
		chip->extcsd.DRIVER_STRENGTH);
	os_printf("ExtCSD.DEVICE_TYPE           [196] = 0x%02X (%d)\n",
		chip->extcsd.DEVICE_TYPE, chip->extcsd.DEVICE_TYPE);
	os_printf("ExtCSD.CSD_STRUCTURE         [194] = 0x%02X (%d)\n",
		chip->extcsd.CSD_STRUCTURE, chip->extcsd.CSD_STRUCTURE);
	os_printf("ExtCSD.EXT_CSD_REV           [192] = 0x%02X (%d)\n",
		chip->extcsd.EXT_CSD_REV, chip->extcsd.EXT_CSD_REV);
	/*  Modes Segment */
	os_printf("ExtCSD.CMD_SET               [191] = 0x%02X (%d)\n",
		chip->extcsd.CMD_SET, chip->extcsd.CMD_SET);
	os_printf("ExtCSD.CMD_SET_REV           [189] = 0x%02X (%d)\n",
		chip->extcsd.CMD_SET_REV, chip->extcsd.CMD_SET_REV);
	os_printf("ExtCSD.POWER_CLASS           [187] = 0x%02X (%d)\n",
		chip->extcsd.POWER_CLASS, chip->extcsd.POWER_CLASS);
	os_printf("ExtCSD.HS_TIMING             [185] = 0x%02X (%d)\n",
		chip->extcsd.HS_TIMING, chip->extcsd.HS_TIMING);
	os_printf("ExtCSD.BUS_WIDTH             [183] = 0x%02X (%d)\n",
		chip->extcsd.BUS_WIDTH, chip->extcsd.BUS_WIDTH);
	os_printf("ExtCSD.ERASED_MEM_CONT       [181] = 0x%02X (%d)\n",
		chip->extcsd.ERASED_MEM_CONT,
		chip->extcsd.ERASED_MEM_CONT);
	os_printf("ExtCSD.PARTITION_CONFIG      [179] = 0x%02X (%d)\n",
		chip->extcsd.PARTITION_CONFIG,
		chip->extcsd.PARTITION_CONFIG);
	os_printf("ExtCSD.BOOT_CONFIG_PROT      [178] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_CONFIG_PROT,
		chip->extcsd.BOOT_CONFIG_PROT);
	os_printf("ExtCSD.BOOT_BUS_CONDITIONS   [177] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_BUS_CONDITIONS,
		chip->extcsd.BOOT_BUS_CONDITIONS);
	os_printf("ExtCSD.ERASE_GROUP_DEF       [175] = 0x%02X (%d)\n",
		chip->extcsd.ERASE_GROUP_DEF,
		chip->extcsd.ERASE_GROUP_DEF);
	os_printf("ExtCSD.BOOT_WP_STATUS        [174] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_WP_STATUS, chip->extcsd.BOOT_WP_STATUS);
	os_printf("ExtCSD.BOOT_WP               [173] = 0x%02X (%d)\n",
		chip->extcsd.BOOT_WP, chip->extcsd.BOOT_WP);
	os_printf("ExtCSD.USER_WP               [171] = 0x%02X (%d)\n",
		chip->extcsd.USER_WP, chip->extcsd.USER_WP);
	os_printf("ExtCSD.FW_CONFIG             [169] = 0x%02X (%d)\n",
		chip->extcsd.FW_CONFIG, chip->extcsd.FW_CONFIG);
	os_printf("ExtCSD.RPMB_SIZE_MULT        [168] = 0x%02X (%d)\n",
		chip->extcsd.RPMB_SIZE_MULT, chip->extcsd.RPMB_SIZE_MULT);
	os_printf("ExtCSD.WR_REL_SET            [167] = 0x%02X (%d)\n",
		chip->extcsd.WR_REL_SET, chip->extcsd.WR_REL_SET);
	os_printf("ExtCSD.WR_REL_PARAM          [166] = 0x%02X (%d)\n",
		chip->extcsd.WR_REL_PARAM, chip->extcsd.WR_REL_PARAM);
	os_printf("ExtCSD.SANITIZE_START        [165] = 0x%02X (%d)\n",
		chip->extcsd.SANITIZE_START, chip->extcsd.SANITIZE_START);
	os_printf("ExtCSD.BKOPS_START           [164] = 0x%02X (%d)\n",
		chip->extcsd.BKOPS_START, chip->extcsd.BKOPS_START);
	os_printf("ExtCSD.BKOPS_EN              [163] = 0x%02X (%d)\n",
		chip->extcsd.BKOPS_EN, chip->extcsd.BKOPS_EN);
	os_printf("ExtCSD.RST_n_FUNCTION        [162] = 0x%02X (%d)\n",
		chip->extcsd.RST_n_FUNCTION, chip->extcsd.RST_n_FUNCTION);
	os_printf("ExtCSD.HPI_MGMT              [161] = 0x%02X (%d)\n",
		chip->extcsd.HPI_MGMT, chip->extcsd.HPI_MGMT);
	os_printf("ExtCSD.PARTITIONING_SUPPORT  [160] = 0x%02X (%d)\n",
		chip->extcsd.PARTITIONING_SUPPORT,
		chip->extcsd.PARTITIONING_SUPPORT);
	os_printf("ExtCSD.MAX_ENH_SIZE_MULT     [157] = 0x%08X (%d)\n",
		chip->extcsd.MAX_ENH_SIZE_MULT,
		chip->extcsd.MAX_ENH_SIZE_MULT);
	os_printf("ExtCSD.PARTITIONS_ATTRIBUTE  [156] = 0x%02X (%d)\n",
		chip->extcsd.PARTITIONS_ATTRIBUTE,
		chip->extcsd.PARTITIONS_ATTRIBUTE);
	os_printf("ExtCSD.PARTITION_SETTING_COMPLETED [155] = 0x%02X (%d)\n",
		chip->extcsd.PARTITION_SETTING_COMPLETED,
		chip->extcsd.PARTITION_SETTING_COMPLETED);
	os_printf("ExtCSD.GP_SIZE_MULT_4        [152] = 0x%08X (%d)\n",
		chip->extcsd.GP_SIZE_MULT[3],
		chip->extcsd.GP_SIZE_MULT[3]);
	os_printf("ExtCSD.GP_SIZE_MULT_3        [149] = 0x%08X (%d)\n",
		chip->extcsd.GP_SIZE_MULT[2],
		chip->extcsd.GP_SIZE_MULT[2]);
	os_printf("ExtCSD.GP_SIZE_MULT_2        [146] = 0x%08X (%d)\n",
		chip->extcsd.GP_SIZE_MULT[1],
		chip->extcsd.GP_SIZE_MULT[1]);
	os_printf("ExtCSD.GP_SIZE_MULT_1        [143] = 0x%08X (%d)\n",
		chip->extcsd.GP_SIZE_MULT[0],
		chip->extcsd.GP_SIZE_MULT[0]);
	os_printf("ExtCSD.ENH_SIZE_MULT         [140] = 0x%08X (%d)\n",
		chip->extcsd.ENH_SIZE_MULT, chip->extcsd.ENH_SIZE_MULT);
	os_printf("ExtCSD.ENH_START_ADDR        [136] = 0x%08X (%d)\n",
		chip->extcsd.ENH_START_ADDR, chip->extcsd.ENH_START_ADDR);
	os_printf("ExtCSD.SEC_BAD_BLK_MGMNT     [134] = 0x%02X (%d)\n",
		chip->extcsd.SEC_BAD_BLK_MGMNT,
		chip->extcsd.SEC_BAD_BLK_MGMNT);

	/* if V4.41 or older, we're done */
	if (chip->extcsd.EXT_CSD_REV <= JESD84_V441)
		return;

	/* V4.5 */
	os_printf("ExtCSD.TCASE_SUPPORT         [132] = 0x%02X (%d)\n",
		chip->extcsd.TCASE_SUPPORT,
		chip->extcsd.TCASE_SUPPORT);
	os_printf("ExtCSD.PERIODIC_WAKEUP       [131] = 0x%02X (%d)\n",
		chip->extcsd.PERIODIC_WAKEUP,
		chip->extcsd.PERIODIC_WAKEUP);
	os_printf("ExtCSD.PROGRAM_CID_CSD_DDR_SUPPORT [130] = 0x%02X (%d)\n",
		chip->extcsd.PROGRAM_CID_CSD_DDR_SUPPORT,
		chip->extcsd.PROGRAM_CID_CSD_DDR_SUPPORT);
	for (i = 0; i < 64; i++)
		os_printf("ExtCSD.VENDOR_SPECIFIC_FIELD       [%d] "
			"= 0x%02X (%d)\n",
			127 - i,
			chip->extcsd.VENDOR_SPECIFIC_FIELD[63 - i],
			chip->extcsd.VENDOR_SPECIFIC_FIELD[63 - i]);
	os_printf("ExtCSD.VENDOR_SPECIFIC_FIELD       [64] = 0x%02X (%d)\n",
		(unsigned int)chip->extcsd.VENDOR_SPECIFIC_FIELD,
		(int)chip->extcsd.VENDOR_SPECIFIC_FIELD);
	os_printf("ExtCSD.NATIVE_SECTOR_SIZE          [63] = 0x%02X (%d)\n",
		chip->extcsd.NATIVE_SECTOR_SIZE,
		chip->extcsd.NATIVE_SECTOR_SIZE);
	os_printf("ExtCSD.USE_NATIVE_SECTOR           [62] = 0x%02X (%d)\n",
		chip->extcsd.USE_NATIVE_SECTOR,
		chip->extcsd.USE_NATIVE_SECTOR);
	os_printf("ExtCSD.DATA_SECTOR_SIZE            [61] = 0x%02X (%d)\n",
		chip->extcsd.DATA_SECTOR_SIZE,
		chip->extcsd.DATA_SECTOR_SIZE);
	os_printf("ExtCSD.INI_TIMEOUT_EMU             [60] = 0x%02X (%d)\n",
		chip->extcsd.INI_TIMEOUT_EMU,
		chip->extcsd.INI_TIMEOUT_EMU);
	os_printf("ExtCSD.CLASS_6_CTRL                [59] = 0x%02X (%d)\n",
		chip->extcsd.CLASS_6_CTRL,
		chip->extcsd.CLASS_6_CTRL);
	os_printf("ExtCSD.DYNCAP_NEEDED               [58] = 0x%02X (%d)\n",
		chip->extcsd.DYNCAP_NEEDED,
		chip->extcsd.DYNCAP_NEEDED);
	os_printf("ExtCSD.EXCEPTION_EVENTS_CTRL       [56] = %016X (%d)\n",
		chip->extcsd.EXCEPTION_EVENTS_CTRL,
		chip->extcsd.EXCEPTION_EVENTS_CTRL);
	os_printf("ExtCSD.EXCEPTION_EVENTS_STATUS     [54] = %016X (%d)\n",
		chip->extcsd.EXCEPTION_EVENTS_STATUS,
		chip->extcsd.EXCEPTION_EVENTS_STATUS);
	os_printf("ExtCSD.EXT_PARTITIONS_ATTRIBUTE    [52] = %016X (%d)\n",
		chip->extcsd.EXT_PARTITIONS_ATTRIBUTE,
		chip->extcsd.EXT_PARTITIONS_ATTRIBUTE);
	for (i = 0; i < 15; i++)
		os_printf
		("ExtCSD.CONTEXT_CONF"
		 "                [%d] = %016X (%d)\n",
		 51 - i, chip->extcsd.CONTEXT_CONF[14 - i],
		 chip->extcsd.CONTEXT_CONF[14 - i]);
	os_printf("ExtCSD.PACKED_COMMAND_STATUS       [36] = 0x%08X (%d)\n",
		chip->extcsd.PACKED_COMMAND_STATUS,
		chip->extcsd.PACKED_COMMAND_STATUS);
	os_printf("ExtCSD.PACKED_FAILURE_INDEX        [35] = 0x%08X (%d)\n",
		chip->extcsd.PACKED_FAILURE_INDEX,
		chip->extcsd.PACKED_FAILURE_INDEX);
	os_printf("ExtCSD.POWER_OFF_NOTIFICATION      [34] = 0x%08X (%d)\n",
		chip->extcsd.POWER_OFF_NOTIFICATION,
		chip->extcsd.POWER_OFF_NOTIFICATION);
	os_printf("ExtCSD.CACHE_CTRL                  [33] = 0x%08X (%d)\n",
		chip->extcsd.CACHE_CTRL, chip->extcsd.CACHE_CTRL);
	os_printf("ExtCSD.FLUSH_CACHE                 [32] = 0x%08X (%d)\n",
		chip->extcsd.FLUSH_CACHE, chip->extcsd.FLUSH_CACHE);
}

static void emmc_print_host_emmc_info(struct emmc_chip *chip)
{
	uint32_t reg_ctrl_set0;
	uint32_t reg_ctrl_set1;
	uint32_t freq_div;
	uint32_t clk_freq;
	uint32_t mode_8bit;
	uint32_t mode_4bit;
	uint32_t bus_volt;
	uint32_t highspeed;
	uint32_t timeout;

	reg_ctrl_set0 = HOST_REG_RD(&chip->regs, CONTROL_0);
	reg_ctrl_set1 = HOST_REG_RD(&chip->regs, CONTROL_1);
	freq_div = HOST_GET_FIELD(reg_ctrl_set1, SDHC_CLKCTL_FREQ);
	if (freq_div == 0)
		clk_freq = 100000;
	else
		clk_freq = (uint32_t) (100000 / (2 * freq_div));

	mode_8bit = reg_ctrl_set0 & SDHC_CTL1_8BIT;
	mode_4bit = reg_ctrl_set0 & SDHC_CTL1_4BIT;
	bus_volt = HOST_GET_FIELD(reg_ctrl_set0, SDHC_BUS_VOLT);
	highspeed = reg_ctrl_set0 & SDHC_CTL1_HIGH_SPEED;
	timeout = HOST_GET_FIELD(reg_ctrl_set1, SDHC_TOCTL_TIMEOUT);

	os_printf("------------------------------\n");
	os_printf(" HOST CTRL_SET Information\n");
	os_printf("  Bus 8 Bit   : %s\n", mode_8bit ? "true" : "false");
	os_printf("  Bus 4 Bit   : %s\n", mode_4bit ? "true" : "false");
	os_printf("  Bus Voltage : %d (7=3.3volts, 6=3.0volt, 5=1.8volt)\n",
		bus_volt);
	os_printf("  Bus Speed   : %d (1=High Speed, 0=Normal Speed)\n",
		highspeed ? 1 : 0);
	os_printf("  Bus Freq.   : %d[KHz] (Base Clk=100MHz)\n", clk_freq);
	os_printf("  TIMEOUT_COUNT   : 2^%d\n", timeout);
	os_printf("  SDIO_1_HOST.CTRL_SET0 : 0x%08X\n", reg_ctrl_set0);
	os_printf("  SDIO_1_HOST.CTRL_SET1 : 0x%08X\n", reg_ctrl_set1);
	os_printf(" ------------------------------\n");
	os_printf(" eMMC CSD, ExtCSD Information\n");
	os_printf("  [Read Info]\n");
	os_printf("  Power Class (3.6V, 52MHz) : %d "
		"(0-15 : 100mA/200mA~>800mA/900mA (RMS/Max))\n",
		chip->extcsd.PWR_CL_52_360);
	os_printf("  Power Class (3.6V, 26MHz) : %d\n",
		chip->extcsd.PWR_CL_26_360);
	os_printf("  Driver Strength           : %d (v4.5)\n",
		chip->extcsd.DRIVER_STRENGTH);
	os_printf("  Device Type               : "
		"0x%02X (xxxx DDR_52M_Low DDR_52M_High SDR_52M SDR_26M)\n",
		chip->extcsd.DEVICE_TYPE);
	os_printf("  [Setup Info]\n");
	os_printf("  Power Class : %d "
		"(0-15 : 100mA/200mA~>800mA/900mA @3.6V(RMS/Max))\n",
		chip->extcsd.POWER_CLASS);
	os_printf("  HS Timing   : %d (0=Full Speed(~26MHz), "
		"1=High Speed(~52MHz,~104MHz(DDR)), 2=HS200)\n",
		chip->extcsd.HS_TIMING);
	os_printf("  Bus Width   : %d (0=1b, 1=4b, 2=8b, 5=4b(DDR), 6=4b(DDR)\n",
		chip->extcsd.BUS_WIDTH);
	os_printf("------------------------------\n\n");

}
#endif

static void decode_cid(struct emmc_chip *chip)
{
	uint32_t *raw_cid = &chip->cid.raw_cid[0];

	chip->cid.MID = (uint8_t)array_to_bits(raw_cid, 120, 8);
	chip->cid.CBX = (uint8_t)array_to_bits(raw_cid, 112, 2);
	chip->cid.OID = (uint8_t)array_to_bits(raw_cid, 104, 8);
	chip->cid.PNM[5] = (uint8_t)array_to_bits(raw_cid, 56 + (0 * 8), 8);
	chip->cid.PNM[4] = (uint8_t)array_to_bits(raw_cid, 56 + (1 * 8), 8);
	chip->cid.PNM[3] = (uint8_t)array_to_bits(raw_cid, 56 + (2 * 8), 8);
	chip->cid.PNM[2] = (uint8_t)array_to_bits(raw_cid, 56 + (3 * 8), 8);
	chip->cid.PNM[1] = (uint8_t)array_to_bits(raw_cid, 56 + (4 * 8), 8);
	chip->cid.PNM[0] = (uint8_t)array_to_bits(raw_cid, 56 + (5 * 8), 8);
	chip->cid.PRV = (uint8_t)array_to_bits(raw_cid, 48, 8);
	chip->cid.PSN = (uint8_t)array_to_bits(raw_cid, 16, 32);
	chip->cid.MDT = (uint8_t)array_to_bits(raw_cid, 8, 8);

#if DEBUG_EMMC_CONFIG
	os_printf(" CMD 2 response_67 CID[127: 96]: 0x%08X\n", raw_cid[3]);
	os_printf(" CMD 2 response_45 CSD[ 95: 64]: 0x%08X\n", raw_cid[2]);
	os_printf(" CMD 2 response_23 CSD[ 63: 32]: 0x%08X\n", raw_cid[1]);
	os_printf(" CMD 2 response_01 CSD[ 31:  0]: 0x%08X\n", raw_cid[0]);
	os_printf(" ManufactureId       : 0x%02X\n", chip->cid.MID);
	os_printf(" CardBGA             : %d (0:RemovableDevice, 1:BGA, "
		"2:POP, 3:RSVD)\n", chip->cid.CBX);
	os_printf(" OEMApplicationId    : 0x%02X\n", chip->cid.OID);
	os_printf(" ProductName         : %s\n", chip->cid.PNM);
	os_printf(" ProductRevision     : %d.%d\n",
		(chip->cid.PRV >> 4), (chip->cid.PRV & 0x0F));
	os_printf(" ProductSerialNumber : 0x%08X\n", chip->cid.PSN);
	os_printf(" ManufacturingDate   : %d/%d\n",
		(uint8_t) (chip->cid.MDT >> 4),
		(uint16_t) (chip->cid.MDT & 0x0F) + 1997);
#endif
}

static void decode_csd(struct emmc_chip *chip)
{
	struct emmcdev_csd *csd = &chip->csd;
	uint32_t *raw = &csd->raw_csd[0];

	csd->CSD_STRUCTURE = array_to_bits(raw, 126, 2);
	csd->SPEC_VERS = array_to_bits(raw, 122, 4);
	csd->TAAC = array_to_bits(raw, 112, 8);
	csd->NSAC = array_to_bits(raw, 104, 8);
	csd->TRAN_SPEED = array_to_bits(raw, 96, 8);
	csd->CCC = array_to_bits(raw, 84, 12);
	csd->READ_BL_LEN = array_to_bits(raw, 80, 4);
	csd->READ_BL_PARTIAL = array_to_bits(raw, 79, 1);
	csd->WRITE_BLK_MISALIGN = array_to_bits(raw, 78, 1);
	csd->READ_BLK_MISALIGN = array_to_bits(raw, 77, 1);
	csd->DSR_IMP = array_to_bits(raw, 76, 1);
	csd->C_SIZE = array_to_bits(raw, 62, 12);
	csd->VDD_R_CURR_MIN = array_to_bits(raw, 59, 3);
	csd->VDD_R_CURR_MAX = array_to_bits(raw, 56, 3);
	csd->VDD_W_CURR_MIN = array_to_bits(raw, 53, 3);
	csd->VDD_W_CURR_MAX = array_to_bits(raw, 50, 3);
	csd->C_SIZE_MULT = array_to_bits(raw, 47, 3);
	csd->ERASE_GRP_SIZE = array_to_bits(raw, 42, 5);
	csd->ERASE_GRP_MULT = array_to_bits(raw, 37, 5);
	csd->WP_GRP_SIZE = array_to_bits(raw, 32, 5);
	csd->WP_GRP_ENABLE = array_to_bits(raw, 31, 1);
	csd->DEFAULT_ECC = array_to_bits(raw, 29, 2);
	csd->R2W_FACTOR = array_to_bits(raw, 26, 3);
	csd->WRITE_BL_LEN = array_to_bits(raw, 22, 4);
	csd->WRITE_BL_PARTIAL = array_to_bits(raw, 21, 1);
	csd->CONTENT_PROT_APP = array_to_bits(raw, 16, 1);
	csd->FILE_FORMAT_GRP = array_to_bits(raw, 15, 1);
	csd->COPY = array_to_bits(raw, 14, 1);
	csd->PERM_WRITE_PROTECT = array_to_bits(raw, 13, 1);
	csd->TMP_WRITE_PROTECT = array_to_bits(raw, 12, 1);
	csd->FILE_FORMAT = array_to_bits(raw, 10, 2);
	csd->ECC = array_to_bits(raw, 8, 2);

#if DEBUG_EMMC_CONFIG
	os_printf(" CMD 9 response_67 CSD[127: 96]: 0x%08X\n",
		csd->raw_csd[3]);
	os_printf(" CMD 9 response_45 CSD[ 95: 64]: 0x%08X\n",
		csd->raw_csd[2]);
	os_printf(" CMD 9 response_23 CSD[ 63: 32]: 0x%08X\n",
		csd->raw_csd[1]);
	os_printf(" CMD 9 response_01 CSD[ 31:  0]: 0x%08X\n",
		csd->raw_csd[0]);
	os_printf(" CSD_CSD_STRUCTURE      [127:126] = 0x%02X (%d)\n",
		csd->CSD_STRUCTURE, csd->CSD_STRUCTURE);
	os_printf(" CSD_SPEC_VERS          [125:122] = 0x%02X (%d)\n",
		csd->SPEC_VERS, csd->SPEC_VERS);
	os_printf(" CSD_TAAC               [119:112] = 0x%02X (%d)\n",
		csd->TAAC, csd->TAAC);
	os_printf(" CSD_NSAC               [111:104] = 0x%02X (%d)\n",
		csd->NSAC, csd->NSAC);
	os_printf(" CSD_TRAN_SPEED         [103: 96] = 0x%02X (%d)\n",
		csd->TRAN_SPEED, csd->TRAN_SPEED);
	os_printf(" CSD_CCC                [ 95: 84] = 0x%02X (%d)\n",
		csd->CCC, csd->CCC);
	os_printf(" CSD_READ_BL_LEN        [ 83: 80] = 0x%02X (%d)\n",
		csd->READ_BL_LEN, csd->READ_BL_LEN);
	os_printf(" CSD_READ_BL_PARTIAL    [ 79: 79] = 0x%02X (%d)\n",
		csd->READ_BL_PARTIAL,
		csd->READ_BL_PARTIAL);
	os_printf(" CSD_WRITE_BLK_MISALIGN [ 78: 78] = 0x%02X (%d)\n",
		csd->WRITE_BLK_MISALIGN,
		csd->WRITE_BLK_MISALIGN);
	os_printf(" CSD_READ_BLK_MISALIGN  [ 77: 77] = 0x%02X (%d)\n",
		csd->READ_BLK_MISALIGN,
		csd->READ_BLK_MISALIGN);
	os_printf(" CSD_DSR_IMP            [ 76: 76] = 0x%02X (%d)\n",
		csd->DSR_IMP, csd->DSR_IMP);
	os_printf(" CSD_C_SIZE             [ 73: 62] = 0x%02X (%d)\n",
		csd->C_SIZE, csd->C_SIZE);
	os_printf(" CSD_VDD_R_CURR_MIN     [ 61: 59] = 0x%02X (%d)\n",
		csd->VDD_R_CURR_MIN,
		csd->VDD_R_CURR_MIN);
	os_printf(" CSD_VDD_R_CURR_MAX     [ 58: 56] = 0x%02X (%d)\n",
		csd->VDD_R_CURR_MAX,
		csd->VDD_R_CURR_MAX);
	os_printf(" raw_csd[3]_W_CURR_MIN  [ 55: 53] = 0x%02X (%d)\n",
		csd->VDD_W_CURR_MIN,
		csd->VDD_W_CURR_MAX);
	os_printf(" CSD_VDD_W_CURR_MAX     [ 52: 50] = 0x%02X (%d)\n",
		csd->VDD_W_CURR_MAX,
		csd->VDD_W_CURR_MAX);
	os_printf(" CSD_C_SIZE_MULT        [ 49: 47] = 0x%02X (%d)\n",
		csd->C_SIZE_MULT, csd->C_SIZE_MULT);
	os_printf(" CSD_ERASE_GRP_SIZE     [ 46: 42] = 0x%02X (%d)\n",
		csd->ERASE_GRP_SIZE,
		csd->ERASE_GRP_SIZE);
	os_printf(" CSD_ERASE_GRP_MULT     [ 41: 37] = 0x%02X (%d)\n",
		csd->ERASE_GRP_MULT,
		csd->ERASE_GRP_MULT);
	os_printf(" CSD_WP_GRP_SIZE        [ 36: 32] = 0x%02X (%d)\n",
		csd->WP_GRP_SIZE, csd->WP_GRP_SIZE);
	os_printf(" CSD_WP_GRP_ENABLE      [ 31: 31] = 0x%02X (%d)\n",
		csd->WP_GRP_ENABLE, csd->WP_GRP_ENABLE);
	os_printf(" CSD_DEFAULT_ECC        [ 30: 29] = 0x%02X (%d)\n",
		csd->DEFAULT_ECC, csd->DEFAULT_ECC);
	os_printf(" CSD_R2W_FACTOR         [ 28: 26] = 0x%02X (%d)\n",
		csd->R2W_FACTOR, csd->R2W_FACTOR);
	os_printf(" CSD_WRITE_BL_LEN       [ 25: 22] = 0x%02X (%d)\n",
		csd->WRITE_BL_LEN, csd->WRITE_BL_LEN);
	os_printf(" CSD_WRITE_BL_PARTIAL   [ 21: 21] = 0x%02X (%d)\n",
		csd->WRITE_BL_PARTIAL,
		csd->WRITE_BL_PARTIAL);
	os_printf(" CSD_CONTENT_PROT_APP   [ 16: 16] = 0x%02X (%d)\n",
		csd->CONTENT_PROT_APP,
		csd->CONTENT_PROT_APP);
	os_printf(" CSD_FILE_FORMAT_GRP    [ 15: 15] = 0x%02X (%d)\n",
		csd->FILE_FORMAT_GRP,
		csd->FILE_FORMAT_GRP);
	os_printf(" CSD_COPY               [ 14: 14] = 0x%02X (%d)\n",
		csd->COPY, csd->COPY);
	os_printf(" CSD_PERM_WRITE_PROTECT [ 13: 13] = 0x%02X (%d)\n",
		csd->PERM_WRITE_PROTECT,
		csd->PERM_WRITE_PROTECT);
	os_printf(" CSD_TMP_WRITE_PROTECT  [ 12: 12] = 0x%02X (%d)\n",
		csd->TMP_WRITE_PROTECT,
		csd->TMP_WRITE_PROTECT);
	os_printf(" CSD_FILE_FORMAT        [ 11: 10] = 0x%02X (%d)\n",
		csd->FILE_FORMAT, csd->FILE_FORMAT);
	os_printf(" CSD_ECC                [  9:  8] = 0x%02X (%d)\n",
		csd->ECC, csd->ECC);
	os_printf("\n");
#endif
}

/*
 * R2 responses, taken from the response registers, are missing the first
 * 8 bits because the first 8 bits are the checksum which is checked
 * in the hardware and discarded. This routine will shift all the bits
 * into the position that matches the spec.
 */
static void fix_r2_buffer(uint32_t *buf)
{
	buf[3] = (buf[3] << 8) + (buf[2] >> 24);
	buf[2] = (buf[2] << 8) + (buf[1] >> 24);
	buf[1] = (buf[1] << 8) + (buf[0] >> 24);
	buf[0] = buf[0] << 8;
}

static void handle_interrupt_error(struct emmc_chip *chip)
{
	uint32_t status;

	status = HOST_REG_RD(&chip->regs, INTR_STAT);
	if (!chip->probing)
		err_msg("%s Command error, interrupt status: %#x",
			chip->regs.name, status);
	HOST_REG_WR(&chip->regs, INTR_STAT, status);
}

static int wait_cmd_complete(struct emmc_chip *chip)
{
	uint32_t timeout_cnt = 0;
	uint32_t int_status;

	/* Wait for COMMAND COMPLETE or error interrupt */
	timeout_cnt = 0;
	while (++timeout_cnt <= (CMD_TIMEOUT_USECS / POLL_DELAY_USECS)) {
		int_status = HOST_REG_RD(&chip->regs, INTR_STAT) &
			(SDHC_INT_CMD_COMPLETE | SDHC_INT_ERR);
		if (int_status)
			break;
		os_usleep(POLL_DELAY_USECS);
	}
	if (timeout_cnt > CMD_SLEEP_CNT) {
		if (!chip->probing)
			err_msg("%s timed out waiting for cmd complete, "
				"intr= %#x (timeout_cnt=%d)",
				chip->regs.name, int_status,
				timeout_cnt);
		return 1;
	}

	/* Check for clean completion */
	if (int_status & SDHC_INT_ERR) {
		handle_interrupt_error(chip);
		return 1;
	}

	/* Clear the interrupt for next time */
	emmc_clear_int_status(chip, int_status);

	DBG_MSG_CMD(" %s OK (intr=%#x timeout_cnt=%d)\n",
		    __func__, int_status, timeout_cnt);
	return 0;
}

static void pio_read_blk(struct emmc_chip *chip, int block_size, uint32_t *ptr)
{
	int cnt = block_size / sizeof(uint32_t);
	uint32_t val;

	while (cnt--) {
		val = HOST_REG_RD(&chip->regs, BUFFER_PORT);
		*ptr++ = le32_to_cpu(val);
	}
}

static void pio_write_blk(struct emmc_chip *chip, int block_size, uint32_t *ptr)
{
	int cnt = block_size / sizeof(uint32_t);
	uint32_t val;

	while (cnt--) {
		val = *ptr++;
		HOST_REG_WR(&chip->regs, BUFFER_PORT, cpu_to_le32(val));
	}
}



static void pio_transfer(struct emmc_chip *chip, int do_read,
			struct mmc_data *data)
{
	int state;

	while (data->block_count) {
		if (do_read)
			state = SDHC_STATE_BUF_READ_RDY;
		else
			state = SDHC_STATE_BUF_WRITE_RDY;
		if ((HOST_REG_RD(&chip->regs, STATE) & state) == 0)
			break;
		if (do_read)
			pio_read_blk(chip, data->block_size,
				(uint32_t *)data->dma_address);
		else
			pio_write_blk(chip, data->block_size,
				(uint32_t *)data->dma_address);
		data->dma_address += data->block_size;
		data->block_count--;
	}
}


static int wait_xfer_complete(struct emmc_chip *chip, struct mmc_data *data)
{
	uint32_t int_status;
	uint32_t timeout = 0;

	while (1) {
		int_status = HOST_REG_RD(&chip->regs, INTR_STAT);

		/* Clear interrupts */
		HOST_REG_WR(&chip->regs, INTR_STAT, int_status);

		if (int_status & SDHC_INT_XFER_COMPLETE)
			break;
		if (int_status & SDHC_INT_XFER_DMA) {
			DBG_MSG_CMD(" %s: DMA interrupt, SDMA addr: %#x\n",
				    __func__,
				    HOST_REG_RD(&chip->regs, SDMA));
			/*
			 * write the address back to the address
			 * to restart the DMA.
			 */
			HOST_REG_WR(&chip->regs, SDMA,
				HOST_REG_RD(&chip->regs, SDMA));
		}
		if (int_status & SDHC_INT_ERR_MASK) {
			err_msg("%s transfer complete intr error, intr=%#x",
				chip->regs.name, int_status);
			return 1;
		}
		if (int_status & (SDHC_INT_WRITE_BUF | SDHC_INT_READ_BUF)) {
			pio_transfer(chip,
				(int_status & SDHC_INT_READ_BUF) ? 1 : 0,
				data);
			continue;
		}
		if (++timeout >= DATA_SLEEP_CNT) {
			err_msg("%s transfer complete timed out, intr: %#x",
				chip->regs.name, int_status);
			return 1;
		}
		os_usleep(POLL_DELAY_USECS);
	}

	DBG_MSG_CMD(" %s OK: (intr=%#x timeout=%d)\n",
		    __func__, int_status, timeout);
	return 0;

}

static int wait_ready_data_xfer(struct emmc_chip *chip)
{
	uint32_t emmc_status = 0;
	uint32_t emmc_state = 0;
	uint32_t timeout_cnt = 0;
	uint32_t cst = 0;

	do {
		if (emmc_cmd13_send_status(chip, &cst))
			return 1;
		emmc_status = (cst & CST_READY_FOR_DATA_MASK) >>
			CST_READY_FOR_DATA_SHIFT;
		emmc_state = (cst & CST_CURRENT_STATE_MASK) >>
			CST_CURRENT_STATE_SHIFT;
		if ((emmc_status == 1) && (emmc_state == CST_STATE_TRAN))
			break;
		os_usleep(POLL_DELAY_USECS);
		timeout_cnt++;
	} while (timeout_cnt < DATA_SLEEP_CNT);
	if (timeout_cnt >= DATA_SLEEP_CNT) {
		err_msg("%s Timeout waiting for data xfer ready, "
			"CST: %#x (timeout_cnt=%d)",
			chip->regs.name, cst, timeout_cnt);
		return 1;
	}
	return 0;
}

static uint32_t get_cmd_mode(struct emmc_chip *chip, int index)

{
	uint32_t cmd_mode = 0;
	const struct cmd_info *entry = &emmc_cmd_table[index];

	HOST_SET_FIELD(cmd_mode, SDHC_CMD_INDEX, index);
	switch (entry->resp) {
	case CMDINFO_RESP_R1:
	case CMDINFO_RESP_R5:
	case CMDINFO_RESP_R6:
	case CMDINFO_RESP_R7:
		cmd_mode |= (SDHC_CMD_CHK_INDEX |
			     SDHC_CMD_CHK_CRC |
			     SDHC_CMD_RESP_48);
		break;
	case CMDINFO_RESP_R2:
		cmd_mode |= (SDHC_CMD_CHK_CRC |
			     SDHC_CMD_RESP_136);
		break;
	case CMDINFO_RESP_R3:
	case CMDINFO_RESP_R4:
		cmd_mode |= SDHC_CMD_RESP_48;
		break;
	case CMDINFO_RESP_R1B:
		cmd_mode |= (SDHC_CMD_CHK_INDEX |
			     SDHC_CMD_CHK_CRC |
			     SDHC_CMD_RESP_48_BSY);
		break;
	}

	/* Setup for transfer on DATA lines */
	if (entry->data_xfer != CMDINFO_DXFER_NONE) {
		cmd_mode |= SDHC_CMD_DATA;
		if (chip->host_config.disable_dma == 0)
			cmd_mode |= SDHC_MODE_DMA;
	}

	/*
	 * If multiblock, set multi bit, enable block cnt register
	 * and enable Auto CMD12
	 */
	if ((entry->data_xfer == CMDINFO_DXFER_READ_MULTI) ||
	    (entry->data_xfer == CMDINFO_DXFER_WRITE_MULTI)) {
		/* if the last command was SET_BLOCK_COUNT, don't use ACMD12 */
		if (chip->last_cmd == 23)
			cmd_mode |= (SDHC_MODE_MULTI  | SDHC_MODE_BLK_CNT);
		else
			cmd_mode |= (SDHC_MODE_MULTI | SDHC_MODE_ACMD12 |
				SDHC_MODE_BLK_CNT);
	}

	/* bit is zero for write, 1 for read */
	if ((entry->data_xfer == CMDINFO_DXFER_READ) ||
	    (entry->data_xfer == CMDINFO_DXFER_READ_MULTI))
		cmd_mode |= SDHC_MODE_XFER_DIR_READ;
	chip->last_cmd = index;
	return cmd_mode;
}

static int wait_inhibit_clear(struct emmc_chip *chip, int data_inhibit)
{
	int mask = SDHC_STATE_CMD_INHIBIT;
	int timeout;

	/*
	 * If new command has a data transfer or is using BUSY signaling,
	 * also wait for DATA INHIBIT to clear.
	 */
	if (data_inhibit) {
		timeout = DATA_SLEEP_CNT;
		mask |= SDHC_STATE_CMD_INHIBIT_DAT;
	} else {
		timeout = CMD_SLEEP_CNT;
	}
	while (1) {
		if ((HOST_REG_RD(&chip->regs, STATE) & mask) == 0)
			break;
		if (timeout-- == 0)
			break;
		os_usleep(POLL_DELAY_USECS);
	}
	if (timeout < 0) {
		err_msg("%s Timeout waiting for INHIBIT lines to clear: 0x%x",
			chip->regs.name,
			HOST_REG_RD(&chip->regs, STATE) & mask);
		return 1;
	}
	return 0;
}

static int emmc_software_reset(struct emmc_chip *chip, uint32_t mask)
{
	uint32_t reg_ctrl_set1;
	int cnt;

	reg_ctrl_set1 = HOST_REG_RD(&chip->regs, CONTROL_1);
	HOST_REG_WR(&chip->regs, CONTROL_1, reg_ctrl_set1 | mask);
	cnt = SOFTWARE_RESET_TIMEOUT_USECS;
	while (cnt > 0) {
		os_usleep(POLL_DELAY_USECS);
		reg_ctrl_set1 = HOST_REG_RD(&chip->regs, CONTROL_1) & mask;
		if (reg_ctrl_set1 == 0)
			return 0;
		cnt -= POLL_DELAY_USECS;
	}
	err_msg("Host RESET timeout waiting for 0x%x to clear", reg_ctrl_set1);
	return 1;
}


static int issue_cmd(struct emmc_chip *chip,
		     struct mmc_cmd *cmd,
		     struct mmc_data *data)
{
	uint32_t cmd_mode;
	const struct cmd_info *entry = &emmc_cmd_table[cmd->index];
	int data_xfer;

	data_xfer = (entry->data_xfer != CMDINFO_DXFER_NONE);
	DBG_MSG_CMD("\nCMD %d (%s), arg: %#x",
		    cmd->index, entry->desc, cmd->arg);

	if (data) {
		DBG_MSG_CMD(" block size: %d, blocks: %d, address: %#x",
			    data->block_size, data->block_count,
			    data->dma_address);
	}
	DBG_MSG_CMD("\n");

	/* sanity check */
	if ((data != NULL) ^ (data_xfer)) {
		err_msg("%s %s - data phase mismatch", chip->regs.name,
			entry->desc);
		goto err;
	}
	if (wait_inhibit_clear(chip, data_xfer))
		goto err;

	/* Start with all interrupts cleared */
	emmc_clear_int_status(chip, HOST_INT_STATUS_ALL);

	/* The command includes a data transfer */
	if (data_xfer) {
		HOST_REG_WR(&chip->regs, BLKCNT_BLKSIZE,
			SDHC_MAKE_BLK_REG(data->block_count,
					data->block_size, 7));
		if (chip->host_config.disable_dma) {
			emmc_enable_int(chip, HOST_INT_ENABLES_PIO);
		} else {
			HOST_REG_WR(&chip->regs, SDMA,
				PHYSADDR(data->dma_address));
			CACHE_FLUSH_RANGE((void *)data->dma_address,
					data->block_size * data->block_count);
			emmc_enable_int(chip, HOST_INT_ENABLES_DMA);
		}
	} else {
		emmc_enable_int(chip, HOST_INT_ENABLES);
	}

	cmd_mode = get_cmd_mode(chip, cmd->index);
	HOST_REG_WR(&chip->regs, ARGUMENT, cmd->arg);

	DBG_MSG_CMD(" SDMA: %#x, BLOCK: %#x, ARG: %#x CMD: %#x\n",
		    HOST_REG_RD(&chip->regs, SDMA),
		    HOST_REG_RD(&chip->regs, BLKCNT_BLKSIZE),
		    HOST_REG_RD(&chip->regs, ARGUMENT),
		    cmd_mode);

	HOST_REG_WR(&chip->regs, CMD_MODE, cmd_mode);
	if (wait_cmd_complete(chip))
		goto err;

	/* If the command has a data transfer, wait for transfer complete */
	if (data_xfer) {
		if (wait_xfer_complete(chip, data))
			goto err;
		if (!chip->host_config.disable_dma &&
			((entry->data_xfer == CMDINFO_DXFER_READ) ||
				(entry->data_xfer == CMDINFO_DXFER_READ_MULTI)))
			CACHE_INVAL_RANGE((void *)data->dma_address,
					data->block_size * data->block_count);
	}
	/*
	 * If command is using BUSY signaling, also wait for DATA INHIBIT
	 * to clear.
	 */
	if (emmc_cmd_table[cmd->index].resp == CMDINFO_RESP_R1B)
		wait_inhibit_clear(chip, 1);

	return 0;

err:
	if (!chip->probing)
		err_msg("Command %d failed", cmd->index);
	emmc_software_reset(chip, SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD);
	return 1;
}

#if DEBUG_EMMC_STATUS
static void emmc_print_card_status(uint32_t status)
{
	os_printf(" CMD 13 resp(Status): 0x%08X\n", status);
	os_printf(" ----------------------------------------\n");
	os_printf(" CST_ADDRESS_OUT_OF_RANGE [    31] = %d\n",
		(status & CST_ADDRESS_OUT_OF_RANGE_MASK) >>
		CST_ADDRESS_OUT_OF_RANGE_SHIFT);
	os_printf(" CST_ADDRESS_MISALIGN     [    30] = %d\n",
		(status & CST_ADDRESS_MISALIGN_MASK) >>
		CST_ADDRESS_MISALIGN_SHIFT);
	os_printf(" CST_BLOCK_LEN_ERROR      [    29] = %d\n",
		(status & CST_BLOCK_LEN_ERROR_MASK) >>
		CST_BLOCK_LEN_ERROR_SHIFT);
	os_printf(" CST_ERASE_SEQ_ERROR      [    28] = %d\n",
		(status & CST_ERASE_SEQ_ERROR_MASK) >>
		CST_ERASE_SEQ_ERROR_SHIFT);
	os_printf(" CST_ERASE_PARAM          [    27] = %d\n",
		(status & CST_ERASE_PARAM_MASK) >>
		CST_ERASE_PARAM_SHIFT);
	os_printf(" CST_WP_VIOLATION         [    26] = %d\n",
		(status & CST_WP_VIOLATION_MASK) >>
		CST_WP_VIOLATION_SHIFT);
	os_printf(" CST_DEVICE_IS_LOCKED     [    25] = %d\n",
		(status & CST_DEVICE_IS_LOCKED_MASK) >>
		CST_DEVICE_IS_LOCKED_SHIFT);
	os_printf(" CST_LOCK_UNLOCK_FAILED   [    24] = %d\n",
		(status & CST_LOCK_UNLOCK_FAILED_MASK) >>
		CST_LOCK_UNLOCK_FAILED_SHIFT);
	os_printf(" CST_COM_CRC_ERROR        [    23] = %d\n",
		(status & CST_COM_CRC_ERROR_MASK) >>
		CST_COM_CRC_ERROR_SHIFT);
	os_printf(" CST_ILLEGAL_COMMAND      [    22] = %d\n",
		(status & CST_ILLEGAL_COMMAND_MASK) >>
		CST_ILLEGAL_COMMAND_SHIFT);
	os_printf(" CST_DEVICE_ECC_FAILED    [    21] = %d\n",
		(status & CST_DEVICE_ECC_FAILED_MASK) >>
		CST_DEVICE_ECC_FAILED_SHIFT);
	os_printf(" CST_CC_ERROR             [    20] = %d\n",
		(status & CST_CC_ERROR_MASK) >>
		CST_CC_ERROR_SHIFT);
	os_printf(" CST_ERROR                [    19] = %d\n",
		(status & CST_ERROR_MASK) >> CST_ERROR_SHIFT);
	os_printf(" CST_CID_CSD_OVERWRITE    [    16] = %d\n",
		(status & CST_CID_CSD_OVERWRITE_MASK) >>
		CST_CID_CSD_OVERWRITE_SHIFT);
	os_printf(" CST_WP_ERASE_SKIP        [    15] = %d\n",
		(status & CST_WP_ERASE_SKIP_MASK) >>
		CST_WP_ERASE_SKIP_SHIFT);
	os_printf(" CST_ERASE_RESET          [    13] = %d\n",
		(status & CST_ERASE_RESET_MASK) >>
		CST_ERASE_RESET_SHIFT);
	os_printf(" CST_CURRENT_STATE        [12 : 9] = %d\n",
		(status & CST_CURRENT_STATE_MASK) >>
		CST_CURRENT_STATE_SHIFT);
	os_printf("  ( 0:Idle  1:Ready  2:Ident  3:Stby  4:Tran  5:Data )\n");
	os_printf("  ( 6:Rcv   7:Prg    8:Dis    9:Btst  10:Slp )\n");
	os_printf(" CST_READY_FOR_DATA       [     8] = %d\n",
		(status & CST_READY_FOR_DATA_MASK) >>
		CST_READY_FOR_DATA_SHIFT);
	os_printf(" CST_SWITCH_ERROR         [     7] = %d\n",
		(status & CST_SWITCH_ERROR_MASK) >>
		CST_SWITCH_ERROR_SHIFT);
	os_printf(" CST_EXCEPTION_EVENT      [     6] = %d\n",
		(status & CST_EXCEPTION_EVENT_MASK) >>
		CST_EXCEPTION_EVENT_SHIFT);
	os_printf(" CST_APP_CMD              [     5] = %d\n",
		(status & CST_APP_CMD_MASK) >> CST_APP_CMD_SHIFT);
	os_printf("\n");
}
#endif

static int emmc_cmd0(struct emmc_chip *chip, uint32_t arg)
{
	struct mmc_cmd cmd;

	/*
	 * CMD 0 - GO_IDLE_STATE (bc,-), GO_PRE_IDLE_STATE (bc,-),
	 * BOOT_INITIATION (-,-)
	 */
	cmd.index = 0;
	cmd.arg = arg;
	return issue_cmd(chip, &cmd, NULL);
}

static int emmc_cmd0_go_idle_state(struct emmc_chip *chip)
{
	int res;

	res = emmc_cmd0(chip, 0x00000000);

	/* wait for card to settle */
	os_msleep(1);
	return res;
}

/*
 * Get the card in idle state To send its Operating Conditions
 * Register (OCR) contents in the response
 */
static int emmc_cmd1_send_op_cond(struct emmc_chip *chip, uint32_t arg,
				  uint32_t *ocr)
{
	struct mmc_cmd cmd;
	int res;

	cmd.index = 1;
	cmd.arg = arg;
	res = issue_cmd(chip, &cmd, NULL);
	*ocr = HOST_REG_RD(&chip->regs, RESPONSE_01);
	return res;
}

/*
 * Gets card CID (Card Identification) from eMMC device.
 */
static int  emmc_cmd2_all_send_cid_single_device(struct emmc_chip *chip)
{
	struct mmc_cmd cmd;
	int res;
	uint32_t *raw_cid = &chip->cid.raw_cid[0];

	cmd.index = 2;
	cmd.arg = 0;
	res = issue_cmd(chip, &cmd, NULL);
	if (res)
		return res;
	raw_cid[0] = HOST_REG_RD(&chip->regs, RESPONSE_01);
	raw_cid[1] = HOST_REG_RD(&chip->regs, RESPONSE_23);
	raw_cid[2] = HOST_REG_RD(&chip->regs, RESPONSE_45);
	raw_cid[3] = HOST_REG_RD(&chip->regs, RESPONSE_67);
	fix_r2_buffer(raw_cid);
	decode_cid(chip);
	return res;

}

/*
 * Set relative card address
 */
static int emmc_cmd3_set_rca(struct emmc_chip *chip, uint32_t arg)
{
	struct mmc_cmd cmd;

	/* CMD3, R1 (Index check  On, CRC check	 On, 48bit) */
	cmd.index = 3;
	cmd.arg = arg;
	return issue_cmd(chip, &cmd, NULL);
}

static int cmd6_switch_extcsd(struct emmc_chip *chip,
			      uint32_t access,
			      uint32_t index,
			      uint32_t val)
{
	struct mmc_cmd cmd;
	uint32_t arg;
	uint32_t status = 0;
	int res;
	uint32_t emmc_state;
	uint32_t timeout_cnt = 0;

	arg = 0;
	arg = (access & 0x03) << 24;
	arg = arg + ((index & 0xFF) << 16);
	arg = arg + ((val & 0xFF) << 8);

	/* CMD6 - R1b(Index check  On, CRC check  On, 48bit/busy_check) */
	cmd.index = 6;
	cmd.arg = arg;
	res = issue_cmd(chip, &cmd, NULL);
	if (res)
		return res;

	/* Wait until we transition from PRG to TRAN state */
	do {
		if (emmc_cmd13_send_status(chip, &status))
			return 1;
		if (status & CST_SWITCH_ERROR_MASK) {
			err_msg("%s CMD 6 - SWITCH error in status: %#x",
				chip->regs.name, status);
			return 1;
		}
		emmc_state = (status & CST_CURRENT_STATE_MASK) >>
			CST_CURRENT_STATE_SHIFT;
		if (emmc_state == CST_STATE_TRAN)
			break;
		os_usleep(POLL_DELAY_USECS);
		timeout_cnt++;
	} while (timeout_cnt < DATA_SLEEP_CNT);
	if (timeout_cnt >= DATA_SLEEP_CNT) {
		err_msg("%s Timeout waiting for TRAN state after SWITCH cmd, "
			"STATUS: %#x (timeout_cnt=%d)",
			chip->regs.name, status, timeout_cnt);
		return 1;
	}
	return 0;
}

static int emmc_cmd7_select_card_stby_tans(struct emmc_chip *chip,
		uint32_t arg)
{
	struct mmc_cmd cmd;

	/*
	 * CMD7
	 * While selecting from Stand-By State to Transfer State
	 *   R1 (Index check  On, CRC check  On, 48bit)
	 * While selecting from Disconnected State to Programming State.
	 *   R1b(Index check  On, CRC check  On, 48bit/busy_check)
	 */
	cmd.index = 7;
	cmd.arg = arg;
	return issue_cmd(chip, &cmd, NULL);
}

static int emmc_cmd8_send_ext_csd(struct emmc_chip *chip)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	unsigned char *buf;
	int res;

	cmd.index = 8;
	cmd.arg = 0;

	buf = KMALLOC(EMMC_BLOCKSIZE, EMMC_DMA_BUF_ALIGN);
	if (!buf)
		return 1;
	memset(buf, 0, EMMC_BLOCKSIZE);
	data.dma_address = (uint32_t)buf;
	data.block_size = 512;
	data.block_count = 1;
	res = issue_cmd(chip, &cmd, &data);
	if (res) {
		KFREE(buf);
		return res;
	}
	emmc_decode_extcsd(chip, buf);
	KFREE(buf);
	return res;
}

static int emmc_cmd9_send_csd(struct emmc_chip *chip)
{
	struct mmc_cmd cmd;
	int res;

	cmd.index = 9;
	cmd.arg = chip->rca;
	res = issue_cmd(chip, &cmd, NULL);
	if (res)
		return res;
	chip->csd.raw_csd[0] = HOST_REG_RD(&chip->regs, RESPONSE_01);
	chip->csd.raw_csd[1] = HOST_REG_RD(&chip->regs, RESPONSE_23);
	chip->csd.raw_csd[2] = HOST_REG_RD(&chip->regs, RESPONSE_45);
	chip->csd.raw_csd[3] = HOST_REG_RD(&chip->regs, RESPONSE_67);
	fix_r2_buffer(&chip->csd.raw_csd[0]);
	decode_csd(chip);
	return res;

}

#if 0
static int emmc_cmd10_send_cid(struct emmc_chip *chip)
{
	struct mmc_cmd cmd;
	uint32_t *cid = &chip->cid.raw_cid[0];
	int res;

	cmd.index = 10;
	cmd.arg = 0;
	res = issue_cmd(chip, &cmd, NULL);
	if (res)
		return res;
	cid[0] = HOST_REG_RD(&chip->regs, RESPONSE_01);
	cid[1] = HOST_REG_RD(&chip->regs, RESPONSE_23);
	cid[2] = HOST_REG_RD(&chip->regs, RESPONSE_45);
	cid[3] = HOST_REG_RD(&chip->regs, RESPONSE_67);

	fix_r2_buffer(cid);
	decode_cid(chip);

	return res;

}
#endif

static int emmc_cmd13_send_status(struct emmc_chip *chip, uint32_t *status)
{
	struct mmc_cmd cmd;
	uint32_t arg;
	int res;

	arg = chip->rca;

	/* CMD13, R1 (Index check  On, CRC check  On, 48bit) */
	cmd.index = 13;
	cmd.arg = arg;
	res = issue_cmd(chip, &cmd, NULL);
	if (res)
		return res;
	*status = HOST_REG_RD(&chip->regs, RESPONSE_01);
#if DEBUG_EMMC_STATUS
	emmc_print_card_status(*status);
#endif
	return res;
}

static int emmc_cmd16_set_blocklen(struct emmc_chip *chip,
				   uint32_t block_length)
{
	struct mmc_cmd cmd;
	uint32_t reg;

	reg = HOST_REG_RD(&chip->regs, BLKCNT_BLKSIZE);
	HOST_REG_WR(&chip->regs, BLKCNT_BLKSIZE,
		(reg & ~SDHC_REG_BLKSIZE_MASK) + block_length);

	/* CMD16 -  R1 (Index check  On, CRC check  On, 48bit) */
	cmd.index = 16;
	cmd.arg = block_length;
	return issue_cmd(chip, &cmd, NULL);

}

static int emmc_cmd17_read_block(struct emmc_chip *chip,
				 uint32_t block,
				 uint32_t dma_addr,
				 uint32_t block_length)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.index = 17;
	cmd.arg = block << chip->dev_config.block_shift;
	data.dma_address = dma_addr;
	data.block_count = 1;
	data.block_size = block_length;
	return issue_cmd(chip, &cmd, &data);

}

int  emmc_cmd18_read_multiple_block(struct emmc_chip *chip,
				uint32_t block,
				uint32_t dma_addr,
				uint32_t block_count,
				uint32_t block_length)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.index = 18;
	cmd.arg = block << chip->dev_config.block_shift;
	data.dma_address = dma_addr;
	data.block_count = block_count;
	data.block_size = block_length;
	return issue_cmd(chip, &cmd, &data);

}

int emmc_cmd23_set_block_count(struct emmc_chip *chip,
			uint32_t count)
{
	struct mmc_cmd cmd;

	/* CMD23 -  R1 (Index check  On, CRC check  On, 48bit) */
	cmd.index = 23;
	cmd.arg = count;
	return issue_cmd(chip, &cmd, NULL);

}

static int emmc_cmd24_write_block(struct emmc_chip *chip,
				  uint32_t block,
				  uint32_t dma_addr,
				  uint32_t block_length)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.index = 24;
	cmd.arg = block << chip->dev_config.block_shift;
	data.dma_address = dma_addr;
	data.block_count = 1;
	data.block_size = block_length;
	return issue_cmd(chip, &cmd, &data);

}

int emmc_cmd25_write_multiple_block(struct emmc_chip *chip,
				uint32_t block,
				uint32_t dma_addr,
				uint32_t block_count,
				uint32_t block_length)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.index = 25;
	cmd.arg = block << chip->dev_config.block_shift;
	data.dma_address = dma_addr;
	data.block_count = block_count;
	data.block_size = block_length;
	return issue_cmd(chip, &cmd, &data);

}

static void emmc_decode_csd_timeouts(struct emmc_chip *chip)
{
	uint32_t read_timeout;
	uint32_t write_timeout;
	uint32_t erase_timeout;
	uint32_t time_unit;
	uint32_t multi_factor;
	uint32_t time_unit_data[8] = {
		1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
	};
	/* values are 10x to avoid using floats */
	int multi_data[16] = {
		0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55,
		60, 70, 80
	};

	time_unit = (uint32_t) (chip->csd.TAAC & 0x07);
	multi_factor = (chip->csd.TAAC >> 3) & 0xFF;
	read_timeout =
		(time_unit_data[time_unit] * multi_data[multi_factor]) / 10;

	write_timeout = read_timeout * (1 << chip->csd.R2W_FACTOR);

	chip->dev_config.read_timeout = read_timeout;
	chip->dev_config.write_timeout = write_timeout;

	/*  Make [ms] unit from [ns] */
	erase_timeout = (uint32_t) (write_timeout / 1000000);
	if (erase_timeout < 1)
		erase_timeout = 1;
	chip->dev_config.erase_timeout = erase_timeout;

#if DEBUG_EMMC_CONFIG
	os_printf("emmc_decode_csd_timeouts\n");
	os_printf("  time_unit_data(time_unit) = %d, (CSD.TAAC=%d)\n",
		time_unit_data[time_unit], chip->csd.TAAC);
	os_printf("  multi_data(multi_factor) = %d.%d\n",
		multi_data[multi_factor] / 10, multi_data[multi_factor] % 10);
	os_printf("  read_timeout[ns] = %d, (CSD_R2W_FACTOR=%d)\n",
		read_timeout, chip->csd.R2W_FACTOR);
	os_printf("  write_timeout[ns] = %d\n", write_timeout);
	os_printf("  EraseWriteTimeout[ms] = %d\n", erase_timeout);
#endif

}

static int emmc_set_clock(struct emmc_chip *chip, unsigned int hz)
{
	uint32_t control1;
	uint32_t base_clock;
	uint32_t div;
	int timeout;

	control1 = HOST_REG_RD(&chip->regs, CONTROL_1);

	/* clear all clock control bits */
	control1 &= ~0xffff;
	HOST_REG_WR(&chip->regs, CONTROL_1, control1);

	/* Get the base clock from the CAPS register */
	base_clock = HOST_REG_RD(&chip->regs, CAPABILITIES_0);
	base_clock = HOST_GET_FIELD(base_clock, SDHC_CAP0_BASE_CLK_FREQ);

	/*
	 * The register value is really value << 1 so divide base_clock
	 * by 2 when going from MHz to Hz to compensate. Also round down.
	 */
	base_clock *= (1000000 / 2);
	div = base_clock / hz;
	while (base_clock / div > hz)
		div++;

	HOST_SET_FIELD(control1, SDHC_CLKCTL_FREQ, div);
	HOST_SET_FIELD(control1, SDHC_CLKCTL_FREQ_UPPER, div >> 8);
	HOST_SET_FIELD(control1, SDHC_TOCTL_TIMEOUT, 0xe);
	control1 |= SDHC_CLKCTL_INTERN_CLK_ENA;

	/* Enable the internal clock */
	HOST_REG_WR(&chip->regs, CONTROL_1, control1);

	/* Wait 50ms for internal clock stable */
	timeout = 50000 / POLL_DELAY_USECS;
	while (1) {
		if (timeout-- == 0)
			break;
		if (HOST_REG_RD(&chip->regs, CONTROL_1) &
			SDHC_CLKCTL_INTERN_CLK_STABLE)
			break;
		os_usleep(POLL_DELAY_USECS);
	}
	if (timeout < 0) {
		err_msg("%s timeout waiting for internal clock stable",
			chip->regs.name);
		return 1;
	}

	/* Enable SD_Clk */
	control1 |= SDHC_CLKCTL_SD_CLK_ENA;
	HOST_REG_WR(&chip->regs, CONTROL_1, control1);
	return 0;
}

static void init_host_config(struct emmc_chip *chip)
{
	if ((BOOT_REG_RD(&chip->regs, CTL) & SDIO_BOOT_REG_CTL_MODE_BOOTING) ||
		(BOOT_REG_RD(&chip->regs, STATUS) &
			SDIO_BOOT_REG_STATUS_MODE_BOOTING)) {

		/* Disable Boot Logic in chip */
		BOOT_REG_WR(&chip->regs, CTL, 0x00);
		while (1) {
			if ((BOOT_REG_RD(&chip->regs, STATUS) &
					SDIO_BOOT_REG_STATUS_MODE_BOOTING) == 0)
				break;
			os_usleep(1);
		}

		/* Clear any boot mode bits */
		HOST_REG_WR(&chip->regs, CONTROL_0, 0);

	}
}

/* Init the eMMC device
 * Return:
 *	0 - success
 *	1 - retry
 *	-1 - fail
 */
static int  init_device(struct emmc_chip *chip, uint32_t freq)
{
	uint32_t ocr_data;
	int res;
	int timeout_cnt;

	if (emmc_software_reset(chip, SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD |
					SDHC_SW_RESET_ALL))
		return 1;

	HOST_REG_WR(&chip->regs, CONTROL_0,
		(SDHC_BUS_PWR +	(chip->host_config.bus_voltage <<
				SDHC_BUS_VOLT_SHIFT)));

	if (emmc_set_clock(chip, freq))
		return 1;

	/* Enable interrupts */
	emmc_enable_int(chip, HOST_INT_ENABLES);

	/* Device Reset */
	if (emmc_cmd0_go_idle_state(chip))
		return 1;

	/*
	 * Voltage validation and wait for device ready.
	 * Wait for up to 1 second  for ready bit
	 */
	timeout_cnt = 1000000 / 1000;
	while (1) {
		res = emmc_cmd1_send_op_cond(chip,
					OCR_SECTOR_MODE | OCR_VDD_33_34,
					&ocr_data);
		if (res)
			return 1;

		/* Make sure the card supports 3.3V */
		if ((ocr_data & OCR_VDD_33_34) == 0) {
			err_msg("%s: eMMC device does not support 3.3V",
				chip->regs.name);
			return -1;
		}
		if (ocr_data & 0x80000000)	/* ready bit */
			break;
		if (timeout_cnt-- == 0)
			break;
		os_msleep(1);
	}
	if (timeout_cnt < 0)
		return 1;

	/* Check for the High Capacity bit */
	if (ocr_data & 0x40000000) {
		chip->dev_config.high_capacity = 1;
		DBG_MSG_DRV("High Capacity Device\n");
	} else {
		DBG_MSG_DRV("Standard Capacity device, uses byte addressing\n");
	}

	res = emmc_cmd2_all_send_cid_single_device(chip);
	if (res)
		return 1;

	/* Set RCA */
	res = emmc_cmd3_set_rca(chip, chip->rca);
	if (res)
		return 1;

	return 0;
}

static int setup_bus_freq_width(struct emmc_chip *chip)
{
	uint32_t reg_ctrl_set0;
	uint32_t freq;
	int res;

	reg_ctrl_set0 = HOST_REG_RD(&chip->regs, CONTROL_0);

	/* Bus Frequency Setting : eMMC, Host controller */
	if (chip->host_config.hosths_on) {
		/* High Speed Mode - 50MHz */
		res  = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					  Idx_ExtCSD_HS_TIMING, HS_TIMING_HS);
		if (res)
			return res;
		reg_ctrl_set0 |= SDHC_CTL1_HIGH_SPEED;
		HOST_REG_WR(&chip->regs, CONTROL_0, reg_ctrl_set0);
		freq = 50000000;
	} else {
		/* Full Speed Mode - 25MHz */
		res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					 Idx_ExtCSD_HS_TIMING, HS_TIMING_FULL);
		if (res)
			return res;
		freq = 25000000;
	}
	emmc_set_clock(chip, freq);

	/*
	 * Bus Width Setting : eMMC, Host controller
	 */
	switch (chip->host_config.bus_width) {
	case BUS_WIDTH_4BIT:
		res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					 Idx_ExtCSD_BUS_WIDTH, BUS_WIDTH_4BIT);
		if (res)
			return res;
		reg_ctrl_set0 &= ~(SDHC_CTL1_8BIT);
		reg_ctrl_set0 |= SDHC_CTL1_4BIT;
		break;
	case BUS_WIDTH_8BIT:
		res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					 Idx_ExtCSD_BUS_WIDTH, BUS_WIDTH_8BIT);
		if (res)
			return res;
		reg_ctrl_set0 &= ~SDHC_CTL1_4BIT;
		reg_ctrl_set0 |= SDHC_CTL1_8BIT;
		break;
	case BUS_WIDTH_1BIT:
	default:
		res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					 Idx_ExtCSD_BUS_WIDTH, BUS_WIDTH_1BIT);
		if (res)
			return res;
		reg_ctrl_set0 &= ~(SDHC_CTL1_8BIT | SDHC_CTL1_4BIT);
		break;
	}
	/*  Set registers */
	HOST_REG_WR(&chip->regs, CONTROL_0, reg_ctrl_set0);

	/* BOOT_BUS_WIDTH : BRCM EMMC BOOT support only 8-bit */
	if ((chip->extcsd.BOOT_BUS_CONDITIONS & 0x03) != 0x02) {
		res = cmd6_switch_extcsd(
			      chip,
			      Idx_ExtCSD_ACC_WRB,
			      Idx_ExtCSD_BOOT_BUS_CONDITIONS,
			      (chip->extcsd.BOOT_BUS_CONDITIONS & 0xFC) | 0x02);
		if (res)
			return res;
	}
	return 0;
}

/*  Data Transfer Mode */
static int init_transfer_mode(struct emmc_chip *chip)
{
	int i;
	int group_size;
	int res = 0;

	/* CSD Retrieval and Host Adjustment */
	res = emmc_cmd9_send_csd(chip);

	/* Change state from stand-by to transfer */
	res = emmc_cmd7_select_card_stby_tans(chip, chip->rca);
	if (res)
		return res;

	/* Extended_CSD Retrieval and Host Adjustment */
	if (chip->csd.SPEC_VERS > 3) {
		res = emmc_cmd8_send_ext_csd(chip);
		if (res)
			return res;
	}

	/* Bus Mode Configuration */
	res = setup_bus_freq_width(chip);
	if (res) {
		err_msg("%s bus mode configuration failed!", chip->regs.name);
		return res;
	}

	/* Calculate DataAreaSize, Boot, RPMB */
	if (chip->dev_config.high_capacity) {
		chip->dev_config.hard_part_sizes[EMMC_HPART_DATA] =
			(uint64_t)chip->extcsd.SEC_COUNT * EMMC_BLOCKSIZE;
	} else {			/* Byte Mode */
		chip->dev_config.hard_part_sizes[EMMC_HPART_DATA] =
			(chip->csd.C_SIZE + 1) *
			(1 << chip->csd.READ_BL_LEN) *
			(1 << (chip->csd.C_SIZE_MULT + 2));
		chip->dev_config.block_shift = 9;
	}

	/* size in bytes */
	chip->dev_config.hard_part_sizes[EMMC_HPART_BOOT1] =
		(uint64_t)chip->extcsd.BOOT_SIZE_MULT * 128 * 1024;
	chip->dev_config.hard_part_sizes[EMMC_HPART_BOOT2] =
		chip->dev_config.hard_part_sizes[EMMC_HPART_BOOT1];
	chip->dev_config.hard_part_sizes[EMMC_HPART_RPMB] =
		(uint64_t)chip->extcsd.RPMB_SIZE_MULT * 128 * 1024;

	group_size = chip->extcsd.HC_ERASE_GRP_SIZE *
		chip->extcsd.HC_WP_GRP_SIZE;

	for (i = 0; i < 4; i++)
		chip->dev_config.hard_part_sizes[EMMC_HPART_GP0 + i] =
			(uint64_t)(chip->extcsd.GP_SIZE_MULT[i] * group_size)
			<< 19;

	/* Partition Config */
	chip->dev_config.partition_access =
		chip->extcsd.PARTITION_CONFIG & PCFG_PARTITION_ACCESS_MASK;
	chip->dev_config.partition_switch_time =
		chip->extcsd.PARTITION_SWITCH_TIME * 10;

	/* get timeouts from CSD */
	emmc_decode_csd_timeouts(chip);

	/*
	 * Calculation erase_unit_size, erase_timeout,
	 * WriteProtectionGroupSize
	 */
	if ((chip->extcsd.EXT_CSD_REV >= JESD84_V43)
	    && (chip->dev_config.high_capacity == 1)
	    && (chip->extcsd.HC_ERASE_GRP_SIZE != 0)
	    && (chip->extcsd.ERASE_TIMEOUT_MULT != 0)) {

		/* Calculate Erase Unit Size : CSD_WRITE_BL_LEN[512B] */
		chip->dev_config.erase_unit_size =
			chip->extcsd.HC_ERASE_GRP_SIZE;
		chip->dev_config.erase_unit_sector =
			chip->dev_config.erase_unit_size * 1024;

		/* Calculate Erase Timeout */
		chip->dev_config.erase_timeout =
			chip->extcsd.HC_ERASE_GRP_SIZE * 100;

		/*
		 * Calculate Write Protection Group Size :
		 * CSD_WRITE_BL_LEN[512B]
		 */
		chip->dev_config.wp_grp_size =
			chip->dev_config.erase_unit_size *
			chip->extcsd.HC_WP_GRP_SIZE;

		/* Set-up High Capacity */
		res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
					 Idx_ExtCSD_ERASE_GROUP_DEF, 1);
		if (res)
			return res;
	} else {
		/* Calculate Erase Unit Size */
		chip->dev_config.erase_unit_size =
			(chip->csd.ERASE_GRP_SIZE + 1) *
			(chip->csd.ERASE_GRP_MULT + 1);
		chip->dev_config.erase_unit_sector =
			chip->dev_config.erase_unit_size;

		/*
		 * Calculate Write Protection Group Size :
		 * CSD_WRITE_BL_LEN[512B]
		 */
		chip->dev_config.wp_grp_size = (chip->csd.WP_GRP_SIZE + 1) *
					   chip->dev_config.erase_unit_size;
		if (chip->dev_config.high_capacity == 1) {
			/* Set-up High Capacity */
			res = cmd6_switch_extcsd(chip, Idx_ExtCSD_ACC_WRB,
						Idx_ExtCSD_ERASE_GROUP_DEF, 0);
			if (res)
				return res;
		}
	}

	if (chip->dev_config.high_capacity == 1) {
		/* SET_BLOCKLEN to set the block length as 512 bytes. */
		res = emmc_cmd16_set_blocklen(chip, EMMC_BLOCKSIZE);
		if (res)
			return res;
	}

	/* Clear all interrupt status bits */
	emmc_clear_int_status(chip, HOST_INT_STATUS_ALL);

	res = emmc_cmd8_send_ext_csd(chip);
	if (res)
		return res;

	/* Check the current device status */
#if DEBUG_EMMC_CONFIG
	os_printf("------------------------------\n");
	os_printf(" - High Capacity Mode : %d (1:On, 0:Off)\n",
		chip->extcsd.ERASE_GROUP_DEF);
	os_printf(" Data Area Size = %lld, %d[MB]\n",
		chip->dev_config.hard_part_sizes[EMMC_HPART_DATA],
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_DATA] /
			(1024 * 1024)));
	os_printf(" Boot 1 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_BOOT1] /
			(1024 * 1024)));
	os_printf(" Boot 2 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_BOOT2] /
			(1024 * 1024)));
	os_printf(" RPMB Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_RPMB] /
			(1024 * 1024)));
	os_printf(" GP0 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_GP0] /
			(1024 * 1024)));
	os_printf(" GP1 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_GP1] /
			(1024 * 1024)));
	os_printf(" GP2 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_GP2] /
			(1024 * 1024)));
	os_printf(" GP3 Area Size = %d[MB]\n",
		(uint32_t)(chip->dev_config.hard_part_sizes[EMMC_HPART_GP3] /
			(1024 * 1024)));

	if (chip->extcsd.ERASE_GROUP_DEF == 1)
		os_printf(" Erase Unit Size   = %d[512KB] : %d[MB]\n",
			chip->dev_config.erase_unit_size,
			chip->dev_config.erase_unit_size / 2);
	else
		os_printf(" Erase Unit Size   = %d[512B] : %d[KB]\n",
			chip->dev_config.erase_unit_size,
			chip->dev_config.erase_unit_size / 2);

	os_printf(" Erase Unit Sector = %d[512B]\n",
		chip->dev_config.erase_unit_sector);
	os_printf(" Erase Timeout	    = %d[ms]\n",
		chip->dev_config.erase_timeout);

	if (chip->extcsd.ERASE_GROUP_DEF == 1)
		os_printf(" Write Protection Group Size = %d[512KB] : %d[MB]\n",
			chip->dev_config.wp_grp_size,
			chip->dev_config.wp_grp_size / 2);
	else
		os_printf(" Write Protection Group Size = %d[512B] : %d[KB]\n",
			chip->dev_config.wp_grp_size,
			chip->dev_config.wp_grp_size / 2);

	os_printf("------------------------------\n");
#endif

	return 0;
}

/*
 * eMMC Initialization
 */
int emmc_initialize(struct emmc_registers *regs,
		struct emmc_chip  *chip,
		int boot_flag,
		struct emmc_host_config *host_config)
{
	int speeds;
	int cnt;
	int res;

	DBG_MSG_DRV("Starting emmc_initialize\n");
	DBG_MSG_DRV("\tSpeed: %dMHz\n", host_config->hosths_on ? 50 : 25);
	DBG_MSG_DRV("\tBus Width: %d\n", host_config->bus_width);
	DBG_MSG_DRV("\t%s mode\n", host_config->disable_dma ? "PIO" : "DMA");

	memset(chip, 0, sizeof(struct emmc_chip));
	chip->boot_device = boot_flag;
	chip->regs = *regs;
	chip->host_config = *host_config;
	chip->last_partition = -1;

	/* For single device, means 2, 0x00020000 For argument register */
	chip->rca = EMMC_RCA;

	/* Host(STB Chip) Configuration */
	init_host_config(chip);

	/*
	 * The early init commands are run in open drain mode where the
	 * bus pull-ups are important. Broadcom chips use a value higher
	 * than recommended by the spec, but the spec says the clock can be
	 * slowed down to compensate. Most eMMC modules handle the default
	 * speed of 400KHz, but a least some SanDisk modules only run at
	 * 200KHz-300KHz. To handle this we start at 400KHz and if we get
	 * errors, retry at 300KHz, 200KHz and 100KHz. Once we're out
	 * of open-drain mode, the pull-ups don't matter.
	 */
	speeds = sizeof(startup_speeds) / sizeof(startup_speeds[0]);
	chip->probing = 1;
	for (cnt = 0; cnt < speeds; cnt++) {
		/* Initialize the eMMC device */
		res = init_device(chip, startup_speeds[cnt]);
		if (res < 1)
			break;
	}
	chip->probing = 0;
	if (res != 0) {
		err_msg("%s device probe, no eMMC device found",
			chip->regs.name);
		return 1;
	}
	if (cnt)
		err_msg("%s device init failed at %dHz, "
			"reduced clock to %dHz",
			chip->regs.name, startup_speeds[0],
			startup_speeds[cnt]);

	/* Get device into transfer mode */
	if (init_transfer_mode(chip)) {
		err_msg("%s device failed to enter transfer mode",
			chip->regs.name);
		return 1;
	}

#if DEBUG_EMMC_DRV
	emmc_print_host_emmc_info(chip);
	emmc_print_extcsd(chip);
	DBG_MSG_DRV("End : emmc_initialize( )\n");
#endif
	return 0;
}

int emmc_select_partition(struct emmc_chip *chip, int id)
{
	uint32_t partition_config = 0x00;
	int res;

	/* If this partition is already selected, just return */
	if (id == chip->last_partition)
		return 0;

	/* In case of failure */
	chip->last_partition = -1;

	res = emmc_cmd8_send_ext_csd(chip);
	if (res)
		return res;
	DBG_MSG_DRV(" chip->ExtCSD.PARTITION_SETTING_COMPLETED = %d\n",
		    chip->extcsd.PARTITION_SETTING_COMPLETED);
	DBG_MSG_DRV(" chip->ExtCSD.PARTITION_CONFIG = 0x%02X (%d)\n",
		    chip->extcsd.PARTITION_CONFIG,
		    chip->extcsd.PARTITION_CONFIG);

	/* select the partition, but retain the boot selection field */
	partition_config = chip->extcsd.PARTITION_CONFIG;
	partition_config &= ~PCFG_PARTITION_ACCESS_MASK;
	partition_config |= id;

	res = cmd6_switch_extcsd(chip,
				 Idx_ExtCSD_ACC_WRB,
				 Idx_ExtCSD_PARTITION_CONFIG,
				 partition_config);
	if (res)
		return res;
	os_msleep(chip->dev_config.partition_switch_time);	/* Important */

#if DEBUG_EMMC_DRV_DISPATCH
	res = emmc_cmd8_send_ext_csd(chip);
	if (res)
		return res;
	os_printf(" partition_config : 0x%02X\n", partition_config);
	os_printf(" chip->ExtCSD.PARTITION_SETTING_COMPLETED = %d\n",
		chip->extcsd.PARTITION_SETTING_COMPLETED);
	os_printf(" chip->ExtCSD.PARTITION_CONFIG = 0x%02X (%d)\n",
		chip->extcsd.PARTITION_CONFIG,
		chip->extcsd.PARTITION_CONFIG);
#endif
	chip->last_partition = id;
	return 0;
}

void emmc_set_boot_partition(struct emmc_chip *chip, int id)
{
	cmd6_switch_extcsd(chip,
			Idx_ExtCSD_ACC_WRB,
			Idx_ExtCSD_PARTITION_CONFIG,
			PCFG_BOOT_ACK |
			(id ? PCFG_BOOT_PARTITION_ENABLE_BOOT2 :
				PCFG_BOOT_PARTITION_ENABLE_BOOT1));
	os_msleep(chip->dev_config.partition_switch_time);
}



int emmc_block_read(struct emmc_chip *chip, uint32_t blk_start,
		uint32_t blk_cnt, uint8_t *buffer)
{
	int res;
	uint32_t blks;

	while (blk_cnt) {
		if (blk_cnt > ARASAN_SDMA_MAX_BLKS)
			blks = ARASAN_SDMA_MAX_BLKS;
		else
			blks = blk_cnt;
		res = wait_ready_data_xfer(chip);
		if (res)
			return res;
		if (blks == 1)
			res = emmc_cmd17_read_block(chip, blk_start,
						    (uint32_t)buffer,
						    EMMC_BLOCKSIZE);
		else
			res = emmc_cmd18_read_multiple_block(chip,
							     blk_start,
							     (uint32_t)buffer,
							     blks,
							     EMMC_BLOCKSIZE);
		if (res)
			return res;
		blk_cnt -= blks;
		blk_start += blks;
		buffer += (blks * EMMC_BLOCKSIZE);
	}
	return 0;
}

int emmc_block_write(struct emmc_chip *chip, uint32_t blk_start,
		uint32_t blk_cnt, uint8_t *buffer)
{
	int res;
	uint32_t blks;

	while (blk_cnt) {
		if (blk_cnt > ARASAN_SDMA_MAX_BLKS)
			blks = ARASAN_SDMA_MAX_BLKS;
		else
			blks = blk_cnt;
		res = wait_ready_data_xfer(chip);
		if (res)
			return res;
		if (blks == 1)
			res = emmc_cmd24_write_block(chip, blk_start,
						     (uint32_t)buffer,
						     EMMC_BLOCKSIZE);
		else
			res = emmc_cmd25_write_multiple_block(chip,
							      blk_start,
							      (uint32_t)buffer,
							      blks,
							      EMMC_BLOCKSIZE);
		if (res)
			return res;
		blk_cnt -= blks;
		blk_start += blks;
		buffer += (blks * EMMC_BLOCKSIZE);
	}
	return 0;
}
