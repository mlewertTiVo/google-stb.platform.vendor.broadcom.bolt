/*****************************************************************************
*
* Copyright 2014 - 2015 Broadcom Corporation.  All rights reserved.
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

#include <error.h>
#include <env_subr.h>
#include "android_types.h"
#include "android_bsu.h"
#include "fastboot.h"
#include "sparse_format.h"
#include "gpt.h"
#include "ioctl.h"
#include "eio_boot.h"

/* Compiler option for debugging fastboot flash code */
#undef FB_DBG_PRINT_SPARSE_IMG_HDR  /* print sparse img header */
#undef FB_DBG_PRINT_CHUNK_HDR /* print chunk hdr while flashing sparse img */
#undef FB_DBG_PRINT_IMG_HDR /* print image hdr & image can be raw or sparse */
#undef FB_DBG_PRINT_TCP_CHUNKS

/*
 * Constant definitions
 */

/* TODO:add runtime check to make sure the board has enough memory for staging buffer*/
/* Use upper 512MB of DDR0 for Android fastboot flash. It is assumed that
 * Android runs on platform with at least 1GB of DDR memory.
 * It is also assumed that BOLT code won't be using upper 512MB.
 * The staging buffer size is set to be 256MB. It could be increased
 * to 512MB but we don't so we can have upper most 256MB for debugging
 * purpose. Any image bigger than the staging buffer size will
 * be re-sparsed by host fastboot application.
 */
#define FB_FLASH_STAGING_BUFFER		(0x20000000)
#define FB_FLASH_STAGING_BUFFER_SIZE	(1024*1024*256)

/* The 64 defined bytes, plus \0 */
#define RESPONSE_LEN	(64 + 1)
/* header when using handshake. */
#define RESPONSE_HDR	(8)

/* The min USB BULK OUT pipe transfer size is based on the max-packet-size
 * field as defined by USB standard and the size changes based on the USB
 * speed. Fastboot protocol expects the max-packet-size to be 64 bytes for
 * Full-Speed USB and 512 bytes for High-Speed USB. We use min(64,512) to
 * define the following constant */
#define USB_BULKOUT_MAXPKT_SIZE	(64)

/* The max USB BULK OUT pipe transfer size is defined by USB BDC driver and
 * current implementation is 64Kbytes. We can define something less in the
 * fastboot layer, but for now use the max unless there is a memory constraint */
#define USB_BULKOUT_BUFFER_SIZE	(64*1024)

/* Use 32*1024 bytes for TCP receive buffer as we want it to be a size larger than
 * MTU size (1500 bytes) and also a power of 2 for efficient cache data fetch*/
#define TCP_RECV_BUFFER_SIZE	(32*1024)

/* Device state to determine if it unlocked (flashable) or locked (unflashable) */
#define UNLOCKED	(0)
#define LOCKED		(1)

/* Use GPT Partition Entry Attribute Flag user-defined bits (48-63) to
 * identify the partition filesystem type. */
#define GPT_ATTR_FLAG_PTN_TYPE_EXT4_MASK	(1ULL<<48)
#define GPT_ATTR_FLAG_PTN_TYPE_F2FS_MASK	(1ULL<<49)

/*
 * Forward Declaration
 */
static int fastboot_tx_write_str(const char *buffer);
static unsigned int dl_image_rx_bytes_expected(void);
static void fastboot_discover_bolt_ptn(const char *devname, char *bolt_devname);
static int oem_get_device_lock_state(void);
extern void clear_dmv_corrupt(const char *partition);

/*
 * Variables
 */
/* Partition table to support the Android-style naming of flash */
static struct fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount;

/* Fastboot context information */
static struct fastboot_info fb_info;

/* Control image download progress */
static unsigned int download_size;
static unsigned int download_bytes;

static unsigned int download_payload;
static unsigned int download_overhead;
static unsigned int chunk_received;
static unsigned int chunk_size;
static unsigned int chunk_header;

/* Compare strings based on length of first input (s1) */
static int strcmp_l1(const char *s1, const char *s2)
{
	if (!s1 || !s2)
		return -1;
	return os_strncmp(s1, s2, os_strlen(s1));
}

/*
 * Android style flash utilties
 */
void fastboot_flash_clean_all_ptn(void)
{
	os_memset(ptable, 0, sizeof(ptable));
	pcount = 0;
}

void fastboot_flash_add_ptn(struct fastboot_ptentry *ptn)
{
	if(pcount < MAX_PTN){
		os_memcpy(ptable + pcount, ptn, sizeof(*ptn));
		pcount++;
	}
}

void fastboot_flash_dump_ptn(void)
{
	unsigned int n;
	struct fastboot_ptentry *ptn;

	for(n = 0; n < pcount; n++) {
		ptn = ptable + n;
		os_printf("ptn %d name='%s' start=%d len=%llu uuid='%s'\n",
			n, ptn->name, ptn->start, ptn->length, ptn->uuid);
	}
}

struct fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
	unsigned int n;

	for(n = 0; n < pcount; n++) {
		/* Make sure a substring is not accepted */
		if (os_strlen(name) == os_strlen(ptable[n].name))
		{
			if(0 == os_strcmp(ptable[n].name, name))
				return ptable + n;
		}
	}
	return 0;
}

struct fastboot_ptentry *fastboot_flash_get_ptn(unsigned int n)
{
	if(n < pcount) {
		return ptable + n;
	} else {
		return 0;
	}
}

unsigned int fastboot_flash_get_ptn_count(void)
{
	return pcount;
}

/* TODO: This limit comes from USB device driver DMA pool size
 * We might need to re-tune it for eMMC */
#define DATA_MAX_SIZE (BYTES_PER_LBA * 192)
#define FILL_MAX_SIZE 4

/* Write sparse image to flash device */
static int fastboot_flash_write_sparse_image(const char *devname,
					uint8_t *transfer_buffer,
					struct fastboot_ptentry *ptn)
{
	int res = BOLT_OK;
	int chunk_hdr_offset;
	unsigned int byte_cnt, amtcopy, limit, offset;
	unsigned long long curr_write_offset;
	unsigned long long curr_write_offset_org;
	unsigned char *curr_read_ptr;
	unsigned char fill_staging_buffer[DATA_MAX_SIZE];
	unsigned int remaining_chunks;
	int fd;
	sparse_header_t *s_header;
	chunk_header_t *c_header;

	fd = bolt_open((char *)devname);
	if (fd < 0) {
		os_printf("Error opening '%s' for fastboot flash\n", devname);
		res = BOLT_ERR_DEVOPEN;
		goto exit;
	}

	s_header = (sparse_header_t *) transfer_buffer;

#ifdef FB_DBG_PRINT_SPARSE_IMG_HDR
	DLOG("s_header->file_hdr_sz = %d\n", s_header->file_hdr_sz);
	DLOG("s_header->chunk_hdr_sz = %d\n", s_header->chunk_hdr_sz);
	DLOG("s_header->blk_sz = %d\n", s_header->blk_sz);
	DLOG("s_header->total_blks = %d\n", s_header->total_blks);
	DLOG("s_header->total_chunks = %d\n", s_header->total_chunks);
#endif
	/* make sure the raw image after unsparse operation will fit into
	 * the partition to be flashed */
	if (((unsigned long long) s_header->blk_sz * (unsigned long long) s_header->total_blks) > ptn->length) {
		os_printf("Sparse image too large for the partition\n");
		res = BOLT_ERR_INV_PARAM;
		goto exit;
	}

	os_printf("Flashing Sparse Image to '%s' partition...\n", ptn->name);

	chunk_hdr_offset = s_header->file_hdr_sz;
	remaining_chunks = s_header->total_chunks;
	c_header = (chunk_header_t *) (transfer_buffer + chunk_hdr_offset);

	/* Note: refer to the comment in cb_flash() to understand why we can
	 * always use the start of the specified partition as the start of the
	 * write-offset even for the case where a large sparse-file is being
	 * resparsed to smaller sparse-file by host and are sent in mutiple
	 * downloads. */
	curr_write_offset = (unsigned long long) ptn->start * BYTES_PER_LBA;

	DLOG("total chunks: %d, start at LBA: 0x%x\n",
				s_header->total_chunks, ptn->start);

	while (remaining_chunks) {
		/* BSU responsbility to call back to BOLT to let background
		 * task to get some time to run as BOLT code operates in
		 * polling model. */
		poll_task();

#ifdef FB_DBG_PRINT_CHUNK_HDR
		DLOG("remaining_chunks=%u\n", remaining_chunks);
		DLOG("writing to LBA: 0x%llx\n", (curr_write_offset/BYTES_PER_LBA));
		DLOG("c_header.chunk_type=0x%x\n", c_header->chunk_type);
		DLOG("c_header.chunk_sz=%d\n", c_header->chunk_sz);
		DLOG("c_header.total_sz=%d\n", c_header->total_sz);
#endif

		switch (c_header->chunk_type) {
		case CHUNK_TYPE_RAW:
			byte_cnt = (c_header->chunk_sz * s_header->blk_sz);
			curr_read_ptr = ((unsigned char *)c_header) + s_header->chunk_hdr_sz;
			/* save a copy of the original curr_write_offset as we
			 * need it to compute the offset for next chunk */
			curr_write_offset_org = curr_write_offset;

			while (byte_cnt >= DATA_MAX_SIZE)
			{
				amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset, curr_read_ptr, DATA_MAX_SIZE);
				if (amtcopy != DATA_MAX_SIZE) {
					os_printf("Failed to write image. Remaining chunk: %d\n", remaining_chunks);
					res = BOLT_ERR_IOERR;
					goto exit;
				}
				curr_write_offset += amtcopy;
				curr_read_ptr += amtcopy;
				byte_cnt -= amtcopy;
				poll_task();
			}

			if (byte_cnt) {
				/* perform block write to the destination device */
				amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset, curr_read_ptr, byte_cnt);
				if (amtcopy != byte_cnt) {
					os_printf("Failed to write image. Remaining chunk: %d\n", remaining_chunks);
					res = BOLT_ERR_IOERR;
					goto exit;
				}
			}
			/* restore curr_write_offset for writing next chunk */
			curr_write_offset = curr_write_offset_org;
			break;
		case CHUNK_TYPE_FILL:
			byte_cnt = (c_header->chunk_sz * s_header->blk_sz);
			/* save a copy of the original curr_write_offset as we
			 * need it to compute the offset for next chunk */
			curr_write_offset_org = curr_write_offset;
			curr_read_ptr = ((unsigned char *)c_header) + s_header->chunk_hdr_sz;

			limit = FILL_MAX_SIZE;
			if (byte_cnt > DATA_MAX_SIZE) {
				limit = DATA_MAX_SIZE;
				os_memset(fill_staging_buffer, 0, DATA_MAX_SIZE);
				offset = 0;
				while (limit != 0) {
					os_memcpy(fill_staging_buffer+offset, curr_read_ptr, FILL_MAX_SIZE);
					limit -= FILL_MAX_SIZE;
					offset += FILL_MAX_SIZE;
				}
				limit = DATA_MAX_SIZE;
				curr_read_ptr = fill_staging_buffer;
			}

