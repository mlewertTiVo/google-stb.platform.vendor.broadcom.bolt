/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <avs_bsu.h> /* for avs_info_print() */
#include <bolt.h>
#include <board.h>
#include <board_init.h>
#include <bsp_config.h>
#include <chipid.h>
#include <common.h>
#include <console.h>
#include <custom_code.h>
#include <devfuncs.h>
#include <device.h>
#include <devtree.h>
#include <env_subr.h>
#include <fsbl-common.h>
#include <gisb.h>
#include <initdata.h>
#include <iocb.h>
#include <ioctl.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <reboot.h>
#include <splash-api.h>
#include <timer.h>
#include <ui_command.h>
#include <ui_init.h>
#include <ssbl-common.h>
#include <ssbl-sec.h>
#include <supplement-fsbl.h>
#include <overtemp.h>
#include <net_api.h>
#include <arch_ops.h>
#if CFG_MON64
#include <mon64.h>
#endif

#include <bchp_sun_top_ctrl.h>

#if BCHP_JTAG_OTP_REG_START
#include <bchp_jtag_otp.h>
#endif

#define WANT_OTP_DECODE 1 /* SWBOLT-263 */
#include <otp_status.h> /* from gen/ */


/*  *********************************************************************
    *  Constants
    ********************************************************************* */
#define CMDLINE_BUF_SIZE	1024
/* "Stringification" of a macro! */
#define M_STR1(x)	(#x)
#define M_STR(x)	M_STR1(x)
#define DEFAULT_COAP_HALT_WAIT_TIME (BOLT_HZ / 10)   /* 100 ms */

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

static void reserve_memory_areas(void);
static void reserve_memory_splash(void);
#ifdef S_UNITTEST
void bolt_aegis_cr_unblock(void);
void bolt_aegis_flash_update_done(void);
void bolt_aegis_loop(void);
#endif
void bfw_main(void);


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

	if (!env_getenv("BOARD_BTMAC")) {
		uint32_t rnd;

		if (!sec_get_random_num(&rnd, 1)) {
			xsprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
				0x22, 0x22, rnd&0xFF, (rnd>>8)&0xFF, (rnd>>16)&0xFF, (rnd>>24)&0xFF);
			env_setenv("BOARD_BTMAC", buffer, ENV_FLG_NORMAL);
			save++;
		} else
			warn_msg("cannot set BOARD_BTMAC");

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

	/* If NODENAME does not exist, set a default name. */
	if (!env_getenv(NODENAME_STR)) {
		char *env_serial = env_getenv(ENVSTR_BOARD_SERIAL);

		if (env_serial)
			xsprintf(buffer, "%s-%s", board_name(), env_serial);
		else
			xsprintf(buffer, "%s", board_name());
		env_setenv(NODENAME_STR, buffer,
			ENV_FLG_BUILTIN | ENV_FLG_NORMAL);
	}

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
			if (v == 0)
				continue;
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
#ifdef BCHP_JTAG_OTP_GENERAL_STATUS_8
	xprintf("bond option: 0x%02x\n",
		REG(BCHP_JTAG_OTP_GENERAL_STATUS_8) & 0xFF);
#endif
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
			xprintf("%s\n", toolchainver);
		else if (i == 4)
			xprintf("Copyright (C) %d Broadcom\n",
					buildyear);
		else
			xprintf("\n");
	}
	xprintf(ANSI_RESET "\n");

	xprintf("Board: %s", board_name());

	prod = BDEV_RD(BCHP_SUN_TOP_CTRL_PRODUCT_ID);
	fam = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
	xprintf(" (%04x of %04x%02x family)\n",
		chipid_without_rev(prod), chipid_without_rev(fam),
		(fam & CHIPID_REV_MASK) + 0xa0);
#ifdef BCHP_SUN_TOP_CTRL_STRAP_VALUE_1
	xprintf("strap=%08x,%08x:",
		BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0),
		BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_1));
#else
	xprintf("strap=%08x:", BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0));
#endif
	if (board_does_strap_disable_pcie())
		__puts(" PCIe is disabled");
	if (board_does_strap_disable_sata())
		__puts(" SATA is disabled");

	print_otp();

	/* SWBOLT-99
	*/
#ifndef SECURE_BOOT
	xprintf("RESET CAUSE: ");
