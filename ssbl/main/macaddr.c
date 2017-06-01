/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bolt.h>
#include <devfuncs.h>
#include <error.h>
#include <lib_printf.h>
#include <lib_types.h>
#include <macaddr.h>

/* don't presume U8 alignment WRT U64, so use these macros: */
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
	unsigned char macaddr[MACADDR_FLASHBUFSIZE];
	int macoffset = sizeof(struct macaddr_header) +
		addrnum * MACADDR_STRIDE;

	/* retreive mac address from flash */
	sfd = bolt_open(MACADDR_FLASHDEVICE);
	if (sfd < 0)
		xprintf("cannot open %s\n", MACADDR_FLASHDEVICE);

	if (bolt_read(sfd, macaddr, MACADDR_FLASHBUFSIZE) < 0)
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
	unsigned char fcksum;
	unsigned char chksum;
	int retval;
	int sfd;
	unsigned int i;
	unsigned char *p, macaddr[MACADDR_FLASHBUFSIZE];

	/* retreive mac address from flash */
	sfd = bolt_open(MACADDR_FLASHDEVICE);
	if (sfd < 0) {
		err_msg("cannot open %s\n", MACADDR_FLASHDEVICE);
		return BOLT_ERR_DEVOPEN;
	}

	retval = bolt_read(sfd, macaddr, MACADDR_FLASHBUFSIZE);
	bolt_close(sfd);
	if (retval < 0) {
		err_msg("cannot read mac address");
		return retval;
	}

	/* Check checksum */
	chksum = 0;
	for (i = 0, p = macaddr; i < (MACADDR_FLASHBUFSIZE - 2); ++i, ++p)
		chksum += *p;

	fcksum = *p++; /* the last two bytes should be identical */
	if (fcksum != *p || chksum != fcksum) {
		err_msg("MAC ADDRESS CHECKSUM FAILED");
		return BOLT_ERR;
	}

	return BOLT_OK;
}
