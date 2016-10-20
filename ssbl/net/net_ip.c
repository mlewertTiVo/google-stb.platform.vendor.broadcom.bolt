/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "net_ebuf.h"
#include "net_ether.h"

#include "net_ip.h"
#include "net_ip_internal.h"

#include "error.h"

typedef struct {
	uint16_t first;
	uint16_t last;
	uint16_t next_hole;
} hole_t;

/**********************************************************************
  *  Forward declarations
  **********************************************************************/

static int ip_rx_callback(ebuf_t *buf, void *ref);

/**********************************************************************
  *  _ip_alloc(ipi)
  *
  *  Allocate an ebuf and reserve space for the IP header in it.
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *
  *  Return value:
  *	 ebuf - an ebuf, or NULL if there are none left
  **********************************************************************/
ebuf_t *_ip_alloc(ip_info_t *ipi)
{
	ebuf_t *buf;

	buf = eth_alloc(ipi->eth_info, ipi->ip_port);

	if (buf == NULL)
		return buf;

	ebuf_seek(buf, IPHDR_LENGTH);
	ebuf_setlength(buf, 0);

	return buf;
}

/**********************************************************************
  *  ip_chksum(initcksum,ptr,len)
  *
  *  Do an IP checksum for the specified buffer.  You can pass
  *  an initial checksum if you're continuing a previous checksum
  *  calculation, such as for UDP headers and pseudoheaders.
  *
  *  Input parameters:
  *	 initcksum - initial checksum (usually zero)
  *	 ptr - pointer to buffer to checksum
  *	 len - length of data in bytes
  *
  *  Return value:
  *	 checksum (16 bits)
  **********************************************************************/
uint16_t ip_chksum(uint16_t initcksum, uint8_t *ptr, int len)
{
	unsigned int cksum;
	int idx;
	int odd;

	cksum = (unsigned int)initcksum;

	odd = len & 1;
	len -= odd;

	for (idx = 0; idx < len; idx += 2) {
		cksum +=
		    ((unsigned long)ptr[idx] << 8) +
		    ((unsigned long)ptr[idx + 1]);
	}

	if (odd) {		/* buffer is odd length */
		cksum += ((unsigned long)ptr[idx] << 8);
	}

	/* Fold in the carries
	 */

	while (cksum >> 16)
		cksum = (cksum & 0xFFFF) + (cksum >> 16);

	return cksum;
}

/**********************************************************************
  *  _ip_send(ipi,buf,destaddr,proto)
  *
  *  Send an IP datagram.  We only support non-fragmented datagrams
  *  at this time.
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *	 buf - an ebuf
  *	 destaddr - destination IP address
  *	 proto - IP protocol number
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
int _ip_send(ip_info_t *ipi, ebuf_t *buf, uint8_t *destaddr, uint8_t proto)
{
	uint16_t cksum;
	uint8_t masksrc[IP_ADDR_LEN];
	uint8_t maskdest[IP_ADDR_LEN];
	int pktlen;
	uint8_t *ptr;
	int res = 0;

	/* Move to the beginning of the IP hdeader */

	ebuf_seek(buf, -IPHDR_LENGTH);

	pktlen = ebuf_length(buf) + IPHDR_LENGTH;

	ipi->ip_id++;

	/* Insert the IP header */

	ebuf_put_u8(buf, IPHDR_VER_4 | IPHDR_LEN_20);
	ebuf_put_u8(buf, IPHDR_TOS_DEFAULT);
	ebuf_put_u16_be(buf, pktlen);
	ebuf_put_u16_be(buf, ipi->ip_id);
	ebuf_put_u16_be(buf, 0);
	ebuf_put_u8(buf, IPHDR_TTL_DEFAULT);
	ebuf_put_u8(buf, proto);
	ebuf_put_u16_be(buf, 0);	/* checksum */
	ebuf_put_bytes(buf, ipi->net_info.ip_addr, IP_ADDR_LEN);
	ebuf_put_bytes(buf, destaddr, IP_ADDR_LEN);

	/* adjust pointer and add in the header length */

	ebuf_prepend(buf, IPHDR_LENGTH);

	/* Checksum the header */

	ptr = ebuf_ptr(buf);
	cksum = ip_chksum(0, ptr, IPHDR_LENGTH);
	cksum = ~cksum;
	ptr[10] = (cksum >> 8) & 0xFF;
	ptr[11] = (cksum >> 0) & 0xFF;

	/* If sending to the IP broadcast address,
	 * send to local broadcast.
	 */

	if (ip_addrisbcast(destaddr)) {
		res = eth_send(buf, (uint8_t *) eth_broadcast);
		eth_free(buf);
		return res;
	}

	/* If the mask has not been set, don't try to
	 * determine if we should use the gateway or not.
	 */

	if (ip_addriszero(ipi->net_info.ip_netmask))
		return _arp_lookup_and_send(ipi, buf, destaddr);

	/* Compute (dest-addr & netmask)  and   (my-addr & netmask)
	 */

	ip_mask(masksrc, destaddr, ipi->net_info.ip_netmask);
	ip_mask(maskdest, ipi->net_info.ip_addr, ipi->net_info.ip_netmask);

	/* if destination and my address are on the same subnet,
	 * send the packet directly.  Otherwise, send via
	 * the gateway.
	 */

	if (ip_compareaddr(masksrc, maskdest) == 0) {
		return _arp_lookup_and_send(ipi, buf, destaddr);
	} else {
		/* if no gw configured, drop packet */
		if (ip_addriszero(ipi->net_info.ip_gateway)) {
			eth_free(buf);	/* silently drop */
			return 0;
		} else {
			return _arp_lookup_and_send(ipi, buf,
						    ipi->net_info.ip_gateway);
		}
	}

}

