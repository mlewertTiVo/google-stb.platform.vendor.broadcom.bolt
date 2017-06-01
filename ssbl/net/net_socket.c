/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "net_socket.h"

#include <arch_ops.h>
#include <error.h>
#include <fileops.h>
#include <lib_ctype.h>
#include <lib_malloc.h>
#include <lib_types.h>

#define DYNAMIC_UDP_PORTBASE 49152
#define DYNAMIC_UDP_PORTMAX  65535
#define DYNAMIC_UDP_NUM_PORTS (DYNAMIC_UDP_PORTMAX - DYNAMIC_UDP_PORTBASE)

static inline unsigned short
get_random_port() {
	return (DYNAMIC_UDP_PORTBASE +
		(arch_getticks() % DYNAMIC_UDP_NUM_PORTS));
}

static int sockopt_to_boltparam(int option_name);

/**********************************************************************
  *  socket()
  *
  *  The socket() function creates an unbound socket in a
  *  communications domain, and returns a file descriptor
  *  that can be used in later function calls that operate on sockets..
  *
  *  Input parameters:
  *	 domain:Specifies the communications domain in
  *  which a socket is to be created.
  *	 type:Specifies the type of socket to be created.
  *	 protocol:Specifies a particular protocol to be used with the socket.
  *  Specifying a protocol of 0 causes socket() to use an
  *  unspecified default protocol
  *  appropriate for the requested socket type.
  *
  *  Return value:
  *	 Upon successful completion, socket() returns a nonnegative
  *  integer, the socket file descriptor.
  *	 Otherwise a value of BOLT_ERR is returned and
  *  errno is set to indicate the error.
  **********************************************************************/
int socket(int domain, int type, int protocol)
{
	int sockfd = 0;

	/* we only support IP4v */
	if (domain != AF_INET)
		return BOLT_ERR;


	/* we only support the default protocol */
	if (protocol != 0)
		return BOLT_ERR;


	/* we only support UDP sockets */
	switch (type) {
	case SOCK_DGRAM:
		/* create UDP socket */
		/* the port with be set when you bind/connect */
		sockfd =  udp_socket(0);
		break;
	default:
		return BOLT_ERR;
	};

	return sockfd;
}

/**********************************************************************
  *  bind()
  *
  *  The bind() function assigns an address to an unnamed socket.
  *  Sockets created with socket() function are initially unnamed;
  *  they are identified only by their address family.
  *
  *  Input parameters:
  *	 socket:Specifies the file descriptor of the socket to be bound.
  *	 address:Points to a sockaddr structure containing the
  *  address to be bound to the socket.
  *	 the length and format of the address depend on the
  *  address family of the socket.
  *	 address_len: Specifies the length of the sockaddr structure pointed
  *  to by the address argument.
  *
  *  Return value:
  *	 Upon successful completion, bind() returns BOLT_OK or 0.
  *	 Otherwise, BOLT_ERR is returned and errno is set to indicate the error.
  **********************************************************************/
int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	int result;

	if ((NULL == address) || (sizeof(struct sockaddr) != address_len))
		return BOLT_ERR;

	/* we only support IP4v */
	switch (address->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *addr = (struct sockaddr_in *)address;

		if (0 == addr->sin_port)
			addr->sin_port = get_random_port();

		result = udp_bind(socket, addr->sin_port);
		break;
	}
	default:
		err_msg("%s: error family 0x%x not supported!\n",
			__func__, address->sa_family);
		return BOLT_ERR;
	}
	return result;
}
/**********************************************************************
  *  setsockopt()
  *
  *  The setsockopt() function sets the option specified by the
  *  option_name argument, the protocol level specified by the
  *  level argument, to the value pointed to by the option_value argument
  *  for the socket associated with the file descriptor
  *  specified by the socket argument.
  *
  *  Input parameters:
  *	 socket:Specifies the file descriptor of the socket
  *	 level:Argument specifies the protocol level at which the option resides
  *  level values are specified in net_socet.h
  *	 option_name: Argument specifies a single option to set.
  *	 option_name values are specified in net_socet.h
  *	 option_value:The pointer to option data
  *	 option_len:The length of the option data
  *  Return value:
  *  Upon successful completion, setsockopt() returns BOLT_OK or 0.
  *  Otherwise, BOLT_ERR is returned and errno is set to indicate the error.
  **********************************************************************/
