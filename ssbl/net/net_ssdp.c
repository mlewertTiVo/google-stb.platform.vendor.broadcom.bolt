/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_malloc.h"
#include "common.h"
#include "error.h"
#include "bchp_bsp_glb_control.h"
#include "chipid.h"
#include "net_ebuf.h"
#include "net_api.h"
#include "net_ssdp.h"
#include "bchp_sun_top_ctrl.h"
#include "timer.h"
#include "env_subr.h"
#include "board.h"
#include "bolt.h"



/* needed for chip part number */
#define chip_id_without_rev(x) (x & 0xf0000000 ? x >> 16 : x >> 8)

/* needed for created uuid */
#define uint32_top(x) ((uint16_t) (x >> 16))
#define uint32_bottom(x) ((uint16_t) x & 0xFFFF)

static ssdp_st_t *ssdp_get_first_st(ssdp_context_t *ctx);

/**********************************************************************
  * ssdp_create_custom_msg()
  *
  * Input parameters:
  *	buf: pointer to char array.
  *	buf_len: buf length.
  *
  * Output parameters:
  *	buf: custom ssdp headers are created here.
  *
**********************************************************************/
static void ssdp_create_custom_msg(char *buf, int buf_len)
{
	char *board_serial = env_getenv(ENVSTR_BOARD_SERIAL);
	char *node_name = env_getenv(NODENAME_STR);

	snprintf(buf, buf_len,
		/*board name */
		"BOARDNAME.BROADCOM.NET: %s\r\n"
		/*board serial number */
		"SERIALNUMBER.BROADCOM.NET: %s\r\n"
		/* node name */
		"NODENAME.BROADCOM.NET: %s\r\n",
		board_name(),
		board_serial ? board_serial : "",
		node_name ? node_name : "");
}

/**********************************************************************
  *  ssdp_create_alive_msg()
  *
  *  create NOTIFY ssdp:alive message
  *  Input parameters:
  *	ctx:ssdp handler.
  *	msg_len:max message buffer length.
  *	search_target:pointer to search target array.
  *	usn:usn value
  *  Output parameters:
  *	msg:pointer to msg buffer.
  *  Return value:
  *  If successful,return msg length.
**********************************************************************/
static int ssdp_create_alive_msg(ssdp_context_t *ctx, char *msg,
				int msg_len, ssdp_st_t *search_target,
					const char *usn)
{
	char ip_addr[20];
	uint8_t *addr;
	char custom_msg[MAX_SSDP_CUSTOM_MSG_LEN];

	addr = net_getparam(NET_IPADDR);
	sprintf_ip(ip_addr, addr);

	ssdp_create_custom_msg(custom_msg, MAX_SSDP_CUSTOM_MSG_LEN);

	return snprintf(msg, msg_len,
		"NOTIFY * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		/* seconds until advertisement expires */
		"CACHE-CONTROL: max-age = %d\r\n"
		 /* URL for UPnP description for root device */
		"LOCATION: %s\r\n"
		"NT: %s\r\n" /* notification type */
		"NTS: ssdp:alive\r\n"
		"SERVER: %s/%s %s/%s\r\n"
		"USN: %s\r\n" /* composite identifier for the advertisement */
		"%s" /* custom msg */
		"\r\n", search_target->max_age, ip_addr,
		search_target->st_name, ctx->device.os,
		ctx->device.os_version, ctx->device.prod,
		ctx->device.prod_version, usn,
		custom_msg);
}
/**********************************************************************
  *  ssdp_create_byebye_msg()
  *
  *  create NOTIFY ssdp:byebye message
  *  Input parameters:
  *	ctx:ssdp handler.
  *	msg_len:max message buffer length.
  *	search_target:pointer to search target array.
  *	usn:usn value
  *  Output parameters:
  *	msg:pointer to msg buffer.
  *  Return value:
  *  If successful,return msg length.
**********************************************************************/
static int ssdp_create_byebye_msg(ssdp_context_t *ctx,
		char *msg, int msg_len, ssdp_st_t *search_target,
			const char *usn)
{
	return snprintf(msg, msg_len,
		"NOTIFY * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"NT: %s\r\n" /* notification type */
		"NTS: ssdp:byebye\r\n"
		"USN: %s\r\n" /* composite identifier for the advertisement */
		"\r\n", search_target->st_name, usn);
}