/**********************************************************************
  *  ip_rx_callback(buf,ref)
  *
  *  Receive callback for IP packets.  This routine is called
  *  by the Ethernet datalink.  We look up a suitable protocol
  *  handler and pass the packet off.
  *
  *  Input parameters:
  *	 buf - ebuf we received
  *	 ref - reference data from the ethernet datalink
  *
  *  Return value:
  *	 ETH_KEEP to keep the packet
  *	 ETH_DROP to drop the packet
  **********************************************************************/
static int ip_rx_callback(ebuf_t *buf, void *ref)
{
	ip_info_t *ipi = ref;
	uint8_t tmp;
	int hdrlen;
	uint8_t *hdr;
	uint16_t origchksum;
	uint16_t calcchksum;
	uint16_t length;
	uint16_t *length_ptr;
	uint16_t tmp16;
	uint16_t fragment_offset;
	uint16_t identification;
	uint8_t more_fragments;
	uint8_t proto;
	uint8_t srcip[IP_ADDR_LEN];
	uint8_t dstip[IP_ADDR_LEN];
	ip_protodisp_t *pdisp;
	int res;
	int idx;

	hdr = ebuf_ptr(buf);	/* save current posn */

	ebuf_get_u8(buf, tmp);	/* version and header length */

	/* Check IP version
	 */

	if ((tmp & 0xF0) != IPHDR_VER_4)
		goto drop;	/* not IPV4 */

	hdrlen = (tmp & 0x0F) * 4;

	/* Check header size
	 */

	if (hdrlen < IPHDR_LENGTH)
		goto drop;	/* header < 20 bytes */

	/* Check the checksum
	 */
	origchksum = ((uint16_t) hdr[10] << 8) | (uint16_t) hdr[11];
	hdr[10] = hdr[11] = 0;
	calcchksum = ~ip_chksum(0, hdr, hdrlen);

	if (calcchksum != origchksum)
		goto drop;

	/* Okay, now go back and check other fields.
	 */

	ebuf_skip(buf, 1);	/* skip TOS field */

	length_ptr = (uint16_t *)ebuf_ptr(buf);
	ebuf_get_u16_be(buf, length);
	/* get Identification field */
	ebuf_get_u16_be(buf, identification);

	/* get Fragment Offset field */
	ebuf_get_u16_be(buf, tmp16);
	fragment_offset = tmp16 & IPHDR_FRAGOFFSET;
	more_fragments = (tmp16 & IPHDR_MOREFRAGMENTS) >> 13;
	ebuf_skip(buf, 1);	/* skip TTL */
	ebuf_get_u8(buf, proto);	/* get protocol */
	ebuf_skip(buf, 2);	/* skip checksum */

	ebuf_get_bytes(buf, srcip, IP_ADDR_LEN);
	ebuf_get_bytes(buf, dstip, IP_ADDR_LEN);

	/* skip rest of header */
	ebuf_skip(buf, hdrlen - IPHDR_LENGTH);

	/* set length to just data portion */
	ebuf_setlength(buf, length - hdrlen);

	/* If our address is not set, let anybody in.  We need this to
	 * properly pass up DHCP replies that get forwarde through routers.
	 * Otherwise, only let in matching addresses or broadcasts.
	 */

	if (!ip_addriszero(ipi->net_info.ip_addr)) {
		if ((ip_compareaddr(dstip, ipi->net_info.ip_addr) != 0) &&
		    !(ip_addrisbcast(dstip))) {
			goto drop;	/* not for us */
		}
	}

	if ((fragment_offset != 0) || (more_fragments != 0)) {
		hole_t *payload;
		hole_t this_fragment;
		hole_t *hole;
		hole_t *new_hole;
		hole_t *previous_hole = 0;

		payload = (hole_t *)ipi->packet_assembly_buffer;

		this_fragment.first = fragment_offset * 8;
		this_fragment.last = fragment_offset * 8 + length - 1 - hdrlen;

		if (this_fragment.last > IP_MAX_PKT) {
			xprintf("Fragmented packet too large: %d\n",
				this_fragment.last);
			goto drop;
		}

		if (identification != ipi->assembly_identification ||
		    proto != ipi->assembly_proto ||
		    memcmp(srcip, &ipi->assembly_ip, IP_ADDR_LEN)) {
			ipi->first_hole = 0;
			ipi->total_len = 0;
			payload->first = 0;
			payload->last = 65535;
			payload->next_hole = 0;
			previous_hole = 0;
			ipi->assembly_identification = identification;
			ipi->assembly_proto = proto;
			memcpy(&ipi->assembly_ip, srcip, IP_ADDR_LEN);
		}

		/* RFC815: 1 */
		hole = (hole_t *)((char *)payload + ipi->first_hole);
		do {
			/* RFC815:2 */
			if (this_fragment.first > hole->last) {
				previous_hole = hole;
				continue;
			}

			/* RFC815:3 */
			if (this_fragment.last < hole->first) {
				previous_hole = hole;
				continue;
			}

			/* RFC815:4 */
			if (previous_hole == 0)
				ipi->first_hole = hole->next_hole;
			else
				previous_hole->next_hole = hole->next_hole;

			/* RFC815:5 */
			if (this_fragment.first > hole->first) {
				new_hole = hole;
				new_hole->first = hole->first;
				new_hole->last = this_fragment.first - 1;
				if (previous_hole == 0) {
					ipi->first_hole = new_hole->first;
					new_hole->next_hole = 0;
				} else {
					previous_hole->next_hole = new_hole->first;
					new_hole->next_hole = previous_hole->next_hole;
				}
			}

			/* RFC815:6 */
			if (this_fragment.last < hole->last &&
			    more_fragments != 0) {
				new_hole = (hole_t *)((char *)payload + this_fragment.last + 1);
				new_hole->first = this_fragment.last + 1;
				new_hole->last = hole->last;
				if (previous_hole != 0) {
					new_hole->next_hole = previous_hole->next_hole;
					previous_hole->next_hole = new_hole->first;
				} else {
					new_hole->next_hole = 0;
					ipi->first_hole = new_hole->first;
				}
			}

			/* RFC815: 1 */
			previous_hole = hole;
			hole = (hole_t *)((char*)payload + hole->next_hole);
		} while (previous_hole->next_hole != 0);

		memcpy(((char *)payload) + this_fragment.first, buf->eb_ptr,
		       this_fragment.last - this_fragment.first + 1);

		if (more_fragments == 0)
			ipi->total_len = this_fragment.last + 1;

		if (ipi->first_hole == 0) {
			*length_ptr = ipi->total_len;
			/* set length to just data portion */
			ebuf_setlength(buf, ipi->total_len);
			memcpy(ebuf_ptr(buf), payload, ipi->total_len);
		} else
			goto drop;
	}

	/* ebuf's pointer now starts at beginning of protocol data
	 */

	/* Find matching protocol dispatch
	 */

	pdisp = ipi->ip_protocols;
	res = ETH_DROP;
	for (idx = 0; idx < IP_MAX_PROTOCOLS; idx++) {
		if (pdisp->cb && (pdisp->protocol == proto)) {
			res = (*(pdisp->cb)) (pdisp->ref, buf, dstip, srcip);
			break;
		}
		pdisp++;
	}

	return res;

drop:
	return ETH_DROP;
}

