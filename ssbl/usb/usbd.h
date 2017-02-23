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

#ifndef _PHYSADDR_T_DEFINED_
#include "lib_physio.h"
#endif

#include "arch.h"
#include "common.h"
#include "bsp_config.h"
#include "usbchap9.h"

/*  *********************************************************************
    *  Forward declarations and opaque types
    ********************************************************************* */

typedef struct usb_hc_s usb_hc_t;
typedef struct usb_ept_s usb_ept_t;
typedef struct usb_hcdrv_s usb_hcdrv_t;
typedef struct usbdev_s usbdev_t;
typedef struct usb_driver_s usb_driver_t;

/*  *********************************************************************
    *  USB Bus structure - one of these per host controller
    ********************************************************************* */

#define USB_MAX_DEVICES	128

typedef struct usbbus_s {
	struct usbbus_s *ub_next;	/* link to other buses */
	usb_hc_t *ub_hwsoftc;	/* bus driver softc */
	usb_hcdrv_t *ub_hwdisp;	/* bus driver dispatch */
	usbdev_t *ub_roothub;	/* root hub device */
	usbdev_t *ub_devices[USB_MAX_DEVICES];	/* pointers to each device, idx by address */
	unsigned int ub_flags;	/* flag bits */
	int ub_num;		/* bus number */
} usbbus_t;

#define UB_FLG_NEEDSCAN	1	/* some device on bus needs scanning */
#define UB_FLG_USB20	2
#define UB_FLG_USB30	4

/*  *********************************************************************
    *  USB Pipe structure - one of these per unidirectional channel
    *  to an endpoint on a USB device
    ********************************************************************* */

#define UP_TYPE_CONTROL		1
#define UP_TYPE_BULK		(1 << 1)
#define UP_TYPE_INTR		(1 << 2)
#define UP_TYPE_ISOC		(1 << 3)
#define UP_TYPE_LOWSPEED	(1 << 4)
#define UP_TYPE_HIGHSPEED	(1 << 5)
#define UP_TYPE_SUPRSPEED	(1 << 6)
#define UP_TYPE_IN		(1 << 7)
#define UP_TYPE_OUT		(1 << 8)
#define UP_TYPE_HUB		(1 << 9)	/* this is a hub device */
#define UP_RPORT_SHIFT		10		/* RH port #        */
#define UP_RPORT_MASK		0x3		/* ...2 bits, 4 max */
#define UP_ROUTE_SHIFT		12		/* hub route string */
#define UP_ROUTE_MASK		0xfffff		/* ...20 bits */

typedef struct usbpipe_s {
	usb_ept_t *up_hwendpoint;	/* OHCI-specific endpoint pointer */
	usbdev_t *up_dev;	/* our device info */
	int up_num;		/* pipe number */
	int up_mps;		/* max packet size */
	int up_flags;	/* upper 16 bits used for USB 3.0 stuff */
} usbpipe_t;

/*  *********************************************************************
    *  USB device structure - one per device attached to the USB
    *  This is the basic structure applications will use to
    *  refer to a device.
    ********************************************************************* */

enum { FS, LS, HS, LSHS, SS };
#define UD_FLAG_HUB	0x0001	/* this is a hub device */
#define UD_FLAG_ROOTHUB	0x0002	/* this is a root hub device */
#define UD_FLAG_LOWSPEED 0x0008	/* this is a low speed device */
#define UD_FLAG_HIGHSPEED 0x0010 /* this is a high speed device */
#define UD_FLAG_SUPRSPEED 0x0020 /* this is a super speed device */
#define UD_FLAG_USB20BUS  0x0100
#define UD_FLAG_USB30BUS  0x0200
#define UD_FLAG_ERROR    0x8000	/* this device is in error state */

#define UD_MAX_PIPES	32
#define USB_EPADDR_TO_IDX(addr) ((((addr)&0x80) >> 3) | ((addr) & 0x0F))
//#define USB_EPADDR_TO_IDX(addr) USB_ENDPOINT_ADDRESS(addr)
//#define UD_MAX_PIPES  16

struct usbdev_s {
	usb_driver_t *ud_drv;	/* Driver's methods */
	usbbus_t *ud_bus;	/* owning bus */
	int ud_address;		/* USB address */
	usbpipe_t *ud_pipes[UD_MAX_PIPES];	/* pipes, 0 is control pipe */
	struct usbdev_s *ud_parent;	/* used for hubs */
	int ud_rport;		/* root port */
	int ud_route;		/* used for external hubs */
	int ud_tier;		/* used for external hubs */
	int ud_flags;		/* upper 16 bits used for USB 3.0 stuff */
	void *ud_private;	/* private data for device driver */
	usb_device_descr_t ud_devdescr;	/* device descriptor */
	usb_config_descr_t *ud_cfgdescr;  /* config, interface, and ep descrs */
};

