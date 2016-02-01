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

#ifndef __DEV_EMMC_RPMB_H__
#define __DEV_EMMC_RPBM_H__

int emmc_rpmb_use_key(struct emmc_chip *chip, void *key);
int emmc_rpmb_program_key(struct emmc_chip *chip);
int emmc_rpmb_get_write_counter(struct emmc_chip *chip, uint32_t *counter);
int emmc_rpmb_block_read(struct emmc_chip *chip, uint32_t blk_start,
			uint32_t blk_cnt, uint8_t *buffer);
int emmc_rpmb_block_write(struct emmc_chip *chip, uint32_t blk_start,
			uint32_t blk_cnt, uint8_t *buffer);

#endif /*__DEV_EMMC_RPMB_H__ */
