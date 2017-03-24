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

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "fileops.h"

#include "net_ebuf.h"
#include "net_ether.h"

#include "bolt.h"

#include "loader.h"

#include "net_api.h"
#include "env_subr.h"

/**********************************************************************
  *  TFTP protocol
  **********************************************************************/

#define UDP_PROTO_TFTP		69

#define TFTP_DEFAULT_BLOCKSIZE	512
#define TFTP_MAX_BLOCKSIZE	16384
#define TFTP_MAX_BLKSIZE_STRLEN	6  /* note: theoretical max is 65464 bytes
				    * therefore this strlen should be good */

#define TFTP_OP_RRQ		1
#define TFTP_OP_WRQ		2
#define TFTP_OP_DATA		3
#define TFTP_OP_ACK		4
#define TFTP_OP_ERROR		5
#define TFTP_OP_OACK		6

#define TFTP_ERR_DISKFULL	3
#define TFTP_ERR_BLKSIZE	8

#define TFTP_MAX_RETRIES	8

#define TFTP_RRQ_TIMEOUT	1	/* seconds */
#define TFTP_RECV_TIMEOUT	1	/* seconds */

#define TFTP_BLOCK_SIZE_OPTION  "BLKSIZE"

/**********************************************************************
  *  TFTP context
  **********************************************************************/

typedef struct tftp_fsctx_s {
	int dummy;
} tftp_fsctx_t;

typedef struct tftp_info_s {
	int tftp_socket;
	uint8_t tftp_data[TFTP_MAX_BLOCKSIZE + 68];
	int tftp_block_size;
	int tftp_blklen;
	int tftp_blkoffset;
	int tftp_fileoffset;
	uint16_t tftp_blknum;
	uint8_t tftp_ipaddr[IP_ADDR_LEN];
	int tftp_lastblock;
	int tftp_error;
	int tftp_filemode;
	unsigned int negotiate_blksize;
} tftp_info_t;

/**********************************************************************
  *  Prototypes
  **********************************************************************/

static int tftp_fileop_init(void **fsctx, void *devicename);
static int tftp_fileop_open(void **ref, void *fsctx,
				const char *filename, int mode);
static int tftp_fileop_read(void *ref, uint8_t *buf, int len);
static int tftp_fileop_write(void *ref, uint8_t *buf, int len);
static int tftp_fileop_seek(void *ref, int offset, int how);
static void tftp_fileop_close(void *ref);
static void tftp_fileop_uninit(void *fsctx);

/**********************************************************************
  *  TFTP fileio dispatch table
  **********************************************************************/

const fileio_dispatch_t tftp_fileops = {
	.method    = "tftp",
	.loadflags = LOADFLG_NOBB,
	.init      = tftp_fileop_init,
	.open      = tftp_fileop_open,
	.read      = tftp_fileop_read,
	.write     = tftp_fileop_write,
	.seek      = tftp_fileop_seek,
	.close     = tftp_fileop_close,
	.uninit    = tftp_fileop_uninit,
};

static int _tftp_send_blksize_error(tftp_info_t *info, const char *reason)
{
	ebuf_t *buf;

	buf = udp_alloc();
	if (!buf)
		return -1;

	ebuf_append_u16_be(buf, TFTP_OP_ERROR);
	ebuf_append_u16_be(buf, TFTP_ERR_BLKSIZE);
	ebuf_append_bytes(buf, reason, strlen(reason) + 1);
	udp_send(info->tftp_socket, buf, info->tftp_ipaddr);
	udp_close(info->tftp_socket);
	info->tftp_socket = -1;

	return BOLT_ERR_PROTOCOLERR;
}

