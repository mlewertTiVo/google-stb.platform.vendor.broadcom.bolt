/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/
#ifndef MEMSYS_H
#define MEMSYS_H

#include <bchp_common.h>
#include <bchp_ddr34_phy_common_regs_0.h>
#include <bchp_memc_gen_0.h>
#include <bchp_shimphy_addr_cntl_0.h>
#include <memsys-if.h>

#ifdef BCHP_DDR34_PHY_COMMON_REGS_1_REG_START
#include <bchp_ddr34_phy_common_regs_1.h>
#include <bchp_memc_gen_1.h>
#include <bchp_shimphy_addr_cntl_1.h>
#endif

/* TODO: remove dependency on the following from bsu.c
 *  - EDIS_NPHY
 *  - EDIS_OFFS
 *  - shmoo_params
 */
#define EDIS_NPHY	0
#define EDIS_OFFS	0
#define MEMC(n)		BCHP_MEMC_GEN_##n##_CORE_REV_ID
#define DDR_PHY(n)	BCHP_DDR34_PHY_COMMON_REGS_##n##_PRIMARY_REVISION
#define SHIM_PHY(n)	BCHP_SHIMPHY_ADDR_CNTL_##n##_CONFIG

#define SHMOO_PARAMS(n) { \
	.memc_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC(n), \
	.phy_reg_base	= BCHP_PHYSICAL_OFFSET + DDR_PHY(n), \
	.shim_reg_base	= BCHP_PHYSICAL_OFFSET + SHIM_PHY(n), \
	.edis_reg_base	= 0,\
}

static const struct memsys_params __maybe_unused shmoo_params[] = {
	SHMOO_PARAMS(0),
#ifdef BCHP_DDR34_PHY_COMMON_REGS_1_REG_START
	SHMOO_PARAMS(1),
#endif

};

#endif /* MEMSYS_H */
