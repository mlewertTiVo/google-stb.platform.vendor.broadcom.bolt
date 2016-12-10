/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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
#include "timer.h"
#include "devfuncs.h"

#include "env_subr.h"
#include "ui_command.h"
#include "ui_init.h"

#include "bolt.h"
#include "board.h"
#include "splash-api.h"
#include "ioctl.h"
#include "reboot.h"

#include "initdata.h"
#include "bsp_config.h"
#include "fsbl-common.h"
#include "ssbl-common.h"
#include "devtree.h"
#include "gisb.h"
#include "overtemp.h"

#include "bchp_sun_top_ctrl.h"
#include "chipid.h"
#include "custom_code.h"

#include "common.h"
#include "ssbl-sec.h"
#include "avs_bsu.h" /* for avs_info_print() */

#define WANT_OTP_DECODE 1 /* SWBOLT-263 */
#include "otp_status.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */
#define CMDLINE_BUF_SIZE	1024
/* "Stringification" of a macro! */
#define M_STR1(x)	(#x)
#define M_STR(x)	M_STR1(x)

static const char * const logo[] = {
	"    ,/   ",
	"  ,'/___,",
	".'__  ,' ",
	"   /,'   ",
	"  /'     ",
};

/*  *********************************************************************
    *  Externs
    ********************************************************************* */
extern unsigned int _ftext, _etext, _fdata, _edata, _fbss, end, _end;

#ifdef S_UNITTEST
void bolt_aegis_cr_unblock(void);
void bolt_aegis_flash_update_done(void);
void bolt_aegis_loop(void);
#endif


/*  *********************************************************************
    *  Globals
    ********************************************************************* */

unsigned int bolt_startflags;


/*  *********************************************************************
    *  bolt_setup_saved_env()
    *
    *  Initialize the environment for BOLT. These are all
    *  the system variables that *do* get stored in the NVRAM
    *  if they don't already exist.
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
static void bolt_setup_saved_env(void)
{
	char buffer[80];
	int save = 0;

	if (!env_getenv("BOARD_SERIAL")) {
		uint32_t rnd[5];

		if (!sec_get_random_num(rnd, 3)) {
			xsprintf(buffer, "%08x%08x", rnd[0], 
				~lib_crc32((void *)rnd, sizeof(rnd)));
			env_setenv("BOARD_SERIAL", buffer, ENV_FLG_NORMAL);
			save++;
		} else
			warn_msg("cannot set BOARD_SERIAL");

	}

	if (save)
		env_save();
}


/*  *********************************************************************
    *  bolt_setup_default_env()
    *
    *  Initialize the default environment for BOLT.  These are all
    *  the temporary variables that do not get stored in the NVRAM
    *  but are available to other programs and command-line macros.
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void bolt_setup_default_env(void)
{
	char buffer[80];
	static char id_str[20];
	uint32_t chipid;
	const enet_params *e;
	unsigned int i;

	xsprintf(buffer, "%d.%02d", BOLT_VER_MAJOR, BOLT_VER_MINOR);
	env_setenv("VERSION", buffer, ENV_FLG_BUILTIN | ENV_FLG_READONLY);

	xsprintf(buffer, "%s", buildtag);
	env_setenv("BUILDTAG", buffer, ENV_FLG_BUILTIN | ENV_FLG_READONLY);

	chipid = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
	sprintf(id_str, "%X", chipid);
	env_setenv("CHIP_FAMILY_ID", id_str,
		ENV_FLG_BUILTIN | ENV_FLG_READONLY);

	env_setenv("BOARDNAME", board_name(),
		ENV_FLG_BUILTIN | ENV_FLG_READONLY);

	/* if we have a config put it there, but respect if the
	 user has one already setup.
	 */
	for (i = 0; i < board_num_enet(); i++) {
		e = board_enet(i);
		if (!e || e->genet == -1)
			continue;

		xsprintf(buffer, "ETH%d_PHY", e->genet);
		if ((!env_getenv(buffer)) && (e->phy_type))
			env_setenv(buffer, e->phy_type, ENV_FLG_BUILTIN);

		xsprintf(buffer, "ETH%d_MDIO_MODE", e->genet);
		if ((!env_getenv(buffer)) && (e->mdio_mode))
			env_setenv(buffer, e->mdio_mode, ENV_FLG_BUILTIN);

		xsprintf(buffer, "ETH%d_SPEED", e->genet);
		if ((!env_getenv(buffer)) && (e->phy_speed))
			env_setenv(buffer, e->phy_speed, ENV_FLG_BUILTIN);

		xsprintf(buffer, "ETH%d_PHYADDR", e->genet);
		if ((!env_getenv(buffer)) && (e->phy_id))
			env_setenv(buffer, e->phy_id, ENV_FLG_BUILTIN);
	}

	xsprintf(buffer, "%ld", mem_totalsize);
	env_setenv("MEMORYSIZE", buffer,
		ENV_FLG_BUILTIN | ENV_FLG_READONLY);
}


