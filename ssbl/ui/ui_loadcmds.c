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
#include "boot.h"
#include "loader.h"
#include "devtree.h"
#include "splash-api.h"

#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_load(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

#if CFG_NETWORK
#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_save(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
#endif

static int ui_cmd_boot(ui_cmdline_t *cmd, int argc, char *argv[]);
#if (CFG_CMD_LEVEL >= 3)
static int ui_cmd_batch(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_go(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

static bolt_loadargs_t bolt_loadargs;

#define DEF_NETDEV "eth0"

#ifdef __long64
#define XTOI(x) xtoq(x)
#else
#define XTOI(x) xtoi(x)
#endif

static const char *boot_help_description =
"boot [-options] host:filename|dev:filename [arg]\n\n"
"This command loads and executes a program from a boot device,\n"
"By default, 'boot' will load a zImage binary at virtual\n"
"address " __stringify(BOOT_START_ADDRESS)
" and then jump to that address";

#if (CFG_CMD_LEVEL >= 1)
static const char *load_help_description =
"load [-options] host:filename|dev:filename\n\n"
"This command loads an executable file into memory, but does not\n"
"execute it.  It can be used for loading data files, overlays or\n"
"other programs needed before the 'boot' command is used.  By\n"
"default, 'load' will load a raw binary at virtual address "
__stringify(BOOT_START_ADDRESS) ".";
#endif


int ui_init_loadcmds(void)
{

	cmd_addcmd("boot", ui_cmd_boot, NULL,
		   "Load an executable file into memory and execute it",
			(char *)boot_help_description,
		   "-elf;Load the file as an ELF executable|"
		   "-srec;Load the file as ASCII S-records|"
		   "-raw;Load the file as a raw binary|"
#if CFG_ZIMG_LDR
		   "-zimg;Load the file as a zImage binary (default)|"
#endif
#if CFG_BSU
		   "-bsu;Load a sidecar app|"
#endif
#if CFG_ZLIB
		   "-z;Load gzip-compressed file (default)|"
		   "-nz;Load uncompressed file|"
#endif
		   "-loader=*;Specify BOLT loader name|"
		   "-tftp;Load the file using the TFTP protocol|"
		   "-fatfs;Load the file from a FAT file system|"
		   "-rawfs;Load the file from an unformatted file system|"
		   "-fs=*;Specify BOLT file system name|"
		   "-max=*;Specify the maximum number of bytes to load (raw and zImage)|"
		   "-addr=*;Specify the load address (hex) (raw and zImage)|"
		   "-noclose;Don't close network link before executing program");

#if (CFG_CMD_LEVEL >= 1)
#if CFG_NETWORK
	cmd_addcmd("save", ui_cmd_save, NULL,
		   "Save a region of memory to a remote file via TFTP",
		   "save host:filename startaddr length\n\n", "");
#endif

	cmd_addcmd("load", ui_cmd_load, NULL,
		   "Load an executable file into memory without executing it",
			(char *)load_help_description,
		   "-elf;Load the file as an ELF executable|"
		   "-srec;Load the file as ASCII S-records|"
		   "-raw;Load the file as a raw binary|"
#if CFG_ZIMG_LDR
		   "-zimg;Load the file as a zImage binary (default)|"
#endif
#if CFG_BSU
		   "-bsu;Load a sidecar app|"
#endif
#if CFG_ZLIB
		   "-z;Load gzip-compressed file (default)|"
		   "-nz;Load uncompressed file|"
#endif
#if CFG_SPLASH
		   "-splash;Load a BMP file and display it|"
#endif
		   "-loader=*;Specify BOLT loader name|"
		   "-tftp;Load the file using the TFTP protocol|"
		   "-fatfs;Load the file from a FAT file system|"
		   "-rawfs;Load the file from an unformatted file system|"
		   "-fs=*;Specify BOLT file system name|"
		   "-offset=*;Begin loading at this offset in the file or device|"
		   "-max=*;Specify the maximum number of bytes to load (raw and zImage)|"
		   "-addr=*;Specify the load address (hex) (raw and zImage)");

	cmd_addcmd("go", ui_cmd_go, NULL,
		   "Start a previously loaded program.",
		   "go [address]\n\n"
		   "The 'go' command will start a program previously loaded with\n"
		   "the 'load' command.  You can override the start address by\n"
		   "specifying it as a parameter to the 'go' command.",
#if CFG_BSU
		   "-bsu;run as a sidecar app|"
#endif
		   "-noclose;Don't close network link before executing program");
#endif

#if (CFG_CMD_LEVEL >= 3)
	cmd_addcmd("batch", ui_cmd_batch, NULL,
		   "Load a batch file into memory and execute it",
		   "batch [-options] host:filename|dev:filename\n\n"
		   "This command loads and executes a batch file from a boot device",
#if CFG_ZLIB
		   "-z;Load compressed file|"
		   "-nz;Load uncompressed file (default)|"
#endif
		   "-tftp;Load the file using the TFTP protocol|"
		   "-fatfs;Load the file from a FAT file system|"
		   "-rawfs;Load the file from an unformatted file system|"
		   "-rawfs;Load the file from an unformatted file system|"
		   "-loader=*;Specify another loader|"
		   "-fs=*;Specify BOLT file system name");
#endif

	return 0;
}

#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_go(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *arg;

	arg = cmd_getarg(cmd, 0);
	if (arg)
		bolt_loadargs.la_entrypt = XTOI(arg);

	if (cmd_sw_isset(cmd, "-noclose"))
		bolt_loadargs.la_flags |= LOADFLG_NOCLOSE;

	if (cmd_sw_isset(cmd, "-bsu"))
		bolt_loadargs.la_flags |= LOADFLG_BSU;
	else
		bolt_loadargs.la_flags &= ~LOADFLG_BSU;

	return bolt_go(&bolt_loadargs);
}
#endif

/*
 * Determine the loader type from the command line flags and device type.
 */
static const char *ui_get_loader(ui_cmdline_t *cmd, int dev_type, int flags)
{
#if defined(CFG_ARCH_MIPS)
	const char *default_loader = "elf";
#elif CFG_ZIMG_LDR
	const char *default_loader = "zimg"; /* default for ARM is '-zimg' */
#elif CFG_RAW_LDR
	const char *default_loader = "raw";
#else
	const char *default_loader = NULL;
#endif
	const char *loader;

	if (cmd_sw_value(cmd, "-loader", &loader))
		return loader;
	if (cmd_sw_isset(cmd, "-zimg"))
		return "zimg";
	if (cmd_sw_isset(cmd, "-raw"))
		return "raw";
	if (cmd_sw_isset(cmd, "-srec"))
		return "srec";
	if (cmd_sw_isset(cmd, "-elf"))
		return "elf";

	/* batch commands are always text, unless overriden above */
	if (flags & LOADFLG_BATCH)
		return "raw";

	/* Special case: S-Rec is default for serial device */
	if (dev_type == BOLT_DEV_SERIAL)
		return "srec";

	return default_loader;
}

/*
 * Determine the filesystem type from the command flags or the device type. If
 * it appears to be an invalid BOLT device, then it's probably a TFTP host
 * name.
 */
static const char *ui_get_filesys(ui_cmdline_t *cmd, int dev_type)
{
	const char *filesys;

	if (cmd_sw_value(cmd, "-fs", &filesys))
		return filesys;
	if (cmd_sw_isset(cmd, "-rawfs"))
		return "raw";
	if (cmd_sw_isset(cmd, "-tftp"))
		return "tftp";
	if (cmd_sw_isset(cmd, "-fatfs"))
		return "fat";

	switch (dev_type) {
	case BOLT_DEV_DISK:
		return "fat";
	case BOLT_DEV_FLASH:
	case BOLT_DEV_SERIAL:
		return "raw";
	case BOLT_DEV_NETWORK:
		return "tftp";
	default:
		break;
	}
	if (dev_type < 0)
		/* It's probably a network boot; default to TFTP */
		return "tftp";

	/* Unknown devices default to raw */
	return "raw";
}

static int ui_cmd_bootcommon(ui_cmdline_t *cmd, int flags)
{
	int res, node;
	int info;
	char *arg;
	char progname[200];
	char *program;
	char *device;
	const char *loader;
	const char *x;
	bolt_loadargs_t *la = &bolt_loadargs;

	memset(la, 0, sizeof(*la));

	la->la_flags = flags;

	/*
	 * Set starting address and maximum size. You can either
	 * explicitly set this or let BOLT decide.
	 * If BOLT decides, the load address
	 * will be BOOT_START_ADDRESS in all cases.
	 * The size is dependant on the device type: block and flash
	 * devices will get this info from the boot block,
	 * and network devices will get the info by reaching EOF
	 * on reads, up to the maximum size of the boot area.
	 */

	la->la_address = BOOT_START_ADDRESS;
	la->la_maxsize = BOOT_AREA_SIZE;

	arg = cmd_getarg(cmd, 0);
	if (!arg) {
		xprintf("No program name specified\n");
		return -1;
	}

	strncpy(progname, arg, sizeof(progname));
	splitpath(progname, &device, &program);

	if (!device) {
		xprintf("No device name specified.\n");
		return -1;
	}

	/* More than 2 arguments means there were extra/unquoted boot args */
	if (cmd->argc > 3) {
		xprintf("Too many arguments, or an unquoted optional [arg]\n");
		return BOLT_ERR_INV_PARAM;
	}

	if (cmd_sw_isset(cmd, "-nz"))
		la->la_flags &= ~LOADFLG_COMPRESSED;
	else if (cmd_sw_isset(cmd, "-z"))
		la->la_flags |= LOADFLG_COMPRESSED;

	if (cmd_sw_isset(cmd, "-bsu"))
		la->la_flags |= LOADFLG_BSU;
	else
		la->la_flags &= ~LOADFLG_BSU;

	/*
	 * This is where we guess based on the device type what
	 * sort of load method we're going to use.
	 */

	info = bolt_getdevinfo(device);
	if (info >= 0)
		info &= BOLT_DEV_MASK;

	loader = ui_get_loader(cmd, info, flags);
	la->la_filesys = ui_get_filesys(cmd, info);

	if (info >= 0) {
		la->la_device = device;
		la->la_filename = program;
	} else {
		/*
		 * It's probably a network boot.  Default to TFTP
		 * if not overridden
		 */
#if CFG_NETWORK
		la->la_device = (char *)net_getparam(NET_DEVNAME);
#else
		la->la_device = NULL;
#endif
		la->la_filename = arg;
	}

	/*
	 * Fill in the loader args
	 */

	if (cmd_sw_value(cmd, "-offset", &x))
		la->la_offs = atoi(x);

	if (cmd_sw_value(cmd, "-max", &x)) {
		la->la_maxsize = atoi(x);
		la->la_flags |= LOADFLG_SPECADDR;
	}

	if (cmd_sw_value(cmd, "-addr", &x)) {
		la->la_address = XTOI(x);
		la->la_flags |= LOADFLG_SPECADDR;
	}

	if (cmd_sw_isset(cmd, "-noclose"))
		la->la_flags |= LOADFLG_NOCLOSE;
#if CFG_NETWORK
	/*
	 * Configure the network if necessary
	 */
	if (!strcmp(la->la_filesys, "tftp") && !la->la_device
	    && bolt_finddev(DEF_NETDEV))
		if (net_init(DEF_NETDEV) < 0 || do_dhcp_request(DEF_NETDEV) < 0)
			return -1;
#endif
	/*
	 * Pick up the FIRST command line parameter for use as an
	 * argument to the loaded program. For multiple arguments
	 * enclose them in quotes, e.g. 'X="text" Y=number'
	 */

	la->la_options = cmd_getarg(cmd, 1);

	if (la->la_options) {
		/*	 If we have (optional) arguments then
			keep Devicetree up to date about them.
			 If no options are given then the
			CURRENT value of bootargs in the Devicetree
			is used if you are booting (boot command.)
		*/
		bolt_devtree_params_t p;
		void *fdt;

		bolt_devtree_getenvs(&p);
		fdt = p.dt_address;

		if (fdt && !bolt_devtree_sane(fdt, NULL)) {
			/* test to see if /chosen node exists and if
			 * not we go and try to create it.
			 */
			node = bolt_devtree_node_from_path(fdt, "/chosen");
			if (node < 0) {
				res = bolt_devtree_addnode_at(fdt, "chosen",
					/* parent: root node */ 0, &node);
				if (res != BOLT_OK)
					goto out;
			}

			res = bolt_dt_addprop_str(fdt, node, "bootargs",
					la->la_options);
			if (res) {
				res = BOLT_ERR;
				goto out;
			}
		}
	}

	/*
	 * Note: we might not come back here if we really launch the program.
	 */

	xprintf("Loader:%s Filesys:%s Dev:%s File:%s Options:%s\n",
		loader, la->la_filesys, la->la_device, la->la_filename,
		la->la_options);

	res = bolt_boot(loader, la);

	/* Success when !LOADFLG_EXECUTE, so exit now. */
	if (res >= 0) {
#if CFG_SPLASH
		if (cmd_sw_isset(cmd, "-splash"))
			splash_api_replace_bmp((uint8_t *)la->la_address,
					NULL);
#endif
		return res;
	}

out:
	/*
	 * Give the bad news.
	 */

	xprintf("Could not load %s: %s\n", program, bolt_errortext(res));

	/* Delete bootargs from DT on failure */
	if (la->la_options) {
		bolt_devtree_params_t p;
		void *fdt;

		bolt_devtree_getenvs(&p);
		fdt = p.dt_address;

		if (fdt && !bolt_devtree_sane(fdt, NULL))
			bolt_devtree_delprop_path(fdt, "chosen", "bootargs");
	}

	return res;
}

#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_load(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int flags = LOADFLG_NOISY;

	return ui_cmd_bootcommon(cmd, flags);
}
#endif

static int ui_cmd_boot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int flags = LOADFLG_EXECUTE;

	return ui_cmd_bootcommon(cmd, flags);
}

#if (CFG_CMD_LEVEL >= 3)
static int ui_cmd_batch(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int flags = LOADFLG_NOISY | LOADFLG_EXECUTE | LOADFLG_BATCH;

	return ui_cmd_bootcommon(cmd, flags);
}
#endif

#if CFG_NETWORK
#if (CFG_CMD_LEVEL >= 1)
static int ui_cmd_save(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *x;
	uint8_t *start, *end;
	int len;
	char *fname;
	int res;

	fname = cmd_getarg(cmd, 0);

	x = cmd_getarg(cmd, 1);
	if (x)
		start = (uint8_t *) XTOI(x);
	else
		return ui_showusage(cmd);

	x = cmd_getarg(cmd, 2);
	if (x)
		len = xtoi(x);
	else
		return ui_showusage(cmd);

	end = start + len;

	res = bolt_savedata("tftp", "", fname, start, end);

	if (res < 0)
		return ui_showerror(res, "Could not dump data to network");
	else
		xprintf("%d bytes written to %s\n", res, fname);

	return 0;
}
#endif
#endif
