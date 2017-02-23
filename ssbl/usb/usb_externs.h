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

#ifndef _USB_EXTERNS_H_
#define _USB_EXTERNS_H_

extern usb_hcdrv_t ohci_driver; /* OHCI Driver dispatch */
extern usb_hcdrv_t ehci_driver; /* EHCI Driver dispatch */
extern usb_hcdrv_t xhci_driver; /* XHCI Driver dispatch */

extern int ohcidebug;		/* OHCI debug control */

extern struct usb_controller_list usb_clist;
#if CFG_USB_BDC
extern int usb_init_bdc(physaddr_t base, physaddr_t ctrl);
#else
static inline int usb_init_bdc(physaddr_t base, physaddr_t ctrl) { return 0; };
#endif


#endif
