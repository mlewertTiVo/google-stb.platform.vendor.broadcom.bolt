/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "fileops.h"
#include "iocb.h"
#include "devfuncs.h"

#include "loader.h"
#include "bolt.h"
#include "env_subr.h"

#include "parttbl.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define MIN_SECTOR_SZ 512

static uint32_t crc32(const void *bfr, uint32_t bfr_sz)
{
	return ~lib_crc32(bfr, bfr_sz);
}

int gpt_is_protective_mbr(const uint8_t *bfr, uint32_t bfr_sz)
{
	mbr_t *mbr;

	if (bfr_sz < MIN_SECTOR_SZ)
		return 0;

	mbr = (mbr_t *)bfr;

	if ((mbr->part_tbl[0].dword[1] & 0xff) == 0xee)
		return 1;

	return 0;
}

int gpt_alloc_hdr(const uint8_t *bfr, uint32_t bfr_sz, gpt_hdr_t **out_gpt_hdr)
{
	gpt_hdr_t *gpt_hdr;
	uint32_t calc_crc32;
	uint32_t saved_crc32;

	if (bfr_sz < sizeof(gpt_hdr_t))
		return -1;

	gpt_hdr = KMALLOC(sizeof(gpt_hdr_t), 0);
	if (gpt_hdr == NULL)
		return -1;

	if (memcmp(GPT_HDR_SIG, bfr, sizeof(GPT_HDR_SIG)) != 0) {
		KFREE(gpt_hdr);
		return -1;
	}

	memcpy(gpt_hdr, bfr, sizeof(gpt_hdr_t));

	saved_crc32 = gpt_hdr->hdr_crc32;
	gpt_hdr->hdr_crc32 = 0;
	calc_crc32 = crc32(gpt_hdr, gpt_hdr->hdr_size);
	gpt_hdr->hdr_crc32 = saved_crc32;
	if (calc_crc32 != gpt_hdr->hdr_crc32) {
		KFREE(gpt_hdr);
		return -1;
	}

	*out_gpt_hdr = gpt_hdr;

	return 0;
}

void gpt_free_hdr(gpt_hdr_t *gpt_hdr)
{
	KFREE(gpt_hdr);
}

/**
 * Get the base LBA of the partition entry table, and the byte offset
 * of the i'th partition table entry.
 */
int gpt_get_part_entry_loc(const gpt_hdr_t *gpt_hdr, unsigned int i,
				uint64_t *lba,
	uint32_t *offset)
{
	uint32_t abs_byte_offset;
	uint64_t _lba;

	if (i >= gpt_hdr->part_entry_cnt)
		return -1;

	_lba = gpt_hdr->part_entry_lba;
	abs_byte_offset = i * gpt_hdr->part_entry_size;

	if (lba)
		*lba = _lba;

	if (offset)
		*offset = abs_byte_offset;

	return 0;
}

#define GUIDCMP_DATA(x) \
	if (a->data##x != b->data##x)\
		return -1
static int guidcmp(const guid_t *a, const guid_t *b)
{
	GUIDCMP_DATA(0);
	GUIDCMP_DATA(1);
	GUIDCMP_DATA(2);
	GUIDCMP_DATA(3);
	return 0;
}

int gpt_is_part_type(const guid_part_entry_t *entry, enum part_type type)
{
	if (type >= PART_TYPE_LAST)
		return 0;

	if (guidcmp(&entry->part_type_guid, &PART_TYPE_GUID[type]) == 0)
		return 1;
	else
		return 0;
}
