/*****************************************************************************
*
* Copyright 2014 - 2015 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*
*****************************************************************************/

#include <error.h>
#include <zimage.h>
#include <arch.h>
#include <bchp_common.h>
#include <lib_physio.h>
#include <bchp_aon_ctrl.h>
#include "android_bsu.h"
#include "fastboot.h" /* to access partition table that has Android-style
			flash naming */

#define AON_REG_ADDR(idx)			((volatile uint32_t *) \
		REG_ADDR(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE + (idx) * 4))

#define AON_REG(idx)				*AON_REG_ADDR(idx)

/* Register index into AON_SYSTEM_DATA_RAM. */
#define AON_REG_ANDROID_RESTART_CAUSE   9


/* BOLT environment variable names for Android recovery/boot images */
#define ENV_ANDROID_RECOVERY_IMG	"ANDROID_RECOVERY_IMG"
#define ENV_ANDROID_BOOT_IMG		"ANDROID_BOOT_IMG"

/* Decided by parameter of pagesize for mkbootimg to create android image */
#define ANDROID_KERNEL_PAGE_SIZE 0x1000

/* See bootimg.h in Android source code for the full definition */
#define BOOT_MAGIC	"ANDROID!"
#define BOOT_MAGIC_SIZE	8
#define BOOT_NAME_SIZE	16
#define BOOT_ARGS_SIZE	512

enum bootmode {
	BOOTMODE_RECOVERY,
	BOOTMODE_ANDROID,
	BOOTMODE_USER_DEFINED
};

struct img_hdr {
	unsigned char magic[BOOT_MAGIC_SIZE];

	unsigned kernel_size;
	unsigned kernel_addr;

	unsigned ramdisk_size;
	unsigned ramdisk_addr;

	unsigned second_size;
	unsigned second_addr;

	unsigned tags_addr;
	unsigned page_size;
	unsigned unused[2];

	unsigned char name[BOOT_NAME_SIZE];
	unsigned char cmdline[BOOT_ARGS_SIZE];
	unsigned id[8];
};

/* Bootloader Message
 *
 * This structure describes the content of a block in flash
 * that is used for recovery and the bootloader to talk to
 * each other.
 *
 * Refer to AOSP "bootable/recovery/bootloader.h" file for
 * the detailed usage of the structure.
 *
 */
struct bootloader_message {
	char command[32];
	char status[32];
	char recovery[768];

	/* The 'recovery' field used to be 1024 bytes.  It has only ever
	 * been used to store the recovery command line, so 768 bytes
	 * should be plenty.  We carve off the last 256 bytes to store the
	 * stage string (for multistage packages) and possible future
	 * expansion.*/
	char stage[32];
	char reserved[224];
};

/* The upper limit for Android property value */
#define ANDROID_PROP_VALUE_MAX_STR_LEN	91

/* Helper function for gen_bootargs() */
char *get_serial_no(void)
{
	char *env_board_sn;
	int env_strlen;

	/* For reference design board, the BOARD_SERIAL will be generated and
	 * saved the first time board is booted-up */
	env_board_sn = env_getenv("BOARD_SERIAL");

	if (env_board_sn) {
		env_strlen = os_strlen(env_board_sn);

		/* Safety check to ensure BOARD_SERIAL environment variable is
		 * not completely unalinged with what is expected by Android.
		 * Note the serial number for reference design board will
		 * always be 53 char in length. */
		if (env_strlen > ANDROID_PROP_VALUE_MAX_STR_LEN) {
			os_printf("BOARD_SERIAL strlen (%d) > allowed len (%d)\n",
				env_strlen, ANDROID_PROP_VALUE_MAX_STR_LEN);
			goto err_exit;
		}
		return env_board_sn;
	}

err_exit:
	return NULL;
}

/* Default value for androidboot.hardware property */
#define ANDROID_HW_NAME_DEFAULT		"bcm_platform"

