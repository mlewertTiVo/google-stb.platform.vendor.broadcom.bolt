/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#if defined(CFG_NOSHMOO) || defined(CFG_EMULATION)
/* don't compile anything in this file
*/
#else

#include <bchp_common.h>
#include "fsbl.h"
#include "fsbl-shmoo.h"
#include "fsbl-pm.h"
#include "boot_defines.h"
#include <aon_defs.h>
#include <memsys-if.h>

#if defined(BCHP_DDR34_PHY_CONTROL_REGS_A_0_REG_START) /* LPDDR4 */
#define DDR_PHY_STANDBY_CONTROL_OFFS_A \
		(BCHP_DDR34_PHY_CONTROL_REGS_A_0_STANDBY_CONTROL - DDR_PHY(0))

#define DDR_PHY_STANDBY_CONTROL_OFFS_B \
		(BCHP_DDR34_PHY_CONTROL_REGS_B_0_STANDBY_CONTROL - DDR_PHY(0))
#else
#define DDR_PHY_STANDBY_CONTROL_OFFS \
		(BCHP_DDR34_PHY_CONTROL_REGS_0_STANDBY_CONTROL - DDR_PHY(0))
#endif

#define ADDROF(base, offset) \
	((void *)((uint32_t)base + ((uint32_t)offset)))


static struct memsys_info *mcb_table = NULL;
static struct memsys_info *mcb_subtable = NULL;

/* loadable shmoo code */
static const struct memsys_interface *gmemsys;
static bool gloud_shmoo;


/* ------------------------------------------------------------------------- */

void set_loud_shmoo(void)
{
	gloud_shmoo = true;
	puts(" [shmoo messages on]");
}

static void print_shmoo_version(memsys_version_t *shmoo_ver)
{
	report_hex("@SHMOO ", shmoo_ver->version);

	__puts(" BLD:");
	__puts(shmoo_ver->build_ver);

	__puts(" HW:");
	__puts(shmoo_ver->hw_ver_str);

	__puts(" shmoo_ver:");
	__puts(shmoo_ver->ver_str);

#if 0 /* debug */
	__puts(" EDIS:");
	putchar('0' + EDIS_NPHY);
	putchar(',');
	writehex(EDIS_OFFS);
#endif
	crlf();
}


uint32_t *find_shmoo(const struct ddr_info *ddr,
		struct memsys_info *mi, uint32_t *sram_offset)
{
	while (mi->values) {
		if ((ddr->ddr_clock == mi->ddr_clock) &&
			(ddr->ddr_size  == mi->ddr_size) &&
			(ddr->phy_width == mi->phy_width) &&
			(ddr->ddr_width == mi->ddr_width)) {
			if (ddr->tag == NULL)
				return (uint32_t *)mi->values;
			if (mi->tag != NULL) {
				/* ddr->tag and mi-tag are not NULL */
				char *p = ddr->tag;
				char *q = (char *)ADDROF(sram_offset, mi->tag);

				while (*p == *q) {
					if (*p == '\0')
						return (uint32_t *)mi->values;
					++p;
					++q;
				}
			}
		}
		mi++;
	}
	return NULL;
}


static int is_valid_mcb(struct memsys_info *mi)
{
	if ((mi->values    != NULL) ||
		(mi->ddr_clock != MCB_MAGIC1) ||
		(mi->ddr_width != MCB_MAGIC3)) {
#if 0 /* debug */
		writehex((uint32_t)mi->values);
		__puts(" ");
		writehex(mi->ddr_clock);
		__puts(" ");
		writehex(mi->ddr_size);
		__puts(" ");
		writehex(mi->ddr_width);
		__puts(" ");
		writehex(mi->phy_width);
		__puts(" ");
#endif
		return 0;
	}

#if (CFG_CMD_LEVEL >= 3)
	/* MCB_MAGIC2 is now informational and
	just reports if any patched-in MCB table
	was from a different BOLT version. */
	if (mi->ddr_size != MCB_MAGIC2) {
		__puts("M2 ");
		fsbl_print_version(mi->ddr_size >> 8,
				mi->ddr_size & 0xff);
	}
#endif
	return 1;
}

