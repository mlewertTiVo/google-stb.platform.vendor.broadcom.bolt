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

/* 7445D0_1067MHz_32b_dev4Gx16_DDR3-2133N_le.mcb
*/
static const uint32_t memsys_config_template[SHMOO_ARRAY_ELEMENTS] __attribute__ ((section(".mcbdata"))) = {
	0x0132a6ef,	/* 0 */
	0x00000000,	/* 1 */
	0x00000000,	/* 2 */
	0x927d3ece,	/* 3 */
	0x00030301,	/* 4 */
	0x00001302,	/* 5 */
	0x003036b3,	/* 6 */
	0x0000b130,	/* 7 */
	0x0000f001,	/* 8 */
	0x00001100,	/* 9 */
	0x0337f980,	/* 10 */
	0x000001b0,	/* 11 */
	0x0000042b,	/* 12 */
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
	0x00010820,	/* 27 */
	0x00001530,	/* 28 */
	0x24070e0e,	/* 29 */
	0x08100a0e,	/* 30 */
	0x00d48118,	/* 31 */
	0x00000000,	/* 32 */
	0x00000000,	/* 33 */
	0x00000000,	/* 34 */
	0x00000000,	/* 35 */
	0x00001124,	/* 36 */
	0x0000000a,	/* 37 */
	0x00000228,	/* 38 */
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
	0x00000220,	/* 53 */
	0x55010012,	/* 54 */
	0x10000000,	/* 55 */
	0x00101027,	/* 56 */
	0x00048174,	/* 57 */
	0x000019b1,	/* 58 */
	0x00032470,	/* 59 */
	0x0fffffff,	/* 60 */
	0x003fffff,	/* 61 */
	0x00000004,	/* 62 */
	0x0000038e,	/* 63 */
	0x0000039b,	/* 64 */
	0x00000484,	/* 65 */
	0x00000493,	/* 66 */
	0x00000008,	/* 67 */
	0x00000000,	/* 68 */
	0x00000000,	/* 69 */
	0x00000001,	/* 70 */
	0x00001124,	/* 71 */
	0x0000000a,	/* 72 */
	0x00000228,	/* 73 */
	0x070e0e24,	/* 74 */
	0x00202608,	/* 75 */
	0x0716160b,	/* 76 */
	0x0e300e10,	/* 77 */
	0x001182ab,	/* 78 */
	0x0281c200,	/* 79 */
	0x001b1718,	/* 80 */
	0x04040404,	/* 81 */
	0x00000007,	/* 82 */
	0x00002941,	/* 83 */
	0x00000018,	/* 84 */
	0x00000018,	/* 85 */
	0x00000004,	/* 86 */
	0x00000011,	/* 87 */
	0x00000002,	/* 88 */
	0x00050000,	/* 89 */
	0x8f800056,	/* 90 */
	0x0337f980,	/* 91 */
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
	0x01fc1440,	/* 104 */
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
	0x00000210,	/* 129 */
	0x00000102,	/* 130 */
	0x0010c000,	/* 131 */
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
	0x00d28000,	/* 144 */
	0x00000000,	/* 145 */
	0x00000000,	/* 146 */
	0x00000000,	/* 147 */
	0x00000000,	/* 148 */
	0x00000000,	/* 149 */
	0x00000000,	/* 150 */
	0x00000000,	/* 151 */
};

/* 7445D0_1067MHz_32b_dev4Gx16_DDR3-2133N_le.mcb
*/
static const uint32_t mcb_1067mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0xffffffff,  0x262d248b,	/* -1 */
}; /* 0 difference */

/* 7445D0_1067MHz_32b_dev4Gx8_DDR3-2133N_le.mcb
*/
static const uint32_t mcb_1067mhz_32b_dev4Gx8[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x906c32de,	/* 3 */
	0x00000006,  0x002036b4,	/* 6 */
	0x0000000f,  0x00000000,	/* 15 */
	0x0000001c,  0x00001540,	/* 28 */
	0x0000001d,  0x24060e0e,	/* 29 */
	0x0000004a,  0x060e0e24,	/* 74 */
	0x0000004b,  0x00201b08,	/* 75 */
	0x0000004c,  0x0616160b,	/* 76 */
	0x00000053,  0x00002841,	/* 83 */
	0xffffffff,  0x220b0cab,	/* -1 */
}; /* 9 differences */

