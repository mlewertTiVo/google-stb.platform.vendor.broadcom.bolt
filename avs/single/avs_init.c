/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides the Resistor Calibration process.
 *
 *  The Resistor Calibration Procedure measures some resistors on the part and
 *  provides a set of ratios for the external versus the internal resistor sets.
 *  This must be run before setting up the AVS processing.
 *
 ***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "../fsbl/fsbl.h" /* for uart i/o */

#include "bchp_common.h"
#include "bchp_timer.h"
#include "bchp_avs_top_ctrl.h"
#include "bchp_avs_hw_mntr.h"
#include "bchp_avs_pvt_mntr_config.h"
#include "bchp_avs_cpu_ctrl.h"
#include "bchp_avs_ro_registers_0.h"
#include "bchp_avs_cpu_data_mem.h"

#include "avs.h"
#include "avs_temp_reset.h"

/* Debug compile time options: */

#ifndef AVS_DEBUG_STARTUP
#define AVS_DEBUG_STARTUP	0
#endif

#ifndef ENABLE_DEBUG_PRINTS
#define ENABLE_DEBUG_PRINTS	1
#endif

#ifndef TIME_PROCESS
/* enable to display the time the init process took to complete */
#define TIME_PROCESS		0
#endif

#ifndef PRINT_TEMPERATURE
/* enable to print the current temperature as part of the init */
#define PRINT_TEMPERATURE	0
#endif

#if AVS_DEBUG_STARTUP
#undef ENABLE_DEBUG_PRINTS
#define ENABLE_DEBUG_PRINTS 1 /* this has to be set to print */
#endif

#define SCALING_FACTOR 10000
#define INT(x) ((int32_t)((x)*SCALING_FACTOR))

#define MAX_ITERATIONS 100

#define avs_sleep(microsec) udelay(microsec)

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#if (ENABLE_DEBUG_PRINTS)
#define avs_print_char(c) putchar(c)
#define avs_print_val(v) writehex(v)
#define avs_print_cr_lf() puts("");
#define avs_print_string(s) __puts(s)
#else
#define avs_print_char(x) do {} while (0)
#define avs_print_val(x)  do {} while (0)
#define avs_print_cr_lf()  do {} while (0)
#define avs_print_string(s) do {} while (0)
#endif

#define avs_print_string_val(s, x) \
	do { \
		avs_print_string(s); \
		avs_print_val(x); \
		avs_print_cr_lf(); \
	} while (0)

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

static void reset_avs_cpu(void)
{
	BDEV_WR_F(AVS_CPU_CTRL_CTRL, AVS_RST, 1);
}

/* Make sure no "left-over" data in data memory space */
static void zero_data_memory(void)
{
	unsigned index;
	for (index = 0; index < 16; index++) {
		BDEV_WR(BCHP_AVS_CPU_DATA_MEM_WORDi_ARRAY_BASE +
			(index * 4), 0);
	}
}

typedef enum {
	PVT_TEMPERATURE = 0,	/* Bit 0 - Temperature measurement */
	PVT_1p8V = 5,		/* Bit 5 - Voltage 1p8V measurement */
#if 0
	PVT_1V_0 = 3,		/* Bit 3 - Voltage 1V<0> measurement */
	PVT_1V_1 = 4,		/* Bit 4 - Voltage 1V<1> measurement */
	PVT_0P85V_0 = 1,	/* Bit 1 - Voltage 0p85V<0> measurement */
	PVT_0P85V_1 = 2,	/* Bit 2 - Voltage 0p85V<1> measurement */
	PVT_3p3V = 6,		/* Bit 6 - Voltage 3p3V measurement */
	PVT_TESTMODE = 7	/* Bit 7 - Testmode measurement */
#endif
} pvt_t;

static uint32_t avs_read_pvt_value(pvt_t which)
{
	uint32_t code = 0;
	switch (which) {
	case PVT_TEMPERATURE:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS);
		break;
	case PVT_1p8V:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_1p8V_MNTR_STATUS);
		break;
#if 0	/* these are unused options */
	case PVT_1V_0:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS);
		break;
	case PVT_1V_1:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_1V_1_MNTR_STATUS);
		break;
	case PVT_0P85V_0:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_0P85V_0_MNTR_STATUS);
		break;
	case PVT_0P85V_1:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_0P85V_1_MNTR_STATUS);
		break;
	case PVT_3p3V:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_3p3V_MNTR_STATUS);
		break;
	case PVT_TESTMODE:
		code = BDEV_RD(
			BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS);
		break;
#endif
	}

	/* Note: all the values use the same data mask so just use any */
	code &= BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS_data_MASK;

	return code;
}

