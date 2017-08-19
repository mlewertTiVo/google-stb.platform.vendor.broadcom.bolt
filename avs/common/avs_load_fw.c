/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "../fsbl/fsbl.h" /* load_from_flash & docsis_booted */
#include "../fsbl/fsbl-clock.h"
#include "../fsbl/fsbl-pm.h"
#include "../fsbl/fsbl-emmc.h"

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_uarta.h"
#include "bchp_timer.h"

#include "bchp_aon_ctrl.h"
#include "bchp_avs_cpu_ctrl.h"
#include "bchp_avs_cpu_prog_mem.h"
#include "bchp_avs_cpu_data_mem.h"
#include "bchp_avs_top_ctrl.h"

#include "avs.h"
#include "avs_fw_interface.h"
#include "avs_temp_reset.h"

#include <aon_defs.h>
#include "boot_defines.h"

/* These are build options -- enable define to enable the option */

#ifndef AVS_DEBUG_STARTUP
#define AVS_DEBUG_STARTUP	0
#endif

#ifndef ENABLE_DEBUG_PRINTS
#define ENABLE_DEBUG_PRINTS	1
#endif

#ifndef CHECK_AVS_RESET
/* enable this to enable debug code to check whether
 * AVS caused last reset */
#define CHECK_AVS_RESET		0
#endif

#ifndef ENABLE_TEST_PROMPTS
/* These let me dynamically over-ride some run-time parameters. */
#define ENABLE_TEST_PROMPTS	0
#endif

#ifndef TIME_PROCESS
/* count (and display) the time it takes to process the AVS */
#define TIME_PROCESS		0
#endif

#if AVS_DEBUG_STARTUP
#define ENABLE_DEBUG_PRINTS	1 /* this has to be set to print */
#endif

#define avs_sleep(microsec) udelay(microsec)

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#if (ENABLE_DEBUG_PRINTS)
#define avs_print_char(c) putchar(c)
#define avs_print_val(v) writehex(v)
#define avs_print_cr_lf() puts("");
#define avs_print_string(s) __puts(s)
#else
#define avs_print_char(x) do {} while (0)
#define avs_print_val(x)  do {} while (0)
#define avs_print_cr_lf() do {} while (0)
#define avs_print_string(s) do {} while (0)
#endif

#define avs_print_decimal(n) writeint(n)
#define avs_return_char() (char)getchar()

#define avs_print_string_val(s, x) \
	do { \
		avs_print_string(s); \
		avs_print_val(x); \
	} while (0)

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* this is debug code and will be optimized out when debug not enabled */
/* returns false if they do NOT want the default */
static bool ask_y_or_n(const char *prompt, bool yes)
{
	char c;
	avs_print_string((char *)prompt);
	if (yes)
		avs_print_string(" <Y/n>? ");
	else
		avs_print_string(" <y/N>? ");
	c = avs_return_char();
	avs_print_char(c);
	avs_print_cr_lf();
	if (yes)
		return c != 'N' && c != 'n';
	else
		return c != 'Y' && c != 'y';
}

/* this is debug code and will be optimized out when debug not enabled */
static int avs_get_int_number(char *prompt, int the_default)
{
	char c;
	int input;
	bool got_something, negative;

	while (0) {
 retry:
		avs_print_cr_lf();
	}
	input = 0;
	got_something = false;
	negative = false;
	if (prompt)
		avs_print_string(prompt);
	avs_print_char('(');
	avs_print_decimal(the_default);
	avs_print_char(')');
	avs_print_char('?');
	avs_print_char(' ');

	for (; (c = avs_return_char()) != '\r';) {
		if (!c)
			goto use_default;
		if (c == 27)
			goto retry;
		avs_print_char(c);
		if (c == '-') {
			negative = true;
			continue;
		}
		if (!(c >= '0' && c <= '9'))
			goto retry;
		input = (input * 10) + (c - '0');
		got_something = true;
	}
	avs_print_cr_lf();
	if (!got_something)
		goto use_default;
	if (negative)
		input *= -1;
	while (0) {
 use_default:
		input = the_default;
	}

	/* show them what we're using in case they picked the default and don't
	 * know what the default is */
	/*
	if (prompt)
		avs_print_string(prompt);
	avs_print_char('=');
	avs_print_decimal(input);
	avs_print_cr_lf();
	*/
	return input;
}

