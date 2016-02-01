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
#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"

/*  *********************************************************************
    *  The list of drivers we support.  If you add more drivers,
    *  list them here.
    ********************************************************************* */

extern usb_driver_t usbhub_driver;
extern usb_driver_t usbhid_driver;
extern usb_driver_t usbmass_driver;
extern usb_driver_t usbserial_driver;
extern usb_driver_t usbeth_driver;

usb_drvlist_t usb_drivers[] = {

	/*
	 * Hub driver
	 */

	{USB_DEVICE_CLASS_HUB, VENDOR_ANY, PRODUCT_ANY, &usbhub_driver},

	/*
	 * Keyboards and mice
	 */

#if (CFG_USB_HID == 1)
	{USB_DEVICE_CLASS_HUMAN_INTERFACE, VENDOR_ANY, PRODUCT_ANY,
	 &usbhid_driver},
#endif

	/*
	 * Mass storage devices
	 */

#if (CFG_USB_DISK == 1)
	{USB_DEVICE_CLASS_STORAGE, VENDOR_ANY, PRODUCT_ANY, &usbmass_driver},
#endif

	/*
	 * Communication Class devices
	 */

#if (CFG_USB_ETH == 1)
	{USB_DEVICE_CLASS_COMMUNICATIONS, VENDOR_ANY, PRODUCT_ANY,
	 &usbeth_driver},
#endif

	/*
	 * Serial ports
	 */

#if (CFG_USB_SERIAL == 1)
	{USB_DEVICE_CLASS_VENDOR_SPECIFIC, 0x557, 0x2008, &usbserial_driver},
	{USB_DEVICE_CLASS_VENDOR_SPECIFIC, 0x67b, 0x2303, &usbserial_driver},
#endif

	/*
	 * Ethernet Adapters...general probe done below
	 */

	{0, 0, 0, NULL}
};

/*  *********************************************************************
    *  usb_find_driver(class,vendor,product)
    *
    *  Find a suitable device driver to handle the specified
    *  class, vendor, or product.
    *
    *  Input parameters:
    *	   devdescr - device descriptor
    *
    *  Return value:
    *      pointer to device driver or NULL
    ********************************************************************* */

usb_driver_t *usb_find_driver(usbdev_t *dev)
{
	usb_device_descr_t *devdescr;
	usb_interface_descr_t *ifdescr;
	usb_drvlist_t *list;
	int dclass, vendor, product;

	devdescr = &(dev->ud_devdescr);

	dclass = devdescr->bDeviceClass;
	if (dclass == 0) {
		ifdescr =
		    usb_find_cfg_descr(dev, USB_INTERFACE_DESCRIPTOR_TYPE, 0);
		if (ifdescr)
			dclass = ifdescr->bInterfaceClass;
	}

	vendor = (int)GETUSBFIELD(devdescr, idVendor);
	product = (int)GETUSBFIELD(devdescr, idProduct);

	list = usb_drivers;
	while (list->udl_disp) {
		if (((list->udl_class == dclass)
		     || (list->udl_class == CLASS_ANY))
		    && ((list->udl_vendor == vendor)
			|| (list->udl_vendor == VENDOR_ANY))
		    && ((list->udl_product == product)
			|| (list->udl_product == PRODUCT_ANY))) {
			if (dclass != USB_DEVICE_CLASS_HUB)
				printf("usb: found %s %04x:%04x\n",
				       list->udl_disp->udrv_name,
				       vendor, product);
			return list->udl_disp;
		}
		list++;
	}

#if (CFG_USB_ETH == 1)
	/* Do a general USB-Ethernet probe to see if device matches */
	usb_driver_t *drv = &usbeth_driver;

	if (drv->udrv_probe(vendor, product)) {
		printf("usb: found %s %04x:%04x\n", drv->udrv_name,
		       vendor, product);
		return drv;
	}
#endif

	printf("usb: no driver found for %04x:%04x\n", vendor, product);

	return NULL;
}
