/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __AON_DEFS_H__
#define __AON_DEFS_H__

#include <common.h>
#include <bchp_aon_ctrl.h>
#include <compiler.h>

/* Magic number in upper 16-bits */
#define BRCMSTB_S3_MAGIC_MASK                   0xffff0000
#define BRCMSTB_S3_MAGIC_SHORT                  0x5AFE0000
#define REUSE_DRAM_SCRAMBLING_KEY_REQ	0x512E115E

enum {
	/* Restore random key for AES memory verification (off = fixed key) */
	S3_FLAG_LOAD_RANDKEY		= (1 << 0),

	/* Scratch buffer page table is present */
	S3_FLAG_SCRATCH_BUFFER_TABLE	= (1 << 1),

	/* Skip all memory verification */
	S3_FLAG_NO_MEM_VERIFY		= (1 << 2),

	/*
	 * Modification of this bit reserved for bootloader only.
	 * 1=PSCI started Linux, 0=Direct jump to Linux.
	 */
	S3_FLAG_PSCI_BOOT		= (1 << 3),

	/*
	 * Modification of this bit reserved for bootloader only.
	 * 1=64 bit boot, 0=32 bit boot.
	 */
	S3_FLAG_BOOTED64		= (1 << 4),
};

#define BRCMSTB_HASH_LEN		(128 / 8) /* 128-bit hash */

#define AON_REG_MAGIC_FLAGS			0
#define AON_REG_CONTROL_LOW			1
#define AON_REG_CONTROL_HIGH			2
#define AON_REG_S3_HASH				3 /* hash of S3 params */
#define AON_REG_CONTROL_HASH_LEN		7
/* AON_REG_BOOT_STATUS 8 is not used anymore from bolt v0.90 and on */
#define AON_REG_DRAM_SCRAMBLE_FLAGS		8
#define AON_REG_AVS_FLAGS			11
/* spare: a whole 4 registers! */
#define AON_REG_MEMSYS_STATE			16
#define MEMSYS_STATE_LEN			80

#define AON_REG_ADDR(idx)			((volatile uint32_t *) \
		REG_ADDR(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE + (idx) * 4))

#define AON_REG(idx)				*AON_REG_ADDR(idx)

#define MEMSYS_STATE_REG_ADDR(memc) \
		AON_REG_ADDR(AON_REG_MEMSYS_STATE + (memc) * MEMSYS_STATE_LEN)

#define BRCMSTB_S3_MAGIC		0x5AFEB007
#define BOOTLOADER_SCRATCH_SIZE		64
#define IMAGE_DESCRIPTORS_BUFSIZE	(2 * 1024)
#define S3_BOOTLOADER_RESERVED		(S3_FLAG_PSCI_BOOT | S3_FLAG_BOOTED64)

/*
 * Store up to 64 4KB page entries; this number is flexible, as long as
 * brcmstb_bootloader_scratch_table::num_entries is adjusted accordingly
 */
#define BRCMSTB_SCRATCH_BUF_SIZE	(256 * 1024)

struct brcmstb_bootloader_scratch_table {
	/* System page size, in KB; likely 4 (i.e., 4KB) */
	uint32_t page_size;
	/*
	 * Number of page entries in this table. Provided for flexibility, but
	 * should be BRCMSTB_SCRATCH_BUF_SIZE / PAGE_SIZE
	 */
	uint16_t num_entries;
	uint16_t reserved;
	struct {
		uint32_t upper;
		uint32_t lower;
	} entries[];
} __packed;

struct brcmstb_s3_params {
	/* scratch memory for bootloader */
	uint8_t scratch[BOOTLOADER_SCRATCH_SIZE];

	uint32_t magic; /* BRCMSTB_S3_MAGIC */
	uint64_t reentry; /* PA */

	/* descriptors */
	uint32_t hash[BRCMSTB_HASH_LEN / 4];

	/*
	 * If 0, then ignore this parameter (there is only one set of
	 *   descriptors)
	 *
	 * If non-0, then a second set of descriptors is stored at:
	 *
	 *   descriptors + desc_offset_2
	 *
	 * The MAC result of both descriptors is XOR'd and stored in @hash
	 */
	uint32_t desc_offset_2;

	/*
	 * (Physical) address of a brcmstb_bootloader_scratch_table, for
	 * providing a large DRAM buffer to the bootloader
	 */
	uint64_t buffer_table;

	uint32_t spare[70];

	uint8_t descriptors[IMAGE_DESCRIPTORS_BUFSIZE];
} __packed;

#endif /* __AON_DEFS_H__ */