/* Use Software Override to do voltage measurements */
static uint32_t sw_takeover_measure(pvt_t which)
{
	unsigned i, j;
	uint32_t reg, busy, code, count, sum;

	/* avs_print_string("Starting takeover..."); */

	busy = 1;
	code = 0;
	sum = 0;
	count = 0;

#define SKIPS 0
#define AVERAGE_LOOPS (16+SKIPS)

	for (i = 0; i < AVERAGE_LOOPS; i++) {
		/* These steps need to be done one-at-a-time */
		reg = 0;
		reg |= BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_takeover_MASK;
		BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, reg);

		reg |=
		    which << BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_sensor_idx_SHIFT;
		BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, reg);

		reg |= BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_do_measure_MASK;
		BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, reg);

		avs_sleep(500);

		/* The busy comes on during processing and goes off when done */
		for (j = 0; j < MAX_ITERATIONS; j++) {
			busy =
			    BDEV_RD
			    (BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY) &
			    BCHP_AVS_HW_MNTR_SW_MEASUREMENT_UNIT_BUSY_busy_MASK;
			if (!busy)
				break;
		}

		/* We sometimes see the busy stuck on for some reason (if its
		 * still on, dismiss this data) */
		if (busy) {
			BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, 0);
			continue;
		}
#if (SKIPS > 0)
		if (i < SKIPS)
			continue;
#endif

		code = avs_read_pvt_value(which);

		sum += code;
		count++;

		reg = BDEV_RD(BCHP_AVS_HW_MNTR_SW_CONTROLS);
		reg &= ~BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_do_measure_MASK;
		BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, reg);
	}

	/* Use an average value to dismiss the variance in the reads */
	code = sum / count;

	if (AVS_DEBUG_STARTUP) {
		avs_print_string_val("final code=", code);
		avs_print_string_val("which=", which);
		avs_print_string_val("final count=", count);
	}

	reg = BDEV_RD(BCHP_AVS_HW_MNTR_SW_CONTROLS);
	reg &= ~BCHP_AVS_HW_MNTR_SW_CONTROLS_sw_takeover_MASK;
	BDEV_WR(BCHP_AVS_HW_MNTR_SW_CONTROLS, reg);

	return code;
}

static void avs_reset_sequencer(void)
{
	BDEV_WR(BCHP_AVS_HW_MNTR_SEQUENCER_INIT, 1);
	BDEV_WR(BCHP_AVS_HW_MNTR_SEQUENCER_INIT, 0);
	avs_sleep(1000);
}


/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#define PVT_MON_CTRL_FOR_RC 0x00000F00

