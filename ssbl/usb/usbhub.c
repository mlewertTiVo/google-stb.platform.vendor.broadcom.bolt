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
#include "console.h"
#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"

/*  *********************************************************************
    *  Macros for common hub requests
    ********************************************************************* */

#define usbhub_set_port_feature(dev, port, feature) \
	usb_simple_request(dev, 0x23, USB_HUBREQ_SET_FEATURE, feature, port)

#define usbhub_set_hub_feature(dev, feature) \
	usb_simple_request(dev, 0x20, USB_HUBREQ_SET_FEATURE, feature, 0)

#define usbhub_clear_port_feature(dev, port, feature) \
	usb_simple_request(dev, 0x23, USB_HUBREQ_CLEAR_FEATURE, feature, port)

#define usbhub_clear_hub_feature(dev, feature) \
	usb_simple_request(dev, 0x20, USB_HUBREQ_CLEAR_FEATURE, feature, 0)

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static int usbhub_attach(usbdev_t *dev, usb_driver_t *drv);
static int usbhub_detach(usbdev_t *dev);

/*  *********************************************************************
    *  Hub-specific data structures
    ********************************************************************* */

#define USB3_HUB_CCS		1
#define USB3_HUB_PED		(1 << 1)
#define USB3_HUB_PR		(1 << 4)
#define USB3_HUB_PP		(1 << 9)
#define USB3_HUB_PS_SHIFT	10
#define USB3_HUB_PS_MASK	0xf

#define UHUB_MAX_DEVICES	8
#define UHUB_FLG_NEEDSCAN	1

typedef struct usbhub_softc_s {
	usb_hub_descr_t uhub_descr;
	usb_hub_status_t uhub_status;
	int uhub_ipipe;
	int uhub_ipipemps;
	int uhub_nports;
	unsigned int uhub_flags;
	uint8_t *uhub_imsg;
	usbdev_t *uhub_devices[UHUB_MAX_DEVICES];
} usbhub_softc_t;

usb_driver_t usbhub_driver = {
	"USB Hub",
	usbhub_attach,
	usbhub_detach,
	NULL
};

/*  *********************************************************************
    *  usbhub_ireq_callback(ur)
    *
    *  this routine is called when the transfer we queued to the
    *  interrupt endpoint on the hub completes.  It means that
    *  *some* port on the hub needs attention.  The data indicates
    *  which port, but for our purposes we don't really care - if
    *  we get this callback, we'll set a flag and re-probe the bus.
    *
    *  Input parameters:
    *	ur - usbreq that completed
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbhub_ireq_callback(usbreq_t *ur)
{
	int idx;
	usbhub_softc_t *uhub = (ur->ur_dev->ud_private);

	/*
	 * Check to see if the request was cancelled by someone
	 * deleting our endpoint.
	 */

	if (ur->ur_status == 0xFF) {
		usb_free_request(ur);
		return 0;
	}

	/*
	 * Check to see if any of our ports need attention
	 */

	for (idx = 1; idx <= uhub->uhub_nports; idx++) {
		if (ur->ur_buffer[0] & (1 << idx)) {

			/*
			 * Mark the hub as needing a scan, and mark the bus as well
			 * so the top-level polling will notice.
			 */

			uhub->uhub_flags |= UHUB_FLG_NEEDSCAN;
			ur->ur_dev->ud_bus->ub_flags |= UB_FLG_NEEDSCAN;
		}
	}

	/*
	 * Do NOT requeue the request here.  We will do this
	 * during exploration.
	 */

	usb_free_request(ur);

	return 0;
}

/*  *********************************************************************
    *  usbhub_get_hub_descriptor(dev,dscr,idx,maxlen)
    *
    *  Obtain the hub descriptor (special for hubs) from the
    *  device.
    *
    *  Input parameters:
    *	dev - usb device
    *	dscr - place to put hub descriptor
    *	idx - which hub descriptor to get (usually zero)
    *	maxlen - max # of bytes to return
    *
    *  Return value:
    *	# of bytes returned
    ********************************************************************* */

static int usbhub_get_hub_descriptor(usbdev_t *dev, usb_hub_descr_t *dscr,
				     int idx, int maxlen)
{
	uint32_t wVal = (dev->ud_flags & UD_FLAG_SUPRSPEED) ? 0x2A00 : 0x2900;

	return usb_std_request(dev, 0xA0,
				USB_HUBREQ_GET_DESCRIPTOR,
				wVal, 0, (uint8_t *) dscr, maxlen);
}

