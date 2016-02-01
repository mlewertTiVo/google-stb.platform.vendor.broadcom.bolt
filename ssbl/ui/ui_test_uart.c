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
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "devfuncs.h"
#include "timer.h"
#include "ioctl.h"

#include "error.h"

#include "ui_command.h"
#include "ui_init.h"

static int ui_cmd_uarttest(ui_cmdline_t *cmd, int argc, char *argv[]);

int ui_init_uarttestcmds(void)
{
	cmd_addcmd("test uart",
		   ui_cmd_uarttest,
		   NULL,
		   "Echo characters to a UART", "test uart [devname]", "");
	return 0;
}

static int ui_cmd_uarttest(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int fd;
	char *x;
	char ch;
	int res;
	char buffer[64];

	x = cmd_getarg(cmd, 0);
	if (!x)
		return ui_showusage(cmd);

	fd = bolt_open(x);
	if (fd < 0) {
		ui_showerror(fd, "could not open %s", x);
		return fd;
	}

	printf
	    ("Device open.  Stuff you type here goes there.  Type ~ to exit.\n");
	for (;;) {
		if (console_status()) {
			console_read((unsigned char *)&ch, 1);
			res = bolt_write(fd, (unsigned char *)&ch, 1);
			if (res < 0)
				break;
			if (ch == '~')
				break;
		}
		if (bolt_inpstat(fd)) {
			res =
			    bolt_read(fd, (unsigned char *)buffer,
				     sizeof(buffer));
			if (res > 0)
				console_write(buffer, res);
			if (res < 0)
				break;
		}
		POLL();
	}

	bolt_close(fd);
	return 0;
}