/**********************************************************************
  *  ssdp_get_line_length()
  *
  *  Input parameters:
  *	msg:pointer to msg buffer.
    *	msg_len:total message length.
  *  Return value:
  *  If successful,return line length.
  *  The value BOLT_ERR indicates an error.
**********************************************************************/
static int ssdp_get_line_length(const char *msg, int msg_len)
{
	int cur_char;

	for (cur_char = 0; cur_char < (msg_len - 1); cur_char++) {
		/* find '\r' && '\n' characters */
		if (msg[cur_char] == '\r' && msg[cur_char + 1] == '\n') {
			cur_char += 2; /* add the '\r' and '\n' to the count */
			return cur_char;
		}
	}
	return BOLT_ERR; /* not a valid SSDP line */
}

/**********************************************************************
  *  ssdp_get_header_value()
  *
  *  Input parameters:
  *	value_len:ssdp header value length.
  *	header:ssdp header.
  *	msg:ssdp message.
  *	msg_len:ssdp message length.
  *  Output parameters:
  *	value:pointer to value of the header.
  *  Return value:
  *  If successful,return header length.
  *  0 indicates no header found in the message
**********************************************************************/
static int ssdp_get_header_value(char *value, int value_len,
				const char *header, const char *msg,
					int msg_len)
{
	int msg_len_left = msg_len;
	int header_len = strlen(header);
	int line_len;

	while (msg_len_left) {
		line_len = ssdp_get_line_length(msg, msg_len_left);

		/* check for token */
			if (strncasecmp(msg, header, header_len) == 0) {
				/* found header so get value and
				skip the header we found */
				msg += header_len;
				line_len -= header_len;

				/* account for colon and
				space following header */
				msg += 2;
				line_len -= 2;

				/* check that we won't
				overflow the value buffer */
				if (line_len > (value_len - 1))
					line_len = (value_len - 1);

				strncpy(value,  msg, line_len);
				value[line_len] = '\0';

				return line_len;
		}

		/* advance the string pointer */
		msg += line_len;
		msg_len_left -= line_len;
	}
	return BOLT_OK;
}

/**********************************************************************
  *  ssdp_create_usn()
  *
  *  Input parameters:
  *	usn_len:max usn message buffer length
  *	uuid:uuid
  *	st_name:  Search Target name
  *  Output parameters:
  *	usn:usn message
  *  Return value:
  *  If successful,return BOLT_OK.
**********************************************************************/
static int ssdp_create_usn(char *usn, int usn_len, const char *uuid,
							const char *st_name)
{
	int uuid_len;

	/* USN always includes the uuid */
	snprintf(usn, usn_len, "%s%s", UUID_PREFIX, uuid);
	uuid_len = strlen(usn);

	/* if NT (search target) is not uuid then append "::NT" to end of USN */
	if (0 != strncmp(st_name, UUID_PREFIX, strlen(UUID_PREFIX))) {
		usn += uuid_len;
		usn_len -= uuid_len;

		snprintf(usn, usn_len, "::%s", st_name);
	}
	return BOLT_OK;
}

