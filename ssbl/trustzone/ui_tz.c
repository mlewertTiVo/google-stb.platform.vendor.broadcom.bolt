/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
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

#include "arch_ops.h"
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

typedef enum {
	e_mon_image,
	e_tz_image,
	e_nw_image
} tz_prog_type;

static const char *tz_prog_names[] = {
	"EL3 monitor",
	"TZ",
	"NW"
};

static bool tz_initialized;
bolt_loadargs_t tz_loadargs;

#ifdef STUB64_START

#if CFG_TRUSTZONE_MON
/* Default EL3 monitor load arguments */
static bolt_loadargs_t tz_mon_loadargs;

static const char *tz_mon_help_summary =
	"Load EL3 monitor into memory";

static const char *tz_mon_help_usage =
	"tz mon host:filename|dev:filename\n\n"
	"This command loads the EL3 monitor from boot device into memory.\n\n"
	"The EL3 monitor that is built into bolt.bin has already been loaded\n"
	"at this time. This command can be used to overwrite it.\n\n"
	"By default, 'tz mon' loads the EL3 monitor as an uncompressed image\n"
	"at the start of the memory region reserved under the name 'MON'.";
#endif

static const char *tz_boot_help_summary =
	"Load NW and TZ executable files into memory, executes via EL3 monitor";

static const char *tz_boot_help_usage =
	"tz boot host:tz_filename|dev:tz_filename\n"
	"        host:nw_filename|dev:nw_filename\n"
	"        [arg]\n\n"
	"This command loads NW and TZ executable files from boot devices\n"
	"and executes both via the EL3 monitor.\n\n"
	"By default, 'tz boot' loads the NW program as a zImage binary at\n"
	"address "
	__stringify(BOOT_START_ADDRESS)
	", and the TZ program as an uncompressed image at\n"
	"fixed offset in the memory region reserved under the name 'TZOS'.";

#else /* STUB64_START */

static const char *tz_boot_help_summary =
	"Load NW and TZ executable files into memory, execute TZ program";

static const char *tz_boot_help_usage =
	"tz boot host:tz_filename|dev:tz_filename\n"
	"        host:nw_filename|dev:nw_filename\n"
	"        [arg]\n\n"
	"This command loads NW and TZ executable files from boot devices,\n"
	"and execute the TZ program.\n\n"
	"By default, 'tz boot' loads the NW program as a zImage binary at\n"
	"address "
	__stringify(BOOT_START_ADDRESS)
	", and the TZ program as an uncompressed image at\n"
	"fixed offset in the memory region reserved under the name 'TZOS'.";

#endif /* STUB64_START */

