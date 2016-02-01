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

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_queue.h"
#include "common.h"

#include "timer.h"
#include "ui_command.h"
#include "env_subr.h"

#include "usbchap9.h"
#include "usbd.h"
#include "usbdt.h"
#include "usb_externs.h"


/*  *********************************************************************
    *  Chip specific headers
    ********************************************************************* */
#include "bchp_usb_ctrl.h"
#include "bchp_usb_ohci.h"
#include "bchp_usb_ehci.h"


/*  *********************************************************************
    *  USB register (offsets)
    ********************************************************************* */
/* a few EHCI register offsets */
#define R_EHCI_USBCMD    0x10

/* a few OHCI register offsets */
#define R_OHCI_CONTROL         0x04
#define R_OHCI_CMDSTAT         0x08
#define R_OHCI_HcRhDescriptorA 0x48
#define R_OHCI_HcRhDescriptorB 0x4c


/*  *********************************************************************
    *  Chip specific address macros
    ********************************************************************* */
#define USB_CTRL_REG(base, reg)	(base + BCHP_USB_CTRL_##reg - \
		BCHP_USB_CTRL_SETUP)


/*  *********************************************************************
    *  Chip  endian constants
    ********************************************************************* */
#if defined(__BIG_ENDIAN)
#define USB_ENDIAN_CTL (BCHP_USB_CTRL_SETUP_FNHW_MASK)
#else
#define USB_ENDIAN_CTL (BCHP_USB_CTRL_SETUP_BABO_MASK | \
			BCHP_USB_CTRL_SETUP_FNHW_MASK)
#endif

#define USB_SETUP_MASK \
	(BCHP_USB_CTRL_SETUP_utmi_pls_en_MASK | \
	BCHP_USB_CTRL_SETUP_soft_reset_MASK | \
	BCHP_USB_CTRL_SETUP_IPP_MASK | \
	BCHP_USB_CTRL_SETUP_IOC_MASK | \
	BCHP_USB_CTRL_SETUP_WABO_MASK | \
	BCHP_USB_CTRL_SETUP_FNBO_MASK | \
	BCHP_USB_CTRL_SETUP_FNHW_MASK | \
	BCHP_USB_CTRL_SETUP_BABO_MASK)


/*
 * We keep track of the pointers to USB buses here.
 * One entry in this array per USB bus (the Opti controller
 * on the SWARM has two functions, so it's two buses)
 */
#define USB_MAX_BUS 8
static usbbus_t *usb_buses[USB_MAX_BUS];

static int in_poll, in_daemon, initdone, usb_buscnt;


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
    *  usb_init_one_ehci(addr)
    *
    *  Initialize one EHCI controller.
    *
    *  Input parameters:
    *	addr - physical address of OHCI registers
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */
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
		usb_buses[usb_buscnt++] = bus;
		usb_initroot(bus);
	}

	return 0;
}


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
static int usb_init_one_ohci(uint32_t addr)
{
	uint32_t reg;
	usbbus_t *bus;
	int res;

	/* Root Hub setup - Choose individual power and OCD modes */
	reg = DEV_RD(addr + R_OHCI_HcRhDescriptorA);
	reg |= 0x900;
	DEV_WR(addr + R_OHCI_HcRhDescriptorA, reg);
	DEV_WR(addr + R_OHCI_HcRhDescriptorB, 0xfe0000);

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
		usb_buses[usb_buscnt++] = bus;
		usb_initroot(bus);
	}

	return 0;
}


/*  *********************************************************************
    *  usb_reset_ohci()
    ********************************************************************* */
static void usb_reset_ohci(uint32_t addr)
{
	DEV_WR(addr + R_OHCI_CMDSTAT, BCHP_USB_OHCI_HcCommandStatus_HCR_MASK);
	DEV_WR(addr + R_OHCI_CONTROL, 0);
}


/*  *********************************************************************
    *  usb_reset_ehci()
    ********************************************************************* */
static void usb_reset_ehci(uint32_t addr)
{
	DEV_WR(addr + R_EHCI_USBCMD, 0);
	DEV_WR(addr + R_EHCI_USBCMD, BCHP_USB_EHCI_USBCMD_HCRESET_MASK);
}


/*  *********************************************************************
    *  usb_gen_workarounds()
    ********************************************************************* */
