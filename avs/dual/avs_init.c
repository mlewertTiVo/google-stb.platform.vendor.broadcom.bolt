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
 *  This performs two things:
 *  1) The Resistor Calibration Procedure
 *  2) AVS Balance Procedure (STB chips with CM)
 *  The Resistor Calibration Procedure measures some resistors on the part and
 *  provides a set of ratios for the external versus the internal resistor sets.
 *  This must be run before setting up the AVS processing.
 *  The Balance Procedure is used to balance the voltages on the two power
 *  islands: STB and DCS.  This is required before being able to access the
 *  Cable Modem block.
 *
***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "../fsbl/fsbl.h"  /* for uart & battery backup support */

#include "bchp_common.h"
#include "bchp_timer.h"

#include "bchp_avs_top_ctrl.h"
#include "bchp_avs_hw_mntr_1.h"
#include "bchp_avs_cpu_ctrl.h"
#include "bchp_avs_pvt_mntr_config_1.h"
#include "bchp_avs_ro_registers_0_1.h"
#include "bchp_avs_cpu_data_mem.h"

#include "avs.h"
#include "avs_temp_reset.h"

/* Debug compile time options: */

#ifndef  AVS_DEBUG_STARTUP
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

#ifndef ASK_VOLTAGE
/* enable for debug to set initial voltage value */
#define ASK_VOLTAGE		0
#endif

#if AVS_DEBUG_STARTUP
#undef ENABLE_DEBUG_PRINTS
#define ENABLE_DEBUG_PRINTS 1 /* this has to be set to print */
#endif

#define SCALING_FACTOR 10000
#define S1 SCALING_FACTOR
#define INT(x) ((int32_t)((x)*S1))

/* Use these for base address definitions and offsets between the two
 * AVS_MONITOR blocks */
#define MONITOR_OFFSET \
	(BCHP_AVS_HW_MNTR_2_REG_START - BCHP_AVS_HW_MNTR_1_REG_START)

static uint32_t avs_monitor_offsets[2] = {
	BCHP_PHYSICAL_OFFSET, BCHP_PHYSICAL_OFFSET + MONITOR_OFFSET };

/* These are used to access one of the AVS_MONITOR registers (of which there
 * are two blocks) */
#define BDEV_WR_DEV(dev, addr, value) \
	(*(volatile uint32_t*)((addr)+avs_monitor_offsets[dev]) = (value))
#define BDEV_RD_DEV(dev, addr) \
	(*(volatile uint32_t*)((addr)+avs_monitor_offsets[dev]))

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

#define avs_print_decimal(n) writeint(n)
#define avs_return_char() (char)getchar()

#define avs_print_string_val(s, x) \
	do { \
		avs_print_string(s); \
		avs_print_val(x); \
		avs_print_cr_lf(); \
	} while (0)

#if 0 /* this enables the prettier hhh(ddd) value prints */
#undef avs_print_val
#define avs_print_val(x) \
	do {\
		writehex(x);
		avs_print_char('(');
		avs_print_decimal(x);
		avs_print_char(')');
	} while (0)
#endif

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* this is debug code and will be optimized out when debug not enabled */
static bool ask_y_or_n(const char *prompt, bool yes)
{
	char c;
	avs_print_string((char *)prompt);
	if (yes)
		avs_print_string(" <Y/n>? ");
	else
		avs_print_string(" <y/N>? ");
	c = avs_return_char();
	avs_print_char(c);
	avs_print_cr_lf();
	if (yes)
		return c != 'N' && c != 'n';
	else
		return c != 'Y' && c != 'y';
}

