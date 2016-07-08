/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <fsbl-common.h>

#define SHMOO_ARRAY_SIZE	0x00000260
#define SHMOO_ARRAY_ELEMENTS	0x00000098

/* 7271A0_1333MHz_32b_dev8Gx16_LPDDR4-2667_le.mcb
*/
static const uint32_t memsys_config_template[SHMOO_ARRAY_ELEMENTS] __attribute__ ((section(".mcbdata"))) = {
	0x0132a6ef,	/* 0 */
	0x00000000,	/* 1 */
	0x00000000,	/* 2 */
	0xffa05ee4,	/* 3 */
	0x00000002,	/* 4 */
	0x00001702,	/* 5 */
	0x002037ff,	/* 6 */
	0x007271a0,	/* 7 */
	0x0000b200,	/* 8 */
	0x00004800,	/* 9 */
	0x0000c000,	/* 10 */
	0x000000d8,	/* 11 */
	0x0337f980,	/* 12 */
	0x000001b0,	/* 13 */
	0x00000000,	/* 14 */
	0x00000535,	/* 15 */
	0x001451d2,	/* 16 */
	0x00000008,	/* 17 */
	0x00000004,	/* 18 */
	0x00000001,	/* 19 */
	0x00002000,	/* 20 */
	0x00002000,	/* 21 */
	0x00486060,	/* 22 */
	0x32323232,	/* 23 */
	0x00001618,	/* 24 */
	0x00001618,	/* 25 */
	0x0000000d,	/* 26 */
	0x03ff03ff,	/* 27 */
	0x00000000,	/* 28 */
	0x0021644c,	/* 29 */
	0x10104400,	/* 30 */
	0x00000010,	/* 31 */
	0x00801400,	/* 32 */
	0x00003c5a,	/* 33 */
	0x0021644c,	/* 34 */
	0x50104400,	/* 35 */
	0x00000010,	/* 36 */
	0x00801400,	/* 37 */
	0x00003c5a,	/* 38 */
	0x000001d8,	/* 39 */
	0x55010012,	/* 40 */
	0x10080000,	/* 41 */
	0x00101031,	/* 42 */
	0x000522d0,	/* 43 */
	0x00000000,	/* 44 */
	0x00000000,	/* 45 */
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
	0x00000000,	/* 56 */
	0x00000000,	/* 57 */
	0x00000000,	/* 58 */
	0x00000000,	/* 59 */
	0x00000000,	/* 60 */
	0x00000000,	/* 61 */
	0x0206fa23,	/* 62 */
	0x800180f0,	/* 63 */
	0x00000000,	/* 64 */
	0x00001458,	/* 65 */
	0xa00080f0,	/* 66 */
	0x059682f0,	/* 67 */
	0x0e0e1850,	/* 68 */
	0x08084f22,	/* 69 */
	0x2e2e1736,	/* 70 */
	0x20151916,	/* 71 */
	0x2e171d10,	/* 72 */
	0x0000000f,	/* 73 */
	0x000007ff,	/* 74 */
	0x400fffe1,	/* 75 */
	0x00000000,	/* 76 */
	0x00000000,	/* 77 */
	0x00000000,	/* 78 */
	0x00000000,	/* 79 */
	0x00000000,	/* 80 */
	0x0001a955,	/* 81 */
	0x000056fa,	/* 82 */
	0x0e181838,	/* 83 */
	0x0020360a,	/* 84 */
	0x0e2e2e17,	/* 85 */
	0x00301c13,	/* 86 */
	0x000f0536,	/* 87 */
	0x0380a00a,	/* 88 */
	0x00000000,	/* 89 */
	0x00204f22,	/* 90 */
	0x2e171d10,	/* 91 */
	0x00000011,	/* 92 */
	0x00000003,	/* 93 */
	0x00695000,	/* 94 */
	0x00000005,	/* 95 */
	0x03030303,	/* 96 */
	0x00000303,	/* 97 */
	0x200fe028,	/* 98 */
	0x019bfcc0,	/* 99 */
	0x40181417,	/* 100 */
	0x00050000,	/* 101 */
	0x00000000,	/* 102 */
	0x00000000,	/* 103 */
	0x00000000,	/* 104 */
	0x00000000,	/* 105 */
	0x00000000,	/* 106 */
	0x00000000,	/* 107 */
	0x0000005a,	/* 108 */
	0x01010101,	/* 109 */
	0x01010101,	/* 110 */
	0x00000001,	/* 111 */
	0x01010101,	/* 112 */
	0x01010101,	/* 113 */
	0x00000001,	/* 114 */
	0x00000000,	/* 115 */
	0x01000101,	/* 116 */
	0x00000404,	/* 117 */
	0x0100016f,	/* 118 */
	0x00006e6e,	/* 119 */
	0x0618000c,	/* 120 */
	0x0000003c,	/* 121 */
	0x00000028,	/* 122 */
	0x0000003c,	/* 123 */
	0xffffffff,	/* 124 */
	0x00000000,	/* 125 */
	0x00000000,	/* 126 */
	0x00000000,	/* 127 */
	0x00000000,	/* 128 */
	0x00000000,	/* 129 */
	0x10396718,	/* 130 */
	0x738c3850,	/* 131 */
	0x3c1cc298,	/* 132 */
	0x529539b6,	/* 133 */
	0x10d4d94a,	/* 134 */
	0x02650428,	/* 135 */
	0x00001458,	/* 136 */
	0x00000000,	/* 137 */
	0x00000000,	/* 138 */
	0x00000000,	/* 139 */
	0x00000000,	/* 140 */
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

/* 7271A0_1333MHz_32b_dev8Gx16_LPDDR4-2667_le.mcb
*/
static const uint32_t mcb_1333mhz_32b_dev8Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0xffffffff,  0x007364b7,	/* -1 */
}; /* 0 difference */