int setsockopt(int socket, int level, int option_name,
			const void *option_value, socklen_t option_len)
{
	switch (option_name) {
	case IP_ADD_MEMBERSHIP:
	case IP_DROP_MEMBERSHIP:
	{
		uint8_t group_addr[IP_ADDR_LEN];
		struct ip_mreq *group = (struct ip_mreq *) option_value;
		int bolt_param;

		if ((IPPROTO_IP != level) || (NULL == option_value))
			return BOLT_ERR;


		/* convert destination IP address */
		uint32to8(group_addr, group->imr_multiaddr.s_addr);

		/* convert sockopt option name to BOLT param */
		bolt_param = sockopt_to_boltparam(option_name);
		net_setparam(bolt_param, group_addr);
		break;
	}

	default:
	break;
	};

	return BOLT_OK;
}

/**********************************************************************
  *  close()
  *
  *  The close() call shuts down the socket associated
  *  with the socket descriptor s and frees
  *  resources allocated to the socket.
  *
  *  Input parameters:
  *	 socket:Descriptor of the socket to be closed

  *  Return value:
  *  Upon successful completion, close() returns BOLT_OK or 0.
  *  Otherwise, BOLT_ERR is returned and errno is set to indicate the error.
  **********************************************************************/

int close(int fildes)
{
	udp_close(fildes);
	return BOLT_OK;
}

/**********************************************************************
  *  sendto()
  *
  *  The sendto() call sends datagrams on the socket with descriptor sockfd.
  *  The sendto() call applies to any datagram socket,
  *  whether connected or unconnected
  *
  *  Input parameters:
  *	 sockfd:Specifies the file descriptor of the socket
  *	 buf:Points to the buffer containing the message to be transmitted.
  *	 len:Length of the message in the buffer pointed to by msg.
  *	 flags:A parameter that can be set to 0
  *	 dest_addr:Address of the target.
  *	 addrlen:Size of the structure pointed to by to.
  *  Return value:
  *  If successful, the number of characters sent is returned.
  *  The value BOLT_ERR indicates an error.
  **********************************************************************/
ssize_t sendto(int sockfd, const void *buf, size_t len,
			int flags, const struct sockaddr *dest_addr,
				socklen_t addrlen)
{
	ebuf_t *ebuf;
	int bytes_sent = -1;
	uint8_t host_addr[IP_ADDR_LEN];
	/* Assuming IP4v only */
	struct sockaddr_in *server_addr = (struct sockaddr_in *) dest_addr;

	/* convert destination IP address */
	uint32to8(host_addr, server_addr->sin_addr.s_addr);

	/* set te destination port */
	if (0 != udp_connect(sockfd, server_addr->sin_port)) {
		/* udp_bind failed! */
		err_msg("%s: udp_bind failed!\n", __func__);
	}

	/* Add data */
	if (len > IP_MAX_PKT) {
		/* this is bigger than the available buffer!!! */
		err_msg("%s: %u is too big for buffer!\n", __func__, len);
		return BOLT_ERR;
	}

	/* allocate an ebuf_t */
	ebuf = udp_alloc();

	/* convert the passed in buffer to ebuf_t */
	ebuf_append_bytes(ebuf, buf, len);

	/* send the ebuf_t buffer */
	/* it will get freed after it's sent */
	if (0 == udp_send(sockfd, ebuf, host_addr)) {
		/* if it was successful assume that we sent the whole buffer */
		bytes_sent = len;
	}

	return bytes_sent;
}
/**********************************************************************
  *  recvfrom()
  *
  *  The recvfrom() call receives data on a socket by name
  *  with descriptor socket   and stores it in a buffer.
  *
  *  Input parameters:
  *	 sockfd:Specifies the file descriptor of the socket
  *	 buffer:Pointer to the buffer to receive the data.
  *	 length:Length in bytes of the buffer pointed to by buffer
  *	 flags:A parameter that can be set to 0
  *	 address:Points to a socket address structure from
  *  which data is received.
  *	 address_len:Points to the size of name in bytes.
  *  Return value:
  *  If successful, the number of characters recevied is returned.
  *  The value BOLT_ERR indicates an error.
   *   the value of BOLT_OK or 0 indicates connection is closed
**********************************************************************/
ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
		struct sockaddr *address, socklen_t *address_len)
{
	size_t bytes_read = 0;