static void usb_gen_workarounds(uint32_t base)
{
	uint32_t reg;

	/* Disable OHCI transaction combining */
	reg = DEV_RD(USB_CTRL_REG(base, OBRIDGE));
	reg &= ~BCHP_USB_CTRL_OBRIDGE_OBR_SEQ_EN_MASK;
	DEV_WR(USB_CTRL_REG(base, OBRIDGE), reg);

	/* Avoid OUT underflows under heavy a/v processing
	 Set EHCI bridge FIFO threshold to 256 bytes
	 */
	reg = DEV_RD(USB_CTRL_REG(base, EBRIDGE));

	/* Disable EHCI transaction combining */
	reg &= ~BCHP_USB_CTRL_EBRIDGE_EBR_SEQ_EN_MASK;

	reg &= ~BCHP_USB_CTRL_EBRIDGE_EBR_SCB_SIZE_MASK;
	reg |= (0x08 << BCHP_USB_CTRL_EBRIDGE_EBR_SCB_SIZE_SHIFT);
	DEV_WR(USB_CTRL_REG(base, EBRIDGE), reg);
}


/*  *********************************************************************
    *  usb_platform_init()
    *
    *  Initialize the USB chip specific h/w
    *
    *  Input parameters:
    *	base - control register base offset
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */
static int usb_platform_init(struct usb_controller *ctl)
{
	uint32_t setup, ioc = 0, ipp = 0;
	uint32_t base = ctl->ctrl_regs;

	usb_gen_workarounds(base); /* per instance */

	/* 'setup' _may_ override things done in
	usb_gen_workarounds() in the future, so
	take note! */
	setup = DEV_RD(USB_CTRL_REG(base, SETUP));

	/*	   0 for positive polarity USB_PWRON,
		0x20 for negative polarity.
	*/
	ipp = ctl->ipp ? BCHP_USB_CTRL_SETUP_IPP_MASK : 0;

	/*	   0 for positive polarity USB_PWRON,
		0x10 for negative polarity USB_PWRFLT
	*/
	ioc = ctl->ioc ? BCHP_USB_CTRL_SETUP_IOC_MASK : 0;

	printf("USB @ 0x%x: IPP is active %s, ", base, (ipp?"low":"high"));
	printf("IOC is %stive polarity\n", (ioc?"nega":"posi"));

	/* Init power and endian control of setup register
	*/
	setup = (setup & ~(USB_SETUP_MASK)) | (ipp | ioc | USB_ENDIAN_CTL);

	DEV_WR(USB_CTRL_REG(base, SETUP), setup);

	return 0;
}


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
	struct usb_port *port;
	int x;
	int y;

	for (x = 0; x < usb_clist.cnt; x++) {
		if (usb_clist.ctrls[x].disabled)
			continue;
		port = &usb_clist.ctrls[x].ports[0];
		for (y = 0; y < usb_clist.ctrls[x].port_cnt; y++, port++)
			if (!port->disabled) {
				if (port->type == USB_PORT_TYPE_OHCI)
					usb_reset_ohci(port->regs);
				else if (port->type == USB_PORT_TYPE_EHCI)
					usb_reset_ehci(port->regs);
			}
	}
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
	int x;
	int y;
	int ehci_enabled = 1, ohci_enabled = 1, bdc_enabled = 1;

	if (initdone) {
		printf("USB has already been initialized.\n");
		return -1;
	}

	initdone = 1;
	usb_buscnt = 0; /* reset count so we can scan again. */

	if (env_getenv("EHCIOFF"))
		ehci_enabled = 0;

	if (env_getenv("OHCIOFF"))
		ohci_enabled = 0;

	if (env_getenv("BDCOFF"))
		bdc_enabled = 0;

	for (x = 0; x < usb_clist.cnt; x++) {

		if (usb_clist.ctrls[x].disabled)
			continue;

		/* Controller init */
		usb_platform_init(&(usb_clist.ctrls[x]));
		for (y = 0; y < usb_clist.ctrls[x].port_cnt; y++) {
			port = &usb_clist.ctrls[x].ports[y];

			/* Port init */
			if (!port->disabled) {
				if (port->type == USB_PORT_TYPE_EHCI) {
					if (ehci_enabled)
						usb_init_one_ehci(port->regs);
				} else if (port->type == USB_PORT_TYPE_OHCI) {
					if (ohci_enabled)
						usb_init_one_ohci(port->regs);
				} else if (port->type == USB_PORT_TYPE_BDC) {
					if (bdc_enabled &&
						usb_clist.ctrls[x].device_mode)
						usb_init_bdc(port->regs);
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
	if (cmd_sw_isset(cmd, "-o"))
		ohcidebug++;
	if (cmd_sw_isset(cmd, "-oo"))
		ohcidebug += 2;
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
