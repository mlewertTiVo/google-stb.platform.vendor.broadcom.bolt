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
#include "error.h"
#include "ioctl.h"
#include "devfuncs.h"
#include "env_subr.h"
#include "ui_command.h"
#include "bolt.h"
#include "bsp_config.h"
#include "common.h"
#include "reboot.h"
#include "fsbl-common.h"
#include "board.h"

static int ui_cmd_reboot(ui_cmdline_t *cmd, int argc, char *argv[]);
#if (CFG_CMD_LEVEL >= 3)
static int ui_cmd_loop(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_console(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
static int ui_cmd_info(ui_cmdline_t *cmd, int argc, char *argv[]);
#if CFG_MHL
static int ui_cmd_mhl_power(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
static int ui_cmd_time(ui_cmdline_t *cmd,int argc,char *argv[]);

int ui_init_misccmds(void)
{
	cmd_addcmd("reboot", ui_cmd_reboot, NULL,
		   "Reboot the system", "reboot\n", "");

#if (CFG_CMD_LEVEL >= 3)
	cmd_addcmd("loop", ui_cmd_loop, NULL,
		   "Loop a command",
		   "loop \"command\" [-count=*]\n"
		   "The 'loop' command causes the specified command or list of commands\n"
		   "to be repeated 'count' times or forever, or until a character is typed",
		   "-count=*;Specifies number of iterations|"
		   "-forever;Loops forever");

	cmd_addcmd("set console", ui_cmd_console, NULL,
		   "Change the active console device",
		   "set console device-name\n\n"
		   "Changes the console device to the specified device name.  The console\n"
		   "must be a serial-style device.  Be careful not to change the console\n"
		   "to a device that is not connected!", "");
#endif

	cmd_addcmd("info", ui_cmd_info, NULL,
		   "Show BOLT configuration information",
		   "This command displays information about the DDR, CPU speed, memory\n"
		   "and build configuration", "");

#if CFG_MHL
	cmd_addcmd("mhl power", ui_cmd_mhl_power, NULL,
		"Show the MHL bootup power",
		"Display the MHL power negotiated from a connected 'sink', if any\n",
		"");
#endif

	cmd_addcmd("time", ui_cmd_time, NULL,
		   "Timing utility.",
		   "time \"command\"\n\n"
		   "Measures and prints time taken for executing a bolt command in\n"
		   "'ms' and 'us'.\n",
		   "");

	return 0;
}

static int ui_cmd_time(ui_cmdline_t *cmd,int argc,char *argv[])
{
	uint64_t  t1, t2;
	uint64_t  delta, delta_us;
	const char *x;
	int res;

	x = cmd_getarg(cmd, 0);
	if (!x)
		return ui_showusage(cmd);

	t1 = arch_getticks64();
	res = bolt_docommands(x);
	t2 = arch_getticks64();

	delta = (t2 > t1) ? (t2 - t1) : (~0llu - t1 + t2);
	delta_us = delta/(arch_get_timer_freq_hz()/1000000);
	xprintf("\nCommand executed in : %llu ms (%llu us)\n",
		delta_us/1000, delta_us);

	if (res > 0)
		res = 0;

	return res;
}

#if CFG_MHL
static int ui_cmd_mhl_power(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct fsbl_info *info = board_info();
	uint32_t plims;
	const char *strpwr[8] = {
		"Nope", "500mA", "900mA", "1500mA",
		"Ivbus_dongle_to_source{min}", "2000mA", "Res1", "Res2"};

	plims = (info->runflags >> FSBL_RUNFLAG_MHL_SHIFT) &
		FSBL_RUNFLAG_MHL_MASK;

	xprintf("MHL power: %s.\n", strpwr[plims]);

	return BOLT_OK;
}
#endif

static int ui_cmd_reboot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	bolt_master_reboot();
	return 0;
}

#if (CFG_CMD_LEVEL >= 3)
static int ui_cmd_loop(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int count = 10;
	const char *x;
	int res;
	int forever;

	if (cmd_sw_value(cmd, "-count", &x))
		count = atoi(x);

	forever = cmd_sw_isset(cmd, "-forever");

	x = cmd_getarg(cmd, 0);
	if (!x)
		return ui_showusage(cmd);

	res = 0;
	while (count || forever) {
		if (console_status())
			break;
		res = bolt_docommands(x);
		if (res != 0)
			break;
		count--;
	}

	return res;
}

static int ui_cmd_console(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res;
	char *dev;

	dev = cmd_getarg(cmd, 0);
	if (!dev)
		return -1;	/* XXX usage */

	res = bolt_getdevinfo(dev);
	if (res < 0) {
		xprintf("Device '%s' is not valid\n", dev);
		return BOLT_ERR_DEVNOTFOUND;
	}

	if ((res & BOLT_DEV_MASK) != BOLT_DEV_SERIAL) {
		xprintf
		    ("Device '%s' is not the appropriate type to be a console\n",
		     dev);
		return BOLT_ERR_WRONGDEVTYPE;
	}

	bolt_set_console(dev);

	return 0;
}
#endif

static int ui_cmd_info(ui_cmdline_t *cmd, int argc, char *argv[])
{
	xprintf
	    ("========================================================================\n");
	bolt_config_info(1);
	xprintf
	    ("========================================================================\n");

	return 0;
}