			while (byte_cnt >= limit)
			{
				amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset, curr_read_ptr, limit);
				if (amtcopy != limit) {
					os_printf("Failed to fill image. Remaining chunk: %d\n", remaining_chunks);
					res = BOLT_ERR_IOERR;
					goto exit;
				}
				curr_write_offset += amtcopy;
				byte_cnt -= amtcopy;
				poll_task();
			}
			if (byte_cnt) {
				amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset, curr_read_ptr, byte_cnt);
				if (amtcopy != byte_cnt) {
					os_printf("Failed to write image. Remaining chunk: %d\n", remaining_chunks);
					res = BOLT_ERR_IOERR;
					goto exit;
				}
			}
			/* restore curr_write_offset for writing next chunk */
			curr_write_offset = curr_write_offset_org;
			break;
		case CHUNK_TYPE_DONT_CARE:
			/* do nothing since this is a don't care chunk */
			break;
		default:
			/* error */
			os_printf("Unknown chunk type\n");
			res = BOLT_ERR;
			goto exit;
		}

		curr_write_offset += (c_header->chunk_sz * s_header->blk_sz);
		c_header = (chunk_header_t *) (((uint8_t *) c_header) + c_header->total_sz);
		remaining_chunks--;

		/* provide some feedback to user */
		os_printf(".");
		if (!((s_header->total_chunks - remaining_chunks) % 64))
			os_printf("\n");
	}
	os_printf("\n");
	os_printf("processed %d chunks\n", s_header->total_chunks);

exit:
	if (fd > 0)
		bolt_close(fd);
	return res;
}

/* Write raw image to flash device */
static int fastboot_flash_write_raw_image(const char *devname,
					uint8_t *transfer_buffer,
					struct fastboot_ptentry *ptn)
{
	int res = BOLT_OK;
	unsigned int byte_cnt, amtcopy;
	unsigned long long curr_write_offset;
	unsigned char *curr_read_ptr;
	int fd;

	fd = bolt_open((char *)devname);
	if (fd < 0) {
		os_printf("Error opening '%s' for fastboot flash\n", devname);
		res = BOLT_ERR_DEVOPEN;
		goto exit;
	}

	byte_cnt = download_bytes;

	/* make sure the raw image can fit into the partition to be flashed */
	if (byte_cnt > ptn->length) {
		os_printf("Raw image too large for the partition\n");
		res = BOLT_ERR_INV_PARAM;
		goto exit;
	}

	os_printf("Flashing Raw Image to '%s' partition...\n", ptn->name);

	curr_write_offset = (unsigned long long) ptn->start * BYTES_PER_LBA;
	curr_read_ptr = transfer_buffer;

	DLOG("start at LBA: 0x%x\n", ptn->start);

	while (byte_cnt >= DATA_MAX_SIZE) {
		amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset,
						curr_read_ptr, DATA_MAX_SIZE);
		if (amtcopy != DATA_MAX_SIZE) {
			os_printf("Failed to write image. Remaining bytes: %d\n",
						byte_cnt);
			res = BOLT_ERR_IOERR;
			goto exit;
		}
		curr_write_offset += amtcopy;
		curr_read_ptr += amtcopy;
		byte_cnt -= amtcopy;
		poll_task();
	}

	if (byte_cnt) {
		amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset,
						curr_read_ptr, byte_cnt);
		if (amtcopy != byte_cnt) {
			os_printf("Failed to write image. Remaining bytes: %d\n",
						byte_cnt);
			res = BOLT_ERR_IOERR;
			goto exit;
		}
	}
	
exit:
	if (fd > 0)
		bolt_close(fd);
	return res;
}

/* Update the partition table with the gpt.bin that is created by 'makegpt'
 * tool provided in Android source repo. The tool creates gpt.bin that
 * includes the Protective MBR and the Primary GPT.
 * That's why the code here will always flash the gpt.bin image to
 * LBA 0 */
static int fastboot_flash_write_gpt(const char *devname,
					const char *partition)
{
	int res = BOLT_OK;
	struct fastboot_ptentry *ptn;
	uint8_t *download_buffer = (uint8_t *)FB_FLASH_STAGING_BUFFER;
	struct fastboot_ptentry gpt_ptn;

	/* First check if gpt partition exist.	If it does, then we use what
	 * is specified. If not, then we setup the data structure so it can be
	 * flashed in like any regular raw image */
	ptn = fastboot_flash_find_ptn(partition);

	if (ptn == 0) {
		os_printf("partition '%s' does not exist...", partition);
		os_printf("but it will still be flashed to GPT location\n");

		/* For flashing Primary GPT, always flash to LBA0 as this code
		 * assumes the gpt.bin provided via fastboot flash includes
		 * the Protective MBR.	Therefore the size of this 'gpt'
		 * paritition accounts for:
		 *  1. Protective MBR (LBA 0)
		 *  2. Primary GPT Header (LBA 1)
		 *  3. 128 GPT entries (LBA 2 - 33) */
		os_sprintf(&gpt_ptn.name[0], "gpt");
		gpt_ptn.length = SIZEOF_GPT * BYTES_PER_LBA;
		gpt_ptn.start = 0; /* start at LBA 0 because of how gpt.bin is created */
		gpt_ptn.flags = 0;

		ptn = &gpt_ptn;
	}

	/* Make sure the image provided is a valid GPT */
	res = fastboot_validate_gpt_image(download_buffer);

	if (res < 0) {
		os_printf("Invalid GPT image -- cannot update GPT content\n");
		goto exit;
	}

	/* GPT is valid, now we can write to device as raw image */
	res = fastboot_flash_write_raw_image(devname, download_buffer, ptn);
	if (res < 0) {
		goto exit;
	}
	os_printf("Finished updating the GPT content\n");

	/* Reload the new GPT from flash to ram */
	os_printf("Reloading partitions from the updated GPT...\n");
	res = fastboot_discover_gpt_tables(fb_info.flash_devname, 1);

	/* Since we have just flashed the device with a new GPT image, it is
	 * expected that a valid GPT can be found on the flash device.
	 * So treat all error code as a failure here.*/
	if (res < 0) {
		os_printf("Error accessing flash device: %s\n",
						fb_info.flash_devname);
		/* override error code to be more explicit about failure reason */
		res = BOLT_ERR_IOERR;
		goto exit;
	}

	/* Try to populate "bootloader" partition as well after all other
	 * partitions are read out from GPT properly. */
	fastboot_discover_bolt_ptn(fb_info.flash_devname, fb_info.boltimg_devname);

exit:
	return res;
}

static int fastboot_flash_write_bootloader(const char *flash_devname,
						const char *boltimg_devname,
						const char *partition,
						char *response)
{
	int res = BOLT_OK;
	char ptn_name[15];
	struct fastboot_ptentry *ptn;
	struct bootloader_img_hdr *bl_img_hdr;
	uint8_t *download_buffer = (uint8_t *)FB_FLASH_STAGING_BUFFER;
	uint8_t *bolt_img_buf;
	uint8_t *bsu_img_buf;
	uint8_t *bl31_img_buf;

	bl_img_hdr = (struct bootloader_img_hdr *) download_buffer;

	/* Check on a few things to make sure the image downloaded is valid */
	if (bl_img_hdr->magic != BL_IMG_HEADER_MAGIC) {
		os_printf("invalid bootloader image hdr magic value '%#010x'\n",
							bl_img_hdr->magic);
		os_sprintf(response, "FAILinvalid bootloader image hdr magic value");
		goto exit;
	}

	if ((download_bytes < bl_img_hdr->bolt_img_size) ||
		(download_bytes < bl_img_hdr->bsu_img_size) ||
		((bl_img_hdr->version > 0x1) && (download_bytes < bl_img_hdr->bl31_img_size))) {
		os_printf("invalid bootloader image size...\n");
		os_printf("download_bytes=%d, bolt_img_size=%d, bsu_img_size=%d, bl31_img_size=%d\n",
				download_bytes, bl_img_hdr->bolt_img_size,
						bl_img_hdr->bsu_img_size, bl_img_hdr->bl31_img_size);
		os_sprintf(response, "FAILinvalid bootloader image size");
		goto exit;
	}

	if ((download_bytes < bl_img_hdr->bolt_img_offset) ||
		(download_bytes < bl_img_hdr->bsu_img_offset) ||
		((bl_img_hdr->version > 0x1) && bl_img_hdr->bl31_img_size && (download_bytes < bl_img_hdr->bl31_img_offset))) {
		os_printf("invalid bootloader image offset...\n");
		os_printf("download_bytes=%d, bolt_img_offset=%d, bsu_img_offset=%d, bl31_img_offset=%d\n",
				download_bytes, bl_img_hdr->bolt_img_offset,
						bl_img_hdr->bsu_img_offset, bl_img_hdr->bl31_img_offset);
		os_sprintf(response, "FAILinvalid bootloader image offset");
		goto exit;
	}

	/***** Handle BOLT image *****/
	os_sprintf(ptn_name, "bolt");

	ptn = fastboot_flash_find_ptn(ptn_name);

	if (ptn == 0) {
		os_printf("'%s' partition does not exist\n", ptn_name);
		os_sprintf(response, "FAIL'%s' partition does not exist", ptn_name);
		goto exit;
	}

	if ((bl_img_hdr->bolt_img_size > ptn->length)) {
		os_printf("image too large for '%s' ptn, img_size=%d, ptn.length=%d\n",
				ptn_name, bl_img_hdr->bolt_img_size, ptn->length);
		os_sprintf(response, "FAILimage too large for '%s' partition", ptn_name);
		goto exit;
	}

	/* override the global variable "download_bytes" to be the size of the
	 * BOLT image as reported in header so we can re-use the same function
	 * as writing any raw image */
	download_bytes = bl_img_hdr->bolt_img_size;
	bolt_img_buf = download_buffer+bl_img_hdr->bolt_img_offset;

	os_printf("%s image: write %d bytes from %#010x to '%s' device\n",
			ptn_name,
			download_bytes,
			(unsigned int)bolt_img_buf,
			boltimg_devname);

	res = fastboot_flash_write_raw_image(boltimg_devname, bolt_img_buf, ptn);

	if (res < 0) {
		os_printf("Failed in flashing %s image\n", ptn_name);
		os_sprintf(response, "FAILerror writing to '%s' partition", ptn_name);
		goto exit;
	}

	os_printf("Done flashing %s image\n", ptn_name);

	/***** Handle BSU image *****/
	os_sprintf(ptn_name, "bsu");

	ptn = fastboot_flash_find_ptn(ptn_name);

	if (ptn == 0) {
		os_printf("'%s' partition does not exist\n", ptn_name);
		os_sprintf(response, "FAIL'%s' partition does not exist", ptn_name);
		goto exit;
	}

	if ((bl_img_hdr->bsu_img_size > ptn->length)) {
		os_printf("image too large for '%s' ptn, img_size=%d, ptn.length=%d\n",
				ptn_name, bl_img_hdr->bsu_img_size, ptn->length);
		os_sprintf(response, "FAILimage too large for '%s' partition", ptn_name);
		goto exit;
	}

	/* override the global variable "download_bytes" to be the size of the
	 * BSU image as reported in header so we can re-use the same function
	 * as writing any raw image */
	download_bytes = bl_img_hdr->bsu_img_size;
	bsu_img_buf = download_buffer+bl_img_hdr->bsu_img_offset;

	os_printf("%s image: write %d bytes from %#010x to '%s' device\n",
			ptn_name,
			download_bytes,
			(unsigned int)bsu_img_buf,
			flash_devname);

	res = fastboot_flash_write_raw_image(flash_devname, bsu_img_buf, ptn);

	if (res < 0) {
		os_printf("Failed in flashing %s image\n", ptn_name);
		os_sprintf(response, "FAILerror writing to '%s' partition", ptn_name);
		goto exit;
	}

	os_printf("Done flashing %s image\n", ptn_name);

	/***** Handle BL31 image (version > 0x1) *****/
	if ((bl_img_hdr->version > 0x1) && bl_img_hdr->bl31_img_size) {
		os_sprintf(ptn_name, "bl31");

		ptn = fastboot_flash_find_ptn(ptn_name);

		if (ptn == 0) {
			os_printf("'%s' partition does not exist\n", ptn_name);
			os_sprintf(response, "FAIL'%s' partition does not exist", ptn_name);
			goto exit;
		}

		if ((bl_img_hdr->bl31_img_size > ptn->length)) {
			os_printf("image too large for '%s' ptn, img_size=%d, ptn.length=%d\n",
					ptn_name, bl_img_hdr->bl31_img_size, ptn->length);
			os_sprintf(response, "FAILimage too large for '%s' partition", ptn_name);
			goto exit;
		}

		/* override the global variable "download_bytes" to be the size of the
		 * BL31 image as reported in header so we can re-use the same function
		 * as writing any raw image */
		download_bytes = bl_img_hdr->bl31_img_size;
		bl31_img_buf = download_buffer+bl_img_hdr->bl31_img_offset;

		os_printf("%s image: write %d bytes from %#010x to '%s' device\n",
				ptn_name,
				download_bytes,
				(unsigned int)bl31_img_buf,
				flash_devname);

		res = fastboot_flash_write_raw_image(flash_devname, bl31_img_buf, ptn);

		if (res < 0) {
			os_printf("Failed in flashing %s image\n", ptn_name);
			os_sprintf(response, "FAILerror writing to '%s' partition", ptn_name);
			goto exit;
		}

		os_printf("Done flashing %s image\n", ptn_name);
	}

	/***** All bootloader images flashed successfully *****/
	os_sprintf(response, "OKAY");

exit:
	return BOLT_OK;
}