/* Helper function for gen_bootargs() */
char *get_hardware_name()
{
	char *env_hwname;
	int env_strlen;

	/* PRODUCTNAME is generated at compile time based on the Android build
	 * environment variable 'TARGET_BOARD_PLATFORM'.  If this build
	 * environment variable does not exist at compile time, PRODUCTNAME
	 * will be an empty string.  In this case, we default it to a
	 * reasonable string value. */
	env_hwname = env_getenv("PRODUCTNAME");

	if (env_hwname) {
		env_strlen = os_strlen(env_hwname);

		if ((env_strlen > ANDROID_PROP_VALUE_MAX_STR_LEN) ||
							(env_strlen == 0)) {
			os_printf("PRODUCTNAME strlen (%d) invalid. Override with '%s'.\n",
					env_strlen, ANDROID_HW_NAME_DEFAULT);
			env_hwname = ANDROID_HW_NAME_DEFAULT;
		}
	} else {
		os_printf("PRODUCTNAME does not exist. Use default '%s'.\n",
				ANDROID_HW_NAME_DEFAULT);
		env_hwname = ANDROID_HW_NAME_DEFAULT;
	}

	return env_hwname;
}

/*  *********************************************************************
    *  gen_bootargs(bootargs_buf,cmdline)
    *
    *  Generate bootargs for kernel and append additional arguments as
    *  required by Android
    *
    *  Input parameters:
    *  	   bootargs_buf - pointer to the buffer for holding bootargs
    *  	   cmdline - the kernel command line to be added to bootargs_buf
    *
    *  Return value:
    *  	   int - bootarg length generated from the calling this function
    *
    ********************************************************************* */
static int gen_bootargs(char *bootargs_buf, const char *cmdline)
{
	int bootargs_buflen;
	char *serialno_propname = "androidboot.serialno";
	char *hardware_propname = "androidboot.hardware";
	char *bootreason_propname = "bootreason";
	int append_serialno = 1; /*default always append serialno property*/

	bootargs_buflen = os_sprintf(bootargs_buf, "%s", cmdline);

	/* Check if existing kernel cmdline already has the property we need
	 * to auto-generate here.  If so, do not append auto-gen params */
	if (os_strstr(cmdline, serialno_propname)) {
		DLOG("'%s' is found in cmdline - won't append new one\n",
			serialno_propname);
		append_serialno = 0;
	}

	if (append_serialno) {
		if (get_serial_no()) {
			bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
					" %s=%s", serialno_propname, get_serial_no());
		} else {
			os_printf("No valid BOARD_SERIAL env-var to create androidboot.serialno\n");
		}
	}

	/* Always append "androidboot.hardware" property */
	bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
				" %s=%s", hardware_propname, get_hardware_name());

	/* Always append "bootreason" property to indicate the system reboot reason */
	bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
				" %s=%s", bootreason_propname, aon_reset_string());

	return bootargs_buflen;
}

/*  *********************************************************************
    *  imgload_internal(fsctx,ref,la,kernelsize)
    *
    *  Read an ELF file (main routine)
    *
    *  Input parameters:
    *  	   fsctx - context
    *  	   ref   - open file handle
    *  	   la    - loader args
    *       kernelsize - returned size of kernel image when valid
    *
    *  Return value:
    *  	   0 if ok
    *  	   else error code
    ********************************************************************* */

