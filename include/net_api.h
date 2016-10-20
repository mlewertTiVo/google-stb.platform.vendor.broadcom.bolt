/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __NET_API_H__
#define __NET_API_H__

#include <stdbool.h>

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#ifndef IP_ADDR_LEN
#define IP_ADDR_LEN 4
#endif

/*  *********************************************************************
    *  DHCP Protocol
    ********************************************************************* */

typedef struct dhcpreply_s {
	uint8_t dr_ipaddr[IP_ADDR_LEN];
	uint8_t dr_netmask[IP_ADDR_LEN];
	uint8_t dr_gateway[IP_ADDR_LEN];
	uint8_t dr_nameserver[IP_ADDR_LEN];
	uint8_t dr_dhcpserver[IP_ADDR_LEN];
	uint8_t dr_bootserver[IP_ADDR_LEN];
	uint8_t dr_dhcpserver_hwaddr[ENET_ADDR_LEN];
	char *dr_hostname;
	char *dr_domainname;
	char *dr_bootfile;
	char *dr_rootpath;
	char *dr_swapserver;
	char *dr_script;
	char *dr_options;
} dhcpreply_t;

int dhcp_bootrequest(dhcpreply_t ** reply);
void dhcp_free_reply(dhcpreply_t * reply);
void dhcp_set_envvars(dhcpreply_t * reply);

/*  *********************************************************************
    *  IP Layer
    ********************************************************************* */

void ip_uninit(void);
int ip_init(char *, uint8_t *);
ebuf_t *ip_alloc(void);
void ip_free(ebuf_t * buf);
int ip_send(ebuf_t * buf, uint8_t * destaddr, uint8_t proto);
uint16_t ip_chksum(uint16_t initchksum, uint8_t * ptr, int len);

int sprintf_ip(char *buf, uint8_t *ipaddr);

/*  *********************************************************************
    *  UDP Layer
    ********************************************************************* */

ebuf_t *udp_alloc(void);
void udp_free(ebuf_t * buf);

int udp_socket(uint16_t port);
int udp_bind(int portnum, uint16_t port);
int udp_connect(int portnum, uint16_t port);
void udp_close(int portnum);
int udp_send(int portnum, ebuf_t * buf, uint8_t * dest);
ebuf_t *udp_recv_with_timeout(int portnum, int seconds);
ebuf_t *udp_recv(int portnum);

/*  *********************************************************************
    *  TCP Layer
    ********************************************************************* */

#if CFG_TCP
#ifndef TCPFLG_NODELAY		/* XXX should be kept in sync with net_tcp.h */
#define TCPFLG_NODELAY	1	/* disable nagle */
#define TCPFLG_NBIO	2	/* non-blocking I/O */

#define TCPSTATUS_NOTCONN	0
#define TCPSTATUS_CONNECTING	1
#define TCPSTATUS_CONNECTED	2
#endif
int tcp_socket(void);
void tcp_destroy(int portnum);
int tcp_connect(int s, uint8_t * dest, uint16_t port);
int tcp_close(int s);
int tcp_send(int s, uint8_t * buf, int len);
int tcp_recv(int s, uint8_t * buf, int len);
int tcp_bind(int s, uint16_t port);
int tcp_peeraddr(int s, uint8_t * addr, uint16_t * port);
int tcp_listen(int s, uint16_t port);
int tcp_status(int s, int *connflag, int *rxready, int *rxeof);
int tcp_debug(int s, int arg);
int tcp_setflags(int s, unsigned int flags);
int tcp_getflags(int s, unsigned int *flags);
#endif 


/*  *********************************************************************
    *  ARP Layer
    ********************************************************************* */

uint8_t *arp_lookup(uint8_t * destip);
void arp_add(uint8_t * destip, uint8_t * desthw);
int arp_enumerate(int entrynum, uint8_t * ipaddr, uint8_t * hwaddr);
int arp_delete(uint8_t * ipaddr);

/*  *********************************************************************
    *  Network Configuration
    ********************************************************************* */

#ifndef NET_IPADDR
#define NET_IPADDR	0
#define NET_NETMASK	1
#define NET_GATEWAY	2
#define NET_NAMESERVER	3
#define NET_HWADDR	4
#define NET_DOMAIN	5
#define NET_HOSTNAME	6
#define NET_SPEED	7
#define NET_LOOPBACK	8
#endif
#define NET_DEVNAME	10

int net_getlinkstatus(void);
uint8_t *net_getparam(int param);
int net_setparam(int param, uint8_t * ptr);
int net_init(char *devname);
int net_unregister(const char *devname);
void net_uninit(void);
void net_setnetvars(void);

/*  *********************************************************************
    *  DNS
    ********************************************************************* */

int dns_lookup(char *hostname, uint8_t * ipaddr);

/*  *********************************************************************
    *  ICMP
    ********************************************************************* */

int icmp_ping(uint8_t * dest, int seq, int len);

/*  *********************************************************************
    *  DHCP
    ********************************************************************* */
int do_dhcp_request(char *devname);

#endif /* __NET_API_H__ */

