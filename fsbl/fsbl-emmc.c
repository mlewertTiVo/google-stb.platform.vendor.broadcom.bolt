/***************************************************************************
 *     Copyright (c) 2016-, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "fsbl-emmc.h"
#include "fsbl.h"

#include <stdbool.h>
#include <common.h>
#include <lib_types.h>

#include <dev_emmc_core_defs.h>
#include <sdhc.h>
#include <emmc_cst.h>
#include <config.h> /* For EMMC_BUS_VOLTAGE */
#include <byteorder.h>

#include <bchp_common.h>
#include <bchp_sdio_1_boot.h>
#include <bchp_sdio_1_cfg.h>
#include <bchp_sdio_1_host.h>


/* Debug options */
#define DEBUG_EMMC_FSBL	0	/* 1:enable, 0:disable */
#define DEBUG_EMMC_RDWR	0
#define DEBUG_EMMC_DATA	0

#if DEBUG_EMMC_FSBL
#define DBG_MSG_PUTS(...) __puts(__VA_ARGS__)
#define DBG_MSG_WR_INT(...) writeint(__VA_ARGS__)
#define DBG_MSG_WR_HEX(...) writehex(__VA_ARGS__)
#else
#define DBG_MSG_PUTS(...)
#define DBG_MSG_WR_INT(...)
#define DBG_MSG_WR_HEX(...)
#endif


#define EMMC_OK 0
#define EMMC_NG 1

/* op-code issue related timeout values */
#define POLL_DELAY_USECS		1
#define CMD_TIMEOUT_USECS		200000
#define CMD_SLEEP_CNT			(CMD_TIMEOUT_USECS / POLL_DELAY_USECS)
#define DATA_TIMEOUT_USECS		1000000
#define DATA_SLEEP_CNT			(DATA_TIMEOUT_USECS / POLL_DELAY_USECS)
#define SOFTWARE_RESET_TIMEOUT_USECS	1000000 /* 1 second */

#define HOST_BUF_SIZE	7
			/* 0: 4K
			 * 1: 8K
			 * 2: 16K
			 * 3: 32K
			 * 4: 64K
			 * 5: 128K
			 * 6: 256K 
			 * 7: 512K - Double Buffering
			 */

#define EMMC_DMA_BUF_ALIGN  512


/*
 * CMD index, argument
*/
struct mmc_cmd {
	int index;
	uint32_t arg;
};

/*
 * MMC data info
*/
struct mmc_data {
	uint32_t dma_address;
	uint32_t block_count;
	bool     dma_used;
};

struct cmd_info {
	uint8_t index;			/* CMD0 - CMD63 */
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
#define IDX_CMD0	0
#define IDX_CMD1	1
#define IDX_CMD2	2
#define IDX_CMD3	3
#define IDX_CMD6	4
#define IDX_CMD7	5
#define IDX_CMD13	6
#define IDX_CMD16	7
#define IDX_CMD18	8
#define IDX_CMD_MAX	9
static const struct cmd_info emmc_cmd_table[IDX_CMD_MAX] = {
	{
		/* CMD0, idx0 */
		0, CMDINFO_TYPE_BC, CMDINFO_RESP_NONE, CMDINFO_DXFER_NONE,
		"GO_IDLE_STATE"
	},
	{
		/* CMD1, idx1 */
		1, CMDINFO_TYPE_BCR, CMDINFO_RESP_R3, CMDINFO_DXFER_NONE,
		"SEND_OP_COND"
	},
	{
		/* CMD2, idx2 */
		2, CMDINFO_TYPE_BCR, CMDINFO_RESP_R2, CMDINFO_DXFER_NONE,
		"ALL_SEND_CID"
	},
	{
		/* CMD3, idx3 */
		3, CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,
		"SET_RELATIVE_ADDR"
	},
	{
		/* CMD6, idx4 */
		6, CMDINFO_TYPE_AC, CMDINFO_RESP_R1B, CMDINFO_DXFER_NONE,
		"SWITCH"
	},
	{
		/* CMD7, idx5 */
		7, CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,
		"SELECT_CARD"
	},
	{
		/* CMD13, idx6 */
		13, CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,
		"SEND_STATUS"
	},
	{
		/* CMD16, idx7 */
		16, CMDINFO_TYPE_AC, CMDINFO_RESP_R1, CMDINFO_DXFER_NONE,
		"SET_BLOCKLEN"
	},
	{
		/* CMD18, idx8 */
		18, CMDINFO_TYPE_ADTC, CMDINFO_RESP_R1,
		CMDINFO_DXFER_READ_MULTI, "READ_MULTIPLE_BLOCK"
	}
};


/*
 * CID(Device IDentification) for CMD1
*/
uint32_t raw_cid[4];

static int emmc_cmd13_send_status(uint32_t *status);

/*
 * Functions.
*/
static void pio_read_blk(uint32_t block_size, unsigned long *dst_addr)
{
	int cnt = block_size / sizeof(unsigned long);
	uint32_t val;

	while (cnt--) {
		val = BDEV_RD(BCHP_SDIO_1_HOST_BUFFDATA);
		*dst_addr++ = le32_to_cpu(val);
	}
}

static void pio_transfer(struct mmc_data *data)
{
	while (data->block_count) {
		if ((BDEV_RD(BCHP_SDIO_1_HOST_STATE) &
			SDHC_STATE_BUF_READ_RDY) == 0)
			break;
		pio_read_blk(EMMC_BLOCK_SIZE, (unsigned long *)data->dma_address);
		data->dma_address += EMMC_BLOCK_SIZE;
		data->block_count--;
	}
}


static void handle_interrupt_error(void)
{
	uint32_t status;

	status = BDEV_RD(BCHP_SDIO_1_HOST_INT_STATUS);
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS, status);
}

