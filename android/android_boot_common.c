/*****************************************************************************
*
* Copyright 2016 Broadcom.  All rights reserved.
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
#include "eio_boot.h"

#define AON_REG_ADDR(idx)			((volatile uint32_t *) \
		REG_ADDR(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE + (idx) * 4))

#define AON_REG(idx)				*AON_REG_ADDR(idx)

/* Register index into AON_SYSTEM_DATA_RAM. */
#define AON_REG_ANDROID_RESTART_CAUSE   9
#define AON_REG_ANDROID_RESTART_TIME    4
#define AON_REG_ANDROID_RESTART_TIME_N  5

/* BOLT environment variable names for Android recovery/boot images */
#define ENV_ANDROID_RECOVERY_IMG	"ANDROID_RECOVERY_IMG"
#define ENV_ANDROID_BOOT_IMG		"ANDROID_BOOT_IMG"

/* Decided by parameter of pagesize for mkbootimg to create android image */
#define ANDROID_KERNEL_PAGE_SIZE 0x1000

/* See bootimg.h in Android source code for the full definition */
#define BOOT_MAGIC_SIZE	8
#define BOOT_NAME_SIZE	16

#define BOOT_SLOT_DEV_DEFAULT   "flash0"

/* Boot path supported. */
enum bootpath {
	BOOTPATH_LEGACY,
	BOOTPATH_AB_SYSTEM,
	BOOTPATH_AB_SYSTEM_RECOVERY,
	BOOTPATH_AB_BOOTLOADER_RECOVERY,
};

static char *bootpath_str[] = {
	"legacy",
	"ab_android",
	"ab_recovery",
	"ab_bl_recovery",
};

enum bootmode {
	BOOTMODE_RECOVERY,
	BOOTMODE_ANDROID,
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

char *get_btmacaddr(void)
{
	char *env_board_bt;
	int env_strlen;

	env_board_bt = env_getenv("BOARD_BTMAC");

	if (env_board_bt) {
		env_strlen = os_strlen(env_board_bt);
		if (env_strlen > ANDROID_PROP_VALUE_MAX_STR_LEN) {
			os_printf("BOARD_BTMAC strlen (%d) > allowed len (%d)\n",
				env_strlen, ANDROID_PROP_VALUE_MAX_STR_LEN);
			goto err_exit;
		}
		return env_board_bt;
	}

err_exit:
	return NULL;
}

static int is_quiescent_mode(void)
{
	char *q_mode = env_getenv("A_QUIESCENT");
	if (q_mode == NULL) {
		return 0;
	}
	if (os_atoi(q_mode)) {
		return 1;
	}
	return 0;
}

static struct eio_boot eio_commander;
void clear_dmv_corrupt(const char *partition)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	struct eio_boot *eio = &eio_commander;
	int ret;
	int slot = -1;

	os_sprintf(flash_devname, "_%s", BOOT_SLOT_0_SUFFIX);
	if (os_strstr(partition, flash_devname) != NULL) {
		slot = 0;
	}
	if (slot == -1) {
		os_sprintf(flash_devname, "_%s", BOOT_SLOT_1_SUFFIX);
		if (os_strstr(partition, flash_devname) != NULL) {
			slot = 1;
		}
	}
	if (slot == -1) {
		return;
	}

	os_memset(eio, 0, sizeof(*eio));

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		return;
	}

	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);

	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n",
				flash_devname, fd);
		return;
	}

	ret = bolt_readblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
	if (ret != sizeof(struct eio_boot)) {
		os_printf("Error reading %s. Can't read commander block: %d\n",
				flash_devname, ret);
		return;
	}

	if (eio->magic != EIO_BOOT_MAGIC) {
		if (fd >= 0) {
			bolt_close(fd);
		}
		return;
	}

	if (eio->slot[slot].dmv_corrupt) {
		eio->slot[slot].dmv_corrupt = 0;
		ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
		if (ret != sizeof(struct eio_boot)) {
			os_printf("Error writing %s. Can't update commander block: %d\n",
				  flash_devname, ret);
		}
		os_printf("Slot:%d clearing CORRUPTED dm-verity.\n", slot);
	}
	if (fd >= 0) {
		bolt_close(fd);
	}
	return;
}

