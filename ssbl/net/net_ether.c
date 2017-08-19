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

#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "error.h"

#include "net_ebuf.h"
#include "net_ether.h"

#include <stdbool.h>

/**********************************************************************
  *  Constants
  **********************************************************************/

#define ETH_MAX_PORTS	4
#define ETH_MAX_BUFFERS	8

/**********************************************************************
  *  Types
  **********************************************************************/

typedef struct ether_port_s {
	int ep_dev;
	uint8_t ep_proto[8];
	int ep_ptype;
	int ep_mtu;
	int (*ep_rxcallback) (ebuf_t *buf, void *ref);
	void *ep_ref;
} ether_port_t;

struct ether_info_s {
	ether_port_t *eth_ports;
	queue_t eth_freelist;
	uint8_t eth_hwaddr[6];
	uint8_t multicast_hwaddr[6];
	int eth_devhandle;
	ebuf_t *eth_bufpool;
};

/**********************************************************************
  *  Globals
  **********************************************************************/

const uint8_t eth_broadcast[ENET_ADDR_LEN] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/**********************************************************************
  *  eth_open(eth,ptye,pdata,cb)
  *
  *  Open an Ethernet portal.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 ptype - protocol type (ETH_PTYPE_xxx)
  *	 pdata - protocol data (two bytes for DIX protocols)
  *	 cb - callback for receive packets
  *
  *  Return value:
  *	 portal number
  *	 or <0 if error occured
  **********************************************************************/
int eth_open(ether_info_t *eth, int ptype, char *pdata,
	     int (*cb) (ebuf_t *buf, void *ref), void *ref)
{
	ether_port_t *p;
	int portnum;

	p = eth->eth_ports;

	for (portnum = 0; portnum < ETH_MAX_PORTS; portnum++, p++) {
		if (p->ep_rxcallback == NULL)
			break;
	}

	if (portnum == ETH_MAX_PORTS)
		return BOLT_ERR_NOHANDLES;	/* no ports left */

	switch (ptype) {
	case ETH_PTYPE_DIX:
		p->ep_proto[0] = pdata[0];
		p->ep_proto[1] = pdata[1];
		p->ep_mtu = ENET_MAX_PKT - ENET_DIX_HEADER;
		break;

	case ETH_PTYPE_802SAP:
	case ETH_PTYPE_802SNAP:
	default:
		/*
		 * we only support DIX etypes right now. If we ever want to
		 * support on-IP stacks (unlikely) this will need to change.
		 */
		return BOLT_ERR_UNSUPPORTED;
	}

	p->ep_ptype = ptype;
	p->ep_rxcallback = cb;
	p->ep_dev = eth->eth_devhandle;
	p->ep_ref = ref;

	return portnum;
}

/**********************************************************************
  *  eth_close(eth,port)
  *
  *  Close an Ethernet portal, freeing resources allocated to it.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 port - portal number
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_close(ether_info_t *eth, int port)
{
	ether_port_t *p = &(eth->eth_ports[port]);

	p->ep_ptype = 0;
	p->ep_rxcallback = NULL;
	p->ep_dev = 0;
	memset(&(p->ep_proto[0]), 0, sizeof(p->ep_proto));
}

/**********************************************************************
  *  eth_findport(eth,buf)
  *
  *  Locate the portal associated with a particular Ethernet packet.
  *  Parse the packet enough to determine if it's addressed
  *  correctly and to a valid protocol, and then look up the
  *  corresponding portal.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 buf - ethernet buffer to check
  *
  *  Return value:
  *	 eth_port_t structure or NULL if packet should be dropped
  **********************************************************************/
