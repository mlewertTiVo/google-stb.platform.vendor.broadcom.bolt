/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __MACADDR_H__
#define __MACADDR_H__

#include <stdint.h>

#define MACADDR_SIZE 6 /* #octets, to be distinguished from MACADDR_STRIDE */
/* IEEE 802 specifies 6 (six) octets (bytes) for the size of a MAC address.
 * MACADDR_STRIDE is defined differently for the data structure of the MAC
 * address flash partition used by BOLT. They should not be confused.
 */

/* structure of MAC address partition in flash
 *
 * uint32_t      board type, ((board type) << 8)
 * uint16_t      0 (zero)
 * uint16_t      # bytes from board serial until checksum
 * uint16_t      board serial number
 * uint16_t      board revision number
 * uint8_t[8][8] 8 MAC addresses, two LSB's of an MAC address do not matter
 * uint8_t[8][8] not used
 * uint8_t       checksum
 * uint8_t       checksum (identical)
 *
 * Big Endian (network byte ordering) for uint32_t and uint16_t
 */

struct macaddr_header {
	uint32_t board_type; /* (board type) << 8 */
	uint16_t zero; /* 0 (zero), always */
	uint16_t size; /* #bytes from board_serial until checksum */
	uint16_t board_serial;
	uint16_t board_revision;
};

#define MACADDR_NUMADDRS 8
#define MACADDR_NUMSETS 2
#define MACADDR_STRIDE 8 /* 8 bytes to represent one MAC address */
#define MACADDR_FLASHBUFSIZE (sizeof(struct macaddr_header) + \
	MACADDR_NUMSETS * MACADDR_NUMADDRS * MACADDR_STRIDE + \
	2) /* 2 : two checksums, one byte each */
#define MACADDR_SIZEUNTILCHKSUM (sizeof(uint16_t) + sizeof(uint16_t) + \
	MACADDR_NUMSETS * MACADDR_NUMADDRS * MACADDR_STRIDE)
#define MACADDR_FLASHDEVICE "flash0.macadr"
#define MACADDR_INCREMENT 1 /* older chips used a 0x100 increment */

void macaddr_flash_get(int addrnum, char *macstr, int flashprog_off);
int  macaddr_flash_set(uint8_t *macaddr, unsigned int bytes);
int  macaddr_flash_verify(int flashprog_off);
void macaddr_increment(uint8_t *macaddr, int increment);
void macaddr_decrement(uint8_t *macaddr, int decrement);
int  macaddr_generate(const char *serial, uint8_t *macaddr, unsigned int bytes);

#endif /* __MACADDR_H__ */
