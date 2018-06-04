/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
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
#include "mon64.h"

bolt_loadargs_t mon64_loadargs;

static const char *mon64_load_help_summary =
	"Load EL3 monitor into memory";

static const char *mon64_load_help_usage =
	"mon64 load host:filename|dev:filename\n\n"
	"This command loads the 64 bit mon64 from boot device into memory.\n\n"
	"The 64 bit mon64 that is built into bolt.bin has already been loaded\n"
	"at this time. This command can be used to overwrite it.\n\n"
	"By default, 'mon64 load' loads the 64 bit mon64 as an uncompressed\n"
	"image into the memory region reserved under the name 'MON64'.";

static int mon64_cmd_load(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;
	char *arg;
	char progname[200];
	char *program;
	char *device;
	int info;
	const char *loader;
	bolt_loadargs_t *la;
	long install_address;

	/* More than 3 arguments means there were extra/unquoted boot args */
	if (argc > 1) {
		xprintf("Too many arguments\n");
		return BOLT_ERR_INV_PARAM;
	}

	arg = cmd_getarg(cmd, 0);
	if (!arg) {
		xprintf("No mon64 binary name specified\n");
		return BOLT_ERR;
	}

	strncpy(progname, arg, sizeof(progname));
	splitpath(progname, &device, &program);

	if (!device) {
		xprintf("No device name specified for mon64 binary\n");
		return BOLT_ERR;
	}

	info = bolt_getdevinfo(device);
	if (info >= 0)
		info &= BOLT_DEV_MASK;

	loader = "raw";

	/*
	 * Fill in the loader args
	 */
	la = &mon64_loadargs;
	memset(la, 0, sizeof(*la));

	la->la_flags   = LOADFLG_SPECADDR;
	la->la_flags  |= LOADFLG_EL3_EXEC;
	la->la_flags  |= LOADFLG_APP64;

	la->la_address = M64BIN_RAM_ADDR;
	la->la_maxsize = M64BIN_SIZE;

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
	 * Load image and launch the program.
	 * Note: we might not come back here if we really launch the program.
	 */
	xprintf("Loader:%s Filesys:%s Dev:%s File:%s Options:%s\n",
		loader, la->la_filesys, la->la_device, la->la_filename,
		la->la_options);

	la->la_flags |= LOADFLG_NOISY;
	rc = bolt_boot(loader, la);

	/* rc holds negative error code or load size */
	if (rc < 0)
		goto out;

	/* Install mon64 */
	mon64_install(la->la_address, rc, &install_address);
	xprintf("Install address is %#lx\n", install_address);
	return BOLT_OK;

out:
	/*
	 * Give the bad news.
	 */
	err_msg("Could not load %s: %s", program, bolt_errortext(rc));
	return rc;
}

int ui_init_mon64(void)
{
	cmd_addcmd("mon64 load", mon64_cmd_load, NULL,
		(char *)mon64_load_help_summary,
		(char *)mon64_load_help_usage,
		"");

	return 0;
}

/* EOF */