static int setget_dmv_corrupt_slot(int setget, int slot)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	struct eio_boot *eio = &eio_commander;
	int ret;

	os_memset(eio, 0, sizeof(*eio));

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		return 0;
	}

	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);

	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n",
				flash_devname, fd);
		return 0;
	}

	ret = bolt_readblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
	if (ret != sizeof(struct eio_boot)) {
		os_printf("Error reading %s. Can't read commander block: %d\n",
				flash_devname, ret);
		return 0;
	}

	if (eio->magic != EIO_BOOT_MAGIC) {
		if (fd >= 0) {
			bolt_close(fd);
		}
		return 0;
	}

	if (setget == 0) {
		os_printf("Slot:%d marked as dm-verity %s.\n",
			slot, eio->slot[slot].dmv_corrupt?"CORRUPTED":"good");
		if (fd >= 0) {
			bolt_close(fd);
		}
		return eio->slot[slot].dmv_corrupt;
	} else if (setget == 1) {
		eio->slot[slot].dmv_corrupt = 1;
		ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
		if (ret != sizeof(struct eio_boot)) {
			os_printf("Error writing %s. Can't update commander block: %d\n",
				  flash_devname, ret);
		}
		if (fd >= 0) {
			bolt_close(fd);
		}
		os_printf("Slot:%d marking as dm-verity CORRUPTED.\n", slot);
		return 0;
	}

	return 0;
}

static int is_dmverity_eio_mode(int slot)
{
	char *dmv_mode = env_getenv("A_DMVERITY_EIO");
	int dmv_corrupt = -1;

	dmv_corrupt = setget_dmv_corrupt_slot(0, slot);
	if (dmv_corrupt) {
		return 1;
	}
	if (dmv_mode == NULL) {
		return 0;
	}
	if (os_atoi(dmv_mode)) {
		setget_dmv_corrupt_slot(1, slot);
		return 1;
	}
	return 0;
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

#define ANDROID_VDR_IMG_TYPE "ext4"
char *get_vendor_image_type()
{
	char *env_vdrimgname;

	env_vdrimgname = env_getenv("AB_VDR_IMG_TYPE");
	if (!env_vdrimgname) {
		env_vdrimgname = ANDROID_VDR_IMG_TYPE;
	}

	return env_vdrimgname;
}

/*  *********************************************************************
    *  gen_bootargs(la,bootargs_buf,cmdline,boot_path,slot)
    *
    *  Generate bootargs for kernel/android:
    *     - append additional arguments if applicable.
    *     - create and populate '/firmware/android/xxx' device-tree nodes.
    *
    *  Input parameters:
    *       la - loader context.
    *  	    bootargs_buf - pointer to the buffer for holding bootargs
    *  	    cmdline - the kernel command line to be added to bootargs_buf
    *       boot_path - whether we boot legacy or a|b update
    *       slot - for non legacy boot path, the slot index to use
    *
    *  Return value:
    *  	   int - bootarg length generated from the calling this function
    *
    ********************************************************************* */
static int gen_bootargs(bolt_loadargs_t *la, char *bootargs_buf, const char *cmdline,
	enum bootpath boot_path, int slot)
{
	int bootargs_buflen = 0;
	char dt_add_cmd[BOOT_ARGS_SIZE+64];
	int fd=-1;
	char *fb_flashdev_mode_str;
	struct fastboot_ptentry *ptn;

	/* Partition table needed in these two cases */
	if ((boot_path == BOOTPATH_LEGACY || boot_path == BOOTPATH_AB_SYSTEM) && la) {
		char *p = NULL;
		p = os_strstr(la->la_device, ".");
		if (p)
			*p = '\0';
		fastboot_discover_gpt_tables(la->la_device, 1);
		if (p)
			*p = '.';
	}

	bootargs_buflen = os_sprintf(bootargs_buf, "%s", cmdline);
	bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
		" androidboot.quiescent=%d", is_quiescent_mode());

	/* bolt E1 - we are always 'orange' state. */
	bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
		" androidboot.verifiedbootstate=orange");

	bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
		" androidboot.btmacaddr=%s", get_btmacaddr());

	if (boot_path == BOOTPATH_LEGACY) {
		ptn = fastboot_flash_find_ptn("system");
		if (ptn)
			bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
				" android_blkdev=PARTUUID=%s", ptn->uuid);
		else
			os_printf("PARTUUID for system failed. Skipping android_blkdev\n");
	}

	os_sprintf(dt_add_cmd, "dt add node / firmware");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add node /firmware android");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /firmware/android compatible s 'android,firmware'");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /firmware/android serialno s '%s'", get_serial_no());
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /firmware/android hardware s '%s'", get_hardware_name());
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /firmware/android bootreason s '%s'", aon_reset_string());
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /firmware/android wificountrycode s '00'");
	bolt_docommands(dt_add_cmd);

	// early-mount is available for treble devices onward which expose a vendor partition.  pre-treble
	// devices do not have a vendor partition.
	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (boot_path == BOOTPATH_LEGACY) {
		os_sprintf(dt_add_cmd, "%s.vendor", fb_flashdev_mode_str);
	} else {
		os_sprintf(dt_add_cmd, "%s.vendor_%s", fb_flashdev_mode_str, BOOT_SLOT_0_SUFFIX);
	}
	fd = bolt_open((char *)dt_add_cmd);
	if (fd >= 0) {
		bolt_close(fd);

		os_sprintf(dt_add_cmd, "dt add node /firmware/android fstab");
		bolt_docommands(dt_add_cmd);
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab compatible s 'android,fstab'");
		bolt_docommands(dt_add_cmd);
		os_sprintf(dt_add_cmd, "dt add node /firmware/android/fstab vendor");
		bolt_docommands(dt_add_cmd);
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor compatible s 'android,vendor'");
		bolt_docommands(dt_add_cmd);
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor dev s '/dev/block/platform/rdb/%s/by-name/vendor'",
			env_getenv("EMMC_DEVNAME"));
		bolt_docommands(dt_add_cmd);
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor mnt_flags s 'ro,barrier=1'");
		bolt_docommands(dt_add_cmd);
		// a|b system mode, we can only early-mount vendor since system is rootfs.  we also assume squashfs because that is
		// the model to support within our bound emmc sizes.
		if (boot_path != BOOTPATH_LEGACY) {
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor type s '%s'", get_vendor_image_type());
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor fsmgr_flags s 'wait,verify,slotselect'");
			bolt_docommands(dt_add_cmd);
		} else {
			// legacy system mode, we can early-mount both vendor and system.
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor type s 'ext4'");
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/vendor fsmgr_flags s %s",
#if defined(DROID_VERITY_n)
				"'wait'");
