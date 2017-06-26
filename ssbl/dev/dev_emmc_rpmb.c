w/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "error.h"
#include "common.h"
#include "board_init.h"
#include "parttbl.h"
#include "ui_init.h"
#include "hmac_sha2.h"
#include "dev_emmcflash.h"

#define SET_BLOCK_COUNT_RELIABLE_WRITE 0x80000000

struct rpmb_data_frame {
	uint8_t stuff[196];
	uint8_t mac[32];
	uint8_t data[256];
	uint8_t nonce[16];
	uint32_t write_counter;
	uint16_t address;
	uint16_t block_count;
	uint16_t result;
#define RPMB_RESULT_OP_MASK		0x0007
#define RPMB_RESULT_COUNTER_MASK	0x0008
#define RPMB_RESULT_OP_OK		0x0000
#define RPMB_RESULT_OP_ERR_GENERAL	0x0001
#define RPMB_RESULT_OP_ERR_AUTH		0x0002
#define RPMB_RESULT_OP_ERR_COUNTER	0x0003
#define RPMB_RESULT_OP_ERR_ADDRESS	0x0004
#define RPMB_RESULT_OP_ERR_WRITE	0x0005
#define RPMB_RESULT_OP_ERR_READ		0x0006
#define RPMB_RESULT_OP_ERR_NO_KEY	0x0007
	uint16_t req_resp;
#define RPMB_REQ_PROGRAM_KEY	0x0001
#define RPMB_REQ_READ_COUNTER	0x0002
#define RPMB_REQ_WRITE_BLOCK	0x0003
#define RPMB_REQ_READ_BLOCK	0x0004
#define RPMB_REQ_READ_RESULT	0x0005
#define RPMB_RESP_SET_KEY	0x0100
#define RPMB_RESP_READ_COUNTER	0x0200
#define RPMB_RESP_WRITE_BLOCK	0x0300
#define RPMB_RESP_READ_BLOCK	0x0400
};


static int rpmb_verify_key_set(struct emmc_chip *chip)
{
	if (!chip->rpmb_key.valid) {
		err_msg("%s The KEY needs to be specified",
			chip->regs.name);
		return BOLT_ERR_IOERR;
	}
	return 0;
}


static int rpmb_command(struct emmc_chip *chip,
			struct rpmb_data_frame *req_frame,
			struct rpmb_data_frame *resp_frame, int writeop)
{
	int res;
	char sbc_err[] = "RPMB error doing SET_BLOCK_COUNT";

	res = emmc_cmd23_set_block_count(chip,
					writeop ?
					1 | SET_BLOCK_COUNT_RELIABLE_WRITE :
					1);
	if (res) {
		err_msg("%s %s", chip->regs.name, sbc_err);
		return res;
	}
	res = emmc_cmd25_write_multiple_block(chip, 0, (uint32_t)req_frame,
					1, EMMC_BLOCKSIZE);
	if (res) {
		err_msg("%s RPMB error sending request",
			chip->regs.name);
		return res;
	}

	/*
	 * WRITE_BLOCK and WRITE_KEY commands need to send a
	 * response request after sending the RPMB command.
	 */
	if (writeop) {
		res = emmc_cmd23_set_block_count(chip, 1);
		if (res) {
			err_msg("%s %s", chip->regs.name, sbc_err);
			return res;
		}

		memset(resp_frame, 0, sizeof(*resp_frame));
		resp_frame->req_resp = cpu_to_be16(RPMB_REQ_READ_RESULT);
		res = emmc_cmd25_write_multiple_block(chip, 0,
						(uint32_t)resp_frame,
						1, EMMC_BLOCKSIZE);
		if (res) {
			err_msg("%s RPMB error sending response request",
				chip->regs.name);
			return res;
		}
	}
	res = emmc_cmd23_set_block_count(chip, 1);
	if (res) {
		err_msg("%s %s", chip->regs.name, sbc_err);
		return res;
	}
	res = emmc_cmd18_read_multiple_block(chip, 0, (uint32_t)resp_frame,
					1, EMMC_BLOCKSIZE);
	if (res) {
		err_msg("%s RPMB error getting response",
			chip->regs.name);
		return res;
	}
	if (resp_frame->result != 0) {
		err_msg("%s RPMB response error. result: 0x%x",
			chip->regs.name, be16_to_cpu(resp_frame->result));
		return BOLT_ERR_IOERR;
	}
	return 0;
}