static int has_boot_commander()
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		goto ret_legacy;
	}
	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);
	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n", flash_devname, fd);
		goto ret_legacy;
	}
	bolt_close(fd);
	return 1;

ret_legacy:
	return 0;
}

static int fastboot_flash_dev_write(const char *flash_devname,
					const char *boltimg_devname,
					const char *partition,
					char *response)
{
	int res;
	struct fastboot_ptentry *ptn;
	sparse_header_t *s_header;
	uint8_t *download_buffer = (uint8_t *)FB_FLASH_STAGING_BUFFER;
	char devname[15];

	/* If the partition to be flashed is called 'bootloader', then it
	 * means both BOLT and BSU images have to be flashed.  We need a
	 * special function to unpack the bootloader image and flash the
	 * both BOLT and BSU images accordingly. */
	if (os_strcmp(partition, "bootloader") == 0) {
		res = fastboot_flash_write_bootloader(flash_devname,
					boltimg_devname, partition, response);
		/* 'response' string has been filled after calling the above
		 * function, so we can just return here */
		return res;
	}

	/* If the partition to be flashed is called 'bolt', then it means BOLT
	 * has to be flashed.  BOLT image is always located in a device
	 * different from the rest of the Android images (including 'bsu'
	 * image) */
	if (os_strcmp(partition, "bolt") == 0)
		os_sprintf(devname, "%s", boltimg_devname);
	else
		os_sprintf(devname, "%s", flash_devname);

	os_printf("BOLT device to be flashed to: %s\n", devname);

	/* If the partition to be flashed is called 'gpt' we need to take
	 * special care because we assume user wants to flash GPT table.
	 * We call a special function to take care of it. */
	if (os_strcmp(partition, "gpt") == 0) {
		res = fastboot_flash_write_gpt(devname, partition);
		goto exit_send_resp;
	}

	/* Take care of partitions except 'gpt' & 'bootloader' */
	ptn = fastboot_flash_find_ptn(partition);

	if (ptn == 0) {
		os_printf("partition '%s' does not exist\n", partition);
		os_sprintf(response, "FAILpartition does not exist");
		return BOLT_OK;
	}

	if (download_bytes > ptn->length) {
		os_printf("image too large for the partition, download_bytes=%d, ptn.length=%d\n",
				download_bytes, ptn->length);
		os_sprintf(response, "FAILimage too large for partition");
		return BOLT_OK;
	}

	s_header = (sparse_header_t *) download_buffer;

#ifdef FB_DBG_PRINT_IMG_HDR
	unsigned char *tmp_buf;
	int n, idx;

	DLOG("s_header.magic = 0x%x\n", s_header->magic);
	tmp_buf = (unsigned char *)s_header;
	for (n=0; n<7; n++) {
		os_printf("n=%d: ", n);
		for (idx=0; idx<4; idx++){
			os_printf("0x%x ", *(tmp_buf+ (n*4+idx)));
		}
		os_printf("\n");
	}
#endif

	/* Check if there is the magic number for Sparse image*/
	if ((s_header->magic == SPARSE_HEADER_MAGIC) &&
					(s_header->major_version == 1))	{
		res = fastboot_flash_write_sparse_image(devname,
							download_buffer, ptn);
	} else {
		res = fastboot_flash_write_raw_image(devname,
							download_buffer, ptn);
	}

exit_send_resp:
	if (res < 0) {
		os_printf("Error: writing to partition: %s\n", partition);
		os_sprintf(response, "FAILerror in writing to flash device (err code: %d)", res);
	} else {
		os_printf("Done\n");	
		os_sprintf(response, "OKAY");
		if (has_boot_commander()) {
			clear_dmv_corrupt(partition);
		}
	}

	return BOLT_OK;
}

static int fastboot_erase_ptn(const char *devname, const char *partition)
{
	int res = BOLT_OK;
	unsigned int byte_cnt, amtcopy;
	unsigned long long curr_write_offset;
	unsigned char *curr_read_ptr;
	int fd = 0;
	struct fastboot_ptentry *ptn;
	uint8_t *input_buffer = (uint8_t *) FB_FLASH_STAGING_BUFFER;

	DLOG("Partition to be erased: %s\n", partition);

	/* Look up for the info for the partition to be erased */
	ptn = fastboot_flash_find_ptn(partition);

	if (ptn == 0) {
		DLOG("partition '%s' does not exist\n", partition);
		res = FB_ERR_PARTITION_NOT_EXIST;
		goto exit;
	}

	/* In order to erase the partition, we need to use the device block
	 * write function as there is no device ioctl call to do the erasing.
	 * So we will reuse the image download staging buffer as the source
	 * pointer for block write.  Zero out the input buffer in order to
	 * wipe out the partition.
	 * DATA_MAX_SIZE is the amount of data we can write for each call to
	 * the device block write function. */
	os_memset(input_buffer, 0, DATA_MAX_SIZE);

	fd = bolt_open((char *)devname);
	if (fd < 0) {
		os_printf("Error opening '%s' for fastboot erase\n", devname);
		res = BOLT_ERR_DEVOPEN;
		goto exit;
	}

	/* Set the amount of data to write to match with partition size */
	byte_cnt = ptn->length;

	curr_write_offset = (unsigned long long) ptn->start * BYTES_PER_LBA;
	curr_read_ptr = input_buffer;

	DLOG("start at LBA: 0x%x\n", ptn->start);

	while (byte_cnt >= DATA_MAX_SIZE) {
		amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset,
						curr_read_ptr, DATA_MAX_SIZE);
		if (amtcopy != DATA_MAX_SIZE) {
			os_printf("Failed to write image. Remaining bytes: %d\n",
						byte_cnt);
			res = BOLT_ERR_IOERR;
			goto exit;
		}
		curr_write_offset += amtcopy;
		byte_cnt -= amtcopy;
		poll_task();
	}

	if (byte_cnt) {
		amtcopy = bolt_writeblk(fd, (bolt_offset_t) curr_write_offset,
						curr_read_ptr, byte_cnt);
		if (amtcopy != byte_cnt) {
			os_printf("Failed to write image. Remaining bytes: %d\n",
						byte_cnt);
			res = BOLT_ERR_IOERR;
			goto exit;
		}
	}

exit:
	if (fd > 0)
		bolt_close(fd);
	if (res == BOLT_OK) {
		if (has_boot_commander()) {
			clear_dmv_corrupt(partition);
		}
	}
	return res;
}

/* Helper function to determine if we can use fastboot flash to update the
 * BOLT image.
 * If this is possible, then add both the "bolt" and "bootloader" partitions to
 * the local copy of the partition table which the rest of the fastboot code
 * uses to respond to other commands (such as "fastboot flash" or "fastboot
 * getvar partition-[type|size]"
 *
 * Note: This function updates the "bolt_devname" parameter in place so the
 * caller function can save it to the global "fb_info" data structure.
 */
static void fastboot_discover_bolt_ptn(const char *devname, char *bolt_devname)
{
	int fd = -1;
	int res;
	struct fastboot_ptentry ptn;
	struct flash_info flashinfo;

	/* Determine if we can flash BOLT by checking if the "-device"
	 * parameter provided by user when calling "android fastboot" is
	 * 'flash0'
	 * The reason we check this exact name is because we know this is the
	 * naming scheme that BOLT uses to distinguish whether BOLT is booted
	 * from SPI or eMMC on platform that has eMMC flash on board.
	 * See emmcdrv_probe().
	 * We limit the use-case of using fastboot to flash BOLT image
	 * for platforms that boot both BOLT image and Android from eMMC.
	 * Everything else, we don't attempt to populate the "bootloader" or
	 * "bolt" partition for other fastboot code to be accessible */
	if (os_strcmp(devname, "flash0") != 0) {
		goto exit;
	}

	/* Set the BOLT flash device name that can be used to access the
	 * device that stores BOLT image.
	 * We assume BOLT image is always in eMMC Boot1 hardware partition and
	 * so hardcoded the name here. */
	os_sprintf(bolt_devname, "flash1");

	/* Get the BOLT partition size */
	fd = bolt_open((char *)bolt_devname);
	if (fd < 0) {
		os_printf("Failed to open '%s'. Bootloader partition not available.\n",
				bolt_devname);
		goto exit;
	}

	res = bolt_ioctl(fd, IOCTL_FLASH_GETPARTINFO, &flashinfo,
			sizeof(flashinfo), NULL, 0);
	if (res != BOLT_OK) {
		os_printf("Failed to get '%s' partition size. Bootloader partition not available.\n",
				bolt_devname);
		goto exit;
	}

	/* Getting to this point means we have enough information to add
	 * the "bolt" and "bootloader" to our local copy of the partition table
	 *
	 * "bolt" partition is added so we can look it up when we actually
	 * flah the bolt image.
	 *
	 * "bootloader" partition is created to allow reporting it as a valid
	 * partition to be used when
	 * "fastboot getvar partition-[type|size]:bootloader" is sent from host */
	os_sprintf(&ptn.name[0], "bolt");
	ptn.length = flashinfo.flash_size;
	ptn.start = 0; /* always write BOLT image to the start of the device partition */
	ptn.flags = 0;

	os_printf("Adding: %32s, offset 0x%8.8x, size 0x%16.16llx, flags 0x%16.16llx\n",
			ptn.name, ptn.start, ptn.length, ptn.flags);

	fastboot_flash_add_ptn(&ptn);

	os_sprintf(&ptn.name[0], "bootloader"); /* keep same length & start as 'bolt' ptn */
	os_printf("Adding: %32s, offset 0x%8.8x, size 0x%16.16llx, flags 0x%16.16llx\n",
			ptn.name, ptn.start, ptn.length, ptn.flags);

	fastboot_flash_add_ptn(&ptn);

	goto exit_ret_valid_devname; /* everything is good, return valid bolt image device name */

exit:
	/* Getting to this point means we encountered error in trying to add the
	 * "bootloader" to our local copy of the partition table. Make sure
	 * the bolt image device name is an empty string to prevent subsequent
	 * call to "fastboot flash bootloader" from executing successfully. */
	bolt_devname[0] = '\0';

exit_ret_valid_devname:
	if (fd > 0)
		bolt_close(fd);
	return;
}