/**********************************************************************
  *  ssdp_send_msg()
  *
  *  Input parameters:
  *	ctx:ssdp instance
  *	msg:ssdp message to send
  *	msg_len:ssdp message length
  *	st_name:  Search Target name
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_send_msg(ssdp_context_t *ctx, const char *msg, int msg_len)
{
	if (sendto(ctx->sockfd, msg, msg_len, 0,
		(struct sockaddr *) &ctx->mcast_addr,
				sizeof(ctx->mcast_addr)) < 0) {
		err_msg("Failed to send ssdp notify!!!!!!!\n");
		return BOLT_ERR;
	}

	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_send_notify()
  *
  *  Input parameters:
  *	ctx:ssdp instance
  *	search_target:pointer to search target
  *	flag:either live or byebye message
  *	st_name:  Search Target name
  *  Return value:
  *  If successful,return message length.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_send_notify(ssdp_context_t *ctx, ssdp_st_t *search_target,
								uint32_t flags)
{
	int notify_len = 0;
	char usn[MAX_USN_LEN];
	char notify_msg[MAX_SSDP_MSG_LEN];

	ssdp_create_usn(usn, MAX_USN_LEN,
		ctx->device.uuid, search_target->st_name);

	switch (flags) {
	case SSDP_NOTIFY_ALIVE:
		notify_len = ssdp_create_alive_msg(ctx, notify_msg,
						MAX_SSDP_MSG_LEN, search_target,
						usn);
		/* if we actually sent the notification
		then set the timer to resend */
		if (notify_len)
			TIMER_SET(ctx->st_timer, ctx->max_age * BOLT_HZ);
		break;
	case SSDP_NOTIFY_BYEBYE:
		notify_len = ssdp_create_byebye_msg(ctx, notify_msg,
					MAX_SSDP_MSG_LEN, search_target, usn);
		break;
	default:
		return BOLT_ERR;
		break;
	}

	if (0 < notify_len)
		ssdp_send_msg(ctx, notify_msg, notify_len);
	return notify_len;
}
/**********************************************************************
  *  ssdp_register_st()
  *
  *  Input parameters:
  *	ctx:ssdp instance
  *	target:search target
  *	max_age:seconds until advertisement expires
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_register_st(ssdp_context_t *ctx, const char *target, uint32_t max_age)
{
	ssdp_st_t *search_target = (ssdp_st_t *) malloc(sizeof(ssdp_st_t));

	if (NULL == search_target)
		return BOLT_ERR;

	if (max_age < SSDP_MAX_AGE_MIN)
		max_age = SSDP_MAX_AGE_DEFAULT;

	strcpy(search_target->st_name, target);
	search_target->st_name[strlen(search_target->st_name)] = 0;
	search_target->max_age = max_age;
	search_target->st_next = NULL;

	if (NULL == ctx->st_list)
		ctx->st_list = search_target;
	else {
		ssdp_st_t *last_target = ctx->st_list;

		while (last_target->st_next)
			last_target = last_target->st_next;

		last_target->st_next = search_target;
	}

	/* we registered a new search target so advertise it */
	ssdp_send_notify(ctx, search_target, SSDP_NOTIFY_ALIVE);

	/* spec compliant we should store the
	time we sent so we can resend once
	the max-age has expired */
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_renew_st()
  *
  *  Input parameters:
  *	ctx:ssdp instance