static int wait_cmd_complete(void)
{
	uint32_t timeout_cnt = 0;
	uint32_t int_status;

	/* Wait for COMMAND COMPLETE or error interrupt */
	timeout_cnt = 0;
	while (++timeout_cnt <= (CMD_TIMEOUT_USECS / POLL_DELAY_USECS)) {
		int_status = BDEV_RD(BCHP_SDIO_1_HOST_INT_STATUS) &
			(SDHC_INT_CMD_COMPLETE | SDHC_INT_ERR);
		if (int_status)
			break;
		udelay(POLL_DELAY_USECS);
	}
	if (timeout_cnt > CMD_SLEEP_CNT) {
		__puts(" timed out waiting for cmd complete, intr = ");
		writehex(int_status);
		__puts("\n");
		return EMMC_NG;
	}

	/* Check for clean completion */
	if (int_status & SDHC_INT_ERR) {
		handle_interrupt_error();
		return EMMC_NG;
	}

	/* Clear the interrupt for next time */
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS, int_status);
	return EMMC_OK;
}

static int wait_xfer_complete(struct mmc_data *data)
{
	uint32_t int_status;
	uint32_t timeout = 0;

	DBG_MSG_PUTS(" wait_xfer_complete\n");
	while (1) {
		int_status = BDEV_RD(BCHP_SDIO_1_HOST_INT_STATUS);
		if (int_status & SDHC_INT_XFER_COMPLETE)
			break;
		if (int_status & SDHC_INT_XFER_DMA) {
			DBG_MSG_PUTS(" DMA interrupt, SDMA addr\n");
			/* Clear the DMA interrupt */
			BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS,
				int_status & SDHC_INT_XFER_DMA);
			/*
			 * write the address back to the address
			 * to restart the DMA.
			 */
			BDEV_WR(BCHP_SDIO_1_HOST_SDMA,
				BDEV_RD(BCHP_SDIO_1_HOST_SDMA));
		}
		if (int_status & SDHC_INT_READ_BUF) {
			DBG_MSG_PUTS(" PIO interrupt\n");
			pio_transfer(data);
		}
		if (int_status & SDHC_INT_ERR_MASK) {
			DBG_MSG_PUTS(" transfer complete intr error : 0x");
			DBG_MSG_WR_HEX(int_status);
			return EMMC_NG;
		}
		if (++timeout >= DATA_SLEEP_CNT) {
			DBG_MSG_PUTS(" transfer complete timed out, intr : 0x");
			DBG_MSG_WR_HEX(int_status);
			return EMMC_NG;
		}
		udelay(POLL_DELAY_USECS);
	}

	DBG_MSG_PUTS(" OK: intr=0x");
	DBG_MSG_WR_HEX(int_status);
	DBG_MSG_PUTS(", timeout=");
	DBG_MSG_WR_INT(timeout);
	return EMMC_OK;
}

