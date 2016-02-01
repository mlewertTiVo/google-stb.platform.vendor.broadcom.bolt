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
#include "lib_malloc.h"
#include "lib_printf.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "common.h"

#include "timer.h"

#include "config.h"

#if CFG_TCP
#include "net_ebuf.h"
#include "net_api.h"

/*
 * Friendly warning:  Don't put printfs in here or enable any
 * debugging messages in the TCP stack if you're really
 * going to use this for your console device.  You'll end up
 * with a recursion loop!
 */

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define TCPCONSOLE_DEFAULT_PORT	23	/* telnet */

/*  *********************************************************************
    *  Forward
    ********************************************************************* */

static void tcpconsole_probe(bolt_driver_t * drv,
			     unsigned long probe_a, unsigned long probe_b,
			     void *probe_ptr);

static int tcpconsole_open(bolt_devctx_t * ctx);
static int tcpconsole_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int tcpconsole_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat);
static int tcpconsole_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int tcpconsole_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int tcpconsole_close(bolt_devctx_t * ctx);

static const bolt_devdisp_t tcpconsole_dispatch = {
	tcpconsole_open,
	tcpconsole_read,
	tcpconsole_inpstat,
	tcpconsole_write,
	tcpconsole_ioctl,
	tcpconsole_close,
	NULL,
	NULL
};

const bolt_driver_t tcpconsole = {
	"TCP Console",
	"tcpconsole",
	BOLT_DEV_SERIAL,
	&tcpconsole_dispatch,
	tcpconsole_probe
};

/* Fastboot protocol has to be transported with different port.
 * So give it a unique prefix for device name */
const bolt_driver_t tcpfastboot = {
        "TCP Fastboot",
        "tcpfastboot",
        BOLT_DEV_SERIAL,
        &tcpconsole_dispatch,
        tcpconsole_probe
};

/*  *********************************************************************
    *  tcpconsole structure
    ********************************************************************* */

/*
 * States our connection can be in
 */

#define TCPCONSTAT_IDLE		0
#define TCPCONSTAT_LISTEN	1
#define TCPCONSTAT_CONNECTED	2
#define TCPCONSTAT_DISCONNECTED	3
#define TCPCONSTAT_BROKEN	4

/*
 * state information
 */

typedef struct tcpconsole_s {
	int tcp_socket;
	int tcp_status;
	int tcp_port;
} tcpconsole_t;

static void tcpconsole_probe(bolt_driver_t * drv,
			     unsigned long probe_a, unsigned long probe_b,
			     void *probe_ptr)
{
	tcpconsole_t *softc;
	char descr[80];

	softc = (tcpconsole_t *) KMALLOC(sizeof(tcpconsole_t), 0);
	if (softc) {
		softc->tcp_socket = -1;
		softc->tcp_status = TCPCONSTAT_IDLE;

		if (probe_a == 0)
			probe_a = TCPCONSOLE_DEFAULT_PORT;

		xsprintf(descr, "%s (port %lu)", drv->drv_description, probe_a);

		softc->tcp_port = (int)probe_a;

		bolt_attach(drv, softc, NULL, descr);
	}
}

static int tcpconsole_isready(tcpconsole_t * softc, int *rxbytes)
{
	int res;
	int connstat, rxeof;

	res = tcp_status(softc->tcp_socket, &connstat, rxbytes, &rxeof);

	/*
	 * Return:
	 *   -1 if we could not get status
	 *    0 if we are not connected or are connected and at EOF
	 *    1 if we are connected.
	 */

	if (res < 0)
		return res;
	if (connstat != TCPSTATUS_CONNECTED)
		return 0;
	if (!rxeof)
		return 1;

	return 0;
}

