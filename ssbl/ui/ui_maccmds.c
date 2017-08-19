/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bolt.h>
#include <env_subr.h>
#include <error.h>
#include <lib_ctype.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <macaddr.h>
#include <ui_command.h>
#include <ui_init.h>

#include <stdbool.h>

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

static int ui_cmd_macprog(ui_cmdline_t *cmd, int argc, char **argv)
{
	int retval;
	uint8_t macaddr[MACADDR_SIZE];
	char macstr[20]; /* strlen("xx-xx-xx-xx-xx-xx") + margin */
	char *str;

	str = cmd_getarg(cmd, 0);
	if (str == NULL)
		return ui_showusage(cmd);

	/* acceptable formats: xx-xx-xx-xx-xx-xx or xx:xx:xx:xx:xx:xx */
	if (is_macstr_valid(str)) {
		uint8_t *p = macaddr;

		*p++ = xtoi(str);
		str += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(str);
		str += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(str);
		str += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(str);
		str += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(str);
		str += 3; /* two hex digits, and then one non-hex */
		*p++ = xtoi(str);
	} else {
		/* DEPRECATED: macprog <board-type> <serial> <rev> */
		char *p, *serial;

		warn_msg("DEPRECATED format of macprog command");

		str = cmd_getarg(cmd, 1);
		if (str == NULL) {
			xprintf("No serial number specified\n");
			return BOLT_ERR_INV_COMMAND;
		}

		/* normalize serial number via convering to upper case */
		serial = strdup(str);
		if (serial == NULL)
			return BOLT_ERR_NOMEM;

		p = serial;
		while (*p != '\0') {
			*p = lib_toupper(*p);
			++p;
		}

		retval = macaddr_generate(serial, macaddr, sizeof(macaddr));
		KFREE(serial);
		if (retval != BOLT_OK)
			return ui_showerror(retval,
				"Could not generate MAC address");

		xprintf(
/* intentionally breaking indentation */
"The command parameters would have generated:\n"
"    %02X:%02X:%02X:%02X:%02X:%02X\n"
"Please use 'setsn' command when initializing a brand new board.\n"
"If you want to try a specific MAC address, please feed the MAC\n"
"address to macprog directly.\n",
			macaddr[0], macaddr[1], macaddr[2], macaddr[3],
			macaddr[4], macaddr[5]);

		return BOLT_ERR_INV_COMMAND; /* refuse flash programming */
	}

	retval = macaddr_flash_set(macaddr, sizeof(macaddr));
	if (retval != BOLT_OK)
		return ui_showerror(retval, "Could not store MAC address");

	str = env_getenv(ENVSTR_MACADDR);
	if (str != NULL) {
		retval = env_delenv(ENVSTR_MACADDR);
		if (retval != BOLT_OK) {
			warn_msg("reboot is required to get the effect "
				"of the new MAC address");
		}
	}

	macaddr_flash_get(BCM7038MAC, macstr, 0);
	env_setenv(MACADDR_ENVSTR, macstr, ENV_FLG_BUILTIN);

	return BOLT_OK;
}

int ui_init_maccmds(void)
{
	cmd_addcmd("macprog",
		   ui_cmd_macprog,
		   NULL,
		   "Program the MAC address.",
		   "macprog XX-XX-XX-XX-XX-XX\n\n"
		   "Please use the 'setsn' command unless a specific\n"
		   "MAC address value is required.\n",
		   "");
	return 0;
}