/* Specify the key to be used for all subsquence RPMB operations */
int emmc_rpmb_use_key(struct emmc_chip *chip, void *key)
{
	memcpy(chip->rpmb_key.key, key, sizeof(chip->rpmb_key.key));
	chip->rpmb_key.valid = 1;
	return 0;
}


/* Program the key into the RPMB device. One Time Programable!!! */
int emmc_rpmb_program_key(struct emmc_chip *chip)
{
	struct rpmb_data_frame req_frame;
	struct rpmb_data_frame resp_frame;
	int res;

	res = rpmb_verify_key_set(chip);
	if (res)
		return res;
	memset(&req_frame, 0, sizeof(req_frame));
	req_frame.req_resp = cpu_to_be16(RPMB_REQ_PROGRAM_KEY);
	memcpy(req_frame.mac, chip->rpmb_key.key, sizeof(chip->rpmb_key.key));
	res = rpmb_command(chip, &req_frame, &resp_frame, 1);
	if (res)
		return res;


	return 0;
}


/* Get the RPMB Write Counter */
int emmc_rpmb_get_write_counter(struct emmc_chip *chip, uint32_t *counter)
{
	struct rpmb_data_frame req_frame;
	struct rpmb_data_frame resp_frame;
	int res;

	memset(&req_frame, 0, sizeof(req_frame));
	req_frame.req_resp = cpu_to_be16(RPMB_REQ_READ_COUNTER);
	res = rpmb_command(chip, &req_frame, &resp_frame, 0);
	if (res)
		return res;
	*counter = be32_to_cpu(resp_frame.write_counter);
	return 0;
}


/* Read RPMB data blocks */
int emmc_rpmb_block_read(struct emmc_chip *chip, uint32_t blk_start,
			uint32_t blk_cnt, uint8_t *buffer)
{
	struct rpmb_data_frame req_frame;
	struct rpmb_data_frame resp_frame;
	int res;
	uint8_t mac[RPMB_KEY_LENGTH];

	memset(&req_frame, 0, sizeof(req_frame));

	req_frame.req_resp = cpu_to_be16(RPMB_REQ_READ_BLOCK);

	while (blk_cnt) {
		req_frame.address = cpu_to_be16((uint16_t)blk_start);
		res = rpmb_command(chip, &req_frame, &resp_frame, 0);
		if (res)
			return res;
		memcpy(buffer, resp_frame.data, sizeof(resp_frame.data));
		blk_cnt--;
		blk_start++;
		buffer += EMMC_BLOCKSIZE_RPMB;

		/* If the key has not been set, don't check it */
		if (!chip->rpmb_key.valid)
			continue;
		hmac_sha256(chip->rpmb_key.key, sizeof(chip->rpmb_key.key),
			resp_frame.data, sizeof(resp_frame) -
			offsetof(struct rpmb_data_frame, data),
			mac, sizeof(mac));
		/* Make sure the keys match */
		if (memcmp(mac, resp_frame.mac, sizeof(mac))) {
			err_msg("RPMB KEY missmatch");
			return BOLT_ERR_IOERR;
		}
	}
	return 0;
}


/* Write RPMB data blocks */
int emmc_rpmb_block_write(struct emmc_chip *chip, uint32_t blk_start,
			uint32_t blk_cnt, uint8_t *buffer)
{
	struct rpmb_data_frame req_frame;
	struct rpmb_data_frame resp_frame;
	int res;
	uint32_t write_counter;

	res = rpmb_verify_key_set(chip);
	if (res)
		return res;
	memset(&req_frame, 0, sizeof(req_frame));
	req_frame.req_resp = cpu_to_be16(RPMB_REQ_WRITE_BLOCK);
	req_frame.block_count = cpu_to_be16(1);
	while (blk_cnt) {
		res = emmc_rpmb_get_write_counter(chip, &write_counter);
		if (res)
			return res;
		req_frame.write_counter = cpu_to_be32(write_counter);
		memcpy(req_frame.data, buffer, sizeof(req_frame.data));
		req_frame.address = cpu_to_be16((uint16_t)blk_start);

		/* Add the MAC to the request */
		hmac_sha256(chip->rpmb_key.key, sizeof(chip->rpmb_key.key),
			req_frame.data,	sizeof(req_frame) -
			offsetof(struct rpmb_data_frame, data),
			req_frame.mac, sizeof(req_frame.mac));
		res = rpmb_command(chip, &req_frame, &resp_frame, 1);
		if (res) {
			err_msg("%s Failed on block %d",
				chip->regs.name, blk_start);
			return res;
		}
		blk_cnt--;
		blk_start++;
		buffer += EMMC_BLOCKSIZE_RPMB;
	}
	return 0;
}


