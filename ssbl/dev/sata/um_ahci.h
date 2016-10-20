/*
 * Copyright (c) 2013, Marc Carino
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __UM_AHCI_H__
#define __UM_AHCI_H__

#include "os.h"
#include "config.h"

#if !defined(__NONOS)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#endif

/*  *********************************************************************
    * Configuration
    ********************************************************************* */
#define DEBUG			0
#define PRINT_REG_ACCESSES	0
#define RELAXED_MEM_ORDER	0

/*  *********************************************************************
    * Common defines
    ********************************************************************* */
#define PAGE_SIZE			(4096)
#define SECS_TO_TICKS(x)		((x) * 1000000)
#define MS_TO_TICKS(x)			((x) * 1000)
#define TIMEOUT_TICKS_5S		SECS_TO_TICKS(5)
#if defined(__SINGLE_THREADED)
# define NUM_TAGS			(1)
#else
# define NUM_TAGS			(32)
#endif
#define NUM_CDBS			(64)
#define NUM_CMD_LIST_HDRS		(NUM_TAGS)
#define DEF_NON_Q_TAG			(0)
#define LOGICAL_SECTOR_SZ		(512)
#define INV_CDB_IDX			(-1)
#define INV_TAG				(-1)

#define SATA_TOP_CTRL_BUS_CTRL		(0x44)
#define SATA_AHCI_GHC_HBA_CAP		(0x2000)
#define SATA_AHCI_GHC_PORTS_IMPLEMENTED	(0x200c)

#define SATA_FIRST_PORT_CTRL		(0x700) /* offset from SATA_AHCI_GHC */
#define SATA_NEXT_PORT_CTRL_OFFSET	(0x80)
#define SATA_PORT_PCTRL6(base)		(base + 0x18)

/*  *********************************************************************
    * Utility macros
    ********************************************************************* */