/**********************************************************************
  *  _tftp_open(info,hostname,filename,mode)
  *
  *  Open a file on a remote host, using the TFTP protocol.
  *
  *  Input parameters:
  *	 info - TFTP information
  *	 hostname - host name or IP address of remote host
  *	 filename - name of file on remote system
  *	 mode - file open mode, read or write
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
static int _tftp_open(tftp_info_t *info, char *hostname,
					int port, char *filename,
					int mode)
{
	ebuf_t *buf = NULL;
	const char *datamode = "octet";
	uint16_t type, error, block;
	int res;
	int retries;
	char *blksize_str;
	int blksize;
	char str[TFTP_MAX_BLKSIZE_STRLEN];
	int i, maxlen;

	/* Look up the remote host's IP address
	 */

	res = dns_lookup(hostname, info->tftp_ipaddr);
	if (res < 0)
		return res;

	/* Open a UDP socket to the TFTP server
	 */
	info->tftp_socket = udp_socket(port);
	if (info->tftp_socket < 0)
		return info->tftp_socket;

	info->tftp_lastblock = 0;
	info->tftp_error = 0;
	info->tftp_filemode = mode;
	info->tftp_block_size = TFTP_DEFAULT_BLOCKSIZE;

	/* Try to send the RRQ packet to open the file
	 */

	for (retries = 0; retries < TFTP_MAX_RETRIES; retries++) {

		buf = udp_alloc();
		if (!buf)
			break;

		if (info->tftp_filemode == FILE_MODE_READ) {
			ebuf_append_u16_be(buf, TFTP_OP_RRQ); /* read file */
		} else {
			ebuf_append_u16_be(buf, TFTP_OP_WRQ); /* write file */
		}

		ebuf_append_bytes(buf, filename, strlen(filename) + 1);
		ebuf_append_bytes(buf, datamode, strlen(datamode) + 1);

		/* Send a BLKSIZE option for reads, since we do support IP
		 * fragmentation on receive, but we do not on transmit.
		 */
		if (info->tftp_filemode == FILE_MODE_READ &&
						info->negotiate_blksize) {
			ebuf_append_bytes(buf, TFTP_BLOCK_SIZE_OPTION,
					strlen(TFTP_BLOCK_SIZE_OPTION) + 1);

			/* Suggest a data length based on NET_TFTP_BLKSIZE*/
			blksize_str = env_getenv("NET_TFTP_BLKSIZE");
			if (blksize_str) {
				blksize = lib_atoi(blksize_str);

				/* Check user provided value to ensure it
				 * is in reasonable range */
				if ((blksize > TFTP_MAX_BLOCKSIZE) ||
					(blksize < TFTP_DEFAULT_BLOCKSIZE)) {
					xprintf("Warning: 'NET_TFTP_BLKSIZE' (%d) not in range (%d-%d)\n",
						blksize,
						TFTP_DEFAULT_BLOCKSIZE,
						TFTP_MAX_BLOCKSIZE);
					xprintf("Limiting it to %d.\n",
						TFTP_MAX_BLOCKSIZE);
					blksize = TFTP_MAX_BLOCKSIZE;
				}
			} else {
				/* Use the largest possible size if
				 * NET_TFTP_BLKSIZE is not set */
				blksize = TFTP_MAX_BLOCKSIZE;
			}

			/* Convert integer to string to fill the buffer */
			sprintf(str, "%d", blksize);
			maxlen = strlen(str);
			for (i = 0; i < maxlen; i++)
				buf->eb_ptr[buf->eb_length++] = str[i];
			buf->eb_ptr[buf->eb_length++] = '\0';
		}

		udp_send(info->tftp_socket, buf, info->tftp_ipaddr);

		buf =
		    udp_recv_with_timeout(info->tftp_socket, TFTP_RRQ_TIMEOUT);
		if (buf)
			break;
	}