	/* Assuming IP4v only */
	struct sockaddr_in *client_addr = (struct sockaddr_in *) address;

	/* Get a UDP buffer */
	ebuf_t *ebuf = udp_recv(socket);

	if (NULL == ebuf) {
		/* printf("%s: no buffer available!\n", __func__); */
		return BOLT_OK;
	}

	bytes_read = ebuf_length(ebuf);

	if (bytes_read > length) {
		err_msg("%s: read more bytes than can fit in buffer so truncating..,\n", __func__);
		bytes_read = length;
	}

	/* Copy from ebuf_t to the user buffer */
	ebuf_get_bytes(ebuf, buffer, bytes_read);

	if (client_addr) {
		client_addr->sin_port = (unsigned short)ebuf->eb_usrdata;
		client_addr->sin_addr.s_addr = uint8to32(ebuf->eb_usrptr);
	}
	/* Free the UDP buffer */
	udp_free(ebuf);

	return bytes_read;
}
/**********************************************************************
  *  inet_addr()
  *
  *  Construct internet address from character string
  *
  *  Input parameters:
  *	 addr:A character string in standard dotted decimal notation.
  *  Return value:
  *  If successful,the internet address is returned in network byte order.
  *  The value BOLT_ERR indicates an error.
**********************************************************************/
in_addr_t inet_addr(char *addr)
{
	int res;
	uint8_t addr_uint8[IP_ADDR_LEN];

	if (isdigit(*addr)) {
		if (parseipaddr(addr, addr_uint8) < 0) {
			err_msg("%s: invalid IP address: %s\n", __func__, addr);
			return BOLT_ERR;
		}
	} else {
		res = dns_lookup(addr, addr_uint8);
		if (res < 0) {
			err_msg("%s: could not resolve IP address of host %s",
				__func__, addr);
			return BOLT_OK;
		}
	}
	return uint8to32(addr_uint8);
}
/**********************************************************************
  *  inet_ntoa()
  *
  *  Return pointer to a string in dotted decimal notation
  *
  *  Input parameters:
  *	 in:The host Internet address.
  *  Return value:
  *  This function returns a pointer to a string
  *  expressed in dotted decimal notation.
**********************************************************************/
char *inet_ntoa(struct in_addr in)
{
	static char str[20];
	uint8_t bolt_ip[IP_ADDR_LEN];

	uint32to8(bolt_ip, in.s_addr);
	sprintf_ip(str, bolt_ip);

	return str;
}
static int sockopt_to_boltparam(int option_name)
{
	switch (option_name) {
	case IP_ADD_MEMBERSHIP:
		return NET_ADD_GROUP;
	case IP_DROP_MEMBERSHIP:
		return NET_DROP_GROUP;
	default:
		return BOLT_ERR;
	}
}
/**********************************************************************
  *  uint8to32()
  *
  *  helper functions to convert
  *  unsigned char array to unsigned int
  *  Input parameters:
  *	 src:pointer to unsigned char array.
  *  Return value:
  *  returns unsigned integer value depends on endianness
**********************************************************************/
uint32_t uint8to32(uint8_t src[])
{
	int byte;
	uint32_t dest = 0;

#if defined __BIG_ENDIAN
	for (byte = 0; byte < 4; byte++) {
		dest = dest << 8;
		dest |= src[byte];
	}
#else
	for (byte = 3; byte >= 0; byte--) {
		dest = dest << 8;
		dest |= src[byte];
	}
#endif
	return dest;
}
/**********************************************************************
  *  uint32to8()
  *
  *  helper functions to convert
  *  unsigned integer value to unsigned char
  *  Input parameters:
  *	src:unsigned integer value.
  *  Output parameters:
  *	dest:pointer to unsigned char array.
  *	returns unsigned integer value depends on endianness
**********************************************************************/
void uint32to8(uint8_t dest[], uint32_t src)
{
#if defined __BIG_ENDIAN
	dest[0] = src >> 24;
	dest[1] = src >> 16;
	dest[2] = src >>  8;
	dest[3] = src;
#else /* Little endian*/
	dest[0] = src;
	dest[1] = src >>  8;
	dest[2] = src >> 16;
	dest[3] = src >> 24;
#endif
}