static int tz_cmd_boot_common(ui_cmdline_t *cmd, tz_prog_type tz_prog)
{
	int rc;
	char *arg;
	char progname[200];
	char *program;
	char *device;
	int info;
	const char *loader;
	bolt_loadargs_t *la;
	int node;
	struct tz_info *t;
	struct tz_mem_layout *mem_layout;

	switch (tz_prog) {
	case e_mon_image:
	case e_tz_image:
	default:
		arg = cmd_getarg(cmd, 0);
		break;
	case e_nw_image:
		arg = cmd_getarg(cmd, 1);
		break;
	}
	if (!arg) {
		xprintf("No %s program name specified\n",
			tz_prog_names[tz_prog]);
		return BOLT_ERR;
	}

	strncpy(progname, arg, sizeof(progname));
	splitpath(progname, &device, &program);

	if (!device) {
		xprintf("No device name specified for %s program\n",
			tz_prog_names[tz_prog]);
		return BOLT_ERR;
	}

	info = bolt_getdevinfo(device);
	if (info >= 0)
		info &= BOLT_DEV_MASK;

	switch (tz_prog) {
	case e_mon_image:
	case e_tz_image:
	default:
		loader = "raw";
		break;
	case e_nw_image:
		loader = "zimg";
		break;
	}

	/*
	 * Fill in the loader args
	 */
	switch (tz_prog) {
	case e_mon_image:
#if CFG_TRUSTZONE_MON
		/* Need to save the mon load args for later */
		la = &tz_mon_loadargs;
#endif
		break;
	case e_tz_image:
	case e_nw_image:
	default:
		la = &tz_loadargs;
		break;
	}

	memset(la, 0, sizeof(*la));

	switch (tz_prog) {
	case e_mon_image:
#if CFG_TRUSTZONE_MON
		t = tz_info();
		if (!t)
			return BOLT_ERR;

		la->la_flags   = LOADFLG_SPECADDR;
		la->la_flags  |= LOADFLG_EL3_EXEC;
		la->la_flags  |= LOADFLG_APP64;
		la->la_address = t->mon_addr;
		la->la_maxsize = t->mon_size;
#endif
		break;
	case e_tz_image:
	default:
		t = tz_info();
		if (!t)
			return BOLT_ERR;

		mem_layout = t->mem_layout;
		if (!mem_layout)
			return BOLT_ERR;

		la->la_flags   = LOADFLG_SPECADDR;
#if !CFG_TRUSTZONE_MON
		la->la_flags  |= LOADFLG_EXECUTE;
#endif
#ifdef STUB64_START
		la->la_flags  |= LOADFLG_SECURE;
		la->la_flags  |= LOADFLG_APP64;
#endif
		la->la_address = t->mem_addr + mem_layout->load_offset;
		la->la_maxsize = mem_layout->load_size;
		break;
	case e_nw_image:
		la->la_flags   = LOADFLG_SPECADDR;
		la->la_flags  |= LOADFLG_NOCLOSE;
#ifdef STUB64_START
		la->la_flags  |= LOADFLG_APP64;
#endif
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
	switch (tz_prog) {
	case e_mon_image:
	case e_tz_image:
	default:
		/* Nothing to do yet */
		break;
	case e_nw_image:
		la->la_options = cmd_getarg(cmd, 2);

		if (la->la_options) {
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
				if (rc != BOLT_OK)
					goto out;
			}
		}
	}

	/*
	 * Load image and launch the program.
	 * Note: we might not come back here if we really launch the program.
	 */
	xprintf("Loader:%s Filesys:%s Dev:%s File:%s Options:%s\n",
		loader, la->la_filesys, la->la_device, la->la_filename,
		la->la_options);

	la->la_flags |= LOADFLG_NOISY;
	rc = tz_boot(loader, la);

	/* rc holds negative error code or load size */
	if (rc < 0)
		goto out;

#if CFG_TRUSTZONE_MON
	/*
	 * In 64 bit mode, handle real boot here.
	 */
	switch (tz_prog) {
	case e_mon_image:
	case e_nw_image:
		return rc;
	case e_tz_image:
		/* All three images loaded, boot the monitor */
		la = &tz_mon_loadargs;
		la->la_flags |= LOADFLG_EXECUTE;
		rc = tz_go(la);
		/* We only come back here in error, fall thru */
	}
#else
	return BOLT_OK;
#endif

out:
	/*
	 * Give the bad news.
	 */
	err_msg("Could not load %s: %s", program, bolt_errortext(rc));

	/* Delete bootargs from DT on failure */
	switch (tz_prog) {
	case e_mon_image:
	case e_tz_image:
		/* Nothing to do yet */
		break;
	case e_nw_image:
		if (la->la_options) {
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

#if CFG_TRUSTZONE_MON
static int tz_cmd_mon(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int tc;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

	/* Load the monitor program */
	tc = tz_cmd_boot_common(cmd, e_mon_image);

	return tc;
}
#endif

static int tz_cmd_boot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;

	if (!tz_initialized) {
		err_msg("TZ not initialized");
		return BOLT_ERR_INV_COMMAND;
	}

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
	rc = tz_cmd_boot_common(cmd, e_nw_image);
	if (rc < 0)
		return rc;

	/* Boot TZ program */
	rc = tz_cmd_boot_common(cmd, e_tz_image);

	return rc;
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

	dtb = t->dt_address;
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

#if CFG_MON64
	if (!arch_booted64()) {
		warn_msg("TZ unspported in 32 bit bootmode");
		return BOLT_ERR;
	}
#endif

	if (tz_initialized) {
		warn_msg("TZ already initialized");
		return BOLT_ERR;
	}

	rc = tz_init();
	if (rc)
		err_msg("TZ init failed");
	else
		tz_initialized = true;

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

#if CFG_TRUSTZONE_MON
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
