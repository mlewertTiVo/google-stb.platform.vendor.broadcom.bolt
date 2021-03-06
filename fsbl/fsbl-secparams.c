/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "fsbl.h"


#if defined(SECURE_BOOT) && defined(ZEUS_REVOCATION)
#define FIRST_IMAGE_ENABLE 0x80000000
#else
#define FIRST_IMAGE_ENABLE 0
#endif


__attribute__ ((section(".init.sec_params")))
uint32_t zeus_params[SEC_PARAM_LEN / sizeof(uint32_t)] = {
#if CFG_ZEUS4_2
#ifndef ZEUS_REVOCATION /* BOLT v1.07 or earlier */
	/* Zeus 4.2 BSECK-SHMOO integration document v1.12, sec 2.3, page 9 */
	[0x00] = 0x00008aa0,
	[0x01] = 0x00000000,
	[0x02] = 0x00000000,
	[0x03] = SSBL_SIZE,
	/* 0x01 -> 0x0b */
	[0x0c] = BFW_TEXT_OFFS,
	[0x0d] = 0x00000800,
	[0x0e] = 0x00000000,
	[0x0f] = AVS_TEXT_OFFS,
	[0x10] = 0x00003000,
	[0x11] = 0x00000B00, /* last 256 bytes is reservered */
	[0x12] = 0x00000800,
	[0x13] = 0x00000000,
	[0x14] = MEMSYS_TEXT_OFFS, /* 0x90 */
	[0x15] = MEMSYS_SIZE,
	[0x16] = 0x00000800,
	[0x17] = 0x00000000,
	[0x18] = 0x00000000, /*A0 */
	[0x19] = 0x00000000,
	[0x1a] = 0x00000000,
	[0x1b] = 0x00000000,
	[0x1c] = 0x00000000, /* B0 */
	[0x1d] = 0x00000000,
	[0x1e] = 0x00000000,
	[0x1f] = 0x00000000,
	[0x20] = 0x00000000, /* C0 */
	[0x21] = 0x00000000,
	[0x22] = 0x00000000,
	[0x23] = 0x00000000,
	[0x24] = SSBL_TEXT_OFFS, /* D0 */
	[0x25] = 0x00000000,
	[0x26] = 0x00000000,
	[0x27] = 0x00000000,
	[0x28] = 0x00000800, /* E0 */
	[0x29] = 0x00000000,
	[0x2a] = 0x00000000,
	[0x2b] = 0x00000000,
	[0x2c] = 0x00000000,
	[0x2d] = 0x00000000,
	[0x2e] = 0x00000000,
	[0x2f] = 0x00000000,
	[0x30] = 0x00000000,
	[0x31] = 0x00000000,
	[0x32] = 0x00000000,
	[0x33] = 0x00000000,
	[0x34] = 0x00000000,
	[0x35] = 0x00000000,
	[0x36] = 0x00000000,
	[0x37] = 0x00000000,
	[0x38] = 0x00000000,
	[0x39] = 0x00000000,
	[0x3a] = 0x00000000,
	[0x3b] = 0x00000000,
	[0x3c] = 0x00000000,
	[0x3d] = 0x00000000,
	[0x3e] = 0x00000000,
	[0x3f] = 0x00000000,
#else /* we're using ZEUS_REVOCATION */
	/* Zeus 4.2 BSECK-SHMOO integration document v1.12, sec 2.3, page 9 */
	[0x00] = 0x00008aa0,
	[0x01] = 0x00000000,
	[0x02] = 0x00000000,
	[0x03] = SSBL_SIZE,
	/* 0x01 -> 0x0a */
	[0x0b] = SECONDIMAGE_PART_OFFS,
	[0x0c] = BFW_TEXT_OFFS-SECONDIMAGE_PART_OFFS,
	[0x0d] = SECONDIMAGE_PART_SIZE>>10,
	[0x0e] = SECONDIMAGE_PART_OFFS,
	[0x0f] = AVS_TEXT_OFFS-SECONDIMAGE_PART_OFFS,
	[0x10] = 0x00003000,
	[0x11] = 0x00000B00, /* last 256 bytes is reservered */
	[0x12] = SECONDIMAGE_PART_SIZE>>10,
	[0x13] = SECONDIMAGE_PART_OFFS,
	[0x14] = MEMSYS_TEXT_OFFS-SECONDIMAGE_PART_OFFS, /* 0x90 */
	[0x15] = MEMSYS_SIZE,
	[0x16] = SECONDIMAGE_PART_SIZE>>10,
	[0x17] = FIRSTIMAGE_PART_OFFS,
	[0x18] = BFW1_TEXT_OFFS-FIRSTIMAGE_PART_OFFS, /*A0 */
	[0x19] = FIRST_IMAGE_ENABLE | (FIRSTIMAGE_PART_SIZE>>10),
	[0x1a] = FIRSTIMAGE_PART_OFFS,
	[0x1b] = AVS1_TEXT_OFFS-FIRSTIMAGE_PART_OFFS,
	[0x1c] = 0x00003000, /* B0 */
	[0x1d] = 0x00000B00,
	[0x1e] = FIRST_IMAGE_ENABLE | (FIRSTIMAGE_PART_SIZE>>10),
	[0x1f] = FIRSTIMAGE_PART_OFFS,
	[0x20] = MEMSYS1_TEXT_OFFS-FIRSTIMAGE_PART_OFFS, /* C0 */
	[0x21] = MEMSYS1_SIZE,
	[0x22] = FIRST_IMAGE_ENABLE | (FIRSTIMAGE_PART_SIZE>>10),
	[0x23] = 0x00000000,
	[0x24] = SSBL_TEXT_OFFS, /* D0 */
	[0x25] = 0x00000000,
	[0x26] = SSBL_RAM_ADDR,
	[0x27] = 0x00000000,
	[0x28] = BOLT_PART_SIZE>>10, /* E0 */
	[0x29] = FLASH_MAP_VER,
	[0x2a] = 0x00000000,
	[0x2b] = 0x00000000,
	[0x2c] = 0x00000000,
	[0x2d] = 0x00000000,
	[0x2e] = 0x00000000,
	[0x2f] = 0x00000000,
	[0x30] = 0x00000000,
	[0x31] = 0x00000000,
	[0x32] = 0x00000000,
	[0x33] = 0x00000000,
	[0x34] = 0x00000000,
	[0x35] = 0x00000000,
	[0x36] = 0x00000000,
	[0x37] = 0x00000000,
	[0x38] = 0x00000000,
	[0x39] = 0x00000000,
	[0x3a] = 0x00000000,
	[0x3b] = 0x00000000,
	[0x3c] = 0x00000000,
	[0x3d] = 0x00000000,
	[0x3e] = 0x00000000,
	[0x3f] = 0x00000000,
#endif /* ZEUS_REVOCATION */
#elif CFG_ZEUS4_1	/* 0x400 */
	[0x00] = 0x0000e780, /* 0xc80-0xf3ff */
	[0x01] = 0x00000000,
	[0x02] = 0x00000000,
	[0x03] = SSBL_SIZE,
	/* 0x004 -> 0x11f */
	/* 0x120 -> 0x1b7 MCB */
	[0x1dd] = 0x00000000,
	[0x1de] = BFW_TEXT_OFFS, /* 0xB78 */
	[0x1df] = 0x00000800,
	[0x1e0] = 0x00000000,
	[0x1e1] = AVS_TEXT_OFFS, /* 0xBB4 */
	[0x1e2] = 0x00000000,
	[0x1e3] = 0x00000000,
	[0x1e4] = 0x00000800,
	/* 0x1e5 -> 0x1f4 */
	[0x1f5] = 0x00000000,
	[0x1f6] = SSBL_TEXT_OFFS,
	[0x1f7] = 0x00000000,
	[0x1f8] = 0x00000000,
	[0x1f9] = 0x00000000,
	[0x1fa] = 0x00000800,
#else
	[0x00] = 0x0,
	[0x01] = 0x0,
	[0x02] = 0x0,
	[0x03] = 0x0,
	/* 0x04 -> 0x83 */
	[0x84] = 0x00008380,
	[0x85] = 0x0,
	[0x86] = 0x0,
	[0x87] = 0x00063a4c,
#endif
};