void print_shmoo_error(memsys_error_t *e)
{
	unsigned int idx;

	puts("eMEMC: ");
	for (idx = 0; idx < MEMSYS_ERROR_MEMC_MAX_WORDS; idx++) {
		report_hex(" ", e->memc[idx]);
	}

	puts("ePHY:");
	for (idx = 0; idx < MEMSYS_ERROR_PHY_MAX_WORDS; idx++) {
		report_hex(" ", e->phy[idx]);
	}

	puts("eSHMOO: ");
	for (idx = 0; idx < MEMSYS_ERROR_SHMOO_MAX_WORDS; idx++) {
		report_hex(" ", e->shmoo[idx]);
	}
}

uint32_t memsys_load(void)
{
	uint32_t *dst = (uint32_t *)MEMSYS_SRAM_ADDR;
	memsys_version_t v;

	gmemsys = (const struct memsys_interface *)dst;

	if (gmemsys->signature != BOARD_MSYS_MAGIC)
		memsys_die(DIE_BAD_BOARD_MSYS_MAGIC, "bad ms");

	gloud_shmoo = false;
	gmemsys->setup();
	gmemsys->get_version(&v);
	print_shmoo_version(&v);

	mcb_table = gmemsys->shmoo_data;

	__puts("MCB: F");
	(mcb_table) ? puts("IX") : puts("LEX");

	return v.version;
}


/* ------------------------------------------------------------------------- */

void shmoo_load(void)
{
#ifndef SECURE_BOOT
	uint32_t *dst = (uint32_t *)SHMOO_SRAM_ADDR;
	struct memsys_info *mi;
	struct board_nvm_info *s = (struct board_nvm_info *)dst;

	/* if we are not using '-fixed -' DDR config then load
	all the SHMOO section data for our FLEXible MCB tables.
	*/
	if (!gmemsys->shmoo_data) {
		if (load_from_flash(dst, SHMOO_TEXT_OFFS, SHMOO_SIZE) < 0)
			sys_die(DIE_SHMOO_LOAD, "shmoo load");

		if (!s->shmoo_data)
			memsys_die(DIE_MISSING_SHMOO_DATA, "bad mi");

		/*	shmoo table = sram base + offset of the mcb table
			(0 based offset, see shmoo.lds)
		*/
		mcb_table = ADDROF(dst, s->shmoo_data);
	}
#endif
	/* Either memsys_load() set this up for FIXed, or we did it
	just now for !SECURE_BOOT & FLEXible MCB tables. */
	if (!is_valid_mcb(mcb_table))
		memsys_die(DIE_BAD_MCB_MAGIC, "mcb magic");

	if (mcb_table[1].ddr_clock != MCB_MAGIC1)
		memsys_die(DIE_BAD_MCB_TABLE, "bad table");

/* Prevent subtables for secure boot. */
#ifndef SECURE_BOOT
	/* custom MCB table is right after the shmoo table */
	mi = &mcb_table[2];

	while (mi->values)
		mi++;

	mi++; /* skip the end marker */
	mcb_subtable = mi;
	if (!is_valid_mcb(mcb_subtable))
#endif
		mcb_subtable = NULL; /* subtable for custom MCB is an option */

	if (QA_LOUD_SHMOO)
		set_loud_shmoo();
}


/* ------------------------------------------------------------------------- */
#if CFG_PM_S3

#if defined(BCHP_DDR34_PHY_CONTROL_REGS_A_0_REG_START) /* LPDDR4 */

static void warm_restart_channel(physaddr_t stdby_cntl)
{
	uint32_t tmp;

	/*
	 * PHY S3 Exit Sequence
	 * --------------------
	 * step 2: CONFIGURE RST_N / CKE levels.  Write these fields atomically
	 */

	/* CKE = 0, RST_N = 1 */
	tmp = BDEV_RD(stdby_cntl);
	tmp &= ~BCHP_DDR34_PHY_CONTROL_REGS_A_0_STANDBY_CONTROL_CKE_MASK;
	tmp |= BCHP_DDR34_PHY_CONTROL_REGS_A_0_STANDBY_CONTROL_RST_N_MASK;
	BDEV_WR(stdby_cntl, tmp);

	/* step 3: wait 10 us */
	udelay(10);
	BARRIER();

	/* step 4: FORCE RST_N / CKE levels. */
	tmp = BDEV_RD(stdby_cntl);
	tmp |=
	BCHP_DDR34_PHY_CONTROL_REGS_A_0_STANDBY_CONTROL_FORCE_CKE_RST_N_MASK;
	BDEV_WR(stdby_cntl, tmp);
	BARRIER();
}