static int ui_cmd_use_key(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *dev_name;
	int fd;
	int res;
	char key[RPMB_KEY_LENGTH];
	int x;
	char *ptr;
	int val;
	size_t retlen;

	dev_name = cmd_getarg(cmd, 0);
	if (!dev_name)
		return ui_showerror(BOLT_ERR_INV_PARAM,
				"No RPMB device provided");
	fd = bolt_open(dev_name);
	if (fd < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, dev_name);
	for (x = 0; x < RPMB_KEY_LENGTH; x++) {
		ptr = cmd_getarg(cmd, x + 1);
		if (ptr == NULL) {
			ui_showerror(BOLT_ERR_INV_PARAM,
				"Key length error");
			goto out;
		}
		val = atoi(ptr);
		if (val > 0xff) {
			ui_showerror(BOLT_ERR_INV_PARAM,
				"Invalid Key value");
			goto out;
		}
		key[x] = (uint8_t)val;
	}
	res = bolt_ioctl(fd, IOCTL_FLASH_RPMB_USE_KEY, &key,
			sizeof(key), &retlen, 0);
	if (res)
		err_msg("Error doing ioctl: 0x%x", res);
out:
	bolt_close(fd);
	return 0;
}


static int ui_cmd_program_key(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res = 0;
	int fd;
	char *dev_name;
	size_t retlen;
	char line[32];

	dev_name = cmd_getarg(cmd, 0);
	if (!dev_name)
		return ui_showerror(BOLT_ERR_INV_PARAM,
				"No RPMB device provided");
	fd = bolt_open(dev_name);
	if (fd < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, dev_name);
	xprintf("This is a ONE TIME ONLY operation!!!\n");
	console_readline("Enter \"y\" to continue, any other key to abort: ",
			line, sizeof(line));
	if (line[0] == 'y') {
		res = bolt_ioctl(fd, IOCTL_FLASH_RPMB_PROGRAM_KEY, NULL,
				0, &retlen, 0);
		if (res)
			err_msg("Error doing ioctl: 0x%x", res);
		else
			xprintf("Successfully Programed authentication key\n");
	}
	bolt_close(fd);
	return res;
}


static int ui_cmd_read_counter(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res;
	int fd;
	char *dev_name;
	size_t retlen;
	uint32_t write_counter;

	dev_name = cmd_getarg(cmd, 0);
	if (!dev_name)
		return ui_showerror(BOLT_ERR_INV_PARAM,
				"No RPMB device provided");
	fd = bolt_open(dev_name);
	if (fd < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, dev_name);
	res = bolt_ioctl(fd, IOCTL_FLASH_RPMB_GET_WRITE_COUNTER,
			&write_counter, sizeof(write_counter), &retlen, 0);
	if (res)
		err_msg("Error doing ioctl: 0x%x", res);
	xprintf("RPMB Write Counter 0x%x\n", write_counter);
	bolt_close(fd);
	return res;
}


int ui_init_rpmb_cmds(void)
{
	cmd_addcmd("rpmb use-key", ui_cmd_use_key, NULL,
		"Specify the key to be used for all RPMB operations.",
		"rpmb use-key [RPMBdevice] [byte1] [byte2]..[byte32]\n\n"
		"Specify the RPMB authentication key to be used for\n"
		"subsquent RPMB operations. The key must be 32 bytes long.\n",
		"");
	cmd_addcmd("rpmb program-key", ui_cmd_program_key, NULL,
		"Program the authentication key.",
		"rpmb program-key [RPMBdevice]\n\n"
		"Program the authentication key using the key specified by\n"
		"the \"rpmb use_key\" command.\n"
		"NOTE: This is a one time programable operation!!!\n",
		"");
	cmd_addcmd("rpmb counter", ui_cmd_read_counter, NULL,
		"Read the RPMB Write Counter.",
		"rpmb counter [RPMBdevice]\n\n"
		"Print the RPMB Write Counter.\n",
		"");
	return 0;
}
