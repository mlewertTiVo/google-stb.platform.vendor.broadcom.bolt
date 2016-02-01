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

#include <lib_hexdump.h>
#include <lib_printf.h>
#include <lib_types.h>
#include <lib_string.h>

#include <common.h>

void lib_hexdump(const void *p, size_t len, int wlen, bool ascii, int prefix)
{
	unsigned int i, j;

	/*
	 * The reason we save the line in this union is to provide the
	 * property that the dump command will only touch the
	 * memory once.  This might be useful when looking at
	 * device registers.
	 */

	union {
		uint8_t bytes[16];
		uint16_t halves[8];
		uint32_t words[4];
		uint64_t quads[2];
	} line;

	if (wlen < 1 || wlen > 8)
		return;

	p = PTR_ALIGN(p, wlen);
	len = ALIGN_UP_TO(len, wlen);

	for (i = 0; i < len; i += 16) {
		/* What's left on this line */
		unsigned int line_rem = min(16U, len - i);

		switch (prefix) {
		case DUMP_PREFIX_ADDRESS:
			printf("%08llx  ", (unsigned long long)(uintptr_t)p + i);
			break;
		case DUMP_PREFIX_OFFSET:
			printf("%08x  ", i);
			break;
		default:
			break;
		}

		memcpy(&line.bytes, p + i, line_rem);

		switch (wlen) {
		default:
		case 1:
			for (j = 0; j < line_rem; j++)
				printf("%02x ", line.bytes[j]);
			break;
		case 2:
			for (j = 0; j < line_rem; j += 2)
				printf("%04x ", line.halves[j / 2]);
			break;
		case 4:
			for (j = 0; j < line_rem; j += 4)
				printf("%08x ", line.words[j / 4]);
			break;
		case 8:
			for (j = 0; j < line_rem; j += 8)
				printf("%016llx ", line.quads[j / 8]);
			break;
		}

		if (ascii) {
			for (j = line_rem; j < 16; j++) {
				/* two spaces per byte */
				printf("  ");
				/* one space per group */
				if (IS_ALIGNED(j, wlen))
					printf(" ");
			}

			printf(" ");
			for (j = 0; j < line_rem; j++) {
				uint8_t b = line.bytes[j];
				if ((b < 32) || (b > 127))
					printf(".");
				else
					printf("%c", b);
			}
		}
		printf("\n");
	}
}