/**********************************************************************
  *  _ip_init(eth)
  *
  *  Initialize the IP layer, attaching it to an underlying Ethernet
  *  datalink interface.
  *
  *  Input parameters:
  *	 eth - Ethernet datalink information
  *
  *  Return value:
  *	 ip_info pointer (IP stack information) or NULL if error
  **********************************************************************/
ip_info_t *_ip_init(ether_info_t *eth)
{
	ip_info_t *ipi;
	uint8_t ipproto[2];

	/* Allocate IP stack info
	 */

	ipi = KMALLOC(sizeof(ip_info_t), 0);
	if (ipi == NULL)
		return NULL;

	memset(ipi, 0, sizeof(ip_info_t));

	ipi->packet_assembly_buffer = KMALLOC(IP_MAX_PKT, 0);
	if (ipi->packet_assembly_buffer == NULL) {
		KFREE(ipi);
		return NULL;
	}

	ipi->eth_info = eth;

	/* Initialize ARP
	 */

	if (_arp_init(ipi) < 0) {
		KFREE(ipi->packet_assembly_buffer);
		KFREE(ipi);
		return NULL;
	}

	/* Open the Ethernet portal for IP packets
	 */

	ipproto[0] = (PROTOSPACE_IP >> 8) & 0xFF;
	ipproto[1] = PROTOSPACE_IP & 0xFF;
	ipi->ip_port =
	    eth_open(ipi->eth_info, ETH_PTYPE_DIX, (char *)ipproto,
		     ip_rx_callback, ipi);

	if (ipi->ip_port < 0) {
		_arp_uninit(ipi);
		KFREE(ipi->packet_assembly_buffer);
		KFREE(ipi);
		return NULL;
	}

	return ipi;
}