/* this is debug code and will be optimized out when debug not enabled */
static void avs_ask_margins(struct at_initialization *params)
{
	if (ask_y_or_n("Use modified margins", false))
		return;

	params->margin_low =
	    avs_get_int_number("Vmargin Low", params->margin_low);
	params->margin_high =
	    avs_get_int_number("Vmargin High", params->margin_high);
#ifdef DVFS_SUPPORT
	params->cpu_offset =
	    avs_get_int_number("CPU voltage offset", params->cpu_offset);
#endif
}

/* this is debug code and will be optimized out when debug not enabled */
static void avs_ask_limits(struct at_initialization *params)
{
	if (ask_y_or_n("Use modified limits", false))
		return;

	params->min_voltage =
	    avs_get_int_number("Minimum voltage", params->min_voltage);
	params->max_voltage =
	    avs_get_int_number("Maximum voltage", params->max_voltage);
#ifdef DVFS_SUPPORT
	params->min_dvfs_voltage =
	    avs_get_int_number("Minimum DVFS voltage",
				params->min_dvfs_voltage);
	params->slope_adj_factor =
	    avs_get_int_number("Slope adj factor (125=1.25)",
				params->slope_adj_factor);
#endif
}

#if (BCHP_CHIP==7445) || defined(CONFIG_BCM7439B0)
/* specials: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=50mV VmarginH=100mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  50     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 100    /* default high margin in mV */

#elif (BCHP_CHIP == 7260) /* support moved to AVS firmware */
/* dual core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=20mV VmarginH=20mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  120    /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 120    /* default high margin in mV */
#define DEFAULT_CPU_OFFSET   40     /* default stb to cpu margin in mV*/

#elif (BCHP_CHIP == 7268) /* only used for A0 parts */
/* dual core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=20mV VmarginH=20mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  20     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 20     /* default high margin in mV */
#define DEFAULT_CPU_OFFSET   150    /* default stb to cpu margin in mV*/

#elif (BCHP_CHIP == 7271) /* only used for A0 parts */
/* dual core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=20mV VmarginH=20mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  20     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 20     /* default high margin in mV */
#define DEFAULT_CPU_OFFSET   150    /* default stb to cpu margin in mV*/

#elif (BCHP_CHIP == 7278)
/* dual core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=20mV VmarginH=20mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  60     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 60     /* default high margin in mV */
#define DEFAULT_CPU_OFFSET   80     /* default stb to cpu margin in mV*/

#else

#ifdef DVFS_SUPPORT
/* dual core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=20mV VmarginH=20mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  20     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 20     /* default high margin in mV */
#define DEFAULT_CPU_OFFSET   150    /* default stb to cpu margin in mV*/
#else
/* single core: Vmin_avs=0.86V Vmax_avs=1.035V VmarginL=30mV VmarginH=50mV */
#define DEFAULT_MIN_VOLTAGE  860    /* default minimum voltage in mV */
#define DEFAULT_MAX_VOLTAGE  1035   /* default maximum voltage in mV */
#define DEFAULT_VMARGIN_LOW  30     /* default low margin in mV */
#define DEFAULT_VMARGIN_HIGH 50     /* default high margin in mV */
#endif /* DVFS_SUPPORT */

#endif

#define DEFAULT_POLLING_DELAY 1000	/* once a second */
#define DEFAULT_EXTRA_DELAY  0	/* extra delay needed by this voltage regulator
				 * to become stable */

/* These setup the defaults to be over-ridden before calling the
 * avs_start_firmware function */
