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
#include "lib_printf.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "console.h"
#include "env_subr.h"
#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define USER_FIFOSIZE	256

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct usbser_linedata_s {
	uint8_t dLineDataBaud0, dLineDataBaud1, dLineDataBaud2, dLineDataBaud3;
	uint8_t bLineDataStopBits; /* 0=1, 1=1.5, 2=2 */
	uint8_t bLineDataParity; /* 0=none, 1=odd, 2=even, 3=mark, 4=space */
	uint8_t bLineDataBits; /* 5,6,7,8 */
} usbser_linedata_t;

/*  *********************************************************************
    *  Macros
    ********************************************************************* */

#define GETDWFIELD(s, f) ((uint32_t)((s)->f##0) | \
			((uint32_t)((s)->f##1) << 8) | \
			((uint32_t)((s)->f##2) << 16) | \
			((uint32_t)((s)->f##3) << 24))

#define PUTDWFIELD(s, f, v) do { \
				(s)->f##0 = (v & 0xFF); \
				(s)->f##1 = ((v)>>8 & 0xFF); \
				(s)->f##2 = ((v)>>16 & 0xFF); \
				(s)->f##3 = ((v)>>24 & 0xFF); \
				} while (0)

/*  *********************************************************************
    *  Forward Definitions
    ********************************************************************* */

static int usbserial_attach(usbdev_t *dev, usb_driver_t *drv);
static int usbserial_detach(usbdev_t *dev);

static void usb_uart_probe(bolt_driver_t *drv,
			   unsigned long probe_a, unsigned long probe_b,
			   void *probe_ptr);

static int usb_uart_open(bolt_devctx_t *ctx);
static int usb_uart_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usb_uart_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int usb_uart_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usb_uart_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usb_uart_close(bolt_devctx_t *ctx);
int ui_init_uarttestcmds(void);

static const bolt_devdisp_t usb_uart_dispatch = {
	usb_uart_open,
	usb_uart_read,
	usb_uart_inpstat,
	usb_uart_write,
	usb_uart_ioctl,
	usb_uart_close,
	NULL,
	NULL
};

const bolt_driver_t usb_uart = {
	"USB UART",
	"uart",
	BOLT_DEV_SERIAL,
	&usb_uart_dispatch,
	usb_uart_probe
};

typedef struct usb_uart_s {
	int uart_unit;
	int uart_speed;
	int uart_flowcontrol;
} usb_uart_t;


#define USBUART_MAXUNITS 4

static usbdev_t *usbuart_units[USBUART_MAXUNITS];
static int Dev_cnt = 0;
static int Dev_num = 0;

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct usbserial_softc_s {
	int user_inpipe;
	int user_outpipe;
	int user_outmps;
	int user_intpipe;
	uint8_t user_inbuf[USER_FIFOSIZE];
	int user_inbuf_in;
	int user_inbuf_out;
	uint8_t *user_devinbuf;
	int user_devinbufsize;
	int user_unit;
	uint8_t *user_intbuf;
	usbser_linedata_t user_linedata;
} usbserial_softc_t;

usb_driver_t usbserial_driver = {
	"USB Serial Port",
	usbserial_attach,
	usbserial_detach,
	NULL
};

usbdev_t *usbserial_dev = NULL;

#if 0
/*  *********************************************************************
    *  usbserial_get_linedata(dev,linedata)
    *
    *  Request line data from the device.
    *
    *  Input parameters:
    *	dev - USB device
    *	linedata - pointer to structure
    *
    *  Return value:
    *	# of bytes returned
    *	<0 if error
    ********************************************************************* */

static int usbserial_get_linedata(usbdev_t *dev, usbser_linedata_t *ldata)
{
	uint8_t *respbuf;
	int res;

	respbuf = KMALLOC(32, DMA_BUF_ALIGN);

	res =
	    usb_std_request(dev, 0xA1, 0x21, 0, 0, respbuf,
			    sizeof(usbser_linedata_t));

	KFREE(respbuf);

	if ((res >= 0) && ldata)
		memcpy(ldata, respbuf, sizeof(usbser_linedata_t));

	return res;
}
#endif

/*  *********************************************************************
    *  usbserial_set_linedata(dev,linedata)
    *
    *  Set line data to the device.
    *
    *  Input parameters:
    *	dev - USB device
    *	linedata - pointer to structure
    *
    *  Return value:
    *	# of bytes returned
    *	<0 if error
    ********************************************************************* */

static int usbserial_set_linedata(usbdev_t *dev, usbser_linedata_t *ldata)
{
	int res;

	/*
	 * Send request to device.
	 */

	res =
	    usb_std_request(dev, 0x21, 0x20, 0, 0, (uint8_t *) ldata,
			    sizeof(usbser_linedata_t));

	return res;
}

#if 0
/*  *********************************************************************
    *  usbserial_song_and_dance(usbdev_t *dev)
    *
    *  Magic incantations from using the CATC on this device.
    *
    *  Input parameters:
    *	dev
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbserial_song_and_dance(usbdev_t *dev)
{
	int res;
	char databuf[1];

	res = usb_std_request(dev, 0xc0, 0x01, 0x8484, 0, databuf, 1); /*READ*/

	res = usb_std_request(dev, 0x40, 0x01, 0x0404, 0, NULL, 0); /*WRITE*/

	res = usb_std_request(dev, 0xc0, 0x01, 0x8484, 0, databuf, 1); /*READ*/
	res = usb_std_request(dev, 0xc0, 0x01, 0x8383, 0, databuf, 1); /*READ*/
	res = usb_std_request(dev, 0xc0, 0x01, 0x8484, 0, databuf, 1); /*READ*/

	res = usb_std_request(dev, 0x40, 0x01, 0x0404, 1, NULL, 0); /*WRITE*/

	res = usb_std_request(dev, 0xc0, 0x01, 0x8484, 0, databuf, 1); /*READ*/
	res = usb_std_request(dev, 0xc0, 0x01, 0x8383, 0, databuf, 1); /*READ*/

	res = usb_std_request(dev, 0x40, 0x01, 0x0, 1, NULL, 0); /*WRITE*/
	res = usb_std_request(dev, 0x40, 0x01, 0x1, 0xC0, NULL, 0); /*WRITE*/
	res = usb_std_request(dev, 0x40, 0x01, 0x2, 4, NULL, 0); /*WRITE*/

	return 0;
}
#endif

/*  *********************************************************************
    *  usbserial_tx_data(dev,buffer,len)
    *
    *  Synchronously transmit data via the USB.
    *
    *  Input parameters:
    *	dev - device pointer
    *	buffer,len - data we want to send
    *
    *  Return value:
    *	number of bytes sent.
    ********************************************************************* */

static int usbserial_tx_data(usbdev_t *dev, uint8_t *buffer, int len)
{
	uint8_t *bptr;
	usbreq_t *ur;
	usbserial_softc_t *softc = (dev->ud_private);
	int res;

	bptr = KMALLOC(len, DMA_BUF_ALIGN);

	memcpy(bptr, buffer, len);

	ur = usb_make_request(dev, softc->user_outpipe, bptr, len, UR_FLAG_OUT);
	res = usb_sync_request(ur);

/*	printf("Data sent, status=%d, xferred=%d\n",res,ur->ur_xferred); */

	res = ur->ur_xferred;

	usb_free_request(ur);

	KFREE(bptr);

	return res;
}

/*  *********************************************************************
    *  usbserial_int_callback(ur)
    *
    *  Callback routine for the interrupt request, for devices
    *  that have an interrupt pipe.  We ignore this.
    *
    *  Input parameters:
    *	ur - usb request
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static int usbserial_int_callback(usbreq_t *ur)
{
/*    int idx; */

	/*
	 * Check to see if the request was cancelled by someone
	 * deleting our endpoint.
	 */

	if (ur->ur_status == 0xFF) {
		usb_free_request(ur);
		return 0;
	}

/*
	printf("serial int msg: ");
	for (idx = 0; idx < ur->ur_xferred; idx++)
		printf("%02X ",ur->ur_buffer[idx]);
	printf("\n");
*/
	usb_queue_request(ur);

	return 0;

}

/*  *********************************************************************
    *  usbserial_rx_callback(ur)
    *
    *  Callback routine for the regular data pipe.
    *
    *  Input parameters:
    *	ur - usb request
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static int usbserial_rx_callback(usbreq_t *ur)
{
	int idx;
	int iptr;
	usbserial_softc_t *user = (ur->ur_dev->ud_private);

	/*
	 * Check to see if the request was cancelled by someone
	 * deleting our endpoint.
	 */

	if (ur->ur_status == 0xFF) {
		usb_free_request(ur);
		return 0;
	}

	/*
	 * Add characters to the receive fifo
	 */

	for (idx = 0; idx < ur->ur_xferred; idx++) {
		iptr = (user->user_inbuf_in + 1) & (USER_FIFOSIZE - 1);
		if (iptr == user->user_inbuf_out)
			break;	/* overflow */
		user->user_inbuf[user->user_inbuf_in] = ur->ur_buffer[idx];
		user->user_inbuf_in = iptr;
	}

	/*
	 * Requeue the request
	 */

	usb_queue_request(ur);

	return 0;

}

/*  *********************************************************************
    *  usbserial_attach(dev,drv)
    *
    *  This routine is called when the bus scan stuff finds a mass-storage
    *  device.  We finish up the initialization by configuring the
    *  device and allocating our softc here.
    *
    *  Input parameters:
    *	dev - usb device, in the "addressed" state.
    *	drv - the driver table entry that matched
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbserial_attach(usbdev_t *dev, usb_driver_t *drv)
{
	usb_config_descr_t *cfgdscr = dev->ud_cfgdescr;
	usb_endpoint_descr_t *epdscr;
	usb_endpoint_descr_t *indscr = NULL;
	usb_endpoint_descr_t *outdscr = NULL;
	usb_endpoint_descr_t *intdscr = NULL;
	usb_interface_descr_t *ifdscr;
	usbser_linedata_t *ldata;
	usbserial_softc_t *softc;
	usbreq_t *ur;
	int idx;

	dev->ud_drv = drv;

	softc = KMALLOC(sizeof(usbserial_softc_t), DMA_BUF_ALIGN);
	memset(softc, 0, sizeof(usbserial_softc_t));
	dev->ud_private = softc;

	ifdscr = usb_find_cfg_descr(dev, USB_INTERFACE_DESCRIPTOR_TYPE, 0);
	if (ifdscr == NULL) {
		printf("Could not get interface descriptor\n");
		return -1;
	}

	for (idx = 0; idx < ifdscr->bNumEndpoints; idx++) {
		epdscr =
		    usb_find_cfg_descr(dev, USB_ENDPOINT_DESCRIPTOR_TYPE, idx);

		if ((epdscr->bmAttributes & USB_ENDPOINT_TYPE_MASK) ==
		    USB_ENDPOINT_TYPE_INTERRUPT) {
			intdscr = epdscr;
		} else if (USB_ENDPOINT_DIR_OUT(epdscr->bEndpointAddress)) {
			outdscr = epdscr;
		} else {
			indscr = epdscr;
		}
	}

	if (!indscr || !outdscr) {
		printf("IN or OUT endpoint descriptors are missing\n");
		/*
		 * Could not get descriptors, something is very wrong.
		 * Leave device addressed but not configured.
		 */
		return 0;
	}

	/*
	 * Choose the standard configuration.
	 */

	usb_set_configuration(dev, cfgdscr->bConfigurationValue);

	/*
	 * Open the pipes.
	 */

	softc->user_inpipe = usb_open_pipe(dev, indscr);
	softc->user_devinbufsize = GETUSBFIELD(indscr, wMaxPacketSize);
	softc->user_devinbuf = KMALLOC(softc->user_devinbufsize, DMA_BUF_ALIGN);
	softc->user_outpipe = usb_open_pipe(dev, outdscr);
	softc->user_outmps = GETUSBFIELD(outdscr, wMaxPacketSize);

	if (intdscr)
		softc->user_intpipe = usb_open_pipe(dev, intdscr);
	else
		softc->user_intpipe = -1;

	ur = usb_make_request(dev, softc->user_inpipe, softc->user_devinbuf,
			      softc->user_devinbufsize,
			      UR_FLAG_IN | UR_FLAG_SHORTOK);
	ur->ur_callback = usbserial_rx_callback;
	usb_queue_request(ur);

	if (softc->user_intpipe) {
		softc->user_intbuf = KMALLOC(32, DMA_BUF_ALIGN);
		ur = usb_make_request(dev, softc->user_intpipe,
					softc->user_intbuf, GETUSBFIELD(intdscr,
					wMaxPacketSize),
				      UR_FLAG_IN | UR_FLAG_SHORTOK);
		ur->ur_callback = usbserial_int_callback;
		usb_queue_request(ur);
	}

	softc->user_unit = -1;
	for (idx = 0; idx < USBUART_MAXUNITS; idx++) {
		if (usbuart_units[idx] == NULL) {
			softc->user_unit = idx;
			usbuart_units[idx] = dev;
			break;
		}
	}

	console_log("USBSERIAL: Unit %d connected", softc->user_unit);

	/* If this is a Prolific HX device,
	then some additional setup is needed */
	if (dev->ud_devdescr.bMaxPacketSize0 == 64) {
		usb_std_request(dev, 0x40, 0x01, 0x0002, 0x0044, NULL, 0);
		usb_std_request(dev, 0x40, 0x01, 0x0008, 0x0000, NULL, 0);
		usb_std_request(dev, 0x40, 0x01, 0x0009, 0x0000, NULL, 0);
	}
/*	usbserial_song_and_dance(dev); */

	ldata = &(softc->user_linedata);
	PUTDWFIELD(ldata, dLineDataBaud, 115200);
	ldata->bLineDataStopBits = 0;
	ldata->bLineDataParity = 0;
	ldata->bLineDataBits = 8;

	usbserial_set_linedata(dev, ldata);
/*	usbserial_get_linedata(dev, NULL); */

	++Dev_cnt;
	if ((Dev_cnt > Dev_num) && (Dev_num < USBUART_MAXUNITS)) {
		usb_uart_probe((bolt_driver_t *) &usb_uart, idx, 0, 0);
		++Dev_num;
	}

	usbserial_dev = dev;

#if (CFG_CMD_LEVEL >= 3)
	if (env_getenv("USBSERIALTEST")) {
		static int test_cmd = 0;

		if (!test_cmd) {
			ui_init_uarttestcmds();
			test_cmd = 1;
		}
	}
#endif

	return 0;
}

