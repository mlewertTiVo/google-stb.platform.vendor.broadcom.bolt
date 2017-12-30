/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides a means to get the current voltage and temperature.
 *  This function is published as a part of the BSU interface. This
 *  provides low leverl Bolt functions to a application loaded by Bolt.
 *
***************************************************************************/

#include <avs_bsu.h>
#include <board.h>

#if !defined(ENABLE_AVS_INIT)

void avs_ssbl_init(unsigned int pmap_id)
{
}

void avs_do_reset_on_overtemp(int en)
{
}

int avs_get_data(unsigned *voltage,
                     signed *temperature,
                     bool *firmware_running)
{
	if (voltage)
		*voltage = 0;

	if (temperature)
		*temperature = 0;

	if (firmware_running)
		*firmware_running = false;

	return 1;
}

void avs_info_print()
{
}

#else /* ENABLE_AVS_INIT */

#include <avs_fw_interface.h>
#include <avs_regs.h>
#include <bchp_aon_ctrl.h>
#include <bchp_avs_cpu_data_mem.h>
#include <bchp_common.h>
#include <board_init.h>
#include <common.h>
#include <lib_printf.h>

#include <stdbool.h>

#ifdef BCHP_AVS_TMON_REG_START
#include <bchp_avs_tmon.h>
#else
#include <bchp_avs_hw_mntr.h>
#endif

static bool fsbl_enabled_overtemp = false;

/*********************************************************************
 * check_overtemp_was_enabled()
 *
 * NOTE: This function should be called before the first reference to
 *       'fsbl_enabled_overtemp'. Calling this multiple times does not
 *       penalize you a lot, but there is no reason to abuse it.
 *
 * 'fsbl_enabled_overtemp' reflects whether the overtemp feature was
 * enabled by FSBL, which may be different from SSBL if they are not
 * from the same build.
 *
 * If the overtemp feature is not enabled by FSBL, the overtemp reset
 * should not be enabled in SSBL. If enabled only in SSBL, the feature
 * gets disabled on S3 warm boot, resulting in inconsistent behaviors.
 *
 * It is considered that the overtemp feature was enabled by FSBL if:
 * - 'overtemp' is a reset reason, or
 *   (The feature was enabled. Or, the current reset could not occur.)
 * - the overtemp reset has already been enabled.
 *   (FSBL is the only suspect. No one else does prior to this point.)
 ********************************************************************* */
static void check_overtemp_was_enabled(void)
{
	static bool has_been_checked = false;

	if (has_been_checked)
		return;

	if (board_init_reset_history_value() &
		BCHP_AON_CTRL_RESET_HISTORY_overtemp_reset_MASK) {
		/* One of reset causes was overtemp. It is assumed
		 * that FSBL enables the overtemp feature.
		 */
		fsbl_enabled_overtemp = true;
	} else {
		/* check whether FSBL enabled the feature */
		const uint32_t regval =
#ifdef BCHP_AVS_TMON_TEMPERATURE_RESET_THRESHOLD
			BDEV_RD_F(AVS_TMON_ENABLE_OVER_TEMPERATURE_RESET,
				enable);
#else
			BDEV_RD_F(AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE,
				reset_enable);
#endif
		fsbl_enabled_overtemp = (regval != 0);
	}

	has_been_checked = true;
}

/*********************************************************************
 * avs_ssbl_init()
 *
 * The PMap of the selected board type gets applied.
 *
 * Input parameter:
 *  pmap_id: PMap ID to be applied
 ********************************************************************* */
void avs_ssbl_init(unsigned int pmap_id)
{
#ifdef DVFS_SUPPORT
	dvfs_init_board_pmap(pmap_id);
#endif
}


/*********************************************************************
 *  avs_do_reset_on_overtemp()
 *
 * Choose if an overtemp event will cause a hardware reboot or not.
 *
 *  Input parameters:
 *	en: 1=enable reboot on overtemp, 0=disable reboot.
 *
 *  Return value:
 *	nothing
 ********************************************************************* */
void avs_do_reset_on_overtemp(int en)
{
	/* make sure that 'fsbl_enabled_overtemp' is valid before using it */
	check_overtemp_was_enabled();

	if (fsbl_enabled_overtemp)
#ifdef BCHP_AVS_TMON_TEMPERATURE_RESET_THRESHOLD
		BDEV_WR_F(AVS_TMON_ENABLE_OVER_TEMPERATURE_RESET,
			enable, en);
#else
		BDEV_WR_F(AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE,
			reset_enable, en);
#endif
}

/* The AVS FW stores current data in the first part of its shared memory.
 * This reads the current data into the supplied memory.
 */
static void get_avs_fw_results(struct at_runtime *results)
{
	uint32_t *p = (uint32_t *)results;
	unsigned i;
	for (i = 0; i < sizeof(*results) / sizeof(*p); i++)
		p[i] =
		    BDEV_RD(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE +
				(i * 4));
}

/* Because we may have started up without AVS firmware running, we need to
 * provide data in both situations.  If the firmware is running then it is
 * updating the current data in the above locations.  If it is not running
 * (i.e. above locations are always zero) then get the data ourselves.
 */