/* 7271A0_1600MHz_32b_dev8Gx16_LPDDR4-3200_le.mcb
*/
static const uint32_t mcb_1600mhz_32b_dev8Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x4a5e0a4d,	/* 3 */
	0x00000006,  0x001037ff,	/* 6 */
	0x0000000f,  0x00000640,	/* 15 */
	0x00000010,  0x001863bf,	/* 16 */
	0x00000018,  0x00001a1c,	/* 24 */
	0x00000019,  0x00001a1c,	/* 25 */
	0x0000001d,  0x00216d5c,	/* 29 */
	0x00000022,  0x00216d5c,	/* 34 */
	0x00000027,  0x00000218,	/* 39 */
	0x0000002a,  0x0010103b,	/* 42 */
	0x0000002b,  0x00033333,	/* 43 */
	0x0000003f,  0x80018120,	/* 63 */
	0x00000041,  0x00001868,	/* 65 */
	0x00000042,  0xa0008120,	/* 66 */
	0x00000043,  0x04a817c8,	/* 67 */
	0x00000044,  0x10101d60,	/* 68 */
	0x00000045,  0x08085e29,	/* 69 */
	0x00000046,  0x34341840,	/* 70 */
	0x00000047,  0x20191d1a,	/* 71 */
	0x00000048,  0x34181d10,	/* 72 */
	0x00000053,  0x101d1d44,	/* 83 */
	0x00000054,  0x0020400c,	/* 84 */
	0x00000055,  0x10343418,	/* 85 */
	0x00000056,  0x00302217,	/* 86 */
	0x00000057,  0x00120640,	/* 87 */
	0x00000058,  0x0400c00c,	/* 88 */
	0x0000005a,  0x00205e29,	/* 90 */
	0x0000005b,  0x34181d10,	/* 91 */
	0x00000062,  0x200fe030,	/* 98 */
	0x00000064,  0x401c181b,	/* 100 */
	0x00000082,  0x1249a81c,	/* 130 */
	0x00000083,  0x844ec460,	/* 131 */
	0x00000084,  0x4820f31d,	/* 132 */
	0x00000085,  0x63197bc0,	/* 133 */
	0x00000086,  0x1419018c,	/* 134 */
	0x00000087,  0x02df0430,	/* 135 */
	0x00000088,  0x00001868,	/* 136 */
	0xffffffff,  0x95eebb89,	/* -1 */
}; /* 37 differences */

/* 7271A0_800MHz_32b_dev8Gx16_LPDDR4-1600_le.mcb
*/
static const uint32_t mcb_800mhz_32b_dev8Gx16[] __attribute__ ((section(".mcbdata"))) = {
	0x00000003,  0x45c5f594,	/* 3 */
	0x00000006,  0x004037ff,	/* 6 */
	0x0000000f,  0x00000320,	/* 15 */
	0x00000010,  0x000c31df,	/* 16 */
	0x00000018,  0x00000c0e,	/* 24 */
	0x00000019,  0x00000c0e,	/* 25 */
	0x0000001d,  0x0021522c,	/* 29 */
	0x00000022,  0x0021522c,	/* 34 */
	0x00000027,  0x00000220,	/* 39 */
	0x00000029,  0x10000000,	/* 41 */
	0x0000002a,  0x0010101d,	/* 42 */
	0x0000002b,  0x00099999,	/* 43 */
	0x0000003f,  0x80018090,	/* 63 */
	0x00000041,  0x00000c34,	/* 65 */
	0x00000042,  0xa0008090,	/* 66 */
	0x00000043,  0x09502f90,	/* 67 */
	0x00000044,  0x08080f30,	/* 68 */
	0x00000045,  0x08083417,	/* 69 */
	0x00000046,  0x1d1d1420,	/* 70 */
	0x00000047,  0x200b0f0c,	/* 71 */
	0x00000048,  0x1d141910,	/* 72 */
	0x00000053,  0x080f0f22,	/* 83 */
	0x00000054,  0x00202008,	/* 84 */
	0x00000055,  0x081d1d14,	/* 85 */
	0x00000056,  0x0030110c,	/* 86 */
	0x00000057,  0x00090320,	/* 87 */
	0x00000058,  0x02806006,	/* 88 */
	0x0000005a,  0x00203417,	/* 90 */
	0x0000005b,  0x1d141910,	/* 91 */
	0x00000062,  0x200fe018,	/* 98 */
	0x00000064,  0x400e0a0d,	/* 100 */
	0x00000082,  0x0b18c40e,	/* 130 */
	0x00000083,  0x4227a230,	/* 131 */
	0x00000084,  0x2410820f,	/* 132 */
	0x00000085,  0x318cc620,	/* 133 */
	0x00000086,  0x0a0c80c6,	/* 134 */
	0x00000087,  0x01710418,	/* 135 */
	0x00000088,  0x00000c34,	/* 136 */
	0xffffffff,  0x8cbe9217,	/* -1 */
}; /* 38 differences */

const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(".mcbtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ memsys_config_template,	MCB_MAGIC1,	SHMOO_ARRAY_SIZE,	SHMOO_ARRAY_ELEMENTS, -1, NULL },
	{ mcb_1333mhz_32b_dev8Gx16,	1333,	8192,	16, 32, NULL },
	{ mcb_1600mhz_32b_dev8Gx16,	1600,	8192,	16, 32, NULL },
	{ mcb_800mhz_32b_dev8Gx16,	800,	8192,	16, 32, NULL },
	{ NULL, 0, 0, 0, 0, NULL },
};