static char *build_stat(void)
{
	if (strstr(buildtag, "dirty"))
		return "UNCONTROLLED";
	return "LOCAL BUILD";
}


static void print_otp(void)
{
#if WANT_OTP_DECODE
	uint32_t i = 0, r = 0, v = 0;

	while (g_otp_status[i].reg != 0) {
		if (g_otp_status[i].reg != r) {
			r = g_otp_status[i].reg;
			v = BDEV_RD(r);
			xprintf("\notp @ 0x%08x = 0x%08x: ",
				BPHYSADDR(r), v);
		}
		if (v & g_otp_status[i].mask)
			xprintf("%s(0x%08x) ",
				g_otp_status[i].name, g_otp_status[i].mask);
		i++;
	}
	xprintf("\n");
#else
	xprintf("otp=%08x/%08x\n",
		REG(BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0),
		REG(BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_1));
#endif
}

static uint32_t chip_id_without_rev(const uint32_t chip_id)
{
	/* 4-digit part numbers look like: 0x7445_0000
	 * 5-digit part numbers look like: 0x0743_7100
	 */
	return chip_id & 0xf0000000 ? chip_id >> 16 : chip_id >> 8;
}

static void say_hello(int blink)
{
	uint32_t prod, fam;
	unsigned int i;
	struct board_type *b = board_thisboard();

	xprintf("\n");
	for (i = 0; i < ARRAY_SIZE(logo); i++) {
		xprintf("%s%s%s%s ", ANSI_YELLOW, blink ? ANSI_BLINK : "",
				logo[i], ANSI_RESET);
		if (i == 1)
			xprintf("BOLT v%d.%02d %s %s\n",
				BOLT_VER_MAJOR, BOLT_VER_MINOR,
				buildtag, build_stat());
		else if (i == 2)
			xprintf("(%s %s)\n", builddate, builduser);
		else if (i == 3)
			xprintf("Copyright (C) %d Broadcom\n",
					buildyear);
		else
			xprintf("\n");
	}
	xprintf(ANSI_RESET "\n");

	xprintf("Board: %s\n", board_name());

	prod = REG(BCHP_SUN_TOP_CTRL_PRODUCT_ID);
	fam = REG(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
	xprintf("SYS_CTRL: product=%04x, family=%04x%02x, strap=%08x, ",
		chip_id_without_rev(prod),
		chip_id_without_rev(fam),
		(fam & CHIPID_REV_MASK) + 0xa0,
		REG(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0));

	print_otp();

	/* SWBOLT-99
	*/
#ifndef SECURE_BOOT
	xprintf("RESET CAUSE: ");
#endif
	aon_reset_history();

	if (b) {
		const uint32_t prid_board = b->prid & ~CHIPID_REV_MASK;
		const uint32_t prid_reg = prod & ~CHIPID_REV_MASK;

		if (prid_board != prid_reg) {
			warn_msg("Overriding chip ID to be %04x, actual "
				"chip ID %04x",
				chip_id_without_rev(prid_board),
				chip_id_without_rev(prid_reg));
		}
	}
}

static void print_clock_info(void)
{
	uint32_t sysif_mhz;

	xprintf("CPU: %dx %s [%08x] %d MHz\n",
		arch_get_num_processors(),
		arch_get_cpu_bootname(),
		arch_get_midr(),
		get_cpu_freq_mhz());

	xprintf("SCB: %d MHz\n", (uint32_t)arch_get_scb_freq_hz()/1000000);
	sysif_mhz = (uint32_t)arch_get_sysif_freq_hz() / 1000000;
	if (sysif_mhz != 0)
		xprintf("SYSIF: %d MHz\n", sysif_mhz);
	board_print_ddrspeed();
}

/*  *********************************************************************
    *  bolt_start(ept)
    *
    *  Start a user program
    *
    *  Input parameters:
    *	   ept - entry point
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
void bolt_start(unsigned long ept, unsigned long param1,
				unsigned long param2, unsigned long param3)
{
	bolt_launch(ept, param1, param2, param3);
}

#ifdef STUB64_START
void bolt_start64(unsigned long ept, unsigned long param1,
				unsigned long param2, unsigned long param3)
{
	bolt_launch64(ept, param1, param2, param3);
}

void bolt_start64_el3(unsigned long ept, unsigned long param1,
				unsigned long param2, unsigned long param3)
{
	bolt_launch64_el3(ept, param1, param2, param3);
}

void bolt_start32(unsigned long ept, unsigned long param1,
				unsigned long param2, unsigned long param3)
{
	bolt_launch32(ept, param1, param2, param3);
}
#endif

/*  *********************************************************************
    *  bolt_config_info()
    *
    *  Display startup memory configuration messages
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
void bolt_config_info(int detail)
{
	struct fsbl_info *info;

	board_printinfo();

	if (!detail)
		return;

	xprintf("\n");

	xprintf("BSP version: '%s'\n", sec_get_bsp_version());

	xprintf("Total memory used by BOLT: 0x%08lX - 0x%08lX (%lu)\n",
		mem_bottomofmem,
		mem_topofmem,
		mem_topofmem - mem_bottomofmem);

	info = board_info();
	if (info) {
		xprintf("FSBL info area:            0x%08X - 0x%08X (%d)\n",
			(uint32_t) info,
			(uint32_t) info + sizeof(*info),
			(uint32_t) sizeof(*info));
		xprintf("Page table base:           0x%08X\n",
			(uint32_t) info->pte);
	}

	xprintf("Text (code) segment:       0x%08lX - 0x%08lX (%lu)\n",
		mem_textbase, mem_textbase + mem_textsize, mem_textsize);

	xprintf("Initialized Data:          0x%08lX - 0x%08lX (%lu)\n",
		(unsigned long)&_fdata + mem_datareloc,
		(unsigned long)&_edata + mem_datareloc,
		(unsigned long)&_edata - (unsigned long)&_fdata);

	xprintf("BSS Area:                  0x%08lX - 0x%08lX (%lu)\n",
		(unsigned long)&_fbss + mem_datareloc,
		(unsigned long)&_end + mem_datareloc,
		(unsigned long)&_end - (unsigned long)&_fbss);

	xprintf("Local Heap:                0x%08lX - 0x%08lX (%u)\n",
		mem_heapstart, mem_heapstart + (__HEAP_SIZE), (__HEAP_SIZE));

	xprintf("Stack Area:                0x%08lX - 0x%08lX (%u)\n",
		mem_heapstart + (__HEAP_SIZE),
		mem_heapstart + (__HEAP_SIZE) + (__STACK_SIZE), __STACK_SIZE);

	xprintf("\nBOLT driver build information:\n");
	xprintf("LDR:     ELF   %d; RAW   %d; SREC  %d; ZIMG  %d\n",
		CFG_ELF_LDR, CFG_RAW_LDR, CFG_SREC_LDR, CFG_ZIMG_LDR);
	xprintf("UI:      LEVEL %d; MIN   %d\n", CFG_CMD_LEVEL,
		CFG_MIN_CONSOLE);
	xprintf("FS:      FAT   %d; FAT32 %d\n", CFG_FATFS, CFG_FAT32FS);
	xprintf("NET:     STACK %d; ENET  %d; TCP   %d\n", CFG_NETWORK,
		CFG_ENET, CFG_TCP);
	xprintf("USB:     STACK %d; ETH   %d; DISK  %d; SERIAL   %d; HID %d\n",
		CFG_USB, CFG_USB_ETH, CFG_USB_DISK, CFG_USB_SERIAL,
		CFG_USB_HID);
#if CFG_NAND_FLASH
	xprintf("FLASH:   NAND\n");
#else
	xprintf("FLASH:   P30   %d; P33   %d; 128J3 %d; Spansion %d\n",
		CFG_INTEL_P30, CFG_INTEL_P33, CFG_INTEL_128J3,
		CFG_AMD_SPANSION);
#endif
	splash_api_info();
}

/*  *********************************************************************
    * bolt_auto_sysinit()
    *
    *  Process automatic commands at startup
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
static void bolt_auto_sysinit(int force_init)
{
#ifdef CFG_SYSINIT
	/* this macro must be defined as a string constant
	*/
	char *cmd = CFG_SYSINIT;

	if (force_init || (env_getval("SYSINIT") != 0)) {

		if (cmd && (strlen(cmd) > 0)) {
			xprintf("AUTOBOOT [%s]\n", cmd);
			bolt_docommands(cmd);
		}
	}
#endif
}
/*  *********************************************************************
    *  bolt_autostart()
    *
    *  Process automatic commands at startup
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */
static void bolt_autostart(void)
{
	char *env;

	env = env_getenv("STARTUP");
	if (env) {
		xprintf("Executing STARTUP...\r");
		bolt_docommands(env);
	}
}