#endif
	board_init_reset_history();

	if (b) {
		const uint32_t prid_board = b->prid & ~CHIPID_REV_MASK;
		const uint32_t prid_reg = prod & ~CHIPID_REV_MASK;

		if (prid_board != prid_reg) {
			warn_msg("Overriding chip ID to be %04x, actual "
				"chip ID %04x",
				chipid_without_rev(prid_board),
				chipid_without_rev(prid_reg));
		}
	}
}

static void print_clock_info(void)
{
	uint32_t sysif_mhz;

	xprintf("CPU %dx %s [%08x] %d MHz",
		arch_get_num_processors(),
		arch_get_cpuname(),
		arch_get_midr(),
		get_cpu_freq_mhz());

	xprintf(", SCB %d MHz", (uint32_t)arch_get_scb_freq_hz()/1000000);
	sysif_mhz = (uint32_t)arch_get_sysif_freq_hz() / 1000000;
	if (sysif_mhz == 0)
		xprintf("\n");
	else
		xprintf(", SYSIF %d MHz\n", sysif_mhz);
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

#if CFG_MON64
void bolt_start_mon64(unsigned long param1,
				unsigned long param2, unsigned long param3)
{
	bolt_launch_mon64(0, param1, param2, param3);
}
#endif
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
	uint32_t shmoo_ver;

	board_printinfo();

	if (!detail)
		return;

	xprintf("\n");

	xprintf("BSP version: '%s'\n", sec_get_bsp_version());

	shmoo_ver = supplement_fsbl_shmoover();
	if (shmoo_ver != 0) {
		xprintf("SHMOO version: %d.%d.%d.%d [0x%08X]\n",
			(shmoo_ver >> 24) & 0xff,
			(shmoo_ver >> 16) & 0xff,
			(shmoo_ver >> 8) & 0xff,
			(shmoo_ver >> 0) & 0xff,
			shmoo_ver);
	}

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

static void bolt_do_auto_start(void)
{
	bolt_auto_sysinit(bolt_startflags & FORCE_SYSINIT);
	bolt_autostart();
#if defined(S_UNITTEST_AUTOFLASH)
	bolt_aegis_flash_update_done();
#endif
}

#if CFG_COAP
static void bolt_coap_halt_cmd_check(void *arg)
{
	bolt_timer_t *timer = (bolt_timer_t *)arg;
	bool timer_expired = TIMER_EXPIRED(*timer);
	bool coap_halt_recvd = false;

	coap_halt_recvd = (bolt_docommands("testenv -eq COAP_HALT_CMD 1") == 0);

	if (coap_halt_recvd || timer_expired) {
		bolt_bg_remove(bolt_coap_halt_cmd_check);
		KFREE(timer);
	}

	if (!coap_halt_recvd && timer_expired)
		bolt_do_auto_start();
}
#endif

/*  *********************************************************************
    *  bolt_init_coap()
    *
    *  This routine is called to initialize coap interface.
    *  It will wait for upto 5 secs for network interface to initialize.
    *  It will wait for further 100ms(customizable through env variable)
    *       to allow coap client to send halt command if it wants to
    *
    *  Input parameters:
    *       nothing
    *
    *  Return value:
    *      0 (zero) if listening for CoAP client request to skip auto-execution
    *        (STARTUP environment variable or SYSINIT),
    *      1 (one) otherwise
    ********************************************************************* */
static int bolt_init_coap(void)
{
#if CFG_COAP
	bolt_device_t *netdev = NULL;
	int err;
	bolt_timer_t *coap_halt_timer;
	char *sleep_time_str;
	int sleep_time;

	/* wait time to allow coap client to send halt command.
	 * default wait time 100ms.
	 * can be customized through STARTUP_WAIT_TIME env.
	 * units for STARTUP_WAIT_TIME should be millisecs.
	 */

	if (bolt_docommands("testenv -eq COAP_SSDP_ENABLE 1") != 0)
		return 1; /* COAP disabled */

	netdev = bolt_finddev(DEF_NETDEV);
	if (netdev == NULL) {
		netdev = bolt_waitdev(1000, "USB-Ethernet");
		if (netdev == NULL || netdev->dev_fullname == NULL)
			return 1;
	}

	err = net_init(netdev->dev_fullname);
	if (err < 0)
		return 1;

	err = do_dhcp_request(netdev->dev_fullname);
	if (err != 0)
		return 1;

	err = do_ssdp_start();
	if (err != 0)
		return 1;

	err = do_coap_listen();
	if (err != 0)
		return 1;

	sleep_time_str = env_getenv(STARTUP_WAIT_TIME_STR);
	if (sleep_time_str == NULL)
		sleep_time = DEFAULT_COAP_HALT_WAIT_TIME;
	else
		sleep_time = lib_atoi(sleep_time_str) * BOLT_HZ / 1000;

	coap_halt_timer = (bolt_timer_t *) KMALLOC(sizeof(bolt_timer_t), 0);
	TIMER_SET(*coap_halt_timer, sleep_time);

	bolt_bg_add(bolt_coap_halt_cmd_check, coap_halt_timer);

	return 0;
#endif
	return 1;
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

	arch_abort_enable();

#ifdef S_UNITTEST
	bolt_aegis_cr_unblock();
#endif

#if CFG_SSBM
	/* With SSBM, SSBL and SSBM are in the same 1MB page.
	 * When SSBL page is set up, all 4KB pages are set up as XN
	 * except SSBL text. Set up page used by SSBM.
	 */
	arch_mark_executable(SSBM_RAM_ADDR, SSBM_SIZE, true);
	arch_mark_uncached(SSBM_RAM_ADDR, SSBM_SIZE);
#endif

#if !CFG_MON64
	bolt_psci_init();
#endif

	/* Printout BOLT identification info */
	say_hello(0);

	/*
	 * Initialize the other devices.
	 */
#if defined(S_UNITTEST)
	bolt_aegis_loop();
#endif
	sflags = bolt_autostart_check();
	bolt_startflags = sflags;

	board_device_init();

#if CFG_ZEUS5_1
	/* flash has been initialized */
	bfw_main();
#endif

#if (CFG_CMD_LEVEL >= 5)
	bolt_config_info(1);
#endif
	print_clock_info(); /* CPU, SCB, DDR,,, */
	bolt_setup_default_env();

	/* USB uses DT, so do setup before board_final_init() */
	bolt_devtree_prep_builtin_dtb();

	reserve_memory_areas();

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
		reserve_memory_splash();
	}

#if CFG_MONITOR_OVERTEMP
	/* We may stall *HERE* during an over temperature
	condition while we wait for the chip to cool down. */
	bolt_overtemp_init();
#endif

	/* Print out the latest status with DVFS enabled */
	avs_info_print();

	ui_init();

	board_final_init();

	bolt_setup_saved_env();

	board_check(0);

#if CFG_MON64
#ifdef STUB64_START
	if (arch_booted64()) {
#ifdef SECURE_BOOT
		/* Delayed mon64 decryption */
		mon64_install(MBIN_RAM_ADDR, MBIN_SIZE, NULL);
#endif
		mon64_init();
	}
	else
#endif
	bolt_psci_init();
#endif

	/* Custom setup that is done just
	before we could autorun. */
	if (CFG_CUSTOM_CODE)
		custom_init();

	bolt_dt_off_check();

	if (!(sflags & NO_STARTUP)) {
		if (bolt_init_coap() == 1)
			bolt_do_auto_start();
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

/* reserve_memory_areas -- reserves memory areas
 *
 * Reservation should be distinguished from allocation because reservation
 * does not necessarily mean the reserved memory area is actually used
 * in BOLT. It is for Linux to stay away from using it, including
 * allocating and passing it to a user applicaiton.
 */
static void reserve_memory_areas(void)
{
	int64_t retval = 0;
	struct fsbl_info *info;
	unsigned int srr_size = 0;
	uint64_t srr_offset;

	info = board_info();
	if (info == NULL) {
		err_msg("failed to obtain FSBL info\n");
	} else {
		/* available only 'fsbl_info' version 3 or higher */
		if (FSBLINFO_VERSION(info->n_boards) >= 3) {
			if (info->srr_size_mb != 0) {
				srr_size = _MB(info->srr_size_mb);
				srr_offset = _MB(1); /* two step for 64-bit */
				srr_offset *= info->srr_offset_mb;
			}
		}
	}

	/* SRR shall be the first to reserve so that what FSBL has specified
	 * is reserved.
	 */
	if (srr_size != 0)
		retval = bolt_reserve_memory(srr_size, srr_offset,
			BOLT_RESERVE_MEMORY_OPTION_ABS |
			BOLT_RESERVE_MEMORY_OPTION_DT_NEW |
			BOLT_RESERVE_MEMORY_OPTION_DT_NOMAP |
			BOLT_RESERVE_MEMORY_OPTION_OVER4GB,
			"SRR");
	if (retval < 0) {
		err_msg("failed to reserve %d MB @ %d MB for SRR %lld\n",
			info->srr_size_mb, info->srr_offset_mb, retval);
	}

#if CFG_TRUSTZONE_MON
	/* TZ MON ("MON") follows right after SRR. */
	retval = bolt_reserve_memory(_MB(CFG_TRUSTZONE_MON_SIZE_MB), _KB(4),
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW, "MON");
	if (retval < 0) {
		err_msg("failed to reserve %d MB for MON %lld\n",
			CFG_TRUSTZONE_MON_SIZE_MB, retval);
	}
#endif

#ifdef STUB64_START
#ifdef BFW_RAM_BASE
	/* Reserve BFW address. */
	retval = bolt_reserve_memory(BFW_SIZE, BFW_RAM_BASE,
		BOLT_RESERVE_MEMORY_OPTION_ABS |
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW,
		"BFW");
#endif

#if CFG_MON64
#if (CFG_SSBM)
	/* Reserve SSBM memory */
	retval = bolt_reserve_memory(SSBM_SIZE, SSBM_RAM_ADDR,
		BOLT_RESERVE_MEMORY_OPTION_ABS |
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW,
		"SSBM");
#endif

	if (arch_booted64()) {
		/* Reserve MON64 memory */
		retval = bolt_reserve_memory(MON64_SIZE, MON64_BASE,
			BOLT_RESERVE_MEMORY_OPTION_ABS |
			BOLT_RESERVE_MEMORY_OPTION_DT_NEW,
			"MON64");
		if (retval < 0) {
			err_msg("failed to reserve %d bytes for MON64 %lld\n",
				MON64_SIZE, retval);
		}
	} else {
		/* Reserve PSCI32 memory */
		retval = bolt_reserve_memory(PSCI_SIZE, PSCI_BASE,
			BOLT_RESERVE_MEMORY_OPTION_ABS |
			BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY,
			"PSCI");
		if (retval < 0) {
			err_msg("failed to reserve %d bytes for PSCI %lld\n",
				PSCI_SIZE, retval);
		}
	}
#else /* CFG_MON64 */
#if (CFG_SSBM && !CFG_ZEUS5_1)
	retval = bolt_reserve_memory(SSBM_SIZE, SSBM_RAM_ADDR,
		BOLT_RESERVE_MEMORY_OPTION_ABS |
		BOLT_RESERVE_MEMORY_OPTION_DT_NEW,
		"SSBM");
#endif

#if (CFG_SSBM && CFG_ZEUS5_1)
	/* For ZEUS5_1, SSBM_SIZE is SSBM+PSCI. */
	retval = bolt_reserve_memory(SSBM_SIZE, SSBM_RAM_ADDR,
#else
	retval = bolt_reserve_memory(PSCI_SIZE, PSCI_BASE,
#endif
		BOLT_RESERVE_MEMORY_OPTION_ABS |
		BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY,
		"PSCI");
	if (retval < 0) {
		err_msg("failed to reserve %d bytes for PSCI %lld\n",
			PSCI_SIZE, retval);
	}
#endif /* CFG_MON64 */
#endif /* STUB64_START */
}

static void reserve_memory_splash(void)
{
	struct board_type *b;
	unsigned int memc;

	b = board_thisboard();
	if (b == NULL)
		return;
	for (memc = 0; memc < b->nddr; memc++) {
		int64_t retval;
		uint32_t memtop, memlow;
		uint32_t bottom;
		unsigned int amount;
		uint32_t options;
		char tag[8]; /* strlen("splash") + memc# */

		retval = splash_glue_getmem(memc, &memtop, &memlow);
		if (retval != 0)
			continue;
		if (memtop == memlow)
			continue;

		bottom = ALIGN_TO(memlow, _KB(4));
		amount = memtop - bottom;
		amount = ALIGN_UP_TO(amount, _KB(4));
		options = 1 << memc;
		options |= BOLT_RESERVE_MEMORY_OPTION_ABS;
		options |= BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY;
		xsprintf(tag, "splash%u", memc);
		retval = bolt_reserve_memory(amount, bottom, options, tag);
		if (retval < 0) {
			err_msg("reserving splash memory [%08x..%08x) failed\n",
				bottom, memtop);
		}
	}
}