#define func_printf(format, ...) \
	printf("%20.20s: "format, __func__, ##__VA_ARGS__)
#define err_printf(format, ...) \
	fprintf(stderr, "%14.14s @ %d: "format, __func__, __LINE__, ##__VA_ARGS__)
#if (DEBUG)
#define dbg_printf(format, ...) \
	printf("%14.14s @ %d: "format, __func__, __LINE__, ##__VA_ARGS__)
#else
#define dbg_printf(format, ...)
#endif

#if (RELAXED_MEM_ORDER)
#define barrier()
#else
#define barrier() __asm__("dsb")
#endif

#define BIT(x) (1 << (x))
/* TODO: Need to figure out where to get the phys base */
#define VIRT_TO_PHYS(x) (((uint32_t)(x)) - g_base + 0xf0458000)

#define for_each_tag(tag) \
	for ((tag) = 0; (tag) < NUM_TAGS; (tag)++)

#define for_each_port(port) \
	for ((port) = 0; (port) < MAX_SATA_PHY_PORTS; (port)++)

#define for_each_bd(bd_curr, bd_head) \
	for ((bd_curr) = (bd_head); (bd_curr) != NULL; bd_curr = bd_curr->next)

enum {
	ERR_BUSY		= -1,
	ERR_TIMEOUT		= -2,
	ERR_INV_PTR		= -3,
	ERR_NOMEM		= -4,
	ERR_INV_TAG		= -5,
	ERR_PORT_MISMATCH	= -6,
	ERR_UNIMPL		= -7,
	ERR_RESET_FAIL		= -8,
	ERR_NO_DRIVE		= -9,
	ERR_DEV_NOT_INIT	= -10,
	ERR_TAG_ALREADY_PUSHED  = -11,
	ERR_BUSY_WILL_CALLBACK  = -12,
	ERR_BUSY_RETRY_LATER    = -13,
	ERR_BUSY_NO_TAGS        = -14,
	ERR_MEMPOOL_ALIGNMENT   = -15,
	ERR_MEMPOOL_NOT_INIT    = -16,
	ERR_MEMPOOL_NO_FREE     = -17,
	ERR_MEMPOOL_INIT_FAIL   = -18,
	ERR_MEMPOOL_INIT_INV_SZ = -19,
	ERR_TASKFILE            = -20,
	ERR_CDB_POOL_INIT_FAIL  = -21,
	ERR_NO_TAGS_FREE        = -22,
};

enum {
	RESET_SW = 0,
	RESET_PORT,
	RESET_HBA
};

enum {
	ATA_NOP			= 0x00,
	ATA_READ_SECTORS	= 0x20,
	ATA_READ_SECTORS_EXT	= 0x24,
	ATA_WRITE_SECTORS	= 0x30,
	ATA_WRITE_SECTORS_EXT	= 0x34,
	ATA_RFPDMA_Q		= 0x60,
	ATA_WFPDMA_Q		= 0x61,
	ATA_FLUSH_CACHE		= 0xE7,
	ATA_FLUSH_CACHE_EXT	= 0xEA,
	ATA_STANDBY_IMMEDIATE	= 0xE0,
	ATA_IDENTIFY_DEVICE	= 0xEC,
	ATA_SET_FEATURES	= 0xEF,
	__ATA_OPCODES_END
};

enum {
	RSTAT_BSY      = 0x80,
	RSTAT_DRDY     = 0x40,
	RSTAT_DRQ      = 0x10,
	RSTAT_ERR      = 0x01,
	RSTAT_MASK_OK  = RSTAT_DRDY | RSTAT_DRQ,
	RSTAT_MASK_ERR = RSTAT_DRDY | RSTAT_ERR
};

enum {
	RERR_ICRC     = 0x80,
	RERR_UNC      = 0x40,
	RERR_IDNF     = 0x10,
	RERR_ABRT     = 0x04,
	RERR_MASK_ERR = 0xFF
};

typedef struct {
	enum {
		TYPE_PIO	= 0,
		TYPE_DMA,
		TYPE_NON_DATA
	} type : 2;
	enum {
		DIR_DATA_IN	= 0,
		DIR_DATA_OUT,
		DIR_NON_DATA
	} dir : 2;
	enum {
		CMD_NON_Q	= 0,
		CMD_Q
	} is_q : 1;
	enum {
		LEN_28BIT	= 0,
		LEN_48BIT
	} len : 1;
	uint8_t rsvd : 2;
} __attribute__ ((packed)) ata_cmd_attrib_t;

static const ata_cmd_attrib_t ata_cmd_attribs[__ATA_OPCODES_END] = {
	[ATA_NOP]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_NON_Q, LEN_28BIT, 0},
	[ATA_READ_SECTORS]
	= {TYPE_PIO,      DIR_DATA_IN,  CMD_NON_Q, LEN_28BIT, 0},
	[ATA_READ_SECTORS_EXT]
	= {TYPE_PIO,      DIR_DATA_IN,  CMD_NON_Q, LEN_48BIT, 0},
	[ATA_WRITE_SECTORS]
	= {TYPE_PIO,      DIR_NON_DATA, CMD_NON_Q, LEN_28BIT, 0},
	[ATA_WRITE_SECTORS_EXT]
	= {TYPE_PIO,      DIR_NON_DATA, CMD_NON_Q, LEN_48BIT, 0},
	[ATA_RFPDMA_Q]
	= {DIR_DATA_IN,   DIR_DATA_IN,  CMD_Q,     LEN_48BIT, 0},
	[ATA_WFPDMA_Q]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_Q,     LEN_48BIT, 0},
	[ATA_FLUSH_CACHE]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_NON_Q, LEN_28BIT, 0},
	[ATA_FLUSH_CACHE_EXT]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_NON_Q, LEN_48BIT, 0},
	[ATA_STANDBY_IMMEDIATE]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_NON_Q, LEN_28BIT, 0},
	[ATA_IDENTIFY_DEVICE]
	= {TYPE_PIO,      DIR_DATA_IN,  CMD_NON_Q, LEN_28BIT, 0},
	[ATA_SET_FEATURES]
	= {TYPE_NON_DATA, DIR_NON_DATA, CMD_NON_Q, LEN_28BIT, 0}
};

#define CMD_DIR(opcode) ata_cmd_attribs[opcode].dir
#define CMD_IS_Q(opcode) ata_cmd_attribs[opcode].is_q

/*  *********************************************************************
    *  Typedefs
    ********************************************************************* */

