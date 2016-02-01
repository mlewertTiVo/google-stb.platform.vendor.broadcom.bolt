/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __USBDT_H__
#define __USBDT_H__

#define USB_MAX_CONTROLLERS 4
#define USB_MAX_PORTS_PER_CONTROLLER (USB_PORT_TYPE_MAX * 3)

#define USB_PORT_TYPE_OHCI 0
#define USB_PORT_TYPE_EHCI 1
#define USB_PORT_TYPE_XHCI 2
#define USB_PORT_TYPE_BDC  3
#define USB_PORT_TYPE_MAX 4

#define USB_CTLR_DEVICE_MODE_OFF 0
#define USB_CTLR_DEVICE_MODE_ON 1
#define USB_CTLR_DEVICE_MODE_DUAL 2

struct usb_controller_list {
	int cnt;
	struct usb_controller {
		uint32_t disabled;
		uint32_t ctrl_regs;
		uint32_t caps_regs;
		uint32_t ipp;
		uint32_t ioc;
		uint32_t device_mode;
		int port_cnt;
		struct usb_port {
			uint8_t disabled;
			uint8_t type;
			uint32_t regs;
		} ports[USB_MAX_PORTS_PER_CONTROLLER];
	} ctrls[USB_MAX_CONTROLLERS];
};


int usb_find_ports(struct usb_controller_list *clist);
void usb_remove_disabled_from_dt(struct usb_controller_list *clist);

#endif /* __USBDT_H__ */
