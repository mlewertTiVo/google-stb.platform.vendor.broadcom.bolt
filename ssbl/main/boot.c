/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <arch_ops.h>
#include <bootblock.h>
#include <bolt.h>
#include <board.h>
#include <bsu-api.h>
#include <cache_ops.h>
#include <console.h>
#include <devfuncs.h>
#include <device.h>
#include <devtree.h>
#include <env_subr.h>
#include <error.h>
#include <fileops.h>
#include <iocb.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <loader.h>
#include <net_api.h>
#include <net_ebuf.h>
#include <net_ether.h>
#include <timer.h>
#include <ui_command.h>

#if CFG_UI
extern int bolt_docommands(const char *buf);
#endif

#ifndef BATCH_CMD_BUF_SIZE	/* allow override via BSP header file */
#define BATCH_CMD_BUF_SIZE	2048
#endif

#define SKIP_WHITE_SPACE(s)\
	do {\
		while (*s && ((*s == ' ') || (*s == '\t')))\
			s++;\
	} while (0)


/*  *********************************************************************
    *  splitpath(path,devname,filename)
    *
    *  Split a path name (a boot path, in the form device:filename)
    *  into its parts
    *
    *  Input parameters:
    *      path - pointer to path string
    *      devname - receives pointer to device name
    *      filename - receives pointer to file name
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void splitpath(char *path, char **devname, char **filename)
{
	char *x = strchr(path, ':');

	if (!x) {
		*devname = path;	/* path consists of device name */
		*filename = NULL;
	} else {
		*x++ = '\0';	/* have both device and file name */
		*filename = x;
		*devname = path;
	}
}


/*  *********************************************************************
    *  bolt_go(la)
    *
    *  Starts a previously loaded program.  bolt_loadargs.la_entrypt
    *  must be set to the entry point of the program to be started
    *
    *  Input parameters:
    *      la - loader args
    *
    *  Return value:
    *      returns only when unable to boot program, and returns a negative int
    ********************************************************************* */

int bolt_go(bolt_loadargs_t *la)
{
	bolt_devtree_params_t p;
	int boot_bsu = la->la_flags & LOADFLG_BSU;
	int ret;
#ifdef STUB64_START
	struct dt_boot_mods bm = {0};
#endif

	if (la->la_entrypt == 0) {
		xprintf("No program has been loaded.\n");
		return BOLT_ERR_INV_COMMAND;
	}

	bolt_devtree_getenvs(&p);
	if (p.dt_address) {
		void *fdt = p.dt_address;

		/* boltset is allowed to fail if the tree is in RONLY memory */
		char *dt_off = env_getenv("DT_OFF");

		if (dt_off && strcmp(dt_off, "1") == 0) {
			xprintf("DT_OFF: bypass device tree modification\n");
		} else {
			/* Run-time DT patching */
			if (!bolt_devtree_sane(fdt, NULL)) {
				ret = bolt_devtree_boltset(fdt);
				if (ret) {
					err_msg("ERROR: DT generation failed");
					return BOLT_ERR;
				}
			}
		}

#ifdef STUB64_START
		if (!boot_bsu) {
			/* inform that we have a 32 bit app that needs to
			 * be run via a jump instead of an indirect method
			 * such as a PSCI call.
			 */
			bm.loader_32bit = (la->la_flags & LOADFLG_DIRECT_CALL);
			(void)bolt_devtree_boltset_boot(fdt, &bm);
		}
#endif
	}

#if CFG_NETWORK
	if (!(la->la_flags & LOADFLG_NOCLOSE)) {
		if (net_getparam(NET_DEVNAME)) {
			xprintf("Closing network '%s'\n",
				(char *)net_getparam(NET_DEVNAME));
			net_uninit();
		}
	}
#endif

	if (!boot_bsu)
		board_final_exit((unsigned long *)la->la_entrypt);

	/* 32/64 bit pointers? If DT_ADDRESS is > 32 bits on
	 * an A15 system then the upper u32 gets chopped off
	 * anyway. The coverity line is to silence warnings
	 * for just the 32 bit systems.
	 */
	if (sizeof(p.dt_address) > 4) {
		/* coverity[result_independent_of_operands] */
		if (upper_32_bits((uintptr_t)p.dt_address)) {
			err_msg("Cannot use DTB above 32-bit address range.\n");
			return BOLT_ERR_INV_PARAM;
		}
	}

	xprintf("Starting program at %#lx (DTB @ %p)\n\n",
		la->la_entrypt, p.dt_address);

	if (CFG_BSU && boot_bsu) {
		unsigned int base = ALIGN_TO(la->la_entrypt, _KB(4));
		unsigned int size =
			ALIGN_UP_TO(la->la_entrypt + la->la_maxsize - base,
				_KB(4));

		/* make sure that the loaded BSU code is executable */
		arch_mark_executable(base, size, true);

		/* BSU has to be in the same operating
		 * environment as BOLT as it calls back into BOLT,
		 * that means no aarch64/32 switches between the
		 * two, hence no alternate bolt_start_bsu64()
		 * for STUB64_START chips. Its all 32 or 64 bit,
		 * no arch or EL swapping.
		 */
		bolt_start_bsu(la->la_entrypt, 0xffffffff,
				(unsigned int)p.dt_address);

		/* DO NOT attempt taking executability back.
		 *
		 * It is possible that BSU returns back. But, taking
		 * executability back requires an extra additional glue
		 * logic between BSU and BOLT. Without such a glue logic,
		 * BSU may override BOLT's various memory areas.
		 */
		return BOLT_OK;
	}

	/* MMU is disabled right before jumping to the target,
	 * and should be re-enabled by code located at the target
	 * after re-constructing page table
	 */
#ifdef STUB64_START
	if (!(la->la_flags & LOADFLG_DIRECT_CALL)) {
		if (la->la_flags & LOADFLG_APP64) {
			xprintf("64 bit PSCI boot...\n");
			bolt_start64(la->la_entrypt, 0xffffffff,
					(unsigned int)p.dt_address, 0);
		} else {
			xprintf("32 bit PSCI boot...\n");
			bolt_start32(la->la_entrypt, 0xffffffff,
					(unsigned int)p.dt_address, 0);
		}
		/* In the remote event a non-BSU app saved
		 * BOLT state for a return.
		 */
		return BOLT_OK;
	} else
#endif
	{
		xprintf("32 bit boot...\n");
		bolt_start(la->la_entrypt, 0xffffffff,
			(unsigned int)p.dt_address, 0);
	}
	/* In the remote event a non-BSU app saved
	 * BOLT state for a return.
	 */
	return BOLT_OK;
}