/* this is debug code and will be optimized out when debug not enabled */
static int avs_get_int_number(char *prompt, int the_default)
{
	char c;
	int input;
	bool got_something, negative;

	while (0) {
 retry:
		avs_print_cr_lf();
	}
	input = 0;
	got_something = false;
	negative = false;
	if (prompt)
		avs_print_string(prompt);
	avs_print_char('(');
	avs_print_decimal(the_default);
	avs_print_char(')');
	avs_print_char('?');
	avs_print_char(' ');

	for (; (c = avs_return_char()) != '\r';) {
		if (!c)
			goto use_default;
		if (c == 27)
			goto retry;
		avs_print_char(c);
		if (c == '-') {
			negative = true;
			continue;
		}
		if (!(c >= '0' && c <= '9'))
			goto retry;
		input = (input * 10) + (c - '0');
		got_something = true;
	}
	avs_print_cr_lf();
	if (!got_something)
		goto use_default;
	if (negative)
		input *= -1;
	while (0) {
 use_default:
		input = the_default;
	}

	/* show them what we're using in case they picked the default and don't
	 * know what the default is */
	/*
	if (prompt)
		avs_print_string(prompt);
	avs_print_char('=');
	avs_print_decimal(input);
	avs_print_cr_lf();
	*/
	return input;
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

typedef enum {
	PVT_TEMPERATURE = 0,	/* Bit 0 - Temperature measurement */
	PVT_1V_0 = 3,		/* Bit 3 - Voltage 1V<0> measurement */
	PVT_1p8V = 5,		/* Bit 5 - Voltage 1p8V measurement */
	PVT_3p3V = 6,		/* Bit 6 - Voltage 3p3V measurement */
#if 0
	PVT_0P85V_0 = 1,	/* Bit 1 - Voltage 0p85V<0> measurement */
	PVT_0P85V_1 = 2,	/* Bit 2 - Voltage 0p85V<1> measurement */
	PVT_1V_1 = 4,		/* Bit 4 - Voltage 1V<1> measurement */
	PVT_TESTMODE = 7	/* Bit 7 - Testmode measurement */
#endif
} pvt_t;

static uint32_t avs_read_pvt_value(unsigned device, pvt_t which)
{
	uint32_t code = 0;
	switch (which) {
	case PVT_TEMPERATURE:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_TEMPERATURE_MNTR_STATUS);
		break;
	case PVT_1V_0:
		/* we get better voltage read if we use STB for both */
		if (device == DCD_DEVICE)
			code =
			    BDEV_RD_DEV(STB_DEVICE,
				BCHP_AVS_RO_REGISTERS_0_1_PVT_1V_1_MNTR_STATUS);
		else
			code =
			    BDEV_RD_DEV(STB_DEVICE,
				BCHP_AVS_RO_REGISTERS_0_1_PVT_1V_0_MNTR_STATUS);
		break;
	case PVT_1p8V:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_1p8V_MNTR_STATUS);
		break;
	case PVT_3p3V:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_3p3V_MNTR_STATUS);
		break;
#if 0	/* these are unused options */
	case PVT_1V_1:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_1V_1_MNTR_STATUS);
		break;
	case PVT_0P85V_0:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_0P85V_0_MNTR_STATUS);
		break;
	case PVT_0P85V_1:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_0P85V_1_MNTR_STATUS);
		break;
	case PVT_TESTMODE:
		code =
		    BDEV_RD_DEV(device,
			BCHP_AVS_RO_REGISTERS_0_1_PVT_TESTMODE_MNTR_STATUS);
		break;
#endif
	}

	/* Note: all the values use the same data mask so just use any */
	code &= BCHP_AVS_RO_REGISTERS_0_1_PVT_TEMPERATURE_MNTR_STATUS_data_MASK;

	return code;
}