/*  *********************************************************************
    *  usbhub_get_hub_status(dev,status)
    *
    *  Obtain the hub status (special for hubs) from the
    *  device.
    *
    *  Input parameters:
    *	dev - usb device
    *	status - where to put hub status structure
    *
    *  Return value:
    *	# of bytes returned
    ********************************************************************* */

#if 0
static int usbhub_get_hub_status(usbdev_t *dev, usb_hub_status_t *status)
{
	return usb_std_request(dev,
			       0xA0,
			       0x00,
			       0,
			       0, (uint8_t *) status, sizeof(usbhub_status_t));
}
#endif

/*  *********************************************************************
    *  usbhub_get_port_status(dev,port,status)
    *
    *  Obtain the port status for a particular port from
    *  device. For USB 3.0, speed representation is driver specific.
    *
    *  Input parameters:
    *	dev - usb device
    *      port - 1-based port number
    *	status - where to put port status structure
    *
    *  Return value:
    *	# of bytes returned
    ********************************************************************* */

static int usbhub_get_port_status(usbdev_t *dev, int port,
				  usb_port_status_t *status)
{
	int res, spd;
	uint16_t curr, change;

	res = usb_std_request(dev,
			      0xA3,
			      0,
			      0,
			      port,
			      (uint8_t *) status, sizeof(usb_port_status_t));
	if (dev->ud_flags & UD_FLAG_SUPRSPEED) {
		curr = GETUSBFIELD((status), wPortStatus);
		change = GETUSBFIELD((status), wPortChange);
		change &= USB_PORT_FEATURE_C_MASK;
		spd = (curr >> USB3_HUB_PS_SHIFT) & USB3_HUB_PS_MASK;
		curr = (curr & USB3_HUB_CCS) | (curr & USB3_HUB_PED) |
			(curr & USB3_HUB_PR) | ((curr & USB3_HUB_PP) >> 1);
		/*  root hub vs downstream hub...different interpretation */
		if (dev->ud_parent)
			curr |= USB_PORT_STATUS_SUPRSPD;
		else {
			switch (spd) {
			case 2:
				curr |= USB_PORT_STATUS_LOWSPD;
				break;
			case 3:
				curr |= USB_PORT_STATUS_HIGHSPD;
				break;
			case 4:
				curr |= USB_PORT_STATUS_SUPRSPD;
				break;
			default:
				break;
			}
		}
		PUTUSBFIELD((status), wPortStatus, curr);
		PUTUSBFIELD((status), wPortChange, change);
	}

	return res;
}

/*  *********************************************************************
    *  usbhub_set_hub_depth(dev, depth)
    *
    *  Set hub depth for SS hub.
    *  device.
    *
    *  Input parameters:
    *	dev - usb device
    *	depth - tier level
    *
    *  Return value:
    *	transfer status
    ********************************************************************* */

static int usbhub_set_hub_depth(usbdev_t *dev, int depth)
{
	return usb_simple_request(dev, 0x20, 0x0C, depth, 0);
}

