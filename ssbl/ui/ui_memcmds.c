/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <bolt.h>
#include <error.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <ssbl-common.h>
#include <ui_command.h>

#include <stdbool.h>

static int ui_cmd_heapstats(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_reserve_memory(ui_cmdline_t *cmd, int argc, char *argv[]);

int ui_init_memcmds(void)
{
	cmd_addcmd("rmem", ui_cmd_reserve_memory, NULL,
		   "Reserve memory area",
		   "rmem <amount> [-4gb] [-abs] [-align=*] [-dt=*] [-memc=*] "
		   "[-nomap]\n\t[-reuse] [-tag=*]\n\n"
		   "The 'rmem' command reserves memory with the specified\n"
		   "amount and alignment (or, at a specified offset).\n\n"
		   "Without 'amount', reserved and available memory areas "
		   "are displayed.",
		   "amount; amount of memory to be reserved in bytes|"
		   "-4gb; okay to exceed the 4GB boundary|"
		   "-abs; the alignment is absolute offset|"
		   "-align=*; specifies the alignment in bytes, default 1MB|"
		   "-dt=*; None, /Memreserve or /Reserved-memory, default N|"
		   "-memc=*; bit vector of preferred MEMC, default MEMC#0|"
		   "-nomap; add no-map if /reserved-memory|"
		   "-reuse; add reusable if /reserved-memory|"
		   "-tag=*; tag that will be associated with reservation");

	cmd_addcmd("show heap",
		   ui_cmd_heapstats,
		   NULL,
		   "Display information about BOLT's heap",
		   "show heap\n\n"
		   "This is a debugging command that can be used to determine the health\n"
		   "of BOLT's internal memory manager.", "");

	return 0;
}

static int ui_cmd_heapstats(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int res;
	memstats_t stats;

	res = KMEMSTATS(&stats);

	xprintf("\n");
	xprintf("Total bytes:       %d\n", stats.mem_totalbytes);
	xprintf("Free bytes:        %d\n", stats.mem_freebytes);
	xprintf("Free nodes:        %d\n", stats.mem_freenodes);
	xprintf("Allocated bytes:   %d\n", stats.mem_allocbytes);
	xprintf("Allocated nodes:   %d\n", stats.mem_allocnodes);
	xprintf("Largest free node: %d\n", stats.mem_largest);
	xprintf("Heap status:       %s\n",
		(res == 0) ? "CONSISTENT" : "CORRUPT!");
	xprintf("\n");

	return res;
}

static int ui_cmd_reserve_memory(ui_cmdline_t *cmd, int argc, char *argv[])
{
	uint32_t memc_selection = BOLT_RESERVE_MEMORY_OPTION_MEMC_0;
	unsigned int amount;
	uint64_t alignment = 1024*1024; /* 1MB, by default */
	unsigned int options = 0;
	const char *x;
	char *tag = NULL;
	int64_t retval;

	if (cmd_sw_isset(cmd, "-4gb"))
		options |= BOLT_RESERVE_MEMORY_OPTION_OVER4GB;

	if (cmd_sw_isset(cmd, "-abs"))
		options |= BOLT_RESERVE_MEMORY_OPTION_ABS;

	if (cmd_sw_value(cmd, "-align", &x))
		alignment = atoi(x);

	if (cmd_sw_value(cmd, "-dt", &x)) {
		switch (x[0]) {
		case 'm':
		case 'M':
			options |= BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY;
			break;
		case 'r':
		case 'R':
			options |= BOLT_RESERVE_MEMORY_OPTION_DT_NEW;
			break;
		case 'n':
		case 'N':
			/* do nothing */
			break;
		default:
			return ui_showusage(cmd);
		}
	}

	if (cmd_sw_value(cmd, "-memc", &x))
		memc_selection = atoi(x) & BOLT_RESERVE_MEMORY_OPTION_MEMC_MASK;
	options |= memc_selection;

	if (cmd_sw_isset(cmd, "-nomap"))
		options |= BOLT_RESERVE_MEMORY_OPTION_DT_NOMAP;

	if (cmd_sw_isset(cmd, "-reuse"))
		options |= BOLT_RESERVE_MEMORY_OPTION_DT_REUSABLE;

	if (cmd_sw_value(cmd, "-tag", &x))
		tag = strdup(x);

	x = cmd_getarg(cmd, 0);
	if (x == NULL) {
		bolt_reserve_memory_getstatus();
		return BOLT_OK;
	}

	amount = atoi(x);
	if (amount == 0) {
		err_msg("Requesting 0 (zero) byte is illegal\n");
		return BOLT_ERR_INV_PARAM;
	}

	if (amount % _KB(4))
		warn_msg("At least 4KB is recommended for reservation unit\n");

	if (!(options & BOLT_RESERVE_MEMORY_OPTION_ABS)) {
		if (alignment < _KB(4))
			warn_msg("At least 4KB is recommended for alignment\n");
	}


	retval = bolt_reserve_memory(amount, alignment, options, tag);
	if (retval < 0)
		return retval;

	xprintf("Reserved at %llx (%lld)\n", retval, retval);

	return BOLT_OK;
}
