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
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"

#include "board_init.h"
#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "bsp_config.h"
#include "byteorder.h"
#include "error.h"

#define PARAM_SZ 0x46
#define NUMADDRS   8
#define BUFLENGTH  (7 + 4 * NUMADDRS * 2)	/* matching non-os */
#define	BOARD_TYPE_MASK	0x1F
#define	SER_NBR_MASK	0xFF
#define	ADDR_NBR_MASK	0x07
#define BOARD_SER_NBR_OFFSET	1000

static char *macaddr_devname;

static int write_macbuf(uint16_t *macbuf, const int buflength)
{
	int i;
	int hdl;
	int retval;
	uint8_t checksum = 0;
	char macstr[18];
	const int bufsize_bytes = buflength * sizeof(uint16_t);

	/* calculate checksum. */
	checksum = 0;
	for (i = 0; i < PARAM_SZ; i++) {
		checksum += ((macbuf[i] & 0xff00) >> 8);
		checksum += (macbuf[i] & 0x00ff);
	}
	macbuf[PARAM_SZ] = ((checksum & 0xFF) << 8) | (checksum & 0xFF);

	hdl = bolt_open(macaddr_devname);
	if (hdl < 0)
		return BOLT_ERR_DEVNOTFOUND;

	xprintf("Programming flash...");

	retval = bolt_writeblk(hdl, (bolt_offset_t) 0x0,
			(unsigned char *)macbuf, bufsize_bytes);
	bolt_close(hdl);

	xprintf("done\n");

	if (retval == bufsize_bytes) {
		macaddr_flash_get(BCM7038MAC, macstr, 0);
		env_setenv(MACADDR_ENVSTR, macstr, ENV_FLG_BUILTIN);
		retval = 0;
	} else {
		xprintf("Failure while writing to flash.\n");
		xprintf("(%d != %d)\n", retval, bufsize_bytes);
		retval = -1;
	}

	return retval;
}

static int ui_cmd_macprog_nibble(ui_cmdline_t *cmd, int argc, char **argv)
{
	int i;
	int mac_idx = 0;
	uint16_t macbuf[BUFLENGTH];
	uint32_t boardtype = 0;
	uint16_t serialnum = 0;
	uint16_t revnum = 0;
	char *str;
	char strtempa[3];
	char strtempb[3];

	for (i = 0; i < BUFLENGTH; i++)
		macbuf[i] = 0xffff;

	str = cmd_getarg(cmd, 0);
	if (!str || (strlen(str) != 17) ||
	    (str[2] != '-') || (str[5] != '-') ||
	    (str[8] != '-') || (str[11] != '-') || (str[14] != '-')) {
		xprintf("Invalid MAC address format\n");
		return -1;
	}

	/* [xm] Is is for something else and will be corrected later */
	macbuf[0] = __swap16(((boardtype >> 16) & 0x0FFF));
	macbuf[1] = __swap16((boardtype & 0xFFFF));
	/* Inject the number of bytes, starting at the next word. */
	macbuf[2] = 0;
	macbuf[3] = __swap16(NUMADDRS * 2 * 4 * 2 + 4);	/* Same as non-os */
	macbuf[4] = __swap16(serialnum);
	macbuf[5] = __swap16(revnum);

	for (i = 0; i < NUMADDRS; i++) {
		mac_idx = 6 + i * 4;
		strncpy(strtempa, &str[0], 2);
		strtempa[2] = '\0';
		strncpy(strtempb, &str[3], 2);
		strtempb[2] = '\0';
		macbuf[mac_idx] =
		    __swap16(((xtoi(strtempa) << 8) & 0xFF00) |
			   (xtoi(strtempb) & 0xFF));
		strncpy(strtempa, &str[6], 2);
		strtempa[2] = '\0';
		strncpy(strtempb, &str[9], 2);
		strtempb[2] = '\0';
		macbuf[mac_idx + 1] =
		    __swap16(((xtoi(strtempa) << 8) & 0xFF00) |
			   (xtoi(strtempb) & 0xFF));
		strncpy(strtempa, &str[12], 2);
		strtempa[2] = '\0';
		strncpy(strtempb, &str[15], 2);
		strtempb[2] = '\0';
		macbuf[mac_idx + 2] =
		    __swap16(((xtoi(strtempa) << 8) & 0xFF00) |
			   (xtoi(strtempb) & 0xFF));
		macbuf[mac_idx + 3] = __swap16(0);
	}

	return write_macbuf(macbuf, BUFLENGTH);
}