**********************************************************************/
static void ssdp_renew_st(ssdp_context_t *ctx)
{
	if (TIMER_EXPIRED(ctx->st_timer)) {
		/* Send a notification to keep services active. Per
		UPnP spec v1.1 A control point should
		 * assume all  services are still available if it
		 receives at least one advertisement
		 */
		ssdp_st_t *search_target = ssdp_get_first_st(ctx);

		ssdp_send_notify(ctx, search_target, SSDP_NOTIFY_ALIVE);
	}
}
static ssdp_st_t *ssdp_get_first_st(ssdp_context_t *ctx)
{
	return ctx->st_list;
}
static ssdp_st_t *ssdp_get_next_st(ssdp_st_t *st)
{
	return st->st_next;
}
/**********************************************************************
  *  ssdp_find_st()
  *
  *  Input parameters:
  *	ctx:ssdp instance
  *	target:target to find
  *  Return value:
  *  If successful,return pointer to search target array.
  *  NULL for failure
**********************************************************************/
static ssdp_st_t *ssdp_find_st(ssdp_context_t *ctx, const char *target)
{
	ssdp_st_t *cur_target = ctx->st_list;

	while (cur_target) {
		if (0 == strncmp(target, cur_target->st_name,
					strlen(cur_target->st_name)))
			return cur_target; /* found a match return it */

		cur_target = cur_target->st_next;
	}
	return NULL; /* didn't find a match so return false */
}
/**********************************************************************
  *  ssdp_message_type()
  *
  *  Input parameters:
  *	msg:ssdp message
  *	msg_len:ssdp message length
  *  Return value:
  *  If successful,return SSDP request type.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_message_type(char *msg, int msg_len)
{
	if (msg_len > 0) {
		if (0 == strncmp(msg, STARTLINE_MSEARCH,
				strlen(STARTLINE_MSEARCH)))
			return SSDP_MSEARCH;
		if (0 == strncmp(msg, STARTLINE_NOTIFY,
				strlen(STARTLINE_NOTIFY)))
			return SSDP_NOTIFY;
		if (0 == strncmp(msg, STARTLINE_RESPONSE,
				strlen(STARTLINE_RESPONSE)))
			return SSDP_RESPONSE;
	}
	return BOLT_ERR;
}
/**********************************************************************
  *  ssdp_msearch_reply()
  *
  *  reply to M Search message
  *  Input parameters:
  *	ctx:ssdp instance
  *	search_target:pointer to search target
  *	mx:max wait time in sec
  *	dst_addr:destination address
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_msearch_reply(ssdp_context_t *ctx, ssdp_st_t *search_target,
				int mx, struct sockaddr_in *dst_addr)
{
	char reply_msg[MAX_SSDP_MSG_LEN];
	int  reply_len;
	char custom_msg[MAX_SSDP_CUSTOM_MSG_LEN];
	char usn[MAX_USN_LEN];
	char ip_addr[20];
	uint8_t *addr;

	/* get our ip address for LOCATION header */
	addr = net_getparam(NET_IPADDR);
	sprintf_ip(ip_addr, addr);

	ssdp_create_usn(usn, MAX_USN_LEN, ctx->device.uuid,
					search_target->st_name);

	ssdp_create_custom_msg(custom_msg, MAX_SSDP_CUSTOM_MSG_LEN);

	reply_len = snprintf(reply_msg, MAX_SSDP_MSG_LEN,
		"HTTP/1.1 200 OK\r\n"
		/*max-age = seconds until advertisement expires */
		"CACHE-CONTROL: max-age = %d\r\n"
		"DATE:\r\n"
		/*Header field name only; no field value. */
		"EXT:\r\n"
		/*URL for UPnP description for root device */
		"LOCATION: %s\r\n"
		/*OS/version UPnP/1.1 product/version */
		"SERVER: %s/%s %s/%s\r\n"
		/*search target */
		"ST: %s\r\n"
		/*composite identifier for the advertisement */
		"USN: %s\r\n"
		/*custom msg*/
		"%s"
		"\r\n", search_target->max_age, ip_addr, ctx->device.os,
		ctx->device.os_version, ctx->device.prod,
		ctx->device.prod_version, search_target->st_name, usn,
		custom_msg);

	if (0 < reply_len) {
		/* find some random time to reply */
		if (sendto(ctx->sockfd, reply_msg, reply_len,
			0, (struct sockaddr *) dst_addr,
			sizeof(*dst_addr)) < 0) {
			err_msg("Failed to send ssdp m-search reply!!!!!!!\n");
			return BOLT_ERR;
		}
	}
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_msearch_handler()
  *
  *  m search parser and reply to the msg
  *  Input parameters:
  *	ctx:ssdp instance
  *	dst_addr:destination address
  *	msg:message received
  *	msg_len:message length
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_msearch_handler(ssdp_context_t *ctx,
					struct sockaddr_in *dst_addr, char *msg,
						int msg_len)
{
	char header_val[MAX_SSDP_HEADER_VAL_LEN];
	ssdp_st_t *st = NULL;
	int mx;

	ssdp_get_header_value(header_val, MAX_SSDP_HEADER_VAL_LEN,
							"MX", msg, msg_len);
	mx = atoi(header_val);

	ssdp_get_header_value(header_val, MAX_SSDP_HEADER_VAL_LEN,
							"ST", msg, msg_len);
	if (0 == strncmp(header_val, "ssdp:all",
						strlen("ssdp:all"))) {
			/* reply for each advertisment we have */
			st = ssdp_get_first_st(ctx);
		while (st) {
			ssdp_msearch_reply(ctx, st, mx, dst_addr);
			st = ssdp_get_next_st(st);
		}
		return BOLT_OK;
	}

	/* check if they're trying to discover something we advertise */
	st = ssdp_find_st(ctx, header_val);
	if (NULL != st) {
		ssdp_msearch_reply(ctx, st, mx, dst_addr);
		return BOLT_OK;
	}

	/* M-SEARCH for something we don't advertise */
	return BOLT_ERR;
}
/**********************************************************************
  *  ssdp_message_handler()
  *
  *  ssdp M SEARCH message processing
  *  Input parameters:
  *	ctx:ssdp instance
  *	dst_addr:destination address
  *	msg:message received
  *	msg_len:message length
  *  Return value:
  *  return BOLT_OK for M Search msg.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_message_handler(ssdp_context_t *ctx,
						struct sockaddr_in *dst_addr,
						char *msg, int msg_len)
{
	int msg_type;

	msg_type = ssdp_message_type(msg, ssdp_get_line_length(msg, msg_len));

	/* we only care about M-SEARCH messages */
	if (SSDP_MSEARCH == msg_type) {
		if (BOLT_OK != ssdp_msearch_handler(ctx,
							dst_addr, msg, msg_len))
			return BOLT_ERR;
	}
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_recv()
  *
  *  receives the ssdp message from udp driver
  *  Input parameters:
  *	ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_recv(ssdp_context_t *ctx)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	char mcast_msg[MAX_SSDP_MSG_LEN];
	size_t mcast_msg_len;

	if (NULL == ctx)
		return BOLT_ERR;

	client_addr_len = sizeof(struct sockaddr);

	/* listen for multicast messages */
	mcast_msg_len = recvfrom(ctx->sockfd, mcast_msg,
					(MAX_SSDP_MSG_LEN - 1), 0,
					(struct sockaddr *) &client_addr,
					&client_addr_len);

	/* if we received a message handle it */
	if (0 < mcast_msg_len)
		ssdp_message_handler(ctx, &client_addr,
						mcast_msg, mcast_msg_len);
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_net_init()
  *
  *  socket programming for ssdp service
  *  Input parameters:
  *	ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
