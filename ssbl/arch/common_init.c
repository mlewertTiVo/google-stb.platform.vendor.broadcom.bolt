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
#include "iocb.h"
#include "device.h"
#include "bolt.h"
#include "lib_printf.h"
#include "lib_string.h"

#include "env_subr.h"
#include "devfuncs.h"

#include "board.h"
#include "board_init.h"

/*  *********************************************************************
    *  console_init()
    *
    *  Add the console device and set it to be the primary
    *  console.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void console_init(void)
{
	struct fsbl_info *inf = board_info();

	/* Add the serial port driver? If unset then
	 the fsbl (if configured) will o/p to console
	 but the SSBL won't. Linux depends on Devicetree
	 so is unaffected. Tested with Linux to autoboot
	 on a USB stick and FSBL hacked for inf->uart_base = 0.
	*/
	if (inf && inf->uart_base) {
		bolt_add_device(&bcm97XXX_uart, inf->uart_base, 0, 0);
		bolt_set_console("uart0");
	}

	bolt_add_device(&dev_mem, 0, 0, 0);
}