/* Use Software Override to do voltage measurements */
/* This just reads the requested register with debounce */
static uint32_t sw_takeover_measure(unsigned device, pvt_t which)
{
	unsigned i, j;
	uint32_t reg, busy, code, count, sum;
	uint32_t high = 0, low = 0;

	busy = 1;
	code = 0;
	sum = 0;
	count = 0;

#define SKIPS 0 /* skip this many loops to allow for settling */
#define AVERAGE_LOOPS (16+SKIPS) /* larger number means better
				    accuracy but takes longer */

	for (i = 0; i < AVERAGE_LOOPS; i++) {
		/* These steps need to be done one-at-a-time */
		reg = 0;
		reg |= BCHP_AVS_HW_MNTR_1_SW_CONTROLS_sw_takeover_MASK;
		BDEV_WR_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS, reg);

		reg |=
		    which << BCHP_AVS_HW_MNTR_1_SW_CONTROLS_sw_sensor_idx_SHIFT;
		BDEV_WR_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS, reg);

		reg |= BCHP_AVS_HW_MNTR_1_SW_CONTROLS_sw_do_measure_MASK;
		BDEV_WR_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS, reg);

		avs_sleep(500);

		/* The busy comes on during processing and goes off when done */
		for (j = 0; j < MAX_ITERATIONS; j++) {
			busy =
			    BDEV_RD_DEV(device,
				BCHP_AVS_HW_MNTR_1_SW_MEASUREMENT_UNIT_BUSY)
			    &
			BCHP_AVS_HW_MNTR_1_SW_MEASUREMENT_UNIT_BUSY_busy_MASK;
			if (!busy)
				break;
		}

		/* We sometimes see the busy stuck on for some reason (if its
		 * still on, dismiss this data) */
		if (busy) {
			BDEV_WR_DEV(device,
				    BCHP_AVS_HW_MNTR_1_SW_CONTROLS,
				    0);
			continue;
		}
#if SKIPS > 0
		if (i < SKIPS)
			continue;
#endif

		code = avs_read_pvt_value(device, which);
		/* avs_print_string_val("code=", code); */

		sum += code;
		count++;

		if (!high || code > high) high = code;
		if (!low || code < low) low = code;

		reg = BDEV_RD_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS);
		reg &= ~BCHP_AVS_HW_MNTR_1_SW_CONTROLS_sw_do_measure_MASK;
		BDEV_WR_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS, reg);
	}

	/* Use an average value to dismiss the variance in the reads */
	code = sum / count;

	if (AVS_DEBUG_STARTUP) {
		avs_print_string_val("final code=", code);
		avs_print_string_val("device=", device);
		avs_print_string_val("which=", which);
		avs_print_string_val("successful loops=", count);
		avs_print_string_val("high=", high);
		avs_print_string_val("low=", low);
	}

	reg = BDEV_RD_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS);
	reg &= ~BCHP_AVS_HW_MNTR_1_SW_CONTROLS_sw_takeover_MASK;
	BDEV_WR_DEV(device, BCHP_AVS_HW_MNTR_1_SW_CONTROLS, reg);

	return code;
}

/* Return the current voltage value generated by the PVTMON */
static unsigned avs_read_voltage(unsigned device, pvt_t which)
{
	uint32_t reg;
	unsigned voltage;

	reg = sw_takeover_measure(device, which);

	/* original: voltage_1p1_0 = ((v_1p1_0/1024)*877.9/.7)/1000 */
	voltage = 10U * (reg * 8779U) / (1024U * 7U);

	if (AVS_DEBUG_STARTUP) {
		avs_print_string_val("voltage=", voltage);
		avs_print_string_val("  device=", device);
		avs_print_string_val("  which=", which);
	}

	return voltage;
}

#define PVT_MON_CTRL 0x00000180
#define DAC_WRITE_DELAY 32

/* This is used to program the DAC to get a desired voltage */
/* This is the ONLY one that should be used to set the DAC to ensure that the
 * voltage is never read before it becomes stable. */
static void avs_set_volltage(unsigned device, uint32_t dac_code,
			  unsigned extra_delay)
{
	if (AVS_DEBUG_STARTUP) {
		avs_print_string_val("device=", device);
		avs_print_string_val(" setting new DAC=", dac_code);
		avs_print_cr_lf();
	}

	/* Be sure to set the new DAC code BEFORE we set the PVTMON_CTRL word
	 * or it could start with a 0 DAC setting */
	BDEV_WR_DEV(device,
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE, dac_code);
	BDEV_WR_DEV(device,
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE_PROGRAMMING_ENABLE,
		    1);
	avs_sleep(1000);

	BDEV_WR_DEV(device, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL,
		    PVT_MON_CTRL);
	avs_sleep(1000);
	BDEV_WR_DEV(device,
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE_PROGRAMMING_ENABLE,
		    0);

	/* Always give time for sequencer to sequence through all the different
	 * available data */
	avs_sleep((DAC_WRITE_DELAY + extra_delay) * 1000);
}