/*  *********************************************************************
    *  usbserial_detach(dev)
    *
    *  This routine is called when the bus scanner notices that
    *  this device has been removed from the system.  We should
    *  do any cleanup that is required.  The pending requests
    *  will be cancelled automagically.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbserial_detach(usbdev_t *dev)
{
	usbserial_softc_t *softc;

	if (!dev)
		goto out;

	softc = dev->ud_private;
	if (!softc)
		goto out;

	console_log("USBSERIAL: USB unit %d disconnected", softc->user_unit);
	if (softc->user_unit >= 0)
		usbuart_units[softc->user_unit] = NULL;

	if (softc->user_devinbuf)
		KFREE(softc->user_devinbuf);

	if (softc->user_intbuf)
		KFREE(softc->user_intbuf);

	KFREE(softc);
	--Dev_cnt;
out:
	return 0;
}

static void usb_uart_probe(bolt_driver_t *drv,
			   unsigned long probe_a, unsigned long probe_b,
			   void *probe_ptr)
{
	usb_uart_t *softc;
	char descr[80];

	softc = (usb_uart_t *) KMALLOC(sizeof(usb_uart_t), DMA_BUF_ALIGN);

	memset(softc, 0, sizeof(usb_uart_t));

	softc->uart_unit = (int)probe_a;

	xsprintf(descr, "USB UART unit %d", (int)probe_a);

	bolt_attach(drv, softc, NULL, descr);
}

static int usb_uart_open(bolt_devctx_t *ctx)
{
/*
	usb_uart_t *softc = ctx->dev_softc;
	int baudrate = CFG_SERIAL_BAUD_RATE;
	usbdev_t *dev = usbuart_units[softc->uart_unit];
*/
	/*
	 * XXX call the uart setup here
	 */

	return 0;
}

