/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdbool.h>

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "devfuncs.h"

#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"

#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"

#include "fileops.h"
#include "common.h"
#include "error.h"
#include "config.h"
#include "loader.h"
#include "devtree.h"

#include "tz.h"
#include "tz_priv.h"


static bolt_loadargs_t bolt_tzloadargs;
static int tz_initialized;

#ifdef STUB64_START
static int tz_monitor_loaded;
static bolt_loadargs_t bolt_tz_mon_loadargs;
static const char *tz_mon_help_summary =
	"Load EL3 monitor into memory";

static const char *tz_mon_help_usage =
	"tz mon host:filename|dev:filename\n\n"
	"This command loads the EL3 monitor into memory.\n"
	"By default, 'tz mon' will load the\n"
	"program as a uncompressed image at the\n"
	"start of the TZ memory + an offset.\n"
	"0x01b00000 for 2GB systems\n"
	"0x00b00000 for 1GB system\n";
#endif

static const char *tz_boot_help_summary =
	"Load NW and TZ executable files into memory, executes TZ program";

#ifdef STUB64_START
static const char *tz_boot_help_usage =
	"tz boot host:tz_filename|dev:tz_filename\n"
	"        host:nw_filename|dev:nw_filename\n"
	"        [arg]\n\n"
	"This command loads NW and TZ executable files from boot devices\n"
	"and executes both via the EL3 monitor. By default, 'tz boot'\n"
	"will load the NW\nprogram as a zImage binary at address "
	__stringify(BOOT_START_ADDRESS)", and\n"
	"the TZ program as an uncompressed image at the start of TZ memory.\n"
	"'tz mon' must be executed successfully before using this command.";
#else
static const char *tz_boot_help_usage =
	"tz boot host:tz_filename|dev:tz_filename\n"
	"        host:nw_filename|dev:nw_filename\n"
	"        [arg]\n\n"
	"This command loads NW and TZ executable files from boot devices,\n"
	"and execute the TZ program. By default, 'tz boot' will load the NW\n"
	"program as a zImage binary at address "
	__stringify(BOOT_START_ADDRESS)", and the TZ program\n"
	"as an uncompressed image at the start of the TZ memory, and then\n"
	"jump to the start of the TZ program.";
#endif

static const char *tz_get_prog_type(tz_payload_type tz_prog)
{
	switch (tz_prog) {
	case e_tz_spd_image:
		return "EL3 monitor";
	case e_tz_tz_image:
		return "TZ";
	case e_tz_nw_image:
		return "NW";
	}
	return "?";
}