static int get_max_transfer_size(int transport_mode, int transport_fh)
{
	int max_transfer_size = -1;
	int res;

	if (transport_mode == FB_TRANSPORT_TCP) {
		/* For TCP transport device, we can set the size based on what
		 * we choose from fastboot application layer */
		max_transfer_size = TCP_RECV_BUFFER_SIZE;
	} else {
		/* For USB transport device, we need to find out the max
		 * BULK OUT transfer size via ioctl call*/
		res = bolt_ioctl(transport_fh,
					IOCTL_USBBDC_GET_BULKOUT_MAXSIZE,
					&max_transfer_size,
					sizeof(max_transfer_size), NULL, 0);
		if (res < 0) {
			/* return error code as transfer size so caller can
			 * report this to user accordingly */
			max_transfer_size = res;
		} else {
			/* we should use the max value reported by the USB BDC
			 * driver unless the number is completely off and we
			 * cannot read more data than we allocate our buffer
			 * for it.*/
			if (max_transfer_size > USB_BULKOUT_BUFFER_SIZE)
				max_transfer_size = USB_BULKOUT_BUFFER_SIZE;
		}
	}

	return max_transfer_size;
}

static int get_min_transfer_size(int transport_mode, int transport_fh)
{
	int min_transfer_size = -1;
	int res;

	if (transport_mode == FB_TRANSPORT_TCP) {
		/* For TCP transport device, both min and max transfer size
		 * are the same. See comment in get_max_transfer_size() */
		min_transfer_size = TCP_RECV_BUFFER_SIZE;
	} else {
		/* For USB transport device, we need to find out the min
		 * BULK OUT transfer size via ioctl call*/
		res = bolt_ioctl(transport_fh,
					IOCTL_USBBDC_GET_BULKOUT_MINSIZE,
					&min_transfer_size,
					sizeof(min_transfer_size), NULL, 0);
		if (res < 0) {
			/* return error code as transfer size so caller can
			 * report this to user accordingly */
			min_transfer_size = res;
		} else {
			/* The min transfer size should be at least the
			 * smallest possible value allowed by USB standard
			 * for max-packet-size.  If not, we treat it as error*/
			if (min_transfer_size < USB_BULKOUT_MAXPKT_SIZE) {
				os_printf("Error: min_transfer_size reported by USB driver is %d bytes\n",
						min_transfer_size);
				os_printf("It is less than what is expected (%d)\n",
						USB_BULKOUT_MAXPKT_SIZE);
				min_transfer_size = -1; /* treat as error */
			}
		}
	}

	return min_transfer_size;
}

static int queue_next_transfer(unsigned int transfer_size)
{
	int res;

	/* There is nothing we need to do for TCP, it is only
	 * the USB transport device that we need to queue the transfer
	 * before we can get any data from the device driver */
	if (fb_info.transport_mode == FB_TRANSPORT_TCP)
		return BOLT_OK;

	if (!transfer_size) {
		os_printf("Invalid transfer size (size=%d)\n", transfer_size);
		return BOLT_ERR_INV_PARAM;
	}

	res = bolt_ioctl(fb_info.transport_dev_fd, IOCTL_USBBDC_QUEUE_BULKOUT,
					&transfer_size,
					sizeof(transfer_size), NULL, 0);
	if (res < 0) {
		os_printf("Failed to queue next transfer (size=%d, res=%d)\n",
						transfer_size, res);
		return BOLT_ERR_IOERR;
	}

	return BOLT_OK;
}

static int fastboot_init(int fb_transport_mode, char *fb_flashdev_name)
{
	int fd, attempts, delay;
	int res = BOLT_OK;
	char trans_devname[15];

	fb_info.dev_lock_state_change_req_cnt = 0;

	fb_info.transport_mode = fb_transport_mode;

	if (fb_transport_mode == FB_TRANSPORT_TCP) {
		os_sprintf(trans_devname, "%s", "tcpfastboot0");
		attempts = 1;
		delay = 0;
	} else {
		os_sprintf(trans_devname, "%s", "usbdev0");
		attempts = 3;
		delay = 500;
	}

	/* Open transport layer device and allocate memory for receive buffer
	 * based on transport protocol requirement */		
	DLOG("Try to open connection to '%s'...\n", trans_devname);
	for(; attempts > 0; attempts--) {
		fd = bolt_open(trans_devname);
		if (fd >= 0)
			break;
		else
			msleep(delay);
	}

	if (fd < 0) {
		os_printf("Error opening %s for fastboot connection. fd=%d.\n",
			  trans_devname, fd);
		res = BOLT_ERR_IOERR;
		goto exit;
	}

	fb_info.transport_dev_fd = fd;

	/* Get the min and max size allowed based on transport mode and save
	 * into internal context structure.
	 * We always allocate the actual buffer based on the max size */
	res = get_max_transfer_size(fb_info.transport_mode,
						fb_info.transport_dev_fd);
	if (res < 0) {
		os_printf("Error reading max transfer size from '%s' (%d)\n",
						trans_devname, res);
		res = BOLT_ERR_IOERR;
		goto exit;
	}
	fb_info.max_transfer_size = (unsigned int) res;
	fb_info.cmd_buf = os_malloc(fb_info.max_transfer_size);
	if (!fb_info.cmd_buf) {
		res = BOLT_ERR_NOMEM;
		goto exit;
	}

	res = get_min_transfer_size(fb_info.transport_mode,
						fb_info.transport_dev_fd);
	if (res < 0) {
		os_printf("Error reading min transfer size from '%s' (%d)\n",
						trans_devname, res);
		res = BOLT_ERR_IOERR;
		goto exit;
	}
	fb_info.min_transfer_size = (unsigned int) res;

	DLOG("Connection to '%s' is OK.\n", trans_devname);

	/* Populate GPT from destination flash device to internal data
	 * structure to prepare for 'fastboot flash' command */
	os_sprintf(fb_info.flash_devname, "%s", fb_flashdev_name);

	DLOG("Checking GPT table from '%s'...\n", fb_info.flash_devname);

	res = fastboot_discover_gpt_tables(fb_info.flash_devname, 1);

	/* First check if something is wrong in accessing the flash device */
	if (FB_ERR_FAIL_TO_ACCESS_FLASH_DEV == res) {
		os_printf("Error accessing flash device: %s\n",
						fb_info.flash_devname);
		res = BOLT_ERR_IOERR;
		goto exit;
	}

	/* We still need to accept fastboot command even if a valid GPT cannot
	 * be found on the flash device. This allows a valid GPT to be flashed
	 * to the device via fastboot*/
	if (FB_ERR_NO_VALID_GPT == res) {
		os_printf("\n~~~~~ WARNING ~~~~~\n");
		os_printf("You can only fastboot flash a gpt image to 'gpt' partition from host.\n");
		os_printf("Do NOT flash other partitions until a valid GPT is loaded!!!\n\n");
		/* override the return code so we can accept fastboot command.
		 * Note that if user attemps to flash partitions other than
		 * 'gpt', then the cb_flash() will catch this error because it
		 * won't be able to find the corresponding partition.*/
		res = BOLT_OK;
	} else {
		/* A valid GPT has been found and all necessary partitions
		 * have been populated except the "bootloader" partition.
		 * Now we try to populate "bootloader" partition as well.*/
		fastboot_discover_bolt_ptn(fb_info.flash_devname, fb_info.boltimg_devname);
	}

	os_printf("Ready to accept fastboot cmd\n");
exit:
#if CFG_SPLASH
	if (res == BOLT_OK) {
		splash_feedback(BOOT_SPLASH_FASTBOOT);
	} else {
		splash_feedback(BOOT_SPLASH_FAILED);
	}
#endif
	return res;
}

static void fastboot_uninit(void)
{
	if (fb_info.transport_dev_fd > 0)
		bolt_close(fb_info.transport_dev_fd);

	if (fb_info.cmd_buf)
		os_free(fb_info.cmd_buf);

	os_printf("Fastboot connection closed.\n");
}

/*
 * Callback functions to handle fastboot commands
 */

static int cb_reboot(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char response[RESPONSE_LEN];

	os_sprintf(response, "OKAY");
	res = fastboot_tx_write_str(response);

	/* Determine if the host command is "reboot" vs. "reboot-bootloader"
	 * because both commands are handled by this callback function.
	 * If it is "reboot-bootloader", we purposely write to the boot reason
	 * register to get device into bootloader in next auto-boot session.*/
	if (!strcmp_l1("reboot-bootloader", cmd)) {
		/* First letter of "bootloader" is written as we need to make
		 * it the same as what Android kernel would do */
		boot_reason_reg_set('b');
	}

	/* 1 sec delay to wait for response to be sent out.
	 * Note that background task is polled during the
	 * call in msleep() -- this enables the transport
	 * device driver code to be called */
	os_printf("Rebooting as requested from host...\n");
	msleep (1000);

	bolt_docommands("reboot");

	return res;
}

#define MEM0_DEVICE "mem0"
static int cb_boot(struct fastboot_info *info)
{
	int res;
	char response[RESPONSE_LEN];
	int len;
	char boot_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */

	android_boot_addloader();

	len = os_sprintf(boot_cmd, "boot -rawfs ");
	if (!os_strncmp((const char *)FB_FLASH_STAGING_BUFFER, BOOT_MAGIC, os_strlen(BOOT_MAGIC))) {
		len += os_sprintf(boot_cmd + len, "-loader=img ");
	}
	len += os_sprintf(boot_cmd + len, "%s:0x%x", MEM0_DEVICE, FB_FLASH_STAGING_BUFFER);

	os_sprintf(response, "OKAY");
	res = fastboot_tx_write_str(response);

	DLOG("boot_cmd=%s\n", boot_cmd);
	bolt_docommands(boot_cmd);
	return res;
}

static void getvar_version_baseband(char *cmd_var, char *response)
{
	char *resp_var;

	resp_var = env_getenv("VERSION_BASEBAND");
	if (resp_var)
		os_sprintf(response, "%s", resp_var);
	else
		os_sprintf(response, "N/A");
}

static void getvar_version_bootloader(char *cmd_var, char *response)
{
	char *resp_var;

	resp_var = env_getenv("BUILDTAG");
	if (resp_var)
		os_sprintf(response, "%s", resp_var);
	else
		os_sprintf(response, "N/A");
}

static void getvar_version_hardware(char *cmd_var, char *response)
{
	char *resp_var;

	resp_var = env_getenv("BOARDNAME");
	if (resp_var)
		os_sprintf(response, "%s", resp_var);
	else
		os_sprintf(response, "N/A");
}

static void getvar_version(char *cmd_var, char *response)
{
	os_sprintf(response, "%s", FASTBOOT_VERSION);
}

static void getvar_max_download_size(char *cmd_var, char *response)
{
	/* returns the maximum size of an image that can be downloaded in
	 * bytes in hex.  Images larger than this size will be sent using
	 * multiple sparse images */
	os_sprintf(response, "%#010lx", FB_FLASH_STAGING_BUFFER_SIZE);
}

extern char *get_serial_no(void);
static void getvar_serialno(char *cmd_var, char *response)
{
	char *resp_var;
	resp_var = get_serial_no();
	if (resp_var)
		os_sprintf(response, "%s", resp_var);
}

static void getvar_product(char *cmd_var, char *response)
{
	char *resp_var;
	resp_var = env_getenv("PRODUCTNAME");
	if (resp_var)
		os_sprintf(response, "%s", resp_var);
	else
		os_sprintf(response, "N/A");
}

static void getvar_secure(char *cmd_var, char *response)
{
	/* returns "yes" or "no" indicating whether bootloader will allow
	 * flashing */
	os_sprintf(response, "no");
}

static void getvar_unlocked(char *cmd_var, char *response)
{
	int lock_state;

	/* returns "yes" if the device is unlocked with "flashing unlocked",
	 * "no" otherwise */
	lock_state = oem_get_device_lock_state();
	if (UNLOCKED == lock_state)
		os_sprintf(response, "yes");
	else
		os_sprintf(response, "no");
}