static int usb_uart_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usb_uart_t *softc;
	usbdev_t *dev;
	usbserial_softc_t *user;
	unsigned char *bptr;
	int blen;

	softc = ctx->dev_softc;
	if (!softc)
		goto out;

	dev = usbuart_units[softc->uart_unit];
	if (!dev)
		goto out;

	user = dev->ud_private;
	if (!user)
		goto out;

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;

	while ((blen > 0) && (user->user_inbuf_out != user->user_inbuf_in)) {
		*bptr++ = user->user_inbuf[user->user_inbuf_out];
		user->user_inbuf_out =
		    (user->user_inbuf_out + 1) & (USER_FIFOSIZE - 1);
		blen--;
	}

	buffer->buf_retlen = buffer->buf_length - blen;
out:
	return 0;
}

static int usb_uart_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	usb_uart_t *softc;
	usbdev_t *dev;
	usbserial_softc_t *user;

	inpstat->inp_status = 0;

	softc = ctx->dev_softc;
	if (!softc)
		goto out;

	dev = usbuart_units[softc->uart_unit];
	if (!dev)
		goto out;

	user = dev->ud_private;
	if (!user)
		goto out;

	inpstat->inp_status = (user->user_inbuf_in != user->user_inbuf_out);

out:
	return 0;
}

