/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "common.h"

#include "timer.h"
#include "ui_command.h"
#include "env_subr.h"

#include "usbd.h"
#include "usbdt.h"
#include "usb_externs.h"
#include "usb-brcm-common-init.h"


#include "bchp_usb_ctrl.h"
/*
 * We keep track of the pointers to USB buses here.
 * Set max of 12 for 4 OHCI, 4 EHCI and 4 XHCI ports
 */
#define USB_MAX_BUS 12
static usbbus_t *usb_buses[USB_MAX_BUS];

static int in_poll, in_daemon, initdone, usb_buscnt;



/*  *********************************************************************
    *  usb_add_bus(arg)
    *
    *  Add a bus to the bus array. Check to make sure we don't exceed
    *  the max number of buses. Return 0 on success and -1 if
    *  we exceed the max number of buses.
    *
    *  Input parameters:
    *	bus - pointer to bus to add
    *
    *  Return value:
    *	0  - Success
    *   -1 - Failure
    ********************************************************************* */

static int usb_add_bus(usbbus_t *bus)
{
	if (usb_buscnt == (USB_MAX_BUS - 1)) {
		err_msg("USB: Error, exceeded max number of Buses\n");
		return -1;
	}
	usb_buses[usb_buscnt++] = bus;
	return 0;
}


/*  *********************************************************************
    *  usb_bolt_timer(arg)
    *
    *  This routine is called periodically by BOLT's timer routines
    *  to give the USB subsystem some time.  Basically we scan
    *  for work to do to manage configuration updates, and handle
    *  interrupts from the USB controllers.
    *
    *  Input parameters:
    *	arg - value we passed when the timer was initialized
    *	       (not used)
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usb_bolt_timer(void *arg)
{
	int idx;

	/*
	 *  Do not allow nested "interrupts" (stack recursion.)
	 *
	 *  We sometimes call the timer routines in here, which calls
	 * the polling loop.  This code is not reentrant, so
	 * prevent us from running the interrupt routine or
	 * bus daemon while we are already in there.
	 */

	/* new items *must* take priority */
	if (!in_poll) {
		in_poll = 1;

		for (idx = 0; idx < usb_buscnt; idx++) {
			if (usb_buses[idx])
				usb_poll(usb_buses[idx]);
		}
		in_poll = 0;
	}

	/* time to do stuff polling may have found for us? */
	if (!in_daemon) {
		in_daemon = 1;

		for (idx = 0; idx < usb_buscnt; idx++) {
			if (usb_buses[idx])
				usb_daemon(usb_buses[idx]);
		}
		in_daemon = 0;
	}
}

/*  *********************************************************************
    *  usb_init_one_xhci(addr)
    *
    *  Initialize one XHCI controller.
    *
    *  Input parameters:
    *	addr - physical address of XHCI registers
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */
#if CFG_USB_XHCI
static int usb_init_one_xhci(uint32_t addr)
{
	usbbus_t *bus;
	int res;

	if (env_getenv("USBDBG"))
		printf(" - XHCI controller at 0x%08X\n", addr);

	bus = UBCREATE(&xhci_driver, addr);
	if (bus == NULL) {
		printf("USB: Could not create XHCI driver structure ");
		printf("for controller at 0x%08X\n", addr);
		return -1;
	}

	bus->ub_num = usb_buscnt;

	res = UBSTART(bus);

	if (res != 0) {
		printf("USB: Could not init XHCI controller at 0x%08X\n", addr);
		UBSTOP(bus);
		return -1;
	} else {
		if (usb_add_bus(bus)) {
			UBSTOP(bus);
			return -1;
		}
		usb_initroot(bus);
	}

	return 0;
}
#else
static int usb_init_one_xhci(uint32_t addr)
{
	return 0;
}
#endif

/*  *********************************************************************
    *  usb_init_one_ehci(addr)
    *
    *  Initialize one EHCI controller.
    *
    *  Input parameters:
    *	addr - physical address of EHCI registers
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */
#if CFG_USB_EHCI_OHCI
static int usb_init_one_ehci(uint32_t addr)
{
	usbbus_t *bus;
	int res;

	if (env_getenv("USBDBG"))
		printf(" - EHCI controller at 0x%08X\n", addr);

	bus = UBCREATE(&ehci_driver, addr);
	if (bus == NULL) {
		printf("USB: Could not create EHCI driver structure ");
		printf("for controller at 0x%08X\n", addr);
		return -1;
	}

	bus->ub_num = usb_buscnt;

	res = UBSTART(bus);

	if (res != 0) {
		printf("USB: Could not init EHCI controller at 0x%08X\n", addr);
		UBSTOP(bus);
		return -1;
	} else {
		if (usb_add_bus(bus)) {
			UBSTOP(bus);
			return -1;
		}
		usb_initroot(bus);
	}

	return 0;
}
#else
static int usb_init_one_ehci(uint32_t addr)
{
	return 0;
}
#endif