#else
				"'wait,verify'");
#endif
			bolt_docommands(dt_add_cmd);

			os_sprintf(dt_add_cmd, "dt add node /firmware/android/fstab system");
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/system compatible s 'android,system'");
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/system dev s '/dev/block/platform/rdb/%s/by-name/system'",
				env_getenv("EMMC_DEVNAME"));
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/system mnt_flags s 'ro,barrier=1'");
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/system type s 'ext4'");
			bolt_docommands(dt_add_cmd);
			os_sprintf(dt_add_cmd, "dt add prop /firmware/android/fstab/system fsmgr_flags s %s",
#if defined(DROID_VERITY_n)
				"'wait'");
#else
				"'wait,verify'");
#endif
			bolt_docommands(dt_add_cmd);
		}
	}

	if (boot_path != BOOTPATH_LEGACY) {
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android slot s '%s'",
			slot == 0 ? BOOT_SLOT_0_SUFFIX : BOOT_SLOT_1_SUFFIX);
		bolt_docommands(dt_add_cmd);
		// redundant setting until it gets deprecated.
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android slot_suffix s '_%s'",
			slot == 0 ? BOOT_SLOT_0_SUFFIX : BOOT_SLOT_1_SUFFIX);
		bolt_docommands(dt_add_cmd);

		os_sprintf(dt_add_cmd, "dt add prop /firmware/android veritymode s '%s'",
			is_dmverity_eio_mode(slot) ? "eio" : "enforcing");
		bolt_docommands(dt_add_cmd);
		os_printf("dm-verity bootmode: '%s'.\n", is_dmverity_eio_mode(slot) ? "eio" : "enforcing");

		if (boot_path == BOOTPATH_AB_SYSTEM && la) {
			char partname[64];
			os_sprintf(partname, "%s_%s",
				BOOT_SLOT_SYSTEM_PREFIX, slot == 0 ? BOOT_SLOT_0_SUFFIX : BOOT_SLOT_1_SUFFIX);
			ptn = fastboot_flash_find_ptn(partname);
			if (ptn != NULL) {
			        bootargs_buflen += os_sprintf(bootargs_buf + bootargs_buflen,
						       " root=/dev/dm-0 dm=\"system none ro,0 1 android-verity PARTUUID=%s\"", ptn->uuid);
			} else {
				os_printf("device '%s', PARTUUID for '%s' failed -- aborting boot.\n", la->la_device, partname);
				fastboot_flash_dump_ptn();
			}
		}
	} else {
#if !defined(DROID_VERITY_n)
		// TODO: add detection for corrupted verity block in non-AB mode.
		os_sprintf(dt_add_cmd, "dt add prop /firmware/android veritymode s '%s'", "enforcing");
		bolt_docommands(dt_add_cmd);
#endif
	}

	return bootargs_buflen;
}

