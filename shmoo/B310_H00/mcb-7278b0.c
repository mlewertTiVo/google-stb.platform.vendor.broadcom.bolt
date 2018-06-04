/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <fsbl-common.h>

#define SHMOO_ARRAY_SIZE	0x00000260
#define SHMOO_ARRAY_ELEMENTS	0x00000098

/* 7278B_1600MHz_32b_dev16Gx1_LPDDR4-3200_le.mcb
*/
static const uint32_t memsys_config_template[SHMOO_ARRAY_ELEMENTS] __attribute__ ((section(".mcbdata"))) = {
	0x0132a6ef,	/* 0 */
	0x00000000,	/* 1 */
	0x00000000,	/* 2 */
	0x433b13f0,	/* 3 */
	0x00040002,	/* 4 */
	0x00002a02,	/* 5 */
	0x0010499d,	/* 6 */
	0x007278b0,	/* 7 */
	0x0000b310,	/* 8 */
	0x00004800,	/* 9 */
	0x0000c000,	/* 10 */
	0x000000d8,	/* 11 */
	0x0337f980,	/* 12 */
	0x000001b0,	/* 13 */
	0x00000000,	/* 14 */
	0x00000640,	/* 15 */
	0x00184487,	/* 16 */
	0x00000008,	/* 17 */
	0x00000004,	/* 18 */
	0x00000002,	/* 19 */
	0x00004000,	/* 20 */
	0x00000000,	/* 21 */
	0x00400000,	/* 22 */
	0x32323232,	/* 23 */
	0x00001a1c,	/* 24 */
	0x00001a1c,	/* 25 */
	0x0000000d,	/* 26 */
	0x03ff03ff,	/* 27 */
	0x00000000,	/* 28 */
	0x002b6d5c,	/* 29 */
	0x100a5500,	/* 30 */
	0x0000000a,	/* 31 */
	0x00801500,	/* 32 */
	0x00003c5a,	/* 33 */
	0x002b6d5c,	/* 34 */
	0x500a5500,	/* 35 */
	0x0000000a,	/* 36 */
	0x00801500,	/* 37 */
	0x00003c5a,	/* 38 */
	0x00000218,	/* 39 */
	0x55010012,	/* 40 */
	0x10080000,	/* 41 */
	0x0010103a,	/* 42 */
	0x000e76c8,	/* 43 */
	0x00002691,	/* 44 */
	0x0004b650,	/* 45 */
	0x03200320,	/* 46 */
	0x03200320,	/* 47 */
	0x03200320,	/* 48 */
	0x03200320,	/* 49 */
	0x03200320,	/* 50 */
	0x03200320,	/* 51 */
	0x03200320,	/* 52 */
	0x03200320,	/* 53 */
	0x03200320,	/* 54 */
	0x03200320,	/* 55 */
	0x0000034c,	/* 56 */
	0x00000000,	/* 57 */
	0x00000000,	/* 58 */
	0x00000000,	/* 59 */
	0x00000000,	/* 60 */
	0x00000000,	/* 61 */
	0x00070a43,	/* 62 */
	0x800181c0,	/* 63 */
	0x00000000,	/* 64 */
	0x00001868,	/* 65 */
	0x800081c0,	/* 66 */
	0x04a817c8,	/* 67 */
	0x10101d60,	/* 68 */
	0x08085e29,	/* 69 */
	0x34341840,	/* 70 */
	0x20191d1a,	/* 71 */
	0x34181d10,	/* 72 */
	0x0000000f,	/* 73 */
	0x000004c0,	/* 74 */
	0x000fffe1,	/* 75 */
	0x00000000,	/* 76 */
	0x00000000,	/* 77 */
	0x00000000,	/* 78 */
	0x00000000,	/* 79 */
	0x00000000,	/* 80 */
	0x0000aa65,	/* 81 */
	0x0000070a,	/* 82 */
	0x101d1d44,	/* 83 */
	0x0020400c,	/* 84 */
	0x10343418,	/* 85 */
	0x00302217,	/* 86 */
	0x001c0640,	/* 87 */
	0x0400f1cc,	/* 88 */
	0x0e0c432c,	/* 89 */
	0x08205e29,	/* 90 */
	0x34181d10,	/* 91 */
	0x00000011,	/* 92 */
	0x00000004,	/* 93 */
	0x00695000,	/* 94 */
	0x00000005,	/* 95 */
	0x03030303,	/* 96 */
	0x00000303,	/* 97 */
	0x800fe030,	/* 98 */
	0x01499700,	/* 99 */
	0x401c181b,	/* 100 */
	0x00050000,	/* 101 */
	0x00000000,	/* 102 */
	0x00000000,	/* 103 */
	0x04000f81,	/* 104 */
	0x800fe030,	/* 105 */
	0x04001181,	/* 106 */
	0x00000360,	/* 107 */
	0x0000005a,	/* 108 */
	0x01010101,	/* 109 */
	0x01010101,	/* 110 */
	0x00000001,	/* 111 */
	0x01010101,	/* 112 */
	0x01010101,	/* 113 */
	0x00000001,	/* 114 */
	0x00000001,	/* 115 */
	0x01000101,	/* 116 */
	0x02590404,	/* 117 */
	0x0100016f,	/* 118 */
	0x00006e6e,	/* 119 */
	0x3018000c,	/* 120 */
	0x0000003c,	/* 121 */
	0x00000028,	/* 122 */
	0x0000003c,	/* 123 */
	0xffffffff,	/* 124 */
	0x000016f3,	/* 125 */
	0x02140a00,	/* 126 */
	0x04372000,	/* 127 */
	0x02160000,	/* 128 */
	0x00000030,	/* 129 */
	0x1249a81c,	/* 130 */
	0x844ec460,	/* 131 */
	0x7020f31d,	/* 132 */
	0x63197bc0,	/* 133 */
	0x1419018c,	/* 134 */
	0x02df0430,	/* 135 */
	0x00001868,	/* 136 */
	0x0000000a,	/* 137 */
	0x00000003,	/* 138 */
	0x0000005a,	/* 139 */
	0x003c003c,	/* 140 */
	0x00000000,	/* 141 */
	0x00000000,	/* 142 */
	0x00000000,	/* 143 */
	0x00000000,	/* 144 */
	0x00000000,	/* 145 */
	0x00000000,	/* 146 */
	0x00000000,	/* 147 */
	0x00000000,	/* 148 */
	0x00000000,	/* 149 */
	0x00000000,	/* 150 */
	0x00000000,	/* 151 */
};

