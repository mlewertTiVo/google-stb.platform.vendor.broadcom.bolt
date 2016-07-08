/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "board.h"
#include "bolt.h"
#include "loader.h"
#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"

#include "tz.h"
#include "tz_priv.h"


/* The function tz_go() is a trimmed version of bolt_go() with the
 * following differeneces:
 *
 *   - device tree address is retrived from tz_info;
 *   - the last unused argument now indicates the UART used by bolt.
 */
int tz_go(bolt_loadargs_t *la)
{
	struct tz_info *t;
	struct fsbl_info *info;

	if (la->la_entrypt == 0) {
		xprintf("No program has been loaded.\n");
		return BOLT_ERR_INV_COMMAND;
	}

#if CFG_NETWORK
	if (!(la->la_flags & LOADFLG_NOCLOSE)) {
		if (net_getparam(NET_DEVNAME)) {
			xprintf("Closing network '%s'\n",
				(char *)net_getparam(NET_DEVNAME));
			net_uninit();
		}
	}
#endif

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	info = board_info();
	if (!info)
		return BOLT_ERR;

	xprintf("Starting TZ program at %#lx (DTB @ %p)\n\n",
		la->la_address, t->dt_addr);

	bolt_start(la->la_entrypt, 0xffffffff,
		(unsigned int)t->dt_addr, info->uart_base);

	return 0;
}


/* The function tz_boot() is a trimmed version of bolt_boot() with the
 * following differences:
 *
 *   - all batch related functionalities are removed (along with buffers);
 *   - at the end, instead of bolt_go(), tz_go() is called.
 */
int tz_boot(const char *ldrname, bolt_loadargs_t *la)
{
	int rc = 0;
	int noise = la->la_flags & LOADFLG_NOISY;

	la->la_entrypt = 0;

	if (noise)
		xprintf("Loading: ");

#if (CFG_CMD_LEVEL >= 5)
		xprintf(" load_program() params:\n");
		xprintf("   ldrname  [%s]\n", ldrname);
		xprintf("   filename [%s]\n", la->la_filename);
		xprintf("   filesys [%s]\n", la->la_filesys);
		xprintf("   device [%s]\n", la->la_device);
		xprintf("   options [%s]\n", la->la_options);
		xprintf("   la_flags %#08x\n", la->la_flags);
		xprintf("   address %#08lx\n", la->la_address);
		xprintf("   maxsize %#08lx\n", la->la_maxsize);
		xprintf("   entrypt %#08lx\n", la->la_entrypt);
		xprintf("\n");
#endif
	rc = bolt_load_program(ldrname, la);

	if (rc < 0) {
		if (noise)
			xprintf("Failed.\n");
		goto out;
	}

	if (noise)
		xprintf("Entry address is %#lx\n", la->la_entrypt);

	/*
	 * Banzai!  Run the program.
	 */

	if ((la->la_flags & LOADFLG_EXECUTE) && (la->la_entrypt != 0))
		rc = tz_go(la);

out:
	return rc;
}
