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
#include "ui_command.h"
#include "bolt.h"

struct netparam {
	const char *str;
	int num;
};

static const struct netparam speedtypes[] = {
	{"auto", ETHER_SPEED_AUTO},
	{"10hdx", ETHER_SPEED_10HDX},
	{"10fdx", ETHER_SPEED_10FDX},
	{"100hdx", ETHER_SPEED_100HDX},
	{"100fdx", ETHER_SPEED_100FDX},
	{"1000hdx", ETHER_SPEED_1000HDX},
	{"1000fdx", ETHER_SPEED_1000FDX},
	{NULL, 0}
};

static int ui_cmd_ethertest(ui_cmdline_t *cmd, int argc, char *argv[]);

int ui_init_ethertestcmds(void)
{
	cmd_addcmd("test ether",
		   ui_cmd_ethertest,
		   NULL,
		   "Do an ethernet test, reading packets from the net",
		   "test ether device-name",
		   "-speed=*;Specify speed|"
		   "-q;Be quiet|" "-send=*;Transmit packets");

	return 0;
}

static int ui_ifconfig_lookup(const char *name,
	const char *val, const struct netparam *list)
{
	const struct netparam *p = list;

	while (p->str) {
		if (strcmp(p->str, val) == 0)
			return p->num;
		p++;
	}

	xprintf("Invalid parameter for %s: Valid options are: ", name);

	p = list;
	while (p->str) {
		xprintf("%s ", p->str);
		p++;
	}

	xprintf("\n");
	return -1;
}

static int ui_cmd_ethertest(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *tok;
	int fh;
	uint8_t packet[2048];
	int res;
	int idx;
	int speed = ETHER_SPEED_AUTO;
	const char *x;
	int count = 0;
	int quiet;
	const uint8_t sdata[15] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x40,
		0x00, 0x00, 0x10, 0x00, 0x00, 0x12, 0x34, 0x00};

	tok = cmd_getarg(cmd, 0);
	if (!tok)
		return -1;

	if (cmd_sw_value(cmd, "-speed", &x)) {
		speed = ui_ifconfig_lookup("-speed", x, speedtypes);
		if (speed < 0)
			return BOLT_ERR_INV_PARAM;
	}

	quiet = cmd_sw_isset(cmd, "-q");

	fh = bolt_open(tok);
	if (fh < 0) {
		xprintf("Could not open device: %s\n", bolt_errortext(fh));
		return fh;
	}

	if (speed != ETHER_SPEED_AUTO) {
		xprintf("Setting speed to %d...\n", speed);
		bolt_ioctl(fh, IOCTL_ETHER_SETSPEED, &speed,
			  sizeof(speed), NULL, 0);
	}

	if (cmd_sw_value(cmd, "-send", &x)) {
		count = atoi(x);
		memset(packet, 0xEE, sizeof(packet));
		memcpy(packet, sdata, sizeof(sdata));
		res = 0;
		for (idx = 0; idx < count; idx++) {
			res = bolt_write(fh, packet, 128);
			if (res < 0)
				break;
		}

		if (res)
			ui_showerror(res, "Could not transmit packet");

		bolt_close(fh);
		return 0;
	}

	xprintf("Receiving... press enter to stop\n");
	while (!console_status()) {
		if (!bolt_inpstat(fh))
				continue;
		res = bolt_read(fh, packet, sizeof(packet));
		if (res < 0)
			xprintf("Read error: %s (%d)\n",
					bolt_errortext(res), res);

		if (res <= 0)
			continue;

		if (!quiet) {
			xprintf("%4d ", res);
			if (res > 32)
				res = 32;

			for (idx = 0; idx < res; idx++) {
				xprintf("%02X", packet[idx]);
				if ((idx == 5) || (idx == 11) || (idx == 13))
					xprintf(" ");
			}

			xprintf("\n");
		}

		count++;
		if (quiet && !(count % 1000))
			printf(".");
	}

	printf("Total packets received: %d\n", count);

	bolt_close(fh);

	return 0;
}