static uint32_t avs_get_dac(unsigned device)
{
	return BDEV_RD_DEV(device, BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */
/* DAC Charging Procedure */

#define PVT_MON_CTRL_FOR_DC 0x00000080

static void enable_dac_driver_only(uint32_t stb_dac_code, uint32_t dcd_dac_code)
{
	BDEV_WR_DEV(STB_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE,
		    stb_dac_code);
	BDEV_WR_DEV(DCD_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE,
		    dcd_dac_code);
	BDEV_WR_DEV(STB_DEVICE,
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE_PROGRAMMING_ENABLE,
		    1);
	BDEV_WR_DEV(DCD_DEVICE,
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE_PROGRAMMING_ENABLE,
		    1);
	BDEV_WR_DEV(STB_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL,
		    PVT_MON_CTRL_FOR_DC);
	BDEV_WR_DEV(DCD_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL,
		    PVT_MON_CTRL_FOR_DC);
	avs_sleep(10 * 1000);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */
/* Resistor Calibration Procedure */

#define PVT_MON_CTRL_FOR_RC 0x00000F00

static void avs_resistor_calibration(unsigned device, bool quiet)
{
	unsigned i;
	uint32_t reg, result0, result1, external, internal, ratios[6];
#ifdef BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ
	uint32_t raw[6];
#endif

	reg = PVT_MON_CTRL_FOR_RC | 7 << 4;
	BDEV_WR_DEV(device, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL, reg);

	/* The setup for Resistor Calibration requires a "charge-up" delay time
	 * before we take our first measuement */
	avs_sleep(70 * 1000);

	external = sw_takeover_measure(device, PVT_1p8V);


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
		BDEV_WR_DEV(device,
			    BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL, reg);

		internal = sw_takeover_measure(device, PVT_1p8V);

	if (AVS_DEBUG_STARTUP) {
		unsigned voltage;
		voltage = (internal * 8779U) / (4U * 1024U);
		avs_print_char('0' + i);
		avs_print_char(':');
		avs_print_string_val(" internal=", internal);
		avs_print_string_val(" voltage=", voltage);
		avs_print_cr_lf();
	}

#define MULTIPLIER 128
		ratios[i] = (external * MULTIPLIER) / internal;
		ratios[i] = min(ratios[i], 0xffU);
#ifdef BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ
		raw[i] = internal;
		raw[i] = min(raw[i], 0x3ffU);
#endif
	}

	result0 =
		(1 << BCHP_AVS_TOP_CTRL_STB_RMON_HZ_VALID_VERSION_SHIFT) |
		(ratios[5] << BCHP_AVS_TOP_CTRL_STB_RMON_HZ_RATIO_HZ_1P0_SHIFT) |
		(ratios[3] << BCHP_AVS_TOP_CTRL_STB_RMON_HZ_RATIO_HZ_0P5_SHIFT) |
		(ratios[1] << BCHP_AVS_TOP_CTRL_STB_RMON_HZ_RATIO_HZ_0P25_SHIFT);
	result1 =
		(1 << BCHP_AVS_TOP_CTRL_STB_RMON_VT_VALID_VERSION_SHIFT) |
		(ratios[4] << BCHP_AVS_TOP_CTRL_STB_RMON_VT_RATIO_VT_1P0_SHIFT) |
		(ratios[2] << BCHP_AVS_TOP_CTRL_STB_RMON_VT_RATIO_VT_0P5_SHIFT) |
		(ratios[0] << BCHP_AVS_TOP_CTRL_STB_RMON_VT_RATIO_VT_0P25_SHIFT);

	if (0 || AVS_DEBUG_STARTUP) {
		avs_print_string_val("spare0/rmon_hz=", result0);
		avs_print_string_val("spare1/rmon_vt=", result1);
	}

#ifndef BCHP_AVS_TOP_CTRL_STB_RMON_HZ
	BDEV_WR(BCHP_AVS_CPU_CTRL_SW_SPARE0, result0);
	BDEV_WR(BCHP_AVS_CPU_CTRL_SW_SPARE1, result1);
#else
	if (device == STB_DEVICE) {
		BDEV_WR(BCHP_AVS_TOP_CTRL_STB_RMON_HZ, result0);
		BDEV_WR(BCHP_AVS_TOP_CTRL_STB_RMON_VT, result1);
	} else {
		BDEV_WR(BCHP_AVS_TOP_CTRL_CM_RMON_HZ, result0);
		BDEV_WR(BCHP_AVS_TOP_CTRL_CM_RMON_VT, result1);
	}
#endif

#ifdef BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ
	/* provide access to raw data as well as ratios */
	result0 =
		(raw[5] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_1UM_SHIFT) |
		(raw[3] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_0P5UM_SHIFT) |
		(raw[1] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ_AVERAGE_R_HZ_0P25UM_SHIFT);
	result1 =
		(raw[4] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_VT_AVERAGE_R_VT_1UM_SHIFT) |
		(raw[2] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_VT_AVERAGE_R_VT_0P5UM_SHIFT) |
		(raw[0] << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_VT_AVERAGE_R_VT_0P25UM_SHIFT);
	if (device == STB_DEVICE) {
		BDEV_WR(BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_HZ, result0);
		BDEV_WR(BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_INT_VT, result1);
	} else {
		BDEV_WR(BCHP_AVS_TOP_CTRL_CM_RMON_RAWR_INT_HZ, result0);
		BDEV_WR(BCHP_AVS_TOP_CTRL_CM_RMON_RAWR_INT_VT, result1);
	}

	external = min(external, 0x3ffU);
	external |= 1 << BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_EXT_VALID_VERSION_SHIFT;

	if (device == STB_DEVICE)
		BDEV_WR(BCHP_AVS_TOP_CTRL_STB_RMON_RAWR_EXT, external);
	else
		BDEV_WR(BCHP_AVS_TOP_CTRL_CM_RMON_RAWR_EXT, external);

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
	BDEV_WR_DEV(device, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL, 0);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */
/* AVS Balance Procedure */

static void get_board_dac_value(
		uint32_t *stb_dac, uint32_t *dcd_dac, bool quiet)
{
	uint32_t reg, VFB_pin_voltage;
	uint32_t stb_board_dac_code, dcd_board_dac_code;

#define PVT_MON_CTRL_TEST 0x200 /* ADC measures external input on pad_ADC */

	BDEV_WR_DEV(STB_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL,
		    PVT_MON_CTRL_TEST);
	BDEV_WR_DEV(DCD_DEVICE, BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL,
		    PVT_MON_CTRL_TEST);
	avs_sleep(30 * 1000);

	reg = sw_takeover_measure(STB_DEVICE, PVT_1p8V);

	/* These are the original statements from BBS script:
	 * STB_VFB_pin_voltage = STB_VFB_pin_voltage_code*877.9/1024
	 * STB_cur_dac_code=CLng((1421 - STB_VFB_pin_voltage)/1.191)
	 * And here's how I did it with integer arithmetic: */
	VFB_pin_voltage = reg * 8779U / 10240U;
	stb_board_dac_code = (1421U - VFB_pin_voltage) * 1000U / 1191U;

	reg = sw_takeover_measure(DCD_DEVICE, PVT_3p3V);

	/* These are the original statements from BBS script:
	 * DCD_VFB_pin_voltage = DCD_VFB_pin_voltage_code*877.9/1024
	 * DCD_cur_dac_code=CLng((1421 - DCD_VFB_pin_voltage)/1.191)
	 * And here's how I did it with integer arithmetic: */
	VFB_pin_voltage = reg * 8779U / 10240U;
	dcd_board_dac_code = (1421U - VFB_pin_voltage) * 1000U / 1191U;

	*stb_dac = stb_board_dac_code;
	*dcd_dac = dcd_board_dac_code;

	/* Restore PVTMON to default configuration */
	/* BDEV_WR_DEV(STB_DEVICE,
	 *	BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL, 0); */
	/* BDEV_WR_DEV(DCD_DEVICE,
	 *	BCHP_AVS_PVT_MNTR_CONFIG_1_PVT_MNTR_CTRL, 0); */

	if (!quiet)
		avs_print_char('B');
}

#define NOMINAL_VOLTAGE (1.030)
#define SWING_VALUE (.005)
#define MAX_ITERATIONS_1V 100

static uint32_t find_voltage(unsigned device, uint32_t dac_code,
			    unsigned target_voltage, unsigned extra_delay,
			    bool quiet)
{
	unsigned i;
	uint32_t voltage = 0, cur_dac_code = dac_code;
	uint32_t last_voltage = 0;
	uint32_t voltage_was = 0;
	char result = 'X';
	bool success = false;

	/* avs_print_string("FindVoltage..."); */
	/* avs_print_string_val("cur_dac_code =", cur_dac_code); */
	for (i = 0; i < MAX_ITERATIONS_1V; i++) {
		/* avs_print_string_val("DAC=", cur_dac_code); */
		avs_set_volltage(device, cur_dac_code, extra_delay);
		voltage = avs_read_voltage(device, PVT_1V_0);
		/* avs_print_string_val("voltage =", voltage); */

		/* CLOSE ENOUGH? If I'm in the ball park, then we're done ... */
		if (voltage >= (target_voltage - INT(SWING_VALUE)) &&
		    voltage <= (target_voltage + INT(SWING_VALUE))) {
			success = true;
			result = '1';
			break;
		}

		if (i == 0)
			voltage_was = voltage;
#if 0
		/* This method uses a stepping technique -- voltage "walks" up
		 * to desired in small steps */
		{
#define STEP_SIZE 1
			if (voltage < target_voltage)
				cur_dac_code += STEP_SIZE;
			if (voltage > target_voltage)
				cur_dac_code -= STEP_SIZE;
		}
#else
		/* This method "jumps" to the new voltage by calculating what
		 * the DAC should be for the desired voltage */
		{
			int step_size;
			step_size = ((signed)target_voltage - (signed)voltage)
				* 10 / 88;
			cur_dac_code += step_size;
		}
#endif
		if (AVS_DEBUG_STARTUP) {
			avs_print_string_val("voltage =", voltage);
			avs_print_string_val("last voltage =", last_voltage);
			avs_print_string_val("new cur_dac_code =",
					     cur_dac_code);
		}

		last_voltage = voltage;

		if (cur_dac_code >
		    BCHP_AVS_PVT_MNTR_CONFIG_1_DAC_CODE_dac_code_MASK) {
			cur_dac_code = dac_code;
			break;
		}
	}

	/* If the IR drop on this board is such that we could not change the
	 * voltage to a point that was close enough to 1V then we might have
	 * a corrupt first DAC value.  Try to determine which DAC value we
	 * should use. */
	if (!success) {
		result = 'O';
		cur_dac_code = dac_code;
	}

	if (AVS_DEBUG_STARTUP) {
		avs_print_string("FindVoltage...\n");
		avs_print_string_val("voltage was=", voltage_was);
		avs_print_string_val("voltage is =", voltage);
		avs_print_string_val("DAC=", cur_dac_code);
		avs_print_string_val("loops=", i);
	}

	if (!quiet)
		avs_print_char(result);

	return cur_dac_code;
}

static uint32_t find_nomnal_voltage(unsigned device, uint32_t dac_code,
				   unsigned extra_delay, bool quiet)
{
	return find_voltage(device, dac_code, INT(NOMINAL_VOLTAGE), extra_delay,
			   quiet);
}

/* This chooses the AVS Monitor used to set the baseline voltage (0=STB, 1=DCS)
 * The other device will be adjusted to be within 1% of the default device */
#define DEFAULT_DEVICE STB_DEVICE
#define OTHER_DEVICE   DCD_DEVICE

#define DAC_STEP 1
#define ONE_PERCENT (.0010)

/* This preforms the voltage balancing process between the two voltage
 * planes (DCS & STB) */
static void avs_balance(unsigned extra_delay, bool quiet)
{
	unsigned i;
	uint32_t cur_dac_code;
	unsigned voltage1, voltage2;

	/* avs_print_string("Balance..."); */

	/* Use the current DAC of the other device as the baseline DAC value */
	cur_dac_code = avs_get_dac(OTHER_DEVICE);

	/* Use the primary device to get the baseline voltage */
	voltage1 = avs_read_voltage(DEFAULT_DEVICE, PVT_1V_0);

	for (i = 0; i < MAX_ITERATIONS; i++) {
		avs_set_volltage(OTHER_DEVICE, cur_dac_code, extra_delay);

		voltage2 = avs_read_voltage(OTHER_DEVICE, PVT_1V_0);
#if 0
		/* This method "walks" up to the desired voltage in small
		 * increments */
		/* Stop when the voltages are within 1% of each other */
		/* Make necessary adjustments for next pass */
		if (voltage2 < voltage1) {
			if ((voltage1 - voltage2) <= INT(ONE_PERCENT))
				break;
			cur_dac_code += DAC_STEP;
		} else {
			if ((voltage2 - voltage1) <= INT(ONE_PERCENT))
				break;
			cur_dac_code -= DAC_STEP;
		}
#else
		/* This method "jumps" to where it thinks the DAC should be for
		 * the desired voltage */
		/* CLOSE ENOUGH? Stop when the voltages are within 1% of each
		 * other */
		if (voltage2 >= voltage1 - INT(ONE_PERCENT) &&
		    voltage2 <= voltage1 + INT(ONE_PERCENT))
			break;

		{
			int step_size;
			step_size = ((signed)voltage1 - (signed)voltage2) *
				10 / 88;
			cur_dac_code += step_size;
		}
#endif
	}

	/* If we run out of iterations should we panic? */
	/* avs_print_string_val("loops=", i); */
}

static void reset_avs_CPU(void)
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

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This is debug code to allow test engineers to supply an initial voltage
 * value for part testing.
 * TODO: move this into branch or use "weak" functions. */

static int custom_voltage;

static void ask_voltage(void)
{
	if (ask_y_or_n("Use custom voltage ", false))
		return;
	custom_voltage =
	    avs_get_int_number("Custom Voltage (in mVolts)",
			    INT(NOMINAL_VOLTAGE) / 10);
	custom_voltage *= 10;
}
static void lower_voltage(unsigned extra_delay, bool quiet)
{
	uint32_t cur_dac_code;
	if (!custom_voltage)
		return;
	cur_dac_code = avs_get_dac(DEFAULT_DEVICE);
	find_voltage(DEFAULT_DEVICE, cur_dac_code, custom_voltage, extra_delay,
		     quiet);
	avs_balance(extra_delay, quiet);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This performs the balancing procedure */
static void avs_setup(unsigned extra_delay, bool quiet)
{
	uint32_t stb_dac_code, dcd_dac_code;

	get_board_dac_value(&stb_dac_code, &dcd_dac_code, quiet);
	enable_dac_driver_only(stb_dac_code, dcd_dac_code);
	find_nomnal_voltage(STB_DEVICE, stb_dac_code, extra_delay, quiet);
	find_nomnal_voltage(DCD_DEVICE, dcd_dac_code, extra_delay, quiet);
	avs_balance(extra_delay, quiet);

	lower_voltage(extra_delay, quiet);

	if (!quiet) {
		uint32_t dac1, dac2, voltage1, voltage2;
		avs_print_cr_lf();
		dac1 = avs_get_dac(STB_DEVICE);
		dac2 = avs_get_dac(DCD_DEVICE);
		voltage1 = avs_read_voltage(STB_DEVICE, PVT_1V_0);
		voltage2 = avs_read_voltage(DCD_DEVICE, PVT_1V_0);
		avs_print_string("STB V=");
		avs_print_val(voltage1);
		avs_print_string(" D=");
		avs_print_val(dac1);
		avs_print_cr_lf();
		avs_print_string("DCM V=");
		avs_print_val(voltage2);
		avs_print_string(" D=");
		avs_print_val(dac2);
		avs_print_cr_lf();
	}
}

static void avs_display_temperature(unsigned device)
{
	uint32_t reg;
	signed temperature;

	reg = sw_takeover_measure(device, PVT_TEMPERATURE);
	/* original: temperature=(854.8-reg)/2.069 */
	temperature = (8548 - (reg * 10)) * 100 / 2069;
	avs_print_string_val("TEMP=", temperature);
}

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This is the main entry point for AVS FSBL processing */
static void _avs_init(unsigned extra_delay, bool quiet)
{
	bool __maybe_unused over_temp;
	unsigned __maybe_unused time;

#if CFG_BATTERY_BACKUP
	/* If docsis is booted then AVS firmware is still running.
	 * In this case we're not to touch it.
	 */
	if (!fsbl_docsis_booted(0, 0))
#endif /* CFG_BATTERY_BACKUP */
	{
		reset_avs_CPU();
		zero_data_memory();
	}

	over_temp = avs_check_temp_reset(0);

	if (TIME_PROCESS)
		time = get_time_diff(0);
	/* newer parts moved some parameters to AON
	 * and don't get reset in over-temperature situations. */
	if (!over_temp)
		avs_resistor_calibration(STB_DEVICE, quiet);
	avs_setup(extra_delay, quiet);
	if (TIME_PROCESS)
		avs_print_string_val("Time=", get_time_diff(time));

	if (PRINT_TEMPERATURE)
		avs_display_temperature(DEFAULT_DEVICE);

	/* Note: we need the sequencer to be stable before we enable the
	 * over-temp threshold as a bad temperature reading could cause the
	 * part to reset.
	 * In this code base, the avs_setup leaves the PVTMON
	 * in a good state for firmware to take-over the processing.
	 * So resetting the sequencer is not needed.
	 */
}

/* Use this entry until we can get these parameters elsewhere */
#define AVS_QUIET 0	/* set to 1 to turn off all AVS prints -- should come
			 * from build environment */
#define AVS_EXTRA_DELAY 0	/* this should come from external source --
				 * implementation dependent */

/* Use this entry until we can get these parameters elsewhere */
void avs_class_init(void)
{
	uint32_t reg;

	if (ASK_VOLTAGE)
		ask_voltage();

#if CFG_BATTERY_BACKUP
	if (fsbl_docsis_booted(0, 0)) {
		avs_print_string("Resuming from BBM...");
		avs_print_cr_lf();
		return;
	}
#endif

	/* Its possible that AVS could be disabled on this part. */
	/* If its disabled then we can't access any of the AVS registers --
	 * just leave! */
	/* Note: we do not check the CM OTP here -- I suppose it possible that
	 * one is enabled and other is disabled (but not likely) */
	reg =
	    BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS_STB) &
	    BCHP_AVS_TOP_CTRL_OTP_STATUS_STB_OTP_AVS_DISABLE_MASK;
	if (reg) {
		avs_print_string("AVS disabled in OTP!");
		avs_print_cr_lf();
		return;
	}

	/* avs_print_string("Performing AVS Init..."); avs_print_cr_lf(); */
	_avs_init(AVS_EXTRA_DELAY, AVS_QUIET);
}