/* 7278B_1600MHz_32b_dev16Gx1_LPDDR4-3200_le.mcb
*/
static const uint32_t mcb_1600mhz_32b_dev16Gx1[] __attribute__ ((section(".mcbdata"))) = {
	0xffffffff,  0x87a8cecf,	/* -1 */
}; /* 0 difference */

/* 7278B_1600MHz_32b_dev8Gx1_LPDDR4-3200_le.mcb
*/
static const uint32_t mcb_1600mhz_32b_dev8Gx1[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x1b23dc8f,	/* 3 */
	0x00000004,  0x00030002,	/* 4 */
	0x00000005,  0x00002702,	/* 5 */
	0x00000006,  0x00404678,	/* 6 */
	0x00000014,  0x00002000,	/* 20 */
	0x0000003e,  0x0006fa43,	/* 62 */
	0x0000003f,  0x80018120,	/* 63 */
	0x00000042,  0x80008120,	/* 66 */
	0x00000051,  0x0000aa55,	/* 81 */
	0x00000052,  0x000006fa,	/* 82 */
	0x00000057,  0x00120640,	/* 87 */
	0x00000058,  0x0400f12c,	/* 88 */
	0x00000059,  0x0e0c442c,	/* 89 */
	0x00000084,  0x4820f31d,	/* 132 */
	0x0000008c,  0x00000000,	/* 140 */
	0xffffffff,  0x377a600d,	/* -1 */
}; /* 15 differences */