/*  *********************************************************************
    *  bolt_autostart_check()
    *
    *  Process automatic commands at startup
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   flag - indictes what autostart items are enabled.
    ********************************************************************* */
enum { NO_STARTUP = 1, NO_ETH = 2, NO_USB = 4,
		NO_SPLASH =	8, FORCE_SYSINIT = 16 };

#define CTRL(c)		((c) & 0x1f)
#define WAIT_SECS	1
#define BIG_BAR		"******************************************"

static int bolt_autostart_check(void)
{
	int flag = 0, first_print = 1;
	char ch;
	/* If keys are being pressed, wait for up to WAIT_SECS */
	int timeout = WAIT_SECS;

	while (console_status()) {
		const char *str = NULL;

		console_read((unsigned char *)&ch, 1);

		if ((ch == CTRL('C')) &&
			!(flag & NO_STARTUP)) {	/* no autostart */
			flag |= NO_STARTUP;
			str = "Automatic startup canceled via Ctrl-C";
		} else if ((ch == CTRL('E')) &&
			!(flag & NO_ETH)) {	/* no ethernet */
			flag |= NO_ETH;
			env_setenv("ETH_OFF", "1", ENV_FLG_BUILTIN);
			str = "Ethernet disabled via Ctrl-E";
		} else if ((ch == CTRL('U')) &&
			!(flag & NO_USB)) {	/* USB in manual mode */
			flag |= NO_USB;
			env_setenv("USBMODE", "2", ENV_FLG_BUILTIN);
			str = "USB set to manual mode via Ctrl-U";
		} else if ((ch == CTRL('S')) &&
			!(flag & NO_SPLASH)) {	/* no splash */
			flag |= NO_SPLASH;
			str = "Splash screen disabled via Ctrl-S";
		}
#ifdef CFG_SYSINIT
		else if ((ch == CTRL('I')) &&
			!(flag & FORCE_SYSINIT)) {	/* Force Sys Init */
			flag |= FORCE_SYSINIT;
			str = "System initialization forced via Ctrl-I";
		}
#endif

		if (str) {
			if (first_print) {
				xprintf(BIG_BAR "\n");
				first_print = 0;
			}
			xprintf("%s\n", str);
			/* Wait a bit longer if we got a valid key combo */
			timeout++;
		}

		if (timeout > 0 && !console_status()) {
			timeout--;
			bolt_sleep(BOLT_HZ);
		}
	}

	if (flag)
		xprintf(BIG_BAR "\n\n");

	return flag;
}