/* 7445D0_800MHz_32b_dev4Gx16_DDR3-1600K_le.mcb
*/
static const uint32_t mcb_800mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x8298043e,	/* 3 */
	0x00000006,  0x001036b3,	/* 6 */
	0x0000000c,  0x00000320,	/* 12 */
	0x0000001d,  0x1c060b0b,	/* 29 */
	0x0000001e,  0x060c080b,	/* 30 */
	0x0000001f,  0x00a460d0,	/* 31 */
	0x00000024,  0x00001d70,	/* 36 */
	0x00000026,  0x00000218,	/* 38 */
	0x00000038,  0x0010101d,	/* 56 */
	0x00000039,  0x00073b64,	/* 57 */
	0x0000003a,  0x00001341,	/* 58 */
	0x0000003b,  0x00025b20,	/* 59 */
	0x00000047,  0x00001d70,	/* 71 */
	0x00000049,  0x00000218,	/* 73 */
	0x0000004a,  0x060b0b1c,	/* 74 */
	0x0000004b,  0x00202006,	/* 75 */
	0x0000004c,  0x0612120a,	/* 76 */
	0x0000004d,  0x0b300b0c,	/* 77 */
	0x0000004e,  0x000d0200,	/* 78 */
	0x0000004f,  0x02014200,	/* 79 */
	0x00000050,  0x00151113,	/* 80 */
	0x0000005a,  0x8f800040,	/* 90 */
	0xffffffff,  0x0662af6b,	/* -1 */
}; /* 22 differences */

/* 7445D0_800MHz_32b_dev4Gx8_DDR3-1600K_le.mcb
*/
static const uint32_t mcb_800mhz_32b_dev4Gx8[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x8096fb4e,	/* 3 */
	0x00000006,  0x001036b4,	/* 6 */
	0x0000000c,  0x00000320,	/* 12 */
	0x0000000f,  0x00000000,	/* 15 */
	0x0000001c,  0x00001540,	/* 28 */
	0x0000001d,  0x1c050b0b,	/* 29 */
	0x0000001e,  0x060c080b,	/* 30 */
	0x0000001f,  0x00a460d0,	/* 31 */
	0x00000024,  0x00001d70,	/* 36 */
	0x00000026,  0x00000218,	/* 38 */
	0x00000038,  0x0010101d,	/* 56 */
	0x00000039,  0x00073b64,	/* 57 */
	0x0000003a,  0x00001341,	/* 58 */
	0x0000003b,  0x00025b20,	/* 59 */
	0x00000047,  0x00001d70,	/* 71 */
	0x00000049,  0x00000218,	/* 73 */
	0x0000004a,  0x050b0b1c,	/* 74 */
	0x0000004b,  0x00201806,	/* 75 */
	0x0000004c,  0x0512120a,	/* 76 */
	0x0000004d,  0x0b300b0c,	/* 77 */
	0x0000004e,  0x000d0200,	/* 78 */
	0x0000004f,  0x02014200,	/* 79 */
	0x00000050,  0x00151113,	/* 80 */
	0x00000053,  0x00002841,	/* 83 */
	0x0000005a,  0x8f800040,	/* 90 */
	0xffffffff,  0x02609d8b,	/* -1 */
}; /* 25 differences */

/* 7445D0_933MHz_32b_dev4Gx16_DDR3-1866M_le.mcb
*/
static const uint32_t mcb_933mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x8a1365e6,	/* 3 */
	0x00000006,  0x002036b3,	/* 6 */
	0x0000000c,  0x000003a5,	/* 12 */
	0x0000001d,  0x20060d0d,	/* 29 */
	0x0000001e,  0x070e090d,	/* 30 */
	0x0000001f,  0x00c470f4,	/* 31 */
	0x00000024,  0x00001f14,	/* 36 */
	0x00000026,  0x00000220,	/* 38 */
	0x00000038,  0x00101022,	/* 56 */
	0x00000039,  0x00059306,	/* 57 */
	0x0000003a,  0x00001681,	/* 58 */
	0x0000003b,  0x0002bf70,	/* 59 */
	0x00000047,  0x00001f14,	/* 71 */
	0x00000049,  0x00000220,	/* 73 */
	0x0000004a,  0x060d0d20,	/* 74 */
	0x0000004b,  0x00202107,	/* 75 */
	0x0000004c,  0x0614140b,	/* 76 */
	0x0000004d,  0x0d300d0e,	/* 77 */
	0x0000004e,  0x000f4256,	/* 78 */
	0x0000004f,  0x02418200,	/* 79 */
	0x00000050,  0x00191516,	/* 80 */
	0x0000005a,  0x8f80004b,	/* 90 */
	0xffffffff,  0x155972bb,	/* -1 */
}; /* 22 differences */

