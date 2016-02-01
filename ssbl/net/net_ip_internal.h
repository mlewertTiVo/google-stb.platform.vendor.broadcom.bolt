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

/**********************************************************************
  *  ARP Protocol
  **********************************************************************/

#define ARP_OPCODE_REQUEST	1
#define ARP_OPCODE_REPLY	2

#define ARP_HWADDRSPACE_ETHERNET 1

#define PROTOSPACE_IP        0x0800
#define PROTOSPACE_ARP	     0x0806

#define ARP_KEEP_TIMER          60
#define ARP_QUERY_TIMER		1
#define ARP_QUERY_RETRIES	4
#define ARP_TXWAIT_MAX		2
#define ARP_TABLE_SIZE		8

typedef enum { ae_unused, ae_arping, ae_established } arpstate_t;

typedef struct arpentry_s {
	arpstate_t ae_state;
	int ae_usage;
	int ae_timer;
	int ae_retries;
	int ae_permanent;
	uint8_t ae_ipaddr[IP_ADDR_LEN];
	uint8_t ae_ethaddr[ENET_ADDR_LEN];
	queue_t ae_txqueue;
} arpentry_t;

/**********************************************************************
  *  IP Protocol
  **********************************************************************/

#define IPHDR_VER_4	0x40
#define IPHDR_LEN_20	0x05
#define IPHDR_LENGTH	20
#define IPHDR_TOS_DEFAULT 0x00
#define IPHDR_TTL_DEFAULT 100

#define IPHDR_RESERVED 0x8000
#define IPHDR_DONTFRAGMENT 0x4000
#define IPHDR_MOREFRAGMENTS 0x2000
#define IPHDR_FRAGOFFSET 0x01FFF

typedef struct ip_protodisp_s {
	uint8_t protocol;
	int (*cb) (void *ref, ebuf_t *buf, uint8_t *dst, uint8_t *src);
	void *ref;
} ip_protodisp_t;

#define IP_MAX_PROTOCOLS	4

struct ip_info_s {
	net_info_t net_info;

	/* Ethernet info */
	ether_info_t *eth_info;

	/* Info specific to IP */
	uint16_t ip_id;
	int ip_port;

	/* IP protocol dispatch table */
	ip_protodisp_t ip_protocols[IP_MAX_PROTOCOLS];

	/* Info specific to ARP */
	arpentry_t *arp_table;
	int arp_port;
	uint8_t arp_hwaddr[ENET_ADDR_LEN];

	uint8_t *packet_assembly_buffer;
	uint16_t first_hole;
	uint16_t total_len;
	uint16_t assembly_identification;
	uint16_t assembly_proto;
	uint32_t assembly_ip;
};