static void bolt_dt_off_check(void)
{
	char *val = env_getenv("DT_OFF");

	if (val && strcmp(val, "1") == 0)
		xprintf("Note: Automatic DT modification is currently off.\n");
}

/*  *********************************************************************
    *  bolt_main(a,b)
    *
    *  It's gotta start somewhere.
    *
    *  Input parameters:
    *	   a,b - not used
    *
    *  Return value:
    *	   does not return
    ********************************************************************* */

void bolt_main(int a, int b)
{
	int sflags;

	/*
	 * By the time this routine is called, the following things have
	 * already been done:
	 *
	 * 1. The processor(s) is(are) initialized.
	 * 2. The caches are initialized.
	 * 3. The memory controller is initialized.
	 * 4. BSS has been zeroed.
	 * 5. The data has been moved to R/W space.
	 * 6. The "C" Stack has been initialized.
	 * 7. mem_bottomofmem, mem_heapstart and mem_topofmem have been set up.
	 */

	mem_totalsize = board_totaldram();
	mem_textbase = (unsigned long)&_ftext;
	mem_textsize = (unsigned long)&_fdata - (unsigned long)&_ftext;

	bolt_startflags = 0;

	bolt_bg_init();		/* init background processing */
	bolt_attach_init();
	bolt_timer_init();	/* Timer process */
	bolt_bg_add(bolt_device_poll, NULL);
	bolt_bg_add(bolt_gisb_task, NULL);

	/*
	 * Initialize the memory allocator
	 */

	KMEMINIT((unsigned char *)(uintptr_t) mem_heapstart, __HEAP_SIZE);

	/*
	 * Initialize the console.  It is done before the other devices
	 * get turned on.  The console init also sets the variable that
	 * contains the CPU speed.
	 */

	console_init();

	/*
	 * Set up the exception vectors
	 */

/* arm    bolt_setup_exceptions(); */

#ifdef S_UNITTEST
	bolt_aegis_cr_unblock();
#endif

	bolt_psci_init();

	/* Printout BOLT identification info */
	say_hello(0);

	/*
	 * Initialize the other devices.
	 */
#if defined(S_UNITTEST)
	bolt_aegis_loop();
#endif
	sflags = bolt_autostart_check();

	board_device_init();
#if (CFG_CMD_LEVEL >= 5)
	bolt_config_info(1);
#endif
	print_clock_info(); /* CPU, SCB, DDR,,, */
	bolt_setup_default_env();

	/* USB uses DT, so do setup before board_final_init() */
	bolt_devtree_prep_builtin_dtb();

	/* Setup after Devicetree is present but before
	its been modified by any DT_OPs, and before
	board_final_init() for USB, SYSTEMPORT. */
	if (CFG_CUSTOM_CODE)
		custom_early();

	/* Init Splash? This piece of code was moved
	to before board_final_init() as this is an earlier
	point in BOLT for flash to be up and running. */
	if (!(sflags & NO_SPLASH)) {
		splash_api_start();
		board_init_post_splash();
	}

#if CFG_MONITOR_OVERTEMP
	/* We may stall *HERE* during an over temperature
	condition while we wait for the chip to cool down. */
	bolt_overtemp_init();
#endif
	avs_info_print();

	ui_init();

	board_final_init();

	bolt_setup_saved_env();

	board_check(0);

	/* Custom setup that is done just
	before we could autorun. */
	if (CFG_CUSTOM_CODE)
		custom_init();

	bolt_dt_off_check();
	if (!(sflags & NO_STARTUP)) {
		bolt_auto_sysinit(sflags & FORCE_SYSINIT);
		bolt_autostart();
#if defined(S_UNITTEST_AUTOFLASH)
		bolt_aegis_flash_update_done();
#endif
	}

	/* Replace the BOLT commandline, or do other things
	just before we get to the BOLT prompt. */
	if (CFG_CUSTOM_CODE)
		custom_main();

	bolt_command_loop();
}