static int tz_cmd_boot_common(ui_cmdline_t *cmd, tz_payload_type tz_prog)
{
	int rc;
	char *arg;
	char progname[200];
	char *program;
	char *device;
	int info;
	const char *loader;
	bolt_loadargs_t *la = &bolt_tzloadargs;
	int node;
	struct tz_info *t;
	struct tz_mem_layout *mem_layout;

	switch (tz_prog) {
	case e_tz_spd_image:
		arg = cmd_getarg(cmd, 0);
		break;
	case e_tz_tz_image:
		arg = cmd_getarg(cmd, 0);
		break;
	case e_tz_nw_image:
		arg = cmd_getarg(cmd, 1);
		break;
	default:
		arg = NULL;
	}
	if (!arg) {
		xprintf("No %s program name specified\n",
			tz_get_prog_type(tz_prog));
		return BOLT_ERR;
	}

	strncpy(progname, arg, sizeof(progname));
	splitpath(progname, &device, &program);

	if (!device) {
		xprintf("No device name specified for %s program.\n",
			tz_get_prog_type(tz_prog));
		return BOLT_ERR;
	}

	info = bolt_getdevinfo(device);
	if (info >= 0)
		info &= BOLT_DEV_MASK;

	if (tz_prog == e_tz_nw_image)
		loader = "zimg";
	else
		loader = "raw";

	/*
	 * Fill in the loader args
	 */

	memset(la, 0, sizeof(*la));

	switch (tz_prog) {
	case e_tz_spd_image:
#ifdef STUB64_START
		t = tz_info();
		if (!t)
			return BOLT_ERR;
		mem_layout = t->mem_layout;
		if (!mem_layout)
			return BOLT_ERR;

		/* Need to save the mon load args for later */
		la = &bolt_tz_mon_loadargs;
		la->la_flags |= LOADFLG_EL3_EXEC|LOADFLG_APP64;
		la->la_address = t->mem_addr + mem_layout->spd_offset;
		la->la_maxsize = BOOT_AREA_SIZE;
		tz_monitor_loaded = 1;
#endif
		break;
	case e_tz_tz_image:
		t = tz_info();
		if (!t)
			return BOLT_ERR;

		mem_layout = t->mem_layout;
		if (!mem_layout)
			return BOLT_ERR;

		la->la_flags = LOADFLG_SPECADDR|LOADFLG_SECURE;
#ifdef STUB64_START
		la->la_flags |= LOADFLG_APP64;
#else
		la->la_flags |= LOADFLG_EXECUTE;
#endif
		la->la_address = t->mem_addr + mem_layout->os_offset;
		la->la_maxsize = BOOT_AREA_SIZE;
		break;
	case e_tz_nw_image:
		la->la_flags = LOADFLG_SPECADDR;
		la->la_address = BOOT_START_ADDRESS;
		la->la_maxsize = BOOT_AREA_SIZE;
		break;
	}

	switch (info) {
	case BOLT_DEV_DISK:
		la->la_filesys = "fat";
		break;
	case BOLT_DEV_FLASH:
	case BOLT_DEV_SERIAL:
		la->la_filesys = "raw";
		break;
	case BOLT_DEV_NETWORK:
	default:
		la->la_filesys = "tftp";
	}

	if (info >= 0) {
		la->la_device = device;
		la->la_filename = program;
	} else {
#if CFG_NETWORK
		la->la_device = (char *)net_getparam(NET_DEVNAME);
#else
		la->la_device = NULL;
#endif
		la->la_filename = arg;
	}

#if CFG_NETWORK
	/*
	 * Configure the network if necessary
	 */

	if (!strcmp(la->la_filesys, "tftp") && !la->la_device
		&& bolt_finddev(DEF_NETDEV))
		if (net_init(DEF_NETDEV) < 0 || do_dhcp_request(DEF_NETDEV) < 0)
			return BOLT_ERR;
#endif

	/*
	 * Process additional boot arguments if necessary
	 */

	la->la_options = cmd_getarg(cmd, 2);

	if (la->la_options) {
		if (tz_prog == e_tz_nw_image) {
			bolt_devtree_params_t p;
			void *fdt;

			bolt_devtree_getenvs(&p);
			fdt = p.dt_address;

			if (fdt && !bolt_devtree_sane(fdt, NULL)) {
				node = bolt_devtree_node_from_path(fdt,
					"/chosen");
				if (node < 0) {
					rc = bolt_devtree_addnode_at(fdt,
						"chosen", 0, &node);
					if (rc != BOLT_OK)
						goto out;
				}

				rc = bolt_dt_addprop_str(fdt, node, "bootargs",
					la->la_options);
				if (rc) {
					rc = BOLT_ERR;
					goto out;
				}
			}
		}
	}

	/*
	 * Note: we might not come back here if we really launch the program.
	 */

	xprintf("Loader:%s Filesys:%s Dev:%s File:%s Options:%s\n",
		loader, la->la_filesys, la->la_device, la->la_filename,
		la->la_options);

	rc = tz_boot(loader, la);

#ifdef STUB64_START
	/*
	 * In 64 bit mode, and we have completed loading all three
	 * images, boot the monitor.
	 */
	if (rc >= 0 && tz_prog == e_tz_tz_image) {
		la = &bolt_tz_mon_loadargs;
		la->la_flags |= LOADFLG_EXECUTE;
		rc = tz_go(la);
	}
#endif
	/* Success when !LOADFLG_EXECUTE, so exit now. */
	if (rc >= 0)
		return rc;

out:
	/*
	 * Give the bad news.
	 */
	err_msg("Could not load %s: %s", program, bolt_errortext(rc));

	/* Delete bootargs from DT on failure */
	if (la->la_options) {
		if (tz_prog == e_tz_nw_image) {
			bolt_devtree_params_t p;
			void *fdt;

			bolt_devtree_getenvs(&p);
			fdt = p.dt_address;

			if (fdt && !bolt_devtree_sane(fdt, NULL))
				bolt_devtree_delprop_path(fdt,
					"chosen", "bootargs");
		}
	}

	return rc;
}