#if CFG_UI
/*  *********************************************************************
    *  bolt_run_batch_file(cmds)
    *
    *  Executes a list of previously loaded commands from a batch file.
    *  Does a command terminated by end of line or ';'.
    *  Environment variable expansion is not handled.
    *
    *  Input parameters:
    *      cmds - batch commands
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

static void bolt_run_batch_file(char *cmds)
{
	char *p;
	int status, prev_status = 0, skip_cmd = 0, comment = 0;
	int inquote_d = 0, inquote_s = 0, tagged = 0, debug = 0;
	char *tag, *envtag;

	p = cmds;

	debug = env_getval("BATCH_DBG");
	if (debug < 0)
		debug = 0;

	while (*p) {		/* parse and execute the batch commands */
		if (cmds[0] == '#')	/* comment line */
			comment = 1;

		/* replace CR & TAB with a space */
		if ((*p == '\r') || (*p == '\t'))
			*p = ' ';
		else if (*p == '\'') {
			/* but only if not in double quotes */
			if (inquote_d == 0)
				inquote_s = !inquote_s;
		} else if (*p == '"') {
			/* but only if not in single quotes */
			if (inquote_s == 0)
				inquote_d = !inquote_d;
		} else if ((*p == '\n') |
				/* ^ new line cmd terminator */
			(!comment && !inquote_d &&
			!inquote_s && (*p == ';'))) {
			/*		^inline cmd terminator */

			*p = 0;
			SKIP_WHITE_SPACE(cmds);
			if (debug & 1)
				printf("{BATCH} <%s>\n", cmds);

			/* Check for logical operators first at start
			 of command for chained commands.
			 */

			if ((cmds[0] == '&') && (cmds[1] == '&')) {
				if (prev_status)
					skip_cmd = 1;
				cmds += 2;
				SKIP_WHITE_SPACE(cmds);
			} else if ((cmds[0] == '|') && (cmds[1] == '|')) {
				if (!prev_status)
					skip_cmd = 1;
				cmds += 2;
				SKIP_WHITE_SPACE(cmds);
			}

			/* Check for batch command file tag to see if this file
			should be continued....
			used mainly for sysinit feature.
			*/

			if (!comment && !lib_strncmp(cmds, "tagsave", 7)) {
				if (!skip_cmd && tagged) {
					if (debug & 1)
						printf
						    ("{BATCH} Saving tag...\n");
					env_save();
					tagged = 0;
				}
				skip_cmd = 1;
			} else if (!comment && !lib_strncmp(cmds, "tag ", 4)) {
				char *s;
				tag = cmds + 3;
				SKIP_WHITE_SPACE(tag);
				s = strchr(tag, ' '); /* skip trailing spaces */
				if (s)
					*s = 0;
				envtag = env_getenv("SYSINIT_TAG");
				if (envtag && !strcmp(tag, envtag)) {
					if (debug & 2)
						printf
						    ("{BATCH} Terminating batch file due to matching tags...\n");
					break;
				} else {
					if (debug & 1)
						printf(
							"{BATCH} Registering tag <%s>\n",
							tag);
					env_setenv("SYSINIT_TAG", tag,
						   ENV_FLG_NORMAL);
					tagged = 1;
				}
				skip_cmd = 1;
			}

			if (!(skip_cmd || comment)) {
				status = bolt_docommands(cmds);
				if (status != CMD_ERR_BLANK)
					prev_status = status;
				inquote_d = 0;
				inquote_s = 0;
			}
			cmds = p + 1;	/* advance to next command sequence */
		}
		++p;
		skip_cmd = 0;
		comment = 0;
	}
}
#endif