static int wait_ready_data_xfer(void)
{
	uint32_t emmc_status = 0;
	uint32_t emmc_state = 0;
	uint32_t timeout_cnt = 0;
	uint32_t cst = 0;

	do {
		if (emmc_cmd13_send_status(&cst))
			return EMMC_NG;
		emmc_status = (cst & CST_READY_FOR_DATA_MASK) >>
			CST_READY_FOR_DATA_SHIFT;
		emmc_state = (cst & CST_CURRENT_STATE_MASK) >>
			CST_CURRENT_STATE_SHIFT;
		if ((emmc_status == 1) && (emmc_state == CST_STATE_TRAN))
			break;
		udelay(POLL_DELAY_USECS);
		timeout_cnt++;
	} while (timeout_cnt < DATA_SLEEP_CNT);
	if (timeout_cnt >= DATA_SLEEP_CNT) {
		DBG_MSG_PUTS("Timeout(1000000cnt) waiting for data xfer ready");
		DBG_MSG_PUTS(", CST: 0x");
		DBG_MSG_WR_HEX(cst);
		DBG_MSG_PUTS("\n");
		return EMMC_NG;
	}
	DBG_MSG_PUTS("[wait_ready_data_xfer], CST: 0x");
	DBG_MSG_WR_HEX(cst);
	DBG_MSG_PUTS("\n");

	return EMMC_OK;
}


/*
 * wait_inhibit_clear
*/
static int wait_inhibit_clear(int data_inhibit)
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
		if ((BDEV_RD(BCHP_SDIO_1_HOST_STATE) & mask) == 0)
			break;
		if (timeout-- == 0)
			break;
		udelay(POLL_DELAY_USECS);
	}
	if (timeout < 0) {
		__puts("Timeout waiting for INHIBIT lines to clear: 0x");
		writehex(BDEV_RD(BCHP_SDIO_1_HOST_STATE));
		return EMMC_NG;
	}
	return EMMC_OK;
}


/*
 * get_cmd_mode
*/
static uint32_t get_cmd_mode(int index)
{
	uint32_t cmd_mode = 0;
	const struct cmd_info *entry = &emmc_cmd_table[index];

	cmd_mode = ((uint32_t)entry->index & 0x3f)
			<< BCHP_SDIO_1_HOST_CMD_MODE_CMD_INDEX_SHIFT;
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
	if (entry->data_xfer != CMDINFO_DXFER_NONE)
		cmd_mode |= (SDHC_CMD_DATA | SDHC_MODE_DMA);

	/*
	 * If multiblock, set multi bit, enable block cnt register
	 * and enable Auto CMD12
	 */
	if ((entry->data_xfer == CMDINFO_DXFER_READ_MULTI) ||
	    (entry->data_xfer == CMDINFO_DXFER_WRITE_MULTI)) {
		cmd_mode |= (SDHC_MODE_MULTI | SDHC_MODE_ACMD12
				| SDHC_MODE_BLK_CNT);
	}

	/* bit is zero for write, 1 for read */
	if ((entry->data_xfer == CMDINFO_DXFER_READ) ||
	    (entry->data_xfer == CMDINFO_DXFER_READ_MULTI))
		cmd_mode |= SDHC_MODE_XFER_DIR_READ;

	return cmd_mode;
}

static int emmc_software_reset(uint32_t mask)
{
	int cnt;

	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET1, BDEV_RD(BCHP_SDIO_1_HOST_CTRL_SET1)
		| mask);

	cnt = SOFTWARE_RESET_TIMEOUT_USECS;
	while (cnt > 0) {
		udelay(POLL_DELAY_USECS);
		if ((BDEV_RD(BCHP_SDIO_1_HOST_CTRL_SET1) & mask) == 0)
			return EMMC_OK;
		cnt -= POLL_DELAY_USECS;
	}
	__puts("Host RESET timeout.");
	DBG_MSG_PUTS("Host RESET timeout waiting for 0x%x");
	DBG_MSG_WR_HEX(BDEV_RD(BCHP_SDIO_1_BOOT_STATUS) & mask);
	return EMMC_NG;
}

/*
 * Issue command
*/
static int issue_cmd(struct mmc_cmd *cmd, struct mmc_data *data)
{
	uint32_t cmd_mode;
	const struct cmd_info *entry = &emmc_cmd_table[cmd->index];
	int data_xfer;

	data_xfer = (entry->data_xfer != CMDINFO_DXFER_NONE);
#if DEBUG_EMMC_FSBL
	if (data_xfer) {
		DBG_MSG_PUTS("\n dma addr : ");
		DBG_MSG_WR_HEX(data->dma_address);
		DBG_MSG_PUTS(", block size : ");
		DBG_MSG_WR_INT(EMMC_BLOCK_SIZE);
		DBG_MSG_PUTS(", blocks : ");
		DBG_MSG_WR_INT(data->block_count);
	}
	DBG_MSG_PUTS("\n");

	/* sanity check */
	if ((data != NULL) ^ (data_xfer)) {
		DBG_MSG_PUTS("entry->desc");
		DBG_MSG_PUTS("- data phase mismatch");
		goto err;
	}
#endif
	if (wait_inhibit_clear(data_xfer))
		goto err;

	/* Start with all interrupts cleared */
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS, HOST_INT_STATUS_ALL);

	/* The command includes a data transfer */
	if (data_xfer) {
		if (data != NULL && data->dma_used)
			BDEV_WR(BCHP_SDIO_1_HOST_SDMA, data->dma_address);
		BDEV_WR(BCHP_SDIO_1_HOST_BLOCK,
			SDHC_MAKE_BLK_REG(data->block_count,
					  EMMC_BLOCK_SIZE, HOST_BUF_SIZE));
	}
	cmd_mode = get_cmd_mode(cmd->index);

	/* If DMA is not  used, clear DMA enable bit */
	if (data != NULL && !(data->dma_used))
		cmd_mode &= ~BCHP_SDIO_1_HOST_CMD_MODE_DMA_ENABLE_MASK;

	BDEV_WR(BCHP_SDIO_1_HOST_ARGUMENT, cmd->arg);