/*  *********************************************************************
    *  usb_init_one_ohci(addr)
    *
    *  Initialize one USB controller.
    *
    *  Input parameters:
    *	addr - physical address of OHCI registers
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */
#if CFG_USB_EHCI_OHCI
static int usb_init_one_ohci(uint32_t addr)
{
	usbbus_t *bus;
	int res;

	if (env_getenv("USBDBG"))
		printf(" - OHCI controller at 0x%08X\n", addr);

	bus = UBCREATE(&ohci_driver, addr);
	if (bus == NULL) {
		printf("USB: Could not create OHCI driver structure ");
		printf("for controller at 0x%08X\n", addr);
		return -1;
	}

	bus->ub_num = usb_buscnt;

	res = UBSTART(bus);
	if (res != 0) {
		printf("USB: Could not init OHCI controller at 0x%08X\n", addr);
		UBSTOP(bus);
		return -1;
	} else {
		if (usb_add_bus(bus)) {
			UBSTOP(bus);
			return -1;
		}
		usb_initroot(bus);
	}

	return 0;
}
#else
static int usb_init_one_ohci(uint32_t addr)
{
	return 0;
}
#endif


/*  *********************************************************************
    *  usb_exit()
    *
    *  Disable the USB h/w by placing it in reset state
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */
int usb_exit(void)
{
	int idx;

	for (idx = 0; idx < usb_buscnt; idx++)
		if (usb_buses[idx])
			UBRESET(usb_buses[idx]);
	return 0;
}


/*  *********************************************************************
    *  usb_system_init()
    *
    *  Initialize the USB subsystem
    *   usb_dt_init() MUST be called before this function.
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */
static int usb_system_init(void)
{
	struct usb_port *port;
	uint32_t ctrl;
	int x;
	int y;
	int ehci_enabled = 1, ohci_enabled = 1, bdc_enabled = 1;
	int xhci_enabled = 1;
	static const char *ipp_msgs[] = {
		"active high",
		"active low",
		"determined by board strap"
	};

	if (initdone) {
		printf("USB has already been initialized.\n");
		return -1;
	}

	initdone = 1;
	usb_buscnt = 0; /* reset count so we can scan again. */

	if (env_getenv("XHCIOFF"))
		xhci_enabled = 0;

	if (env_getenv("EHCIOFF"))
		ehci_enabled = 0;

	if (env_getenv("OHCIOFF"))
		ohci_enabled = 0;

	if (env_getenv("BDCOFF"))
		bdc_enabled = 0;

	for (x = 0; x < usb_clist.cnt; x++) {

		if (usb_clist.ctrls[x].disabled)
			continue;
		if (env_getenv("USBDBG")) {
			printf("USB @ 0x%x: IPP polarity is %s, ", 
				usb_clist.ctrls[x].ctrl_regs,
				ipp_msgs[usb_clist.ctrls[x].ipp]);
			printf("IOC is %stive polarity\n",
				(usb_clist.ctrls[x].ioc ? "nega" : "posi"));
		}
		for (y = 0; y < usb_clist.ctrls[x].port_cnt; y++) {
			port = &usb_clist.ctrls[x].ports[y];
#ifdef BCHP_USB_CTRL_USBD_DRD_STATUS
			ctrl = usb_clist.ctrls[x].ctrl_regs +
				(BCHP_USB_CTRL_USBD_DRD_STATUS -
					BCHP_USB_CTRL_SETUP);
#else
			ctrl = 0;
#endif

			/* Port init */
			if (!port->disabled) {
				if (port->type == USB_PORT_TYPE_XHCI) {
					if (xhci_enabled)
						usb_init_one_xhci(port->regs);
				} else if (port->type == USB_PORT_TYPE_EHCI) {
					if (ehci_enabled)
						usb_init_one_ehci(port->regs);
				} else if (port->type == USB_PORT_TYPE_OHCI) {
					if (ohci_enabled)
						usb_init_one_ohci(port->regs);
				} else if (port->type == USB_PORT_TYPE_BDC) {
					if (bdc_enabled &&
						usb_clist.ctrls[x].device_mode)
						usb_init_bdc(port->regs, ctrl);
				}
			}
		}
	}
	bolt_bg_add(usb_bolt_timer, NULL);
	return 0;
}


static int usb_system_exit(void)
{
       if (!initdone) {
	       printf("USB has not been initialized.\n");
	       return -1;
       }

       initdone = 0;

       /* give usb a chance to finish processing
       any pending "interrupts". */
       POLL();

       bolt_bg_remove(usb_bolt_timer);
       POLL();

       return usb_exit();
}


