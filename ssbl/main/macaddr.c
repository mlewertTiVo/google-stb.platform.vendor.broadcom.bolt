/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bchp_sun_top_ctrl.h>
#include <bolt.h>
#include <byteorder.h>
#include <chipid.h>
#include <devfuncs.h>
#include <error.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_types.h>
#include <macaddr.h>
#include <sha2.h>

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

/* MAC Address Block Large (MA_L) assigned to Broadcom Limited per:
 * http://standards-oui.ieee.org/oui/oui.txt
 */
static const uint8_t MA_L[3] = { 0x00, 0x10, 0x18 };

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

/* macaddr_flash_set -- programs a flash partition with a given MAC address
 *
 * Parameters:
 *  macaddr [in] pointer to buffer of 6 (six) bytes
 *  bytes   [in] size of 'macaddr' in bytes
 *
 * Returns:
 *  BOLT_ERR_INV_PARAM if macaddr is NULL or bytes is not MACADDR_SIZE (6)
 *  BOLT_ERR_DEVNOTFOUND if failing to open a flash partition for MAC
 *  BOLT_ERR_IOERR if failing to program the flash partition
 *  BOLT_OK if successfully programs MAC into the partition
 */
int macaddr_flash_set(uint8_t *macaddr, unsigned int bytes)
{
	unsigned int i;
	int sfd;
	int retval;
	unsigned char checksum = 0;
	unsigned char *p, buf[MACADDR_FLASHBUFSIZE];
	struct macaddr_header *header;

	if (macaddr == NULL || bytes != MACADDR_SIZE)
		return BOLT_ERR_INV_PARAM;

	memset(buf, 0xff, sizeof(buf));
	header = (struct macaddr_header *) buf;
	memset(header, 0, sizeof(*header));
	header->size = cpu_to_be16(MACADDR_SIZEUNTILCHKSUM);

	p = buf + sizeof(struct macaddr_header);
	for (i = 0; i < MACADDR_NUMADDRS; i++) {
		memcpy(p, macaddr, MACADDR_SIZE);
		p += MACADDR_STRIDE; /* not MACADDR_SIZE */
	}

	/* calculate checksum. */
	p = buf;
	checksum = 0;
	for (i = 0; i < (sizeof(buf) - 2); i++, p++)
		checksum += *p;
	*p++ = checksum;
	*p = checksum; /* repeat */

	sfd = bolt_open(MACADDR_FLASHDEVICE);
	if (sfd < 0) {
		xprintf("cannot open %s\n", MACADDR_FLASHDEVICE);
		return BOLT_ERR_DEVNOTFOUND;
	}

	xprintf("Programming flash...");

	retval = bolt_writeblk(sfd, (bolt_offset_t) 0, buf, sizeof(buf));
	bolt_close(sfd);

	if (retval != sizeof(buf)) {
		xprintf("failed while writing to flash.\n");
		xprintf("(%d != %d)\n", retval, sizeof(buf));
		return BOLT_ERR_IOERR;
	}

	xprintf("done\n");
	return BOLT_OK;
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

/* macaddr_generate -- generates a MAC address based on a serial number
 *
 * Parameters:
 *  serial  [in]  pointer to serial number
 *  macaddr [out] pointer to MAC address buffer to be filled out
 *  bytes   [in]  size of 'macaddr' in bytes
 *
 * Returns:
 *  BOLT_ERR_INV_PARAM if serial/macaddr is NULL or bytes is not MACADDR_SIZE
 *  BOLT_OK otherwise
 */
int macaddr_generate(const char *serial, uint8_t *macaddr, unsigned int bytes)
{
	uint32_t family_id;
	char boardtype[8]; /* "97271", "974371" and so on */
	sha256_ctx ctx;
	unsigned char digest[SHA256_DIGEST_SIZE];

	if (serial == NULL || macaddr == NULL || bytes != MACADDR_SIZE)
		return BOLT_ERR_INV_PARAM;

	family_id = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
	xsprintf(boardtype, "9%x", chipid_without_rev(family_id));

	sha256_init(&ctx);
	sha256_update(&ctx, (unsigned char *)boardtype, strlen(boardtype));
	sha256_update(&ctx, (unsigned char *)serial, strlen(serial));
	sha256_final(&ctx, digest);

	memcpy(macaddr, digest, MACADDR_SIZE);
	memcpy(macaddr, MA_L, sizeof(MA_L));

	return BOLT_OK;
}
