/***************************************************************************
 *     Copyright (c) 2012-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <fsbl-common.h>

#define SHMOO_ARRAY_SIZE	0x00000260
#define SHMOO_ARRAY_ELEMENTS	0x00000098

/* 74371A0_667MHz_32b_dev4Gx16_DDR3-1333H_le.mcb
*/
static const uint32_t memsys_config_template[SHMOO_ARRAY_ELEMENTS] __attribute__ ((section(".mcbdata"))) = {
	0x0132a6ef,	/* 0 */
	0x00000000,	/* 1 */
	0x00000000,	/* 2 */
	0xaea01bce,	/* 3 */
	0x00030101,	/* 4 */
	0x00000802,	/* 5 */
	0x00102e67,	/* 6 */
	0x0000b120,	/* 7 */
	0x0000e101,	/* 8 */
	0x00001100,	/* 9 */
	0x0337f980,	/* 10 */
	0x00000144,	/* 11 */
	0x0000029b,	/* 12 */
	0x00000000,	/* 13 */
	0x00000001,	/* 14 */
	0x00000001,	/* 15 */
	0x00001000,	/* 16 */
	0x00000000,	/* 17 */
	0x2c002100,	/* 18 */
	0x00000000,	/* 19 */
	0x0000038e,	/* 20 */
	0x0000039b,	/* 21 */
	0x00000484,	/* 22 */
	0x00000493,	/* 23 */
	0x00000008,	/* 24 */
	0x00000243,	/* 25 */
	0x00000001,	/* 26 */
	0x00000820,	/* 27 */
	0x00001530,	/* 28 */
	0x18050909,	/* 29 */
	0x050a0709,	/* 30 */
	0x008450b0,	/* 31 */
	0x00000000,	/* 32 */
	0x00000000,	/* 33 */
	0x00000000,	/* 34 */
	0x00000000,	/* 35 */
	0x00001b50,	/* 36 */
	0x0000000a,	/* 37 */
	0x00000210,	/* 38 */
	0x00000000,	/* 39 */
	0x00000000,	/* 40 */
	0x00000000,	/* 41 */
	0x00000000,	/* 42 */
	0x00000000,	/* 43 */
	0x00000000,	/* 44 */
	0x00000000,	/* 45 */
	0x00000000,	/* 46 */
	0x00000000,	/* 47 */
	0x00000000,	/* 48 */
	0x00000000,	/* 49 */
	0x00000000,	/* 50 */
	0x00000000,	/* 51 */
	0x00000000,	/* 52 */
	0x00000000,	/* 53 */
	0x018d0012,	/* 54 */
	0x94000000,	/* 55 */
	0x00203093,	/* 56 */
	0x0005568c,	/* 57 */
	0x000121a1,	/* 58 */
	0x000bc960,	/* 59 */
	0x0fffffff,	/* 60 */
	0x003fffff,	/* 61 */
	0x00000000,	/* 62 */
	0x0000038e,	/* 63 */
	0x0000039b,	/* 64 */
	0x00000484,	/* 65 */
	0x00000493,	/* 66 */
	0x00000008,	/* 67 */
	0x00000000,	/* 68 */
	0x00000000,	/* 69 */
	0x00000001,	/* 70 */
	0x00001b50,	/* 71 */
	0x0000000a,	/* 72 */
	0x00000210,	/* 73 */
	0x05090918,	/* 74 */
	0x00201e05,	/* 75 */
	0x05101009,	/* 76 */
	0x0062d200,	/* 77 */
	0x0962c200,	/* 78 */
	0x0a724200,	/* 79 */
	0x00110d10,	/* 80 */
	0x04040404,	/* 81 */
	0x00000007,	/* 82 */
	0x00002941,	/* 83 */
	0x00000018,	/* 84 */
	0x00000018,	/* 85 */
	0x00000004,	/* 86 */
	0x0000000f,	/* 87 */
	0x00000000,	/* 88 */
	0x00038000,	/* 89 */
	0x8f800040,	/* 90 */
	0x00000000,	/* 91 */
	0x00000000,	/* 92 */
	0x00000000,	/* 93 */
	0x00000000,	/* 94 */
	0x00000000,	/* 95 */
	0x00000000,	/* 96 */
	0x00000000,	/* 97 */
	0x00000000,	/* 98 */
	0x00000000,	/* 99 */
	0x00000000,	/* 100 */
	0x00000000,	/* 101 */
	0x00000000,	/* 102 */
	0x00000000,	/* 103 */
	0x00000000,	/* 104 */
	0x00000000,	/* 105 */
	0x00000000,	/* 106 */
	0x00000000,	/* 107 */
	0x00000000,	/* 108 */
	0x00000000,	/* 109 */
	0x00000000,	/* 110 */
	0x00000000,	/* 111 */
	0x00000000,	/* 112 */
	0x00000000,	/* 113 */
	0x00000000,	/* 114 */
	0x00000000,	/* 115 */
	0x00000000,	/* 116 */
	0x00000000,	/* 117 */
	0x00000087,	/* 118 */
	0x01010101,	/* 119 */
	0x01010101,	/* 120 */
	0x01010101,	/* 121 */
	0x01010101,	/* 122 */
	0x01010101,	/* 123 */
	0x01010101,	/* 124 */
	0x01010101,	/* 125 */
	0x00000101,	/* 126 */
	0x00000000,	/* 127 */
	0x01000101,	/* 128 */
	0x01111120,	/* 129 */
	0x00000102,	/* 130 */
	0x00000000,	/* 131 */
	0x00000000,	/* 132 */
	0x00000000,	/* 133 */
	0x00000000,	/* 134 */
	0x00000000,	/* 135 */
	0x00000000,	/* 136 */
	0x00000000,	/* 137 */
	0x00000000,	/* 138 */
	0x00000000,	/* 139 */
	0x00000028,	/* 140 */
	0x00000028,	/* 141 */
	0x00000078,	/* 142 */
	0x00000078,	/* 143 */
	0x009de000,	/* 144 */
	0x00000000,	/* 145 */
	0x00000000,	/* 146 */
	0x00000000,	/* 147 */
	0x00000000,	/* 148 */
	0x00000000,	/* 149 */
	0x00000000,	/* 150 */
	0x00000000,	/* 151 */
};