#ifdef STUB64_START
static int tz_cmd_mon(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int tc;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

	/* Load the monitor program */
	tc = tz_cmd_boot_common(cmd, e_tz_spd_image);

	return tc;
}
#endif

static int tz_cmd_boot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int tc;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

#ifdef STUB64_START
	/* Check that we have a monitor to boot the images */
	if (!tz_monitor_loaded) {
		err_msg("EL3 monitor required to boot TZ in 64 bit mode");
		return BOLT_ERR_INV_COMMAND;
	}
#endif

	/* More than 3 arguments means there were extra/unquoted boot args */
	if (argc > 3) {
		xprintf("Too many arguments, or an unquoted optional [arg]\n");
		return BOLT_ERR_INV_PARAM;
	}

	/* Finalize TZ device tree */
	tz_devtree_set();

	/* Finalize NW device tree */
	tz_devtree_set_nwos();

	/* Load NW program */
	tc = tz_cmd_boot_common(cmd, e_tz_nw_image);
	if (tc < 0)
		return tc;

	/* Boot TZ program */
	tc = tz_cmd_boot_common(cmd, e_tz_tz_image);

	return tc;
}


static int tz_cmd_dt_show(ui_cmdline_t *cmd, int argc, char *argv[])
{
	void *dtb;
	struct tz_info *t;
	struct dtb_to_dts_params p;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	p.offset = 0;
	p.depth = 0;

	dtb = t->dt_addr;
	if (!dtb)
		return BOLT_ERR_BADADDR;

	if (argc >= 1)
		p.match = argv[0];
	else
		p.match = NULL;

	p.justnodes = cmd_sw_isset(cmd, "-nodes");
	p.fullpaths = cmd_sw_isset(cmd, "-paths");

	/* 1. 'justnodes' is not meant to do the full '{}' brace
	 * DT node/property, its just to give an overview of the
	 * hierarchy.
	 *
	 * 2. 'fullpaths' requires a match param as its not the
	 * full DT but a subset. 'dt show' with no params will
	 * print the whole thing anyway.
	 */
	if (p.fullpaths && (p.justnodes || !p.match))
		return BOLT_ERR_INV_PARAM;

	return bolt_devtree_dtb_to_dts(dtb, &p);
}


static int tz_cmd_init(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;

	if (tz_initialized) {
		warn_msg("TZ already initialized");
		return BOLT_ERR;
	}

	rc = tz_init();
	if (rc)
		err_msg("TZ init failed");
	else
		tz_initialized = 1;

#ifdef STUB64_START
	tz_monitor_loaded = 0;
#endif
	return rc;
}


static int tz_cmd_console_on(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int uart = -1;
	struct tz_info *t;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	if (argc >= 1) {
		if (!strcmp(argv[0], "uart1"))
			uart = 1;
		else if (!strcmp(argv[0], "uart2"))
			uart = 2;
	}
	return tz_config_uart(uart);
}


static int tz_cmd_console_off(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct tz_info *t;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	t->uart_base = 0;

	return 0;
}


int ui_init_tz(void)
{
	cmd_addcmd("tz init", tz_cmd_init, NULL,
		"Initialize TrustZone loader",
		"tz init",
		"");

#ifdef STUB64_START
	cmd_addcmd("tz mon", tz_cmd_mon, NULL,
	   (char *)tz_mon_help_summary,
	   (char *)tz_mon_help_usage,
		"");
#endif

	cmd_addcmd("tz boot", tz_cmd_boot, NULL,
		(char *)tz_boot_help_summary,
		(char *)tz_boot_help_usage,
	    "");

	cmd_addcmd("tz dt show", tz_cmd_dt_show, NULL,
		"Decode contents of the memory resident TZ DTB file as a DTS\n",
		"tz dt show [-nodes|-paths] [partial or full node name]",
		"-nodes;show only the nodes|"
		"-paths;show full paths if node name is specified");

	cmd_addcmd("tz console on", tz_cmd_console_on, NULL,
		"Enable a second UART for TZ console\n",
		"tz console on [uart1|uart2]",
		"uart1;use uart1|"
		"uart2;use uart2");

	cmd_addcmd("tz console off", tz_cmd_console_off, NULL,
		"Disable TZ console (default)\n",
		"tz console off",
		"");

	return 0;
}

/* EOF */