static __optimize_O0 void avs_resistor_calibration(bool quiet)
{
	unsigned i;
	uint32_t reg, result0, result1, external, internal, ratios[6];
#ifdef BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ
	uint32_t raw[6];
#endif

	reg = PVT_MON_CTRL_FOR_RC | 7 << 4;
	BDEV_WR(BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL, reg);

	/* The setup for Resistor Calibration requires a "charge-up" delay time
	 * before we take our first measuement */
	avs_sleep(70 * 1000);

	external = sw_takeover_measure(PVT_1p8V);

	if (AVS_DEBUG_STARTUP) {
		unsigned voltage;
		/* original: v_1p8=v_1p8/1024*877.9/0.4 */
		voltage = (external * 8779U) / (4U * 1024U);
		avs_print_cr_lf();
		avs_print_string_val("external=", external);
		avs_print_string_val(" voltage=", voltage);
		avs_print_cr_lf();
	}

	for (i = 0; i < 6; i++) {
		reg = PVT_MON_CTRL_FOR_RC | i << 4;
		BDEV_WR(BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL, reg);

		internal = sw_takeover_measure(PVT_1p8V);

		if (AVS_DEBUG_STARTUP) {
			unsigned voltage;
			voltage = (internal * 8779U) / (4U * 1024U);
			avs_print_char('0' + i);
			avs_print_char(':');
			avs_print_string_val(" internal=", internal);
			avs_print_string_val(" voltage=", voltage);
			/* avs_print_cr_lf(); */
		}

#define MULTIPLIER 128
		ratios[i] = (external * MULTIPLIER) / internal;
		ratios[i] = min(ratios[i], 0xffU);
#ifdef BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ
		raw[i] = internal;
		raw[i] = min(raw[i], 0x3ffU);
#endif
	}

	result0 =
                (1 << BCHP_AVS_TOP_CTRL_RMON_HZ_VALID_VERSION_SHIFT) |
		(ratios[5] << BCHP_AVS_TOP_CTRL_RMON_HZ_RATIO_HZ_1P0_SHIFT) |
		(ratios[3] << BCHP_AVS_TOP_CTRL_RMON_HZ_RATIO_HZ_0P5_SHIFT) |
		(ratios[1] << BCHP_AVS_TOP_CTRL_RMON_HZ_RATIO_HZ_0P25_SHIFT);
	result1 =
                (1 << BCHP_AVS_TOP_CTRL_RMON_VT_VALID_VERSION_SHIFT) |
		(ratios[4] << BCHP_AVS_TOP_CTRL_RMON_VT_RATIO_VT_1P0_SHIFT) |
		(ratios[2] << BCHP_AVS_TOP_CTRL_RMON_VT_RATIO_VT_0P5_SHIFT) |
		(ratios[0] << BCHP_AVS_TOP_CTRL_RMON_VT_RATIO_VT_0P25_SHIFT);

	if (0 || AVS_DEBUG_STARTUP) {
		if (!quiet) {
			avs_print_string_val("spare0/rmon_hz=", result0);
			avs_print_string_val("spare1/rmon_vt=", result1);
		}
	}

	/* This position was moved in later builds */
#ifndef BCHP_AVS_TOP_CTRL_RMON_HZ
	BDEV_WR(BCHP_AVS_CPU_CTRL_SW_SPARE0, result0);
	BDEV_WR(BCHP_AVS_CPU_CTRL_SW_SPARE1, result1);
#else
	BDEV_WR(BCHP_AVS_TOP_CTRL_RMON_HZ, result0);
	BDEV_WR(BCHP_AVS_TOP_CTRL_RMON_VT, result1);
#endif

#ifdef BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ
	/* provide access to raw data as well as ratios */
	result0 =
		(raw[5] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_1UM_SHIFT) |
		(raw[3] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_0P5UM_SHIFT) |
		(raw[1] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_0P25UM_SHIFT);
	result1 =
		(raw[4] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_VT_AVERAGE_R_VT_1UM_SHIFT) |
		(raw[2] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_VT_AVERAGE_R_VT_0P5UM_SHIFT) |
		(raw[0] << BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_VT_AVERAGE_R_VT_0P25UM_SHIFT);

	BDEV_WR(BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_HZ, result0);
	BDEV_WR(BCHP_AVS_TOP_CTRL_RMON_RAWR_INT_VT, result1);

	external = min(external, 0x3ffU);
	external |= 1 << BCHP_AVS_TOP_CTRL_RMON_RAWR_EXT_VALID_VERSION_SHIFT;

	BDEV_WR(BCHP_AVS_TOP_CTRL_RMON_RAWR_EXT, external);

	if (0 || AVS_DEBUG_STARTUP) {
		if (!quiet) {
			avs_print_string_val("rmon_rawr_hz=", result0);
			avs_print_string_val("rmon_rawr_vt=", result1);
			avs_print_string_val("external=", external);
		}
	}
#endif
	/* Restore the PVT_MNTR_CTRL to the default state or else it will
	 * affect temperature and voltage readings */
	BDEV_WR(BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL, 0);
}

static void avs_display_temperature(void)
{
	uint32_t reg;
	unsigned temperature;

	reg = sw_takeover_measure(PVT_TEMPERATURE);

#ifdef BCHP_AVS_TMON_REG_START
	/* original: temperature=410.04-(reg*0.48705) */
	temperature = (4100400 - (reg * 4870)) / 10000;
#else
	/* original: temperature=(854.8-reg)/2.069 */
	temperature = (8548 - (reg * 10)) * 100 / 2069;
#endif
	avs_print_string_val("TEMP=", temperature);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This is the main entry point for AVS FSBL processing */
static void _avs_init(bool quiet)
{
	bool __maybe_unused over_temp;
	unsigned __maybe_unused time;

	/* If the system rebooted then the AVS firmware may still be running.
	 * Kill it! */
	reset_avs_cpu();
	zero_data_memory();

	over_temp = avs_check_temp_reset(0);

	if (TIME_PROCESS)
		time = get_time_diff(0);
/* Newer parts do not reset the calibration values on over-temp condition */
#if (BCHP_CHIP==74371)
	avs_resistor_calibration(quiet);
#else
	if (!over_temp)
		avs_resistor_calibration(quiet);
#endif
	if (TIME_PROCESS)
		avs_print_string_val("Time=", get_time_diff(time));

	if (PRINT_TEMPERATURE)
		avs_display_temperature();

	/* Before we enable the over-temp threshold we need the PVTMON
	 * to be stable.  Reset it before we continue.
	 */
	avs_reset_sequencer();
}

#define AVS_QUIET 0	/* set to 1 to turn off all AVS prints -- should come
			 * from build environment */

/* Use this entry until we can get these parameters elsewhere */
void avs_class_init(void)
{
	/* On 7445/A0&B0 parts, the AVS was disabled by default in OTP */
	/* If its disabled then we can't access any of the AVS registers --
	 * just leave! */
	uint32_t reg =
	    BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS) &
	    BCHP_AVS_TOP_CTRL_OTP_STATUS_OTP_AVS_DISABLE_MASK;
	if (reg) {
		avs_print_string("AVS disabled in OTP!");
		avs_print_cr_lf();
		return;
	}

	/* avs_print_string("Performing AVS Init..."); avs_print_cr_lf(); */
	_avs_init(AVS_QUIET);
}