/*  *********************************************************************
    *  usbhub_queue_intreq(dev,softc)
    *
    *  Queue the transfer to the interrupt pipe that will catch
    *  the hub's port status changes
    *
    *  Input parameters:
    *	dev - usb device
    *	softc - hub-specific data
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usbhub_queue_intreq(usbdev_t *dev, usbhub_softc_t *softc)
{
	usbreq_t *ur;

	ur = usb_make_request(dev,
			      softc->uhub_ipipe,
			      softc->uhub_imsg, softc->uhub_ipipemps,
			      UR_FLAG_IN | UR_FLAG_SHORTOK);

	ur->ur_callback = usbhub_ireq_callback;

	usb_queue_request(ur);
}

/*  *********************************************************************
    *  usbhub_attach(dev,drv)
    *
    *  This routine is called when the hub attaches to the system.
    *  We complete initialization for the hub and set things up so
    *  that an explore will happen soon.
    *
    *  Input parameters:
    *	dev - usb device
    *	drv - driver structure
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbhub_attach(usbdev_t *dev, usb_driver_t *drv)
{
	usb_device_status_t devstatus;
	usb_device_descr_t *devdscr;
	usb_config_descr_t *cfgdscr;
	usb_endpoint_descr_t *epdscr;
	usbhub_softc_t *softc;
	uint16_t ver;

	/*
	 * Remember the driver dispatch.
	 */

	dev->ud_drv = drv;

	softc = KMALLOC(sizeof(usbhub_softc_t), DMA_BUF_ALIGN);
	memset(softc, 0, sizeof(usbhub_softc_t));
	softc->uhub_imsg = KMALLOC(8, DMA_BUF_ALIGN);
	dev->ud_private = softc;
	if (dev->ud_parent) {	/* only for downstrean hubs */
		dev->ud_tier = dev->ud_parent->ud_tier + 1;
		dev->ud_flags |= UD_FLAG_HUB;
	} else
		dev->ud_flags |= UD_FLAG_ROOTHUB;

	/* Check if USB 3.0 hub */
	devdscr = &dev->ud_devdescr;
	ver = GETUSBFIELD(devdscr, bcdUSB);
	if (ver >= 0x0300)
		dev->ud_flags |= UD_FLAG_SUPRSPEED;

	/*
	 * Dig out the data from the configuration descriptor
	 * (we got this from the device before attach time)
	 */

	cfgdscr = dev->ud_cfgdescr;
	epdscr = usb_find_cfg_descr(dev, USB_ENDPOINT_DESCRIPTOR_TYPE, 0);

	/*
	 * Get device status (is this really necessary?)
	 */

	usb_get_device_status(dev, &devstatus);

	/*
	 * Set us to configuration index 0
	 */

	usb_set_configuration(dev, cfgdscr->bConfigurationValue);

	/*
	 * Get the hub descriptor.  Get the first 8 bytes first, then get the rest
	 * if there is more.
	 */

	if (usbhub_get_hub_descriptor
	    (dev, &(softc->uhub_descr), 0,
	     USB_HUB_DESCR_SIZE) > USB_HUB_DESCR_SIZE) {
		usbhub_get_hub_descriptor(dev, &(softc->uhub_descr), 0,
					  softc->uhub_descr.bDescriptorLength);
	}

	/*
	 * remember stuff from the hub descriptor
	 */

	softc->uhub_nports = softc->uhub_descr.bNumberOfPorts;

	/* For SS hubs, set tier depth */
	if ((dev->ud_flags & UD_FLAG_SUPRSPEED) && dev->ud_tier)
		usbhub_set_hub_depth(dev, (dev->ud_tier-1));

	/*
	 * Open the interrupt pipe
	 */

	softc->uhub_ipipe = usb_open_pipe(dev, epdscr);
	softc->uhub_ipipemps = GETUSBFIELD(epdscr, wMaxPacketSize);

	/*
	 * Mark the bus and the hub as needing service.
	 */

	softc->uhub_flags |= UHUB_FLG_NEEDSCAN;
	dev->ud_bus->ub_flags |= UB_FLG_NEEDSCAN;

	/*
	 * Okay, that's it.  The top-level USB daemon will notice
	 * that the bus needs service and will invoke the exploration code.
	 * This may in turn require additional explores until
	 * everything settles down.
	 */

	return 0;
}

/*  *********************************************************************
    *  usbhub_detach(dev)
    *
    *  Called when a hub is removed from the system - we remove
    *  all subordinate devicees.
    *
    *  Input parameters:
    *	dev - device (hub) that was removed
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbhub_detach(usbdev_t *dev)
{
	usbhub_softc_t *hub;
	usbdev_t *deldev;
	int idx;

	if (!IS_HUB(dev))
		return 0;	/* should not happen */

	hub = dev->ud_private;
	for (idx = 0; idx < UHUB_MAX_DEVICES; idx++) {
		deldev = hub->uhub_devices[idx];
		if (deldev) {
			console_log
			    ("USB: Removing device on bus %d hub %d port %d",
			     dev->ud_bus->ub_num, (dev->ud_tier+1), idx + 1);
			if (deldev->ud_drv) {
				(*(deldev->ud_drv->udrv_detach)) (deldev);
			} else {
				if (usb_noisy > 0) {
					console_log
					    ("USB: Detached device on bus %d hub %d port %d "
					     "has no methods",
					     dev->ud_bus->ub_num,
					     (dev->ud_tier+1), idx + 1);
				}
			}
			usb_destroy_device(deldev);
		}
	}

	KFREE(hub->uhub_imsg);
	KFREE(hub);		/* remove softc */

	return 0;
}