got_response:

	/* If we got no response, bail now.
	 */

	if (!buf) {
		udp_close(info->tftp_socket);
		info->tftp_socket = -1;
		return BOLT_ERR_TIMEOUT;
	}

	/* Otherwise, process the response.
	 */

	ebuf_get_u16_be(buf, type);

	switch (type) {
	case TFTP_OP_ACK:
		/* Acks are what we get back on a WRQ command,
		 * but are otherwise unexpected.
		 */
		if (info->tftp_filemode == FILE_MODE_WRITE) {
			udp_connect(info->tftp_socket,
				    (uint16_t) buf->eb_usrdata);
			info->tftp_blknum = 1;
			info->tftp_blklen = 0;
			udp_free(buf);
			return 0;
			break;
		}
		/* fall through */
	case TFTP_OP_RRQ:
	case TFTP_OP_WRQ:
	default:
		/* we aren't expecting any of these messages
		 */
		udp_free(buf);
		udp_close(info->tftp_socket);
		info->tftp_socket = -1;
		return BOLT_ERR_PROTOCOLERR;

	case TFTP_OP_OACK:
		/*
		 * Process the options requested (buffer size)
		 */
		udp_connect(info->tftp_socket, (uint16_t)buf->eb_usrdata);
		if (0 == lib_strcmpi((char *)ebuf_ptr(buf), TFTP_BLOCK_SIZE_OPTION)) {
			ebuf_skip(buf, strlen(TFTP_BLOCK_SIZE_OPTION) + 1);
			/* Ok, now at the data */
			info->tftp_block_size = lib_atoi((char *)ebuf_ptr(buf));
			if ((info->tftp_block_size > TFTP_MAX_BLOCKSIZE) ||
			    (info->tftp_block_size < TFTP_DEFAULT_BLOCKSIZE)) {
				udp_free(buf);
				return _tftp_send_blksize_error(info, "Do not like that size");
			}
			udp_free(buf);
			buf = udp_alloc();
			/* Send ACK in response and wait for data */
			ebuf_append_u16_be(buf, TFTP_OP_ACK);
			ebuf_append_u16_be(buf, 0);
			udp_send(info->tftp_socket, buf, info->tftp_ipaddr);
			buf = udp_recv_with_timeout(info->tftp_socket, TFTP_RRQ_TIMEOUT);
			goto got_response;
		} else {
			/* Since this is the only option that we requested
			 * We cannot get any other response!
			 * This we consider to be an error!
			 */
			udp_free(buf);
			return _tftp_send_blksize_error(info, "Unknown option send back");
		}
	case TFTP_OP_ERROR:
		/* Process the error return (XXX: remove xprintf here)
		 */
		ebuf_get_u16_be(buf, error);
		xprintf("TFTP error %d: %s\n", error, ebuf_ptr(buf));
		udp_free(buf);
		udp_close(info->tftp_socket);
		info->tftp_socket = -1;
		return BOLT_ERR_PROTOCOLERR;

	case TFTP_OP_DATA:
		/* Yay, we've got data!  Store the first block.
		 */
		ebuf_get_u16_be(buf, block);
		udp_connect(info->tftp_socket, (uint16_t) buf->eb_usrdata);
		info->tftp_blknum = block;
		info->tftp_blklen = ebuf_length(buf);
		ebuf_get_bytes(buf, info->tftp_data, ebuf_length(buf));
		udp_free(buf);

		if (info->tftp_blklen < info->tftp_block_size)
			info->tftp_lastblock = 1; /* EOF */

		return 0;
		break;

	}
}

/**********************************************************************
  *  _tftp_readmore(info)
  *
  *  Read the next block of the file.  We do this by acking the
  *  previous block.  Once that block is acked, the TFTP server
  *  should send the next block to us.
  *
  *  Input parameters:
  *	 info - TFTP information
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
static int _tftp_readmore(tftp_info_t *info)
{
	ebuf_t *buf;
	uint16_t cmd, block;
	int retries;

	/* If we've already read the last block, there is no more
	 */

	if (info->tftp_lastblock)
		return 1;
	if (info->tftp_error)
		return BOLT_ERR_TIMEOUT;

	/* Otherwise, ack the current block so another one will come
	 */

	for (retries = 0; retries < TFTP_MAX_RETRIES; retries++) {

		buf = udp_alloc();
		if (!buf)
			return -1;

		/* Send the ack
		 */

		ebuf_append_u16_be(buf, TFTP_OP_ACK);
		ebuf_append_u16_be(buf, info->tftp_blknum);
		udp_send(info->tftp_socket, buf, info->tftp_ipaddr);

		/* Wait for some response, retransmitting as necessary
		 */

		buf =
		    udp_recv_with_timeout(info->tftp_socket, TFTP_RECV_TIMEOUT);
		if (buf == NULL)
			continue;

		/* Got a response, make sure it's right
		 */

		ebuf_get_u16_be(buf, cmd);
		if (cmd != TFTP_OP_DATA) {
			udp_free(buf);
			continue;
		}

		ebuf_get_u16_be(buf, block);
		if (block != (uint16_t) (info->tftp_blknum + 1)) {
			udp_free(buf);
			continue;
		}

		/* It's the correct response.  Copy the user data
		 */

		info->tftp_blknum = block;
		info->tftp_blklen = ebuf_length(buf);
		ebuf_get_bytes(buf, info->tftp_data, ebuf_length(buf));
		udp_free(buf);
		break;
	}

	if (retries == TFTP_MAX_RETRIES) {
		info->tftp_error = 1;
		return BOLT_ERR_TIMEOUT;
	}

	/*
	 * If the block is less than block size bytes long, it's the EOF block.
	 */
	if (info->tftp_blklen < info->tftp_block_size)
		info->tftp_lastblock = 1; /* EOF */

	return 0;
}

