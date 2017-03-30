/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
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
#include "console.h"

#include "ui_command.h"
#include "timer.h"
#include "bolt.h"

#include "iocb.h"
#include "devfuncs.h"

#include "config.h"

#include "net_ssdp.h"

/*  *********************************************************************
    *  Configuration
    ********************************************************************* */

/*  *********************************************************************
    *  prototypes
    ********************************************************************* */

#if CFG_SSDP
static int ui_cmd_ssdp(ui_cmdline_t *cmd, int argc, char *argv[]);

/*  *********************************************************************
    *  ui_init_ssdpcmds()
    * Add SSDP-specific commands to the command table
    * Input parameters:
    * nothing
    * Return value:
    *0
    ********************************************************************* */

int ui_init_ssdpcmds(void)
{

	cmd_addcmd("ssdp",
			ui_cmd_ssdp,
			NULL,
			"Starts SSDP discovery protocol",
			"ssdp",
			"");

	return 0;
}

ssdp_context_t g_ctx;

static int ui_cmd_ssdp(ui_cmdline_t *cmd, int argc, char *argv[])
{
	ssdp_context_t *ctx = &g_ctx;

	/* initialize ssdp */
	if (BOLT_OK != ssdp_init(ctx))
		return BOLT_ERR;


	/* add it as a background task */
	bolt_bg_add(ssdp_poll, ctx);

	return BOLT_OK;
}

#endif /* CFG_SSDP */