static ether_port_t *eth_findport(ether_info_t *eth, ebuf_t *buf)
{
	int idx;
	ether_port_t *p;

	/* A few pre-flight checks: packets *from* multicast addresses
	 * are not allowed.
	 */

	if (buf->eb_ptr[6] & 1)
		return NULL;

	/* Packets smaller than minimum size are not allowed.
	 */

	if (buf->eb_length < ENET_ZLEN)
		return NULL;

	/* Packets with bad status are not allowed
	 */

	/* XXX if (buf->eb_status != 0) return NULL; */

	/* Okay, scan the port list and find the matching portal.
	 */

	for (idx = 0, p = eth->eth_ports; idx < ETH_MAX_PORTS; idx++, p++) {
		if (!p->ep_rxcallback)
			continue;	/* port not in use */

		switch (p->ep_ptype) {
		case ETH_PTYPE_DIX:
			if ((p->ep_proto[0] == buf->eb_ptr[12]) &&
			    (p->ep_proto[1] == buf->eb_ptr[13])) {
				ebuf_skip(buf, ENET_DIX_HEADER);
				return p;
			}
			break;
		case ETH_PTYPE_802SAP:
		case ETH_PTYPE_802SNAP:
		default:
			break;
		}
	}

	return NULL;
}

/**********************************************************************
  *  eth_poll(eth)
  *
  *  Poll devices and process inbound packets.  If new packets arrive,
  *  call the appropriate callback routine.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_poll(ether_info_t *eth)
{
	ebuf_t *buf;
	ether_port_t *p;
	int res;

	/* XXX should this loop until all packets are processed? */

	/* If no packets, just get out now
	 */

	if (bolt_inpstat(eth->eth_devhandle) == 0)
		return;

	/* get a packet from the free list
	 */

	buf = (ebuf_t *) q_deqnext(&(eth->eth_freelist));
	if (!buf)
		return;

	/* Receive network data into the packet buffer
	 */

	ebuf_init_rx(buf);
	res = bolt_read(eth->eth_devhandle, buf->eb_ptr, ENET_MAX_PKT);

	/* if receive error, get out now.
	 */

	if (res <= 0) {
		q_enqueue(&(eth->eth_freelist), (queue_t *) buf);
		return;
	}

	/* init the rest of the fields in the ebuf
	 */

	buf->eb_length = res;
	buf->eb_status = 0;
	buf->eb_device = eth;
	buf->eb_usrdata = 0;

	/* Look up the portal to receive the new packet
	 */

	p = eth_findport(eth, buf);

	/* Call the callback routine if we want to keep this
	 * buffer.  Otherwise, drop it on the floor
	 */

	if (p) {
		buf->eb_port = p - eth->eth_ports;
		res = (*(p->ep_rxcallback)) (buf, p->ep_ref);
		if (res == ETH_DROP)
			eth_free(buf);
	} else {
		eth_free(buf);
	}
}

/**********************************************************************
  *  eth_gethwaddr(eth,hwaddr)
  *
  *  Obtain the hardware address of the Ethernet interface.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 hwaddr - place to put hardware address - 6 bytes
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_gethwaddr(ether_info_t *eth, uint8_t *hwaddr)
{
	memcpy(hwaddr, eth->eth_hwaddr, ENET_ADDR_LEN);
}

/**********************************************************************
  *  eth_sethwaddr(eth,hwaddr)
  *
  *  Set the hardware address of the Ethernet interface.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 hwaddr - new hardware address - 6 bytes
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_sethwaddr(ether_info_t *eth, uint8_t *hwaddr)
{
	memcpy(eth->eth_hwaddr, hwaddr, ENET_ADDR_LEN);
	bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_SETHWADDR,
		  &(eth->eth_hwaddr[0]), sizeof(eth->eth_hwaddr), NULL, 0);

}
/**********************************************************************
  *  eth_setmulticast_hwaddr(eth,hwaddr)
  *
  *  Set the multicast address.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 hwaddr - new hardware address - 6 bytes
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_setmulticast_hwaddr(ether_info_t *eth, uint8_t *hwaddr)
{
	memcpy(eth->multicast_hwaddr, hwaddr, ENET_ADDR_LEN);
	bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_SETMULTICAST_HWADDR,
		&(eth->multicast_hwaddr[0]), sizeof(eth->multicast_hwaddr),
		NULL, 0);

}
/**********************************************************************
  *  eth_unsetmulticast_hwaddr(eth,hwaddr)
  *
  *  Unset the multicast address.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 hwaddr - new hardware address - 6 bytes
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_unsetmulticast_hwaddr(ether_info_t *eth, uint8_t *hwaddr)
{
	memcpy(eth->multicast_hwaddr, hwaddr, ENET_ADDR_LEN);
	bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_UNSETMULTICAST_HWADDR,
		&(eth->multicast_hwaddr[0]), sizeof(eth->multicast_hwaddr),
		NULL, 0);
}
/**********************************************************************
  *  eth_setspeed(eth,speed)
  *
  *  Set the speed of the Ethernet interface.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 speed - target speed (or auto for automatic)
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
int eth_setspeed(ether_info_t *eth, int speed)
{
	return bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_SETSPEED,
			 &speed, sizeof(speed), NULL, 0);

}

/**********************************************************************
  *  eth_setloopback(eth,loop)
  *
  *  Configure loopback mode options for the Ethernet
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 loop - loopback mode to set
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
int eth_setloopback(ether_info_t *eth, int loop)
{
	return bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_SETLOOPBACK,
			 &loop, sizeof(loop), NULL, 0);

}

/**********************************************************************
  *  eth_getlinkstatus(eth,linkstatus)
  *
  *  Get the current status for the Ethernet link
  *
  *  Input parameters:
  *     eth - ethernet context
  *     linkstatus - pointer to int to receive the status of link
  *
  *  Return value:
  *     true if linkstatus is valid
  *     false otherwise
  **********************************************************************/