#define H2D_FIS_0_FIS_TYPE	(0)
#define H2D_FIS_0_FLAGS		(8)
#define H2D_FIS_0_FLAGS_C	(H2D_FIS_0_FLAGS + 7)
#define H2D_FIS_0_CMD		(16)
#define H2D_FIS_0_FEAT_7_0	(24)
#define H2D_FIS_1_LBA_7_0	(0)
#define H2D_FIS_1_LBA_15_8	(8)
#define H2D_FIS_1_LBA_23_16	(16)
#define H2D_FIS_1_DEVICE	(24)
#define H2D_FIS_2_LBA_31_24	(0)
#define H2D_FIS_2_LBA_39_32	(8)
#define H2D_FIS_2_LBA_47_40	(16)
#define H2D_FIS_2_FEAT_15_8	(24)
#define H2D_FIS_3_COUNT_7_0	(0)
#define H2D_FIS_3_COUNT_15_8	(8)
#define H2D_FIS_3_ICC		(16)
#define H2D_FIS_3_CONTROL	(24)
#define H2D_FIS_TYPE_VALUE	(0x27)
typedef union {
	struct {
		uint8_t fis_type;
		union {
			struct {
				uint8_t rsvd0 : 7;
				uint8_t c : 1;
			} bits;
			uint8_t all;
		} flags;
		uint8_t cmd;
		uint8_t feat7_0;

		uint8_t lba7_0;
		uint8_t lba15_8;
		uint8_t lba23_16;
		uint8_t device;

		uint8_t lba31_24;
		uint8_t lba39_32;
		uint8_t lba47_40;
		uint8_t feat15_8;

		uint8_t count7_0;
		uint8_t count15_8;
		uint8_t icc;
		union {
			struct {
				uint8_t rsvd0 : 2;
				uint8_t srst : 1;
				uint8_t rsvd1 : 5;
			} bits;
			uint8_t all;
		} control;
		uint8_t rsvd0[4];
	} fields;
	uint32_t dw[5];
} h2d_fis_t;

typedef union {
	struct {
		uint8_t fis_type;
		uint8_t flags;
		uint8_t status;
		uint8_t error;

		uint8_t lba7_0;
		uint8_t lba15_8;
		uint8_t lba23_16;
		uint8_t device;

		uint8_t lba31_24;
		uint8_t lba39_32;
		uint8_t lba47_40;
		uint8_t rsvd0;

		uint8_t count7_0;
		uint8_t count15_8;
		uint8_t rsvd1[2];

		uint8_t rsvd2[4];
	} fields;
	uint32_t dw[5];
} d2h_fis_t;

typedef struct {
	uint8_t type;
	uint8_t flags;
	uint8_t status;
	uint8_t error;
	uint32_t protocol;
} sdb_fis_t;

typedef union {
	struct {
		uint8_t fis_type;
		uint8_t flags;
		uint8_t status;
		uint8_t error;

		uint8_t lba7_0;
		uint8_t lba15_8;
		uint8_t lba23_16;
		uint8_t device;

		uint8_t lba31_24;
		uint8_t lba39_32;
		uint8_t lba47_40;
		uint8_t rsvd0;

		uint8_t count7_0;
		uint8_t count15_8;
		uint8_t rsvd1;
		uint8_t e_status;

		uint16_t xfer_cnt;
		uint8_t rsvd2[2];
	} fields;
	uint32_t dw[5];
} pio_setup_fis_t;

typedef struct {
	uint32_t dw[7];
} dma_setup_fis_t;

typedef struct {
	uint32_t dw[4];
} phys_region_desc_t;

typedef struct {
	dma_setup_fis_t		dma_setup;
	uint32_t		reserved_1c;
	pio_setup_fis_t		pio_setup;
	d2h_fis_t		d2h;
	sdb_fis_t		sdb;
	uint32_t		unk[0xA0 / 4];
} ahci_rx_fises_t;

#define CLH_DW0_CFIS_LEN	(0)
#define CLH_DW0_WRITE		(6)
#define CLH_DW0_RESET		(8)
#define CLH_DW0_CLR_BSY_ON_R_OK	(10)
#define CLH_DW0_PRDT_LEN	(16)
typedef struct {
	uint32_t dw0;
	uint32_t prd_byte_cnt;
	uint32_t cmd_tbl_base;
	uint32_t cmd_tbl_base_upper;
	uint32_t dw4_7[4];
} ahci_cmd_list_hdr_t;