static void avs_get_default_params(struct at_initialization *params)
{
	memset(params, 0, sizeof(*params));

	/* These are the startup parameters --
	 * some of these need to come from somewhere else */
	params->margin_low  = DEFAULT_VMARGIN_LOW;
	params->margin_high = DEFAULT_VMARGIN_HIGH;
	params->min_voltage = DEFAULT_MIN_VOLTAGE;
	params->max_voltage = DEFAULT_MAX_VOLTAGE;
	params->extra_delay = DEFAULT_EXTRA_DELAY;
	params->polling_delay = DEFAULT_POLLING_DELAY;

#ifdef DVFS_SUPPORT
#if defined(CONFIG_BCM7260A0)
#define DEFAULT_DVFS_VOLTAGE 900
#define DEFAULT_SCALING_FACTOR 150 /* 1.5 times */
#else
#define DEFAULT_DVFS_VOLTAGE 860
#define DEFAULT_SCALING_FACTOR 150 /* 1.5 times */
#endif
	params->cpu_offset = DEFAULT_CPU_OFFSET;
	params->min_dvfs_voltage = DEFAULT_DVFS_VOLTAGE;
	params->slope_adj_factor = DEFAULT_SCALING_FACTOR;
#endif

	/* Let firmware know if this is a warm boot due to S3 resume */
	if (fsbl_ack_warm_boot()) {
		puts("AVS resuming S3...\n");
		params->flags_2 |= AVS_FLAGS_2_RESUME_S3;
	}

	/* The firmware needs to know if this reset was due to an
	 * over-temperature situation. */
	if (avs_check_temp_reset(true)) {
		puts("AVS over-temp start...\n");
		params->flags_1 |= AVS_FLAGS_1_OVER_TEMP;
	}

	/* tell the firmware what part we're running on */
	params->chip_id = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
	params->product_id = BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID);

#ifndef DVFS_SUPPORT
	/* Special case: some parts run at 1.7GHz and need extra margin.
	 * Use this for anything greater than 1.503GHz */
	if (get_cpu_freq_mhz() > 1503)
	{
		params->margin_low  = 50; /* extra low margin of 50mV */
		params->margin_high = 100; /* extra high margin of 100mV */
	}
#endif

#if defined(AVS_DUAL_DOMAINS)
	/* set for boards with single regulator */
	struct board_type *b = get_tmp_board();

/* single voltage domain margin correction in mV */
#define SINGLE_DOMAIN_MARGIN_CORRECTION 120

	if (!b || AVS_DOMAINS(b->avs) == 1) {
		unsigned correction = SINGLE_DOMAIN_MARGIN_CORRECTION;
		if (ENABLE_TEST_PROMPTS) {
			correction = avs_get_int_number("Vmargin Low correction", correction);
		}
		params->single_domain = true;
		params->margin_low  = correction;
		params->margin_high = correction;
	}
#endif

	/* Special case for dongle version of the 7250 (72502) */
	/* Dongle version uses a smaller set of margin values */
	if ((params->product_id & 0x0FFFFF00) == 0x07250200) {
		params->margin_low  = 5;
		params->margin_high = 25;
	}

	if (ENABLE_TEST_PROMPTS) {
		avs_ask_margins(params);
		avs_ask_limits(params);
	}
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#define START_MAX_COUNT 5000 /* max # of loops we'll wait for AVS to start */
static bool did_we_resume;


/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#define AVS_RESERVED_SPACE 256 /* this area is reserved for over-temp restore */
#define AVS_PARAM_SIZE     0x400+0x100 /* There is 0x100 bytes of padding */

