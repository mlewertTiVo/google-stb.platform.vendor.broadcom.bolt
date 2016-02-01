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
#include "timer.h"
#include "env_subr.h"
#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"

static int ui_cmd_devnames(ui_cmdline_t *cmd, int argc, char *argv[])
{
	queue_t *qb;
	bolt_device_t *dev;

	xprintf("Device Name          Description\n");
	xprintf("-------------------  ---------------------------------------------------------\n");

	for (qb = bolt_devices.q_next; qb != &bolt_devices; qb = qb->q_next) {
		dev = (bolt_device_t *) qb;

		xprintf("%19s  %s\n", dev->dev_fullname, dev->dev_description);

	}

	return 0;
}

#define POLL_SLEEP_TIME		10	/* mS */
static int bolt_devinit_wait(int cnt, const char *devname)
{
	queue_t *qb;
	bolt_device_t *dev;
	int i, res = 1;

	if (cnt)
		cnt = (cnt * 1000) / POLL_SLEEP_TIME;
	else
		cnt = 500 / POLL_SLEEP_TIME;  /* special case of 0.5 seconds */
	for (i = 0; (i < cnt) && res; ++i) {
		for (qb = bolt_devices.q_next; qb != &bolt_devices;
		     qb = qb->q_next) {
			dev = (bolt_device_t *) qb;
			if (lib_strstr(dev->dev_description, devname) &&
			    lib_strstr(dev->dev_description, "USB")) {
				res = 0;
				break;
			}
		}
		if (res)
			bolt_msleep(POLL_SLEEP_TIME);
	}

	return res;
}

static int ui_cmd_usbdev_wait(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res;
	int cnt = 10;  /* about 10 seconds */
	const char *s, *devname;

	devname = "USB";
	if (cmd_sw_value(cmd, "-t", &s))
		cnt = lib_atoi(s);
	if (cmd_sw_value(cmd, "-d", &s))
		devname = s;

	res = bolt_devinit_wait(cnt, devname);
	if (res)
		xprintf("USB device matching <%s> not found!\n", devname);

	return res;
}

static int ui_cmd_sleep(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int ms = 1000;
	char *s;

	s = cmd_getarg(cmd, 0);
	if (s)
		ms = lib_atoi(s);

	bolt_msleep(ms);

	return 0;
}

int ui_init_devcmds(void)
{
	cmd_addcmd("show devices", ui_cmd_devnames, NULL,
		   "Display information about the installed devices.",
		   "show devices\n\n"
		   "This command displays the names and descriptions of the devices\n"
		   "BOLT is configured to support.", "");

	cmd_addcmd("waitusb", ui_cmd_usbdev_wait, NULL,
		   "Wait for USB device to be installed.",
		   "waitusb [-t=timeout] [-d=device_name]\n\n"
		   "This command waits for a USB device to be recognized and installed.\n"
		   "The wait will time out after the specified time in seconds\n"
		   "(the default is 10 seconds).",
		   "-t=*;Specify new timeout period (0 = 0.5 seconds)|"
		   "-d=*;Specify device name to look for...partial names ok but case-sensitive (default=USB)");

	cmd_addcmd("sleep", ui_cmd_sleep, NULL,
		   "Sleep for specified milliseconds.",
		   "sleep [timeout]\n\n"
		   "This command sleeps for a specified time in mS (default is 1000 mS).\n",
		   "");

	return 0;
}
