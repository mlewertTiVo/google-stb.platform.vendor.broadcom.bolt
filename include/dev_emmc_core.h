/***************************************************************************
 *     Copyright (c) 2015 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEV_EMMC_CORE_H__
#define __DEV_EMMC_CORE_H__

#define EMMC_BLOCKSIZE 512
#define EMMC_BLOCKSIZE_RPMB 256
#define EMMC_DMA_BUF_ALIGN  512


/* eMMC hard partitions */
#define EMMC_HPART_DATA  0	/* Data Partition in EMMC */
#define EMMC_HPART_BOOT1 1	/* Boot1 Partition in EMMC */
#define EMMC_HPART_BOOT2 2	/* Boot2 Partition in EMMC */
#define EMMC_HPART_RPMB  3	/* Replay Protected Memory Block in EMMC */
#define EMMC_HPART_GP0   4	/* General Purpose Partition 0 Block in EMMC */
#define EMMC_HPART_GP1   5	/* General Purpose Partition 1 Block in EMMC */
#define EMMC_HPART_GP2   6	/* General Purpose Partition 2 Block in EMMC */
#define EMMC_HPART_GP3   7	/* General Purpose Partition 3 Block in EMMC */
#define EMMC_HPART_MAX   8      /* Max possible hard partitions */

#define RPMB_KEY_LENGTH 32

struct emmcdev_cid {
	uint32_t raw_cid[4];

	uint8_t MID;
	uint8_t CBX;
	uint8_t OID;
	char PNM[6];
	uint8_t PRV;
	uint32_t PSN;
	uint8_t MDT;
};

struct emmcdev_csd {
	uint32_t raw_csd[4];

	uint8_t CSD_STRUCTURE;
	uint8_t SPEC_VERS;
	uint8_t TAAC;
	uint8_t NSAC;
	uint8_t TRAN_SPEED;
	uint16_t CCC;
	uint8_t READ_BL_LEN;
	uint8_t READ_BL_PARTIAL;
	uint8_t WRITE_BLK_MISALIGN;
	uint8_t READ_BLK_MISALIGN;
	uint8_t DSR_IMP;
	uint16_t C_SIZE;
	uint8_t VDD_R_CURR_MIN;
	uint8_t VDD_R_CURR_MAX;
	uint8_t VDD_W_CURR_MIN;
	uint8_t VDD_W_CURR_MAX;
	uint8_t C_SIZE_MULT;
	uint8_t ERASE_GRP_SIZE;
	uint8_t ERASE_GRP_MULT;
	uint8_t WP_GRP_SIZE;
	uint8_t WP_GRP_ENABLE;
	uint8_t DEFAULT_ECC;
	uint8_t R2W_FACTOR;
	uint8_t WRITE_BL_LEN;
	uint8_t WRITE_BL_PARTIAL;
	uint8_t CONTENT_PROT_APP;
	uint8_t FILE_FORMAT_GRP;
	uint8_t COPY;
	uint8_t PERM_WRITE_PROTECT;
	uint8_t TMP_WRITE_PROTECT;
	uint8_t FILE_FORMAT;
	uint8_t ECC;
};

