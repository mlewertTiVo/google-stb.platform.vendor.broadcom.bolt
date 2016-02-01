/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
***************************************************************************/

#ifndef AVS_REGS_H__
#define AVS_REGS_H__

#include "bchp_common.h"

/* This will be defined on devices with dual monitors */
#ifdef BCHP_AVS_RO_REGISTERS_0_1_REG_START
#include "bchp_avs_pvt_mntr_config_1.h"
#include "bchp_avs_ro_registers_0_1.h"
#else
#include "bchp_avs_pvt_mntr_config.h"
#include "bchp_avs_ro_registers_0.h"
#endif

/* This will be defined IF the block exists in the new hardware */
#ifdef BCHP_AVS_TMON_REG_START
#include "bchp_avs_tmon.h"
#define AVS_TMON_EXISTS
#endif

/* Use a common definition for these registers */
#ifdef AVS_DUAL_MONITORS
#define BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS \
	BCHP_AVS_RO_REGISTERS_0_1_PVT_1V_0_MNTR_STATUS

#define BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS_data_MASK \
	BCHP_AVS_RO_REGISTERS_0_1_PVT_1V_0_MNTR_STATUS_data_MASK

#define BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS \
	BCHP_AVS_RO_REGISTERS_0_1_PVT_TEMPERATURE_MNTR_STATUS

#define BCHP_AVS_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUS_data_MASK \
	BCHP_AVS_RO_REGISTERS_0_1_PVT_TEMPERATURE_MNTR_STATUS_data_MASK
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Function Prototypes */

#ifdef __cplusplus
}
#endif

#endif /*AVS_REGS_H__*/