/*  *********************************************************************
    *  USB Request - basic structure to describe an in-progress 
    *  I/O request.  It associates buses, pipes, and buffers 
    *  together.
    ********************************************************************* */

#define UR_FLAG_SYNC		0x8000

#define UR_FLAG_SETUP		0x0001
#define UR_FLAG_IN		0x0002
#define UR_FLAG_OUT		0x0004
#define UR_FLAG_STATUS_IN	0x0008	/* status phase of a control WRITE */
#define UR_FLAG_STATUS_OUT	0x0010	/* status phase of a control READ */
#define UR_FLAG_SHORTOK		0x0020	/* short transfers are ok */

typedef struct usbreq_s {
	queue_t ur_qblock;

	/*
	 * pointers to our device and pipe 
	 */

	usbdev_t *ur_dev;
	usbpipe_t *ur_pipe;

	/*
	 * stuff to keep track of the data we transfer
	 */

	uint8_t *ur_buffer;
	int ur_length;
	int ur_xferred;
	int ur_status;
	int ur_flags;

	/*
	 * Stuff needed for the callback
	 */
	void *ur_ref;
	int ur_inprogress;
	int (*ur_callback) (struct usbreq_s * req);

	/*
	 * For use inside the ohci driver
	 */
	void *ur_tdqueue;
	int ur_tdcount;
} usbreq_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

int usb_create_pipe(usbdev_t * dev, int pipenum, int mps, int flags);
void usb_destroy_pipe(usbdev_t * dev, int pipenum);
int usb_set_address(usbdev_t * dev, int addr);
usbdev_t *usb_create_device(usbbus_t *bus, int lowspeed, int rport, int route);
void usb_destroy_device(usbdev_t * dev);
usbreq_t *usb_make_request(usbdev_t * dev, int pipenum, uint8_t * buf,
			   int length, int flags);
void usb_poll(usbbus_t * bus);
void usb_daemon(usbbus_t * bus);
int usb_cancel_request(usbreq_t * ur);
void usb_free_request(usbreq_t * ur);
int usb_queue_request(usbreq_t * ur);
int usb_wait_request(usbreq_t * ur);
int usb_sync_request(usbreq_t * ur);
int usb_get_descriptor(usbdev_t * dev, uint8_t reqtype, int dsctype, int dscidx,
		       uint8_t * buffer, int buflen);
int usb_get_config_descriptor(usbdev_t * dev, usb_config_descr_t * dscr,
			      int idx, int maxlen);
int usb_get_device_status(usbdev_t * dev, usb_device_status_t * status);
int usb_set_configuration(usbdev_t * dev, int config);
int usb_open_pipe(usbdev_t * dev, usb_endpoint_descr_t * epdesc);
int usb_simple_request(usbdev_t * dev, uint8_t reqtype, int bRequest,
		       int wValue, int wIndex);
void usb_complete_request(usbreq_t * ur, int status);
int usb_get_device_descriptor(usbdev_t * dev, usb_device_descr_t * dscr,
			      int smallflg);
int usb_set_ep0mps(usbdev_t * dev, int mps);
int usb_new_address(usbbus_t * bus);
int usb_get_string(usbdev_t * dev, int id, char *buf, int maxlen);
int usb_std_request(usbdev_t * dev, uint8_t bmRequestType,
		    uint8_t bRequest, uint16_t wValue,
		    uint16_t wIndex, uint8_t * buffer, int length);
void *usb_find_cfg_descr(usbdev_t * dev, int dtype, int idx);
void usb_delay_ms(usbbus_t * bus, int ms);
int usb_clear_stall(usbdev_t * dev, int pipe);

void usb_scan(usbbus_t * bus);
void usbhub_map_tree(usbbus_t * bus, int (*func) (usbdev_t * dev, void *arg),
		     void *arg);
void usbhub_dumpbus(usbbus_t * bus, uint32_t verbose);

void usb_initroot(usbbus_t * bus);

/*  *********************************************************************
    *  Host Controller Driver
    *  Methods for abstracting the USB host controller from the 
    *  rest of the goop.
    ********************************************************************* */