static int imgload_internal(fileio_ctx_t *fsctx, void *ref, bolt_loadargs_t *la, int32_t *kernelsize)
{
	struct img_hdr hdr;
	int n, m;
	uint32_t kernel_offset;
	uint32_t ramdisk_offset;
	char magic[9];
	char dt_add_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */
	char bootargs_buf[BOOT_ARGS_SIZE];
	int bootargs_len;

	if (fs_read(fsctx, ref, (uint8_t *) &hdr, sizeof(hdr)) != sizeof(hdr))
		return BOLT_ERR_IOERR;

	/* Check for magic value in header */
	os_sprintf(magic, "%c%c%c%c%c%c%c%c", hdr.magic[0], hdr.magic[1],
		hdr.magic[2], hdr.magic[3], hdr.magic[4], hdr.magic[5],
		hdr.magic[6], hdr.magic[7]);
	if (os_strcmp(magic, BOOT_MAGIC) != 0) {
		os_printf("Invalid magic string (%s) in header\n", magic);
		return BOLT_ERR_IOERR;
	}

	os_printf("%14s: %s\n", "magic", magic);
	os_printf("%14s: %u\n", "kernel_size", hdr.kernel_size);
	os_printf("%14s: %p\n", "kernel_addr", hdr.kernel_addr);
	os_printf("%14s: %u\n", "ramdisk_size", hdr.ramdisk_size);
	os_printf("%14s: %p\n", "ramdisk_addr", hdr.ramdisk_addr);
	os_printf("%14s: %u\n", "second_size", hdr.second_size);
	os_printf("%14s: %p\n", "second_addr", hdr.second_addr);
	os_printf("%14s: %p\n", "tags_addr", hdr.tags_addr);
	os_printf("%14s: %u\n", "page_size", hdr.page_size);
	os_printf("%14s: '%s'\n", "name", hdr.name);
	os_printf("%14s: '%s'\n", "cmdline", hdr.cmdline);
	os_printf("%14s: %u\n", "id", hdr.id);

	n = (hdr.kernel_size + hdr.page_size - 1) / hdr.page_size;
	m = (hdr.ramdisk_size + hdr.page_size - 1) / hdr.page_size;

	*kernelsize = hdr.kernel_size;

	kernel_offset = 1 * hdr.page_size;
	ramdisk_offset = (1 + n) * hdr.page_size;

	os_printf("%14s: %p, %d pages\n", "kernel_offset", kernel_offset, n);
	os_printf("%14s: %p, %d pages\n", "ramdisk_offset", ramdisk_offset, m);

	if (hdr.page_size != ANDROID_KERNEL_PAGE_SIZE) {
		os_printf("ERROR: Currently code expects page_size to be 0x%08x. "
			"Fix skip in zlibfs_fileop_open()\n",
			ANDROID_KERNEL_PAGE_SIZE);
		return BOLT_ERR_IOERR;
	}

	os_printf("\nLoading ramdisk image to address 0x%08x\n",
							hdr.ramdisk_addr);

	fs_seek(fsctx, ref, ramdisk_offset, FILE_SEEK_BEGINNING);
	if (fs_read(fsctx, ref, (uint8_t *)hdr.ramdisk_addr, hdr.ramdisk_size)
						!= (int)hdr.ramdisk_size) {
		os_printf("failed to load ramdisk\n");
		return BOLT_ERR_IOERR;
	}

	/* Add initrd info to DT (BOLT does not support ATAGS method) */
	DLOG("Adding initrd info to DT\n");
	os_sprintf(dt_add_cmd, "dt add prop chosen linux,initrd-start i 0x%x",
		hdr.ramdisk_addr);
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop chosen linux,initrd-end i 0x%x",
		hdr.ramdisk_addr + hdr.ramdisk_size);
	bolt_docommands(dt_add_cmd);

	/* Add kernel command line to DT only if there was none specified */
	if ((hdr.cmdline != NULL) && (la->la_options == NULL)) {
		DLOG("Adding kernel command line to DT\n");

		bootargs_len = gen_bootargs(bootargs_buf, (const char *) hdr.cmdline);
		/* Safety check in case the final bootargs is larger than what
		 * is allowed based on the boot.img header struct */
		if (bootargs_len > BOOT_ARGS_SIZE) {
			os_printf("Invalid boot cmdline length (%d)\n",
					bootargs_len);
			os_printf("boot cmdline: %s\n", bootargs_buf);
			return BOLT_ERR;
		}

		os_sprintf(dt_add_cmd, "dt add prop chosen bootargs s '%s'",
							bootargs_buf);
		bolt_docommands(dt_add_cmd);
	}

	return 0;
}

