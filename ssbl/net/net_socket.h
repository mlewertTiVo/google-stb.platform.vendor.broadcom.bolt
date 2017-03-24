/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __NET_SOCKET_H__
#define __NET_SOCKET_H__
#include <stdint.h>
#include "lib_string.h"
#include "lib_printf.h"
#include "net_ebuf.h"
#include "net_api.h"
#include "net_ether.h"
#include "net_ip.h"

#define INADDR_ANY 0 /* Don't think we need to worry about this for BOLT */

#define NI_MAXHOST      1025
#define NI_MAXSERV      32

#define htons(x) x
#define ntohs(x) x
#define htonl(x) x



#define exit(x) err_msg("***** CoAP had a fatal error!!! *****\n")
#define assert(x) do { if (0 == (x)) \
	    err_msg("%s: assertting!!  %s:%d\n",\
		__func__, __FILE__, __LINE__); \
		} while (0)

#define malloc(x) KMALLOC(x, 0)
#define free(x) KFREE(x)
#define snprintf lib_snprintf

#define	IN_CLASSD(a)		((((long int) (a)) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(a)		IN_CLASSD(a)

#define IN6_IS_ADDR_MULTICAST(a) (((const uint8_t *) (a))[0] == 0xff)

#define UINT_MAX	4294967295U

/* For setsockopt(2) */
#define SOL_SOCKET      1
#define SO_REUSEADDR    2

/* for multicast */
/* Dummy protocol for TCP               */
#define IPPROTO_IP			0
#define IP_ADD_MEMBERSHIP               35
#define IP_DROP_MEMBERSHIP              36

/* Supported address families. */
#define AF_INET         2       /* Internet IP Protocol         */
#define AF_INET6	10	/* IP version 6			*/

typedef int ssize_t;
typedef unsigned int size_t;
typedef uint32_t in_addr_t;


/* Types of sockets.  */
enum __socket_type {
	SOCK_DGRAM = 2,               /* Connectionless, unreliable datagrams
	of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
};

typedef uint32_t socklen_t;

/* All pointers to socket address structures are often cast to pointers */
/* to this type before use in various functions and system calls: */

struct sockaddr {
	unsigned short    sa_family;    /* address family, AF_xxx */
	char              sa_data[14];  /* 14 bytes of protocol address */
};


/* IPv4 AF_INET sockets: */

struct in_addr {
	uint32_t s_addr;          /* load with inet_pton() */
};

struct sockaddr_in {
	short            sin_family;   /* e.g. AF_INET, AF_INET6 */
	unsigned short   sin_port;     /* e.g. htons(3490) */
	struct in_addr   sin_addr;     /* see struct in_addr, below */
	char             sin_zero[8];  /* zero this if you want to */
};


/* IPv6 AF_INET6 sockets: */

struct in6_addr {
	unsigned char   s6_addr[16];   /* load with inet_pton() */
};

struct sockaddr_in6 {
	uint16_t        sin6_family;   /* address family, AF_INET6 */
	uint16_t        sin6_port;     /* port number, Network Byte Order */
	uint32_t        sin6_flowinfo; /* IPv6 flow information */
	struct in6_addr sin6_addr;     /* IPv6 address */
	uint32_t        sin6_scope_id; /* Scope ID */
};

/* Request struct for multicast socket ops */

struct ip_mreq  {
	struct in_addr imr_multiaddr;   /* IP multicast address of group */
	struct in_addr imr_interface;   /* local IP address of interface */
};

/* General socket address holding structure, big enough to hold either */
/* struct sockaddr_in or struct sockaddr_in6 data: */

typedef unsigned short int sa_family_t;

/*
 *  Desired design of maximum size and alignment.
 */
#define _SS_MAXSIZE 128
	/* Implementation-defined maximum size. */
#define _SS_ALIGNSIZE (sizeof(int64_t))
	/* Implementation-defined desired alignment. */


/*
 *  Definitions used for sockaddr_storage structure paddings design.
 */
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof(sa_family_t) + \
						_SS_PAD1SIZE + _SS_ALIGNSIZE))

struct sockaddr_storage {
	sa_family_t  ss_family;     /* address family */

	/* all this is padding, implementation specific, ignore it: */
	char      __ss_pad1[_SS_PAD1SIZE];
	int64_t   __ss_align;
	char      __ss_pad2[_SS_PAD2SIZE];
};



/* helper functions */
uint32_t uint8to32(uint8_t src[]);
void uint32to8(uint8_t dest[], uint32_t src);


/* POSIX functions */
int socket(int domain, int type, int protocol);
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
int setsockopt(int socket, int level, int option_name, const void *option_value,
		socklen_t option_len);
int close(int fildes);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
		struct sockaddr *address, socklen_t *address_len);

in_addr_t inet_addr(char *cp);
char *inet_ntoa(struct in_addr in);
#endif
