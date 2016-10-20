/***************************************************************************
 *     Copyright (c) 2016, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *   This provides the interface definitions between the host (ARM) code
 *   and the AVS CPU firmware.
 *
***************************************************************************/

#ifndef _AVS_FW_H_
#define _AVS_FW_H_

/* #include <stdint.h> */
/* #include <stdbool.h> */

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This defines the structure of the common "message" area during startup.
 * This is used to pass parameters to the firmware. The status is returned
 * once the firmware has successfully started. Both parameters are defined
 * by the "VALID" and "DATA" macros below. (Note: this would be a bit field,
 * but the ARC processor has trouble with these and I'm not sure why). */
struct at_initialization {
	uint32_t unused;
	uint32_t status;

	/* This is the initialization data host provides: */
	int32_t margin_low;	 /* low margin value */
	int32_t margin_high;	 /* high margin value */
	uint32_t min_voltage;	 /* minimum voltage allowed */
	uint32_t max_voltage;	 /* maximum voltage allowed */
	uint32_t extra_delay;	 /* additional delay (uSec) to delay before
				  * reading voltage */
	uint32_t polling_delay;  /* delay between polling loops */
	uint32_t chip_id;	 /* this STB family ID */
	uint32_t bbu_used;	 /* flag indicating if Battery Backup (Battery)
				  * used in this project */
	uint32_t bbm_flag;	 /* true if booting under battery backup mode */
	uint32_t product_id;	 /* this STB product ID */
	uint32_t over_temp_flag; /* flag telling firmware init due to
				  * over-temperature condition */
	uint32_t resuming_s3;	 /* this startup is due to S3 resume */
} at_initialization;

/* Initialization responses: */
#define INIT_COMPLETE  0xFF	/* firmware initialized and running */
#define INIT_FAILED    0xF0	/* firmware failed to initialize */
#define AVS_NOT_FOUND  0xF1	/* firmware entered AVS_NOT_FOUND */
#define CHIP_MISMATCH  0xF2	/* firmware build doesn't match chip version */

/* AVS "not found" condition is used to indicate when AVS is not able to use
   the voltage regulator to control the voltage level.	That is, it received
   the same voltage values for two different DAC settings that should have
   generated two different voltage values.  This typically indicates that
   either the voltage regulator is being by-passed, that the AVS resistor
   circuit was implemented incorrectly (preventing the DAC from controlling
   the regulator), or there is so much IR drop in the design that the voltage
   regulator is not able to control the voltage to a level that shows two
   different voltage results for two different DAC values (this is also
   a design error).
*/

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

#define AVS_FIRMWARE_MAIC 0xa11600d1 /* says firmware started */

/* This defines the different firmware states concerning BBM support.
 * This is returned to host as part of status processing. */
enum power_state {
	initializing,		/* cold boot, preforming init processing */
	normal_tracking,	/* under AC power -- continue tracking */
	bbm_active,	/* received AC power loss interrupt -- no STB access
			 * allowed */
	bbm_engaged,	/* received ignore interrupt -- stop driving DAC */
	ac_restored,	/* received power restore interrupt -- balance STB to
			 * DCD */
	firmware_is_dead,	/* failed to initialize -- host should reset */
} power_state;

/* This defines the structure of the common "message" area during run-time.
 * The command parameter passes a notification to the firmware. The status
 * parameter is updated by the firmware to indicate command complete. Both
 * parameters are defined by the "VALID" and "DATA" macros below. */
struct at_runtime {
	uint32_t command;
	uint32_t status;

	/* This is the data firmware supplies: */
	uint32_t voltage0;	/* current voltage for STB (in milli-volts) */
	int32_t temperature0;	/* current temperature for STB (in 1000th) */
	uint32_t PV0;	/* initial predicted voltage value (in milli-volts) */
	uint32_t MV0;	/* initial measured voltage value (in milli-volts) */
	/* These next values are only used when two implementations of AVS
	 * Monitor exist (CM side of STB chip - if we have one) */
	uint32_t voltage1;	/* (see above for units) */
	int32_t temperature1;
	uint32_t PV1;
	uint32_t MV1;

	uint32_t revision;	/* the revision of the AVS firmware */
	enum power_state state;	/* the current state of the firmware */
	uint32_t heartbeat;	/* updated everytime the process runs */
	uint32_t avs_magic;	/* set to magic value to show firmware
				 *  actually started */
} at_runtime;

#define AVS_VALID   (1<<31)
#define AVS_DATA(x) ((x)&~AVS_VALID)

/* Commands: */
#define CMD_DISABLE_AVS 0x10	/* Disable AVS Processing.
				 * This is used to tell the AVS CPU to
				 * temporarily discontinue performing the
				 * AVS-Track process. This is typically
				 * used for diagnostics purposes. */
#define CMD_ENABLE_AVS 0x11	/* Re-enabled AVS Processing.
				 * This is used to un-do the Disable AVS
				 * Processing request. */
#define CMD_STB_ENTER_S2 0x12	/* Entering S2 mode.
				 * The host uses this to let the AVS CPU
				 * know that the processor is entering S2
				 * Power Mode. Once this is enabled, the
				 * AVS CPU will use the special S2 wakeup. */
#define CMD_STB_EXIT_S2	0x13	/* Exiting S2 mode.
				 * The host uses this to let the AVS CPU know
				 * that it is no longer in the S2 power mode. */
#define CMD_ENTER_BBM	0x14	/* Entering Battery Backup mode.
				 * The host uses this notice to indicate that
				 * it is about to Enter Battery Back mode
				 * (voluntarily). */
#define CMD_EXIT_BBM	0x15	/* Exiting from Battery Backup Mode.
				 * The host uses this notice to indicate that
				 * power has been restored and system is no
				 * longer running on battery power. */
#define CMD_STB_S3_ENTER 0x16	/* Powering off STB (entering S3 Mode). */
#define CMD_STB_S3_EXIT  0x17	/* Powering on STB (existing S3 Mode). */
#define CMD_BALANCE	 0x18	/* Balance.
				 * This request is used to periodically balance
				 * the voltage settings between the two blocks
				 * (CM and STB).
				 */

/* Responses: */
#define RSP_SUCCESS	0xF0	/* Notification accepted */
#define RSP_FAILURE	0xFF	/* Notification rejected */
#define RSP_INVALID	0xF1	/* Invalid notification (unknown) */

#endif /*_AVS_FW_H_*/
