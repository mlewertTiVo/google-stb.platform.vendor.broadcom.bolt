/*
 * Copyright 2014-current Broadcom Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <byteorder.h>
#include "part_efi.h"
#include "gpt.h"
#include "fastboot.h"
#include "android_bsu.h"

/* Compiler option for debugging GPT code */
#undef GPT_DBG_PRINT_GPT_HDR  /* print GPT header read out from flash device */
#undef GPT_DBG_PRINT_PTN_ENTRIES /* print all partition entries before passing
					to crc32 check function */


/* Assume flash device (eMMC & USB) block size is same as GPT block size */
#define	BLOCKSIZE	(GPT_BLOCK_SIZE)

/* GPT table directly read out from flash device */
static efi_ptable gpt;

/*
 * Reuse the library function from BOLT instead of other
 * open-source implementation.
 */
static uint32_t crc32(const void *bfr, uint32_t bfr_sz)
{
	return ~lib_crc32(bfr, bfr_sz);
}

/* 
 * Check if the header is valid 
 * Returns 1 if valid, 0 if invalid. 
 */
static int gpt_is_valid_hdr(efi_gpt_header_t *hdr)
{
	u32 n;
	u32 origcrc;
	u32 hdrsize = le32_to_cpu(hdr->header_size);

	/* Check the GUID Partition Table signature */
	if (le64_to_cpu(hdr->signature) != GPT_HEADER_SIGNATURE) {
		os_printf("%s: GPT Header signature is wrong 0x%llx != 0x%llx\n", __func__, 
			le64_to_cpu(hdr->signature), GPT_HEADER_SIGNATURE);
		return 0;
	}

	/* Check the CRCs 
	 *  - need to save original CRC, and
	 *  - zero out crc field as per spec */
	origcrc = le32_to_cpu(hdr->header_crc32);
	os_memset(&hdr->header_crc32, 0, sizeof(hdr->header_crc32));
	n = crc32((void*) hdr, hdrsize);
	hdr->header_crc32 = cpu_to_le32(origcrc);
	if (n != origcrc) {
		os_printf("%s: GPT Header CRC is wrong 0x%x != 0x%x\n", __func__, n, origcrc);
		return 0;
	}

	return 1;
}

/* 
 * Check if the entries array is  valid 
 * Returns 1 if valid, 0 if invalid. 
 */
static int gpt_is_valid_entries(efi_gpt_header_t *hdr, u32 numEntries, efi_gpt_entry_t *entry)
{
	u32 n;

#ifdef GPT_DBG_PRINT_GPT_HDR
  	int idx;
  	unsigned char *tmp_buf = (unsigned char *)hdr;

	for (n=0; n<11; n++) {
		os_printf("n=%d: ", n);
		for (idx=0; idx<8; idx++){
			os_printf("0x%x ", *(tmp_buf+ (n*8+idx)));
		}
		os_printf("\n");
	}

	DLOG("signature = 0x%llx\n", hdr->signature);
	DLOG("revision = 0x%x\n", hdr->revision);
	DLOG("header_size = 0x%x\n", hdr->header_size);
	DLOG("header_crc32 = 0x%x\n", hdr->header_crc32);
	DLOG("reserved1 = 0x%x\n", hdr->reserved1);
	DLOG("my_lba = 0x%llx\n", hdr->my_lba);
	DLOG("alternate_lba = 0x%llx\n", hdr->alternate_lba);
	DLOG("first_usable_lba = 0x%llx\n", hdr->first_usable_lba);
	DLOG("last_usable_lba = 0x%llx\n", hdr->last_usable_lba);
	DLOG("num_partition_entries = 0x%x\n", hdr->num_partition_entries);
	DLOG("partition_entry_crc32 = 0x%x\n", le32_to_cpu(hdr->partition_entry_array_crc32));
#endif

#ifdef GPT_DBG_PRINT_PTN_ENTRIES
	unsigned char *data;
	unsigned int tmp_len;
	unsigned int i;
	data = (unsigned char *) entry;
	tmp_len = sizeof(*entry) * numEntries;

	DLOG("Print partition entries before calling crc32\n");
	for (i = 0; i < tmp_len; i++)
	{
		if ((i & 0xF) == 0)
			DLOG("\n");

		DLOG("0x%x ", data[i]);
	}
	DLOG("input len: %u\n", tmp_len);
#endif


	n = crc32((void*) entry, sizeof(*entry) * numEntries);
	if (n != le32_to_cpu(hdr->partition_entry_array_crc32)) {
		os_printf("%s: GPT Entries CRC is wrong  0x%x != 0x%x\n", __func__, 
			n, le32_to_cpu(hdr->partition_entry_array_crc32));
		return 0;
	}
 
	return 1;
}