/**********************************************************************
  *  _tftp_writemore(info)
  *
  *  Write the next block of the file, sending the data from our
  *  holding buffer.  Note that the holding buffer must be full
  *  or else we consider this to be an EOF.
  *
  *  Input parameters:
  *	 info - TFTP information
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
static int _tftp_writemore(tftp_info_t *info)
{
	ebuf_t *buf;
	uint16_t cmd, block, error;
	int retries;

	/* If we've already written the last block, there is no more
	 */

	if (info->tftp_lastblock)
		return 1;
	if (info->tftp_error)
		return BOLT_ERR_TIMEOUT;

	/* Otherwise, send a block
	 */

	for (retries = 0; retries < TFTP_MAX_RETRIES; retries++) {

		buf = udp_alloc();
		if (!buf)
			return -1;

		/* Send the data
		 */

		ebuf_append_u16_be(buf, TFTP_OP_DATA);
		ebuf_append_u16_be(buf, info->tftp_blknum);
		ebuf_append_bytes(buf, info->tftp_data, info->tftp_blklen);
		udp_send(info->tftp_socket, buf, info->tftp_ipaddr);

		/* Wait for some response, retransmitting as necessary
		 */

		buf =
		    udp_recv_with_timeout(info->tftp_socket, TFTP_RECV_TIMEOUT);
		if (buf == NULL)
			continue;

		/* Got a response, make sure it's right
		 */

		ebuf_get_u16_be(buf, cmd);

		if (cmd == TFTP_OP_ERROR) {
			/* Process the error return (XXX: remove xprintf here)
			 */
			ebuf_get_u16_be(buf, error);
			xprintf("TFTP write error %d: %s\n", error,
				ebuf_ptr(buf));
			info->tftp_error = 1;
			info->tftp_lastblock = 1;
			udp_free(buf);
			return BOLT_ERR_IOERR;
		}

		if (cmd != TFTP_OP_ACK) {
			udp_free(buf);
			continue;
		}

		ebuf_get_u16_be(buf, block);
		if (block != (info->tftp_blknum)) {
			udp_free(buf);
			continue;
		}

		/* It's the correct response.  Update the block #
		 */

		info->tftp_blknum++;
		if (info->tftp_blklen != info->tftp_block_size)
			info->tftp_lastblock = 1;
		udp_free(buf);
		break;
	}

	/* If we had some failure, mark the stream with an error
	 */

	if (retries == TFTP_MAX_RETRIES) {
		info->tftp_error = 1;
		return BOLT_ERR_TIMEOUT;
	}

	return 0;
}

/**********************************************************************
  *  _tftp_close(info)
  *
  *  Close a TFTP file.  There are two cases for what we do
  *  here.  If we're closing the file mid-stream, send an error
  *  packet to tell the host we're getting out early.  Otherwise,
  *  just ack the final packet and the connection will close
  *  gracefully.
  *
  *  Input parameters:
  *	 info - TFTP info
  *
  *  Return value:
  *	 0 if ok, else error code
  **********************************************************************/
static int _tftp_close(tftp_info_t *info)
{
	ebuf_t *buf;
	const char *emsg = "transfer cancelled"; /* some error message */

	if (info->tftp_socket == -1)
		return 0;

	if (info->tftp_filemode == FILE_MODE_READ) {
		buf = udp_alloc();
		if (buf) {
			/* If we're on the EOF packet, just send an ack */
			if (info->tftp_lastblock) {
				ebuf_append_u16_be(buf, TFTP_OP_ACK);
				ebuf_append_u16_be(buf, info->tftp_blknum);
			} else {
				ebuf_append_u16_be(buf, TFTP_OP_ERROR);
				ebuf_append_u16_be(buf, TFTP_ERR_DISKFULL);
				ebuf_append_bytes(buf, emsg, strlen(emsg) + 1);
			}
			udp_send(info->tftp_socket, buf, info->tftp_ipaddr);
		}
	} else {
		/* Just flush out the remaining write data, if any */
		_tftp_writemore(info);
	}

	udp_close(info->tftp_socket);
	info->tftp_socket = -1;
	return 0;
}

