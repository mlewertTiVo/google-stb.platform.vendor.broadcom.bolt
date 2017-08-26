/*****************************************************************************
*
* Copyright 2014-2015 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*
*****************************************************************************/

#ifndef _FASTBOOT_H
#define _FASTBOOT_H

#include "android_types.h"

#define FASTBOOT_VERSION                "0.4"
#define FASTBOOT_HANDSHAKE              "FB01"
#define FASTBOOT_HANDSHAKE_SIZE         4

/* The code to access flash device assumes 512 bytes per LBA */
#define BYTES_PER_LBA           (512)

/* Fastboot Error Code Definition
 * Note that fastboot code uses the BOLT error code
 * (BOLT-ROOT-DIR/include/error.h) whenever possible. The exception is when we
 * need to distinguish failure cases that is specific for Fastboot operation
 * in order to determine the appropriate code path for handling the error.
 * Error code starts from -100 to avoid any overlap with the generic BOLT
 * error code.
 * */
#define FB_ERR_NO_VALID_GPT			-101
#define FB_ERR_FAIL_TO_ACCESS_FLASH_DEV		-102
#define FB_ERR_PARTITION_NOT_EXIST		-103

/* Android-style flash naming */
/* flash partitions are defined in terms of blocks */
struct fastboot_ptentry
{
	/* Logical name for this partition, null terminated */
	char name[40];
	/* Start of partition in unit of LBA. Size of LBA is BYTES_PER_LBA */
	unsigned int start;
	/* Length of the partition, must be multiple of LBA */
	unsigned long long length;
	/* Controls the details of how operations are done on the partition */
	unsigned long long flags;
	/* Partition unique UUID (generated) */
	char uuid[64];
};

/* Constant to define transport mode selected by user */
#define FB_TRANSPORT_TCP 0
#define FB_TRANSPORT_USB 1

/* Fastboot command processor states defintions */
#define FB_STATE_CMD_PROC 0
#define FB_STATE_DATA_DL 1

/* if hand-shaking with peer, we need to account for a 8-bytes overhead on
 * command and responses which carries the size of the paylaod.
 */
#define FB_HS_OVERHEAD 8

#define MAX_PTN 64

/* Fastboot context information */
struct fastboot_info
{
	/* Transport mode selected by user. Note the underlying transport
	 * device name is abstracted from user. */
	int transport_mode;

	/* Handle to BOLT transport device -- either TCP or USB-OTG */
	int transport_dev_fd;

	/* Flash device name -- either USB-disk-drive or eMMC flash */
	char flash_devname[15];

	/* Flash device name that stores the BOLT image */
	char boltimg_devname[15];

	/* Pointer to the receive buffer for incoming command from host */
	unsigned char *cmd_buf;
	unsigned int cmd_buf_size;

	/* State of Fastboot command processor */
	int cmd_state;

	/* Size of data read from transport layer */
	unsigned int transport_read_bytes;

	/* Min and Max transfer size based on transport layer. Note that for
	 * TCP transport device, min and max are the same. It is the 
	 * USB transport device that will be different to improve the image
	 * download throughput */
	unsigned int min_transfer_size;
	unsigned int max_transfer_size;

	/* Counter to keep track of the valid state change request to ensure a
	 * valid lock state change is submitted twice before erasing data */
	int dev_lock_state_change_req_cnt;

	/* handshake - marked when we successfully shake hands with host, used for
	 *             tcp support.
	 * header - after handshake took place, command/response stream hace a mandatory
	 *          header.
	 */
   int handshake;
   int header;
};

/* Constants releted to Bootloader Image Header */
#define BL_IMG_HEADER_MAGIC 0x214c4225
#define BL_IMG_HEADER_VERSION 0x1

/* Bootloader Image Header to allow both BOLT and BSU images to be combined
 * into a single image that can be fastboot flash at the same time. */
struct bootloader_img_hdr
{
	/* magic value defined by BL_IMG_HEADER_MAGIC */
	__le32 magic;

	/* version of the header in case it needs to be adjusted in future */
	__le32 version;

	/* BOLT image offset location within bootloader image */
	__le32 bolt_img_offset;

	/* BOLT image size */
	__le32 bolt_img_size;

	/* BSU image offset location within bootloader image */
	__le32 bsu_img_offset;

	/* BSU image size*/
	__le32 bsu_img_size;

	/* BL31 image offset location within bootloader image */
	__le32 bl31_img_offset;

	/* BL31 image size*/
	__le32 bl31_img_size;
};

/* The Android-style flash handling */

/* tools to populate and query the partition table */
extern int fastboot_discover_gpt_tables(char *flash_devname);
extern int fastboot_validate_gpt_image(unsigned char *gpt_buf_ptr);
extern void fastboot_flash_clean_all_ptn(void);
extern void fastboot_flash_add_ptn(struct fastboot_ptentry *ptn);
extern struct fastboot_ptentry *fastboot_flash_find_ptn(const char *name);
extern struct fastboot_ptentry *fastboot_flash_get_ptn(unsigned n);
extern unsigned int fastboot_flash_get_ptn_count(void);
extern void fastboot_flash_dump_ptn(void);
extern int fastboot_populate_canned_gpt(const uint8_t *data, const uint32_t size,
	struct fastboot_ptentry *entry, unsigned int *total);

#endif /* _FASTBOOT_H */
