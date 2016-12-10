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
#include "android_bsu.h"

#define BL31_LOAD_ADDR	0x7FB00000
#define BL31_PART_SIZE	0x200000
#define BL31_PART_NAME "flash0.bl31"

#define TRUSTY_LOAD_ADDR 0x7F000000
#define TRUSTY_PART_SIZE 0x800000
#define TRUSTY_PART_NAME "flash0.tee"

/*
 * Patches the Linux kernel device tree with the Trusty specific
 * driver device nodes. Nodes and properties are added in reverse
 * order so that the show up in the correct order in the DT.
 */
static int patch_kernel_dt(void)
{
	char dt_add_cmd[BOOT_ARGS_SIZE+64];

	DLOG("Adding trusty nodes to Linux DT\n");

	/* Trusty node */
	os_sprintf(dt_add_cmd, "dt add node / trusty");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /trusty/ #size-cells i 2");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /trusty/ #address-cells i 2");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd, "dt add prop /trusty/ ranges b");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd,
	   "dt add prop /trusty/ compatible s \"android,trusty-smc-v1\"");
	bolt_docommands(dt_add_cmd);

	/* LOG subnode */
	os_sprintf(dt_add_cmd, "dt add node /trusty/ log");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd,
	   "dt add prop /trusty/log/ compatible s \"android,trusty-log-v1\"");
	bolt_docommands(dt_add_cmd);

	/* VIRTIO subnode */
	os_sprintf(dt_add_cmd, "dt add node /trusty/ virtio");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd,
	   "dt add prop /trusty/virtio/ compatible s \"android,trusty-virtio-v1\"");
	bolt_docommands(dt_add_cmd);

	/* FIQ subnode */
	os_sprintf(dt_add_cmd, "dt add node /trusty/ fiq");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd,
	   "dt add prop /trusty/fiq compatible s \"android,trusty-fiq-v1\"");
	bolt_docommands(dt_add_cmd);

	/* IRQ subnode */
	os_sprintf(dt_add_cmd, "dt add node /trusty/ irq");
	bolt_docommands(dt_add_cmd);
	os_sprintf(dt_add_cmd,
	   "dt add prop /trusty/irq compatible s \"android,trusty-irq-v1\"");
	bolt_docommands(dt_add_cmd);

	/*
	 * Add "dont-use-secure-irq" property to the timer dt node. This
	 * prevent the Linux arm generic timer driver from using its secure
	 * line, which can then be used by trusty.
	 */
	os_sprintf(dt_add_cmd, "dt add prop /timer/ dont-use-secure-irq b");
	bolt_docommands(dt_add_cmd);

	return BOLT_OK;
}

/*
 * Load and boot BL31
 */
static int bl31_boot(void)
{
	char boot_cmd[BOOT_ARGS_SIZE+64];

	os_sprintf(boot_cmd, "boot -64 -el3 -raw -addr=%x %s",
		BL31_LOAD_ADDR, BL31_PART_NAME);

	DLOG("boot_cmd=%s\n", boot_cmd);
	bolt_docommands(boot_cmd);

	os_printf("Android trusted boot FAILED\n");
	return BOLT_ERR;
}

/*  *********************************************************************
    *  android_trusty_boot(cmd, argc, argv[])
    *
    *  Entry function to handle 'android trusty-boot' command
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
int android_trusty_boot(ui_cmdline_t *cmd, int argc, char *argv[])
{
	bolt_loadargs_t la;
	char boot_partition[BOOT_ARGS_SIZE];
	int rc = 0;
	int is_legacy_boot;

	/* Allocate and carve out memory for Trusty */
	/*
	 * TODO: Currently hardcoded to top of 2nd memory bank. But
	 * could reserve memory for trusty here in the future.
	 */

	/* Patch Linux DT with trusty driver device nodes */
	patch_kernel_dt();

	/* Load Trusty in memory and save it's entry point in SMM */
	os_memset(&la, 0, sizeof(la));
	la.la_flags = LOADFLG_SPECADDR | LOADFLG_SECURE;
	if (!cmd_sw_isset(cmd, "-32"))
		la.la_flags |= LOADFLG_APP64;
	la.la_address = TRUSTY_LOAD_ADDR;
	la.la_maxsize = TRUSTY_PART_SIZE;
	la.la_filesys = "raw";
	la.la_device = TRUSTY_PART_NAME;

	rc = bolt_load_program("raw", &la);
	if (rc < 0) {
		os_printf("Failed to load %s\n",
			  la.la_device);
		return BOLT_ERR;
	}
	tz_smm_set_params(&la);

	/*
	 * Load boot.img in memory and save the Linux entry point/FDT
	 * in SMM
	 */
	os_memset(&la, 0, sizeof(la));
	la.la_flags = LOADFLG_SPECADDR;
	la.la_address = BOOT_START_ADDRESS;
	la.la_maxsize = BOOT_AREA_SIZE;
	la.la_filesys = "raw";

	/* Get boot partition (legacy or a|b boot method) */
	rc = android_get_boot_partition(cmd, boot_partition, &is_legacy_boot);
	if ((rc == BOLT_OK) && !*boot_partition)
		return rc;
	else if (rc) {
		os_printf("Failed to get boot partition\n");
		return rc;
	}
	la.la_device = (char*)boot_partition;

	rc = bolt_load_program("img", &la);
	if (rc < 0) {
		os_printf("Failed to load %s\n",
			  la.la_device);
		return BOLT_ERR;
	}
	tz_smm_set_params(&la);

#if defined(BCHP_WKTMR_REG_START)
	/* Adjust the wake-up timer */
	android_wktmr_adjust();
#endif

	/*
	 * Boot bl31. It will launch with information about Trusty(BL32)
	 * and Linux(BL33) which it will use to initialize and launch those
	 * components.
	 */
	return bl31_boot();
}