/**********************************************************************
  *  tftp_fileop_init(fsctx,device)
  *
  *  Create a file system device context for the TFTP service
  *
  *  Input parameters:
  *	 fsctx - location to place context information
  *	 device - underlying device (unused)
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
static int tftp_fileop_init(void **fsctx, void *dev)
{
	void *ref;

	ref = KMALLOC(sizeof(tftp_fsctx_t), 0);

	if (!ref)
		return BOLT_ERR_NOMEM;
	*fsctx = ref;
	return 0;
}

/**********************************************************************
  *  tftp_fileop_open(ref,fsctx,filename,mode)
  *
  *  This is the filesystem entry point for opening a TFTP file.
  *  Allocate a tftp_info structure, open the TFTP file, and
  *  return a handle.
  *
  *  Input parameters:
  *	 ref - location to place reference data (the TFTP structure)
  *	 fsctx - filesystem context
  *	 filename - name of remote file to open
  *	 mode - FILE_MODE_READ or FILE_MODE_WRITE
  *
  *  Return value:
  *	 0 if ok
  *	 else error code
  **********************************************************************/
static int tftp_fileop_open(void **ref, void *fsctx, const char *name, int mode)
{
	tftp_info_t *info;
	char *host, *filename, *portNumChar, *swapTemp;
	int portNum;
	int res;
	int auto_blksize;

	if ((mode != FILE_MODE_READ) && (mode != FILE_MODE_WRITE)) {
		/* must be either read or write, not both */
		return BOLT_ERR_UNSUPPORTED;
	}

	if (!name)
		return BOLT_ERR_INV_PARAM;

	/* Make a copy of the file name so we can hack it up */

	host = strdup(name);
	if (!host)
		return -1;

	/* break the file name into the hostname/filename parts */

	filename = strchr(host, ':');
	if (!filename) {
		KFREE(host);
		return -1;
	}
	*filename++ = '\0';

	portNumChar = strchr(filename, ':');
	if (!portNumChar)
		portNum = UDP_PROTO_TFTP;
	else {
		*portNumChar++ = '\0';
		swapTemp = portNumChar;
		portNumChar = filename;
		filename = swapTemp;
		portNum = lib_atoi(portNumChar);
	}

	/* Allocate the tftp info structure */

	info = KMALLOC(sizeof(tftp_info_t), 0);
	if (!info) {
		KFREE(host);
		return 0;
	}

	/* reset all params to a known value (0) */
	memset(info, '\0', sizeof(tftp_info_t));

	/* If NET_TFTP_AUTO_BLKSIZE is set, perform automatic block size
	 * negotiation. */
	auto_blksize = env_getval("NET_TFTP_AUTO_BLKSIZE");

	if (auto_blksize > 0)
		info->negotiate_blksize = 1;

	/* Open the file */

	res = _tftp_open(info, host, portNum, filename, mode);

	KFREE(host);

	if (res == 0) {
		info->tftp_blkoffset = 0;
		info->tftp_fileoffset = 0;
		*ref = info;
	} else {
		KFREE(info);
		*ref = NULL;
	}
	return res;
}

/**********************************************************************
  *  tftp_fileop_read(ref,buf,len)
  *
  *  Read some bytes from the remote TFTP file.  Do this by copying
  *  data from the block buffer, and reading more data from the
  *  remote file as necessary.
  *
  *  Input parameters:
  *	 ref - tftp_info structure
  *	 buf - destination buffer address (NULL to read data and
  *	       not copy it anywhere)
  *	 len - number of bytes to read
  *
  *  Return value:
  *	 number of bytes read, 0 for EOF, <0 if an error occured.
  **********************************************************************/
static int tftp_fileop_read(void *ref, uint8_t *buf, int len)
{
	tftp_info_t *info = (tftp_info_t *) ref;
	int amtcopy, res = 0, copied = 0;

	if (info->tftp_error)
		return BOLT_ERR_IOERR;

	if (info->tftp_filemode == FILE_MODE_WRITE)
		return BOLT_ERR_UNSUPPORTED;

	while (len) {

		if (info->tftp_blkoffset >= info->tftp_blklen)
			break;

		amtcopy = len;

		if (amtcopy > (info->tftp_blklen - info->tftp_blkoffset))
			amtcopy = (info->tftp_blklen - info->tftp_blkoffset);

		if (buf) {
			memcpy(buf, &(info->tftp_data[info->tftp_blkoffset]),
			       amtcopy);
			buf += amtcopy;
		}

		info->tftp_blkoffset += amtcopy;
		len -= amtcopy;
		info->tftp_fileoffset += amtcopy;
		copied += amtcopy;

		if (info->tftp_blkoffset >= info->tftp_blklen) {
			res = _tftp_readmore(info);
			if (res != 0)
				break;
			info->tftp_blkoffset = 0;
		}
	}

	return (res < 0) ? res : copied;
}