static int usb_uart_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usb_uart_t *softc = ctx->dev_softc;
	unsigned char *bptr;
	int blen;
	usbdev_t *dev = usbuart_units[softc->uart_unit];

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;

	if (!dev) {
		buffer->buf_retlen = blen;
		return 0;
	}

	usbserial_tx_data(dev, bptr, blen);

	buffer->buf_retlen = blen;
	return 0;
}

static int usb_uart_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usb_uart_t *softc = ctx->dev_softc;
	usbdev_t *dev = usbuart_units[softc->uart_unit];
	unsigned int *info;

	if (!dev)
		return -1;

	info = (unsigned int *)buffer->buf_ptr;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_SERIAL_GETSPEED:
		*info = softc->uart_speed;
		break;
	case IOCTL_SERIAL_SETSPEED:
		softc->uart_speed = *info;
		/* NYI */
		break;
	case IOCTL_SERIAL_GETFLOW:
		*info = softc->uart_flowcontrol;
		break;
	case IOCTL_SERIAL_SETFLOW:
		softc->uart_flowcontrol = *info;
		/* NYI */
		break;
	default:
		return -1;
	}

	return 0;
}

static int usb_uart_close(bolt_devctx_t *ctx)
{
/*	usb_uart_t *softc = ctx->dev_softc; */

	return 0;
}