/* ---------------- UI commands ---------------- */

static int ui_cmd_usb_start(ui_cmdline_t *cmd, int argc, char *argv[])
{
	if (cmd_sw_isset(cmd, "-u"))
		usb_noisy++;
	if (cmd_sw_isset(cmd, "-uu"))
		usb_noisy += 2;

	return usb_system_init();
}

static int ui_cmd_usb_stop(ui_cmdline_t * cmd, int argc, char *argv[])
{
	return usb_system_exit();
}

#if (CFG_CMD_LEVEL >= 2)
static int ui_cmd_usb_show(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int busnum;
	int devnum;
	char *x;
	int idx;
	uint32_t arg;

	if (!initdone) {
		printf("USB has NOT been initialized.\n");
		return -1;
	}

	x = cmd_getarg(cmd, 1);
	if (!x)
		devnum = 0;
	else
		devnum = atoi(x);

	x = cmd_getarg(cmd, 0);
	if (!x)
		x = "*";
	busnum = atoi(x);

	if (busnum >= usb_buscnt) {
		printf
		    ("Invalid bus number,  %d USB Buses currently configured.\n",
		     usb_buscnt);
		return -1;
	}

	arg = cmd_sw_isset(cmd, "-v") ? 0x100 : 0;
	arg |= (devnum & 0xFF);

	if (x[0] == '*') {
		for (idx = 0; idx < usb_buscnt; idx++)
			usbhub_dumpbus(usb_buses[idx], arg);
	} else {
		usbhub_dumpbus(usb_buses[busnum], arg);
	}

	return 0;

}
#endif


/*  *********************************************************************
    *  ui_init_usbcmds(void)
    *
    *  Initialize the USB commands
    *
    *  Input parameters:
    *	init - to add manual initialize command or not
    *
    *  Return value:
    *	0
    ********************************************************************* */
void ui_init_usbcmds(void)
{
	cmd_addcmd("usb init", ui_cmd_usb_start, NULL,
		   "Initialize the USB controller.",
		   "usb init",
		   "-o;OHCI debug messages|"
		   "-oo;more OHCI debug messages|"
		   "-u;USBD debug messages|"
		   "-uu;more USBD debug messages");

	cmd_addcmd("usb exit", ui_cmd_usb_stop, NULL,
		"Stop & reset the USB controller.",
		"usb exit",
		NULL);

#if (CFG_CMD_LEVEL >= 2)
	cmd_addcmd("show usb", ui_cmd_usb_show, NULL,
		"Display devices connected to USB bus.",
		"usb show [bus [device]]\n\n"
		"Displays the configuration descriptors for"
		" devices connected to the USB\n"
		"If you specify a bus, the entire bus is displayed. "
		"If you specify the\n device number as well, only the"
		" specified device is displayed\n",
		"-v;Display descriptors from the devices");
#endif
}


/*  *********************************************************************
    *  usb_init()
    *
    *  Initialize the USB driver
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */
int usb_init(int usbmode)
{
	initdone = 0;
	in_poll = 0;
	in_daemon = 0;

	switch (usbmode) {
	case USBD_NOUSB:
		break;

	default:
	case USBD_NORMAL:	/* normal mode...activated */
		usb_system_init();
		ui_init_usbcmds();
		break;

	case USBD_ONDEMAND:	/* USB only available on command */
		ui_init_usbcmds();
		break;
	}

	return 0;
}

/*  *********************************************************************
    *  usb_board_init()
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
int usb_board_init(struct usb_controller_list *clist)
{
	int usbmode = CFG_USB_STARTMODE;
	int usbmode_userchoice;
	uint32_t __maybe_unused minor_rev;
	struct brcm_usb_common_init_params params;
	int x;

	for (x = 0; x < clist->cnt; x++) {
		struct usb_controller *ctl = &clist->ctrls[x];

		if (ctl->disabled)
			continue;
		params.ctrl_regs = ctl->ctrl_regs;
		params.device_mode = ctl->device_mode;
		params.ioc = ctl->ioc;
		params.ipp = ctl->ipp;
		params.has_xhci = ctl->has_xhci;
		params.xhci_ec_regs = ctl->xhci_ec_regs;
		brcm_usb_common_init(&params);
	}

	usbmode_userchoice = env_getval("USBMODE");
	if (usbmode_userchoice >= 0) {
		usbmode = usbmode_userchoice;
		/* always report USBMODE is set. */
		warn_msg("Override USB start mode %d -> %d",
			CFG_USB_STARTMODE, usbmode_userchoice);
	}

	return usbmode;
}