static int zimage_check(void *ptr)
{
	struct bolt_zimage_s *hdr = ptr;

	if (hdr->magic != DT_ZIMAGE_SIGNATURE)
		return BOLT_ERR_BADEXE;
	if (hdr->astart >= hdr->aend)
		return BOLT_ERR_BADEXE;
	/* TODO: any more overflow checks needed? */
	return hdr->aend - hdr->astart;
}

static void zimage_set_end_env(unsigned int address)
{
	char buffer[40];

	os_sprintf(buffer, "setenv ZIMAGE_END %x", address);
	bolt_docommands(buffer);
}

/*  *********************************************************************
    *  bolt_imgload(la)
    *
    *  Read an android boot image file
    *
    *  Input parameters:
    *      la - loader args
    *
    *  Return value:
    *  	   0 if ok, else error code
    ********************************************************************* */
static int bolt_imgload(bolt_loadargs_t *la)
{
	int res;
	fileio_ctx_t *fsctx;
	void *ref;
	int ttlcopy = 0;
	uint8_t *ptr;
	uint8_t *bootcode;
	int32_t maxsize;
	int32_t kernelsize;
	int amtcopy;
	int thisamt;
	int onedot;
	int imagebytes;
	struct fastboot_ptentry *ptn;
	unsigned long long pt_bytes_offset;
	char filename[25];
	const char *partition;
	int iter =0;

	bootcode = (uint8_t *) la->la_address;
	maxsize = la->la_maxsize;

	/* SWBOLT-340: Compression not allowed for zImage
	*/
	if (la->la_flags & LOADFLG_COMPRESSED)
		return BOLT_ERR_INV_PARAM;

	/*
	 * Check file system type - either 'raw' or 'fat'.
	 * If file-system is 'raw' & we are using USB flash drive, then we
	 * need to construct the proper filename with "offset,length" format
	 * before passing to fileops function to enable fs_read() to access
	 * the raw data in the correct partition.
	 * This is to workaround the fact that "fastboot flash" function
	 * cannot perform file-write to FAT partition on USB drive.
	 * This is an unsupported operation as indicated in fatfs.c.
	 * So the "fastboot flash" has to write the boot image as raw.
	 * Therefore, we also have to load the file as raw in "android boot"
	 */
	if (!os_strcmp(la->la_filesys, "raw") &&
				(!os_strcmp(la->la_device, "usbdisk0"))) {

		partition = la->la_filename;

		if (partition == NULL) {
			os_printf("Partition can't be empty to read from %s\n",
								la->la_device);
			return BOLT_ERR;
		}

		/* always re-populate GPT from device to ram to take care of
		 * the case where android-fastboot command is not called before
		 * calling android-boot */
		res = fastboot_discover_gpt_tables(la->la_device);
		if (res < 0) {
			os_printf("Can't load raw file from %s without GPT\n",
								la->la_device);
			return BOLT_ERR_IOERR;
		}
		ptn = fastboot_flash_find_ptn(partition);
		if (ptn == 0) {
			os_printf("Partition '%s' does not exist\n", partition);
			return BOLT_ERR;
		}

		pt_bytes_offset = (unsigned long long) ptn->start * BYTES_PER_LBA;

		os_sprintf(filename, "0x%llx,0x%llx",
					pt_bytes_offset, ptn->length);

		DLOG("Use rawfs: replace filename '%s' with '%s'\n",
							partition, filename);
	} else {
		/* Note: the input filename can be NULL, but it is OK to copy
		 * it as the underlying fileops function will ignore it
		 * accordingly. */
		os_sprintf(filename, "%s", la->la_filename);
	}

	/*
	 * Create a file system context
	 */

	res = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (res != 0)
		return res;

	/*
	 * Open the boot device
	 */

	res = fs_open(fsctx, &ref, filename, FILE_MODE_READ);
	if (res != 0)
		goto uninit_fs;

	res = imgload_internal(fsctx, ref, la, &kernelsize);
	if (res != 0)
		goto close_file;

	DLOG("Seeking to start of the kernel image\n");
	res = fs_seek(fsctx, ref, ANDROID_KERNEL_PAGE_SIZE,
		      FILE_SEEK_BEGINNING);
	if (res != ANDROID_KERNEL_PAGE_SIZE)
		goto close_file;

	/*
	 * Okay, go load the boot file.
	 */

	ptr = bootcode;
	amtcopy = maxsize;

	res = fs_read(fsctx, ref, ptr, ZIMAGE_HEADER);
	if (res < 0) {
		goto close_file;
	} else if (res < ZIMAGE_HEADER) {
		res = BOLT_ERR_BADEXE;
		goto close_file;
	}

	res = zimage_check(ptr);
	if (res >= 0) {
		la->la_flags &= ~(LOADFLG_APP64);
		imagebytes = res;
		os_printf("Reading %d bytes from zImage.\n", imagebytes);
		amtcopy = imagebytes - ZIMAGE_HEADER;
		zimage_set_end_env((unsigned int)bootcode + imagebytes);
		ttlcopy = 0;
	} else {
		la->la_flags |= LOADFLG_APP64;
		ttlcopy += ZIMAGE_HEADER;
		amtcopy = kernelsize  - ZIMAGE_HEADER;
		os_printf("Reading %d bytes from Image.\n", kernelsize);
	}

	ptr += ZIMAGE_HEADER;

	onedot = amtcopy / 10;	/* ten dots for entire load */
	if (onedot < 4096)
		onedot = 4096;	/* but minimum 4096 bytes per dot */
	onedot = (onedot + 1) & ~4095;	/* round to multiple of 4096 */

	while (amtcopy > 0) {
		thisamt = onedot;
		if (thisamt > amtcopy)
			thisamt = amtcopy;

		res = fs_read(fsctx, ref, ptr, thisamt);
		if (res <= 0)
			break;
		os_printf(".");

		ptr += res;
		amtcopy -= res;
		ttlcopy += res;

		iter++;
	}

	/*
	 * We're done with the file.
	 */

close_file:
	os_printf("\n");
	fs_close(fsctx, ref);
uninit_fs:
	fs_uninit(fsctx);

	la->la_entrypt = (uintptr_t) bootcode;

	if (la->la_flags & LOADFLG_NOISY)
		os_printf(" %d bytes read\n", ttlcopy);

	return (res < 0) ? res : ttlcopy;
}