static int tcpconsole_process(tcpconsole_t * softc)
{
	int res = 0;

	switch (softc->tcp_status) {
	case TCPCONSTAT_IDLE:
		/* Idle, set up listening socket */
		res = tcp_socket();
		if (res < 0) {
			softc->tcp_status = TCPCONSTAT_BROKEN;
			return res;
		}
		softc->tcp_socket = res;

		res = tcp_listen(softc->tcp_socket, softc->tcp_port);

		if (res < 0) {
			tcp_close(softc->tcp_socket);
			softc->tcp_status = TCPCONSTAT_BROKEN;
			softc->tcp_socket = -1;
			return res;
		}
		softc->tcp_status = TCPCONSTAT_LISTEN;
		break;

	case TCPCONSTAT_LISTEN:
		/* Still waiting for a connection */
		res = 0;
		if (tcpconsole_isready(softc, NULL) > 0) {
			softc->tcp_status = TCPCONSTAT_CONNECTED;
		}
		break;

	case TCPCONSTAT_CONNECTED:
		res = 0;	/* do nothing, we're okay */
		break;

	case TCPCONSTAT_DISCONNECTED:
		/* Currently connected, kill off this connection */
		tcp_close(softc->tcp_socket);
		softc->tcp_socket = -1;
		softc->tcp_status = TCPCONSTAT_IDLE;
		break;

	case TCPCONSTAT_BROKEN:
		/* Broken.  Stay broken. */
		res = 0;
		break;
	}

	return res;
}

static int tcpconsole_open(bolt_devctx_t * ctx)
{
	tcpconsole_t *softc = ctx->dev_softc;
	int res = 0;

	POLL();
	res = tcpconsole_process(softc);

	return res;
}

static int tcpconsole_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	tcpconsole_t *softc = ctx->dev_softc;
	unsigned char *bptr;
	int blen;
	int res;

	POLL();
	res = tcpconsole_process(softc);

	/* Return error code if TCP state machine is in a bad state */
	if (res < 0)
		return res;

	buffer->buf_retlen = 0;

	if (softc->tcp_status == TCPCONSTAT_CONNECTED) {
		bptr = buffer->buf_ptr;
		blen = buffer->buf_length;

		if (tcpconsole_isready(softc, NULL) <= 0) {
			softc->tcp_status = TCPCONSTAT_DISCONNECTED;
			return 0;
		}

		res = tcp_recv(softc->tcp_socket, bptr, blen);

		if (res > 0) {
			buffer->buf_retlen = res;
		}
	}

	return 0;
}

static int tcpconsole_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat)
{
	tcpconsole_t *softc = ctx->dev_softc;
	int rxbytes;

	POLL();
	tcpconsole_process(softc);

	inpstat->inp_status = 0;

	if (softc->tcp_status == TCPCONSTAT_CONNECTED) {
		if (tcpconsole_isready(softc, &rxbytes) <= 0) {
			softc->tcp_status = TCPCONSTAT_DISCONNECTED;
		} else {
			inpstat->inp_status = (rxbytes > 0);
		}
	}

	return 0;
}

static int tcpconsole_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	tcpconsole_t *softc = ctx->dev_softc;
	unsigned char *bptr;
	int blen;
	int res;

	POLL();
	res = tcpconsole_process(softc);

	/* Return error code if TCP state machine is in a bad state */
	if (res < 0)
		return res;

	buffer->buf_retlen = 0;

	if (softc->tcp_status == TCPCONSTAT_CONNECTED) {
		bptr = buffer->buf_ptr;
		blen = buffer->buf_length;

		if (tcpconsole_isready(softc, NULL) <= 0) {
			softc->tcp_status = TCPCONSTAT_DISCONNECTED;
			return 0;
		}

		res = tcp_send(softc->tcp_socket, bptr, blen);

		if (res > 0) {
			buffer->buf_retlen = res;
		}
	}

	return 0;
}

static int tcpconsole_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	return -1;
}

static int tcpconsole_close(bolt_devctx_t * ctx)
{
	tcpconsole_t *softc = ctx->dev_softc;

	if (softc->tcp_status == TCPCONSTAT_CONNECTED) {
		softc->tcp_status = TCPCONSTAT_DISCONNECTED;
	}

	return 0;
}

#endif
