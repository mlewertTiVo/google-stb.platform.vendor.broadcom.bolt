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

#ifndef __PARTTBL_H__
#define __PARTTBL_H__

#include <compiler.h>
#include "byteorder.h"

typedef struct {
	uint32_t dword[4];
} legacy_part_entry_t;

typedef struct __packed {
	uint8_t bootstrap_code[446];
	legacy_part_entry_t part_tbl[4];
	uint16_t boot_sig;
} mbr_t;

typedef struct {
	uint32_t data0;
	uint16_t data1;
	uint16_t data2;
	be64     data3;
} guid_t;

static const uint8_t GPT_HDR_SIG[] = {
	0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54
};

typedef struct {
	uint8_t  signature[8]; /* 0-7 */
	uint32_t revision;     /* 8-11 */
	uint32_t hdr_size;     /* 12-15 */
	uint32_t hdr_crc32;    /* 16-19 */
	uint32_t rsvd_20;      /* 20-23 */
	uint64_t curr_lba;     /* 24-31 */
	uint64_t bkup_lba;     /* 32-39 */
	uint64_t first_usable_lba; /* 40-47 */
	uint64_t last_usable_lba;  /* 48-55 */
	guid_t   disk_guid;        /* 56-71 */
	uint64_t part_entry_lba;   /* 72-79 */
	uint32_t part_entry_cnt;   /* 80-83 */
	uint32_t part_entry_size;  /* 84-87 */
	uint32_t part_entry_crc32; /* 88-91 */
	uint8_t  rsvd_92[420];     /* up to 512 bytes, but can be larger for
				      alternative sector sizes */
} gpt_hdr_t;

#define ATTRIB_FLAG_SYS_PART         0
#define ATTRIB_FLAG_LEGACY           2
#define ATTRIB_FLAG_READ_ONLY        60
#define ATTRIB_FLAG_HIDDEN           62
#define ATTRIB_FLAG_DO_NOT_AUTOMOUNT 63

#define GUID_PART_NAME_SIZE 36

typedef struct {
	guid_t   part_type_guid;
	guid_t   unique_part_guid;
	uint64_t first_lba;
	uint64_t last_lba;
	uint64_t attrib_flags;
	uint16_t part_name[GUID_PART_NAME_SIZE];
} guid_part_entry_t;

enum part_type {
	PART_TYPE_INVALID = 0,
	PART_TYPE_MBR,
	PART_TYPE_WIN,		/* gdisk generated this guy */
	PART_TYPE_LINUX,
	PART_TYPE_LAST,
};

static const guid_t PART_TYPE_GUID[] = {
	[PART_TYPE_INVALID] =
		{ 0,          0,      0,      0                  },
	[PART_TYPE_MBR] =
		{ 0x024dee41, 0xe733, 0x11d3, swap64(0x9d690008c781f39fULL) },
	[PART_TYPE_WIN] =
		{ 0xebd0a0a2, 0xb9e5, 0x4433, swap64(0x87c068b6b72699c7ULL) },
	[PART_TYPE_LINUX] =
		{ 0x0fc63daf, 0x8483, 0x4772, swap64(0x8e793d69d8477de4ULL) },
	[PART_TYPE_LAST] =
		{ 0,          0,      0,      0                  },
};

int gpt_is_protective_mbr(const uint8_t *bfr, uint32_t bfr_sz);
int gpt_alloc_hdr(const uint8_t *bfr, uint32_t bfr_sz, gpt_hdr_t **out_gpt_hdr);
void gpt_free_hdr(gpt_hdr_t *gpt_hdr);
int gpt_get_part_entry_loc(const gpt_hdr_t *gpt_hdr, unsigned int i,
		uint64_t *lba, uint32_t *offset);
int gpt_is_part_type(const guid_part_entry_t *entry, enum part_type type);

#define CFG_GPT 1

#endif /* __PARTTBL_H__ */