/* 7278B_1600MHz_32b_dev8Gx2_LPDDR4-3200_le.mcb
*/
static const uint32_t mcb_1600mhz_32b_dev8Gx2[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x5d258c91,	/* 3 */
	0x00000004,  0x00030002,	/* 4 */
	0x00000005,  0x00002702,	/* 5 */
	0x00000006,  0x0040467a,	/* 6 */
	0x00000014,  0x00002000,	/* 20 */
	0x00000015,  0x00002000,	/* 21 */
	0x00000016,  0x00404000,	/* 22 */
	0x0000003e,  0x0206fa43,	/* 62 */
	0x0000003f,  0x80018120,	/* 63 */
	0x00000042,  0xa0008120,	/* 66 */
	0x00000051,  0x0001aa55,	/* 81 */
	0x00000052,  0x000056fa,	/* 82 */
	0x00000057,  0x00120640,	/* 87 */
	0x00000058,  0x0400f12c,	/* 88 */
	0x00000059,  0x0e0c442c,	/* 89 */
	0x00000062,  0xa00fe030,	/* 98 */
	0x00000084,  0x4820f31d,	/* 132 */
	0x0000008c,  0x00000000,	/* 140 */
	0xffffffff,  0xbb7dc011,	/* -1 */
}; /* 18 differences */

/* 7278B_1867MHz_32b_dev16Gx1_LPDDR4-3733_le.mcb
*/
static const uint32_t mcb_1867mhz_32b_dev16Gx1[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0xa8da0406,	/* 3 */
	0x00000006,  0x0020499d,	/* 6 */
	0x0000000d,  0x000001e6,	/* 13 */
	0x0000000f,  0x0000074b,	/* 15 */
	0x00000010,  0x001c513f,	/* 16 */
	0x00000018,  0x00001e20,	/* 24 */
	0x00000019,  0x00001e20,	/* 25 */
	0x0000001d,  0x002b766c,	/* 29 */
	0x00000022,  0x002b766c,	/* 34 */
	0x0000002a,  0x00101044,	/* 42 */
	0x0000002b,  0x000bbcd8,	/* 43 */
	0x0000002c,  0x00002d01,	/* 44 */
	0x0000002d,  0x00057fa0,	/* 45 */
	0x0000003f,  0x8001820c,	/* 63 */
	0x00000041,  0x00001c78,	/* 65 */
	0x00000042,  0x8000820c,	/* 66 */
	0x00000043,  0x03fdcb3c,	/* 67 */
	0x00000044,  0x13132270,	/* 68 */
	0x00000045,  0x08086b30,	/* 69 */
	0x00000046,  0x3c3c1b4b,	/* 70 */
	0x00000047,  0x201d211e,	/* 71 */
	0x00000048,  0x3c1b2010,	/* 72 */
	0x00000053,  0x1322224f,	/* 83 */
	0x00000054,  0x00204b0e,	/* 84 */
	0x00000055,  0x133c3c1b,	/* 85 */
	0x00000056,  0x0030281b,	/* 86 */
	0x00000057,  0x0020c74b,	/* 87 */
	0x00000058,  0x0481121c,	/* 88 */
	0x00000059,  0x100e4b31,	/* 89 */
	0x0000005a,  0x08206b30,	/* 90 */
	0x0000005b,  0x3c1b2010,	/* 91 */
	0x0000005e,  0x00768000,	/* 94 */
	0x00000062,  0x800fe038,	/* 98 */
	0x00000063,  0x0172c9e0,	/* 99 */
	0x00000064,  0x40201c1f,	/* 100 */
	0x00000069,  0x800fe038,	/* 105 */
	0x0000006b,  0x000003cc,	/* 107 */
	0x00000082,  0x1451e920,	/* 130 */
	0x00000083,  0x9d114f70,	/* 131 */
	0x00000084,  0x832713a2,	/* 132 */
	0x00000085,  0x739dbdcb,	/* 133 */
	0x00000086,  0x179d2dce,	/* 134 */
	0x00000087,  0x03590438,	/* 135 */
	0x00000088,  0x00001c78,	/* 136 */
	0xffffffff,  0x52e6aefb,	/* -1 */
}; /* 44 differences */

