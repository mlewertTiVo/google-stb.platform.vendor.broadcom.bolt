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

#include "bolt.h"
#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"

#include "bsp_config.h"

/* Rx status register */
#define RXRDA          0x01
#define RXOVFERR       0x02
#define RXPARERR       0x04
#define RXFRAMERR      0x08

#define THRE				0x20
#define UART_SDW_RBR		0x00
#define UART_SDW_THR		0x00
#define UART_SDW_DLL		0x00
#define UART_SDW_DLH		0x04
#define UART_SDW_IER		0x04
#define UART_SDW_IIR		0x08
#define UART_SDW_FCR		0x08
#define UART_SDW_LCR		0x0c
#define UART_SDW_MCR		0x10
#define UART_SDW_LSR		0x14
#define UART_SDW_MSR		0x18
#define UART_SDW_SCR		0x1c

#define WRITEREG(csr, val) (*(csr) = (val))
#define READREG(csr)     (*(csr))

static void bcm97XXX_uart_probe(bolt_driver_t *drv,
				unsigned long probe_a, unsigned long probe_b,
				void *probe_ptr);

static int bcm97XXX_uart_open(bolt_devctx_t *ctx);
static int bcm97XXX_uart_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int bcm97XXX_uart_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int bcm97XXX_uart_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int bcm97XXX_uart_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int bcm97XXX_uart_close(bolt_devctx_t *ctx);

static const bolt_devdisp_t bcm97XXX_uart_dispatch = {
	bcm97XXX_uart_open,
	bcm97XXX_uart_read,
	bcm97XXX_uart_inpstat,
	bcm97XXX_uart_write,
	bcm97XXX_uart_ioctl,
	bcm97XXX_uart_close,
	NULL,
	NULL
};

const bolt_driver_t bcm97XXX_uart = {
	"16550 DUART",
	"uart",
	BOLT_DEV_SERIAL,
	&bcm97XXX_uart_dispatch,
	bcm97XXX_uart_probe
};

struct bcm97XXX_uart_s {
	int baudrate;

	volatile uint32_t *rxstat;
	volatile uint32_t *rxdata;
	volatile uint32_t *txstat;
	volatile uint32_t *txdata;
};

static void bcm97XXX_set_baudrate(struct bcm97XXX_uart_s *softc)
{
	return;
}

static void bcm97XXX_uart_probe(bolt_driver_t *drv,
				unsigned long probe_a, unsigned long probe_b,
				void *probe_ptr)
{
	struct bcm97XXX_uart_s *softc;
	char descr[80];

	/* enable the transmitter interrupt? */

	/*
	 * probe_a is the DUART base address.
	 * probe_b is the channel-number-within-duart (0 or 1)
	 * probe_ptr is unused.
	 */
	softc = (struct bcm97XXX_uart_s *)KMALLOC(
			sizeof(struct bcm97XXX_uart_s), 0);
	if (softc) {
		softc->rxstat = (uint32_t *) (probe_a + UART_SDW_LSR);
		softc->rxdata = (uint32_t *) (probe_a + UART_SDW_RBR);
		softc->txstat = (uint32_t *) (probe_a + UART_SDW_LSR);
		softc->txdata = (uint32_t *) (probe_a + UART_SDW_THR);
		xsprintf(descr, "%s at %#lx channel %lu", drv->drv_description,
			 probe_a, probe_b);
		bolt_attach(drv, softc, NULL, descr);
	}
}

static int bcm97XXX_uart_open(bolt_devctx_t *ctx)
{
	struct bcm97XXX_uart_s *softc = ctx->dev_softc;

	softc->baudrate = CFG_SERIAL_BAUD_RATE;
	bcm97XXX_set_baudrate(softc);

	WRITEREG(softc->rxstat, 0x0);
	WRITEREG(softc->txstat, 0x0);

	return 0;
}

static int bcm97XXX_uart_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct bcm97XXX_uart_s *softc = ctx->dev_softc;
	unsigned char *bptr;
	int blen;
	uint32_t status;

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;

	while (blen > 0) {
		status = READREG(softc->rxstat);
		if (status & (RXOVFERR | RXPARERR | RXFRAMERR)) {
			/* Just read the bad character to clear the bit. */
			READREG(softc->rxdata);
		} else if (status & RXRDA) {
			*bptr++ = READREG(softc->rxdata) & 0xFF;
			blen--;
		} else
			break;
	}

	buffer->buf_retlen = buffer->buf_length - blen;
	return 0;
}

static int bcm97XXX_uart_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	struct bcm97XXX_uart_s *softc = ctx->dev_softc;

	inpstat->inp_status = (READREG(softc->rxstat) & RXRDA) ? 1 : 0;

	return 0;
}

static int bcm97XXX_uart_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct bcm97XXX_uart_s *softc = ctx->dev_softc;
	unsigned char *bptr;
	int blen;
	uint32_t status;

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;

	while (blen > 0) {
		do {
			status = READREG(softc->txstat) & THRE;
		} while (!status);
		WRITEREG(softc->txdata, *bptr);

		bptr++;
		blen--;
	}

	buffer->buf_retlen = buffer->buf_length - blen;

	return 0;
}

static int bcm97XXX_uart_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct bcm97XXX_uart_s *softc = ctx->dev_softc;
	unsigned int *info = (unsigned int *)buffer->buf_ptr;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_SERIAL_GETSPEED:
		*info = softc->baudrate;
		break;
	case IOCTL_SERIAL_SETSPEED:
		softc->baudrate = *info;
		bcm97XXX_set_baudrate(softc);
		break;
	case IOCTL_SERIAL_GETFLOW:
		*info = SERIAL_FLOW_HARDWARE;
		break;
	case IOCTL_SERIAL_SETFLOW:
	/* Fall through */
	default:
		return -1;
	}

	return 0;
}

static int bcm97XXX_uart_close(bolt_devctx_t *ctx)
{
	return 0;
}