#if DEBUG_EMMC_FSBL
	DBG_MSG_PUTS(" SDMA : 0x");
	DBG_MSG_WR_HEX(BDEV_RD(BCHP_SDIO_1_HOST_SDMA));
	DBG_MSG_PUTS(", BLOCK : ");
	DBG_MSG_WR_HEX(BDEV_RD(BCHP_SDIO_1_HOST_BLOCK));
	DBG_MSG_PUTS(", ARG : ");
	DBG_MSG_WR_HEX(BDEV_RD(BCHP_SDIO_1_HOST_ARGUMENT));
	DBG_MSG_PUTS(", CMD : ");
	DBG_MSG_WR_HEX(cmd_mode);
	DBG_MSG_PUTS("\n");
#endif
	BDEV_WR(BCHP_SDIO_1_HOST_CMD_MODE, cmd_mode);
	if (wait_cmd_complete())
		goto err;

	/* If the command has a data transfer, wait for transfer complete */
	if (data_xfer) {
		if (wait_xfer_complete(data))
			goto err;
	}
	/*
	 * If command is using BUSY signalling, also wait for DATA INHIBIT
	 * to clear.
	 */
	if (emmc_cmd_table[cmd->index].resp == CMDINFO_RESP_R1B)
		wait_inhibit_clear(1);

	DBG_MSG_PUTS("[issue_cmd] success\n");
	return EMMC_OK;

err:
	DBG_MSG_PUTS("[issue_cmd] error\n");
	emmc_software_reset(SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD);
	return EMMC_NG;
}


#define ARG_CMD0_GO_IDEL_STATE	0x00000000
static int emmc_cmd0_go_idle_state(void)
{
	int res;
	struct mmc_cmd cmd;

	/*
	 * CMD 0 - GO_IDLE_STATE (bc,-), GO_PRE_IDLE_STATE (bc,-),
	 * BOOT_INITIATION (-,-)
	 */
	cmd.index = IDX_CMD0;
	cmd.arg = ARG_CMD0_GO_IDEL_STATE;
	res = issue_cmd(&cmd, NULL);

	/* wait for card to settle */
	udelay(1000);
	return res;
}

/*
 * Get the card in idle state To send its Operating Conditions
 * Register (OCR) contents in the response
 */
static int emmc_cmd1_send_op_cond(uint32_t *ocr)
{
	struct mmc_cmd cmd;
	int res;

	cmd.index = IDX_CMD1;
	cmd.arg = OCR_SECTOR_MODE | OCR_VDD_33_34;
	res = issue_cmd(&cmd, NULL);
	*ocr = BDEV_RD(BCHP_SDIO_1_HOST_RESP_01);
	return res;
}

/*
 * Gets card CID (Card Identification) from eMMC device.
 */
static int  emmc_cmd2_all_send_cid_single_device(void)
{
	struct mmc_cmd cmd;
	int res;

	cmd.index = IDX_CMD2;
	cmd.arg = 0;
	res = issue_cmd(&cmd, NULL);
	if (res)
		return res;
	raw_cid[0] = BDEV_RD(BCHP_SDIO_1_HOST_RESP_01);
	raw_cid[1] = BDEV_RD(BCHP_SDIO_1_HOST_RESP_23);
	raw_cid[2] = BDEV_RD(BCHP_SDIO_1_HOST_RESP_45);
	raw_cid[3] = BDEV_RD(BCHP_SDIO_1_HOST_RESP_67);
	return res;
}

/*
 * Set relative card address
 */
static int emmc_cmd3_set_rca(void)
{
	struct mmc_cmd cmd;

	/* CMD3, R1 (Index check  On, CRC check	 On, 48bit) */
	cmd.index = IDX_CMD3;
	cmd.arg = EMMC_RCA;
	return issue_cmd(&cmd, NULL);
}