/*  *********************************************************************
    *  usbhub_map_tree1(dev,level,func,arg)
    *
    *  This routine is used in recursive device tree exploration.
    *  We call 'func' for each device at this tree, and descend
    *  when we run into hubs
    *
    *  Input parameters:
    *	dev - current device pointer
    *	level - current nesting level
    *	func - function to call
    *	arg - argument to pass to function
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usbhub_map_tree1(usbdev_t *dev, int level,
			     int (*func)(usbdev_t *dev, void *arg), void *arg)
{
	usbhub_softc_t *hub;
	int idx;

	(*func) (dev, arg);

	if (IS_HUB(dev)) {
		hub = dev->ud_private;
		for (idx = 0; idx < UHUB_MAX_DEVICES; idx++) {
			if (hub->uhub_devices[idx]) {
				usbhub_map_tree1(hub->uhub_devices[idx],
						 level + 1, func, arg);
			}
		}
	}
}

/*  *********************************************************************
    *  usbhub_map_tree(bus,func,arg)
    *
    *  Call a function for each device in the tree
    *
    *  Input parameters:
    *	bus - bus to scan
    *	func - function to call
    *	arg - argument to pass to function
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void usbhub_map_tree(usbbus_t *bus, int (*func) (usbdev_t *dev, void *arg),
		     void *arg)
{
	usbhub_map_tree1(bus->ub_roothub, 0, func, arg);
}

/*  *********************************************************************
    *  usbhub_dumpbus1(dev,arg)
    *
    *  map function to dump devices in the device tree
    *
    *  Input parameters:
    *	dev - device we're working on
    *	arg - argument from map_tree call
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbhub_dumpbus1(usbdev_t *dev, void *arg)
{
	uint32_t *verbose = (uint32_t *) arg;

	if ((*verbose & 0x00FF) &&
		((uint32_t)dev->ud_address != (*verbose & 0x00FF)))
		return 0;

	if (*verbose & 0x100) {
		printf
		    ("============================================================================\n");
	}

	printf("Bus %d Device %d   Class %d Vendor %04X Product %04X  ",
	       dev->ud_bus->ub_num,
	       dev->ud_address,
	       dev->ud_devdescr.bDeviceClass,
	       GETUSBFIELD(&(dev->ud_devdescr), idVendor),
	       GETUSBFIELD(&(dev->ud_devdescr), idProduct));

	if (IS_HUB(dev))
		printf("[HUB]\n");
	else
		printf("[DEVICE]\n");

	if (*verbose & 0x100) {
		usb_dbg_dumpdescriptors(dev, (uint8_t *) &(dev->ud_devdescr),
					dev->ud_devdescr.bLength);
		usb_dbg_dumpcfgdescr(dev);
	}

	return 0;
}

/*  *********************************************************************
    *  usbhub_dumpbus(bus,verbose)
    *
    *  Dump information about devices on the USB bus.
    *
    *  Input parameters:
    *	bus - bus to dump
    *	verbose - nonzero to display more info, like descriptors
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void usbhub_dumpbus(usbbus_t *bus, uint32_t verbose)
{
	usbhub_map_tree(bus, usbhub_dumpbus1, &verbose);
}

/*  *********************************************************************
    *  usbhub_reset_devicee(dev,port,status)
    *
    *  Reset a device on a hub port.  This routine does a
    *  USB_PORT_FEATURE_RESET on the specified port, waits for the
    *  bit to clear, and returns.  It is used to get a device into the
    *  DEFAULT state according to the spec.
    *
    *  Input parameters:
    *	dev - hub device
    *	port - port number(1-based)
    *	status - place to return port_status structure after
    *	        reset completes
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

#define USB_HUB_RESET_WAIT	50 /* mS */

/* mS...power stabilization time, as per USB spec */
#define USB_DEV_ACCESS_WAIT	100

static void usbhub_reset_device(usbdev_t *dev, int port,
				usb_port_status_t *portstatus)
{
	usb_delay_ms(dev->ud_bus, USB_DEV_ACCESS_WAIT);

	console_log("usb: resetting device on bus %d hub %d port %d",
		    dev->ud_bus->ub_num, (dev->ud_tier+1), port);
	usbhub_set_port_feature(dev, port, USB_PORT_FEATURE_RESET);
	usbhub_get_port_status(dev, port, portstatus);

	for (;;) {
		usbhub_get_port_status(dev, port, portstatus);
		if ((GETUSBFIELD((portstatus), wPortStatus) &
		     USB_PORT_STATUS_RESET) == 0)
			break;
		usb_delay_ms(dev->ud_bus, USB_HUB_RESET_WAIT);
	}

	usb_delay_ms(dev->ud_bus, USB_HUB_RESET_WAIT);
	usbhub_clear_port_feature(dev, port, USB_PORT_FEATURE_C_PORT_RESET);
}