static void getvar_off_mode_charge(char *cmd_var, char *response)
{
	/* Note: BOLT has not implemented "fastboot oem off-mode-charge" cmd
	 * and so use the same value reported from Nexus player for now. */
	os_sprintf(response, "0");
}

static void getvar_variant(char *cmd_var, char *response)
{
	/* returns the device variant (for instance US, ROW…) and must be
	 * empty if no variants exist
	 *
	 * Assume no variant at this moment and so purposely leaving this
	 * function as empty as there is nothing to report back */
}

static void getvar_partition_type(char *cmd_var, char *response)
{
	struct fastboot_ptentry *ptn;

	/* Continue to look for the next ":" token from same 'cmd_var' string
	 * pass in from caller function */
	os_strtok_r(NULL, ":", &cmd_var);

	/* Check if partition-name is provided.  If this parameter is not
	 * provided, then don't report anything extra */
	if (os_strlen(cmd_var) != 0) {
		ptn = fastboot_flash_find_ptn(cmd_var);

		/* Check if partition exists. If not exist, then don't report
		 * anything extra and return wihtout looking into the
		 * partition type */
		if (ptn == 0) {
			DLOG("partition '%s' does not exist\n", cmd_var);
			return;
		}

		/* Partition exists, now check if the partition type is defined
		 * for the specified partition.  If none of the bits for
		 * indicating filesystem type is set, then it means partition
		 * type is not defined and we always report 'emmc' */
		if (ptn->flags & GPT_ATTR_FLAG_PTN_TYPE_EXT4_MASK) {
			os_sprintf(response, "ext4");
		} else if (ptn->flags & GPT_ATTR_FLAG_PTN_TYPE_F2FS_MASK) {
			os_sprintf(response, "f2fs");
		} else {
			os_sprintf(response, "emmc");
		}
	}
}

static void getvar_partition_size(char *cmd_var, char *response)
{
	struct fastboot_ptentry *ptn;

	/* Continue to look for the next ":" token from same 'cmd_var' string
	 * pass in from caller function */
	os_strtok_r(NULL, ":", &cmd_var);

	/* Check if partition-name is provided.  If this parameter is not
	 * provided, then don't report anything extra */
	if (os_strlen(cmd_var) != 0) {
		ptn = fastboot_flash_find_ptn(cmd_var);

		/* Check if partition exists. If not exist, then don't report
		 * anything extra and return wihtout looking into the
		 * partition type */
		if (ptn == 0) {
			DLOG("partition '%s' does not exist\n", cmd_var);
			return;
		}

		/* Partition exists and we can report the size of partition in
		 * bytes in hex, i.e. "0x00000007255fbc00" */
		os_sprintf(response, "%#018llx", ptn->length);
	}
}

static int has_slot(char *name)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		goto ret_legacy;
	}
	os_sprintf(flash_devname, "%s.%s_%s", fb_flashdev_mode_str, name, BOOT_SLOT_0_SUFFIX);
	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s.\n", flash_devname);
		goto ret_legacy;
	}
	bolt_close(fd);
	os_sprintf(flash_devname, "%s.%s_%s", fb_flashdev_mode_str, name, BOOT_SLOT_1_SUFFIX);
	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s.\n", flash_devname);
		goto ret_legacy;
	}
	bolt_close(fd);
	return 1;

ret_legacy:
	return 0;
}

static int get_boot_commander(struct eio_boot *eio)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	int ret;

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		goto ret_legacy;
	}
	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);
	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n", flash_devname, fd);
		goto ret_legacy;
	}
	ret = bolt_readblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
	if (ret != sizeof(struct eio_boot)) {
		os_printf("Error reading %s. Can't read commander block: %d\n", flash_devname, ret);
		bolt_close(fd);
		goto ret_legacy;
	}
	bolt_close(fd);
	return 1;

ret_legacy:
	return 0;
}

static int write_boot_commander(struct eio_boot *eio)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	int ret;

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		goto ret_fail;
	}
	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);
	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n", flash_devname, fd);
		goto ret_fail;
	}
	ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
	if (ret != sizeof(struct eio_boot)) {
		os_printf("Error writing %s. Can't write commander block: %d\n", flash_devname, ret);
		bolt_close(fd);
		goto ret_fail;
	}
	bolt_close(fd);
	return 1;

ret_fail:
	return 0;
}

static void getvar_has_slot(char *cmd_var, char *response)
{
	os_strtok_r(NULL, ":", &cmd_var);
	if (os_strlen(cmd_var) != 0) {
		if (has_boot_commander() && has_slot(cmd_var)) {
			os_sprintf(response, "%s", "yes");
		} else {
			os_sprintf(response, "%s", "no");
		}
	}
}

static void getvar_current_slot(char *cmd_var, char *response)
{
	struct eio_boot eio;
	if (get_boot_commander(&eio)) {
		os_sprintf(response, "%s", eio.current == 1 ? BOOT_SLOT_1_SUFFIX : BOOT_SLOT_0_SUFFIX);
	}
}

static void getvar_slot_suffixes(char *cmd_var, char *response)
{
	if (has_boot_commander()) {
		os_sprintf(response, "%s,%s", BOOT_SLOT_0_SUFFIX, BOOT_SLOT_1_SUFFIX);
	}
}

static void getvar_slot_count(char *cmd_var, char *response)
{
	if (has_boot_commander()) {
		os_sprintf(response, "2");
	}
}

static void getvar_slot_successful(char *cmd_var, char *response)
{
	struct eio_boot eio;
	os_strtok_r(NULL, ":", &cmd_var);
	if (os_strlen(cmd_var) != 0) {
		if (get_boot_commander(&eio) &&
				((!os_strcmp(cmd_var, BOOT_SLOT_0_SUFFIX) && eio.slot[0].boot_ok) ||
				 (!os_strcmp(cmd_var, BOOT_SLOT_1_SUFFIX) && eio.slot[1].boot_ok))) {
			os_sprintf(response, "%s", "yes");
		} else {
			os_sprintf(response, "%s", "no");
		}
	}
}

static void getvar_slot_unbootable(char *cmd_var, char *response)
{
	struct eio_boot eio;
	os_strtok_r(NULL, ":", &cmd_var);
	if (os_strlen(cmd_var) != 0) {
		if (get_boot_commander(&eio) &&
				((!os_strcmp(cmd_var, BOOT_SLOT_0_SUFFIX) && eio.slot[0].boot_fail) ||
				 (!os_strcmp(cmd_var, BOOT_SLOT_1_SUFFIX) && eio.slot[1].boot_fail))) {
			os_sprintf(response, "%s", "yes");
		} else {
			os_sprintf(response, "%s", "no");
		}
	}
}

static void getvar_slot_retry_count(char *cmd_var, char *response)
{
	/* for now, we only+always try once. */
	if (has_boot_commander()) {
		os_sprintf(response, "1");
	}
}

/* Look-up table for handling "fastboot getvar <variable>" command
 *
 * Remarks on how to update this table:
 *  1. In order to deal with the variables that have extra parameter
 *     in <variable>, such as "fastboot getvar partition-type:<partition name>",
 *     the code only compares up to the string length of the variable 'name'
 *     listed in this table.
 *     The caveat of this approach is that if there is an actual variable
 *     that matches other ones with the same prefix, then the order in which
 *     the <variable> listed in this table matters.  In particular,
 *     'version' *MUST* always be after 'version-*' as illustrated below.
 *  2. Because we need to handle "fastboot getvar all" command to report
 *     all the variables listed in this table, the order in which the variable
 *     would show up as the response to this command is dependent on the order
 *     it is listed.  The exact order does not really matter, but to make the
 *     response looks nice and coherant, try to keep "partition-type" and
 *     "partition-size" as the last variables in the table.
 */
struct getvar_dispatch {
	char *name;
	void (*cb)(char *cmd_var, char *response);
};

static const struct getvar_dispatch getvar_dispatch[] = {
	{
		.name = "version-baseband",
		.cb = getvar_version_baseband,
	}, {
		.name = "version-bootloader",
		.cb = getvar_version_bootloader,
	}, {
		.name = "version-hardware",
		.cb = getvar_version_hardware,
	}, {
		.name = "version",
		.cb = getvar_version,
	}, {
		.name = "max-download-size",
		.cb = getvar_max_download_size,
	}, {
		.name = "serialno",
		.cb = getvar_serialno,
	}, {
		.name = "product",
		.cb = getvar_product,
	}, {
		.name = "secure",
		.cb = getvar_secure,
	}, {
		.name = "unlocked",
		.cb = getvar_unlocked,
	}, {
		.name = "off-mode-charge",
		.cb = getvar_off_mode_charge,
	}, {
		.name = "variant",
		.cb = getvar_variant,
	}, {
		.name = "partition-type:",
		.cb = getvar_partition_type,
	}, {
		.name = "partition-size:",
		.cb = getvar_partition_size,
	}, {
		.name = "has-slot:",
		.cb = getvar_has_slot,
	}, {
		.name = "current-slot",
		.cb = getvar_current_slot,
	}, {
		.name = "slot-suffixes",
		.cb = getvar_slot_suffixes,
	}, {
		.name = "slot-successful:",
		.cb = getvar_slot_successful,
	}, {
		.name = "slot-unbootable:",
		.cb = getvar_slot_unbootable,
	}, {
		.name = "slot-retry-count:",
		.cb = getvar_slot_retry_count,
	}, {
		.name = "slot-count",
		.cb = getvar_slot_count,
	},
};

static int getvar_all()
{
	int res;
	int len;
	unsigned int i, j, ptn_count;
	char response[RESPONSE_LEN];
	char cmd_var[RESPONSE_LEN];
	struct fastboot_ptentry *ptn;
	void (*getvar_cb)(char *cmd_var, char *response) = NULL;

	/* "fastboot getvar all" command must return all the supported
	 * variables.  To enable the host application to display all the data
	 * reported from device with a single fastboot command, the device has
	 * to use "INFO" as the prefix in the response packet.
	 * Only use "OKAY" after all the variables have been reported back to
	 * host. */


	/* Loop through the entire look-up table and call callbacks associated
	 * with the "getvar <variable>" */
	for (i = 0; i < ARRAY_SIZE(getvar_dispatch); i++) {

		/* BSU responsbility to call back to BOLT to let background
		 * task to get some time to run as BOLT code operates in
		 * polling model. */
		poll_task();

		getvar_cb = getvar_dispatch[i].cb;

		/* If <variable> is either "partition-type" or "partition-size",
		 * then we need to loop through the entire partition table to
		 * report the type and size of each partition. */
		if ( (os_strcmp(getvar_dispatch[i].name, "partition-type:") == 0) ||
		     (os_strcmp(getvar_dispatch[i].name, "partition-size:") == 0) ) {

			/* Find out the max number of non-empty partitions */
			ptn_count = fastboot_flash_get_ptn_count();

			for (j = 0; j < ptn_count; j++) {

				/* BSU responsbility to call back to BOLT to let background
				 * task to get some time to run as BOLT code operates in
				 * polling model. */
				poll_task();

				/*
				 * The response format must be:
				 *  "INFOpartition-[type|size]:<partiton name>: <value>"
				 */
				len = os_sprintf(response, "INFO");

				/* Get the pointer to each partition entry */
				ptn = fastboot_flash_get_ptn(j);

				/* safety check to ensure return ptn is not empty */
				if (0 == ptn) {
					/* unexpected error, just report back
					 * without partition name */
					os_printf("Unexpected error: curr-ptn=%d, max-ptn=\n",
							j, ptn_count);
					len += os_sprintf(response + len, "%s: ", getvar_dispatch[i].name);

				} else {
					/* Construct input to callback with format:
					 * partition-[type|size]:partition_name */
					os_sprintf(cmd_var, "%s%s",
						getvar_dispatch[i].name, ptn->name);
					len += os_sprintf(response + len, "%s: ", cmd_var);

					DLOG("var[%u,%u]: %s\n", i, j, cmd_var);

					getvar_cb(cmd_var, response + len);
				}
				res = fastboot_tx_write_str(response);

				/* if we fail to send the response for
				 * unexpected reason, then we have to abort
				 * the loop and report the error to caller */
				if (res < 0)
					goto exit;
			}

		} else if ( (os_strcmp(getvar_dispatch[i].name, "has-slot:") == 0) ) {

			len = os_sprintf(response, "INFO");
			len += os_sprintf(response + len, "%s: ", getvar_dispatch[i].name);
			if (has_boot_commander()) {
				int comma = 0;
				if (has_slot(BOOT_SLOT_BOOT_PREFIX)) {
					len += os_sprintf(response + len, "%s", BOOT_SLOT_BOOT_PREFIX);
					comma = 1;
				}
				if (has_slot(BOOT_SLOT_SYSTEM_PREFIX)) {
					len += os_sprintf(response + len, "%c%s", comma?',':' ', BOOT_SLOT_SYSTEM_PREFIX);
					comma = 1;
				}
				if (has_slot(BOOT_SLOT_VENDOR_PREFIX)) {
					len += os_sprintf(response + len, "%c%s", comma?',':' ', BOOT_SLOT_VENDOR_PREFIX);
					comma = 1;
				}
			}
			res = fastboot_tx_write_str(response);
			if (res < 0)
				goto exit;

		} else {
			/*
			 * The response format must be:
			 *  "INFO<variable>: <value>"
			 */
			len = os_sprintf(response, "INFO");
			len += os_sprintf(response + len, "%s: ", getvar_dispatch[i].name);

			DLOG("var[%u]: %s\n", i, getvar_dispatch[i].name);

			getvar_cb(getvar_dispatch[i].name, response + len);
			res = fastboot_tx_write_str(response);
			if (res < 0)
				goto exit;
		}
	}

	DLOG("Done\n");

	/* If code can go through all the for-loops without any error, then it
	 * means all the variables have been reported successfully.  The last
	 * thing to do is to send OKAY to allow host to send in another
	 * fastboot command. */

	res = fastboot_tx_write_str("OKAY");

exit:
	return res;
}