/**********************************************************************
  *  _ip_uninit(ipi)
  *
  *  Un-initialize the IP layer, freeing resources
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_uninit(ip_info_t *ipi)
{
	/* Close the IP portal
	 */

	eth_close(ipi->eth_info, ipi->ip_port);

	/* Turn off the ARP layer.
	 */

	_arp_uninit(ipi);

	/* free strings containing the domain and host names
	 */

	if (ipi->net_info.ip_domain)
		KFREE(ipi->net_info.ip_domain);

	if (ipi->net_info.ip_hostname)
		KFREE(ipi->net_info.ip_hostname);
	if (ipi->packet_assembly_buffer)
		KFREE(ipi->packet_assembly_buffer);

	/* Free the stack information
	 */

	KFREE(ipi);
}

/**********************************************************************
  *  _ip_timer_tick(ipi)
  *
  *  Called once per second while the IP stack is active.
  *
  *  Input parameters:
  *	 ipi - ip stack information
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_timer_tick(ip_info_t *ipi)
{
	_arp_timer_tick(ipi);
}

/**********************************************************************
  *  _ip_free(ipi,buf)
  *
  *  Free an ebuf allocated via _ip_alloc
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *	 buf - ebuf to free
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_free(ip_info_t *ipi, ebuf_t *buf)
{
	eth_free(buf);
}

/**********************************************************************
  *  _ip_getaddr(ipi,buf)
  *
  *  Return our IP address (is this used?)
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *	 buf - pointer to 4-byte buffer to receive IP address
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_getaddr(ip_info_t *ipi, uint8_t *buf)
{
	memcpy(buf, ipi->net_info.ip_addr, IP_ADDR_LEN);
}

/**********************************************************************
  *  _ip_getparam(ipi,param)
  *
  *  Return the value of an IP parameter (address, netmask, etc.).
  *  The return value may need to be coerced if it's not normally
  *  a uint8_t* pointer.
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *	 param - parameter number
  *
  *  Return value:
  *	 parameter value, or NULL if the parameter is invalid or
  *	 not set.
  **********************************************************************/
uint8_t *_ip_getparam(ip_info_t *ipinfo, int param)
{
	uint8_t *ret = NULL;

	switch (param) {
	case NET_IPADDR:
		ret = ipinfo->net_info.ip_addr;
		break;
	case NET_NETMASK:
		ret = ipinfo->net_info.ip_netmask;
		break;
	case NET_GATEWAY:
		ret = ipinfo->net_info.ip_gateway;
		break;
	case NET_NAMESERVER:
		ret = ipinfo->net_info.ip_nameserver;
		break;
	case NET_HWADDR:
		ret = ipinfo->arp_hwaddr;
		break;
	case NET_DOMAIN:
		ret = (unsigned char *)ipinfo->net_info.ip_domain;
		break;
	case NET_HOSTNAME:
		ret = (unsigned char *)ipinfo->net_info.ip_hostname;
		break;
	case NET_SPEED:
		return NULL;
		break;
	case NET_LOOPBACK:
		return NULL;
		break;
	}

	return ret;
}

