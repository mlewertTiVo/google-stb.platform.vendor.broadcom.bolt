/*****************************************************************************
*
* Copyright 2014 - 2016 Broadcom Corporation.  All rights reserved.
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

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_physio.h"

#include "error.h"
#include "env_subr.h"
#include "timer.h"
#include "bchp_common.h"
#include "bchp_gio.h"
#include "bchp_gio_aon.h"
#include "ui_command.h"

struct gio_regset_s {
	uint32_t oden;
	uint32_t data;
	uint32_t iodir;
	uint32_t ec;
	uint32_t ei;
	uint32_t mask;
	uint32_t level;
	uint32_t stat;
} __packed;

static struct gio_regset_s gio_regset[] = {
#if defined(CONFIG_BCM7439B0) || \
	defined(CONFIG_BCM7250B0) || \
	defined(CONFIG_BCM7268B0) || \
	defined(CONFIG_BCM7271B0) || \
	defined(CONFIG_BCM7260A0)
	{
		.oden = BCHP_GIO_ODEN_LO,	/* GPIO[31:0] */
		.data = BCHP_GIO_DATA_LO,
		.iodir = BCHP_GIO_IODIR_LO,
		.ec = BCHP_GIO_EC_LO,
		.ei = BCHP_GIO_EI_LO,
		.mask = BCHP_GIO_MASK_LO,
		.level = BCHP_GIO_LEVEL_LO,
		.stat = BCHP_GIO_STAT_LO,
	},
	{
		.oden = BCHP_GIO_ODEN_HI,	/* GPIO[63:32] */
		.data = BCHP_GIO_DATA_HI,
		.iodir = BCHP_GIO_IODIR_HI,
		.ec = BCHP_GIO_EC_HI,
		.ei = BCHP_GIO_EI_HI,
		.mask = BCHP_GIO_MASK_HI,
		.level = BCHP_GIO_LEVEL_HI,
		.stat = BCHP_GIO_STAT_HI,
	},
	{
		.oden = BCHP_GIO_ODEN_EXT_HI,	/* GPIO[95:64] || GPIO[82:64] */
		.data = BCHP_GIO_DATA_EXT_HI,
		.iodir = BCHP_GIO_IODIR_EXT_HI,
		.ec = BCHP_GIO_EC_EXT_HI,
		.ei = BCHP_GIO_EI_EXT_HI,
		.mask = BCHP_GIO_MASK_EXT_HI,
		.level = BCHP_GIO_LEVEL_EXT_HI,
		.stat = BCHP_GIO_STAT_EXT_HI,
	},
#if defined(CONFIG_BCM7439B0) || defined(CONFIG_BCM7250B0)
	{
		.oden = BCHP_GIO_ODEN_EXT2,	/* GPIO[111:96] */
		.data = BCHP_GIO_DATA_EXT2,
		.iodir = BCHP_GIO_IODIR_EXT2,
		.ec = BCHP_GIO_EC_EXT2,
		.ei = BCHP_GIO_EI_EXT2,
		.mask = BCHP_GIO_MASK_EXT2,
		.level = BCHP_GIO_LEVEL_EXT2,
		.stat = BCHP_GIO_STAT_EXT2,
	},
#endif
#endif
};

#if defined(CONFIG_BCM7439B0) || \
	defined(CONFIG_BCM7250B0) || \
	defined(CONFIG_BCM7268B0) || \
	defined(CONFIG_BCM7271B0) || \
	defined(CONFIG_BCM7260A0)
static struct gio_regset_s gio_aon_regset[] = {
	{
		/* AON_GPIO[17:0] for CONFIG_BCM7439B0 */
		/* AON_GPIO[20:0] for CONFIG_BCM7250B0 */
		/* AON_GPIO[27:0] for CONFIG_BCM7268B0 */
		/* AON_GPIO[27:0] for CONFIG_BCM7271B0 */
		/* AON_GPIO[27:0] for CONFIG_BCM7260A0 */
		.oden = BCHP_GIO_AON_ODEN_LO,
		.data = BCHP_GIO_AON_DATA_LO,
		.iodir = BCHP_GIO_AON_IODIR_LO,
		.ec = BCHP_GIO_AON_EC_LO,
		.ei = BCHP_GIO_AON_EI_LO,
		.mask = BCHP_GIO_AON_MASK_LO,
		.level = BCHP_GIO_AON_LEVEL_LO,
		.stat = BCHP_GIO_AON_STAT_LO,
	},
};
#endif