struct emmcdev_extcsd {
	/* Properties Segment */
	uint8_t S_CMD_SET;
	uint8_t HPI_FEATURES;
	uint8_t BKOPS_SUPPORT;
	uint8_t MAX_PACKED_READS;
	uint8_t MAX_PACKED_WRITES;
	uint8_t DATA_TAG_SUPPORT;
	uint8_t TAG_UNIT_SIZE;
	uint8_t TAG_RES_SIZE;
	uint8_t CONTEXT_CAPABILITIES;
	uint8_t LARGE_UNIT_SIZE_M1;
	uint8_t EXT_SUPPORT;
	uint32_t CACHE_SIZE;
	uint8_t GENERIC_CMD6_TIME;
	uint8_t POWER_OFF_LONG_TIME;
	uint8_t BKOPS_STATUS;
	uint32_t CORRECTLY_PRG_SECTORS_NUM;
	uint8_t INI_TIMEOUT_AP;
	uint8_t PWR_CL_DDR_52_360;
	uint8_t PWR_CL_DDR_52_195;
	uint8_t PWR_CL_200_360;
	uint8_t PWR_CL_200_195;
	uint8_t MIN_PERF_DDR_W_8_52;
	uint8_t MIN_PERF_DDR_R_8_52;
	uint8_t TRIM_MULT;
	uint8_t SEC_FEATURE_SUPPORT;
	uint8_t SEC_ERASE_MULT;
	uint8_t SEC_TRIM_MULT;
	uint8_t BOOT_INFO;
	uint8_t BOOT_SIZE_MULT;
	uint8_t ACC_SIZE;
	uint8_t HC_ERASE_GRP_SIZE;
	uint8_t ERASE_TIMEOUT_MULT;
	uint8_t REL_WR_SEC_C;
	uint8_t HC_WP_GRP_SIZE;
	uint8_t S_C_VCC;
	uint8_t S_C_VCCQ;
	uint8_t S_A_TIMEOUT;
	uint32_t SEC_COUNT;
	uint8_t MIN_PERF_W_8_52;
	uint8_t MIN_PERF_R_8_52;
	uint8_t MIN_PERF_W_8_26_4_52;
	uint8_t MIN_PERF_R_8_26_4_52;
	uint8_t MIN_PERF_W_4_26;
	uint8_t MIN_PERF_R_4_26;
	uint8_t PWR_CL_26_360;
	uint8_t PWR_CL_52_360;
	uint8_t PWR_CL_26_195;
	uint8_t PWR_CL_52_195;
	uint8_t PARTITION_SWITCH_TIME;
	uint8_t OUT_OF_INTERRUPT_TIME;
	uint8_t DRIVER_STRENGTH;
	uint8_t DEVICE_TYPE;
	uint8_t CSD_STRUCTURE;
	uint8_t EXT_CSD_REV;

	/* Modes Segment */
	uint8_t CMD_SET;
	uint8_t CMD_SET_REV;
	uint8_t POWER_CLASS;
	uint8_t HS_TIMING;
	uint8_t BUS_WIDTH;
	uint8_t ERASED_MEM_CONT;
	uint8_t PARTITION_CONFIG;
	uint8_t BOOT_CONFIG_PROT;
	uint8_t BOOT_BUS_CONDITIONS;
	uint8_t ERASE_GROUP_DEF;
	uint8_t BOOT_WP_STATUS;
	uint8_t BOOT_WP;
	uint8_t USER_WP;
	uint8_t FW_CONFIG;
	uint8_t RPMB_SIZE_MULT;
	uint8_t WR_REL_SET;
	uint8_t WR_REL_PARAM;
	uint8_t SANITIZE_START;
	uint8_t BKOPS_START;
	uint8_t BKOPS_EN;
	uint8_t RST_n_FUNCTION;
	uint8_t HPI_MGMT;
	uint8_t PARTITIONING_SUPPORT;
	uint32_t MAX_ENH_SIZE_MULT;
	uint8_t PARTITIONS_ATTRIBUTE;
	uint8_t PARTITION_SETTING_COMPLETED;
	uint32_t GP_SIZE_MULT[4];
	uint32_t ENH_SIZE_MULT;
	uint32_t ENH_START_ADDR;
	uint8_t SEC_BAD_BLK_MGMNT;
	uint8_t TCASE_SUPPORT;
	uint8_t PERIODIC_WAKEUP;
	uint8_t PROGRAM_CID_CSD_DDR_SUPPORT;
	uint8_t VENDOR_SPECIFIC_FIELD[64];
	uint8_t NATIVE_SECTOR_SIZE;
	uint8_t USE_NATIVE_SECTOR;
	uint8_t DATA_SECTOR_SIZE;
	uint8_t INI_TIMEOUT_EMU;
	uint8_t CLASS_6_CTRL;
	uint8_t DYNCAP_NEEDED;
	uint16_t EXCEPTION_EVENTS_CTRL;
	uint16_t EXCEPTION_EVENTS_STATUS;
	uint16_t EXT_PARTITIONS_ATTRIBUTE;
	uint8_t CONTEXT_CONF[15];
	uint8_t PACKED_COMMAND_STATUS;
	uint8_t PACKED_FAILURE_INDEX;
	uint8_t POWER_OFF_NOTIFICATION;
	uint8_t CACHE_CTRL;
	uint8_t FLUSH_CACHE;
};