static int cb_getvar(struct fastboot_info *info)
{
	int res;
	int len;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	char response[RESPONSE_LEN];
	unsigned int i;
	void (*getvar_cb)(char *cmd_var, char *response) = NULL;

	/* Extract input variable name */
	os_strtok_r(cmd, ":", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing variable\n");
		res = fastboot_tx_write_str("FAILmissing var");
		return res;
	}

	/* Determine if variable name is "all".  If yes, it means we have to
	 * report all variables and we have to handle it separately from
	 * all other variables. Note: getvar_all() handles all responses and
	 * we can return right after calling it. */
	if (os_strcmp("all", cmd_var) == 0) {
		res = getvar_all();
		return res;
	}

	/* Prepare response packet, start with 'OKAY' tag*/
	len = os_sprintf(response, "OKAY");

	/* Look for the callback associated with the "variable" */
	for (i = 0; i < ARRAY_SIZE(getvar_dispatch); i++) {
		if (!strcmp_l1(getvar_dispatch[i].name, cmd_var)) {
			getvar_cb = getvar_dispatch[i].cb;
			break;
		}
	}

	if (!getvar_cb) {
		/* getvar "variable" is not listed in the look-up table.
		 * According to Fastboot protocol v0.4, undefined variable
		 * will be ignored and should be responded with OKAY */
		os_printf("Ignore unknown variable: %s\n", cmd_var);
	} else {
		/* Handle getvar "variable" with its specific callback
		 * in order to update 'response' buffer accordingly */
		getvar_cb(cmd_var, response + len);
	}

	res = fastboot_tx_write_str(response);
	return res;
}

static int cb_download(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	char *dummy;
	char response[RESPONSE_LEN];

	os_strtok_r(cmd, ":", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing variable\n");
		res = fastboot_tx_write_str("FAILmissing var");
		return res;
	}

	/* Store the expected size of image to be downloaded
	 * Note that the 2nd input argument cannot be NULL, but we don't
	 * expect non-numerical part in the input str, so the 2nd paramenter
	 * is a don't care */
	download_size = os_strtoul(cmd_var, &dummy, 16);

	/* Reset the bytes already downloaded to 0 to prepare image download
	 * handler */
	download_bytes = 0;
	download_payload = 0;
	download_overhead = 0;
	chunk_header = 0;

	os_printf("Starting download of %d bytes\n", download_size);

	if (0 == download_size) {
		os_sprintf(response, "FAILdata invalid size");
	} else if (download_size > FB_FLASH_STAGING_BUFFER_SIZE) {
		os_sprintf(response, "FAILdata too large");
	} else {
		/* Indicate to host that target platform is ready for
		 * the data phase */
		os_sprintf(response, "DATA%08x", download_size);

		/* Setup internal state and transfer size for next read to
		 * handle image download.  Try to use larger transfer size if
		 * it is allowed by the underlying transport device. */
		info->cmd_state = FB_STATE_DATA_DL;
		fb_info.cmd_buf_size = dl_image_rx_bytes_expected();
	}

	res = fastboot_tx_write_str(response);

	return res;
}

static int cb_flash(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	char response[RESPONSE_LEN];
	char *devname = info->flash_devname;
	char *bl_devname = info->boltimg_devname;
	char *pname = NULL;
	int lock_state;

	os_strtok_r(cmd, ":", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing partition\n");
		res = fastboot_tx_write_str("FAILmissing partition name");
		return res;
	}

	lock_state = oem_get_device_lock_state();
	if (LOCKED == lock_state) {
		os_printf("Error: device is locked - cannot flash partition\n");
		res = fastboot_tx_write_str("FAILdevice is locked - cannot flash partition");
		return res;
	}

	/* Host application sends "noaction" as the extra parameter
	 * in the flash command to indicate the start of resparsing a
	 * large sparse file into smaller sparse file that can fit into the
	 * target memory as indicated in the response of the
	 * "getvar:max-download-size" command from host.
	 * But we don't need to use this to setup state info in target as the
	 * host also resparses the file in such a way that target can deal
	 * with it like any sparse image. The magic is in the first
	 * and last chunk of resparsed file.  The host resparsed the file such
	 * that the first chunk of all resparsed files (except the very first
	 * one) is a DONT_CARE chunk type such that the target would skip over
	 * the region that was already flashed from the previous resparsed
	 * file. The last chunk of all resparsed files (except the very last
	 * one) is also a DONT_CARE chunk type such the target won't flash the
	 * region that is expected to be flashed in the subsequent resparsed
	 * file. All we have to do is to strip out the extra parameter before
	 * passing the partition name to the actual flash-write function. */
	pname = os_strtok_r(NULL, ":", &cmd_var);
	if (os_strlen(cmd_var) != 0) {
		DLOG("Found extra param: %s\n", cmd_var);
		if (os_strcmp(cmd_var, "noaction") == 0)
			DLOG("Resparsed file has been received.\n");
	}

	DLOG("Partition to be flashed: %s\n", pname);

	res = fastboot_flash_dev_write(devname, bl_devname, pname, response);

	if (res < 0) {
		/* Note fastboot_flash_dev_write always return BOLT_OK
		 * so report any other unexpected error code here */
		os_printf("Error after calling fastboot_flash_dev_write (%d)\n",
				res);
	}

	/* Note any error returned from previous call will be overwritten
	 * by this call to send the FAIL message to host. Therefore, this
	 * function will likely return BOLT_OK unless something is wrong
	 * in the transport layer. Returning BOLT_OK will at least the
	 * the target platform to stay in fastboot-mode if the failure
	 * is specific to a particular fastboot cmd */
	res = fastboot_tx_write_str(response);

	return res;
}

static int cb_erase(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	char response[RESPONSE_LEN];
	char *devname = info->flash_devname;

	os_strtok_r(cmd, ":", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing partition\n");
		res = fastboot_tx_write_str("FAILmissing partition name");
		return res;
	}

	res = fastboot_erase_ptn(devname, cmd_var);

	if (FB_ERR_PARTITION_NOT_EXIST == res) {
		os_sprintf(response, "FAILpartition does not exist");
	} else if (res < 0) {
		os_sprintf(response, "FAILerror in erasing partition");
	} else {
		os_sprintf(response, "OKAY");
	}
	res = fastboot_tx_write_str(response);

	return res;
}

static int cb_continue(struct fastboot_info *info)
{
	int res;
	char response[RESPONSE_LEN];

	os_sprintf(response, "OKAY");
	res = fastboot_tx_write_str(response);

	/* 1 sec delay to wait for response to be sent out.
	 * Note that background task is polled during the
	 * call in msleep() -- this enables the transport
	 * device driver code to be called */
	os_printf("Host request to continue auto-boot...\n");
	msleep (1000);

	/* Clean up before we execute the "android boot" command to auto-boot
	 * This ensures fastboot code won't hold on to any device handle that
	 * might be needed by "android boot" command */
	fastboot_uninit();

	bolt_docommands("android boot -rawfs");

	return res;
}

/*
 * Helper function to determine if device is locked or not
 * return:  1 = LOCKED
 *          0 = UNLOCKED
 */
static int oem_get_device_lock_state(void)
{
	int lock_state;

	/* FIXME - need to replace the BOLT environment variable with a secure
	 * flag to store the device state */

	lock_state = env_getval("ANDROID_LOCKED");

	if (lock_state < 0) {
		os_printf("ANDROID_LOCKED env var does not exist. Assume device is unlocked.\n");
		lock_state = UNLOCKED;
	} else if (lock_state > 0) {
		/* ensure 1 is always return to indicate device is locked */
		lock_state = LOCKED;
	}

	return lock_state;
}

/*
 * Helper function to set device state to either LOCKED or UNLOCKED
 * input:  1 = LOCKED
 *         0 = UNLOCKED
 */
static void oem_set_device_lock_state(int lock_state)
{
	/* FIXME - need to replace the BOLT environment variable with a secure
	 * flag to store the device state */

	if (LOCKED == lock_state)
		env_setenv("ANDROID_LOCKED", "1", ENV_FLG_NORMAL);
	else
		env_setenv("ANDROID_LOCKED", "0", ENV_FLG_NORMAL);

	env_save();
}

/* list of partitions to be erased when "fastboot oem" command is called */
static char * erase_ptn_list[] = {
	"userdata",
	"cache",
};

/* Helper function to erase user data to ensure they are not recoverable when
 * the device changes between LOCKED and UNLOCKED state */
static int oem_erase_data(struct fastboot_info *info)
{
	int res;
	char response[RESPONSE_LEN];
	char *partition;
	char *devname = info->flash_devname;
	unsigned int i;

	/* Erase all paritions with user data */
	for (i = 0; i < ARRAY_SIZE(erase_ptn_list); i++) {
		partition = erase_ptn_list[i];

		os_sprintf(response, "INFOerasing %s...", partition);
		res = fastboot_tx_write_str(response);
		if (res < 0)
			break;

		res = fastboot_erase_ptn(devname, partition);
		if (res < 0)
			break;

		os_sprintf(response, "INFOerasing %s done", partition);
		res = fastboot_tx_write_str(response);
		if (res < 0)
			break;
	}

	return res;
}