/*  *********************************************************************
	* int gio_offset_to_regset(void)
	*
	* Helper function for button_name() to determine
	* which gpio register and mask should be used for the given gpio pin.
	*
	* WARNING: bit offset ('boffs') may *NOT* reflect
	* the actual gpio pin number! Check your chip datasheets as
	* well to really know the mapping and not just assume that
	* boffs == gpio pin.
	*
	*  This is chip specific !! Refer to the chip datasheet for mapping
	*
	*  Input parameters:
	*     int boffs: bit offset to the gpio pin
	*
	*  Return value:
	*     NULL - if no match bit offset are found for the given gpio pin
	*     pointer to struct gio_regset_s[]
	*     mask - bit mask for the given gpio pin
	********************************************************************* */

struct gio_regset_s *gio_offset_to_regset(int boffs, uint32_t *mask)
{
	if ((boffs >= 0) && (boffs <= 31)) {
		*mask = (1 << boffs);
		return &(gio_regset[0]);
	}

	if ((boffs >= 32) && (boffs <= 63)) {
		*mask = (1 << (boffs - 32));
		return &(gio_regset[1]);
	}

#if defined(CONFIG_BCM7439B0) || defined(CONFIG_BCM7250B0)
	if ((boffs >= 64) && (boffs <= 95)) {
		*mask = (1 << (boffs - 64));
		return &(gio_regset[2]);
	}

	if ((boffs >= 96) && (boffs <= 111)) {
		*mask = (1 << (boffs - 96));
		return &(gio_regset[3]);
	}
#elif defined(CONFIG_BCM7260A0) || defined(CONFIG_BCM7268B0) || defined(CONFIG_BCM7271B0)
	if ((boffs >= 64) && (boffs <= 82)) {
		*mask = (1 << (boffs - 64));
		return &(gio_regset[2]);
	}
#endif

	return NULL;
}

struct gio_regset_s *gio_aon_offset_to_regset(int boffs, uint32_t *mask)
{
#if defined(CONFIG_BCM7439B0)
	if ((boffs >= 0) && (boffs <= 17)) {
		*mask = (1 << boffs);
		return &(gio_aon_regset[0]);
	}
#elif defined(CONFIG_BCM7250B0)
	if ((boffs >= 0) && (boffs <= 20)) {
		*mask = (1 << boffs);
		return &(gio_aon_regset[0]);
	}
#elif defined(CONFIG_BCM7260A0) || defined(CONFIG_BCM7268B0) || defined(CONFIG_BCM7271B0)
	if ((boffs >= 0) && (boffs <= 27)) {
		*mask = (1 << boffs);
		return &(gio_aon_regset[0]);
	}
#endif

	return NULL;
}

/*  *********************************************************************
	*  int cmd_gpio_btn_state(char* bank, int pin)
	*
	*  Helper function for checking the state of the gpio button
	*
	*  Input parameters:
	*     char* bank: the bank which gpio pin resides on
	*     int pin: the pin to which the gpio button is connected
	*
	*  Return value:
	*     0 - button is NOT pressed
	*     1 - button is pressed
	********************************************************************* */

int cmd_gpio_btn_state(char* bank, int pin)
{
	uint32_t v,mask;
	struct gio_regset_s *s;
	int res = BOLT_ERR_INV_PARAM;

	if (!strcmp(bank, "upg_gio_aon"))
		s = gio_aon_offset_to_regset(pin, &mask);
	else if (!strcmp(bank, "upg_gio"))
		s = gio_offset_to_regset(pin, &mask);
	else {
		ui_showerror(res, "invalid gpio bank \"%s\"", bank);
		return res;
	}

	if (!s) {
		ui_showerror(res, "gpio pin offset mapping are not defined for \"%s %d\"", bank, pin);
		return res;
	}

	v = DEV_RD(BPHYSADDR(s->data)) & mask;

	/* gpio button is low active */
	if (!v) {
		/* wait 100ms for debouncing */
		bolt_msleep(100);
		v = DEV_RD(BPHYSADDR(s->data)) & mask;
		if (!v)
			return 1;
	}

	return 0;
}