const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(".mcbtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ memsys_config_template,	MCB_MAGIC1,	SHMOO_ARRAY_SIZE,	SHMOO_ARRAY_ELEMENTS, -1, NULL },
	{ mcb_1067mhz_32b_dev4Gx16,	1067,	4096,	16, 32, NULL },
	{ mcb_1067mhz_32b_dev4Gx8,	1067,	4096,	8, 32, NULL },
	{ mcb_800mhz_32b_dev4Gx16,	800,	4096,	16, 32, NULL },
	{ mcb_800mhz_32b_dev4Gx8,	800,	4096,	8, 32, NULL },
	{ mcb_933mhz_32b_dev4Gx16,	933,	4096,	16, 32, NULL },
	{ NULL, 0, 0, 0, 0, NULL },
};


/* under 7445d0/... */

/* ddr4_1200MHz_32b_dev4Gx16_DDR4-2400U_le.mcb
*/
static const uint32_t ddr4_1200mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0xbbba9230,	/* 3 */
	0x00000006,  0x001036b5,	/* 6 */
	0x0000000c,  0x000004b0,	/* 12 */
	0x0000000d,  0x00000001,	/* 13 */
	0x0000001a,  0x00000004,	/* 26 */
	0x0000001b,  0x00010b20,	/* 27 */
	0x0000001c,  0x00000001,	/* 28 */
	0x0000001d,  0x27071212,	/* 29 */
	0x0000001e,  0x09121012,	/* 30 */
	0x0000001f,  0x01143138,	/* 31 */
	0x00000024,  0x00000940,	/* 36 */
	0x00000025,  0x00000109,	/* 37 */
	0x00000026,  0x00000028,	/* 38 */
	0x00000027,  0x00000220,	/* 39 */
	0x00000028,  0x00000800,	/* 40 */
	0x00000029,  0x00000440,	/* 41 */
	0x0000002a,  0x0000080f,	/* 42 */
	0x00000035,  0x000001d8,	/* 53 */
	0x00000037,  0x10080000,	/* 55 */
	0x00000038,  0x0010102c,	/* 56 */
	0x00000039,  0x0002d916,	/* 57 */
	0x0000003a,  0x00001cf1,	/* 58 */
	0x0000003b,  0x000388c0,	/* 59 */
	0x0000003c,  0x2f7fffff,	/* 60 */
	0x0000003d,  0x003f3fff,	/* 61 */
	0x00000047,  0x00000940,	/* 71 */
	0x00000048,  0x00000109,	/* 72 */
	0x00000049,  0x00000028,	/* 73 */
	0x0000004a,  0x07121227,	/* 74 */
	0x0000004b,  0x00202409,	/* 75 */
	0x0000004c,  0x081d1709,	/* 76 */
	0x0000004d,  0x12301218,	/* 77 */
	0x0000004e,  0x00138400,	/* 78 */
	0x0000004f,  0x03020300,	/* 79 */
	0x00000050,  0x00231f22,	/* 80 */
	0x00000053,  0x00006942,	/* 83 */
	0x00000058,  0x00000003,	/* 88 */
	0x0000005a,  0x8f800080,	/* 90 */
	0x0000005d,  0x00000220,	/* 93 */
	0x0000005e,  0x00000800,	/* 94 */
	0x0000005f,  0x00000440,	/* 95 */
	0x00000060,  0x0000080f,	/* 96 */
	0x00000076,  0x0000006e,	/* 118 */
	0x0000008c,  0x0000003c,	/* 140 */
	0x0000008e,  0x0000003c,	/* 142 */
	0x0000008f,  0xffffffff,	/* 143 */
	0xffffffff,  0x78a7cb4f,	/* -1 */
}; /* 46 differences */

static const char tag_ddr4_1200mhz_32b_dev4Gx16[] __attribute__ ((section(".mcbdata"))) = "ddr4";
const struct memsys_info __maybe_unused customshmoo_data[] __attribute__ ((section(".mcbsubtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ ddr4_1200mhz_32b_dev4Gx16,	1200,	4096,	16, 32,	tag_ddr4_1200mhz_32b_dev4Gx16 },
	{ NULL, 0, 0, 0, 0, NULL },
};