static int cb_oem(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	char response[RESPONSE_LEN];
	int lock_state;

	/* Extract input variable name */
	os_strtok_r(cmd, " ", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing variable\n");
		res = fastboot_tx_write_str("FAILmissing var");
		return res;
	}

	lock_state = oem_get_device_lock_state();

	if (os_strcmp("lock", cmd_var) == 0) {
		if (LOCKED == lock_state) {
			/* reset counter to ensure user sends the same cmd
			 * consecutively */
			info->dev_lock_state_change_req_cnt = 0;

			os_sprintf(response, "FAILAlready Locked");
		} else {
			/* note that we currently cannot erase data when
			 * device changes from unlocked to locked state
			 * because Android BSU cannot format the partition
			 * back to ext4 filesystem internally and device
			 * cannot be flashed by calling 'fastboot format' from
			 * host machine */
			oem_set_device_lock_state(LOCKED);
			os_sprintf(response, "OKAY");
		}
	} else if (os_strcmp("unlock", cmd_var) == 0) {
		if (UNLOCKED == lock_state) {
			/* reset counter to ensure user sends the same cmd
			 * consecutively */
			info->dev_lock_state_change_req_cnt = 0;

			os_sprintf(response, "FAILAlready Unlocked");
		} else {
			if (info->dev_lock_state_change_req_cnt > 0) {
				/* getting to here means user has confirmed
				 * and we can erase user data now */
				os_printf("erasing partition...\n");
				res = oem_erase_data(info);

				if (BOLT_OK == res) {
					/* only change state when we have
					 * successfully erased user data*/
					oem_set_device_lock_state(UNLOCKED);

					/* reset counter after state change
					 * to track next round of update */
					info->dev_lock_state_change_req_cnt = 0;

					os_sprintf(response, "OKAY");
				} else {
					os_sprintf(response, "FAILfail to erase user data before unlocking device");
				}
			} else {
				/* this is to satify the requirement to ask
				 * for user confirmation before erasing data */
				info->dev_lock_state_change_req_cnt++;
				os_sprintf(response, "FAILTo confirm, send the lock command one more time");
			}
		}
	} else {
		os_sprintf(response, "FAILunknown parameter %s", cmd_var);
	}

	res = fastboot_tx_write_str(response);

	return res;
}

static int cb_active_slot(struct fastboot_info *info)
{
	int res;
	char *cmd = (char *)(info->cmd_buf+(info->header?FB_HS_OVERHEAD:0));
	char *cmd_var = NULL;
	struct eio_boot eio;
	int slot = -1;

	os_strtok_r(cmd, ":", &cmd_var);

	if (os_strlen(cmd_var) == 0) {
		os_printf("Error: missing slot-suffix\n");
		res = fastboot_tx_write_str("FAILmissing slot-suffix");
		return res;
	}

	if (!get_boot_commander(&eio)) {
		os_printf("Error: invalid A|B target\n");
		res = fastboot_tx_write_str("FAILinvalid A|B target");
		return res;
	}

	if (!os_strcmp(cmd_var, BOOT_SLOT_0_SUFFIX) &&
		!os_strcmp(cmd_var, BOOT_SLOT_1_SUFFIX) &&
		!os_strcmp(cmd_var, "a") &&
		!os_strcmp(cmd_var, "b")) {
		os_printf("Error: invalid slot-suffix\n");
		res = fastboot_tx_write_str("FAILinvalid slot-suffix");
		return res;
	}

	if (!os_strcmp(cmd_var, BOOT_SLOT_0_SUFFIX) ||
		!os_strcmp(cmd_var, "a")) {
		slot = 0;
	} else if (!os_strcmp(cmd_var, BOOT_SLOT_1_SUFFIX) ||
		!os_strcmp(cmd_var, "b")) {
		slot = 1;
	}
	if (slot == -1) {
		res = fastboot_tx_write_str("FAIL");
		return res;
	}

	eio.current = slot;
	eio.slot[slot].boot_fail = 0;
	eio.slot[slot].boot_ok   = 0;
	eio.slot[slot].boot_try  = 0;
	if (!write_boot_commander(&eio)) {
		os_printf("Error: failed setting slot-suffix\n");
		res = fastboot_tx_write_str("FAILfailed setting slot-suffix");
		return res;
	}

	res = fastboot_tx_write_str("OKAY");
	return res;
}

static int cb_handshake(struct fastboot_info *info)
{
	int res;

	if (info->handshake) {
		info->header = 0;
	}
	/* we are in handsake mode, reply, then start accepting/sending headers
	 * in the command stream. */
   info->handshake = 1;
	res = fastboot_tx_write_str("FB01");
	info->header = 1;
	return res;
}

/* Look-up table to handle Fastboot commands */
struct cmd_dispatch {
	char *cmd;
	int (*cb)(struct fastboot_info *info);
};

static const struct cmd_dispatch cmd_dispatch[] = {
	{
		.cmd = "reboot",
		.cb = cb_reboot,
	}, {
		.cmd = "getvar:",
		.cb = cb_getvar,
	}, {
		.cmd = "download:",
		.cb = cb_download,
	}, {
		.cmd = "boot",
		.cb = cb_boot,
	}, {
		.cmd = "flash:",
		.cb = cb_flash,
	}, {
		.cmd = "erase:",
		.cb = cb_erase,
	}, {
		.cmd = "continue",
		.cb = cb_continue,
	}, {
		.cmd = "flashing",
		.cb = cb_oem, /* 'flashing' is the new name for 'oem', so map to same handler func */
	}, {
		.cmd = "oem",
		.cb = cb_oem,
	}, {
		.cmd = "set_active",
		.cb = cb_active_slot,
	}, {
		.cmd = FASTBOOT_HANDSHAKE,
		.cb = cb_handshake,
	},
};


/* Fastboot command processor to call the appropriate callback (cb_*) func */
int fastboot_rx_cmd_handler(void)
{
	char *cmdbuf = (char *)(fb_info.cmd_buf+(fb_info.header?FB_HS_OVERHEAD:0));
	int (*func_cb)(struct fastboot_info *info) = NULL;
	unsigned int i;
	int res = BOLT_OK;

	for (i = 0; i < ARRAY_SIZE(cmd_dispatch); i++) {
		if (!strcmp_l1(cmd_dispatch[i].cmd, cmdbuf)) {
			func_cb = cmd_dispatch[i].cb;
			break;
		}
	}

	if (!func_cb) {
		os_printf("Error: unknown command: %s\n", cmdbuf);
		res = fastboot_tx_write_str("FAILunknown command");
	} else {
		/* Default next transfer size to min transfer size, but the
		 * callback function might override it in the case when we
		 * need to handle image download in next transfer*/
		fb_info.cmd_buf_size = fb_info.min_transfer_size;
		res = func_cb(&fb_info);
	}

	if (res >= 0) {
		/* Queue the next transfer only when we have processed the
		 * data.  This is a limitation from the USB BDC driver */
		res = queue_next_transfer(fb_info.cmd_buf_size);
	}

	return res;
}

/* Helper function to figure out the size of next transfer request
 * If the transport device is USB, it is critical to ensure the transfer size
 * for the last chunk of data to be exact size of what we expect from host.
 * Otherwise, the driver will know when to terminate the transfer and it would
 * hung up the USB bus.*/
static unsigned int dl_image_rx_bytes_expected(void)
{
	int rx_remain = download_size - download_payload;
	if (rx_remain < 0)
		return 0;
	if (rx_remain > (int) fb_info.max_transfer_size)
		return fb_info.max_transfer_size;
	if (rx_remain < (int) fb_info.min_transfer_size)
		return fb_info.min_transfer_size;
	return rx_remain;
}

static void	fastboot_rx_dl_read(const unsigned char *buffer,
	unsigned int *buffer_consumed, unsigned int *buffer_size)
{
	unsigned int copy = 0;
	unsigned int chunk_outstanding = chunk_size - chunk_received;

	if (*buffer_size < chunk_outstanding) {
		copy = *buffer_size;
		*buffer_size = 0;
	} else {
		copy = chunk_outstanding;
		*buffer_size -= copy;
	}

	os_memcpy((void *)FB_FLASH_STAGING_BUFFER + download_payload,
		(uint8_t *)buffer+(*buffer_consumed), copy);

	download_payload += copy;
	chunk_received   += copy;
	*buffer_consumed += copy;

	if (chunk_received == chunk_size) {
#if defined(FB_DBG_PRINT_TCP_CHUNKS)
		os_printf("\nfilled-chunk:%lu bytes (%lu:%lu:%lu)",
			chunk_size, download_payload, download_size, download_bytes);
#endif
		chunk_header   = 0;
		chunk_size     = 0;
		chunk_received = 0;
	}
}

static void	fastboot_rx_dl_chunk(const unsigned char *buffer,
	unsigned int *buffer_consumed, unsigned int *buffer_size)
{
	unsigned int header = 0;
	unsigned int i;
	uint64_t size = 0, current;

	if (*buffer_size >= FB_HS_OVERHEAD-chunk_header) {
		header = FB_HS_OVERHEAD-chunk_header;
	} else {
		header = *buffer_size;
	}

	if (header+chunk_header > FB_HS_OVERHEAD) {
		header = FB_HS_OVERHEAD-chunk_header;
	}

	for (i = 0 ; i < header ; i++) {
		current = (uint8_t)buffer[*buffer_consumed+i];
		current <<= (56 - (i+chunk_header) * 8);
		size += current;
	}
	chunk_size   += size;
	chunk_header += header;

	*buffer_consumed += header;
	if (*buffer_size > header) {
		*buffer_size -= header;
	} else {
		*buffer_size = 0;
	}

#if defined(FB_DBG_PRINT_TCP_CHUNKS)
	os_printf("\ncurrent-chunk:%lu bytes (%lu:%s)",
		chunk_size, chunk_header, (chunk_header==FB_HS_OVERHEAD)?"full":"partial");
#endif

	if (chunk_header == FB_HS_OVERHEAD) {
		download_overhead += FB_HS_OVERHEAD;
		chunk_received = 0;
	}
}

/* Function to save download image to local staging buffer */
#define BYTES_PER_DOT	0x20000
static int fastboot_rx_dl_image_handler(void)
{
	int res = BOLT_OK;
	char response[RESPONSE_LEN];
	unsigned int transfer_size = download_size - download_bytes;
	const unsigned char *buffer = fb_info.cmd_buf;
	unsigned int buffer_size = fb_info.transport_read_bytes;
	unsigned int pre_dot_num, now_dot_num;
	unsigned int buffer_consumed = 0;

	if (transfer_size > buffer_size)
		transfer_size = buffer_size;

	pre_dot_num = download_bytes / BYTES_PER_DOT;

	if (fb_info.transport_mode != FB_TRANSPORT_TCP) {
		os_memcpy((void *)FB_FLASH_STAGING_BUFFER + download_payload,
				(uint8_t *)buffer, transfer_size);
		download_bytes   += transfer_size;
		download_payload += transfer_size;
	} else {
		download_bytes   += transfer_size;
		while (buffer_size) {
			/* read the chunk size if none set fully. */
			if (chunk_header < FB_HS_OVERHEAD) {
				fastboot_rx_dl_chunk(buffer, &buffer_consumed, &buffer_size);
			}
			/* read the data from the chunk. */
			fastboot_rx_dl_read(buffer, &buffer_consumed, &buffer_size);
		}
	}

	now_dot_num = download_bytes / BYTES_PER_DOT;
	if (pre_dot_num != now_dot_num) {
		os_printf(".");
		if (!(now_dot_num % 64))
			os_printf("\n");
	}
	/* Check if transfer is done */
	if (download_payload >= download_size) {
		download_size = 0;
		download_overhead = 0;
		fb_info.cmd_buf_size = fb_info.min_transfer_size;
		fb_info.cmd_state = FB_STATE_CMD_PROC;
		os_sprintf(response, "OKAY");
		res = fastboot_tx_write_str(response);

		os_printf("\ndownloading of %d bytes finished\n",
				download_bytes);
	} else {
		fb_info.cmd_buf_size = dl_image_rx_bytes_expected();
	}

	if (res >= 0) {
		/* Queue the next transfer only when data from previous
		 * transfer been processed*/
		res = queue_next_transfer(fb_info.cmd_buf_size);
	}

	return res;
}