/*  *********************************************************************
    *  usbhub_setup_device(dev,ps,idx)
    *
    *  Sets up a new device.
    *
    *  Input parameters:
    *	dev - hub device
    *	ps  - current port status
    *	idx - port number
    *
    *  Return value:
    *	pointer to new device
    ********************************************************************* */

static usbdev_t *usbhub_setup_device(usbdev_t *dev, uint16_t ps, int idx)
{
	int spd;
	usbdev_t *newdev;
	char *st;
	int rport = 0, route = 0;

	if (ps & USB_PORT_STATUS_SUPRSPD) {
		spd = SS;
		st = "Super";
	} else if (ps & USB_PORT_STATUS_HIGHSPD) {
		spd = HS;
		st = "high";
	} else if (ps & USB_PORT_STATUS_LOWSPD) {
		spd = LS;
		st = "low";
	} else {
		spd = FS;
		st = "full";
	}
	console_log(
		"USB: New %s speed device connected to bus %d hub %d port %d",
		st,
		dev->ud_bus->ub_num,
		(dev->ud_tier+1),
		idx+1);

	/*
	 * Set up device info devices under hubs for USB 3.0 bus
	 * and high speed.
	 */

	if (dev->ud_parent) {
		if (dev->ud_bus->ub_flags & UB_FLG_USB30) {
			rport = dev->ud_rport;
			route = dev->ud_route |
				((idx+1) << ((dev->ud_tier-1)*4));
		} else if ((dev->ud_flags & UD_FLAG_USB20BUS) &&
			   ((spd == FS) || (spd == LS))) {
			if (dev->ud_flags & UD_FLAG_HIGHSPEED) {
				rport = idx + 1;
				route = dev->ud_address;
			} else { /* use parent HS info */
				rport = dev->ud_rport;
				route = dev->ud_route;
			}
		}
	} else {
		rport = idx;
		route = 0;
	}

	/*
	 * Create a device for this port and link it.
	 */

	newdev = usb_create_device(dev->ud_bus,
				   spd,
				   rport,
				   route);
	newdev->ud_parent = dev;
	if (dev->ud_bus->ub_flags & UB_FLG_USB30)
		newdev->ud_flags |= UD_FLAG_USB30BUS;
	else if (dev->ud_bus->ub_flags & UB_FLG_USB20)
		newdev->ud_flags |= UD_FLAG_USB20BUS;

	return newdev;
}