static const bolt_loader_t imgloader = {
	.name = "img",
	.loader = bolt_imgload,
	.flags = 0
};

#if defined(BCHP_WKTMR_REG_START)
#define BCHP_WKTMR_COUNTER_REG_OFFSET     0x4
#define DEFAULT_ANDROID_DATE_UTC_SEC  (1420070400) /* Jan-01-2015 00:00 */

/*  *********************************************************************
    *  wktmr_adjust(cmd, argc, argv[])
    *
    *  Adjust the WKTMR value, which may be used for synchronizing the system
    *  time, to be a more current date if needed.
    *
    *  Input parameters:
    *     none
    *
    *  Return value:
    *     none
    ********************************************************************* */
static void wktmr_adjust(void)
{
	volatile uint32_t *counter = (volatile uint32_t *)
		REG_ADDR(BCHP_WKTMR_REG_START + BCHP_WKTMR_COUNTER_REG_OFFSET);

	/* Do not adjust if WKTMR further in time than the default time */
	if (*counter < DEFAULT_ANDROID_DATE_UTC_SEC) {
		os_printf("Setting WKTMR to %u sec (UTC)\n",
			DEFAULT_ANDROID_DATE_UTC_SEC);
		*counter = DEFAULT_ANDROID_DATE_UTC_SEC;
	}
}
#endif