int ssdp_net_init(ssdp_context_t *ctx)
{
	struct sockaddr_in server_addr;

	if (!ctx)
		return BOLT_ERR;

	/* create socket */
	ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ctx->sockfd < 0) {
		err_msg("%s: cannot create socket\n", __func__);
		return BOLT_ERR;
	}

	/* setup the server interface */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(MULTICAST_PORT);

	/* bind the server address */
	if (0 > bind(ctx->sockfd, (struct sockaddr *)&server_addr,
		sizeof(server_addr))) {
		err_msg("%s: failed to bind server interface\n", __func__);
		close(ctx->sockfd);
		return BOLT_ERR;
	}

	/* join the multicast group */
	ctx->group.imr_multiaddr.s_addr =
		inet_addr(MULTICAST_ADDR);
	/* multicast group address */
	ctx->group.imr_interface.s_addr =
		htonl(INADDR_ANY);
	/* interface to join group */

	/* request to join group */
	if (setsockopt(ctx->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		&ctx->group, sizeof(ctx->group)) < 0) {
		err_msg("%s: failed to join multicast group\n", __func__);
		close(ctx->sockfd);
		return BOLT_ERR;
	}

	/* setup the multicast address for sending data */
	ctx->mcast_addr.sin_family = AF_INET;
	ctx->mcast_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
	ctx->mcast_addr.sin_port = htons(MULTICAST_PORT);

	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_device_init()
  *
  *  get device specific params
  *  Input parameters:
  *	ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int ssdp_device_init(ssdp_context_t *ctx)
{
	/* get the chip family */
	uint32_t fam = REG(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);

	/* get the OTP chip id */
	uint32_t hi = REG(BCHP_BSP_GLB_CONTROL_v_PubOtpUniqueID_hi);
	uint32_t lo = REG(BCHP_BSP_GLB_CONTROL_v_PubOtpUniqueID_lo);

	snprintf(ctx->device.os, MAX_OS_NAME_LEN, "BOLT");
	snprintf(ctx->device.os_version, MAX_OS_VERSION_LEN,
		"%d.%02d", BOLT_VER_MAJOR, BOLT_VER_MINOR);
	snprintf(ctx->device.prod, MAX_PROD_NAME_LEN,
		"%04x", chip_id_without_rev(fam));
	snprintf(ctx->device.prod_version, MAX_PROD_VERSION_LEN,
		"%02x", (fam & CHIPID_REV_MASK) + 0xa0);
	snprintf(ctx->device.uuid, MAX_UUID_LEN,
		"BROADCOM-2016-1111-%04x-%04x%08x",
		uint32_top(hi), uint32_bottom(hi), lo);

	return BOLT_OK;
}
/**********************************************************************
  *  register_search_targets()
  *
  *  register search targets
  *  Input parameters:
  *	ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
static int register_search_targets(ssdp_context_t *ctx)
{
	char search_target[MAX_ST_NAME_LEN];

	/* register root device */
	ssdp_register_st(ctx, "upnp:rootdevice", ctx->max_age);

	/* register chip type */
	snprintf(search_target, MAX_ST_NAME_LEN,
			"urn:broadcom-com:BCG:%s:%s", ctx->device.prod,
				ctx->device.prod_version);
	ssdp_register_st(ctx, search_target, ctx->max_age);

	/* register UUID */
	snprintf(search_target, MAX_ST_NAME_LEN, "uuid:%s", ctx->device.uuid);
	ssdp_register_st(ctx, search_target, ctx->max_age);

	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_init()
  *
  *  initialize ssdp services and register different
  *  search target
  *  Input parameters:
  *	ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
