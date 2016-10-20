/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __NET_EBUF_H__
#define __NET_EBUF_H__

#include <lib_queue.h>
#include <stdint.h>

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define ENET_MAX_PKT 1514
#define IP_MAX_PKT (16384+46)
#define ENET_CRC_SIZE 4
#define ENET_ADDR_LEN 6
#define ENET_DIX_HEADER 14
#define ENET_ZLEN 60
#define ENET_VLAN_HLEN 4

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct ebuf_s {
	queue_t eb_qblock;	/* linkage */
	unsigned int eb_length;	/* length of area at eb_ptr */
	unsigned int eb_status;	/* rx/tx status */
	int eb_port;		/* eth port that owns buffer */
	void *eb_device;	/* underlying net device */
	int eb_usrdata;		/* user-defined stuff */
	uint8_t *eb_usrptr;	/* user-defined stuff */
	uint8_t *eb_ptr;	/* current ptr within pkt */
	uint8_t eb_data[IP_MAX_PKT+2];	/* data, must be > ENET_MAX_PKT */
	/* and divisible by sizeof(uint) */
} ebuf_t;

/*
 * Macros to initialize ebufs
 */

#define ebuf_init_rx(eb) (eb)->eb_ptr = (eb)->eb_data; \
                         (eb)->eb_length = 0; \
                         (eb)->eb_status = 0

#define ebuf_init_tx(eb) (eb)->eb_ptr = (eb)->eb_data; \
                         (eb)->eb_length = 0; \
                         (eb)->eb_status = 0

/*
 * Macros to move the currens position within an ebuf
 */

#define ebuf_prepend(eb,amt) (eb)->eb_ptr -= amt; (eb)->eb_length += (amt)
#define ebuf_adjust(eb,amt) (eb)->eb_ptr += (amt); (eb)->eb_length += (amt)
#define ebuf_seek(eb,amt) (eb)->eb_ptr += (amt)

#define ebuf_remlen(eb) ((eb)->eb_length)
#define ebuf_length(eb) ((eb)->eb_length)
#define ebuf_ptr(eb) ((eb)->eb_ptr)
#define ebuf_setlength(eb,amt) (eb)->eb_length = (amt)

/*
 * ebuf_append_xxx macros - these macros add data to the
 * end of a packet, adjusting the length
 */

#define ebuf_append_u8(eb,b) ((eb)->eb_ptr[(eb)->eb_length++]) = (b) ;
#define ebuf_append_u16_be(eb,w) ebuf_append_u8(eb,((w) >> 8) & 0xFF) ; \
                              ebuf_append_u8(eb,((w) & 0xFF))

#define ebuf_append_u32_be(eb,w) ebuf_append_u8(eb,((w) >> 24) & 0xFF) ; \
                              ebuf_append_u8(eb,((w) >> 16) & 0xFF) ; \
                              ebuf_append_u8(eb,((w) >> 8) & 0xFF) ; \
                              ebuf_append_u8(eb,((w) & 0xFF))

#define ebuf_append_bytes(eb,b,l) memcpy(&((eb)->eb_ptr[(eb)->eb_length]),(b),(l)); \
                              (eb)->eb_length += (l)

/*
 * ebuf_put_xxx macros - these macros modify data in the middle
 * of a packet, typically in an area already allocated with
 * ebuf_adjust.  The length is not updated, but the pointer is.
 */

#define ebuf_put_u8(eb,b) *((eb)->eb_ptr++) = (b)
#define ebuf_put_u16_be(eb,w) ebuf_put_u8(eb,((w) >> 8) & 0xFF) ; \
                              ebuf_put_u8(eb,((w) & 0xFF))

#define ebuf_put_u32_be(eb,w) ebuf_put_u8(eb,((w) >> 24) & 0xFF) ; \
                              ebuf_put_u8(eb,((w) >> 16) & 0xFF) ; \
                              ebuf_put_u8(eb,((w) >> 8) & 0xFF) ; \
                              ebuf_put_u8(eb,((w) & 0xFF))

#define ebuf_put_bytes(eb,b,l) memcpy((buf)->eb_ptr,(b),(l)) ; (buf)->eb_ptr += (l)

#define ebuf_srcaddr(eb) &((eb)->eb_data[6])
#define ebuf_destaddr(eb) &((eb)->eb_data[0])

/* return next byte from the ebuf, and advance pointer */

#define ebuf_skip(eb,amt) (eb)->eb_length -= (amt); (eb)->eb_ptr += (amt)

#define ebuf_get_u8(eb,v)    (eb)->eb_length--; \
                                v = (*((eb)->eb_ptr++))

/* return next u16 from the ebuf, big-endian */

#define ebuf_get_u16_be(eb,v)   (eb)->eb_ptr+=2; \
                                (eb)->eb_length-=2; \
                                v = ( ((uint16_t)*((eb)->eb_ptr-1)) | \
                                      (((uint16_t)*((eb)->eb_ptr-2)) << 8) )

/* return next u32 from the ebuf, big-endian */

#define ebuf_get_u32_be(eb,v)   (eb)->eb_ptr+=4;        \
                                (eb)->eb_length-=4; \
                                v = ( ((uint32_t)*((eb)->eb_ptr-1)) | \
                                      (((uint32_t)*((eb)->eb_ptr-2)) << 8) | \
                                      (((uint32_t)*((eb)->eb_ptr-3)) << 16) | \
                                      (((uint32_t)*((eb)->eb_ptr-4)) << 24) )

#define ebuf_get_bytes(eb,b,l) memcpy((b),(eb)->eb_ptr,(l)) ; (eb)->eb_ptr += (l) ; (eb)->eb_length -= (l)

#endif /* __NET_EBUF_H__ */