static int load_code(void)
{
	uint32_t flash_offs = AVS_TEXT_OFFS; /* this is where the firmware
					      * lives in the flash */
	unsigned bytes;
	int code_length = (BCHP_AVS_CPU_PROG_MEM_WORDi_ARRAY_END + 1) *
		sizeof(uint32_t); /* in bytes */
	int data_length = (BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_END + 1) *
		sizeof(uint32_t) - AVS_RESERVED_SPACE; /* in bytes */
	image_info info;
	int bypass_emmc_data_part = 1;
#if (AVS_USE_EMMC_DATA_PART == 1)
	uint32_t aon_reg;
#endif

#define round_up(x) (((x)+3)/4*4)

	flash_offs = sec_avs_select_image();
	avs_print_string("AVS load_code offset =");
	avs_print_val(flash_offs);
	avs_print_string("\n");

	if (get_image_info(&info) < 0)
		sys_die(DIE_NO_AVS_IMAGE_INFO, "no AVS image info");

	bytes = round_up(code_length);
	if (AVS_DEBUG_STARTUP)
		avs_print_string("Loading code...\n");

	/* Check if we got here due to booting error from eMMC data partition.
	 * If we got here because booting from data partition failed,
	 * then clear the flag and bypass booting from eMMC data partition.
	 * If we are booting from eMMC data partition for the first time
	 * just set the eMMC data partition flag and use eMMC data partition.
	 */
#if (AVS_USE_EMMC_DATA_PART == 1)
	aon_reg = BDEV_RD(BCHP_AON_CTRL_UNCLEARED_SCRATCH);
	aon_reg |= AON_EMMC_DATA_PART_BOOT_AVS_BYPASS;
	if (!(aon_reg & AON_EMMC_DATA_PART_BOOT_AVS)) {
		bypass_emmc_data_part = 0;
		aon_reg &= ~AON_EMMC_DATA_PART_BOOT_AVS_BYPASS;
		aon_reg |= AON_EMMC_DATA_PART_BOOT_AVS;
	}
	BDEV_WR(BCHP_AON_CTRL_UNCLEARED_SCRATCH, aon_reg);
#endif

	if (bypass_emmc_data_part != 1) {
		/* read the code, data and security params to the buffer */
		if (emmc_read_fsbl(AVS_SRAM_ADDR,
			bytes+round_up(data_length)+AVS_PARAM_SIZE,
			(flash_offs+info.flash.part_offs), false) != 0)
			sys_die(DIE_AVS_CODE_LOAD_FROM_EMMC_FAILED,
				"AVS code load from eMMC failed");
		/* copy the code to AVS PROG_ARRAY */
		memcpy(((void*)BCHP_PHYSICAL_OFFSET+
			BCHP_AVS_CPU_PROG_MEM_WORDi_ARRAY_BASE),
			(void*)MEMSYS_SRAM_ADDR, bytes);
	}
	else {
		if (load_from_flash_ext((uint32_t *)(BCHP_PHYSICAL_OFFSET+
			BCHP_AVS_CPU_PROG_MEM_WORDi_ARRAY_BASE), flash_offs,
			bytes, &info.flash) < 0)
			sys_die(DIE_AVS_CODE_LOAD_FAILED,
				"AVS code load failed");
	}

	flash_offs += bytes;

	bytes = round_up(data_length);
	if (AVS_DEBUG_STARTUP)
		avs_print_string("Loading data...\n");
	if (bypass_emmc_data_part != 1) {
		/* copy the data to AVS DATA_ARRAY */
		memcpy(((void*)BCHP_PHYSICAL_OFFSET+
			BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE),
			(void*)(MEMSYS_SRAM_ADDR+round_up(code_length)), bytes);
	}
	else {
		if (load_from_flash_ext(
			(uint32_t *)(BCHP_PHYSICAL_OFFSET+
			BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE), flash_offs,
			bytes, &info.flash) < 0)
			sys_die(DIE_AVS_DATA_LOAD_FAILED,
				"AVS data load failed");
	}

	return AVS_LOADED;
}