/*  *********************************************************************
    *  int is_in_boot_recovery_mode(void)
    *
    *  Helper function for android_boot() to determine if the platform is
    *  still in recovery mode by reading the Bootloader Control Block (BCB)
    *  that resides in the 'misc' partition.
    *
    *  Input parameters:
    *     none
    *
    *  Return value:
    *     0 - not in recovery
    *     1 - still in recovery
    ********************************************************************* */

static struct bootloader_message bootloader_ctrl_blk;

static int is_in_boot_recovery_mode(void)
{
	int fd=-1;
	int ret;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	struct bootloader_message *bcb = &bootloader_ctrl_blk;

	/* Re-use the FB_DEVICE_TYPE env var, which is intended for auto-boot
	 * into bootloader mode, to determine the flash device name that
	 * should be used to access the 'misc' partition.
	 * If this env var is not defined for any reason, then just do normal
	 * boot */
	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read misc partition\n");
		goto ret_normal_boot;
	}

	/* Construct the BOLT device name to access 'misc' partition. This way
	 * of constructing the BOLT device name only works for eMMC device.
	 * If FB_DEVICE_TYPE is 'usbdisk0' (to boot from USB), then it will
	 * fail to open such device as well. This is done intentionally as we
	 * don't expect production Android platform to boot from USB.*/
	os_sprintf(flash_devname, "%s.misc", fb_flashdev_mode_str);
	DLOG("Try to open '%s'...\n", flash_devname);

	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read bootloader ctrl blk: %d\n",
				flash_devname, fd);
		goto ret_normal_boot;
	}

	ret = bolt_readblk(fd, 0, (unsigned char *)bcb, sizeof(bootloader_ctrl_blk));
	if (ret != sizeof(bootloader_ctrl_blk)) {
		os_printf("Error reading %s. Can't read bootloader ctrl blk: %d\n",
				flash_devname, ret);
		goto ret_normal_boot;
	}
	DLOG("Read %d bytes. bcb->command: %s\n", ret, bcb->command);

	/* Go into recovery mode if the bcb->command matches with
	 * "boot-recovery" because it means the recovery was not completed
	 * from last boot-up session.*/
	if (!os_strcmp(bcb->command, "boot-recovery"))
		goto ret_recovery_boot;

	/* Always fall back to normal boot if the bcb->command does not match
	 * or if we encounter any errors*/
ret_normal_boot:
	if (fd >= 0)
		bolt_close(fd);
	return 0;

ret_recovery_boot:
	if (fd >= 0)
		bolt_close(fd);
	return 1;
}

/* Helper functions to access boot reason register */
void boot_reason_reg_set(uint32_t reg_val)
{
	AON_REG(AON_REG_ANDROID_RESTART_CAUSE) = reg_val;
}

uint32_t boot_reason_reg_get(void)
{
	return AON_REG(AON_REG_ANDROID_RESTART_CAUSE);
}

/*  *********************************************************************
	*  int recovery_mode_boot_override(void)
	*
	*  Helper function for android_boot() to determine if user pressed
	*  the front panel BT Pair gpio button during boot up to force device
	*  boot into recovery mode.
	*
	*  Input parameters:
	*     none
	*
	*  Return value:
	*     0 - boot normal image
	*     1 - boot recovery image
	********************************************************************* */
