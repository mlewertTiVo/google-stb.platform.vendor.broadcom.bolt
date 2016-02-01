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

/*  *********************************************************************
    *  TCP Flags - keep in sync with net_api.h
    ********************************************************************* */

#define TCPFLG_NODELAY	1	/* disable nagle */
#define TCPFLG_NBIO	2	/* Non-blocking I/O */

#define TCPSTATUS_NOTCONN	0
#define TCPSTATUS_CONNECTING	1
#define TCPSTATUS_CONNECTED	2

/*  *********************************************************************
    *  TCP API
    ********************************************************************* */

typedef struct tcp_info_s tcp_info_t;

tcp_info_t *_tcp_init(ip_info_t * ipi, void *ref);
void _tcp_uninit(tcp_info_t * info);

int _tcp_socket(tcp_info_t * info);
int _tcp_connect(tcp_info_t * ti, int s, uint8_t * dest, uint16_t port);
int _tcp_close(tcp_info_t * ti, int s);
int _tcp_send(tcp_info_t * ti, int s, uint8_t * buf, int len);
int _tcp_recv(tcp_info_t * ti, int s, uint8_t * buf, int len);
int _tcp_bind(tcp_info_t * ti, int s, uint16_t port);
int _tcp_peeraddr(tcp_info_t * ti, int s, uint8_t * addr, uint16_t * port);
int _tcp_listen(tcp_info_t * ti, int s, uint16_t port);
int _tcp_status(tcp_info_t * ti, int s, int *connflag, int *rxready,
		int *rxeof);
int _tcp_debug(tcp_info_t * ti, int s, int arg);
int _tcp_setflags(tcp_info_t * ti, int s, unsigned int flags);
int _tcp_getflags(tcp_info_t * ti, int s, unsigned int *flags);

void _tcp_poll(void *arg);