#define MAX_PRDT 8
typedef struct {
	h2d_fis_t cfis;
	uint32_t rsvd0[0x2C / 4];
	uint32_t rsvd1[0x10 / 4];
	uint32_t rsvd2[0x30 / 4];
	phys_region_desc_t prdt[MAX_PRDT];
} ahci_cmd_tbl_t;

#define HBA_GHC_CAP_HR		(0)
#define HBA_GHC_CAP_IE		(1)
#define HBA_GHC_CAP_MRSM	(2)
#define HBA_GHC_CAP_AE		(31)
typedef union {
	struct {
		uint32_t hba_rst : 1;
		uint32_t int_en : 1;
		uint32_t mrsm : 1;
		uint32_t rsvd0 : 28;
		uint32_t ahci_en : 1;
	} bits;
	uint32_t all;
} hba_ghc_cap_t;

typedef struct {
#define HBA_GHC_CAP_HR		(0)
	union {
		struct {
			uint32_t num_of_ports : 5;
			uint32_t rsvd0 : 27;
		} bits;
		uint32_t all;
	} cap;
	hba_ghc_cap_t ghc;
	uint32_t is;
	uint32_t pi;
	uint32_t vs;
	uint32_t ccc_ctl;
	uint32_t ccc_ports;
	uint32_t em_loc;
	uint32_t em_ctl;
	uint32_t cap2;
	uint32_t bohc;
	uint32_t rsvd[(0x100 - 0x2C) / 4]; /* future, nvmhci, etc... */
} ahci_hba_ghc_regs_t;

#define PXIS_DHRS	(0)
#define PXIS_PSS	(1)
#define PXIS_DSS	(2)
#define PXIS_SDBS	(3)
#define PXIS_UFS	(4)
#define PXIS_DPS	(5)
#define PXIS_PCS	(6)
#define PXIS_DMPS	(7)
#define PXIS_PRCS	(22)
#define PXIS_IPMS	(23)
#define PXIS_OFS	(24)
#define PXIS_INFS	(26)
#define PXIS_IFS	(27)
#define PXIS_HBDS	(28)
#define PXIS_HBFS	(29)
#define PXIS_TFES	(30)
#define PXIS_CPDS	(31)
typedef union {
	struct {
		uint32_t d2h_fis_int : 1;
		uint32_t pio_setup_fis_int : 1;
		uint32_t dma_setup_fis_int : 1;
		uint32_t sdb_fis_int : 1;
		uint32_t unk_fis_int : 1;
		uint32_t desc_processed : 1;
		uint32_t port_connect_chg_stat : 1;
		uint32_t dev_mech_presence_stat : 1;
		uint32_t rsvd0 : 14;
		uint32_t phy_rdy_chg_stat : 1;
		uint32_t inc_port_mult_stat : 1;
		uint32_t overflow_stat : 1;
		uint32_t rsvd1 : 1;
		uint32_t iface_notfatal_err_stat : 1;
		uint32_t iface_fatal_err_stat : 1;
		uint32_t hbus_data_err_stat : 1;
		uint32_t hbus_fatal_err_stat : 1;
		uint32_t tf_err_stat : 1;
		uint32_t cold_port_det_stat : 1;
	} bits;
	uint32_t all;
} pxis_int_stat_t;

