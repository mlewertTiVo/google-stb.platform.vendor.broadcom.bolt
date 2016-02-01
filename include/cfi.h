#ifndef __CFI_H__
#define __CFI_H__

#include <stdint.h>
#include <byteorder.h>
#include <compiler.h>

struct cfi_erase_region {
	le16 num_sectors; /* sectors = X + 1 */
	le16 density; /* size = X * 256 */
} __packed;

struct cfi_ident {
	char qry[3];
	le16 mfr_id;
	le16 ext_addr;
	le16 alt_mfr_id;
	le16 alt_ext_addr;
	uint8_t vcc_min;
	uint8_t vcc_max;
	uint8_t vpp_min;
	uint8_t vpp_max;
	uint8_t t_word_write;
	uint8_t t_buf_write;
	uint8_t t_blk_erase;
	uint8_t t_chip_erase;
	uint8_t t_word_write_max;
	uint8_t t_buf_write_max;
	uint8_t t_blk_erase_max;
	uint8_t t_chip_erase_max;
	uint8_t dev_size; /* log2 */
	le16 interface_desc;
	le16 max_buf_write; /* log2 */
	uint8_t num_erase_regions;
	struct cfi_erase_region erase_regions[];
} __packed;

#define CFI_MANUFACTURER	0x00
#define CFI_DEVICE		0x01
#define CFI_OFFSET		0x10
#define MAX_CFIDATA		256	/* total size of CFI Data */

#define CFI_CMDSET_INTEL_ECS	0x0001	/* Intel extended */
#define CFI_CMDSET_AMD_STD	0x0002	/* AMD Standard */
#define CFI_CMDSET_INTEL_STD	0x0003	/* Intel Standard */
#define CFI_CMDSET_AMD_ECS	0x0004	/* AMD Extended */

#define CFI_DEVIF_X8		0x0000	/* 8-bit asynchronous */
#define CFI_DEVIF_X16		0x0001	/* 16-bit asynchronous */
#define CFI_DEVIF_X8X16		0x0002	/* 8 or 16-bit with BYTE# pin */
#define CFI_DEVIF_X32		0x0003	/* 32-bit asynchronous */

#endif /* __CFI_H__ */