static int ui_cmd_macprog(ui_cmdline_t *cmd, int argc, char **argv)
{
	int i;
	int mac_idx = 0;
	unsigned short temp;
	uint16_t macbuf[BUFLENGTH];
	uint32_t boardtype = 0, btype = 0;
	uint16_t ser, serialnum;
	uint16_t revnum = 0;
	char *str;

	for (i = 0; i < BUFLENGTH; i++)
		macbuf[i] = 0xffff;

	str = cmd_getarg(cmd, 0);
	if (!str) {
		xprintf("Type \"help macprog\" to learn about its arguments.\n");
		xprintf("Typical usage: macprog <board-type> <serial-num> <board-rev>\n");
		return -1;
	}

	if (strlen(str) == 17 && str[2] == '-' && str[5] == '-' && str[8] == '-'
	    && str[11] == '-' && str[14] == '-')
		return ui_cmd_macprog_nibble(cmd, argc, argv);

	boardtype = xtoi(str);
	boardtype = boardtype << 8;
	macbuf[0] = __swap16(((boardtype >> 16) & 0x0FFF));
	macbuf[1] = __swap16((boardtype & 0xFFFF));
	boardtype = boardtype >> 8;
	btype = boardtype % 0xff;
	/* Inject the number of bytes, starting at the next word. */
	macbuf[2] = 0;
	macbuf[3] = __swap16(NUMADDRS * 2 * 4 * 2 + 4);	/* Same as non-os */

	str = cmd_getarg(cmd, 1);
	if (!str) {
		xprintf("No serial number specified; type \"help macprog\".\n");
		return -1;
	}

	serialnum = atoi(str);
	macbuf[4] = __swap16(serialnum);

	str = cmd_getarg(cmd, 2);
	if (!str) {
		xprintf("No revision number specified; type \"help macprog\".\n");
		return -1;
	}

	revnum = atoi(str);
	macbuf[5] = __swap16(revnum);
	ser = 0x1800 | (serialnum >> 8);

	for (i = 0; i < NUMADDRS; i++) {
		mac_idx = 6 + i * 4;
		temp = (btype << 12) |
			((i & ADDR_NBR_MASK) << 8) | (serialnum & 0xFF);

		macbuf[mac_idx + 0] = cpu_to_be16(0x0010);
		macbuf[mac_idx + 1] = cpu_to_be16(ser);
		macbuf[mac_idx + 2] = cpu_to_be16(temp);
		macbuf[mac_idx + 3] = cpu_to_be16(0);
	}

	return write_macbuf(macbuf, BUFLENGTH);
}

int ui_init_maccmds(void)
{
	cmd_addcmd("macprog",
		   ui_cmd_macprog,
		   NULL,
		   "Program the MAC address.",
		   "macprog XX-XX-XX-XX-XX-XX\n"
		   "macprog <board-type> <serial-num> <board-rev>\n\n"
		   "This command has two modes: the first mode sets the specific value of\n"
		   "the MAC to the address you specify.  The second mode sets the MAC based\n"
		   "to a pseudo random setting based on the board-type, serial number, and\n"
		   "board-rev.  Here are two example usages:\n\n"
		   "    macprog 00-60-6E-70-0D-A2\n"
		   "    macprog 97110 1549734 3\n",
		   "");
	return 0;
}

int bolt_set_macdevice(char *name)
{
	int hdl;

	macaddr_devname = strdup(name);
	hdl = bolt_open(macaddr_devname);
	if (hdl < 0) {
		KFREE(macaddr_devname);
		macaddr_devname = NULL;
		return BOLT_ERR_DEVNOTFOUND;
	}

	bolt_close(hdl);

	return 0;
}