#define PXCMD_ST		(0)
#define PXCMD_SUD		(1)
#define PXCMD_POD		(2)
#define PXCMD_CLO		(3)
#define PXCMD_FRE		(4)
#define PXCMD_CCS		(8)
#define PXCMD_MPSS		(13)
#define PXCMD_FR		(14)
#define PXCMD_CR		(16)
#define PXCMD_CPS		(16)
#define PXCMD_PMA		(17)
#define PXCMD_HPCP		(18)
#define PXCMD_MPSP		(19)
#define PXCMD_CPD		(20)
#define PXCMD_ESP		(21)
#define PXCMD_FBSCP		(22)
#define PXCMD_APSTE		(23)
#define PXCMD_ATAPI		(24)
#define PXCMD_DLAE		(25)
#define PXCMD_ALPE		(26)
#define PXCMD_ASP		(27)
#define PXCMD_ICC		(28)
typedef union {
	struct {
		uint32_t start : 1;
		uint32_t spin_up : 1;
		uint32_t power_on : 1;
		uint32_t cmd_list_override : 1;
		uint32_t fis_rx_en : 1;
		uint32_t rsvd0 : 3;
		uint32_t curr_cmd_slot : 5;
		uint32_t mech_pres_sw_state : 1;
		uint32_t fis_rx_running : 1;
		uint32_t cmd_list_running : 1;
		uint32_t cold_presence_state : 1;
		uint32_t port_mult_attached : 1;
		uint32_t hot_plug_capable : 1;
		uint32_t mech_pres_sw_attached : 1;
		uint32_t cold_presence_detect : 1;
		uint32_t esata : 1;
		uint32_t fis_based_sw_capable : 1;
		uint32_t apts_en : 1;
		uint32_t is_atapi : 1;
		uint32_t led_on_atapi_en : 1;
		uint32_t aggro_link_pwr_mgmt : 1;
		uint32_t aggro_slumber_partial : 1;
		uint32_t icc : 4;
	} bits;
	uint32_t all;
} pxcmd_cmd_n_stat_t;

#define PXTFD_TF_STS_ERR (0)
#define PXTFD_TF_STS_DRQ (3)
#define PXTFD_TF_STS_BSY (7)
typedef union {
	struct {
		union {
			struct {
				uint8_t err : 1;
				uint8_t cs0 : 2;
				uint8_t drq : 1;
				uint8_t cs1 : 3;
				uint8_t bsy : 1;
			} bits;
			uint8_t all;
		} status;
		uint8_t error;
		uint16_t rsvd0;
	} fields;
	uint32_t all;
} pxtfd_tf_data_t;

#define PXSSTS_SCR0_DET (0)
#define PXSSTS_SCR0_SPD (4)
#define PXSSTS_SCR0_IPM (8)
#define SCR2_DET_DEVICE_AND_COMM_OK (0x3)
typedef union {
	struct {
		uint32_t det : 4;
		uint32_t spd : 4;
		uint32_t ipm : 4;
		uint32_t rsvd0 : 20;
	} bits;
	uint32_t all;
} pxssts_scr0_sstatus_t;

#define PXSSTS_SCR2_DET (0)
#define PXSSTS_SCR2_SPD (4)
#define PXSSTS_SCR2_IPM (8)
#define SCR2_DET_PERFORM_COMINIT (0x1)
#define SCR2_IPM_DISALLOW_PARTIAL_SLUMBER (0x3)
typedef union {
	struct {
		uint32_t det : 4;
		uint32_t spd : 4;
		uint32_t ipm : 4;
		uint32_t rsvd0 : 20;
	} bits;
	uint32_t all;
} pxsctl_scr2_scontrol_t;


#define PXSERR_SCR1_ERR_I  (0)
#define PXSERR_SCR1_ERR_M  (1)
#define PXSERR_SCR1_ERR_T  (8)
#define PXSERR_SCR1_ERR_C  (9)
#define PXSERR_SCR1_ERR_P  (10)
#define PXSERR_SCR1_ERR_E  (11)
#define PXSERR_SCR1_DIAG_N (16)
#define PXSERR_SCR1_DIAG_I (17)
#define PXSERR_SCR1_DIAG_W (18)
#define PXSERR_SCR1_DIAG_B (19)
#define PXSERR_SCR1_DIAG_D (20)
#define PXSERR_SCR1_DIAG_C (21)
#define PXSERR_SCR1_DIAG_H (22)
#define PXSERR_SCR1_DIAG_S (23)
#define PXSERR_SCR1_DIAG_T (24)
#define PXSERR_SCR1_DIAG_F (25)
#define PXSERR_SCR1_DIAG_X (26)
typedef union {
	union {
		struct {
			uint16_t recov_data_integ_err : 1;
			uint16_t recov_comm_err : 1;
			uint16_t trans_data_integ_err : 1;
			uint16_t persis_com_dat_integ_err : 1;
			uint16_t proto_err : 1;
			uint16_t internal_err : 1;
		} bits;
		uint16_t all;
	} err;
	union {
		struct {
			uint16_t phyrdy_chg : 1;
			uint16_t phy_int_err : 1;
			uint16_t comm_wake : 1;
			uint16_t ten_b_8_b_dec_err : 1;
			uint16_t disparity_err : 1;
			uint16_t crc_err : 1;
			uint16_t handshake_err : 1;
			uint16_t link_seq_err : 1;
			uint16_t xport_stat_trans_err : 1;
			uint16_t unk_fis_type : 1;
			uint16_t eXchanged : 1;
		} bits;
		uint16_t all;
	} diag;
	uint32_t all;
} pxserr_scr1_serror_t;