static void warm_restart_memc(int memc)
{
	const struct memsys_params *p = &shmoo_params[memc];

	/*
	 * PHY S3 Exit Sequence
	 * --------------------
	 * step 1: Configure S3 powerdown exit PHY pin sequence
	 */
	BDEV_WR_F(SHIMPHY_ADDR_CNTL_0_DDR_PAD_CNTRL, S3_PWRDWN_SEQ, 3);
	BDEV_WR_F(DDR34_PHY_COMMON_REGS_0_PLL_CONFIG, ENABLE_EXT_CTRL, 1);
	BDEV_WR_F(SHIMPHY_ADDR_CNTL_0_DFI_CONTROL,
			PHY_PLL_CTRL_RDB_OVERRIDE, 0);
	BARRIER(); /* Fully complete before moving on. */

	warm_restart_channel(p->phy_reg_base + DDR_PHY_STANDBY_CONTROL_OFFS_A);
	warm_restart_channel(p->phy_reg_base + DDR_PHY_STANDBY_CONTROL_OFFS_B);
}

#else

static void warm_restart_memc(int memc)
{
	uint32_t tmp;
	physaddr_t stdby_cntl;
	const struct memsys_params *p = &shmoo_params[memc];

	stdby_cntl = p->phy_reg_base + DDR_PHY_STANDBY_CONTROL_OFFS;

	tmp = BDEV_RD(stdby_cntl);
	/* Keep CKE pin low */
	tmp &= ~BCHP_DDR34_PHY_CONTROL_REGS_0_STANDBY_CONTROL_CKE_MASK;
	/* Keep RST_N high */
	tmp |= BCHP_DDR34_PHY_CONTROL_REGS_0_STANDBY_CONTROL_RST_N_MASK;
	BDEV_WR(stdby_cntl, tmp);

	/* Delay 1000 us */
	sleep_ms(1);

	/* Force the RST_N / CKE levels */
	tmp = BDEV_RD(stdby_cntl);

	tmp |=
	BCHP_DDR34_PHY_CONTROL_REGS_0_STANDBY_CONTROL_FORCE_CKE_RST_N_MASK;

	BDEV_WR(stdby_cntl, tmp);
}

#endif

void memsys_warm_restart(int num_memc)
{
	int i;

	__puts("Resume DDR PHYs...");

	for (i = 0; i < num_memc; i++)
		warm_restart_memc(i);

	fsbl_resume_ddr_phys();

	puts("Done");
}
#endif /* CFG_PM_S3 */