/* 
 * Check if the primary or alternate GPT table is valid 
 * Returns 1 if valid, 0 if invalid. 
 * Returns the number of entries and the actual starting entry for use by the caller, 
 */
static int gpt_is_valid_table(efi_ptable *ptbl, int primary, int blk_cnt, u32 *numEntriesp, efi_gpt_entry_t **entrypp)
{
	efi_gpt_header_t *hdr;
	efi_gpt_entry_t *entry;
	u32 numEntries;
	u32 entriesToBackup;

	if (primary) {
		DLOG("%s: Checking Primary...\n", __func__);
		hdr = &ptbl->primary_ptable.header;
		numEntries = le32_to_cpu(hdr->num_partition_entries);
		entry = ptbl->primary_ptable.entry;
	} else {
		DLOG("%s: Checking Alternate...\n", __func__);
		hdr = &ptbl->secondary_ptable.header;
		numEntries = le32_to_cpu(hdr->num_partition_entries);

		/* cast because we start at header and back up */
		entry = (efi_gpt_entry_t *)&ptbl->secondary_ptable.header; 

		/* 
 	 	 * We need to back up to the correct starting entry based
	 	 * on the actual number of partition entries in the header. 
		 * If the number of entries is a multiple of 4, then we
		 * backup an integral number of sectors. But if the number
		 * of entries doesn't fit in an integral number of sectors,
		 * we have to back up so that the first entry is on a 
		 * sector boundary, and we ignore the padding between the
		 * last entry and the start of the header.
	 	 */
		entriesToBackup = EFI_PART2SECT(numEntries)/sizeof(efi_gpt_entry_t); 

		entry -= entriesToBackup; /* pointer arithmetic */
	}

	*numEntriesp = numEntries;
	*entrypp = entry;
	
	/* Print out a warning if the number of partition entries reported
	 * from GPT header is larger than 'entry' array allocated in the 'gpt'
	 * stored in RAM. We won't overwrite the data, but the CRC check for
	 * partition entries will likely fail because it will read data that
	 * are not for the 'gpt' in RAM. 
	 * Error out here to be on the safe side. */
	if (numEntries > EFI_ENTRIES) {
		os_printf("Warning: num of partition entries reported from GPT hdr (%d) > array size in ram (%d)\n",
					numEntries, EFI_ENTRIES);
		return 0; /* Indicate this is an invalid table */
	}

	return (gpt_is_valid_hdr(hdr) && gpt_is_valid_entries(hdr, numEntries, entry));
}

/* 
 * Get valid gpt table from flash device
 */
