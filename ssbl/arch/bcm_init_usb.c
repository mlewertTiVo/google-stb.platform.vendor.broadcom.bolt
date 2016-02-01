/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#if CFG_USB

#include "board_init.h"
#include "bchp_common.h"
#include "board.h"
#include "chipid.h"
#include "devtree.h"
#include "usb-brcm-common-init.h"
#include "usbd.h"
#include "usbdt.h"

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_usb_ctrl.h"
#if defined(BCHP_USB1_CTRL_REG_START)
#include "bchp_usb1_ctrl.h"
#endif


/*  *********************************************************************
    *  board_init_usb()
    *
    *  Chip and board dependent USB initialization
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      USBD_NOUSB if USB should not be activated at all
    *      USBD_ACTIVATED if USB is to be initialized and ready to use
    *      USBD_ONDEMAND if USB is to be available, but only on command
    ********************************************************************* */
int board_init_usb(struct usb_controller_list *clist)
{
	int usbmode = CFG_USB_STARTMODE;
	int usbmode_userchoice;
	uint32_t __maybe_unused minor_rev;
	struct brcm_usb_common_init_params params;
	int x;

	/* Disable XHCI so 3.0 devices will be recognized as 2.0
	 * devices by BOLT.
	 */
	params.ioc = 0;
	params.ipp = 0;
	params.has_xhci = 0;

	for (x = 0; x < clist->cnt; x++) {
		struct usb_controller *ctl = &clist->ctrls[x];

		if (ctl->disabled)
			continue;
		params.ctrl_regs = ctl->ctrl_regs;
		params.device_mode = ctl->device_mode;
		brcm_usb_common_init(&params);
	}

#if defined(CONFIG_BCM7439B0)
	/* 7439B0, 7439B1 and so on if any */
	minor_rev = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID) &
		CHIPID_MINOR_REV_MASK;
	if (minor_rev == 0x0)
		/* prepare USB stack, but not start it up yet for B0 */
		usbmode = USBD_ONDEMAND;
#endif

	usbmode_userchoice = env_getval("USBMODE");
	if (usbmode_userchoice >= 0) {
		usbmode = usbmode_userchoice;
		/* always report USBMODE is set. */
		warn_msg("Override USB start mode %d -> %d",
			CFG_USB_STARTMODE, usbmode_userchoice);
	}

	return usbmode;
}

#endif /* CFG_USB */