/*  *********************************************************************
    *  usbhub_scan_ports(dev,arg)
    *
    *  Scan the ports on this hub for new or removed devices.
    *
    *  Input parameters:
    *	dev - hub device
    *	arg - passed from bus scan main routines
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usbhub_scan_ports(usbdev_t *dev, void *arg)
{
	uint16_t current;
	uint16_t changed;
	usbhub_softc_t *softc;
	int idx;
	int res;
	int len;
	uint8_t *buf;
	usbdev_t *newdev;
	usb_driver_t *newdrv = 0;
	int addr;
	usb_port_status_t portstatus;
	usb_config_descr_t cfgdescr;
	unsigned int powerondelay;
	static int ocd = 10;	/* overcurrent counter */

	if (!IS_HUB(dev))
		return;		/* should not happen.  */

	/*
	 * We know this is a hub.  Get the softc back.
	 */

	softc = (usbhub_softc_t *) dev->ud_private;

	powerondelay =
	    ((unsigned int)softc->uhub_descr.bPowerOnToPowerGood) * 2 + 20;

	/*
	 * Turn on the power to the ports whose power is not yet on.
	 */

	for (idx = 0; idx < softc->uhub_nports; idx++) {

		usbhub_get_port_status(dev, idx + 1, &portstatus);

		current = GETUSBFIELD((&portstatus), wPortStatus);
		changed = GETUSBFIELD((&portstatus), wPortChange);
		if (usb_noisy > 1) {
			printf
			    ("BeforePowerup: port %d status %04X changed %04X\n",
			     idx + 1, current, changed);
		}

		if (!(current & USB_PORT_STATUS_POWER)) {
			if (usb_noisy > 1)
				console_log("USB: Powering up bus %d port %d",
					    dev->ud_bus->ub_num, idx + 1);
			usbhub_set_port_feature(dev, idx + 1,
						USB_PORT_FEATURE_POWER);
			usb_delay_ms(dev->ud_bus, powerondelay);
		}
	}

	/*
	 * Begin exploration at this level.
	 */

	for (idx = 0; idx < softc->uhub_nports; idx++) {

		usbhub_get_port_status(dev, idx + 1, &portstatus);

		current = GETUSBFIELD((&portstatus), wPortStatus);
		changed = GETUSBFIELD((&portstatus), wPortChange);

		if (usb_noisy > 0) {
			printf
			    ("USB: Explore: Bus %d Hub %d port %d status %04X changed %04X\n",
			     dev->ud_bus->ub_num, (dev->ud_tier+1), (idx+1),
			     current, changed);
			usb_dbg_dumpportstatus(idx + 1, &portstatus, 1);
		}

/*		if (changed & USB_PORT_STATUS_RESET)
			usbhub_clear_port_feature(dev, idx+1,
				USB_PORT_FEATURE_C_PORT_RESET);
*/

		if (changed & USB_PORT_STATUS_ENABLED) {
			usbhub_clear_port_feature(dev, idx + 1,
						  USB_PORT_FEATURE_C_PORT_ENABLE);
		}

		if ((changed & USB_PORT_STATUS_OVERCUR)
		    || (current & USB_PORT_STATUS_OVERCUR)) {

			printf("USB: Bus %d Hub %d port %d in overcurrent ",
				dev->ud_bus->ub_num, (dev->ud_tier+1), (idx+1));

			if ((ocd == 0) || (current & USB_PORT_STATUS_OVERCUR)) {
				printf("state... not re-enabling power\n");
			} else {
				printf("overcurrent change...\n");

				usbhub_clear_port_feature(dev, idx + 1,
					USB_PORT_FEATURE_C_PORT_OVER_CURRENT);
				softc->uhub_flags |= UHUB_FLG_NEEDSCAN;
				dev->ud_bus->ub_flags |= UB_FLG_NEEDSCAN;
				--ocd;
			}
		}

		if (changed & USB_PORT_STATUS_CONNECT) {
			/*
			 * A device was either connected or disconnected.
			 * Clear the status change first.
			 */

			usbhub_clear_port_feature(dev, idx + 1,
				USB_PORT_FEATURE_C_PORT_CONNECTION);

			if (current & USB_PORT_STATUS_CONNECT) {

				/*
				 * The device has been CONNECTED.
				 * Reset the device to check speed.
				 * Reuse our old port status structureso we get
				 * the latest status for connect status.
				 * Full/Low speed devices will disconnect
				 * when handed over.
				 */

				usbhub_reset_device(dev, idx+1, &portstatus);

				current = GETUSBFIELD((&portstatus), wPortStatus);
				changed = GETUSBFIELD((&portstatus), wPortChange);

				if (changed & USB_PORT_STATUS_CONNECT) {
					usbhub_clear_port_feature(dev, idx + 1,
						USB_PORT_FEATURE_C_PORT_CONNECTION);
					goto done;
				}
			}

			if (current & USB_PORT_STATUS_CONNECT) {
				/*
				 * The device connection is stable.
				 */

				newdev = usbhub_setup_device(dev, current, idx);

				/*
				 * Get the device descriptor.
				 */

				res =
				    usb_get_device_descriptor(newdev,
							      &
							      (newdev->ud_devdescr),
							      TRUE);

				if (res > 0) {
					if (usb_noisy > 0)
						usb_dbg_dumpdescriptors(newdev,
									(uint8_t
									 *) &
									(newdev->ud_devdescr),
									8);

					/*
					 * Set up the max packet size for the control endpoint,
					 * set the new device's address and
					 * then get the rest of the descriptor.
					 */

					usb_set_ep0mps(newdev,
						       newdev->
						       ud_devdescr.bMaxPacketSize0);
					addr = usb_new_address(newdev->ud_bus);
					usb_set_address(newdev, addr);
					res =
					    usb_get_device_descriptor(newdev,
								      &(newdev->ud_devdescr),
								      FALSE);

					/*
					 * Get the configuration descriptor and all the
					 * associated interface and endpoint descriptors.
					 */

					res =
					    usb_get_config_descriptor(newdev,
								      &cfgdescr,
								      0,
								      sizeof
								      (usb_config_descr_t));
					if (res != sizeof(usb_config_descr_t)) {
						printf("USB: usb_get_config_descriptor(a) returns %d\n",
							res);
						goto cfg_desc_err;
					}

					len =
					    GETUSBFIELD(&cfgdescr,
							wTotalLength);
					buf = KMALLOC(len, DMA_BUF_ALIGN);

					res =
					    usb_get_config_descriptor(newdev,
								      (usb_config_descr_t
								       *) buf,
								      0, len);
					if (res != len) {
						printf("USB: usb_get_config_descriptor(b) returns %d\n",
							res);
						KFREE(buf);
						goto cfg_desc_err;
					}

					newdev->ud_cfgdescr =
					    (usb_config_descr_t *) buf;

					if (usb_noisy > 0)
						usb_dbg_dumpdescriptors(newdev,
									buf,
									len);

					/*
					 * Point the hub at the devices it owns
					 */

					softc->uhub_devices[idx] = newdev;
				}

				/*
				 * If no error, find the driver for this.
				 */

cfg_desc_err:
				if (newdev->ud_flags & UD_FLAG_ERROR) {
					printf
					    ("USB: Device disabled due to communication error!\n");
					usb_destroy_device(newdev);
					newdev = NULL;
				} else
					newdrv = usb_find_driver(newdev);

				/*
				 * Call the attach method.
				 */

				if (newdrv) {
					/* remember driver dispatch in device */
					newdev->ud_drv = newdrv;
					(*(newdrv->udrv_attach))
						(newdev, newdrv);
					newdrv = NULL;
				}
			}

			else {

				/*
				 * The device has been DISCONNECTED.
				 */

				if (dev->ud_flags & UD_FLAG_SUPRSPEED) {
					usbhub_clear_port_feature(
						dev,
						idx + 1,
						USB_PORT_FEATURE_C_LINK_STATE);
				}
				console_log
				    ("USB: Device disconnected from bus %d hub %d port %d",
				     dev->ud_bus->ub_num, (dev->ud_tier+1),
				     idx + 1);

				/*
				 * Recover pointer to device below hub and clear
				 * this pointer.
				 */

				/* Get device pointer */
				newdev = softc->uhub_devices[idx];

				/* remove device from hub */
				softc->uhub_devices[idx] = NULL;

				/*
				 * Deassign the USB device's address and then
				 * call detach method to free resources. Devices
				 * that do not have drivers will not have any
				 * methods.
				 */

				if (newdev) {
					if (newdev->ud_drv) {
						(*(newdev->ud_drv->udrv_detach))
						    (newdev);
					} else {
						if (usb_noisy > 0) {
							console_log
							    ("USB: Detached device on bus %d hub %d port %d "
							     "has no methods",
							     dev->
							     ud_bus->ub_num,
							     (dev->ud_tier+1),
							     idx + 1);
						}
					}
					usb_destroy_device(newdev);
					newdev = NULL;
				}

			}
		}

	}