void shmoo_set(const struct ddr_info *ddr, bool warm_boot)
{
	uint32_t *template, *dst = (uint32_t *)SHMOO_SRAM_ADDR;
	uint32_t bytes, *v, *scratch, count = 0, csum = 0;
	static uint32_t *v_saved = NULL;
	uint32_t options = 0;

	if (gmemsys->shmoo_data) {
		__puts("*");
		/* If we're using fixed full MCB tables, index
		each MCB by the ddr #num and modify the ddr params
		so that	the 'info' command can report the MCB
		params correctly, EXCEPT for ddr base & size as
		we cannot determine that from the MCB.

		 IMPORTANT: This does mean you must have *ALL* MCBs
		present	in the MEMSYS section for each ddr or you
		will by trying to SHMOO using junk data.

		 IMPORTANT: Check the MCB ordering in the file
		bolt/$(FAMILY)/memsys-fixed(-alt).c with respect
		to which DDR it is meant for. Ordering is by
		the 'ddr' command order in your $(FAMILY)
		config file. Acending order is the only one
		allowed in BOLT.

		 IMPORTANT: Sub-tables ('-custom' flag in 'ddr'
		 script command) are NOT ALLOWED in this configuration
		 as we don't search them.
		*/
		struct memsys_info *mi = &(mcb_table[2 + ddr->which]);
		struct ddr_info *d = (struct ddr_info *)ddr;

		d->ddr_clock = mi->ddr_clock;
		d->ddr_size  = mi->ddr_size;
		d->ddr_width = mi->ddr_width;
		d->phy_width = mi->phy_width;

		scratch = v_saved = (uint32_t *)mi->values;

		if (CFG_ZEUS4_1) {
			scratch = v_saved = (uint32_t *) (SRAM_ADDR |
					BOOT_PARAMETER_OFFSET);
		}

	} else { /* Flex MCB tables */

		if (ddr->tag == NULL || mcb_subtable == NULL) {
			v = find_shmoo(ddr, &mcb_table[2], NULL);
		} else {
			uint32_t *sram_offset = (uint32_t *)SHMOO_SRAM_ADDR;

			v = find_shmoo(ddr, &mcb_subtable[1], sram_offset);
		}

		/* We did not find any matching ddr params for flex so
		 we have to die now. */
		if (!v)
			sys_die(DIE_MCB_NOT_FOUND, "mcb not found");

		v = ADDROF(dst, v); /* adjust to sram offset */

		/* leap over loaded mcb data */
		scratch = ADDROF(dst, SHMOO_SIZE);

		if (v_saved != v) {
			v_saved = v;

			template = ADDROF(dst, mcb_table[1].values);

			bytes = mcb_table[1].ddr_size; /* SHMOO_ARRAY_SIZE */

			memcpy4(scratch, template, bytes);

			while (v[0] != (uint32_t)-1) {
				scratch[v[0]] = v[1];
				v += 2;
				count += 4;
				/* we went off the deep end */
				if (count > bytes)
					sys_die(DIE_PATCH_MCB, "patch");
			}

			for (count = 0; count < (bytes / sizeof(uint32_t));
				count++)
				csum += scratch[count];

			if (csum != v[1]) {
				writehex(csum);
				report_hex("!=", v[1]);
				sys_die(DIE_MCB_CHECKSUM, "checksum");
			}
			putchar('!');
		} else
			putchar('=');
	}

	report_hex("@@ ", (uint32_t)scratch);
	report_hex("@ <= ", (uint32_t)v_saved);

	if (!gloud_shmoo)
		options |= SHMOO_OPTION_DISABLE_CONSOLE;
	if (warm_boot)
		options |= SHMOO_OPTION_WARMBOOT;

#if !defined(CFG_FULL_EMULATION)
	do_shmoo(gmemsys, ddr, scratch, options);
#endif
}


/* ------------------------------------------------------------------------- */

struct ddr_info *shmoo_ddr_by_index(uint32_t d)
{
	static struct ddr_info ddr;

	d += 2; /* bypass header and template entries */
	ddr.ddr_clock = mcb_table[d].ddr_clock;
	ddr.ddr_size  = mcb_table[d].ddr_size;
	ddr.ddr_width = mcb_table[d].ddr_width;
	ddr.phy_width = mcb_table[d].phy_width;
	return &ddr;
}

void shmoo_menu(struct board_nvm_info *nvm)
{
#ifndef SECURE_BOOT
	int i, max = 'a';
	struct memsys_info *mi = mcb_table;

	crlf();
	mi += 2; /* bypass header and template entries */
	while (mi->values != NULL) {
		putchar(max);
		__puts(")\t");
		writeint(mi->ddr_clock);
		__puts("MHz\t");
		if (mi->ddr_size < 1024) {
			/* smaller than 1 Gbit */
			writeint(mi->ddr_size);
			__puts("Mx");
		} else {
			uint32_t ddr_size = mi->ddr_size / 1024;

			writeint(ddr_size);
			__puts("Gx");
			/* TODO: consider tera bits */
		}
		writeint(mi->ddr_width);
		putchar(' ');
		writeint(mi->phy_width);
		puts("b");
		max++;
		mi++;
	}

	if (gmemsys->shmoo_data)
		return;

	while (1) {
		__puts("\n0)\tundo override\n1)\texit\n2)\tdebug on\nshmoo: ");
		i = getchar();
		/* coverity[negative_returns] */
		putchar(i);
		crlf();

		if (i == '1')
			return;

		if (i == '2') {
			set_loud_shmoo();
			continue;
		}

		if ((i >= 'a' && i < max) || (i == '0'))
			break;
	}

	nvm->ddr_override = (i == '0') ? (int)DDR_OVERRIDE_NO : i - 'a';
	nvm->board_idx = CFG_BOARDDEFAULT-1; /* force SSBL to write nvm info */
#endif
}

#endif	/* CFG_EMULATION || CFG_NOSHMOO */