/*
 * Switch ExtCSD
 */
static int emmc_cmd6_switch_extcsd(uint32_t access,
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
	cmd.index = IDX_CMD6;
	cmd.arg = arg;
	res = issue_cmd(&cmd, NULL);
	if (res)
		return res;

	/* Wait until we transition from PRG to TRAN state */
	do {
		if (emmc_cmd13_send_status(&status))
			return EMMC_NG;
		if (status & CST_SWITCH_ERROR_MASK) {
			__puts("CMD 6 - SWITCH error in status: 0x");
			writehex(status);
			__puts("\n");
			return EMMC_NG;
		}
		emmc_state = (status & CST_CURRENT_STATE_MASK) >>
			CST_CURRENT_STATE_SHIFT;
		if (emmc_state == CST_STATE_TRAN)
			break;
		udelay(POLL_DELAY_USECS);
		timeout_cnt++;
	} while (timeout_cnt < DATA_SLEEP_CNT);
	if (timeout_cnt >= DATA_SLEEP_CNT) {
		__puts("Timeout waiting for TRAN state after SWITCH cmd, ");
		__puts("STATUS: 0x");
		writehex(status);
		__puts("\n");
		return EMMC_NG;
	}
	return EMMC_OK;
}

static int emmc_cmd7_select_card_stby_tans(void)
{
	struct mmc_cmd cmd;

	/*
	 * CMD7
	 * While selecting from Stand-By State to Transfer State
	 *   R1 (Index check  On, CRC check  On, 48bit)
	 * While selecting from Disconnected State to Programming State.
	 *   R1b(Index check  On, CRC check  On, 48bit/busy_check)
	 */
	cmd.index = IDX_CMD7;
	cmd.arg = EMMC_RCA;
	return issue_cmd(&cmd, NULL);
}

static int emmc_cmd13_send_status(uint32_t *status)
{
	struct mmc_cmd cmd;
	int res;

	/* CMD13, R1 (Index check  On, CRC check  On, 48bit) */
	cmd.index = IDX_CMD13;
	cmd.arg = EMMC_RCA;
	res = issue_cmd(&cmd, NULL);
	if (res)
		return res;
	*status = BDEV_RD(BCHP_SDIO_1_HOST_RESP_01);

	return res;
}

static int emmc_cmd16_set_blocklen(uint32_t block_length)
{
	struct mmc_cmd cmd;
	uint32_t reg;

	/*reg = HOST_REG(chip, BLKCNT_BLKSIZE);*/
	reg = BDEV_RD(BCHP_SDIO_1_HOST_BLOCK);
	BDEV_WR(BCHP_SDIO_1_HOST_BLOCK, (reg & ~SDHC_REG_BLKSIZE_MASK)
		+ block_length);

	/* CMD16 -  R1 (Index check  On, CRC check  On, 48bit) */
	cmd.index = IDX_CMD16;
	cmd.arg = block_length;
	return issue_cmd(&cmd, NULL);
}

static int emmc_cmd18_read_multiple_block(
		uint32_t dma_addr, uint32_t block_count, uint32_t block_addr,
		bool dma_used)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.index = IDX_CMD18;
	cmd.arg = block_addr;
	data.dma_address = dma_addr;
	data.block_count = block_count;
	data.dma_used = dma_used;
	return issue_cmd(&cmd, &data);
}

static int emmc_set_clock(uint32_t set_clock)
{
	uint32_t control1;
	int timeout;

	/* Set clock values : freq, timeout, enable int_clk
	 * SDHC_CLKCTL_FREQ, SDHC_CLKCTL_FREQ_UPPER,
	 * SDHC_TOCTL_TIMEOUT, SDHC_CLKCTL_INTERN_CLK_ENA
	 */
	control1 = BDEV_RD(BCHP_SDIO_1_HOST_CTRL_SET1);
	DBG_MSG_PUTS(" HOST_CTRL_SET1 0x");
	DBG_MSG_WR_HEX(control1);
	DBG_MSG_PUTS("\n");
	control1 = (control1 & ~0xfffff) | set_clock;
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET1, control1);

