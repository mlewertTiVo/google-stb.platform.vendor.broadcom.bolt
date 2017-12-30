/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "board_init.h"
#include "devtree.h"
#include "usbdt.h"

/*  *********************************************************************
    *  board_device_reset()
    *
    *  Reset devices.  This call is done when the firmware is restarted,
    *  as might happen when an operating system exits, just before the
    *  "reset" command is applied to the installed devices.   You can
    *  do whatever board-specific things are here to keep the system
    *  stable, like stopping DMA sources, interrupts, etc.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void board_device_reset(void)
{
}


/*  *********************************************************************
    *  board_device_init()
    *
    *  Initialize and add other devices.  Add everything you need
    *  for bootstrap here, like disk drives, flash memory, UARTs,
    *  network controllers, etc.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void board_device_init(void)
{
	/* Set up AVS features, must be done
	 * before we do e.g. a temperature park
	 * action or monitoring.
	 */
	board_init_avs();

	board_init_flash();
#if CFG_GENET && CFG_ENET
	board_init_enet();
#endif
#if CFG_SATA
	board_init_sata();
#endif

	/* Moved RTS setup here as the splash screen start has relocated
	to before board_final_init() but after board_device_init() and
	so we ensure the display RTS is still setup correctly for it. */
	board_init_rts();
}


/*  *********************************************************************
    *  board_final_init()
    *
    *  Do any final initialization, such as adding commands to the
    *  user interface.

    *  If you don't want a user interface, put the startup code here.
    *  This routine is called just before BOLT starts its user interface.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void board_final_init(void)
{
	bolt_devtree_params_t params;
	void *fdt;
#if CFG_USB
	struct usb_controller_list *clist;
	int usbmode;
#endif

	bolt_devtree_getenvs(&params);
	fdt = params.dt_address;
	bolt_board_specific_mods(fdt);

#if CFG_SYSTEMPORT && CFG_ENET
	board_init_enet();
#endif
#if CFG_USB
	clist = usb_dt_init();
	usbmode = usb_board_init(clist);
	usb_init(usbmode);
#endif /* CFG_USB */
}

/*  *********************************************************************
    *  board_init_post_splash()
    *
    *  Do any initialization after starting splash.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void board_init_post_splash(void)
{
	/* A certain board may require special initialization for its
	 * HDMI output, but the generated splash run touches HDMI TX
	 * registers. Do such board specific initialization after starting
	 * splash.
	 */
	board_init_hdmi();
}

/*  *********************************************************************
    *  board_final_exit()
    *
    *  This call is done just before the firmware exits and jumps
    *  to a loaded program to run, e.g. the Linux kernel
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void board_final_exit(unsigned long *start_address)
{
#if CFG_SATA
	sata_exit();
#endif
#if CFG_USB
	usb_exit();
#endif

#if CFG_ENET
	enet_exit();
#endif
}