bool eth_getlinkstatus(ether_info_t *eth, int *linkstatus)
{
	const int res = bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_GETLINK,
		linkstatus, sizeof(*linkstatus), NULL, 0);
	return (res == 0);
}

/**********************************************************************
  *  eth_getspeed(eth,speed)
  *
  *  Get the current setting for the Ethernet speed (note that this
  *  is the speed we want to achieve, not the current speed)
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 speed - pointer to int to receive speed
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
int eth_getspeed(ether_info_t *eth, int *speed)
{
	return bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_GETSPEED,
			 speed, sizeof(*speed), NULL, 0);

}

/**********************************************************************
  *  eth_getloopback(eth,loop)
  *
  *  Read the loopback state of the Ethernet interface
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 loop - pointer to int to receive loopback state
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
int eth_getloopback(ether_info_t *eth, int *loop)
{
	return bolt_ioctl(eth->eth_devhandle, IOCTL_ETHER_GETLOOPBACK,
			 loop, sizeof(*loop), NULL, 0);

}

/**********************************************************************
  *  eth_send(buf,dest)
  *
  *  Transmit a packet.
  *
  *  Input parameters:
  *	 buf - ebuf structure describing packet
  *	 dest - destination hardware address
  *
  *  Return value:
  *	 0 - no error
  *	 else error code
  **********************************************************************/
int eth_send(ebuf_t *buf, uint8_t *dest)
{
	ether_info_t *eth = buf->eb_device;
	ether_port_t *p = &(eth->eth_ports[buf->eb_port]);
	int res;

	switch (p->ep_ptype) {
	case ETH_PTYPE_DIX:
		ebuf_seek(buf, -ENET_DIX_HEADER);
		ebuf_put_bytes(buf, dest, ENET_ADDR_LEN);
		ebuf_put_bytes(buf, eth->eth_hwaddr, ENET_ADDR_LEN);
		ebuf_put_bytes(buf, p->ep_proto, 2);
		/* adjust pointer and add in DIX header length */
		ebuf_prepend(buf, ENET_DIX_HEADER);
		break;
	case ETH_PTYPE_802SAP:
	case ETH_PTYPE_802SNAP:
	default:
		eth_free(buf);	/* should not happen */
		return BOLT_ERR_UNSUPPORTED;
	}

	res = bolt_write(p->ep_dev, ebuf_ptr(buf), ebuf_length(buf));

	/* XXX - should we free buffers here? */

	return res;
}

/**********************************************************************
  *  eth_alloc(eth,port)
  *
  *  Allocate an Ethernet buffer.  Ethernet buffers know what
  *  ports they are associated with, since we need to reserve
  *  space for the EThernet header, which might vary in size
  *  for DIX, 802, etc.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 port - portal ID
  *
  *  Return value:
  *	 ebuf, or NULL if no ebufs left
  **********************************************************************/