done:
	/*
	 * Queue up a request for the interrupt pipe.  This will catch further
	 * changes at this port.
	 */

	usbhub_queue_intreq(dev, softc);

}

/*  *********************************************************************
    *  usbhub_scan1(dev,arg)
    *
    *  Scan one device at this level, or descend if we run into a hub
    *  This is part of the device discovery code.
    *
    *  Input parameters:
    *	dev - current device, maybe a hub
    *	arg - passed from main scan routine
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbhub_scan1(usbdev_t *dev, void *arg)
{
	usbhub_softc_t *hub;

	/*
	 * If the device is not a hub, we've reached the leaves of the
	 * tree.
	 */

	if (!IS_HUB(dev))
		return 0;

	/*
	 * Otherwise, scan the ports on this hub.
	 */

	hub = dev->ud_private;

	if (hub->uhub_flags & UHUB_FLG_NEEDSCAN) {
		hub->uhub_flags &= ~UHUB_FLG_NEEDSCAN;
		usbhub_scan_ports(dev, arg);
	}

	return 0;
}

/*  *********************************************************************
    *  usb_scan(bus)
    *
    *  Scan the bus looking for new or removed devices
    *
    *  Input parameters:
    *	bus - bus to scan
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void usb_scan(usbbus_t *bus)
{
	/*
	 * Call our tree walker with the scan function.
	 */

	usbhub_map_tree(bus, usbhub_scan1, NULL);
}