/* 74371A0_667MHz_32b_dev4Gx16_DDR3-1333H_le.mcb
*/
static const uint32_t mcb_667mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0xffffffff,  0x5e72de8b,	/* -1 */
}; /* 0 difference */

/* 74371A0_800MHz_32b_dev4Gx16_DDR3-1600K_le.mcb
*/
static const uint32_t mcb_800mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0xb9f4ff1b,	/* 3 */
	0x00000006,  0x00202e67,	/* 6 */
	0x0000000c,  0x00000320,	/* 12 */
	0x0000001d,  0x1c060b0b,	/* 29 */
	0x0000001e,  0x060c080b,	/* 30 */
	0x0000001f,  0x00a460d0,	/* 31 */
	0x00000024,  0x00001d70,	/* 36 */
	0x00000026,  0x00000218,	/* 38 */
	0x00000038,  0x002030b0,	/* 56 */
	0x00000039,  0x000b645a,	/* 57 */
	0x0000003a,  0x00015b61,	/* 58 */
	0x0000003b,  0x000e2300,	/* 59 */
	0x00000047,  0x00001d70,	/* 71 */
	0x00000049,  0x00000218,	/* 73 */
	0x0000004a,  0x060b0b1c,	/* 74 */
	0x0000004b,  0x00202006,	/* 75 */
	0x0000004c,  0x0612120a,	/* 76 */
	0x0000004d,  0x00636200,	/* 77 */
	0x0000004e,  0x0b634200,	/* 78 */
	0x0000004f,  0x0c825200,	/* 79 */
	0x00000050,  0x00151113,	/* 80 */
	0xffffffff,  0x751ca525,	/* -1 */
}; /* 21 differences */

const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(".mcbtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ memsys_config_template,	MCB_MAGIC1,	SHMOO_ARRAY_SIZE,	SHMOO_ARRAY_ELEMENTS, -1, NULL },
	{ mcb_667mhz_32b_dev4Gx16,	667,	4096,	16, 32, NULL },
	{ mcb_800mhz_32b_dev4Gx16,	800,	4096,	16, 32, NULL },
	{ NULL, 0, 0, 0, 0, NULL },
};
