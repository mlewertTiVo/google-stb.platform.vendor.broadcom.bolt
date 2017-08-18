/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bolt.h>
#include <console.h>
#include <device.h>
#include <env_subr.h>
#include <error.h>
#include <iocb.h>
#include <lib_ctype.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <macaddr.h>
#include <stdbool.h>
#include <ui_command.h>
#include <ui_init.h>

static int ui_cmd_printenv(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char varname[80], *vname, *vvalue;
	char value[ENV_MAX_RECORD + 1];
	int varlen, vallen;
	int idx;

	vname = cmd_getarg(cmd, 0);
	if (!vname) {
		xprintf("Variable Name        Value\n");
		xprintf("-------------------- --------------------------------------------------\n");

		idx = 0;
		for (;;) {
			varlen = sizeof(varname);
			vallen = sizeof(value);
			if (env_enum(idx, varname, &varlen, value, &vallen) < 0)
				break;
			xprintf("%20s %s\n", varname, value);
			idx++;
		}
	} else {
		vvalue = env_getenv(vname);
		if (vvalue)
			xprintf("    %s = %s\n", vname, vvalue);
	}

	return 0;

}

static int ui_cmd_setenv(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *varname;
	char *value;
	int roflag = ENV_FLG_NORMAL;
	int res;

	varname = cmd_getarg(cmd, 0);

	if (!varname)
		return ui_showusage(cmd);

	value = cmd_getarg(cmd, 1);
	if (!value)
		return ui_showusage(cmd);

	if (!cmd_sw_isset(cmd, "-p"))
		roflag = ENV_FLG_BUILTIN;	/* just in memory, not NVRAM */

	if (cmd_sw_isset(cmd, "-ro"))
		roflag = ENV_FLG_READONLY;

	res = env_setenv(varname, value, roflag);
	if (res) {
		return ui_showerror(res,
				    "Could not set environment variable '%s'",
				    varname);
	}
	if (roflag != ENV_FLG_BUILTIN)
		env_save();

	return 0;
}

/* ui_cmd_setsn -- sets a board serial number */
static int ui_cmd_setsn(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *p, *str, *serial;
	int roflag = ENV_FLG_NORMAL;
	bool force_macprog;
	uint8_t macaddr[MACADDR_SIZE];
	char macstr[20]; /* strlen("xx:xx:xx:xx:xx:xx") + margin */
	char *envmac;
	int res;

	str = cmd_getarg(cmd, 0);
	if (str == NULL)
		return ui_showusage(cmd);

	force_macprog = cmd_sw_isset(cmd, "-f");

	if (cmd_sw_isset(cmd, "-ro"))
		roflag = ENV_FLG_READONLY;

	/* normalize serial number via convering to upper case */
	serial = strdup(str);
	if (serial == NULL)
		return BOLT_ERR_NOMEM;

	p = serial;
	while (*p != '\0') {
		*p = lib_toupper(*p);
		++p;
	}

	res = env_setenv(ENVSTR_BOARD_SERIAL, serial, roflag);
	if (res != BOLT_OK) {
		KFREE(serial);
		return ui_showerror(res, "Could not set board serial number");
	}

	env_save();

	/* program a MAC address into flash if forced or has not been done */
	if (!force_macprog && macaddr_flash_verify(0) == BOLT_OK) {
		KFREE(serial);
		return BOLT_OK;
	}

	res = macaddr_generate(serial, macaddr, sizeof(macaddr));
	KFREE(serial); /* release as soon as not need anymore */
	serial = NULL;
	if (res != BOLT_OK)
		return ui_showerror(res, "Could not generate MAC address");

	res = macaddr_flash_set(macaddr, sizeof(macaddr));
	if (res != BOLT_OK)
		return ui_showerror(res, "Could not store MAC address");

	envmac = env_getenv(ENVSTR_MACADDR);
	if (envmac != NULL) {
		res = env_delenv(ENVSTR_MACADDR);
		if (res != BOLT_OK) {
			warn_msg("reboot is required to get the effect "
				"of the new MAC address");
		}
	}

	macaddr_flash_get(BCM7038MAC, macstr, 0);
	(void)env_setenv(ENVSTR_MACADDR, macstr, ENV_FLG_BUILTIN);

	return BOLT_OK;
}

static int ui_cmd_unsetenv(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *varname;
	int res;
	int type;

	varname = cmd_getarg(cmd, 0);

	if (!varname)
		return ui_showusage(cmd);

	type = env_envtype(varname);

	res = env_delenv(varname);
	if (res)
		return ui_showerror(res,
				    "Could not delete environment variable '%s'",
				    varname);
	if ((type >= 0) && (type != ENV_FLG_BUILTIN))
		env_save();

	return 0;
}