/*  *********************************************************************
    *  bolt_command_restart()
    *
    *  This routine is called when an application wants to restart
    *  the firmware's command processor.  Reopen the console and
    *  jump back into the command loop.
    *
    *  Input parameters:
    *	   status - A0 value of program returning to firmware
    *
    *  Return value:
    *	   does not return
    ********************************************************************* */

void bolt_command_restart(uint64_t status)
{
	/*
	 * Call board reset functions
	 */
	board_device_reset();

	/*
	 * Reset devices
	 */
	bolt_device_reset();

	/*
	 * Reset timers
	 */
	bolt_timer_init();

	/*
	 * Reopen console
	 */
	console_open(console_name);

	/*
	 * Display program return status
	 */

	xprintf("*** program exit status = %d\n", (int)status);

	/*
	 * Back to processing user commands
	 */
	bolt_command_loop();
}

/*  *********************************************************************
    *  bolt_docommands(buf)
    *
    *  Process (possibly multiple) commands from a buffer
    *
    *  Input parameters:
    *	   buf - buffer
    *
    *  Return value:
    *	   exit status of first command that failed, or null
    ********************************************************************* */
int bolt_docommands(const char *buf)
{
	queue_t cmdqueue;
	ui_command_t *cmd;
	int status = CMD_ERR_BLANK;
	int term;

	q_init(&cmdqueue);

	while ((cmd = cmd_readcommand(&buf)))
		q_enqueue(&cmdqueue, (queue_t *) cmd);

	while ((cmd = (ui_command_t *) q_deqnext(&(cmdqueue)))) {
		if (env_getenv("CMD_ECHO"))
			printf("{CMD} <%s>\n", cmd->ptr);
		status = ui_docommand(cmd->ptr);
		term = cmd->term;
		KFREE(cmd);
		if (status == CMD_ERR_BLANK)
			continue;

		/*
		 * And causes us to stop at the first failure.
		 */
		if ((term == CMD_TERM_AND) && (status != 0))
			break;

		/*
		 * OR causes us to stop at the first success.
		 */

		if ((term == CMD_TERM_OR) && (status == 0))
			break;

		/*
		 * Neither AND nor OR causes us to keep chugging away.
		 */
	}

	return status;
}