static int gpt_get_table(const char *devname, efi_ptable *ptbl, u32 *numEntriesp, efi_gpt_entry_t **entry)
{
	int blk_cnt;		/* number of blocks to read to get entire table */
	int ret;		/* return code */
	int isPrimary;		/* flag for primary or alternate identifier */
	int fd;

	fd = bolt_open((char *)devname);
	if (fd < 0) {
		os_printf("Error opening %s to read GPT: %d\n", devname, fd);
		goto err_exit;
	}

	blk_cnt = sizeof(gpt) / BLOCKSIZE;

	/* Check primary GPT table */
	isPrimary = 1;

	/* Always read multiple of BLOCKSIZE */
	ret = bolt_readblk(fd, 0, (unsigned char *)ptbl, BLOCKSIZE*blk_cnt);
	if (ret != BLOCKSIZE*blk_cnt) {
		os_printf("Read error: %d\n", ret);
		goto err_exit;
	}
	DLOG("%s: read %d bytes from '%s'\n", __func__, ret, devname);

	bolt_close(fd);

	if (gpt_is_valid_table(ptbl, isPrimary, blk_cnt, numEntriesp, entry)) {
		DLOG("%s: Primary GPT valid\n", __func__);
		return 0;
	}

	/* Need to return this specific error code so the caller function can
	 * decide whether to treat it as a real failure*/
	return FB_ERR_NO_VALID_GPT;

	/* This handles the case for any unexpected error in reading the flash
	 * device */
err_exit:
	if (fd >= 0)
		bolt_close(fd);
        return FB_ERR_FAIL_TO_ACCESS_FLASH_DEV;
}


/* 
 * Dynamically register partitions discovered in GPT table 
 */
static void register_partitions(u32 numEntries, efi_gpt_entry_t *entry,
	struct fastboot_ptentry *fb_entry, unsigned int *fb_total, int chatty)
{
	u32 i,j;
	u64 starting_lba;	/* starting sector */
	u64 ending_lba;		/* ending sector */
	struct fastboot_ptentry ptn;	/* partition structure to fill in and register */
	int len;
	u32 namelen;

	/* 
	 * For all entries, fill in the name/start/length/flags fields.
	 * Future enhancement might be to only fill in certain partitions
	 * that have one or more specific flags set. But currently we
	 * don't use the flags consistently so for now all partitions will
	 * be registered.
	 */
	for (i=0; i<numEntries; entry++, i++)
	{
		/* Skip empty partition entries by checking First LBA field
		 * and Last FBA field.  If both are zeros, then assumes it is
		 * an empty partition entry. */
		if ((0 == le64_to_cpu(entry->starting_lba)) &&
			(0 == le64_to_cpu(entry->ending_lba))) {
			continue;
		}

		/* clear stack struct before assigning new value to be safe */
		os_memset(&ptn, 0, sizeof(ptn));

		/* Set the name */
		/* Convert unicode name to simple char * name */
		namelen = EFI_GPT_NAMELEN_BYTES / sizeof (efi_char16_t);
		if (sizeof(ptn.name)-1 < namelen)
		{
			namelen = sizeof(ptn.name)-1;
		}
		for (j=0; j<namelen; j++)
		{
			if (entry->partition_name[j])
			{
				ptn.name[j] = entry->partition_name[j];
			}
		}
		ptn.name[sizeof(ptn.name)-1] = '\0'; /* force terminating null */

		/* Get the start/end sectors. Note the benefit of the __le64 type */
		starting_lba = le64_to_cpu(entry->starting_lba);
		ending_lba = le64_to_cpu(entry->ending_lba);

		/* Set the starting sector */
		ptn.start = starting_lba;

		/* Calculate the total length */	
		ptn.length = (ending_lba + 1 - starting_lba) * EFI_SECTORSIZE;

		ptn.flags = (le64_to_cpu(entry->attributes));

		len = 0;
		len += os_sprintf(ptn.uuid + len, "%02x%02x%02x%02x-",
			entry->unique_partition_guid.b[3], entry->unique_partition_guid.b[2],
			entry->unique_partition_guid.b[1], entry->unique_partition_guid.b[0]);
		len += os_sprintf(ptn.uuid + len, "%02x%02x-",
			entry->unique_partition_guid.b[5], entry->unique_partition_guid.b[4]);
		len += os_sprintf(ptn.uuid + len, "%02x%02x-",
			entry->unique_partition_guid.b[7], entry->unique_partition_guid.b[6]);
		len += os_sprintf(ptn.uuid + len, "%02x%02x-",
			entry->unique_partition_guid.b[8], entry->unique_partition_guid.b[9]);
		len += os_sprintf(ptn.uuid + len, "%02x%02x%02x%02x%02x%02x",
			entry->unique_partition_guid.b[10], entry->unique_partition_guid.b[11],
			entry->unique_partition_guid.b[12], entry->unique_partition_guid.b[13],
			entry->unique_partition_guid.b[14], entry->unique_partition_guid.b[15]);
		ptn.uuid[sizeof(ptn.uuid)-1] = '\0';

		if (chatty) {
			os_printf("Adding: %32s, offset 0x%8.8x, size 0x%16.16llx, flags 0x%16.16llx, uuid %48s\n",
					ptn.name, ptn.start, ptn.length, ptn.flags, ptn.uuid);
		}

		if (fb_entry == NULL) {
			fastboot_flash_add_ptn(&ptn);
		} else {
			if(*fb_total < MAX_PTN) {
				os_memcpy(fb_entry + *fb_total, &ptn, sizeof(ptn));
				*fb_total += 1;
			}
		}
	}
}