#define INT_CLK_STABLE_TIMEOUT	50000	/* 50[ms] */
	/* Wait 50ms for internal clock stable */
	timeout = INT_CLK_STABLE_TIMEOUT / POLL_DELAY_USECS;
	while (1) {
		if (timeout-- == 0)
			break;
		if (BDEV_RD(BCHP_SDIO_1_HOST_CTRL_SET1)
			& SDHC_CLKCTL_INTERN_CLK_STABLE)
			/*BCHP_SDIO_1_HOST_CTRL_SET1_INTERNAL_CLK_STABLE_MASK*/
			break;
		udelay(POLL_DELAY_USECS);
	}
	if (timeout <= 0) {
		__puts(" timeout : internal clock stable");
		return 1;
	}

	/* Enable SD_Clk */
	control1 |= SDHC_CLKCTL_SD_CLK_ENA;
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET1, control1);
	DBG_MSG_PUTS(" Enable HOST_CTRL_SET1 0x");
	DBG_MSG_WR_HEX(control1);
	DBG_MSG_PUTS("\n");

	return 0;
}


static int setup_bus_freq_width(void)
{
	uint32_t reg_ctrl_set0;
	int res = 0;

	reg_ctrl_set0 = BDEV_RD(BCHP_SDIO_1_HOST_CTRL_SET0);

	/* Bus Frequency Setting : eMMC, Host controller */
#if (EMMC_HS_TIMING_FSBL == HS_TIMING_FULL)
#define EMMC_DATA_TRAN_CLK_SET 0xe0201
	/* timeout=0xe0000, freq=0x0200(25MHz),
		enable_int_clk=0x01 frequency to work all eMMC as possible */
	/* Full Speed Mode - 25MHz */
	DBG_MSG_PUTS("[setup_bus_freq_width] HS_TIMING_FULL\n");
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_HS_TIMING, HS_TIMING_FULL);
#else
#define EMMC_DATA_TRAN_CLK_SET 0xe0101
	/* timeout=0xe0000, freq=0x0100(50MHz),
		enable_int_clk=0x01 frequency to work all eMMC as possible */
	/* High Speed Mode - 50MHz */
	DBG_MSG_PUTS("[setup_bus_freq_width] HS_TIMING_HS\n");
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_HS_TIMING, HS_TIMING_HS);
	reg_ctrl_set0 |= SDHC_CTL1_HIGH_SPEED;
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET0, reg_ctrl_set0);
#endif
	if (res)
		return res;
	DBG_MSG_PUTS("[setup_bus_freq_width] emmc_set_clock\n");
	if (emmc_set_clock(EMMC_DATA_TRAN_CLK_SET))
		return EMMC_NG;

	/*
	 * Bus Width Setting : eMMC, Host controller, BUS_WIDTH_8BIT
	 */
	DBG_MSG_PUTS("[setup_bus_freq_width] BUS_WIDTH_8BIT\n");
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_BUS_WIDTH, BUS_WIDTH_8BIT);
	if (res)
		return res;
	reg_ctrl_set0 &= ~SDHC_CTL1_4BIT;
	reg_ctrl_set0 |= SDHC_CTL1_8BIT;
	/*  Set registers */
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET0, reg_ctrl_set0);

	/* BOOT_BUS_WIDTH : BRCM EMMC BOOT support only 8-bit */
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_BOOT_BUS_CONDITIONS, 0x2);
	if (res)
		return res;

	return EMMC_OK;
}

#define SDIO_SW_RESET_MASK 0x07000000
	/* (SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD | SDHC_SW_RESET_ALL) */

/* Init the eMMC device
 * Return:
 *	0 - success
 *	1 - retry
 *	-1 - fail
 */
static int init_device(void)
{
	uint32_t ocr_data;
	int res;
	int timeout_cnt;

	DBG_MSG_PUTS("\n[init_device] emmc_software_reset\n");
	if (emmc_software_reset(SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD
	    | SDHC_SW_RESET_ALL))
		return EMMC_NG;

	/* (SDHC_BUS_PWR + (EMMC_BUS_VOLTAGE << SDHC_BUS_VOLT_SHIFT) */
#if (EMMC_BUS_VOLTAGE == BUS_VOLTAGE_33)
 #define EMMC_BUS_PWR_VOLT	0x00000F00
#else /* (EMMC_BUS_VOLTAGE == BUS_VOLTAGE_18) */
 #define EMMC_BUS_PWR_VOLT	0x00000B00
#endif
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET0, EMMC_BUS_PWR_VOLT);