int ssdp_init(ssdp_context_t *ctx)
{
	/* initialize a context */
	memset(ctx, 0, sizeof(*ctx));

	/* just use the default max-age */
	ctx->max_age = SSDP_MAX_AGE_DEFAULT;

	/* Initialize the network */
	if (BOLT_OK != ssdp_net_init((ssdp_context_t *) ctx)) {
		/*err_msg("%s: failed to initialized
				the network interface\n", __func__);*/
		return BOLT_ERR;
	}

	/* Get the device information */
	if (BOLT_OK != ssdp_device_init((ssdp_context_t *) ctx)) {
		err_msg("%s: failed to initialize device information\n", __func__);
		return BOLT_ERR;
	}

	/* Register our valid search targets */
	if (BOLT_OK != register_search_targets((ssdp_context_t *) ctx)) {
		err_msg("%s: failed to register search targets\n", __func__);
		return BOLT_ERR;
	}
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_term()
  *
  *  terminate ssdp services
  *  Input parameters:
  *	 ctx:ssdp instance
  *  Return value:
  *  If successful,return BOLT_OK.
  *  BOLT_ERR for failure
**********************************************************************/
int ssdp_term(ssdp_context_t *ctx)
{
	/* request to drop group */
	if (setsockopt(ctx->sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		&ctx->group, sizeof(ctx->group)) < 0) {
		err_msg("%s: failed to drop multicast group\n", __func__);
		close(ctx->sockfd);
		return BOLT_ERR;
	}
	close(ctx->sockfd);
	/* initialize a context */
	memset(ctx, 0, sizeof(*ctx));
	return BOLT_OK;
}
/**********************************************************************
  *  ssdp_poll()
  *
  *  ssdp task handler
  *  Input parameters:
  *	ctx:ssdp instance
**********************************************************************/
void ssdp_poll(void *arg)
{
	ssdp_context_t *ctx = (ssdp_context_t *) arg;

	/* Check if we received any messages */
	ssdp_recv(ctx);

	/* Check expiration of search targets */
	ssdp_renew_st(ctx);
}
