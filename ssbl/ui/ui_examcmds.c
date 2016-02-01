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
#include "lib_hexdump.h"

#include "error.h"
#include "console.h"

#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"
#include "disasm.h"

#include "exchandler.h"
#include "bsp_config.h"

static int ui_cmd_memdump(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_memedit(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_memfill(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_disasm(ui_cmdline_t *cmd, int argc, char *argv[]);

#ifdef __long64
#define XTOI(x) xtoq(x)
#else
#define XTOI(x) xtoi(x)
#endif

#define ATYPE_SIZE_NONE	0
#define ATYPE_SIZE_BYTE	1
#define ATYPE_SIZE_HALF	2
#define ATYPE_SIZE_WORD	4
#define ATYPE_SIZE_QUAD	8
#define ATYPE_SIZE_MASK	0x0F

#define ATYPE_TYPE_NONE	0
#define ATYPE_TYPE_PHYS 0x10
#define ATYPE_TYPE_KERN 0x20
#define ATYPE_TYPE_MASK 0xF0

static long prev_addr;	/* initialized below in ui_init_examcmds */
static int prev_length = 256;
static int prev_dlength = 16;
static int prev_wlen = ATYPE_SIZE_WORD;

static int getaddrargs(ui_cmdline_t *cmd, int *curtype, long *addr,
		       int *length)
{
	int wlen = *curtype;
	long newaddr;
	int newlen;
	char *x;

	if (cmd_sw_isset(cmd, "-b")) {
		wlen = ATYPE_SIZE_BYTE;
	} else if (cmd_sw_isset(cmd, "-h")) {
		wlen = ATYPE_SIZE_HALF;
	} else if (cmd_sw_isset(cmd, "-w")) {
		wlen = ATYPE_SIZE_WORD;
	} else if (cmd_sw_isset(cmd, "-q")) {
		wlen = ATYPE_SIZE_QUAD;
	}
	if (wlen == 0)
		wlen = 1;	/* bytes are the default */

#if CFG_ARCH_ARM /* SWBOLT-96 */
	CACHE_FLUSH_ALL();
#endif

	*curtype = wlen;

	if (addr) {
		x = cmd_getarg(cmd, 0);
		if (x) {
			if (strcmp(x, ".") == 0)
				newaddr = *addr;
			else {
				/*
				 * hold on to your lunch, this is really,
				 * really bad! Make 64-bit addresses expressed
				 * as 8-digit numbers sign extend automagically.
				 * Saves typing, but is very gross.
				 */
				int longaddr = 0;

				longaddr = strlen(x);
				if (memcmp(x, "0x", 2) == 0)
					longaddr -= 2;
				longaddr = (longaddr > 8) ? 1 : 0;

				if (longaddr)
					newaddr = (long)xtoq(x);
				else
					newaddr = (long)xtoi(x);
			}
			/* align to natural boundary */
			*addr = newaddr & ~(wlen - 1);
		}
	}

	if (length) {
		x = cmd_getarg(cmd, 1);
		if (x) {
			newlen = (long)xtoi(x);
			*length = newlen;
		}
	}

	return 0;

}

static int stuffmem(long addr, int wlen, char *tail)
{
	char *tok;
	int count = 0;
	uint8_t b;
	uint16_t h;
	uint32_t w;
	uint64_t q;
	int res = 0;

	addr &= ~(wlen - 1);

	while ((tok = gettoken(&tail))) {
		switch (wlen) {
		default:
		case 1:
			b = (uint8_t) xtoq(tok);
			res = mem_poke(addr, b, MEM_BYTE);
			if (res) {
				/* Did not edit */
				return res;
			}
			break;
		case 2:
			h = (uint16_t) xtoq(tok);
			res = mem_poke(addr, h, MEM_HALFWORD);
			if (res) {
				/* Did not edit */
				return res;
			}
			break;
		case 4:
			w = (uint32_t) xtoq(tok);
			res = mem_poke(addr, w, MEM_WORD);
			if (res) {
				/* Did not edit */
				return res;
			}
			break;
		case 8:
			q = (uint64_t) xtoq(tok);
			res = mem_poke(addr, q, MEM_QUADWORD);
			if (res) {
				/* Did not edit */
				return res;
			}
			break;
		}

		addr += wlen;
		count++;
	}

	return count;
}

static int ui_cmd_memedit(ui_cmdline_t *cmd, int argc, char *argv[])
{
	uint8_t b;
	uint16_t h;
	uint32_t w;
	uint64_t q;

	long addr;
	char *vtext;
	int wlen;
	int count;
	int idx = 1;
	int stuffed = 0;
	int res = 0;

	getaddrargs(cmd, &prev_wlen, &prev_addr, NULL);

	wlen = prev_wlen;

	vtext = cmd_getarg(cmd, idx++);

	addr = prev_addr;

	while (vtext) {
		count = stuffmem(addr, wlen, vtext);
		if (count < 0) {
			ui_showerror(count, "Could not modify memory");
			return count;	/* error */
		}
		addr += count * wlen;
		prev_addr += count * wlen;
		stuffed += count;
		vtext = cmd_getarg(cmd, idx++);
	}

	if (stuffed == 0) {
		char line[256];
		char prompt[32];

		xprintf
		    ("Type '.' to exit, '-' to back up, '=' to dump memory.\n");
		for (;;) {

			addr = prev_addr;

			xprintf("%08lx%c ", prev_addr,
				(addr != prev_addr) ? '%' : ':');

			switch (wlen) {
			default:
			case 1:
				res = mem_peek(&b, addr, MEM_BYTE);
				if (res)
					return res;
				xsprintf(prompt, "[%02x]: ", b);
				break;
			case 2:
				res = mem_peek(&h, addr, MEM_HALFWORD);
				if (res)
					return res;
				xsprintf(prompt, "[%04x]: ", h);
				break;
			case 4:
				res = mem_peek(&w, addr, MEM_WORD);
				if (res)
					return res;
				xsprintf(prompt, "[%08x]: ", w);
				break;
			case 8:
				res = mem_peek(&q, addr, MEM_QUADWORD);
				if (res)
					return res;
				xsprintf(prompt, "[%016llx]: ", q);
				break;
			}

			console_readline(prompt, line, sizeof(line));
			if (line[0] == '-') {
				prev_addr -= wlen;
				continue;
			}
			if (line[0] == '=') {
				lib_hexdump((void *)prev_addr, 16, wlen, true,
					    DUMP_PREFIX_ADDRESS);
				continue;
			}
			if (line[0] == '.')
				break;
			if (line[0] == '\0') {
				prev_addr += wlen;
				continue;
			}
			count = stuffmem(addr, wlen, line);
			if (count < 0)
				return count;
			if (count == 0)
				break;
			prev_addr += count * wlen;
		}
	}

	return 0;
}

static int ui_cmd_memfill(ui_cmdline_t *cmd, int argc, char *argv[])
{
	long addr;
	char *atext;
	int wlen;
	int idx = 2;
	int len;
	uint64_t pattern;
	uint8_t *b_ptr;
	uint16_t *h_ptr;
	uint32_t *w_ptr;
	uint64_t *q_ptr;
	int res;

	getaddrargs(cmd, &prev_wlen, &prev_addr, &len);

	atext = cmd_getarg(cmd, idx++);
	if (!atext)
		return ui_showusage(cmd);
	pattern = xtoq(atext);

	addr = prev_addr;

	wlen = prev_wlen;

	switch (wlen) {
	case 1:
		b_ptr = (uint8_t *) addr;
		while (len > 0) {
			res = mem_poke(((long)(b_ptr)), pattern, MEM_BYTE);
			if (res) {
				/*Did not edit */
				return 0;
			}
			b_ptr++;
			len--;
		}
		break;
	case 2:
		h_ptr = (uint16_t *) addr;
		while (len > 0) {
			res = mem_poke(((long)(h_ptr)), pattern, MEM_HALFWORD);
			if (res)
				return 0;
			h_ptr++;
			len--;
		}
		break;
	case 4:
		w_ptr = (uint32_t *) addr;
		while (len > 0) {
			res = mem_poke(((long)(w_ptr)), pattern, MEM_WORD);
			if (res)
				return -1;
			w_ptr++;
			len--;
		}
		break;
	case 8:
		q_ptr = (uint64_t *) addr;
		while (len > 0) {
			res = mem_poke(((long)(q_ptr)), pattern, MEM_QUADWORD);
			if (res)
				return 0;
			q_ptr++;
			len--;
		}
		break;
	}

	return 0;
}

#define FILL(ptr, len, pattern) \
	do { \
		printf("Pattern: %016llX\n", pattern); \
		for (idx = 0; idx < len; idx++) \
			ptr[idx] = pattern; \
	} while (0)
#define CHECK(ptr, len, pattern) \
	do { \
		for (idx = 0; idx < len; idx++) { \
			if (ptr[idx] != pattern) { \
				printf("Mismatch at %016llX: Expected %016llX got %016llX", \
				       (uint64_t) (uintptr_t) &(ptr[idx]), \
				       pattern, ptr[idx]); \
				error = 1; loopmode = 0; break; \
			} \
		} \
	} while (0)

#define MEMTEST(ptr, len, pattern) \
	do { \
		if (!error) { \
			FILL(ptr, len, pattern); \
			CHECK(ptr, len, pattern); \
		} \
	} while (0)

static int ui_cmd_memtest(ui_cmdline_t *cmd, int argc, char *argv[])
{
	long addr = 0;
	int len = 0;
	int wtype = 0;
	int wlen;
	int idx = 0;
	volatile uint64_t *ptr;
	unsigned int error = 0;
	int loopmode = 0;
	int pass = 0;

	getaddrargs(cmd, &wtype, &addr, &len);

	wlen = 8;
	addr &= ~(wlen - 1);

	if (cmd_sw_isset(cmd, "-loop"))
		loopmode = 1;

	len /= wlen;

	ptr = (uint64_t *) addr;
	pass = 0;
	for (;;) {
		if (loopmode) {
			printf("Pass %d\n", pass);
			if (console_status())
				break;
		}
		MEMTEST(ptr, len, (unsigned long long)(idx * 8LL));
		MEMTEST(ptr, len, 0LL);
		MEMTEST(ptr, len, 0xFFFFFFFFFFFFFFFFLL);
		MEMTEST(ptr, len, 0x5555555555555555LL);
		MEMTEST(ptr, len, 0xAAAAAAAAAAAAAAAALL);
		MEMTEST(ptr, len, 0xFF00FF00FF00FF00LL);
		MEMTEST(ptr, len, 0x00FF00FF00FF00FFLL);
		if (!loopmode)
			break;
		pass++;
	}

	return 0;
}

static int ui_cmd_memdump(ui_cmdline_t *cmd, int argc, char *argv[])
{
	getaddrargs(cmd, &prev_wlen, &prev_addr, &prev_length);

	lib_hexdump((void *)prev_addr, prev_length, prev_wlen, true,
		    DUMP_PREFIX_ADDRESS);

	prev_addr += prev_length;

	return 0;
}

static int ui_cmd_disasm(ui_cmdline_t *cmd, int argc, char *argv[])
{
	long addr;
	char buf[512];
	int idx;
	uint32_t inst;
	int res;

	getaddrargs(cmd, &prev_wlen, &prev_addr, &prev_dlength);
	prev_addr &= ~3;

	addr = prev_addr;

	for (idx = 0; idx < prev_dlength; idx++) {
		res = mem_peek(&inst, addr, MEM_WORD);
		if (res) {
			ui_showerror(res, "Could not disassemble memory");
			return res;
		}
		disasm_inst(buf, sizeof(buf), inst, (uint64_t) prev_addr);

		xprintf("%08lx%c %08x    %s\n", prev_addr,
			(addr != prev_addr) ? '%' : ':', inst, buf);
		addr += 4;
		prev_addr += 4;
	}

	return 0;
}

static char *COND = "?<=>&";
static int ui_cmd_memeval(ui_cmdline_t *cmd, int argc, char *argv[])
{
	long addr;
	int res, wlen;
	int op = 2;
	uint8_t b;
	uint16_t h;
	uint32_t w, val;
	char *arg;

	if (cmd_sw_isset(cmd, "-lt"))
		op = 1;
	else if (cmd_sw_isset(cmd, "-eq"))
		op = 2;
	else if (cmd_sw_isset(cmd, "-gt"))
		op = 3;
	else if (cmd_sw_isset(cmd, "-and"))
		op = 4;

	getaddrargs(cmd, &prev_wlen, &prev_addr, NULL);

	addr = prev_addr;
	wlen = prev_wlen;
	addr &= ~(wlen - 1);

	switch (wlen) {
	default:
	case 1:
		res = mem_peek(&b, addr, MEM_BYTE);
		if (!res)
			w = (uint32_t) b;
		break;
	case 2:
		res = mem_peek(&h, addr, MEM_HALFWORD);
		if (!res)
			w = (uint32_t) h;
		break;
	case 4:
		res = mem_peek(&w, addr, MEM_WORD);
		break;
	case 8:
		res = BOLT_ERR_INV_PARAM;
		break;
	}

	if (res < 0)
		ui_showerror(res, "Could not test memory with value");
	else {
		arg = cmd_getarg(cmd, 1);
		if (!arg)
			return ui_showusage(cmd);
		val = xtoi(arg);
		if (cmd_sw_isset(cmd, "-pr"))
			printf("*** Testing %#x %c %#x\n", w, COND[op], val);
		switch (op) {
		case (1):
			res = (w < val) ? 0 : 1;
			break;
		case (2):
			res = (w == val) ? 0 : 1;
			break;
		case (3):
			res = (w > val) ? 0 : 1;
			break;
		case (4):
			res = (w & val) ? 0 : 1;
			break;
		}
	}

	return res;
}

int ui_init_examcmds(void)
{
	cmd_addcmd("u",
		   ui_cmd_disasm,
		   NULL,
		   "Disassemble instructions.",
		   "u [addr [length]]\n\n"
		   "This command disassembles instructions at the specified address.\n"
		   "BOLT will display standard register names and symbolic names for\n"
		   "certain CP0 registers.  The 'u' command remembers the last address\n"
		   "that was disassembled so you can enter 'u' again with no parameters\n"
		   "to continue a previous request.\n",
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)");

	cmd_addcmd("d",
		   ui_cmd_memdump,
		   NULL,
		   "Dump memory.",
		   "d [-b|-h|-w|-q] [addr [length]]\n\n"
		   "This command displays data from memory as bytes, halfwords, words,\n"
		   "or quadwords.  ASCII text, if present, will appear to the right of\n"
		   "the hex data.  The dump command remembers the previous word size,\n"
		   "dump length and last displayed address, so you can enter 'd' again\n"
		   "to continue a previous dump request.",
		   "-b;Dump memory as bytes|"
		   "-h;Dump memory as halfwords (16-bits)|"
		   "-w;Dump memory as words (32-bits)|"
		   "-q;Dump memory as quadwords (64-bits)|"
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)");

	cmd_addcmd("e",
		   ui_cmd_memedit,
		   NULL,
		   "Modify contents of memory.",
		   "e [-b|-h|-w|-q] [addr [data...]]\n\n"
		   "This command modifies the contents of memory.  If you do not specify\n"
		   "data on the command line, BOLT will prompt for it.  When prompting for\n"
		   "data you may enter '-' to back up, '=' to dump memory at the current\n"
		   "location, or '.' to exit edit mode.",
		   "-b;Edit memory as bytes|"
		   "-h;Edit memory as halfwords (16-bits)|"
		   "-w;Edit memory as words (32-bits)|"
		   "-q;Edit memory as quadwords (64-bits)|"
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)");

	cmd_addcmd("f",
		   ui_cmd_memfill,
		   NULL,
		   "Fill contents of memory.",
		   "f [-b|-h|-w|-q] addr length pattern\n\n"
		   "This command modifies the contents of memory.  You can specify the\n"
		   "starting address, length, and pattern of data to fill (in hex)\n",
		   "-b;Edit memory as bytes|"
		   "-h;Edit memory as halfwords (16-bits)|"
		   "-w;Edit memory as words (32-bits)|"
		   "-q;Edit memory as quadwords (64-bits)|"
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)");

	cmd_addcmd("t",
		   ui_cmd_memeval,
		   NULL,
		   "Test contents of memory.",
		   "t [-b|-h|-w] [-lt|-eq|-gt] addr pattern\n\n"
		   "This command tests the contents of memory against the specified\n"
		   "pattern.\n",
		   "-b;Edit memory as bytes|"
		   "-h;Edit memory as halfwords (16-bits)|"
		   "-w;Edit memory as words (32-bits)|"
		   "-eq;test if variable is equal to specified value.|"
		   "-gt;test if variable is greater than specified value.|"
		   "-lt;test if variable is less than specified value.|"
		   "-and;bit test variable against mask pattern.|"
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)|"
		   "-pr;print operation");

	cmd_addcmd("memtest",
		   ui_cmd_memtest,
		   NULL,
		   "Test memory.",
		   "memtest [options] addr length\n\n"
		   "This command tests memory.  It is a very crude test, so don't\n"
		   "rely on it for anything really important.  Addr and length are in hex\n",
		   "-p;Address is an uncached physical address (deprecated)|"
		   "-v;Address is a kernel virtual address (deprecated)|"
		   "-loop;Loop till keypress");

	prev_addr = UNCADDR(0);

	return 0;
}