int avs_get_data(unsigned int *voltage,
		     signed int *temperature,
		     bool *firmware_running)
{
	struct at_runtime results;
	unsigned int t_voltage;
	signed int t_temperature;
	bool t_fw_running = true; /* assume its running */

	get_avs_fw_results(&results); /* ask for the current firmware status */

	t_voltage = results.voltage0;
	t_temperature = results.temperature0;

	/* If the firmware is not running for some reason then these values
	 * will be zero.  Use this to determine if FW is indeed running.
	 * If FW is not running, then do the measurements manually.
	 */
	if (!t_voltage) {
		uint32_t v_reg, t_reg;

		v_reg = BDEV_RD(BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS) &
			BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS_data_MASK;
#ifdef AVS_TMON_EXISTS
		t_reg = (BDEV_RD(BCHP_AVS_TMON_TEMPERATURE_MEASUREMENT_STATUS) &
			BCHP_AVS_TMON_TEMPERATURE_MEASUREMENT_STATUS_data_MASK) >> 1;
#else
		t_reg = BDEV_RD(BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS) &
			BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS_data_MASK;
#endif
		if (AVS_USE_NEW_CONVERSION_FORMULAS) {
			/* formula is: voltage=((v/1024)*880/0.7) */
			t_voltage = (v_reg * 8800U) / 7168U;
			/* formula is: temperature=410.04-(t*0.48705) */
			t_temperature = (4100400 - (signed)(t_reg * 4870)) / 10;
		} else {
			/* formula is: voltage=((v/1024)*877.9/0.7) */
			t_voltage = (v_reg * 8779U) / 7168U;
			/* formula is: temperature=(854.8-t)/2.069 */
			t_temperature = 1000 * (854800 -
				(signed)(t_reg * 1000)) / 2069;
		}
		t_fw_running = false;
	}

	if (voltage)
		*voltage = t_voltage;
	if (temperature)
		*temperature = t_temperature;
	if (firmware_running)
		*firmware_running = t_fw_running;

	return 0;
}

/* Some simple marcos to assist in printing pseudo floating point values */
/* This assumes that items are in units of 1000 (milli) */
char sign(signed f) { return f<0?'-':'+'; }
unsigned mantissa(signed f) { if (f<0) f=-f; return (unsigned)(f/1000); }
unsigned fraction(signed f) { if (f<0) f=-f; return (unsigned)(f - f/1000*1000); }

/* Print out the current status.  Output will be:
 * AVS: STB: V=0.925V, T=+42.842C, PV=0.850V, MV=0.917V, FW=30323478 [0.2.4.x]
 *   -or-
 * AVS: V=1.14V, T=+42.842C (no AVS FW)
 * Dual AVS monitor devices will print DCD voltage and temperature too.
 */
void avs_info_print(void)
{
	unsigned voltage = 1;
	signed temperature = 2;
	bool firmware_running = false;

	avs_get_data(&voltage, &temperature, &firmware_running);

	if (firmware_running) {
		struct at_runtime results;
		unsigned revision;
		const char *revision_format;
		const char *new_format = ", FW=[%d.%d.%d.%d]\n";
		const char *old_format = ", FW=[%c.%c.%c.%c]\n";

		get_avs_fw_results(&results);

		xprintf("AVS: STB: V=%d.%03dV, T=%c%d.%03dC, PV=%d.%03dV, MV=%d.%03dV",
			mantissa(results.voltage0), fraction(results.voltage0),
			sign(results.temperature0),
			mantissa(results.temperature0),
			fraction(results.temperature0),
			mantissa(results.PV0), fraction(results.PV0),
			mantissa(results.MV0), fraction(results.MV0)
		);

		revision = results.revision;

		if ((revision >> 24) < '0')
			revision_format = new_format;
		else
			revision_format = old_format;

		xprintf(revision_format,
			(revision >> 24) & 0xFF,
			(revision >> 16) & 0xFF,
			(revision >>  8) & 0xFF,
			(revision >>  0) & 0xFF);

#if defined(AVS_DUAL_MONITORS) || defined(AVS_DUAL_DOMAINS)
		struct board_type *b = board_thisboard();

		if (!b || (AVS_DOMAINS(b->avs) == 2)) {
			xprintf("AVS: %s: V=%d.%03dV, T=%c%d.%03dC, "
				"PV=%d.%03dV, MV=%d.%03dV\n",
				"CPU",
				mantissa(results.voltage1),
				fraction(results.voltage1),
				sign(results.temperature1),
				mantissa(results.temperature1),
				fraction(results.temperature1),
				mantissa(results.PV1), fraction(results.PV1),
				mantissa(results.MV1), fraction(results.MV1)
			);
		}
#endif
	}
	else
	{
		xprintf("AVS: V=%d.%03dV, T=%c%d.%03dC (no AVS FW)\n",
			mantissa(voltage), fraction(voltage),
			sign(temperature),
			mantissa(temperature),
			fraction(temperature)
		);
	}
}

#endif /* ENABLE_AVS_INIT */