/* Send out fastboot response to host */
static int fastboot_tx_write_str(const char *buffer)
{
	int res = BOLT_OK;
	char with_header[RESPONSE_HDR+RESPONSE_LEN];
	uint32_t size = os_strlen(buffer);
	int i;

	if (fb_info.header) {
		for (i = 0; i < RESPONSE_HDR ; i++) {
			with_header[i] = size >> (56 - i * 8);
		}
		DLOG("fb-out-hdr: %01x%01x%01x%01x%01x%01x%01x%01x\n",
				with_header[0], with_header[1], with_header[2], with_header[3],
				with_header[4], with_header[5], with_header[6], with_header[7]);
	}

	os_sprintf(with_header+(fb_info.header?RESPONSE_HDR:0), "%s", buffer);

	res = bolt_writeblk(fb_info.transport_dev_fd,
				0x0,
				(unsigned char *)with_header,
				/* when hand-shaking used: add response header, no null termination.
				 * otherwise: add the null termination. */
				size+(fb_info.header?RESPONSE_HDR:0)+(fb_info.handshake?0:1));
	DLOG("fb-out-cmd (%u), res %d: %s\n", size, res, buffer);

	/* Handle error here instead of passing up to caller by reporting
	 * the error. But let the fastboot-polling loop going */
	if (res < 0)
		os_printf("Error: failed to send response to host (%d)\n", res);
	return res;
}

/* Poll for incoming packet from transport layer and run the fastboot state
 * machine to either handle command or handle image download */
static int fastboot_poll(void)
{
	int res;
	int skip = 0;

	/* If no data from transport layer, just get out */
	if (bolt_inpstat(fb_info.transport_dev_fd) == 0)
		return BOLT_OK;

	/* Read the host command into receive buffer for further processing */
	res = bolt_readblk(fb_info.transport_dev_fd,
				0x0,
				fb_info.cmd_buf,
				fb_info.cmd_buf_size);

	/* Return error code if something is wrong in reading the transport
	 * layer buffer */
	if (res < 0) {
		os_printf("Error: failed to rx data from host (%d)\n", res);
		return res;
	}

	/* Determine whether target platform is in regular Command Processor
	 * or Data Downloading state */
	if (fb_info.cmd_state == FB_STATE_DATA_DL) {
		/* Save the actual bytes read from transport device driver as
		 * it might be smaller than what is requested in cmd_buf_size */
		fb_info.transport_read_bytes = res;
		res = fastboot_rx_dl_image_handler();
	} else {
		/* Stuff a NULL in to terminate command string properly.
		 * Fastboot command can be as long as 64 bytes */
		if (res < (int)fb_info.cmd_buf_size)
			fb_info.cmd_buf[res] = '\0';

		/* tcp mode handshake can be received anytime to confirm
		 * mode and sync, so look for it. */
		if (fb_info.transport_mode == FB_TRANSPORT_TCP) {
			if (os_strncmp((const char *)fb_info.cmd_buf,
								(const char *)FASTBOOT_HANDSHAKE,
								FASTBOOT_HANDSHAKE_SIZE) == 0) {
				fb_info.header = 0;
			}
		}

		if (fb_info.header)
			skip = FB_HS_OVERHEAD;

		/* If a command is received, process the command */
		if (skip) {
			int i;
			uint64_t current, size = 0;
			for (i = 0 ; i < skip ; i++) {
				current = (uint8_t)fb_info.cmd_buf[i];
				current <<= (56 - i * 8);
				size += current;
			}
			DLOG("fb-in-hdr: %llu\n", size);
		}
		DLOG("fb-in-cmd (%d): %s\n", res, fb_info.cmd_buf+skip);

		res = fastboot_rx_cmd_handler();
	}
	/* Let caller to handle any error return from command or image
	 * download handler functions */
	return res;
}

/*  *********************************************************************
    *  android_fastboot(cmd, argc, argv[])
    *
    *  Entry function to handle 'android fastboot' command
    *
    *  Input parameters:
    *	   cmd - input command
    *	   argc - number of input arguments
    *	   argv - variable number of input arguments
    *
    *  Return value:
    *	   0 if ok
    *	   else error code
    ********************************************************************* */

int android_fastboot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res;
	int devtype;
	int fb_transport_mode;
	char *fb_flashdev_name;
	const char *input_opts;
	int fb_exit_on_tty = 0;

	/* Before parsing command line parameters for "android fastboot"
	 * command, we blindly clear the boot reason register because we
	 * want the device to forgot the last reboot reason when users
	 * attempt to put device into Fastboot mode.
	 * Getting into Fastboot mode implies the last boot reason is
	 * "bootloader".
	 * This takes care of the case when users purposely break (Ctrl+C)
	 * from the BOLT boot sequence and prevent STARTUP environment
	 * variable/script to execute the autoboot command.
	 * (i.e. "android boot -rawfs") as it would have prevented the boot
	 * reason to be cleared for the next reboot. */
	boot_reason_reg_set(0);

	/*
	 * Parse command line parameters
	 */

	if (cmd_sw_value(cmd, "-transport", &input_opts))
	{
		/* We abstract the user away from the actual BOLT device name
		 * to be used as the transport device */
		if (os_strcmp(input_opts, "tcp") == 0)
			fb_transport_mode = FB_TRANSPORT_TCP;
		else if (os_strcmp(input_opts, "usb") == 0)
			fb_transport_mode = FB_TRANSPORT_USB;
		else
			return ui_showerror(BOLT_ERR_INV_PARAM,
				"Invalid 'transport' option. Use either 'tcp' or 'usb'\n");
	}
	else
	{
		fb_transport_mode = FB_TRANSPORT_USB;
	}

	if (cmd_sw_value(cmd, "-device", &input_opts))
	{
		/* User is required to provide the actual BOLT devie name for
		 * image to be flashed to. We don't want to make assumption
		 * about which BOLT device to be used in case the name is
		 * based on how the hardware platform is configured.*/
		fb_flashdev_name = (char *)input_opts;
		res = bolt_getdevinfo(fb_flashdev_name);
		if (res < 0)
			return ui_showerror(BOLT_ERR_DEVNOTFOUND, fb_flashdev_name);

		devtype = res & BOLT_DEV_MASK;

		if ((devtype != BOLT_DEV_FLASH) && (devtype != BOLT_DEV_DISK)) {
			return ui_showerror(BOLT_ERR_INV_PARAM,
				"Device '%s' is not a flash or disk device.\n",
					fb_flashdev_name);
		}
	}
	else
	{
		/* -device is a required parameter. We won't try to guess
		 * which device the image should be written to. */
		return ui_showerror(BOLT_ERR_INV_PARAM,
			"The 'device' option must be specified\n");
	}

	DLOG("transport=%d; device=%s\n", fb_transport_mode, fb_flashdev_name);

	/* Initialize fastboot data struct and transport device */
	res = fastboot_init(fb_transport_mode, fb_flashdev_name);
	if (res < 0)
		goto exit;

	/* Request transport device to queue first transfer before getting
	 * into the while-1 loop.
	 * Init the default cmd_buf_size to minimum as we know the first
	 * Fastboot command will always fit into the min transfer size */
	fb_info.cmd_buf_size = fb_info.min_transfer_size;
	res = queue_next_transfer(fb_info.cmd_buf_size);
	if (res < 0)
		goto exit;

	fb_exit_on_tty = env_getval("FB_EXIT_ON_TTY");
	if (fb_exit_on_tty > 0) {
		os_printf("fastboot may exit on console input...\n");
	} else {
		fb_exit_on_tty = 0;
	}

	/* Transport device opened OK. Wait for connection from remote host */
	while (1) {

		/* Note that it is BSU responsibility to call poll_task() in
		 * order to maintain BOLT counters and execute background tasks
		 * required by the device drivers like USB and network layer
		 * functions. Without calling this function, we cannot
		 * receive any data from the trasnport layer. */
		poll_task();

		/* See if there is a command to be processed */
		res = fastboot_poll();

		if (res < 0) {
			os_printf("Unexpected error (%d): stop fastboot\n", res);
			break;
		}

		if (fb_exit_on_tty) {
			res = console_status();
			if (res == 1) {
				os_printf("Console keyevent: stop fastboot\n");
				os_printf("***You can re-enter fastboot mode only if you are using -transport=tcp***\n\n");
				break;
			}
		}
	}

exit:
	/* Clean up before exiting */
	fastboot_uninit();

	return BOLT_OK;
}

/* variables present in the auto-generated gpt module. */
extern const uint32_t gpt_4_blimg_size;
extern const uint8_t gpt_4_blimg_data[];

void fastboot_process_canned_gpt(uint32_t boot_reason)
{
	int res = BOLT_OK;
	char *fb_flashdev_mode_str;
	struct fastboot_ptentry mem_ptable[MAX_PTN];
	unsigned int mem_pcount;
	int needs_flashing = 0;
	unsigned int i;
	struct fastboot_ptentry gpt_ptn;
	uint8_t *transfer_buffer = NULL;

	if (!gpt_4_blimg_size) {
		os_printf("canned-gpt: ignored - no canned gpt\n");
		return;
	}

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (fb_flashdev_mode_str == NULL) {
		os_printf("canned-gpt: abort - no FB_DEVICE_TYPE\n");
		return;
	}

	os_sprintf(fb_info.flash_devname, fb_flashdev_mode_str);
	res = fastboot_discover_gpt_tables(fb_info.flash_devname, 0);
	if (res) {
		os_printf("canned-gpt: failed - invalid gpt in %s\n", fb_info.flash_devname);
		return;
	}

	os_memset(mem_ptable, 0, sizeof(mem_ptable));
	mem_pcount = 0;
	fastboot_populate_canned_gpt(gpt_4_blimg_data, gpt_4_blimg_size, mem_ptable, &mem_pcount);
	if (!mem_pcount) {
		os_printf("canned-gpt: failed - not a valid gpt\n");
		return;
	}

	if (mem_pcount != pcount) {
		os_printf("canned-gpt: device count %d != memory count %d\n", pcount, mem_pcount);
	}
	for (i = 0 ; i < mem_pcount ; i++) {
		needs_flashing = os_memcmp(mem_ptable + i, ptable + i, sizeof(struct fastboot_ptentry));
		if (needs_flashing != 0) {
			if (0 == os_strcmp(ptable[i].name, "bsu")) {
				os_printf("gpt-update: refusing to update gpt changing bootloader partition\n");
				os_printf("gpt-update: for this action, a manual process is required.\n");
				needs_flashing = 0;
				break;
			}
			os_printf("canned-gpt: partition %d (%s:%s) differs\n", i, mem_ptable[i].name, ptable[i].name);
			needs_flashing = 1;
			break;
		}
	}

	if (needs_flashing) {
		os_sprintf(&gpt_ptn.name[0], "gpt");
		gpt_ptn.length = SIZEOF_GPT * BYTES_PER_LBA;
		gpt_ptn.start = 0;
		gpt_ptn.flags = 0;
		download_bytes = gpt_4_blimg_size;
		transfer_buffer = (uint8_t *)gpt_4_blimg_data;
		res = fastboot_flash_write_raw_image(fb_flashdev_mode_str, transfer_buffer, &gpt_ptn);
		os_printf("canned-gpt: requiring gpt update, applied: %d\n", res);
		/* now force an immediate restart in bootloader mode to apply the partition change and allow flashing. */
		if (res == BOLT_OK) {
			if ((boot_reason & BOOT_REASON_MASK) == 'b') {
				os_printf("canned-gpt: rebooting into bootloader...\n");
				boot_reason_reg_set('b');
				msleep (250);
				bolt_docommands("reboot");
			} else {
				os_printf("canned-gpt: a reboot is needed for full use of the new gpt.\n");
			}
		}
	}
}
