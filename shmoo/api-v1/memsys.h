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
#include <memsys-if.h>

#if defined(BCHP_DDR34_PHY_CONTROL_REGS_0_REG_START)

#include <bchp_ddr34_phy_control_regs_0.h>
#include <bchp_memc_gen_0.h>
#include <bchp_shimphy_addr_cntl_0.h>

#ifdef BCHP_DDR34_PHY_CONTROL_REGS_1_REG_START
 #include <bchp_ddr34_phy_control_regs_1.h>
 #include <bchp_memc_gen_1.h>
 #include <bchp_shimphy_addr_cntl_1.h>
#endif

#ifdef BCHP_DDR34_PHY_CONTROL_REGS_2_REG_START
 #include <bchp_ddr34_phy_control_regs_2.h>
 #include <bchp_memc_gen_2.h>
 #include <bchp_shimphy_addr_cntl_2.h>
#endif

/* EDIS HW block information
       bits[3:0]  - Number of EDIS HW blocks for each PHY
       bits[31:4] - Register offset between EDIS HW blocks (0 for one EDIS)
*/
#define EDIS_NPHY 0x2 /* FIXME: should snoop from RDB on config */
#define EDIS_OFFS (BCHP_MEMC_EDIS_0_1_REG_START - BCHP_MEMC_EDIS_0_0_REG_START)

#define MEMC(n)		BCHP_MEMC_GEN_##n##_CORE_REV_ID
#define SHIM_PHY(n)	BCHP_SHIMPHY_ADDR_CNTL_##n##_CONFIG
#define MEMC_EDIS(n)	BCHP_MEMC_EDIS_##n##_0_REG_START

#ifdef BCHP_DDR34_PHY_CONTROL_REGS_0_PRIMARY_REVISION
 #define DDR_PHY(n)	BCHP_DDR34_PHY_CONTROL_REGS_##n##_PRIMARY_REVISION
#else
 #define DDR_PHY(n)	BCHP_DDR34_PHY_CONTROL_REGS_##n##_REVISION
#endif

#define SHMOO_PARAMS(n) { \
	.memc_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC(n), \
	.phy_reg_base	= BCHP_PHYSICAL_OFFSET + DDR_PHY(n), \
	.shim_reg_base	= BCHP_PHYSICAL_OFFSET + SHIM_PHY(n), \
	.edis_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC_EDIS(n), \
}

static const struct memsys_params __maybe_unused shmoo_params[] = {
	SHMOO_PARAMS(0),
#ifdef BCHP_DDR34_PHY_CONTROL_REGS_1_REG_START
	SHMOO_PARAMS(1),
#endif
#ifdef BCHP_DDR34_PHY_CONTROL_REGS_2_REG_START
	SHMOO_PARAMS(2),
#endif
};


#elif defined(BCHP_DDR34_PHY_CONTROL_REGS_A_0_REG_START) /* LPDDR4 */

#include <bchp_ddr34_phy_common_regs_0.h>
#include <bchp_ddr34_phy_control_regs_a_0.h>
#include <bchp_ddr34_phy_control_regs_b_0.h>
#include <bchp_memc_edis_0_0.h>
#include <bchp_memc_gen_0.h>
#include <bchp_shimphy_addr_cntl_0.h>

#define MEMC(n)		BCHP_MEMC_GEN_##n##_CORE_REV_ID
#define DDR_PHY(n)	BCHP_DDR34_PHY_COMMON_REGS_##n##_PRIMARY_REVISION
#define SHIM_PHY(n)	BCHP_SHIMPHY_ADDR_CNTL_##n##_CONFIG
#define MEMC_EDIS(n)	BCHP_MEMC_EDIS_##n##_0_REV_ID

#define SHMOO_PARAMS(n) { \
	.memc_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC(n), \
	.phy_reg_base	= BCHP_PHYSICAL_OFFSET + DDR_PHY(n), \
	.shim_reg_base	= BCHP_PHYSICAL_OFFSET + SHIM_PHY(n), \
	.edis_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC_EDIS(n), \
}

#define EDIS_NPHY 0x2 /* FIXME: should snoop from RDB on config */

#ifdef BCHP_MEMC_EDIS_0_1_REG_START
#define EDIS_OFFS (BCHP_MEMC_EDIS_0_1_REG_START - BCHP_MEMC_EDIS_0_0_REG_START)
#else
#define EDIS_OFFS 0
#endif

static const struct memsys_params __maybe_unused shmoo_params[] = {
	SHMOO_PARAMS(0),
};


#else

#error "Add support for the new memory controller!!"

#endif

#endif /* MEMSYS_H */