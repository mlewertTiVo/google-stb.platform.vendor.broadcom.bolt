/*****************************************************************************
*
* Copyright 2014 -2015 Broadcom Corporation.  All rights reserved.
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
#include <env_subr.h>
#include "android_bsu.h"

const struct bsu_cmd {
	char *command;
	int (*func)(ui_cmdline_t *, int argc, char *argv[]);
	char *help;
	char *usage;
	char *switches;
} android_cmd[] = {
	{	.command = "android boot",
		.func = android_boot,
		.help = "Load an Android boot image into memory and boot it",
		.usage =
			"android boot [options] [arg]\n\n"
			"This command loads and boots and Android image. The\n"
			"optional arg can be used to override any builtin\n"
			"kernel command line arguments.\n\n"
			"The file or partition to use for booting can be\n"
			"specified with environment variable or explicitly\n"
			"in the command with -i option.\n"
			"The format can be:\n"
			"   dev:filename (e.g. usbdisk0:boot.img)\n"
			"   dev (e.g. flash1.partition)\n"
			"If -rawfs option is enabled, then it must be:\n"
			"   usbdisk0:partition  (e.g. usbdisk0:boot)\n\n"
			"The following environment variables can be used:\n"
			"  ANDROID_BOOT_IMG - for normal boot image\n"
			"  ANDROID_RECOVERY_IMG - for recovery boot image",
		.switches =
			"-rawfs;Load the file from an unformatted file system|"
#if CFG_TRUSTZONE_MON
			"-tee; Android trusted boot with BL31/Trusty OS|"
			"-32; Trusty is a 32-bit binary (default is 64-bit)|"
#endif
	},
	{	.command = "android fastboot",
		.func = android_fastboot,
		.help = "Put target platform in Android fastboot mode",
		.usage =
			"android fastboot [options]\n\n"
			"This command puts target platform in fastboot mode\n"
			"for updating the system images on target platform\n"
			"using Android fastboot protocol.\n"
			"In fastboot mode, the target platform processes\n"
			"fastboot command from the host.\n\n"
			"The fastboot command can be sent to target via either\n"
			"TCP or USB depending on the 'transport' option.\n"
			"The device to store the system image can either be\n"
			"USB drive or eMMC flash device depending on the\n"
			"'device' parameter. The 'device' parameter must be\n"
			"a valid BOLT flash or disk device.\n"
			"E.g. for USB drive: -device=usbdisk0\n"
			"     for eMMC flash: -device=emmcflash0 or -device=flash0",
		.switches =
			"-transport=*; Use tcp or usb (default)|"
			"-device=*; Use BOLT device full name|"
	},
};

struct bsu_api *bsuapi;

/*  *********************************************************************
    *  android_bsu_entry(unused,param1,param2,param3)
    *
    *  Android BSU entry point
    *
    *  Input parameters:
    *      unused
    *      param1
    *      param2
    *      param3 - BSU API handle
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */

void android_bsu_entry(unsigned long unused, unsigned long param1,
		       unsigned long param2, unsigned long param3)
{
	unsigned int i;
	int ret;
	char buffer[80];

	bsuapi = (struct bsu_api *)param3;
	if (bsuapi == NULL)
		return;

	if ((bsuapi->xfd_signature != BSU_SIGNATURE) ||
	    (bsuapi->xfd_api_version < BSU_API_VERSION))
		return;

	os_printf("Adding Android commands to BOLT\n");
	for (i = 0; i < (sizeof(android_cmd) / sizeof(*android_cmd)); i++) {
		ret = cmd_addcmd(android_cmd[i].command, android_cmd[i].func,
				 NULL, android_cmd[i].help,
				 android_cmd[i].usage, android_cmd[i].switches);
		if (ret)
			os_printf("Failed to add '%s' command (%d)\n",
				android_cmd[i].command, ret);
	}
	os_printf("Done loading Android BSU\n");

	/* DROID_PRODUCT is defined in Makefile based on an Android build
	 * environment variable. If the required Android build environment
	 * variable does not exist at compile time, then PRODUCTNAME will
	 * be an empty string */
#ifdef DROID_PRODUCT
	os_printf("Autogen PRODUCTNAME = %s\n", DROID_PRODUCT);
	os_sprintf(buffer, "%s", DROID_PRODUCT);
	env_setenv("PRODUCTNAME", buffer, ENV_FLG_BUILTIN);
#endif /* DROID_PRODUCT */

	os_printf("Validating GPT (may update)...\n");
	fastboot_process_canned_gpt();
}