/*  *********************************************************************
    *  bolt_command_loop()
    *
    *  This routine reads and processes user commands
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   does not return
    ********************************************************************* */

void bolt_command_loop(void)
{
	char buffer[CMDLINE_BUF_SIZE];
	int status;
	char *prompt, *env;

	for (;;) {
#if CFG_MONITOR_OVERTEMP
		prompt = bolt_overtemp_prompt();
			if (!prompt)
#endif
				prompt = env_getenv("PROMPT");

		if (!prompt)
			prompt = "BOLT> ";
		console_readline(prompt, buffer, sizeof(buffer));

		/* Expand the command if entered as an environment
		variable. Otherwise, command concatenation within
		a variable won't work. */
		if (buffer[0] == '$') {
			env = env_getenv(&buffer[1]);
			if (env)
				lib_strcpy(buffer, env);
		}

		status = bolt_docommands(buffer);
		if (status != CMD_ERR_BLANK)
			xprintf("*** command status = %d\n", status);
	}
}

void bolt_master_reboot(void)
{
	BDEV_WR_F(SUN_TOP_CTRL_RESET_SOURCE_ENABLE, sw_master_reset_enable, 1);
	BDEV_WR_F(SUN_TOP_CTRL_SW_MASTER_RESET, chip_master_reset, 1);
}
