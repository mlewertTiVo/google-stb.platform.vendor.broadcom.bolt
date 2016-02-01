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

#include <stdarg.h>

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_setjmp.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "error.h"
#include "env_subr.h"
#include "bolt.h"
#include "ui_command.h"

/*  *********************************************************************
    *  Types
    ********************************************************************* */

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

static jmp_buf ui_jmpbuf;	/* for getting control in exceptions */

const char *ui_errstring(int errcode)
{
	return bolt_errortext(errcode);
}

int ui_showerror(int errcode, char *tmplt, ...)
{
	va_list marker;

	va_start(marker, tmplt);
	xvprintf(tmplt, marker);
	va_end(marker);
	xprintf(": %s\n", ui_errstring(errcode));

	return errcode;
}

int ui_docommand(const char *buffer)
{
	int res;
	ui_cmdline_t cmd;

	res = cmd_lookup(buffer, &cmd);
	if (res == 0) {

		res = cmd_sw_validate(&cmd, cmd.switches);
		if (res != -1) {
			xprintf("Invalid switch: %s\n", cmd_sw_name(&cmd, res));
			return BOLT_ERR_INV_PARAM;
		}

		if (lib_setjmp(ui_jmpbuf) != 0)
			return -1;
		res = (*cmd.func) (&cmd, cmd.argc - cmd.argidx,
				   &(cmd.argv[cmd.argidx]));
	}
	cmd_free(&cmd);
	return res;
}

void ui_restart(int arg)
{
	if (arg == 0)
		arg = -1;

	lib_longjmp(ui_jmpbuf, arg);
}

int ui_init_cmddisp(void)
{
	cmd_init();

	return 0;
}

int ui_showusage(ui_cmdline_t *cmd)
{
	cmd_showusage(cmd);

	return BOLT_ERR_INV_COMMAND;
}