struct emmc_host_config {
	int hosths_on;
	int bus_width; /* BUS_WIDTH_<n>BIT */
	int bus_voltage; /* BUS_VOLTAGE_<n> */
	int disable_dma;
};

struct emmc_dev_config {
	/* Partition sizes in bytes*/
	uint64_t hard_part_sizes[EMMC_HPART_MAX];

	/* Partition Info */
	uint8_t partition_access;
	uint16_t partition_switch_time;

	/* Block Size & Control */
	uint32_t block_shift;
	uint32_t high_capacity;
	uint32_t erase_unit_size;
	uint32_t erase_unit_sector;
	uint32_t erase_timeout;
	uint32_t read_timeout;
	uint32_t write_timeout;
	uint32_t wp_grp_size;
};

/* Register offset and bit mask defines to replace bchp*.h defines */
#define SDIO_BOOT_REG_CTL	0x00	/* Register offset */
#define   SDIO_BOOT_REG_CTL_MODE_BOOTING	0x01
#define SDIO_BOOT_REG_STATUS	0x04	/* Register offset */
#define   SDIO_BOOT_REG_STATUS_MODE_BOOTING	0x01


#define EMMC_REG_BLK_BASE_HOST		0
#define EMMC_REG_BLK_BASE_BOOT		1
#define EMMC_REG_BLK_BASE_MAX		2

/* base pointers to register blocks */
struct emmc_registers {
	void *reg_bases[EMMC_REG_BLK_BASE_MAX];
	int id;
	char *name;
};

struct rpmb_key {
	uint8_t key[RPMB_KEY_LENGTH];
	int valid;
};

struct emmc_chip {
	/* Registers in eMMC device */
	struct emmcdev_cid cid;
	struct emmcdev_csd csd;
	struct emmcdev_extcsd extcsd;
	uint32_t rca;

	/* Host Configuration */
	struct emmc_host_config host_config;
	/* Configuration from eMMC device register data */
	struct emmc_dev_config dev_config;

	/* pointers to host register blocks */
	struct emmc_registers regs;
	int boot_device;		/* TRUE if this is the boot device */
	int probing;			/* TRUE if we are probing the device */
	int last_cmd;
	int last_partition;
	struct rpmb_key rpmb_key;
};

int emmc_cmd23_set_block_count(struct emmc_chip *chip, uint32_t count);
int emmc_cmd25_write_multiple_block(struct emmc_chip *chip, uint32_t block,
				uint32_t dma_addr, uint32_t block_count,
				uint32_t block_length);
int  emmc_cmd18_read_multiple_block(struct emmc_chip *chip, uint32_t block,
				uint32_t dma_addr, uint32_t block_count,
				uint32_t block_length);

int emmc_block_read(struct emmc_chip *chip, uint32_t blk_start,
		uint32_t blk_cnt, uint8_t *buffer);
int emmc_block_write(struct emmc_chip *chip, uint32_t blk_start,
		uint32_t blk_cnt, uint8_t *buffer);
int emmc_select_partition(struct emmc_chip *chip, int id);
void emmc_set_boot_partition(struct emmc_chip *chip, int id);
int emmc_initialize(struct emmc_registers *regs,
		struct emmc_chip  *chip,
		int boot_flag,
		struct emmc_host_config *host_config);

#endif /*__DEV_EMMC_CORE_H__ */
