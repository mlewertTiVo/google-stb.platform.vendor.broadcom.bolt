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
 * This is used to pass parameters to the firmware at initialization.
 * The status is returned once the firmware has successfully started.
 */
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
	uint32_t single_domain;  /* set for 72xx boards with single regulator */
	uint32_t cpu_offset;	 /* for devices with STB/CPU domains */
	uint32_t product_id;	 /* this STB product ID */
	uint32_t over_temp_flag; /* flag telling firmware init due to
				  * over-temperature condition */
	uint32_t resuming_s3;	 /* this startup is due to S3 resume */
	uint32_t min_dvfs_voltage; /* minimum voltage for the CPU domain */
	uint32_t slope_adj_factor; /* scaling factor when adjusting voltage */
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
   AVS "chip mismatch" condition is used to indicate that the wrong AVS firmware
   was loaded for this part.  The correct firmware for the part is required.
*/

/* /\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\ */

/* This defines the different firmware states concerning BBM support.
 * This is returned to host as part of status processing.
 */
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
 * The read-only data is always available unless indicated.
 *
 * The status parameter is updated by the firmware to indicate command complete.
 * The status must show a completion status (non-zero) before a new command can
 * be issued.
 * The status must be cleared by host before writing new command.
 * Command will be cleared by AVS FW when status has been posted and command
 * has completed.
 */
struct at_runtime {
	uint32_t command;
	uint32_t status;

	/* This is the data firmware supplies: */
	uint32_t voltage0;	/* current voltage for STB (in milli-volts) */
	int32_t temperature0;	/* current temperature for STB (in 1000th) */
	uint32_t PV0;	/* initial predicted voltage value (in milli-volts) */
	uint32_t MV0;	/* initial measured voltage value (in milli-volts) */

	uint32_t command_p0;	/* used to pass parameters for commands */
	uint32_t command_p1;	/* and receive resullts when applicable */
	uint32_t command_p2;
	uint32_t command_p3;

	uint32_t revision;	/* the revision of the AVS firmware */
	enum power_state state;	/* the current state of the firmware */
	uint32_t heartbeat;	/* updated everytime the process runs */
	uint32_t avs_magic;	/* set to magic value to show firmware
				 *  actually started */
	int32_t sigma_hvt;	/* parts HVT sigma value (if enabled) */
	int32_t sigma_svt;	/* parts SVT sigma value (if enabled) */

#ifdef AVS_DUAL_DOMAINS
	/* These next values are only used when two AVS voltage domains exist.
	 * i.e. 7172, 7268.  Zeros will be returned for other platforms. */
	uint32_t voltage1;	/* (see above for units) */
	int32_t temperature1;
	uint32_t PV1;
	uint32_t MV1;
#endif
} at_runtime;

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
				 * (CM and STB). */

/* The following are only for 7271 and 7268 products */

#define CMD_GET_P_MAP	 0x30	/* Get the current mode and P-map being used. */
#define CMD_SET_P_MAP	 0x31	/* Set the new mode and P-map to be used.
		 *  parameter in command_p0 specifies MODE.
		 *  parameters in command_p1 & command_p2 are new PMAP. */
#define CMD_GET_P_STATE  0x40	/* Get current AVS P-state.
		 *  parameter in command_p0 is the current state. */
#define CMD_SET_P_STATE  0x41	/* Set AVS P-state.
		 *  parameter in command_p0 is the state to switch to. */

/* Command parameter values for 'CMD_SET_P_MAP' and 'CMD_GET_P_MAP' commands */
/* 'command_p0' values specify the mode to be used or is being used. */
#define AVS_MODE  0x0
#define DFS_MODE  0x1
#define DVS_MODE  0x2
#define DVFS_MODE 0x3
/* 'command_p1': unused:31-24, mdiv_p0:23-16,
unused:15-14, pdiv:13-10 , ndiv_int:9-0 */
#define NDIV_INT_SHIFT 0
#define NDIV_INT_MASK  0x3ff
#define PDIV_SHIFT     10
#define PDIV_MASK      0xf
#define MDIV_P0_SHIFT  16
#define MDIV_P0_MASK   0xff
/* 'command_p2': mdiv_p4:31-24, mdiv_p3:23-16, mdiv_p2:15:8, mdiv_p1:7:0  */
#define MDIV_P1_SHIFT  0
#define MDIV_P1_MASK   0xff
#define MDIV_P2_SHIFT  8
#define MDIV_P2_MASK   0xff
#define MDIV_P3_SHIFT  16
#define MDIV_P3_MASK   0xff
#define MDIV_P4_SHIFT  24
#define MDIV_P4_MASK   0xff

/* Command parameter values for 'CMD_GET_P_STATE' and
'CMD_SET_P_STATE' commands */
/* These are the possible 'command_p0' values for CMD_SET_P_STATE command */
#define P_STATE_0  0
#define P_STATE_1  1
#define P_STATE_2  2
#define P_STATE_3  3
#define P_STATE_4  4

/* Responses: */
#define RSP_SUCCESS	0xF0	/* Command/notification accepted */
#define RSP_FAILURE	0xFF	/* Command/notification rejected */
#define RSP_INVALID	0xF1	/* Invalid command/notification (unknown) */
#define RSP_NO_SUPPORT	0xF2	/* Non-AVS modes are not supported */
#define RSP_NO_MAP	0xF3	/* Cannot set P-State until P-Map supplied */
/* Cannot change P-Map after initial P-Map set */
#define RSP_MAP_SET	0xF4

#endif /*_AVS_FW_H_*/