static int recovery_mode_boot_override(void)
{
	int pin,ret;
	char buffer[20];
	char *pin_str;
	char *gpio_bank_pin_str;

	gpio_bank_pin_str = env_getenv("BT_PAIR");

	if (!gpio_bank_pin_str) {
		os_printf("No button is configured, proceed with normal booting\n");
		return 0;
	}
	os_strncpy(buffer, gpio_bank_pin_str, 20);
	buffer[19] = '\0';

	os_strtok_r(buffer, " ", &pin_str);
	pin = os_atoi(pin_str);

	ret = cmd_gpio_btn_state(buffer, pin);

	if (ret < 0) {
		/* always fall back to boot android image with error message */
		return 0;
	} else
		return ret;
}

/*  *********************************************************************
    *  android_boot(cmd, argc, argv[])
    *
    *  Entry function to handle 'android boot' command
    *
    *  Input parameters:
    *  	   cmd - input command
    *  	   argc - number of input arguments
    *      argv - variable number of input arguments
    *
    *  Return value:
    *  	   0 if ok
    *  	   else error code
    ********************************************************************* */

int android_boot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int ret;
	int len;
	char boot_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */
	char bootargs_buf[BOOT_ARGS_SIZE];
	int bootargs_len;
	const char *boot_image = NULL;
	uint32_t boot_reason;
	enum bootmode boot_mode;
	char *fb_transport_mode_str;
	char *fb_flashdev_mode_str;

	if (argc > 1) {
		os_printf("Too many arguments, or an unquoted optional [arg]\n");
		return BOLT_ERR_INV_PARAM;
	}

	os_printf("Adding Android img loader to BOLT\n");
	ret = boot_addloader(&imgloader);
	if (ret) {
		os_printf("Failed to add Android img loader to BOLT\n");
		return ret;
	}

	/*
	 * We'll be using the built-in "boot" command in BOLT to boot the
	 * Android boot image, but specify the use of the img loader
	 */
	len = os_sprintf(boot_cmd, "boot -loader=img");

	/* Built-in "boot" command has the "-rawfs" switch to allow user
	 * to indicate the image should be read from device as raw image
	 * This allows reading USB device as raw instead of as FAT-fs */
	if (cmd_sw_isset(cmd, "-rawfs"))
		len += os_sprintf(boot_cmd + len, " -rawfs");

	/* Get the boot image based on reason register (if set) or command
    * line switches */
	{
		/* Read and clear the boot reason register */
		boot_reason = boot_reason_reg_get();
		boot_reason_reg_set(0);

		DLOG("boot_reason = %u\n", boot_reason);

		/* Determine the boot mode based on boot reason register.
		 * Note that first letter of the reboot command is saved in
		 * the register. */
		if (boot_reason == 'r') {
			os_printf("boot reason = recovery\n");
			boot_mode = BOOTMODE_RECOVERY;
		} else if (boot_reason == 'b') {
			os_printf("boot reason = bootloader\n");

			/* If environment variables are set, then directly use
			 * what are defined by users.  Default to
			 * use '-transport=usb' option, however, '-device='
			 * must be specified by users.
			 * Let the "android fastboot" cmd to check whether
			 * input is valid */
			fb_transport_mode_str = env_getenv("FB_TRANSPORT_MODE");
			if (!fb_transport_mode_str) {
				fb_transport_mode_str = "usb";
			}

			fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
			if (!fb_flashdev_mode_str) {
				os_printf("Android fastboot device type ('FB_DEVICE_TYPE' env var) not defined in BOLT\n");
				goto failed;
			}

			/* Call "android fastboot" because reboot reason is
			 * "bootloader" */
			len = os_sprintf(boot_cmd, "android fastboot -transport=%s -device=%s",
						fb_transport_mode_str, fb_flashdev_mode_str);

			os_printf("Entering fastboot mode...\n%s\n", boot_cmd);

			ret = bolt_docommands(boot_cmd);

			/* Handle the cases when device exits out of fastboot
			 * mode because user hits any keyboard keys or
			 * user provides invalid transport/device option
			 * through the environment variables. */
			if (ret == BOLT_OK) {
				os_printf("Exited fastboot mode, but stays in BOLT.\n");
				os_printf("If you have just fastboot-flashed images, then call 'android boot' or 'reboot' now.\n");
				return BOLT_OK;
			} else {
				/* Error reported from 'android fastboot' cmd.
				 * Report failure to user accordingly. */
				goto failed;
			}
		} else {
			/* Check 'misc' partition to determine if recovery
			 * command exists.  If yes, it means the board was in
			 * the middle of recovery image update when a reboot
			 * happened (e.g. reboot caused by accidental power
			 * outage during recovery). We need to go back into
			 * recovery mode to allow a complete image to be
			 * updated before getting into normal boot */
			/* Check gpio button state to determine the BT_PAIR
			 * button is configured and it's pressed. If yes,
			 * it means user want to boot into recovery image. */
			os_printf("Checking 'misc' partition and front panel button state...\n");

			if (is_in_boot_recovery_mode() || recovery_mode_boot_override()) {
				os_printf("boot reason = recovery\n");
				boot_mode = BOOTMODE_RECOVERY;
			} else {
				/* Treat all other reasons as Android normal boot */
				os_printf("boot reason = normal\n");
				boot_mode = BOOTMODE_ANDROID;
			}
		}

		if (boot_mode == BOOTMODE_ANDROID) {
			if (cmd_sw_value(cmd, "-i", &boot_image)) {
				os_printf("boot reason = select image '%s'\n", boot_image);
				boot_mode = BOOTMODE_USER_DEFINED;
			} else if (cmd_sw_isset(cmd, "-r")) {
				os_printf("boot reason = recovery (user setup)\n");
				boot_mode = BOOTMODE_RECOVERY;
			}
		}
	}

	switch (boot_mode) {
	case BOOTMODE_RECOVERY:
		boot_image = env_getenv(ENV_ANDROID_RECOVERY_IMG);
		if (boot_image == NULL) {
			os_printf("Recovery image ('%s' env var) not defined in BOLT\n",
				ENV_ANDROID_RECOVERY_IMG);
			goto failed;
		}
		break;
	case BOOTMODE_ANDROID:
		boot_image = env_getenv(ENV_ANDROID_BOOT_IMG);
		if (boot_image == NULL) {
			os_printf("Boot image ('%s' env var) not defined in BOLT\n",
				ENV_ANDROID_BOOT_IMG);
			goto failed;
		}
		break;
	case BOOTMODE_USER_DEFINED:
	default:
		/* The 'boot_image' file is provided via user-command.
		 * Safety check to ensure boot_image!=NULL if we get here */
		if (boot_image == NULL) {
			os_printf("Boot image not provided in command\n");
			goto failed;
		}
	}

	len += os_sprintf(boot_cmd + len, " %s", boot_image);

	/* If user provides the optional arg, then override the built-in
	 * kernel command line by passing argv[0] to 'boot' command. */
	if (argc == 1) {

		os_printf("Override cmdline from boot.img...\n");

		bootargs_len = gen_bootargs(bootargs_buf, argv[0]);
		/* Safety check in case the final bootargs is larger than what
		 * is allowed based on the boot.img header struct */
		if (bootargs_len > BOOT_ARGS_SIZE) {
			os_printf("Invalid boot cmdline length (%d)\n",
					bootargs_len);
			os_printf("boot cmdline: %s\n", bootargs_buf);
			goto failed;
		}

		/* Update the 'boot' command with all the arguments
		 * constructed from above */
		os_sprintf(boot_cmd + len, " '%s'", bootargs_buf);
	}

#if defined(BCHP_WKTMR_REG_START)
	/* Adjust the wake-up timer */
	wktmr_adjust();
#endif

	DLOG("boot_cmd=%s\n", boot_cmd);

	bolt_docommands(boot_cmd);

failed:
	/* If we reach here then we have failed to boot */
	os_printf("Boot FAILED\n");

	return BOLT_ERR;
}
