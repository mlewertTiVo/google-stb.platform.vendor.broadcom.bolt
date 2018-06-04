/*
 * Copyright 2014-current Broadcom Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <error.h>
#include <arch.h>
#include <bchp_common.h>
#include <lib_physio.h>
#include <bchp_aon_ctrl.h>
#include "android_bsu.h"
#include "android_types.h"

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
	int is_legacy_boot;
	char boot_cmd[BOOT_ARGS_SIZE+64]; /* extra 64 bytes for bolt cmd */
	char bootargs_buf[BOOT_ARGS_SIZE];
	char boot_partition[BOOT_ARGS_SIZE];

	if (argc > 1) {
		os_printf("Too many arguments, or an unquoted optional [arg]\n");
#if CFG_SPLASH
		splash_feedback(BOOT_SPLASH_FAILED);
#endif
		return BOLT_ERR_INV_PARAM;
	}

	ret = android_boot_addloader();
	if (ret != BOLT_OK) {
		os_printf("Failed to add Android img loader to BOLT\n");
#if CFG_SPLASH
		splash_feedback(BOOT_SPLASH_FAILED);
#endif
		return ret;
	}

#if CFG_TRUSTZONE_CMD
	/* Check if secure/Trusty boot requested */
	if (cmd_sw_isset(cmd, "-tee"))
		return android_trusty_boot(cmd, argc, argv);
#endif

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

	/* Get the boot image based on legacy or a|b system or command
	 * line switches */
	ret = android_get_boot_partition(cmd, boot_partition, &is_legacy_boot);
	if ((ret == BOLT_OK) && !*boot_partition) {
#if CFG_SPLASH
		splash_feedback(BOOT_SPLASH_FAILED);
#endif
		return ret;
	}
	if (ret)
		goto failed;

	len += os_sprintf(boot_cmd + len, " %s", boot_partition);
	if (is_legacy_boot) {
		int info;
		info = bolt_getdevinfo((char *)boot_partition);
		if (info < 0) {
			os_printf("%s: no such device, aborting.\n", boot_partition);
			goto failed;
		}

		/* If user provides the optional arg, then override the built-in
		 * kernel command line by passing argv[0] to 'boot' command. */
		if (argc == 1) {
			ret = android_override_bootargs(argv[0], bootargs_buf);
			if (ret != BOLT_OK)
				goto failed;

			/* Update the 'boot' command with all the arguments
			 * constructed from above */
			os_sprintf(boot_cmd + len, " '%s'", bootargs_buf);
		}
	}

#if defined(BCHP_WKTMR_REG_START)
	/* Adjust the wake-up timer */
	android_wktmr_adjust();
#endif
	android_start_wdt();

	DLOG("boot_cmd=%s\n", boot_cmd);

	bolt_docommands(boot_cmd);

failed:
	/* If we reach here then we have failed to boot */
	os_printf("Boot FAILED\n");

#if CFG_SPLASH
	splash_feedback(BOOT_SPLASH_FAILED);
#endif
	return BOLT_ERR;
}