struct usb_hcdrv_s {
	usbbus_t *(*hcdrv_create)(physaddr_t regaddr);
	void (*hcdrv_delete)(usbbus_t *);
	int (*hcdrv_start)(usbbus_t *);
	void (*hcdrv_stop)(usbbus_t *);
	int (*hcdrv_intr)(usbbus_t *);
	usb_ept_t *(*hcdrv_ept_create)(usbbus_t *, int usbaddr, int eptnum,
				int mps, int flags);
	void (*hcdrv_ept_delete)(usbbus_t *, usb_ept_t *);
	void (*hcdrv_ept_setmps)(usbbus_t *, usb_ept_t *, int mps);
	void (*hcdrv_ept_setaddr)(usbbus_t *, usb_ept_t *, int addr);
	void (*hcdrv_ept_cleartoggle)(usbbus_t *, usb_ept_t *);
	int (*hcdrv_xfer)(usbbus_t *, usb_ept_t *uept, usbreq_t *ur);
	void (*hcdrv_reset)(usbbus_t *);
};

#define UBCREATE(driver,addr) (*((driver)->hcdrv_create))(addr)
#define UBDELETE(bus) (*((bus)->ub_hwdisp->hcdrv_delete))(bus)
#define UBSTART(bus) (*((bus)->ub_hwdisp->hcdrv_start))(bus)
#define UBSTOP(bus) (*((bus)->ub_hwdisp->hcdrv_stop))(bus)
#define UBINTR(bus) (*((bus)->ub_hwdisp->hcdrv_intr))(bus)
#define UBEPTCREATE(bus,addr,num,mps,flags) (*((bus)->ub_hwdisp->hcdrv_ept_create))(bus,addr,num,mps,flags)
#define UBEPTDELETE(bus,ept) (*((bus)->ub_hwdisp->hcdrv_ept_delete))(bus,ept)
#define UBEPTSETMPS(bus,ept,mps) (*((bus)->ub_hwdisp->hcdrv_ept_setmps))(bus,ept,mps)
#define UBEPTSETADDR(bus,ept,addr) (*((bus)->ub_hwdisp->hcdrv_ept_setaddr))(bus,ept,addr)
#define UBEPTCLEARTOGGLE(bus,ept) (*((bus)->ub_hwdisp->hcdrv_ept_cleartoggle))(bus,ept)
#define UBXFER(bus,ept,xfer) (*((bus)->ub_hwdisp->hcdrv_xfer))(bus,ept,xfer)
#define UBRESET(bus) (*((bus)->ub_hwdisp->hcdrv_reset))(bus)

/*  *********************************************************************
    *  Devices - methods for abstracting things that _use_ USB
    *  (devices you can plug into the USB) - the entry points
    *  here are basically just for device discovery, since the top half
    *  of the actual driver will be device-specific.
    ********************************************************************* */

struct usb_driver_s {
	char *udrv_name;
	int (*udrv_attach) (usbdev_t *, usb_driver_t *);
	int (*udrv_detach) (usbdev_t *);
	int (*udrv_probe) (unsigned int, unsigned int);
};

typedef struct usb_drvlist_s {
	int udl_class;
	int udl_vendor;
	int udl_product;
	usb_driver_t *udl_disp;
} usb_drvlist_t;

extern usb_driver_t *usb_find_driver(usbdev_t * dev);
extern int usb_noisy;

#define CLASS_ANY	-1
#define VENDOR_ANY	-1
#define PRODUCT_ANY	-1

void mydelay(int x);

#define IS_HUB(dev) ((dev)->ud_devdescr.bDeviceClass == USB_DEVICE_CLASS_HUB)

/* How USB host controller is started */
enum {
	USBD_NOUSB = 0,
	USBD_ACTIVATED = 1,
	USBD_NORMAL = USBD_ACTIVATED,
	USBD_ONDEMAND = 2, /* available on command, but not activated */
};

/*  *********************************************************************
    *  Error codes
    ********************************************************************* */

#define USBD_ERR_OK		0	/* Request ok */
#define USBD_ERR_STALLED	-1	/* Endpoint is stalled */
#define USBD_ERR_IOERROR	-2	/* I/O error */
#define USBD_ERR_HWERROR	-3	/* Hardware failure */
#define USBD_ERR_CANCELED	-4	/* Request canceled */
#define USBD_ERR_NOMEM		-5	/* Out of memory */
#define USBD_ERR_TIMEOUT	-6	/* Request timeout */

/*  *********************************************************************
    *  Debug routines
    ********************************************************************* */

void usb_dbg_dumpportstatus(int port, usb_port_status_t * portstatus,
			    int level);
void usb_dbg_dumpdescriptors(usbdev_t * dev, uint8_t * ptr, int len);
void usb_dbg_dumpcfgdescr(usbdev_t * dev);
void usb_dbg_dumpeptdescr(usb_endpoint_descr_t * epdscr);