/**********************************************************************
  *  tftp_fileop_write(ref,buf,len)
  *
  *  Write some bytes to the remote TFTP file.  Do this by copying
  *  data to the block buffer, and writing data to the
  *  remote file as necessary.
  *
  *  Input parameters:
  *	 ref - tftp_info structure
  *	 buf - source buffer address
  *	 len - number of bytes to write
  *
  *  Return value:
  *	 number of bytes written, 0 for EOF, <0 if an error occured.
  **********************************************************************/
static int tftp_fileop_write(void *ref, uint8_t *buf, int len)
{
	tftp_info_t *info = (tftp_info_t *) ref;
	int amtcopy, res = 0, copied = 0;

	if (info->tftp_error)
		return BOLT_ERR_IOERR;

	if (info->tftp_filemode == FILE_MODE_READ)
		return BOLT_ERR_UNSUPPORTED;

	if (!buf)
		return BOLT_ERR_INV_PARAM;

	while (len) {

		amtcopy = len;

		if (amtcopy > (info->tftp_block_size - info->tftp_blklen))
			amtcopy = (info->tftp_block_size - info->tftp_blklen);

		memcpy(&(info->tftp_data[info->tftp_blklen]), buf, amtcopy);
		buf += amtcopy;

		info->tftp_blklen += amtcopy;
		len -= amtcopy;
		info->tftp_fileoffset += amtcopy;
		copied += amtcopy;

		if (info->tftp_blklen == info->tftp_block_size) {
			res = _tftp_writemore(info);
			if (res != 0)
				break;

			info->tftp_blklen = 0;
		}
	}

	return (res < 0) ? res : copied;
}

/**********************************************************************
  *  tftp_fileop_seek(ref,offset,how)
  *
  *  Seek within a TFTP file.  Note that you can only seek *forward*,
  *  as TFTP doesn't really let you go backwards.  (I suppose you
  *  could reopen the file, but thus far nobody needs to go
  *  backwards).  You can only seek in a file in read mode.
  *
  *  Input parameters:
  *	 ref - our tftp information
  *	 offset - distance to move
  *	 how - how to move, (FILE_SEEK_*)
  *
  *  Return value:
  *	 new offset, or <0 if an error occured.
  **********************************************************************/
static int tftp_fileop_seek(void *ref, int offset, int how)
{
	tftp_info_t *info = (tftp_info_t *) ref;
	int delta;
	int startloc;
	int res;

	if (info->tftp_filemode == FILE_MODE_WRITE)
		return BOLT_ERR_UNSUPPORTED;

	switch (how) {
	case FILE_SEEK_BEGINNING:
		startloc = info->tftp_fileoffset;
		break;
	case FILE_SEEK_CURRENT:
		startloc = 0;
		break;
	default:
		startloc = 0;
		break;
	}

	delta = offset - startloc;
	if (delta < 0) {
		xprintf("Warning: negative seek on tftp file attempted\n");
		return BOLT_ERR_UNSUPPORTED;
	}
	res = tftp_fileop_read(ref, NULL, delta);
	if (res < 0)
		return res;

	return info->tftp_fileoffset;
}

/**********************************************************************
  *  tftp_fileop_close(ref)
  *
  *  Close the TFTP file.
  *
  *  Input parameters:
  *	 ref - our TFTP info
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
static void tftp_fileop_close(void *ref)
{
	tftp_info_t *info = (tftp_info_t *) ref;

	_tftp_close(info);

	KFREE(info);
}

/**********************************************************************
  *  tftp_fileop_uninit(fsctx)
  *
  *  Uninitialize the filesystem context, freeing allocated
  *  resources.
  *
  *  Input parameters:
  *	 fsctx - our context
  *
  *  Return value:
  *	 nothing
  **********************************************************************/
static void tftp_fileop_uninit(void *fsctx)
{
	KFREE(fsctx);
}