/* This loads the firmware (code and data) into the AVS CPU memory */
static int avs_load_firmware(void)
{
	/* We may need to reset it just in case it was in hung status */
	BDEV_WR_F(AVS_CPU_CTRL_CTRL, AVS_RST, 1);

	/* The ARC CPU needs time (200ns) to complete the reset
	 * before it can be accessed again */
	avs_sleep(1);

	/* Initialize the processor (setting this lets us read the core
	 * registers from the host side) */
	BDEV_WR_F(AVS_CPU_CTRL_CTRL, HOSTIF_SEL, 1);

	return load_code(); /* load the code into the AVS SRAM memoory */
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This passes the initialization parameters to the firmware */
/* This can either be done at firmware load or before start */
static void load_init_parameters(struct at_initialization *params)
{
	unsigned i;
	uint32_t *p = (uint32_t *)params;

	for (i = 0; i < sizeof(*params) / sizeof(p); i++)
		BDEV_WR(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE + (i * 4),
			p[i]);
}

/* The AVS CPU needs to be started after its loaded */
static void avs_start_firmware(struct at_initialization *params)
{
	avs_sleep(1000);

	if (AVS_DEBUG_STARTUP)
		avs_print_string("Starting FW...\n");

	load_init_parameters(params);	/* set the startup parameters needed by
					 * fimrware */

	/* Start the processor (starts on 0->1 transition) */
	BDEV_WR(BCHP_AVS_TOP_CTRL_START_AVS_CPU, 0);
	BDEV_WR(BCHP_AVS_TOP_CTRL_START_AVS_CPU, 1);
	BDEV_WR(BCHP_AVS_TOP_CTRL_START_AVS_CPU, 0);

	if (AVS_DEBUG_STARTUP)
		avs_print_string("Started...\n");
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

static void avs_print_this(const char *header, unsigned value)
{
	avs_print_string((char *)header);
	avs_print_val(value);
	avs_print_char('(');
	avs_print_decimal(value);
	avs_print_char(')');
	avs_print_cr_lf();
}

/* TODO: these are now available all the time so we could print these later in
 * SSBL */
static void avs_dump_results(void)
{
	struct at_runtime results;
	uint32_t *p = (uint32_t *)&results;
	unsigned i, revision;

	for (i = 0; i < sizeof(results) / sizeof(*p); i++)
		p[i] =
		    BDEV_RD(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE +
				(i * 4));

	revision = results.revision;
	avs_print_string_val("status=", results.status);
	avs_print_cr_lf();

	avs_print_this("STB: Current voltage=", results.voltage0);
	avs_print_this(" temperature=", results.temperature0);
	avs_print_this(" PV=", results.PV0);
	avs_print_this(" MV=", results.MV0);
#if defined(AVS_DUAL_MONITORS) || defined(AVS_DUAL_DOMAINS)
	struct board_type *b = get_tmp_board();

	if (!b || AVS_DOMAINS(b->avs) == 2) {
		avs_print_this("CPU: Current voltage=", results.voltage1);
		avs_print_this(" temperature=", results.temperature1);
		avs_print_this(" PV=", results.PV1);
		avs_print_this(" MV=", results.MV1);
	}
#endif

	avs_print_string_val("AVS FW rev=", revision);
	avs_print_string(" [");
	avs_print_char(revision >> 24 & 0xFF);
	avs_print_char('.');
	avs_print_char(revision >> 16 & 0xFF);
	avs_print_char('.');
	avs_print_char(revision >> 8 & 0xFF);
	avs_print_char('.');
	avs_print_char(revision >> 0 & 0xFF);
	avs_print_string("]");
	avs_print_cr_lf();
}

/* Firmware will signal when its running */
static int avs_wait_for_firmware(int en)
{
	unsigned i;
	int complete = AVS_TIMEOUT;
	uint32_t data;

	for (i = 0; i < START_MAX_COUNT; i++) {
		data = BDEV_RD(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE + 4);
			/* this is where FW places status information */
		if (data == INIT_COMPLETE) {
			complete = AVS_SUCCESS;
			break;
		}
		if (data == INIT_FAILED) {
			complete = AVS_FAILURE;
			break;
		}
		if (data == AVS_NOT_FOUND || data == CHIP_MISMATCH) {
			complete = data;
			break;
		}
		avs_sleep(1000);
	}
	if (complete == AVS_TIMEOUT) {
		/* note: data should be last step completed */
		avs_print_string("AVS FW timed out waiting to Start! (data=");
		avs_print_val(data);
		avs_print_string(")\n");
	} else if (complete == AVS_FAILURE) {
		/* note: data should be last step completed */
		avs_print_string("AVS FW Failed to Start! (data=");
		avs_print_val(data);
		avs_print_string(")\n");
	} else {
		if (AVS_DEBUG_STARTUP)
			avs_print_string("AVS FW Running!\n");
		if (en)
			avs_dump_results();
	}

	return complete;	/* SUCCESS means its started successfully */
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* AVS firmware will perform a convergence process that changes the voltage.
 * If this gets changed too far it could cause the part to reset (VTRAP) or
 * just crash. We need to know if AVS was the reason we just rebooted and,
 * if so, either try an alternate firmware or don't try AVS on this pass.
 * The process is to set a flag, that won't be cleared by the reset, BEFORE we
 * load AVS firmware (that changes the voltage). Then, process the firmware
 * as usual.  If we get back here with that flag still set then WE caused the
 * reset. */
#define AVS_AON_MAGIC 0x99beef88	/* this is the bread-crumb we'll leave
					 * to see if we finished */
#define AVS_MAILBOX(a)	(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE + 4*(a))
#define RD_AVS_MAILBOX(a)		BDEV_RD(AVS_MAILBOX(a))
#define WR_AVS_MAILBOX(a, v)		BDEV_WR(AVS_MAILBOX(a), (v))
#define MAGIC_FLAG_MAILBOX		22

static void setup_reset_flag(void)
{
	WR_AVS_MAILBOX(MAGIC_FLAG_MAILBOX, AVS_AON_MAGIC);
}

static void clear_reset_flag(void)
{
	WR_AVS_MAILBOX(MAGIC_FLAG_MAILBOX, 0);
}

static uint32_t read_reset_flag(void)
{
	return RD_AVS_MAILBOX(MAGIC_FLAG_MAILBOX);
}

static bool did_I_cause_the_last_reset(void)
{
	bool result = false;
	/* uint32_t aon_reset_history = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY); */
	/* avs_print_string_val("Last reset was: reset_history=",
	 *	aon_reset_history);
	 avs_print_cr_lf(); */

#if 0
	/* We can qualify the reason for the reset here... */
	/* For example: Are ANY of the voltage resets flagged as a reason for
	 * the reset? */
	if (aon_reset_history &
	    (BCHP_AON_CTRL_RESET_HISTORY_overvoltage_1_reset_MASK |
	     BCHP_AON_CTRL_RESET_HISTORY_undervoltage_1_reset_MASK |
	     BCHP_AON_CTRL_RESET_HISTORY_undervoltage_0_reset_MASK |
#ifdef AVS_DUAL_MONITORS
	     BCHP_AON_CTRL_RESET_HISTORY_cm_overvoltage_1_reset_MASK |
	     BCHP_AON_CTRL_RESET_HISTORY_cm_undervoltage_1_reset_MASK |
	     BCHP_AON_CTRL_RESET_HISTORY_cm_undervoltage_0_reset_MASK |
#endif
	     0))
#endif
	{
		/* avs_print_string_val
		("Last reset was due to voltage over/under run! reset_history=",
		aon_reset_history);
		avs_print_cr_lf(); */

		/* If the AVS_AON_MAGIC is still there then the convergence
		 * process CAUSED the reset! */
		if (read_reset_flag() == AVS_AON_MAGIC) {
			avs_print_string
		("AVS caused the last reset -- not loading AVS firmware!\n");
			result = true;
		}
	}
	clear_reset_flag();
	return result;
}

static void __maybe_unused avs_time_process_report(char *msg, unsigned was)
{
	unsigned time = get_time_diff(was);
	avs_print_string_val(msg, time);
	avs_print_string("(");
	avs_print_decimal(time);
	avs_print_string(")\n");
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#if defined(AVS_DUAL_DOMAINS)
static void setup_avs_params(int en, int pmap_id)
{
	/* set for boards with single regulator */
	struct board_type __maybe_unused *b = get_tmp_board();
	unsigned long param0 = 0;
	unsigned long param1 = 0;

#ifdef PARAM_AVS_PARAM_0
	/* read run time AVS params from secure */
	/* parameter section, if they exist.    */
	param0 = DEV_RD(SRAM_ADDR + PARAM_AVS_PARAM_0);
	param1 = DEV_RD(SRAM_ADDR + PARAM_AVS_PARAM_1);
#endif

#define DEFAULT_PSTATE 4 /* 0 starts up at fastest state, 4 at slowest */

#if !defined(SECURE_BOOT)
	if (!b || AVS_DOMAINS(b->avs) == 1)
		param1 = 0x00000100;
	else
		param1 = 0x00000200;

	param1 |= (pmap_id & 0x1F);
	if (en) /* if fw not enabled, default to pstate 0 */
		param1 |= (DEFAULT_PSTATE & 0x7) << 5;
#endif

	/* pass AVS_PARAM_0 and AVS_PARAM_1 to AVS processor */
	BDEV_WR(BCHP_AVS_TOP_CTRL_SPARE_LOW, param0);
	BDEV_WR(BCHP_AVS_TOP_CTRL_SPARE_HIGH, param1);
}
#endif

/* These are the public functions that do EVERYTHING to setup the AVS firmware.
 */
/* Once security is enabled we'll need to break these steps up in order to
 * verify the firware after its been loaded but before its started. */
/* TODO: Once this happens we should delete these entry points in favor of the
 * individual steps. */

int avs_common_load(void)
{
	unsigned __maybe_unused time;
	int result = AVS_FAILURE;

	if (avs_check_disabled())
		return AVS_DISABLED;

	if (ENABLE_TEST_PROMPTS) {
		if (!ask_y_or_n("Load AVS firmware", true))
			return AVS_DISABLED;
	}

	/* The UART needs to be setup in order to map it to a particular port
	 * -- do that now */
	/* This is for debug only in that this port can be used by other
	 * hardward blocks */
	/* A BBS script is available to do this same thing */
	setup_chip_for_avs_uart();

	/* If AVS was the reason for the last reset then don't load AVS
	 * firmware this time */
	if (CHECK_AVS_RESET && did_I_cause_the_last_reset())
		return result;

	/* avs_sleep(1*1000*1000); */
	if (AVS_DEBUG_STARTUP)
		avs_print_string("Load...\n");

	if (TIME_PROCESS)
		time = get_time_diff(0);

	/* If we failed to resume OR we're cold booting, then load firmware */
	if (result != AVS_SUCCESS) {
		result = avs_load_firmware();

		if (CHECK_AVS_RESET)
			setup_reset_flag();

		if (sec_verify_avs())
			result = AVS_VERIFY_FAILED;
	}

	if (TIME_PROCESS)
		avs_time_process_report("load time=", time);

	return result;
}

int avs_common_start(int en, int pmap_id)
{
	unsigned __maybe_unused time;
	struct at_initialization params;
	int result = AVS_FAILURE;

	/* If we resumed operation instead of reloading new code
	 * then we don't need to start and wait -- its already running!
	 */
	if (did_we_resume)
		return AVS_SUCCESS;

	if (ENABLE_TEST_PROMPTS) {
		if (!ask_y_or_n("Start AVS firmware", true))
			return AVS_DISABLED;
	}

	if (AVS_DEBUG_STARTUP)
		avs_print_string("Start...\n");

	avs_get_default_params(&params);

	if (CHECK_AVS_RESET)
		setup_reset_flag();

	if (TIME_PROCESS)
		time = get_time_diff(0);

#if defined(AVS_DUAL_DOMAINS)
	setup_avs_params(en, pmap_id);
#endif

	/* If they don't want firmware to run then just tell FW not to run */
	if (!en)
		params.flags_1 |= AVS_FLAGS_1_NO_CONVERGE;

	/* But we ALWAYS start it now */
	avs_start_firmware(&params);

	result = avs_wait_for_firmware(en);

	/* enable this to stop everything after AVS starts */
	if (1 && ENABLE_TEST_PROMPTS)
		ask_y_or_n("Continue", true);

	if (TIME_PROCESS)
		avs_time_process_report("start time=", time);

	if (CHECK_AVS_RESET)
		clear_reset_flag();

	return result;
}

/* This is used to wake-up the AVS CPU after a resume from S3 power mode */
void avs_warm_start(void)
{
	/* TBD */
}

/* This is a public function that can be used to retreive the version of the
 * AVS firmware.
 * Returns the AVS firmware revision in the form: MMmmeexx where
 * MM=Major,mm=minor,ee=edit,xx=test
 * So result of 0x30313178 = '0','1','1','x' or 0.1.1.x (note: only display the
 * test if not zero) */
unsigned int avs_firmware_revision(void)
{
	struct at_runtime results;
	uint32_t *p = (uint32_t *)&results;
	unsigned i;
	for (i = 0; i < sizeof(results) / sizeof(p); i++)
		p[i] =
		    BDEV_RD(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE +
				(i * 4));
	return results.revision;
}

/* This is a public function to verify whether AVS is enabled in OTP */
int avs_check_disabled(void)
{
	/* If AVS is disabled then we can't access any of the AVS registers --
	 * just leave! */
	/* Note: on 7445/A0&B0 parts, the AVS was disabled by default in OTP */
#ifdef AVS_DUAL_MONITORS
	uint32_t reg =
	    BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS_STB) &
	    BCHP_AVS_TOP_CTRL_OTP_STATUS_STB_OTP_AVS_DISABLE_MASK;
#else
	uint32_t reg =
	    BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS) &
	    BCHP_AVS_TOP_CTRL_OTP_STATUS_OTP_AVS_DISABLE_MASK;
#endif
	return !!reg;
}