#define EMMC_BOOT_MODE_CLK_SET 0xef001
	/* timeout=0xe0000, freq=0xf000(208KHz),
		enable_int_clk=0x01 frequency to work all eMMC as possible */
	DBG_MSG_PUTS("[init_device] emmc_set_clock\n");
	if (emmc_set_clock(EMMC_BOOT_MODE_CLK_SET))
		return EMMC_NG;

	DBG_MSG_PUTS("[init_device] Enable interrupts\n");
	/* Enable interrupts */
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS_ENA, HOST_INT_ENABLES_FSBL);

	/* CMD0 : Device Reset */
	DBG_MSG_PUTS("[init_device] emmc_cmd0_go_idle_state\n");
	if (emmc_cmd0_go_idle_state())
		return EMMC_NG;

	/*
	 * Voltage validation and wait for device ready.
	 * Wait for up to 1 second  for ready bit
	 */
	DBG_MSG_PUTS("[init_device] Voltage validation\n");
	timeout_cnt = 1000000 / 1000;
	while (1) {
		res = emmc_cmd1_send_op_cond(&ocr_data);
		if (res)
			return EMMC_NG;

		/* Make sure the card supports 3.3V */
		if ((ocr_data & OCR_VDD_33_34) == 0) {
			__puts("eMMC device does not support 3.3V");
			return -1;
		}
		if (ocr_data & 0x80000000)	/* ready bit */
			break;
		if (timeout_cnt-- == 0)
			break;
		udelay(1000);
	}
	if (timeout_cnt < 0)
		return EMMC_NG;

	/* eMMC must be over 4GB */

	/* Set CID : CMD2 */
	DBG_MSG_PUTS("[init_device] emmc_cmd2_all_send_cid_single_device\n");
	res = emmc_cmd2_all_send_cid_single_device();
	if (res)
		return EMMC_NG;

	/* Set RCA : CMD3*/
	DBG_MSG_PUTS("[init_device] emmc_cmd3_set_rca\n");
	res = emmc_cmd3_set_rca();
	if (res)
		return EMMC_NG;

	return EMMC_OK;
}

/*  Data Transfer Mode */
static int init_transfer_mode(void)
{
	int res = 0;

	DBG_MSG_PUTS("\n<<< Start : init_transfer_mode( ) >>>\n");

	/* Change state from stand-by to transfer */
	DBG_MSG_PUTS("[init_transfer_mode] emmc_cmd7_select_card_stby_tans\n");
	res = emmc_cmd7_select_card_stby_tans();
	if (res)
		return res;

	/* Bus Mode Configuration */
	DBG_MSG_PUTS("[init_transfer_mode] setup_bus_freq_width\n");
	res = setup_bus_freq_width();
	if (res) {
		__puts(" bus mode configuration failed!");
		return res;
	}

	/* Set-up High Capacity */
	DBG_MSG_PUTS("[init_transfer_mode] ERASE_GROUP_DEF High Capacity\n");
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_ERASE_GROUP_DEF, 1);
	/* SET_BLOCKLEN to set the block length as 512 bytes. */
	DBG_MSG_PUTS("[init_transfer_mode] emmc_cmd16_set_blocklen\n");
	res = emmc_cmd16_set_blocklen(EMMC_BLOCK_SIZE);
	if (res)
		return res;

	/* Clear all interrupt status bits */
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS, HOST_INT_STATUS_ALL);
	return EMMC_OK;
}

/*
 * eMMC Initialization
 */
static int emmc_initialize(void)
{
	int res;

	DBG_MSG_PUTS("\n<<< Start : emmc_initialize( ) >>>\n");

	/* [Step 1] Host(STB Chip) Configuration */
	/* Disable Boot Logic in chip.
	 * FSBL eMMC driver run only boot from eMMC */
	DBG_MSG_PUTS("\n[Step 1] Host(STB Chip) Configuration\n");
	BDEV_WR(BCHP_SDIO_1_BOOT_MAIN_CTL, 0);
	while (1) {
		if ((BDEV_RD(BCHP_SDIO_1_BOOT_STATUS) & 1) == 0)
			break;
		udelay(10);
	}
	/* Clear any boot mode bits */
	BDEV_WR(BCHP_SDIO_1_HOST_CTRL_SET0, 0);

	/*
	 * The early init commands are run in open drain mode where the
	 * bus pull-ups are important. Broadcom chips use a value higher
	 * than recommended by the spec, but the spec says the clock can be
	 * slowed down to compensate. Most eMMC modules handle the default
	 * speed of 400KHz, but a least some SanDisk modules only run at
	 * 200KHz-300KHz. We use 200KHz. Once we're out of open-drain mode,
	 * the pull-ups don't matter.
	 */
	DBG_MSG_PUTS("\n[Step 2] Initialize eMMC : CMD0 - CMD3\n");
	/* Initialize the eMMC device */
	res = init_device();
	if (res) {
		__puts("eMMC error!\n");
		return EMMC_NG;
	}

	/* Get device into transfer mode */
	DBG_MSG_PUTS("\n[Step 3] Initialize transfer_mode\n");
	if (init_transfer_mode()) {
		__puts(" failed to transfer mode!");
		return EMMC_NG;
	}
	DBG_MSG_PUTS("End : emmc_initialize()\n");
	return EMMC_OK;
}