/*  *********************************************************************
    *  imgload_internal(fsctx,ref,la,kernelsize)
    *
    *  Read an ELF file (main routine)
    *
    *  Input parameters:
    *      bootmode - boot mode to apply (legacy, a/b-system).
    *      slot - slot to be used when in a/b-system mode.
    *  	   fsctx - context
    *  	   ref   - open file handle
    *  	   la    - loader args
    *      kernelsize - returned size of kernel image when valid
    *
    *  Return value:
    *  	   0 if ok
    *  	   else error code
    ********************************************************************* */

static int imgload_internal(enum bootpath boot_path, int slot, fileio_ctx_t *fsctx,
			void *ref, bolt_loadargs_t *la, int32_t *kernelsize)
{
	struct img_hdr hdr;
	int n, m;
	uint32_t kernel_offset;
	uint32_t ramdisk_offset;
	char magic[9];
	char dt_add_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */
	char bootargs_buf[BOOT_ARGS_SIZE];
	int bootargs_len;
	int skip_ramfs = (boot_path == BOOTPATH_AB_SYSTEM) ? 1 : 0;

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

	if (skip_ramfs) {
		os_printf("\nSkipping ramdisk loading.\n");
	} else {
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
	}

	/* Add kernel command line to DT only if there was none specified */
	if ((hdr.cmdline != NULL) && (la->la_options == NULL)) {
		DLOG("Adding kernel command line to DT\n");

		bootargs_len = gen_bootargs(la, bootargs_buf, (const char *) hdr.cmdline, boot_path, slot);
		if (skip_ramfs) {
			bootargs_len += os_sprintf(bootargs_buf + bootargs_len, " skip_initramfs");
		}
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

static void boot_path_set_env(enum bootpath boot_path)
{
	char buffer[40];

	os_sprintf(buffer, "setenv A_BOOT_PATH %s", bootpath_str[boot_path]);
	bolt_docommands(buffer);
}

static void boot_quiescent_set_env(int quiescent)
{
	char buffer[40];

	os_sprintf(buffer, "setenv A_QUIESCENT %d", (quiescent > 0) ? 1 : 0);
	bolt_docommands(buffer);
}

static void boot_dmverity_eio_set_env(int dmveio)
{
	char buffer[40];

	os_sprintf(buffer, "setenv A_DMVERITY_EIO %d", (dmveio > 0) ? 1 : 0);
	bolt_docommands(buffer);
}

static struct eio_boot eio_commander;
static enum bootpath select_boot_path(int *slot, int selected)
{
	int fd=-1;
	char *fb_flashdev_mode_str;
	char flash_devname[20];
	struct eio_boot *eio = &eio_commander;
	int ret;

	os_memset(eio, 0, sizeof(*eio));

	fb_flashdev_mode_str = env_getenv("FB_DEVICE_TYPE");
	if (!fb_flashdev_mode_str) {
		os_printf("FB_DEVICE_TYPE env var is not defined. Can't read boot commander.\n");
		goto ret_legacy;
	}

	os_sprintf(flash_devname, "%s.%s", fb_flashdev_mode_str, BOOT_SLOT_COMMANDER);
	DLOG("Lookup commander @'%s'...\n", flash_devname);

	fd = bolt_open((char *)flash_devname);
	if (fd < 0) {
		os_printf("Error opening %s. Can't read boot commander: %d\n",
				flash_devname, fd);
		goto ret_legacy;
	}

	ret = bolt_readblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
	if (ret != sizeof(struct eio_boot)) {
		os_printf("Error reading %s. Can't read commander block: %d\n",
				flash_devname, ret);
		goto ret_legacy;
	}
	DLOG("Read %d bytes from commander, magic %x (%s, sel: %d). \n",
		ret, eio->magic, (eio->magic == EIO_BOOT_MAGIC)?"good":"BAD", selected);

	if (slot != NULL) {
		*slot = -1;
		if (selected) {
			/* this is a re-entry in this function for the purpose of booting up, we do not apply
			 * the slot selection in this case, it must be the one we picked prior.
			 */
			*slot = eio->current;
			goto ret_ab_system;
		}
		if (eio->magic == EIO_BOOT_MAGIC) {
			if (eio->current < EIO_BOOT_NUM_ALT_PART) {
				/* one time only switching of slot following application reboot into a new context,
				 * this happens following update of inactive boot partition content.
				 */
				if (eio->onboot != -1) {
					eio->current = eio->onboot;
					eio->onboot = -1;
					os_printf("Warning: on-boot new slot %d from application\n", eio->current);
				}
				if (eio->slot[eio->current].boot_ok) {
					*slot = eio->current;
					eio->slot[eio->current].boot_ok = 0; /* reset, let application mark it. */
					ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
					if (ret != sizeof(struct eio_boot)) {
						os_printf("Error writing %s. Can't reset commander block: %d\n",
							  flash_devname, ret);
					}
				} else if (eio->slot[eio->current].boot_fail) {
					*slot = (eio->current == 0) ? 1 : 0; /* switch slot. */
					eio->current = *slot;
					if (eio->slot[eio->current].boot_fail) {
						/* all slots are marked 'fail', give up... */
						*slot = -1;
						goto ret_ab_bl_recovery;
					}
					ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
					if (ret != sizeof(struct eio_boot)) {
						os_printf("Error writing %s. Can't swap current on commander block: %d\n",
							  flash_devname, ret);
					}
				} else {
					if (++eio->slot[eio->current].boot_try < EIO_BOOT_TRY_ATTEMPT) {
						*slot = eio->current;
					} else {
						*slot = eio->current; /* last chance. */
						eio->slot[eio->current].boot_fail = 1; /* on boot success, application reset. */
					}
					ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
					if (ret != sizeof(struct eio_boot)) {
						os_printf("Error writing %s. Can't update commander block: %d\n",
							  flash_devname, ret);
					}
				}
			} else {
				/* no valid slot marked.  enter fastboot mode for flashing. */
				goto ret_ab_bl_recovery;
			}
		} else {
			/* empty (or badly corrupted) commander, seed with default data. */
			os_memset(eio, 0, sizeof(*eio));
			eio->current = 0;
			os_sprintf(eio->slot[0].suffix, "%s", BOOT_SLOT_0_SUFFIX);
			os_sprintf(eio->slot[1].suffix, "%s", BOOT_SLOT_1_SUFFIX);
			eio->magic = EIO_BOOT_MAGIC;
			ret = bolt_writeblk(fd, 0, (unsigned char *)eio, sizeof(struct eio_boot));
			if (ret != sizeof(struct eio_boot)) {
				os_printf("Error writing %s. Can't write commander block: %d\n",
					  flash_devname, ret);
				goto ret_legacy;
			}
			*slot = 0;
		}
	}
	goto ret_ab_system;

ret_legacy:
	if (fd >= 0)
		bolt_close(fd);
	return BOOTPATH_LEGACY;

ret_ab_bl_recovery:
	if (fd >= 0)
		bolt_close(fd);
	DLOG("Booting AB-System Bootloader.\n");
	return BOOTPATH_AB_BOOTLOADER_RECOVERY;

ret_ab_system:
	if (fd >= 0)
		bolt_close(fd);
	if (slot != NULL) {
		DLOG("Booting AB-System Slot: %d. \n", *slot);
	}
	return BOOTPATH_AB_SYSTEM;
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
	enum bootpath boot_path = BOOTPATH_LEGACY;
	int slot = -1;
	char *boot_path_env = NULL;

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
		res = fastboot_discover_gpt_tables(la->la_device, 1);
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
	 * Check the boot path we need to apply...
	 *   - legacy mode: just continue booting the kernel+ramdisk image.
	 *   - a/b-system: figure out the proper slot to use, apply consequently,
	 *     fallback to legacy mode if we think we are not properly setup.
	 */
	boot_path_env = env_getenv("A_BOOT_PATH");
	if (boot_path_env != NULL) {
		if (!os_strncmp(boot_path_env, "legacy", os_strlen("legacy"))) {
			/* fall back to legacy. */
		} else if (!os_strncmp(boot_path_env, "ab_android", os_strlen("ab_android"))) {
			boot_path = select_boot_path(&slot, 1);
		} else if (!os_strncmp(boot_path_env, "ab_recovery", os_strlen("ab_recovery"))) {
			boot_path = select_boot_path(&slot, 1);
			if (boot_path != BOOTPATH_LEGACY) {
				boot_path = BOOTPATH_AB_SYSTEM_RECOVERY;
			}
		}
	}

	/*
	 * Open the boot device
	 */
	res = fs_open(fsctx, &ref, filename, FILE_MODE_READ);
	if (res != 0)
		goto uninit_fs;

	res = imgload_internal(boot_path, slot, fsctx, ref, la, &kernelsize);
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

const bolt_loader_t imgloader = {
	.name = "img",
	.loader = bolt_imgload,
	.flags = 0
};

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

int android_get_boot_partition(ui_cmdline_t *cmd,
		char *boot_partition, int *is_legacy_boot)
{
	int ret;
	uint32_t boot_reason;
	enum bootmode boot_mode;
	char *fb_transport_mode_str;
	char *fb_flashdev_mode_str;
	enum bootpath boot_path;
	int slot = -1;
	const char *bimage;
	char boot_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */

	/* Make sure partition is NULL to begin with */
	*boot_partition = '\0';

	/* Read and clear the boot reason register */
	boot_reason = boot_reason_reg_get();
	boot_reason_reg_set(0);

	if ((boot_reason & BOOT_REASON_MASK) == 'b') {
		boot_path = BOOTPATH_AB_BOOTLOADER_RECOVERY;
	} else {
		/* determine if this will be a legacy boot or a a|b update
		*  boot.
		*/
		boot_path = select_boot_path(&slot, 0);
	}

	DLOG("boot_path = %s; boot_reason = %u\n", bootpath_str[boot_path], boot_reason);

	boot_quiescent_set_env(boot_reason & BOOT_QUIESCENT_MASK);
	boot_dmverity_eio_set_env(boot_reason & BOOT_DMVERITY_EIO_MASK);

	/* Determine the boot mode based on boot reason register.
	 * Note that first letter of the reboot command is saved in
	 * the register. */
	if ((boot_reason & BOOT_REASON_MASK) == 'r') {
		os_printf("boot reason = recovery\n");
		boot_mode = BOOTMODE_RECOVERY;
	} else if (((boot_reason & BOOT_REASON_MASK) == 'b') ||
		   (boot_path == BOOTPATH_AB_BOOTLOADER_RECOVERY)) {
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
			return BOLT_ERR;
		}

		/* Call "android fastboot" because reboot reason is
		 * "bootloader" */
		os_sprintf(boot_cmd, "android fastboot -transport=%s -device=%s",
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
			return BOLT_ERR;
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

	switch (boot_path) {
		case BOOTPATH_AB_SYSTEM:
			if (boot_mode == BOOTMODE_RECOVERY) {
				boot_path = BOOTPATH_AB_SYSTEM_RECOVERY;
			}
			break;
		case BOOTPATH_LEGACY:
		default:
		{
			switch (boot_mode) {
			case BOOTMODE_RECOVERY:
				bimage = env_getenv(ENV_ANDROID_RECOVERY_IMG);
				if (bimage == NULL) {
					os_printf("Recovery image ('%s' env var) not defined in BOLT\n",
						  ENV_ANDROID_RECOVERY_IMG);
					return BOLT_ERR;
				}
				break;
			case BOOTMODE_ANDROID:
				bimage = env_getenv(ENV_ANDROID_BOOT_IMG);
				if (bimage == NULL) {
					os_printf("Boot image ('%s' env var) not defined in BOLT\n",
						  ENV_ANDROID_BOOT_IMG);
					return BOLT_ERR;
				}
				break;
			default:
				/* The 'boot_image' file is provided via user-command.
				 * Safety check to ensure boot_image!=NULL if we get here */
				if (bimage == NULL) {
					os_printf("Boot image not provided in command\n");
					return BOLT_ERR;
				}
			}
		}
	}

	os_printf("boot_path = %s, boot_mode = %d\n", bootpath_str[boot_path], boot_mode);

	if (boot_path == BOOTPATH_LEGACY) {
		*is_legacy_boot = 1;
		os_sprintf(boot_partition, "%s", bimage);
	} else {
		char *boot_dev;

		*is_legacy_boot = 0;

		/* pass information to second stage processing of the image loader
		 * about the boot_path to be applied.
		 */
		boot_dev = env_getenv("FB_DEVICE_TYPE");
		boot_path_set_env(boot_path);
		os_sprintf(boot_partition, "%s.boot_%s",
			boot_dev ? boot_dev : BOOT_SLOT_DEV_DEFAULT,
			slot == 0 ? BOOT_SLOT_0_SUFFIX : BOOT_SLOT_1_SUFFIX);
	}

	return BOLT_OK;
}

int android_override_bootargs(char* bootargs, char *bootargs_buf)
{
	int bootargs_len;

	os_printf("Override cmdline from boot.img...\n");
	bootargs_len = gen_bootargs(NULL, bootargs_buf, bootargs, BOOTPATH_LEGACY, -1);

	/* Safety check in case the final bootargs is larger than what
	 * is allowed based on the boot.img header struct */
	 if (bootargs_len > BOOT_ARGS_SIZE) {
		os_printf("Invalid boot cmdline length (%d)\n", bootargs_len);
		os_printf("boot cmdline: %s\n", bootargs_buf);
		return BOLT_ERR;
	}

	return BOLT_OK;
}

int android_boot_addloader(void)
{
	os_printf("Adding Android img loader to BOLT\n");
	return boot_addloader(&imgloader);
}

#if defined(BCHP_WKTMR_REG_START)
#define BCHP_WKTMR_COUNTER_REG_OFFSET     0x4
#define DEFAULT_ANDROID_DATE_UTC_SEC  (1420070400) /* Jan-01-2015 00:00 */

/*  *********************************************************************
    *  android_wktmr_adjust(cmd, argc, argv[])
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
void android_wktmr_adjust(void)
{
	volatile uint32_t *counter = (volatile uint32_t *)
		REG_ADDR(BCHP_WKTMR_REG_START + BCHP_WKTMR_COUNTER_REG_OFFSET);
	uint32_t restart_time = AON_REG(AON_REG_ANDROID_RESTART_TIME);
	uint32_t restart_time_n = AON_REG(AON_REG_ANDROID_RESTART_TIME_N);

	/* Do not adjust if WKTMR further in time than the default time */
	if (*counter < DEFAULT_ANDROID_DATE_UTC_SEC) {
		/* If time of reboot is saved, add it to current wktmr counter
		 * to get the current time. */
		if ((restart_time == ~restart_time_n) &&
			(restart_time > DEFAULT_ANDROID_DATE_UTC_SEC)) {
			os_printf("Setting WKTMR ahead by %u sec (UTC)\n",
				restart_time);
			*counter += restart_time;
		} else {
			os_printf("Setting WKTMR to %u sec (UTC)\n",
				DEFAULT_ANDROID_DATE_UTC_SEC);
			*counter = DEFAULT_ANDROID_DATE_UTC_SEC;
		}
	}
}
#endif


#define BCHP_TIMER_WDTIMEOUT_OFFSET	0x28
#define BCHP_TIMER_WDTCMD_OFFSET	0x2c
#define WDT_RATE			27000000
#define WDT_START1			0xff00
#define WDT_START2			0x00ff
#define WDT_TIMEOUT_ENVAR		"WDT_TIMEOUT"

/* Start watchdog timer with the user-specified timeout */
void android_start_wdt(void)
{
	volatile uint32_t *wdt_timeout = (volatile uint32_t *)
		REG_ADDR(BCHP_TIMER_REG_START + BCHP_TIMER_WDTIMEOUT_OFFSET);
	volatile uint32_t *wdt_cmd = (volatile uint32_t *)
		REG_ADDR(BCHP_TIMER_REG_START + BCHP_TIMER_WDTCMD_OFFSET);
	char *timeout_str;
	int timeout;

	timeout_str = env_getenv(WDT_TIMEOUT_ENVAR);
	if (!timeout_str) {
		os_printf("'%s' env var not set. WDT not started\n",
			  WDT_TIMEOUT_ENVAR);
		return;
	}

	timeout = os_atoi(timeout_str);
	if (timeout <= 0) {
		os_printf("'%s' (%d) must be > 0. WDT not started\n",
			  WDT_TIMEOUT_ENVAR, timeout);
		return;
	}

	os_printf("Starting WDT with timeout of %d seconds\n", timeout);

	/* Program watchdog timeout */
	*wdt_timeout = timeout * WDT_RATE;

	/* Start watchdog timer */
	*wdt_cmd = WDT_START1;
	*wdt_cmd = WDT_START2;
}