ebuf_t *eth_alloc(ether_info_t *eth, int port)
{
	ebuf_t *buf;
	ether_port_t *p = &(eth->eth_ports[port]);

	buf = (ebuf_t *) q_deqnext(&(eth->eth_freelist));
	if (buf == NULL)
		return NULL;

	buf->eb_status = 0;
	buf->eb_port = port;
	buf->eb_device = eth;
	ebuf_init_tx(buf);

	switch (p->ep_ptype) {
	case ETH_PTYPE_NONE:
		break;
	case ETH_PTYPE_DIX:
		ebuf_seek(buf, ENET_DIX_HEADER);
		break;
	case ETH_PTYPE_802SAP:
	case ETH_PTYPE_802SNAP:
	default:
		/* XXX Other ether types here */
		break;
	}

	/* 'eb_ptr' points at new data, length is cleared.
	 * We will add the length back in at send time when the
	 * ethernet header is filled in.
	 */
	buf->eb_length = 0;

	return buf;
}

/**********************************************************************
  *  eth_free(buf)
  *
  *  Free an ebuf.
  *
  *  Input parameters:
  *	 buf - ebuf to free
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_free(ebuf_t *buf)
{
	ether_info_t *eth = buf->eb_device;

	q_enqueue(&(eth->eth_freelist), (queue_t *) buf);
}

/**********************************************************************
  *  eth_getmtu(eth,port)
  *
  *  Return the mtu of the specified Ethernet port.  The mtu
  *  is the maximum number of bytes you can put in the buffer,
  *  excluding the Ethernet header.
  *
  *  Input parameters:
  *	 eth - ethernet context
  *	 port - portal ID
  *
  *  Return value:
  *	 number of bytes
  **********************************************************************/
int eth_getmtu(ether_info_t *eth, int port)
{
	ether_port_t *p = &(eth->eth_ports[port]);

	return p->ep_mtu;
}

/**********************************************************************
  *  eth_init(devname)
  *
  *  Create an Ethernet context for a particular Ethernet device.
  *
  *  Input parameters:
  *	 devname - device name for underlying Ethernet driver
  *
  *  Return value:
  *	 ethernet context, or NULL of it could not be created.
  **********************************************************************/
ether_info_t *eth_init(char *devname)
{
	int idx;
	ebuf_t *buf;
	ether_info_t *eth;
	int devhandle;

	/* Open the device driver
	 */

	devhandle = bolt_open(devname);
	if (devhandle < 0)
		return NULL;

	eth = KMALLOC(sizeof(ether_info_t), 0);
	if (!eth) {
		bolt_close(devhandle);
		return NULL;
	}

	memset(eth, 0, sizeof(ether_info_t));

	/* Obtain hardware address
	 */

	bolt_ioctl(devhandle, IOCTL_ETHER_GETHWADDR, &(eth->eth_hwaddr[0]),
		  sizeof(eth->eth_hwaddr), NULL, 0);

	/* Allocate portal table
	 */

	eth->eth_ports = KMALLOC(ETH_MAX_PORTS * sizeof(ether_port_t), 0);
	if (!eth->eth_ports) {
		bolt_close(devhandle);
		KFREE(eth);
		return NULL;
	}

	memset(eth->eth_ports, 0, ETH_MAX_PORTS * sizeof(ether_port_t));

	/* Allocate buffer pool
	 */

	eth->eth_bufpool =
	    (ebuf_t *) KMALLOC(sizeof(ebuf_t) * ETH_MAX_BUFFERS, 0);
	if (!eth->eth_bufpool) {
		bolt_close(devhandle);
		KFREE(eth->eth_ports);
		KFREE(eth);
		return NULL;
	}

	/* Chain buffers onto the free list
	 */

	q_init(&(eth->eth_freelist));
	buf = eth->eth_bufpool;
	for (idx = 0; idx < ETH_MAX_BUFFERS; idx++) {
		q_enqueue(&(eth->eth_freelist), (queue_t *) buf);
		buf++;
	}

	/* Remember the device handle
	 */

	eth->eth_devhandle = devhandle;

	return eth;
}

/**********************************************************************
  *  eth_uninit(eth)
  *
  *  Close and free up an Ethernet context
  *
  *  Input parameters:
  *	 eth - ethernet context
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
void eth_uninit(ether_info_t *eth)
{
	bolt_close(eth->eth_devhandle);
	KFREE(eth->eth_bufpool);
	KFREE(eth->eth_ports);
	KFREE(eth);
}
