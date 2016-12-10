/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides a low level ability to enable an over-temperature reset.
 *  This also provides a means to check whether the last reset was due to
 *  an over temperature situation.  This also clears the condition.
 *
***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "../fsbl/fsbl.h"  /* for uart */

#include "bchp_common.h"
#include "bchp_aon_ctrl.h"
#ifdef BCHP_AVS_TMON_REG_START
#include "bchp_avs_tmon.h"
#else
#include "bchp_avs_hw_mntr.h"
#include "bchp_avs_ro_registers_0.h"
#endif
#include "avs_temp_reset.h"

/* Maximum supported temperature allowed */
#define MAX_TEMPERATURE 130 /* degrees C */

/* Allow this to be over-ridden with compile time define */
#ifdef AVS_MAX_TEMPERATURE
#undef MAX_TEMPERATURE
#define MAX_TEMPERATURE AVS_MAX_TEMPERATURE
#endif

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This enables a fail-safe temperature monitor.  If the temperature ever
 * reaches this value the part will reset.
 * This is really just a last ditch effort to save the part from burning up.
 * The part is guaranteed to run up to 125C so using 150C is just to
 * catch run-away temperature situations.
 *
 * The result of a reset due to over-temp can be read in:
 * "BCHP_AON_CTRL_RESET_HISTORY" with bit:
 * AON_CTRL :: RESET_HISTORY :: * overtemp_reset [13:13]
 * 'device' is unused for single avs.
 */
void avs_set_temp_threshold(unsigned int device, int en)
{
	uint32_t reg;

#ifdef BCHP_AVS_TMON_TEMPERATURE_RESET_THRESHOLD
	/* original: o_ADC_data=(410.04-T)/0.48705 */
	/* For T=130 reg=574.97176=>574=23Eh, 23Eh<<1=>47Ch */
	reg = (4100400-(MAX_TEMPERATURE*10000))/4870;
	/* The actual temperature is located in bits 10:1 (not 10:0) */
	BDEV_WR(BCHP_AVS_TMON_TEMPERATURE_RESET_THRESHOLD, reg << 1);
	BDEV_WR(BCHP_AVS_TMON_ENABLE_OVER_TEMPERATURE_RESET, en);
#else
	/* original: reg=854.8-2.069*T */
	reg = (854800 - (2069 * MAX_TEMPERATURE)) / 1000;
	BDEV_WR(BCHP_AVS_HW_MNTR_TEMPERATURE_THRESHOLD, reg);
	BDEV_WR(BCHP_AVS_HW_MNTR_TEMPERATURE_RESET_ENABLE, en);
#endif
}

/* This can be used to verify that reset occured due to over-temp */
/* Note that this doesn't clear the condition as a clear clears
 * ALL reset reasons.  This would prevent the other reasons from
 * being passed up to Linux. */
bool avs_check_temp_reset(bool quiet)
{
	uint32_t reg;
	bool result = false;

	reg = BDEV_RD(BCHP_AON_CTRL_RESET_HISTORY);
	if (reg & BCHP_AON_CTRL_RESET_HISTORY_overtemp_reset_MASK)
		result = true;
	if (result && !quiet) {
		unsigned temperature;
		puts("\n** Over-temp Reset! **\n");
#ifdef BCHP_AVS_TMON_TEMPERATURE_RESET_THRESHOLD
		/* The actual temperature is located in bits 10:1 (not 10:0) */
		reg = (BDEV_RD(BCHP_AVS_TMON_TEMPERATURE_MEASUREMENT_STATUS) &
			BCHP_AVS_TMON_TEMPERATURE_MEASUREMENT_STATUS_data_MASK) >> 1;
		/* temperature=410.04-(reg*0.48705) */
		temperature = (4100400 - (reg * 4870)) / 10000;
#else
		reg = BDEV_RD(BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS) &
			BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS_data_MASK;
		/* temperature=(854.8-reg)/2.069 */
		temperature = (8548 - (reg * 10)) * 100 / 2069;
#endif
		__puts("TEMP=");
		writehex(temperature);
		puts("");
	}

	return result;
}
