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

#ifndef __NET_ETHER_H__
#define __NET_ETHER_H__

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define ETH_PTYPE_NONE 0
#define ETH_PTYPE_DIX	1
#define ETH_PTYPE_802SAP	2
#define ETH_PTYPE_802SNAP	3

#define ETH_KEEP	1
#define ETH_DROP	0

/*  *********************************************************************
    *  types
    ********************************************************************* */

typedef struct ether_info_s ether_info_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

int eth_open(ether_info_t * eth, int ptype, char *pdata,
	     int (*cb) (ebuf_t * buf, void *ref), void *ref);
void eth_close(ether_info_t * eth, int port);
void eth_poll(ether_info_t * eth);
int eth_send(ebuf_t * buf, uint8_t * dest);
ebuf_t *eth_alloc(ether_info_t * eth, int port);
void eth_free(ebuf_t * buf);
ether_info_t *eth_init(char *devname);
void eth_uninit(ether_info_t * eth);
int eth_getmtu(ether_info_t *, int port);
void eth_gethwaddr(ether_info_t *, uint8_t * hwaddr);
void eth_sethwaddr(ether_info_t *, uint8_t * hwaddr);
int eth_getspeed(ether_info_t *, int *speed);
int eth_setspeed(ether_info_t *, int speed);
int eth_getloopback(ether_info_t *, int *loop);
int eth_setloopback(ether_info_t *, int loop);
extern const uint8_t eth_broadcast[ENET_ADDR_LEN];

#endif /* __NET_ETHER_H__ */

