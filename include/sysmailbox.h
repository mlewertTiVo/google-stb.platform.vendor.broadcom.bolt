/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

/*
 * Various system specific IPC glue
 */

#ifndef _SYSMAILBOX_H
#define _SYSMAILBOX_H

#if (BCHP_CHIP == 7145) && CFG_BATTERY_BACKUP
/* These aren't defined in the RDB for some reason
*/
#define BCHP_MBOX_CPUC_DATA0_POWER_STATE_ac_is_present   1
#define BCHP_MBOX_CPUC_DATA0_POWER_STATE_on_battery      2
#define BCHP_MBOX_CPUC_DATA0_POWER_STATE_on_battery_low  3


#define BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_MASK			0x0F
#define BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_SHIFT		0
#define BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_unknown		0
#define BCHP_MBOX_CPUC_DATA3_STB_RUN_STATE_bootloader	1

#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_MASK		0x30
#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_SHIFT		4
#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_unknown	0
#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_no_power	1
#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_low_power	2
#define BCHP_MBOX_CPUC_DATA3_STB_POWER_STATE_high_power 3
#endif /* 7145 battery */


#endif /* _SYSMAILBOX_H */
