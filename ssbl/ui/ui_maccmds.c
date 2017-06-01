/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bolt.h>
#include <byteorder.h>
#include <env_subr.h>
#include <error.h>
#include <devfuncs.h>
#include <lib_ctype.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_types.h>
#include <macaddr.h>
#include <sha2.h>
#include <ui_command.h>
#include <ui_init.h>

#include <stdbool.h>

/* MAC Address Block Large (MA_L) assigned to Broadcom Limited per:
 * http://standards-oui.ieee.org/oui/oui.txt
 */
static const unsigned char MA_L[3] = { 0x00, 0x10, 0x18 };

static char *macaddr_devname;

static bool is_macstr_valid(char *str)
{
	int i;
	char *p;
	char delimiter;

	/* acceptable formats: xx-xx-xx-xx-xx-xx or xx:xx:xx:xx:xx:xx */
	if (str == NULL || strlen(str) != 17)
		return false;

	p = str;
	/* Xx:xx:xx:xx:xx:xx
	 * ^
	 */
	if (!isxdigit(*p))
		return false;
	++p;
	/* xX:xx:xx:xx:xx:xx
	 *  ^
	 */
	if (!isxdigit(*p))
		return false;
	++p;
	/* xx:xx:xx:xx:xx:xx
	 *   ^
	 */
	delimiter = *p;
	if (delimiter != '-' && delimiter != ':')
		return false;

	/* 5 more groups to go as the first one has been checked */
	for (i = 0; i < 5; i++) {
		if (delimiter != *p)
			return false;
		delimiter = *p;
		++p;
		if (!isxdigit(*p))
			return false;
		++p;
		if (!isxdigit(*p))
			return false;
		++p;
	}

	return true;
}

static int write_macbuf(unsigned char *macbuf, const int bufsize)
{
	int i;
	int hdl;
	int retval;
	uint8_t checksum = 0;
	char macstr[18];
	unsigned char *p;

	/* calculate checksum. */
	p = macbuf;
	checksum = 0;
	for (i = 0; i < (bufsize - 2); i++, p++)
		checksum += *p;
	*p++ = checksum;
	*p = checksum; /* repeat */

	hdl = bolt_open(macaddr_devname);
	if (hdl < 0)
		return BOLT_ERR_DEVNOTFOUND;

	xprintf("Programming flash...");

	retval = bolt_writeblk(hdl, (bolt_offset_t) 0x0,
			(unsigned char *)macbuf, bufsize);
	bolt_close(hdl);

	xprintf("done\n");

	if (retval == bufsize) {
		macaddr_flash_get(BCM7038MAC, macstr, 0);
		env_setenv(MACADDR_ENVSTR, macstr, ENV_FLG_BUILTIN);
		retval = 0;
	} else {
		xprintf("Failure while writing to flash.\n");
		xprintf("(%d != %d)\n", retval, bufsize);
		retval = -1;
	}

	return retval;
}

static int ui_cmd_macprog_nibble(ui_cmdline_t *cmd, int argc, char **argv)
{
	int i;
	unsigned char *p, buf[MACADDR_FLASHBUFSIZE];
	struct macaddr_header *header;
	char *str;

	/* no sanity check as it was done by the caller in this file */
	str = cmd_getarg(cmd, 0);

	memset(buf, 0xff, sizeof(buf));
	header = (struct macaddr_header *) buf;
	memset(header, 0, sizeof(*header));
	header->size = cpu_to_be16(MACADDR_SIZEUNTILCHKSUM);

	p = buf + sizeof(struct macaddr_header);
	for (i = 0; i < MACADDR_NUMADDRS; i++) {
		char *q = str;

		*p++ = xtoi(q);
		q += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(q);
		q += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(q);
		q += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(q);
		q += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(q);
		q += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(q);
		p += 2; /* skip two LSB's */
	}

	return write_macbuf(buf, MACADDR_FLASHBUFSIZE);
}

static int ui_cmd_macprog(ui_cmdline_t *cmd, int argc, char **argv)
{
	int i;
	unsigned char *p, buf[MACADDR_FLASHBUFSIZE];
	struct macaddr_header *header;
	uint32_t boardtype = 0;
	uint16_t serialnum;
	uint16_t revnum = 0;
	char *str;
	sha256_ctx ctx;
	unsigned char digest[SHA256_DIGEST_SIZE];

	memset(buf, 0xff, sizeof(buf));
	header = (struct macaddr_header *) buf;

	str = cmd_getarg(cmd, 0);
	if (!str) {
		xprintf("Type \"help macprog\" to learn about its arguments.\n");
		xprintf("Typical usage: macprog <board-type> <serial-num> <board-rev>\n");
		return -1;
	}

	/* acceptable formats: xx-xx-xx-xx-xx-xx or xx:xx:xx:xx:xx:xx */
	if (is_macstr_valid(str))
		return ui_cmd_macprog_nibble(cmd, argc, argv);

	sha256_init(&ctx);
	sha256_update(&ctx, (unsigned char *)str, strlen(str));
	boardtype = xtoi(str);
	header->board_type = cpu_to_be32(boardtype << 8);
	header->zero = 0;
	header->size = cpu_to_be16(MACADDR_SIZEUNTILCHKSUM);

	str = cmd_getarg(cmd, 1);
	if (!str) {
		xprintf("No serial number specified; type \"help macprog\".\n");
		return -1;
	}
	sha256_update(&ctx, (unsigned char *)str, strlen(str));
	sha256_final(&ctx, digest);

	serialnum = atoi(str);
	header->board_serial = cpu_to_be16(serialnum);

	str = cmd_getarg(cmd, 2);
	if (!str) {
		xprintf("No revision number specified; type \"help macprog\".\n");
		return -1;
	}

	revnum = atoi(str);
	header->board_revision = cpu_to_be16(revnum);

	p = buf + sizeof(struct macaddr_header);
	for (i = 0; i < MACADDR_NUMADDRS; i++) {
		memcpy(p, digest, MACADDR_STRIDE);
		memcpy(p, MA_L, sizeof(MA_L));
		p += MACADDR_STRIDE;
	}

	return write_macbuf(buf, MACADDR_FLASHBUFSIZE);
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