/* 
 * Function to find a valid GPT table and use it to populate the partition data structure
 * and register these with fastboot so the tables need not be hardcoded in the custom board 
 * configuration files. The rationale here is that customers may already have a gpt image
 * built with a layout file, and duplicating that layout file in u-boot is bad. Customers
 * probably won't run the "fastboot oem format" command either since they typically want to
 * avoid human intervention as part of the system build process.
 * Returns 0 on success, -1 on failure.
 */
int fastboot_discover_gpt_tables(char *flash_devname, int chatty)
{
	u32 numEntries;			/* Number of partition entries */
	efi_gpt_entry_t *entry;		/* partition entry */
	efi_ptable *ptbl = &gpt;	/* gpt table image in ram */
	int ret;			/* return code */

	/* Always clean the local copy of the partition table in the case
	 * a valid GPT cannot be found and yet we might still have to accept
	 * fastboot command from host for updating the device with valid GPT.*/
	fastboot_flash_clean_all_ptn();

	ret = gpt_get_table(flash_devname, ptbl, &numEntries, &entry);
	if (ret)
	{
		os_printf("%s: failed to get valid gpt table from device\n", __func__);
		return ret;
	}
	register_partitions(numEntries, entry, NULL, NULL, chatty);
	return 0;
}

int fastboot_populate_canned_gpt(const uint8_t *data, const uint32_t size,
	struct fastboot_ptentry *fb_entry, unsigned int *fb_total)
{
	int ret;
	u32 numEntries;
	int blk_cnt = SIZEOF_GPT;
	efi_gpt_entry_t *entry;
	efi_ptable *ptbl = (efi_ptable *)data;

	*fb_total = 0;
	if (!size) {
		return 0;
	}

	ret = gpt_is_valid_table(ptbl, 1, blk_cnt, &numEntries, &entry);
	if (!ret) {
		return 0;
	}
	register_partitions(numEntries, entry, fb_entry, fb_total, 0);
	return 0;
}

int fastboot_validate_gpt_image(unsigned char *gpt_buf_ptr)
{
	u32 numEntries;
	efi_gpt_entry_t *entry;
	efi_ptable *ptbl = (efi_ptable *) gpt_buf_ptr;
	int isPrimary = 1; /* Default to validate primary GPT */
	int blk_cnt = SIZEOF_GPT; /* Default to validate entire GPT */

	if (gpt_is_valid_table(ptbl, isPrimary, blk_cnt, &numEntries, &entry)) {
		DLOG("%s: Primary GPT valid\n", __func__);
		return 0;
	}

	/* invalid GPT is detected -- return fail*/
	return -1;
}
