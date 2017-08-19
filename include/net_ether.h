/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __NET_ETHER_H__
#define __NET_ETHER_H__

#include <net_ebuf.h>

#include <stdbool.h>
#include <stdint.h>

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define ETH_PTYPE_NONE 0
#define ETH_PTYPE_DIX	1
#define ETH_PTYPE_802SAP	2
#define ETH_PTYPE_802SNAP	3

#define ETH_KEEP	1
#define ETH_DROP	0

/* Maximum specified link time for 10/100 is less than 4 seconds,
 * although PHY's occasionally cycle through auto-negotiation
 * more than once, taking a few more seconds.
 *
 * 1000BASE-T requires 3 extra NEXT PAGE exchanges, which lengthens
 * auto-negotiation by ~ 1 second. Each page adds 336 milliseconds.
 *
 * It is still possible that auto-negotiation extends indefinitely
 * because PHY's are allowed to exchange additional NEXT PAGES.
 *
 * 8 (eight) seconds should be enough. If not, increase the following
 * #define'd value or provide an environment variable to Ethernet
 * driver so that it can be tuned without rebuilding.
 */
#define ETH_ANEG_TIMEOUT 8

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
void eth_setmulticast_hwaddr(ether_info_t *eth, uint8_t *hwaddr);
void eth_unsetmulticast_hwaddr(ether_info_t *eth, uint8_t *hwaddr);
bool eth_getlinkstatus(ether_info_t *, int *linkstatus);
int eth_getspeed(ether_info_t *, int *speed);
int eth_setspeed(ether_info_t *, int speed);
int eth_getloopback(ether_info_t *, int *loop);
int eth_setloopback(ether_info_t *, int loop);
extern const uint8_t eth_broadcast[ENET_ADDR_LEN];

#endif /* __NET_ETHER_H__ */

