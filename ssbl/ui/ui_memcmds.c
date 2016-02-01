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

#include "ui_command.h"
#include "bolt.h"

static int ui_cmd_heapstats(ui_cmdline_t *cmd, int argc, char *argv[]);

int ui_init_memcmds(void)
{
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