/* 7278B_1867MHz_32b_dev8Gx1_LPDDR4-3733_le.mcb
*/
static const uint32_t mcb_1867mhz_32b_dev8Gx1[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x79c10c51,	/* 3 */
	0x00000004,  0x00030002,	/* 4 */
	0x00000005,  0x00002702,	/* 5 */
	0x00000006,  0x00504678,	/* 6 */
	0x0000000d,  0x000001e6,	/* 13 */
	0x0000000f,  0x0000074b,	/* 15 */
	0x00000010,  0x001c513f,	/* 16 */
	0x00000014,  0x00002000,	/* 20 */
	0x00000018,  0x00001e20,	/* 24 */
	0x00000019,  0x00001e20,	/* 25 */
	0x0000001d,  0x002b766c,	/* 29 */
	0x00000022,  0x002b766c,	/* 34 */
	0x0000002a,  0x00101044,	/* 42 */
	0x0000002b,  0x000bbcd8,	/* 43 */
	0x0000002c,  0x00002d01,	/* 44 */
	0x0000002d,  0x00057fa0,	/* 45 */
	0x0000003e,  0x0006fa43,	/* 62 */
	0x0000003f,  0x80018150,	/* 63 */
	0x00000041,  0x00001c78,	/* 65 */
	0x00000042,  0x80008150,	/* 66 */
	0x00000043,  0x03fdcb3c,	/* 67 */
	0x00000044,  0x13132270,	/* 68 */
	0x00000045,  0x08086b30,	/* 69 */
	0x00000046,  0x3c3c1b4b,	/* 70 */
	0x00000047,  0x201d211e,	/* 71 */
	0x00000048,  0x3c1b2010,	/* 72 */
	0x00000051,  0x0000aa55,	/* 81 */
	0x00000052,  0x000006fa,	/* 82 */
	0x00000053,  0x1322224f,	/* 83 */
	0x00000054,  0x00204b0e,	/* 84 */
	0x00000055,  0x133c3c1b,	/* 85 */
	0x00000056,  0x0030281b,	/* 86 */
	0x00000057,  0x0015074b,	/* 87 */
	0x00000058,  0x04811160,	/* 88 */
	0x00000059,  0x100e4c31,	/* 89 */
	0x0000005a,  0x08206b30,	/* 90 */
	0x0000005b,  0x3c1b2010,	/* 91 */
	0x0000005e,  0x00768000,	/* 94 */
	0x00000062,  0x800fe038,	/* 98 */
	0x00000063,  0x0172c9e0,	/* 99 */
	0x00000064,  0x40201c1f,	/* 100 */
	0x00000069,  0x800fe038,	/* 105 */
	0x0000006b,  0x000003cc,	/* 107 */
	0x00000082,  0x1451e920,	/* 130 */
	0x00000083,  0x9d114f70,	/* 131 */
	0x00000084,  0x542713a2,	/* 132 */
	0x00000085,  0x739dbdcb,	/* 133 */
	0x00000086,  0x179d2dce,	/* 134 */
	0x00000087,  0x03590438,	/* 135 */
	0x00000088,  0x00001c78,	/* 136 */
	0x0000008c,  0x00000000,	/* 140 */
	0xffffffff,  0xf4b4bf91,	/* -1 */
}; /* 51 differences */

