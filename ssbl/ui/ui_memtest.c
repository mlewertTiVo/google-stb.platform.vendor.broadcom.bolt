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
#if (CFG_CMD_LEVEL >= 3)

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "common.h"
#include "devfuncs.h"
#include "error.h"
#include "board.h"
#include "cache_ops.h"

#include "ui_command.h"


#define WRAP_TEST_VAL 0xaa55aa55
#define WRAP_TEST_CMP (~(WRAP_TEST_VAL))

#define U32toV32(v) (volatile uint32_t *)((uint32_t)(v))
#define V32toU32(v) ((uint32_t)(v))

#define MKV32MB(v) U32toV32((uint32_t)_MB((uint32_t)v))



static int ui_cmd_memwrap(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_uncache(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_crc(ui_cmdline_t *cmd, int argc, char *argv[]);


int ui_init_memtestcmds(void)
{
	cmd_addcmd("memwrap",
		ui_cmd_memwrap,
		NULL,
		"Test for end of (ddr) memory.\n"
		"The -ddr option pre-fills -base & -top values, which\n"
		"may be overridden by your own (optional) -base & -top.",
		"memwrap [options]",
		"-ddr=*;which ddr to test on (optional)|"
		"-base=*;start address|"
		"-top=*;end address|"
		"-cont;continue testing after a test fail|"
		"-step=*;step size, in Mb");

	cmd_addcmd("uncache",
		ui_cmd_uncache,
		NULL,
		"Mark (all ddr) memory access via the mmu as uncached.\n"
		"Effects vary depending upon the architecture.",
		"uncache [options]",
		"-override;ignore ddr settings and config ALL memory up to\n"
		"MMU_MEMTOP as unacached in the mmu|"
		"-nommu;Turn off the mmu. On some architectures this also\n"
		"turns off all caches e.g. ARM");

	cmd_addcmd("crc",
		ui_cmd_crc,
		NULL,
		"Report the CRC32 for a memory range.",
		"crc [options]",
		"-offset=*;start address|"
		"-size=*;size, in bytes");

	return 0;
}


static void uncache_range(unsigned int b, unsigned int s)
{
	xprintf("uncache: 0x%08x => 0x%08x...", b, b + s);

	/* this fn should also be responsible for cache & tlb
	flushing */
	arch_mark_uncached(b, s);

	xprintf("ok\n");
}

static int ui_cmd_uncache(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct board_type *bt;
	struct ddr_info *ddr;
	unsigned int i;

	bt = board_thisboard();
	if (!bt)
		return BOLT_ERR_UNSUPPORTED;

	if (cmd_sw_isset(cmd, "-override")) {
		uncache_range(0, _MB((uint32_t)MMU_MEMTOP));
	} else {
		for (i = 0; i < bt->nddr; i++) {
			ddr = board_find_ddr(bt, i);
			if (!ddr)
				continue;
			uncache_range(_MB(ddr->base_mb), _MB(ddr->size_mb));
		}
	}

	if (cmd_sw_isset(cmd, "-nommu")) {
		xprintf("disable mmu...");
		disable_caches();
		xprintf("ok\n");
	}
	return 0;
}


/* check we can write & then read a value
*/
static int __optimize_O0 wr_test_running_bit(volatile uint32_t *p)
{
	int i;
	volatile uint32_t q = 1;

	/* use a for loop as testing *p on RO memory
	may have us looping forever.
	*/
	for (i = 0; i < 32; i++) {
			*p = q;
		CACHE_FLUSH_ALL();
		if (*p != q)
			return 1;
		q = q << 1;
	}
	return 0;
}


/* check memory wrap/shadowing between two locations.
*/
static int __optimize_O0 wrap_test_eq(volatile uint32_t *base,
	volatile uint32_t *pos)
{
	int b, p, w;

	/* Note the ordering of the writes!
	*/
	*pos  = WRAP_TEST_CMP;
	*base = WRAP_TEST_VAL;

	CACHE_FLUSH_ALL();

	xprintf("\tcmp[%x %x]", *base, *pos);

	b = ((*base) == WRAP_TEST_VAL);
	p = ((*pos)  == WRAP_TEST_CMP);

	w = ((*pos)  == WRAP_TEST_VAL);

	xprintf(" ref %s, addr %s%s\n",
			b ? "ok" : "BAD!",
			p ? "ok" : "BAD!",
			w ? ", WRAP detected!" : "");

	return  !(b && p);
}


static int ui_cmd_memwrap(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct board_type *b;
	struct ddr_info *ddr;
	volatile uint32_t *base = MKV32MB(0), *top = MKV32MB(MMU_MEMTOP);
	volatile uint32_t *pos, *next;
	uint32_t offs, step = 256;
	int i, cont;
	const char *s;

	cont = cmd_sw_isset(cmd, "-cont");

	if (cmd_sw_value(cmd, "-step", &s))
		step = atoi(s);

	if (cmd_sw_value(cmd, "-ddr", &s)) {
		i = atoi(s);

		xprintf("ddr #%d, ", i);

		b = board_thisboard();
		if (!b)
			return BOLT_ERR_UNSUPPORTED;

		ddr = board_find_ddr(b, i);
		if (!ddr) {
			xprintf("no such ddr!\n");
			return BOLT_ERR_NOMEM;
		}

		base = MKV32MB(ddr->base_mb);
		top  = MKV32MB(ddr->base_mb + ddr->size_mb);
	}

	if (cmd_sw_value(cmd, "-base", &s))
		base = U32toV32(atoi(s));

	if (cmd_sw_value(cmd, "-top", &s))
		top = U32toV32(atoi(s));

	if (top > MKV32MB(MMU_MEMTOP)) {
		xprintf("warning: 'top' exceeds mmu mapping. %p > %p\n",
				top, MKV32MB(MMU_MEMTOP));
	}

	xprintf("scan %p to %p step 0x%08x (%dMb)\n",
			base, top, _MB(step), step);

	step = _MB(step);

	pos = U32toV32(V32toU32(base) + step /*- sizeof(uint32_t)*/);

	/* --- begin tests --- */

	if (wr_test_running_bit(base)) {
		xprintf("cannot modify 'base' (%p)\n", base);
		if (!cont)
			return BOLT_ERR;
	}

	do {
		offs = sizeof(uint32_t) * (pos - base + sizeof(uint32_t));
		xprintf("%dMb\t%p - %p\t", offs/_MB(1), base, pos);

		/* If we run off the end of a ddr then we *may* get a wrap, but
		its not 100% guaranteed on all architectures. The other symptom
		is memory being unchangeable. This happens for ARM, with and
		without the mmu/caches being active.
		 For ARM: If caches are enabled (mmu on) unmapped memory, e.g.
		 more than MMU_MEMTOP but less than the peripheral space,
		 BOLT will hang (< v0.84) or get an (ABT) exception.
		*/
		if (wr_test_running_bit(pos)) {
			xprintf("cannot modify %p", pos);
			if (!wr_test_running_bit(&pos[-1]))
				xprintf(", can modify %p", &pos[-1]);
			if (!cont) {
				xprintf("\n");
				return BOLT_ERR;
			}
		}

		if (wrap_test_eq(base, pos)) {
			if (!cont)
				return BOLT_ERR;
		}

		next = (volatile uint32_t *)(((uint32_t)pos) + step);
		if (pos > next) {
			xprintf("end of test (pointer wrapped) %p => %p\n",
				pos, next);
			break;
		}

		pos = next;

	} while (pos <= top);

	return 0;
}


/* On the PC side, install crc32 (sudo apt-get install libarchive-zip-perl)
 if you want to do comparisons with the BOLT crc.
*/
static int ui_cmd_crc(ui_cmdline_t *cmd, int argc, char *argv[])
{
	unsigned int crc, offset = 0, size = 0;
	const char *s;
	const unsigned char *b;

	if (cmd_sw_value(cmd, "-size", &s))
		size = atoi(s);

	if (cmd_sw_value(cmd, "-offset", &s))
		offset = atoi(s);

	if (!size)
		return BOLT_ERR_INV_PARAM;

	b = (const unsigned char *)offset;

	xprintf("crc: 0x%x to 0x%x = ", offset, offset + size);
	crc = ~lib_crc32(b, size);
	xprintf(" 0x%x\n", crc);

	return 0;
}


#endif /* CFG_CMD_LEVEL >=3 */
