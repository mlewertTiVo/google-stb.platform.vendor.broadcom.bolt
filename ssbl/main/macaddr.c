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
#include "lib_printf.h"
#include "lib_string.h"

#include "iocb.h"
#include "device.h"
#include "bolt.h"

#include "env_subr.h"
#include "devfuncs.h"


#define FLASH_LENGTH   0x8A
#define FIRSTMAC_ADDR   0xC
#define MACADDR_STRIDE  0x8


/* don't presume U8 alignment WRT U64, so use these macros:
*/
#ifdef __LITTLE_ENDIAN

#define MACtoU64(m) \
	((((uint64_t)m[5] <<  0) & 0x00000000000000ffULL) | \
	 (((uint64_t)m[4] <<  8) & 0x000000000000ff00ULL) | \
	 (((uint64_t)m[3] << 16) & 0x0000000000ff0000ULL) | \
	 (((uint64_t)m[2] << 24) & 0x00000000ff000000ULL) | \
	 (((uint64_t)m[1] << 32) & 0x000000ff00000000ULL) | \
	 (((uint64_t)m[0] << 40) & 0x0000ff0000000000ULL))

#define U64toMAC(m, u) \
	{ \
		m[5] = (uint8_t) ((u >>  0) & 0xff); \
		m[4] = (uint8_t) ((u >>  8) & 0xff); \
		m[3] = (uint8_t) ((u >> 16) & 0xff); \
		m[2] = (uint8_t) ((u >> 24) & 0xff); \
		m[1] = (uint8_t) ((u >> 32) & 0xff); \
		m[0] = (uint8_t) ((u >> 40) & 0xff); \
	}

#else

#define MACtoU64(m) \
	((((uint64_t)m[0] <<  0) & 0x00000000000000ffULL) | \
	 (((uint64_t)m[1] <<  8) & 0x000000000000ff00ULL) | \
	 (((uint64_t)m[2] << 16) & 0x0000000000ff0000ULL) | \
	 (((uint64_t)m[3] << 24) & 0x00000000ff000000ULL) | \
	 (((uint64_t)m[4] << 32) & 0x000000ff00000000ULL) | \
	 (((uint64_t)m[5] << 40) & 0x0000ff0000000000ULL))

#define U64toMAC(m, u) \
	{ \
		m[0] = (uint8_t) ((u >>  0) & 0xff); \
		m[1] = (uint8_t) ((u >>  8) & 0xff); \
		m[2] = (uint8_t) ((u >> 16) & 0xff); \
		m[3] = (uint8_t) ((u >> 24) & 0xff); \
		m[4] = (uint8_t) ((u >> 32) & 0xff); \
		m[5] = (uint8_t) ((u >> 40) & 0xff); \
	}
#endif


void macaddr_flash_get(int addrnum, char *macstr, int flashprog_off)
{
	int sfd;
	uint8_t macaddr[256];
	int macoffset = FIRSTMAC_ADDR + addrnum * MACADDR_STRIDE;

	/* retreive mac address from flash */
	sfd = bolt_open("flash0.macadr");
	if (sfd < 0)
		xprintf("cannot open flash0.macadr\n");

	if (bolt_read(sfd, (unsigned char *)macaddr, 256) < 0)
		xprintf("!!!cannot read mac address\n");

	bolt_close(sfd);

	xsprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X",
		 macaddr[0 + macoffset], macaddr[1 + macoffset],
		 macaddr[2 + macoffset], macaddr[3 + macoffset],
		 macaddr[4 + macoffset], macaddr[5 + macoffset]);
}


void macaddr_increment(uint8_t *macaddr, int increment)
{
	uint64_t bigmac;

	bigmac = MACtoU64(macaddr);
	bigmac += (uint64_t)increment;
	U64toMAC(macaddr, bigmac);
}

void macaddr_decrement(uint8_t *macaddr, int decrement)
{
	uint64_t bigmac;

	bigmac = MACtoU64(macaddr);
	bigmac -= (uint64_t)decrement;
	U64toMAC(macaddr, bigmac);
}


int macaddr_flash_verify(int flashprog_off)
{
	uint8_t fcksum;
	uint8_t chksum;
	uint16_t *src;
	int retval = 0;
	int sfd;
	uint8_t macaddr[256];

	/* retreive mac address from flash */
	sfd = bolt_open("flash0.macadr");
	if (sfd < 0) {
		err_msg("cannot open flash0.macadr");
		return -1;
	}

	if (bolt_read(sfd, (unsigned char *)macaddr, 256) < 0) {
		err_msg("cannot read mac address");
		return -1;
	}
	bolt_close(sfd);

	/* Check checksum */
	chksum = 0;
	for (src = (uint16_t *) (macaddr);
	     src <= (uint16_t *) (macaddr + FLASH_LENGTH);) {
		chksum += ((*src) & 0xFF00) >> 8;
		chksum += (*src) & 0x00FF;
		src++;
	}

	fcksum = (*src) & 0x00FF;

	if (chksum != fcksum) {
		err_msg("MAC ADDRESS CHECKSUM FAILED");
		retval = -1;
	}

	return retval;
}
