/***************************************************************************
 *     Copyright (c) 2012-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "board_init.h"
#include "board.h"
#include "bsp_config.h"
#include "ssbl-common.h"
#include "flash.h"
#include "splash-api.h"
#include "bchp_common.h"
#include "bchp_ebi.h"

static void turnoff_xor_cs0()
{
	/* disable mask_en, so that bolt partition is at block 0 */
	BDEV_WR_F(EBI_CS_CONFIG_0, mask_en, 0);
}


static void enable_nand_flash(int cs)
{
#if CFG_NAND_FLASH
	bolt_add_device(&nandflashdrv, cs, 0, NULL);
#endif
}

static int nand_boot_read_disturb(void)
{
	struct flash_info flashinfo;
	int sfd, ret;

	/* check for and handle read disturbance in BOLT */
	sfd = bolt_open("flash0.bolt");
	if (sfd < 0) {
		xprintf("cannot open flash0.bolt: %d\n", sfd);
		return sfd;
	}
	ret = bolt_ioctl(sfd, IOCTL_FLASH_HANDLE_PARTITION_READ_DISTURB,
			&flashinfo, sizeof(flashinfo), NULL, 0);
	if (ret)
		xprintf("cannot get flash info\n");

	bolt_close(sfd);

	return ret;
}

void enable_emmc_flash(void)
{
#if CFG_EMMC_FLASH
	uint32_t x;
	const sdio_params *params;
	int bus_width;
	struct emmc_registers regs[] = {
#ifdef BCHP_SDIO_0_HOST_REG_START
		{
			{ (void *)BCHP_SDIO_0_HOST_REG_START,
			  NULL,
			},
		  0,
		  "EMMC0:"
		},
#endif
#ifdef BCHP_SDIO_1_HOST_REG_START
		{
			{ (void *)BCHP_SDIO_1_HOST_REG_START,
			  (void *)BCHP_SDIO_1_BOOT_REG_START,
			},
			1,
			"EMMC1:"
		},
#endif
	};

	for (x = 0; x < (sizeof(regs) / sizeof(struct emmc_registers)); x++) {
		params = board_sdio(x);
		if (params) {
			int bootdev;

			emmc_init_pinmux(regs[x].id, params->type);
			/*
			 * If this controller has the boot control
			 * registers and we booted from eMMC, set the BOOT
			 * flag
			 */
			if (regs[x].reg_bases[EMMC_REG_BLK_BASE_BOOT]
				&& (board_bootmode() == BOOT_FROM_EMMC))
				bootdev = 1;
			else
				bootdev = 0;
			bus_width = 0;
			if (params->type == SDIO_TYPE_EMMC)
				bus_width = 8;
			if (params->type == SDIO_TYPE_EMMC_ON_SDPINS)
				bus_width = 4;
			if (bus_width)
				bolt_add_device(&emmcflashdrv, bootdev,
						bus_width, (void *)&regs[x]);
		}
	}
#endif
}

void board_init_flash(void)
{
	char *envmac;
	char macstr[18];
	norflash_probe_t fprobe;
	int sfd;
	unsigned int boot_mode;
	struct flash_info flashinfo;
	int flashprog_offset = 0;

	boot_mode = board_bootmode();

	/* turn off xor for CS0 NOR and NAND
	 * This way, bolt partition is at the block 0 and
	 * the all blocks are physically contiguous.
	 */
	turnoff_xor_cs0();

	if (boot_mode == BOOT_FROM_SPI) {
		bolt_add_device(&spiflashdrv, 0, 0, NULL);
	} else if (boot_mode == BOOT_FROM_NOR) {
		/* nor flash detected */
		memset(&fprobe, 0, sizeof(fprobe));
		fprobe.flash_phys = REG(BCHP_EBI_CS_BASE_0) &
		    BCHP_EBI_CS_BASE_0_base_addr_MASK;
		fprobe.flash_flags = FLASH_FLG_BUS16 | FLASH_FLG_DEV16;

		bolt_add_device(&norflashdrv, 0, 0, &fprobe);
	} else if (boot_mode == BOOT_FROM_NAND) {
		enable_nand_flash(0); /* CS0 */
	} else if (boot_mode != BOOT_FROM_EMMC) {
		xprintf("Error: Invalid Flash\n");
	}
	enable_emmc_flash();

	/* ----------------------------------------
	   Splash (Option)
	   ----------------------------------------
	*/
	splash_api_setenv();

	if (boot_mode == BOOT_FROM_NOR || boot_mode == BOOT_FROM_SPI) {
		enable_nand_flash(1); /* CS1 */
	}

	/* Perform final configuration, after registering all flashes */
	flash_configure_finalize();

	if (boot_mode == BOOT_FROM_NAND)
		nand_boot_read_disturb();

	/* Set macadr/nvram
	*/
	bolt_set_envdevice("flash0.nvram");
	bolt_set_macdevice("flash0.macadr");

	/* retreive mac address from flash
	*/
	sfd = bolt_open("flash0.macadr");
	if (sfd < 0)
		xprintf("cannot open flash0.macadr\n");

	if (bolt_ioctl(sfd, IOCTL_FLASH_GETPARTINFO,
		      &flashinfo, sizeof(flashinfo), NULL, 0) != 0) {
		xprintf("cannot get flash info\n");
	}

	flashprog_offset = flashinfo.flash_base;
	bolt_close(sfd);

	if (macaddr_flash_verify(flashprog_offset) == 0) {
		envmac = env_getenv("ETH0_HWADDR");
		if (!envmac) {
			macaddr_flash_get(BCM7038MAC, macstr, flashprog_offset);
			(void)env_setenv("ETH0_HWADDR",
				macstr, ENV_FLG_BUILTIN);
		}
	} else {
#if CFG_ENET
		err_msg("MAC ADDRESS MUST BE PROGRAMMED; use macprog command");
#else /* gentler message for MAC address not related to an actual
		physical device	used by BOLT, but just passed e.g. via
		Devicetree. */
		warn_msg("MAC ADDRESS NOT PROGRAMMED; use macprog command");
#endif /* CFG_ENET */
	}
}