/**********************************************************************
  *  _ip_getparam(ipi,param,value)
  *
  *  Set the value of an IP parameter (address, netmask, etc.).
  *  The value may need to be coerced if it's not normally
  *  a uint8_t* pointer.
  *
  *  Input parameters:
  *	 ipi - IP stack information
  *	 param - parameter number
  *	 value - parameter's new value
  *
  *  Return value:
  *	 0 if ok, else error code
  **********************************************************************/
int _ip_setparam(ip_info_t *ipinfo, int param, uint8_t *ptr)
{
	int res = -1;

	switch (param) {
	case NET_IPADDR:
		memcpy(ipinfo->net_info.ip_addr, ptr, IP_ADDR_LEN);
		_arp_send_gratuitous(ipinfo);
		res = 0;
		break;
	case NET_NETMASK:
		memcpy(ipinfo->net_info.ip_netmask, ptr, IP_ADDR_LEN);
		res = 0;
		break;
	case NET_GATEWAY:
		memcpy(ipinfo->net_info.ip_gateway, ptr, IP_ADDR_LEN);
		res = 0;
		break;
	case NET_NAMESERVER:
		memcpy(ipinfo->net_info.ip_nameserver, ptr, IP_ADDR_LEN);
		res = 0;
		break;
	case NET_DOMAIN:
		if (ipinfo->net_info.ip_domain) {
			KFREE(ipinfo->net_info.ip_domain);
			ipinfo->net_info.ip_domain = NULL;
		}
		if (ptr)
			ipinfo->net_info.ip_domain = strdup((char *)ptr);
		break;
	case NET_HOSTNAME:
		if (ipinfo->net_info.ip_hostname) {
			KFREE(ipinfo->net_info.ip_hostname);
			ipinfo->net_info.ip_hostname = NULL;
		}
		if (ptr)
			ipinfo->net_info.ip_hostname = strdup((char *)ptr);
		break;
	case NET_HWADDR:
		memcpy(ipinfo->arp_hwaddr, ptr, ENET_ADDR_LEN);
		eth_sethwaddr(ipinfo->eth_info, ptr);
		res = 0;
		break;
	case NET_SPEED:
		res = eth_setspeed(ipinfo->eth_info, (int)*ptr);
		break;
	case NET_LOOPBACK:
		res = eth_setloopback(ipinfo->eth_info, (int)ptr);
		break;
	}

	return res;
}

/**********************************************************************
  *  _ip_register(ipinfo,proto,cb)
  *
  *  Register a protocol handler with the IP layer.  IP client
  *  protocols such as UDP, ICMP, etc. call this to register their
  *  callbacks.
  *
  *  Input parameters:
  *	 ipinfo - IP stack information
  *	 proto - IP protocol number
  *	 cb - callback routine to register
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_register(ip_info_t *ipinfo,
		  int proto,
		  int (*cb) (void *ref, ebuf_t *buf, uint8_t *dst,
			     uint8_t *src), void *ref)
{
	int idx;

	for (idx = 0; idx < IP_MAX_PROTOCOLS; idx++) {
		if (ipinfo->ip_protocols[idx].cb == NULL)
			break;
	}

	if (idx == IP_MAX_PROTOCOLS)
		return;

	ipinfo->ip_protocols[idx].protocol = (uint8_t) proto;
	ipinfo->ip_protocols[idx].cb = cb;
	ipinfo->ip_protocols[idx].ref = ref;
}

/**********************************************************************
  *  _ip_deregister(ipinfo,proto)
  *
  *  Deregister an IP protocol.
  *
  *  Input parameters:
  *	 ipinfo - IP stack information
  *	 proto - protocol number
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void _ip_deregister(ip_info_t *ipinfo, int proto)
{
	int idx;

	for (idx = 0; idx < IP_MAX_PROTOCOLS; idx++) {
		if (ipinfo->ip_protocols[idx].protocol == (uint8_t) proto) {
			ipinfo->ip_protocols[idx].protocol = 0;
			ipinfo->ip_protocols[idx].ref = 0;
			ipinfo->ip_protocols[idx].cb = NULL;
		}
	}
}

int sprintf_ip(char *buf, uint8_t *ipaddr)
{
	return sprintf(buf, "%u.%u.%u.%u",
			ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
}