static int emmc_read(
		uint32_t dma_addr, uint32_t bfw_size,
		uint32_t emmc_addr, bool dma_used)
{
#if DEBUG_EMMC_DATA
	int i = 0, j = 0;
	uint32_t *data = 0;
#endif
	int res = 0;

	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
				      Idx_ExtCSD_PARTITION_CONFIG,
				      PCFG_BOOT_ACK
				      | PCFG_BOOT_PARTITION_ENABLE_BOOT1
				      | PCFG_PARTITION_ACCESS_DATA);
	udelay(50000);
	if (res) {
		__puts("\n[emmc_read] Error to select DATA partition!\n");
		return res;
	}

	res = wait_ready_data_xfer();
	if (res) {
		__puts("\n[emmc_read] eMMC is not ready!\n");
		return res;
	}

	res = emmc_cmd18_read_multiple_block(
				dma_addr,
				(uint32_t)(bfw_size>>EMMC_BLOCK_SHIFT),
				(uint32_t)(emmc_addr>>EMMC_BLOCK_SHIFT),
				dma_used);
	if (res) {
		DBG_MSG_PUTS("\n[emmc_read] failed!\n");
		return res;
	}
#if DEBUG_EMMC_DATA
	__puts("\n Memory data\n");
	res = dma_addr;
	writehex(res);
	__puts("\n");
	data = (uint32_t *)(dma_addr);
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			writehex(*data++);	/* 32-bit is reversed 4-byte. */
			__puts(" ");
		}
		__puts("\n");
	}
#endif
	DBG_MSG_PUTS("\n[emmc_read] success!\n");
	/* Clear all interrupt status bits */
	BDEV_WR(BCHP_SDIO_1_HOST_INT_STATUS, HOST_INT_STATUS_ALL);
	return EMMC_OK;
}

static int emmc_ready_boot(void)
{
	int res = 0;

	/* [Step 1] Switch BOOT partition. */
	DBG_MSG_PUTS("\n[Step 1] Switch BOOT partition\n");
	res = emmc_cmd6_switch_extcsd(Idx_ExtCSD_ACC_WRB,
		Idx_ExtCSD_PARTITION_CONFIG,
		PCFG_BOOT_ACK | PCFG_BOOT_PARTITION_ENABLE_BOOT1
		| PCFG_PARTITION_ACCESS_BOOT1);
	udelay(30 * 1000);
	if (res) {
		DBG_MSG_PUTS("\n[emmc_ready_boot] Error");
		DBG_MSG_PUTS("to select BOOT partition!\n");
		return res;
	}

	/* [Step 2] Reset SDIO_HOST. */
	DBG_MSG_PUTS("[Step 2] Reset SDIO_HOST\n");
	if (emmc_software_reset(SDHC_SW_RESET_DAT | SDHC_SW_RESET_CMD
	    | SDHC_SW_RESET_ALL))
		return EMMC_NG;
	/* [Step 3] Enable Boot Logic in SDIO_BOOT. */
	DBG_MSG_PUTS("[Step 3] Enable Boot Logic in SDIO_BOOT.\n");
	BDEV_WR(BCHP_SDIO_1_BOOT_MAIN_CTL, 1);
	while (1) {
		if ((BDEV_RD(BCHP_SDIO_1_BOOT_STATUS) & 1) == 1)
			break;
		udelay(10);
	}
	return EMMC_OK;
}


int emmc_read_fsbl(uint32_t dma_addr,
		  uint32_t bfw_size,
		  uint32_t emmc_addr,
		  bool     dma_used)
{
	if (emmc_initialize() == EMMC_OK) {
		/* dma_addr(DDR address) should be aligned 256K
			and size should be less than 256KB */
		/* bfw_size  : should be padded as 512B. */
		/* emmc_addr : Byte address */
		if (emmc_read(dma_addr, bfw_size, emmc_addr, dma_used) ==
			EMMC_OK)
			__puts(">>> 'emmc_read' success!\n");
		else
			__puts(">>> 'emmc_read' failed!\n");
	} else {
		__puts(">>> 'emmc_initialize' failed!\n");
	}

	if (emmc_ready_boot() == EMMC_OK) {
		DBG_MSG_PUTS(">>> emmc_ready_boot!\n");
		return EMMC_OK;
	} else {
		__puts(">>> emmc_ready_boot failed!\n");
		return EMMC_NG;
	}
}
