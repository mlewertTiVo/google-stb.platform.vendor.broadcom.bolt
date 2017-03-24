/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __NET_SSDP_H__
#define __NET_SSDP_H__

#ifndef X86
#include "net_socket.h"
#endif

/******************************************************************************
 * SSDP specifics
 *****************************************************************************/
#define MULTICAST_ADDR			"239.255.255.250"
#define MULTICAST_PORT			1900

#define SSDP_MAX_AGE_MIN                1800
#define SSDP_MAX_AGE_DEFAULT            SSDP_MAX_AGE_MIN

/* start-line: every SSDP message contains
exactly one of the following start-line: */
#define STARTLINE_NOTIFY		"NOTIFY * HTTP/1.1\r\n"
#define STARTLINE_MSEARCH		"M-SEARCH * HTTP/1.1\r\n"
#define STARTLINE_RESPONSE		"HTTP/1.1 200 OK\r\n"

/* SSDP request types */
#define SSDP_MSEARCH		1
#define SSDP_NOTIFY		2
#define SSDP_RESPONSE		3

#define MAX_SSDP_HEADER_VAL_LEN		256

/******************************************************************************
 * SSDP device descriptor
 *****************************************************************************/
#define MAX_OS_NAME_LEN                 16
#define MAX_OS_VERSION_LEN              16
#define MAX_PROD_NAME_LEN               16
#define MAX_PROD_VERSION_LEN            16
/* 32 bit UUID plus 4 '-'s to seperate plus terminator */
#define MAX_UUID_LEN                    37
#define MAX_SEARCH_TARGET_LEN           256
#define MAX_NUM_SEARCH_TARGETS          4

typedef struct ssdp_device_desc_s {
	char os[MAX_OS_NAME_LEN];
	char os_version[MAX_OS_VERSION_LEN];
	char prod[MAX_PROD_NAME_LEN];
	char prod_version[MAX_PROD_VERSION_LEN];
	char uuid[MAX_UUID_LEN];
} ssdp_device_desc_t;

/******************************************************************************
 * SSDP search target
 *****************************************************************************/

#define MAX_ST_NAME_LEN				256
#define UUID_PREFIX					"uuid:"
/* add 2 for "::" to seperate */
#define MAX_USN_LEN			(MAX_UUID_LEN + MAX_ST_NAME_LEN + 2)


/* SSDP notify type */
#define SSDP_NOTIFY_ALIVE		21
#define SSDP_NOTIFY_BYEBYE		22
#define SSDP_NOTIFY_UPDATE		23

typedef struct ssdp_st_s ssdp_st_t;
typedef struct ssdp_st_s {
	char			st_name[MAX_ST_NAME_LEN];
	uint32_t		max_age;
	ssdp_st_t		*st_next;
} ssdp_st_t;

/******************************************************************************
 * SSDP message
 *****************************************************************************/

#define MAX_SSDP_MSG_LEN		1024

typedef struct ssdp_msg_s ssdp_msg_t;
typedef struct ssdp_msg_s {
	char					data[MAX_SSDP_MSG_LEN];
	int						length;
	uint32_t				timestamp;
	struct sockaddr_in		dst_addr;
	ssdp_msg_t				*next;
} ssdp_msg_t;


/******************************************************************************
 * SSDP context
 *****************************************************************************/

typedef struct ssdp_context_s {
	int						sockfd;
	ssdp_st_t				*st_list;
	uint32_t				max_age;
	int64_t					st_timer;
	ssdp_device_desc_t		device;
	struct sockaddr_in		mcast_addr;
	struct ip_mreq			group;
} ssdp_context_t;

/******************************************************************************
 * Functions
 *****************************************************************************/

int ssdp_init(ssdp_context_t *ctx);
void ssdp_poll(void *arg);

#endif