/*  *********************************************************************
    *  bolt_boot(la)
    *
    *  Bootstrap the system.
    *
    *  Input parameters:
    *      la - loader arguments
    *
    *  Return value:
    *      error, or does not return
    ********************************************************************* */
int bolt_boot(const char *ldrname, bolt_loadargs_t *la)
{
	int res = 0, noise = la->la_flags & LOADFLG_NOISY;
#if CFG_UI
	void *bcmdp = NULL;
#endif

	la->la_entrypt = 0;

	if (noise)
		xprintf("Loading: ");

	if (la->la_flags & LOADFLG_BATCH) {
#if CFG_UI
		/* allocate buffer for command file */
		bcmdp = KMALLOC(BATCH_CMD_BUF_SIZE, 0);

		memset(bcmdp, 0, BATCH_CMD_BUF_SIZE);

		la->la_address = (long)bcmdp;
		la->la_maxsize = BATCH_CMD_BUF_SIZE;
		la->la_flags |= LOADFLG_SPECADDR;
#else
		xprintf("Batch commands cannot work since UI is disabled.\n");
		return 0;
#endif
	}

#if (CFG_CMD_LEVEL >= 5)
		xprintf(" load_program() params:\n");
		xprintf(" ldrname  [%s]\n", ldrname);
		xprintf(" filename [%s]\n", la->la_filename);
		xprintf("  filesys [%s]\n", la->la_filesys);
		xprintf("   device [%s]\n", la->la_device);
		xprintf("  options [%s]\n", la->la_options);
		xprintf("  la_flags %#08x\n", la->la_flags);
		xprintf("  address %#08lx\n", la->la_address);
		xprintf("  maxsize %#08lx\n", la->la_maxsize);
		xprintf("  entrypt %#08lx\n", la->la_entrypt);
		xprintf("\n");
#endif
	res = bolt_load_program(ldrname, la);

	/* SWBOLT-332. Do a full d$ flush even if we fail (for
	* BBS/JTAG memory inspection.)
	*/
	CACHE_FLUSH_ALL();

	if (res < 0) {
		if (noise)
			xprintf("Failed.\n");
		goto out;
	}

	/*
	 * Special case: If loading a batch file, just do the commands here
	 * and return.
	 */

	if (la->la_flags & LOADFLG_BATCH) {
#if CFG_UI
		bolt_run_batch_file((char *)la->la_entrypt);
#endif
		res = 0;
		goto out;
	}

	/*
	 * Otherwise set up for running a real program.
	 */

	if (noise)
		xprintf("Entry address is %#lx\n", la->la_entrypt);

	/*
	 * Banzai!  Run the program.
	 */

	if ((la->la_flags & LOADFLG_EXECUTE) && (la->la_entrypt != 0))
		res = bolt_go(la);

out:
#if CFG_UI
	KFREE(bcmdp);
#endif

	return res;
}