static int ui_cmd_incenv(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *varname, valstr[20];
	int res;
	int val, type = ENV_FLG_NORMAL;

	varname = cmd_getarg(cmd, 0);
	if (!varname)
		return ui_showusage(cmd);

	val = env_getval(varname);
	if (val < 0)
		val = 1;
	else {
		++val;
		type = env_envtype(varname);
	}
	sprintf(valstr, "%d", val);
	res = env_setenv(varname, valstr, type);
	if (res)
		return ui_showerror(res,
				    "Could not increment environment variable '%s'",
				    varname);
	if (type != ENV_FLG_BUILTIN)
		env_save();
	xprintf("    %s = %d\n", varname, val);

	return 0;
}

static int ui_cmd_testenv(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *varname, *refval;
	int res;
	int val, cond = 0;

	varname = cmd_getarg(cmd, 0);
	if (!varname)
		return ui_showusage(cmd);

	if (cmd_sw_isset(cmd, "-lt"))
		cond = 1;
	else if (cmd_sw_isset(cmd, "-eq"))
		cond = 2;
	else if (cmd_sw_isset(cmd, "-gt"))
		cond = 3;

	if (cmd_sw_isset(cmd, "-n"))	/* var non-existance */
		res = env_getenv(varname) != NULL;
	else if (cond != 0) {
		refval = cmd_getarg(cmd, 1);
		if (!refval)
			return ui_showusage(cmd);
		val = env_getval(varname);
		if (val == -1)
			res = 1;
		else {
			switch (cond) {
			case (1):
				res = (val < atoi(refval)) ? 0 : 1;
				break;
			case (2):
				res = (val == atoi(refval)) ? 0 : 1;
				break;
			case (3):
				res = (val > atoi(refval)) ? 0 : 1;
				break;
			}
		}
	} else
		res = env_getenv(varname) == NULL;	/* var existance */

	return res;
}

int ui_init_envcmds(void)
{

	cmd_addcmd("setenv",
		   ui_cmd_setenv,
		   NULL,
		   "Set an environment variable.",
		   "setenv [-ro] [-p] varname value\n\n"
		   "This command sets an environment variable.  By default, an environment variable\n"
		   "is stored only in memory and will not be retained across system restart.",
		   "-p;Store environment variable permanently in the NVRAM device, if present|"
		   "-ro;Causes variable to be read-only\n"
		   "(cannot be changed in the future, implies -p)");

	cmd_addcmd("printenv",
		   ui_cmd_printenv,
		   NULL,
		   "Display the environment variables",
		   "printenv [varname]\n\n"
		   "This command prints the value(s) of all or a specified environment\n"
		   "variable(s)\n",
		   "");

	cmd_addcmd("unsetenv",
		   ui_cmd_unsetenv,
		   NULL,
		   "Delete an environment variable.",
		   "unsetenv varname\n\n"
		   "This command deletes an environment variable from memory and also\n"
		   "removes it from the NVRAM device (if present).", "");

	cmd_addcmd("incenv",
		   ui_cmd_incenv,
		   NULL,
		   "Increment an integer environment variable.",
		   "incenv varname\n\n"
		   "This command increments the integer value of an environment variable.\n"
		   "If it doesn't exist, then it creates one (in NVRAM device) with a\n"
		   "value of 1.",
		   "");

	cmd_addcmd("testenv",
		   ui_cmd_testenv,
		   NULL,
		   "Tests environment variable for various conditions. The default is to\n"
		   "test the existence of the variable.\n",
		   "testenv [-n] [-lt][-eq][-gt] varname [value]\n\n"
		   "Tests environment variable for various conditions.\n",
		   "-n;check for non-existence of variable|"
		   "-eq;test if variable is equal to specified value.|"
		   "-gt;test if variable is greater than specified value.|"
		   "-lt;test if variable is less than specified value.");

	cmd_addcmd("setsn",
		   ui_cmd_setsn,
		   NULL,
		   "Sets a board serial number",
"setsn [-f] [-ro] <board-serial>\n\n"
"This command sets a given serial number to the environment variable,\n"
ENVSTR_BOARD_SERIAL ", If the board does not have a valid MAC address stored\n"
"in a designated flash partition, one is generated based on the serial\n"
"number and stored in the designated flash partition.",
		   "-f;force MAC programming|"
		   "-ro;" ENVSTR_BOARD_SERIAL " cannot change in the future."
		   " The '-f' option cannot work as well.");

	return 0;
}