/* 7278B_1867MHz_32b_dev8Gx2_LPDDR4-3733_le.mcb
*/
static const uint32_t mcb_1867mhz_32b_dev8Gx2[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0xbbc2bc53,	/* 3 */
	0x00000004,  0x00030002,	/* 4 */
	0x00000005,  0x00002702,	/* 5 */
	0x00000006,  0x0050467a,	/* 6 */
	0x0000000d,  0x000001e6,	/* 13 */
	0x0000000f,  0x0000074b,	/* 15 */
	0x00000010,  0x001c513f,	/* 16 */
	0x00000014,  0x00002000,	/* 20 */
	0x00000015,  0x00002000,	/* 21 */
	0x00000016,  0x00404000,	/* 22 */
	0x00000018,  0x00001e20,	/* 24 */
	0x00000019,  0x00001e20,	/* 25 */
	0x0000001d,  0x002b766c,	/* 29 */
	0x00000022,  0x002b766c,	/* 34 */
	0x0000002a,  0x00101044,	/* 42 */
	0x0000002b,  0x000bbcd8,	/* 43 */
	0x0000002c,  0x00002d01,	/* 44 */
	0x0000002d,  0x00057fa0,	/* 45 */
	0x0000003e,  0x0206fa43,	/* 62 */
	0x0000003f,  0x80018150,	/* 63 */
	0x00000041,  0x00001c78,	/* 65 */
	0x00000042,  0xa0008150,	/* 66 */
	0x00000043,  0x03fdcb3c,	/* 67 */
	0x00000044,  0x13132270,	/* 68 */
	0x00000045,  0x08086b30,	/* 69 */
	0x00000046,  0x3c3c1b4b,	/* 70 */
	0x00000047,  0x201d211e,	/* 71 */
	0x00000048,  0x3c1b2010,	/* 72 */
	0x00000051,  0x0001aa55,	/* 81 */
	0x00000052,  0x000056fa,	/* 82 */
	0x00000053,  0x1322224f,	/* 83 */
	0x00000054,  0x00204b0e,	/* 84 */
	0x00000055,  0x133c3c1b,	/* 85 */
	0x00000056,  0x0030281b,	/* 86 */
	0x00000057,  0x0015074b,	/* 87 */
	0x00000058,  0x04811160,	/* 88 */
	0x00000059,  0x100e4c31,	/* 89 */
	0x0000005a,  0x08206b30,	/* 90 */
	0x0000005b,  0x3c1b2010,	/* 91 */
	0x0000005e,  0x00768000,	/* 94 */
	0x00000062,  0xa00fe038,	/* 98 */
	0x00000063,  0x0172c9e0,	/* 99 */
	0x00000064,  0x40201c1f,	/* 100 */
	0x00000069,  0x800fe038,	/* 105 */
	0x0000006b,  0x000003cc,	/* 107 */
	0x00000082,  0x1451e920,	/* 130 */
	0x00000083,  0x9d114f70,	/* 131 */
	0x00000084,  0x542713a2,	/* 132 */
	0x00000085,  0x739dbdcb,	/* 133 */
	0x00000086,  0x179d2dce,	/* 134 */
	0x00000087,  0x03590438,	/* 135 */
	0x00000088,  0x00001c78,	/* 136 */
	0x0000008c,  0x00000000,	/* 140 */
	0xffffffff,  0x78b81f95,	/* -1 */
}; /* 53 differences */

const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(".mcbtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ memsys_config_template,	MCB_MAGIC1,	SHMOO_ARRAY_SIZE,	SHMOO_ARRAY_ELEMENTS, -1, NULL },
	{ mcb_1600mhz_32b_dev16Gx1,	1600,	16384 | 0,	0, 32, NULL },
	{ mcb_1600mhz_32b_dev8Gx1,	1600,	8192 | 0,	0, 32, NULL },
	{ mcb_1600mhz_32b_dev8Gx2,	1600,	8192 | 1,	0, 32, NULL },
	{ mcb_1867mhz_32b_dev16Gx1,	1867,	16384 | 0,	0, 32, NULL },
	{ mcb_1867mhz_32b_dev8Gx1,	1867,	8192 | 0,	0, 32, NULL },
	{ mcb_1867mhz_32b_dev8Gx2,	1867,	8192 | 1,	0, 32, NULL },
	{ NULL, 0, 0, 0, 0, NULL },
};