typedef struct {
	uint32_t               cmd_list_base;
	uint32_t               cmd_list_base_upper;
	uint32_t               fis_base;
	uint32_t               fis_base_upper;
	pxis_int_stat_t        int_status;
	pxis_int_stat_t        int_enable;
	pxcmd_cmd_n_stat_t     cmd_and_stat;
	uint32_t               rsvd0;
	pxtfd_tf_data_t        tf_data;
	uint32_t               signature;
	pxssts_scr0_sstatus_t  sstatus;
	pxsctl_scr2_scontrol_t scontrol;
	pxserr_scr1_serror_t   serror;
	uint32_t               sactive;
	uint32_t               ci;
	uint32_t               snotification;
	uint32_t               fbs_control;
	uint32_t               rsvd1[0x2C / 4];
	uint32_t               vs[0x10 / 4];
} ahci_port_regs_t;

typedef struct {
	mutex_t             mtx;
	uint32_t            reg_base;
	ahci_hba_ghc_regs_t *hba_regs;
	ahci_port_regs_t    *port_regs[MAX_SATA_PHY_PORTS];
	void                *phy_regs[MAX_SATA_PHY_PORTS];
	uint32_t            tags_allocated[MAX_SATA_PHY_PORTS];
	uint32_t            tags_started[MAX_SATA_PHY_PORTS];
	int                 non_q_barrier[MAX_SATA_PHY_PORTS];
	struct {
		uint32_t is_ready : 1;
		uint32_t rsvd0    : 31;
	} status;
	int (*irq_wait)(void);
	void (*irq_en)(int);
} sata_dev_t;

struct sata_cdb_t;
typedef void (*callback_t)(struct sata_cdb_t (*), int);

typedef struct bfr_desc_t {
	uint64_t pa;
	void     *va;
	uint32_t sz;
	struct bfr_desc_t *next;
} bfr_desc_t;

typedef struct sata_cdb_t {
	sata_dev_t *dev;

	int port;
	int tag;

	int opcode;
	int lba;
	int block_cnt;

	uint32_t bfr_ofs;
	uint32_t bfr_len;

	callback_t  callback;
	bfr_desc_t  *bd;

	uint32_t flags;

	pxtfd_tf_data_t tf_data;

	int cdb_idx;

	uint32_t start;
} sata_cdb_t;

typedef struct {
	uint64_t pa_base;
	void     *va_base;
	uint32_t sz;
	uint32_t *bitmap;
	uint32_t bitmap_sz;
} mem_pool_t;

typedef struct {
	sata_cdb_t cdbs[NUM_CDBS];
	uint64_t bitmap;

	int evt;
	mutex_t mtx;
	cond_t cnd;
} cdb_pool_t;

typedef struct {
	sata_dev_t *dev;
	uint32_t reg_base;
	uint32_t *mem_base_va;
	uint64_t mem_base_pa;
	uint32_t mem_base_sz;
	int (*irq_wait)(void);
	void (*irq_en)(int);
} um_ahci_cfg_t;

void init_phy(int port, uint32_t base);
int sata_reset(sata_dev_t *dev, int port, int reset_type);
int sata_scan(sata_dev_t *dev, uint32_t ports_to_scan, uint32_t *devs_found);
int um_ahci_core_init(const um_ahci_cfg_t *cfg);
int sata_cdb_pop(sata_dev_t *dev, int port, sata_cdb_t **cdb);
int sata_cdb_push(sata_cdb_t *cdb);
int sata_cmd_issue(sata_cdb_t *cdb);
int sata_cmd_cleanup(sata_cdb_t *cdb);
int mem_pool_alloc(int size, uint32_t alignment, bfr_desc_t *bd);
void mem_pool_free(bfr_desc_t *bd);

#endif /* __UM_AHCI_H__ */
