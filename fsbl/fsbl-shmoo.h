/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef __FSBL_SHMOO_H__
 #define __FSBL_SHMOO_H__

#include <bchp_common.h>
#include <fsbl-common.h>

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
#define EDIS_NPHY	0x2 /* FIXME: should snoop from RDB on config */

#define EDIS_OFFS	BCHP_MEMC_EDIS_0_1_REG_START - \
			BCHP_MEMC_EDIS_0_0_REG_START

#define MEMC(n) 	BCHP_MEMC_GEN_##n##_CORE_REV_ID
#define DDR_PHY(n)	BCHP_DDR34_PHY_CONTROL_REGS_##n##_REVISION
#define SHIM_PHY(n)	BCHP_SHIMPHY_ADDR_CNTL_##n##_CONFIG
#define MEMC_EDIS(n)	BCHP_MEMC_EDIS_##n##_0_REG_START

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

#elif defined(BCHP_PHY_CONTROL_REGS_0_REG_START)

#include <bchp_phy_control_regs_0.h>
#include <bchp_mc_glb_0.h>

#ifdef BCHP_PHY_CONTROL_REGS_1_REG_START
 #include <bchp_phy_control_regs_1.h>
 #include <bchp_mc_glb_1.h>
#endif

#ifdef BCHP_PHY_CONTROL_REGS_2_REG_START
 #include <bchp_phy_control_regs_2.h>
 #include <bchp_mc_glb_2.h>
#endif

#define EDIS_NPHY	0x2 /* FIXME: should snoop from RDB */
#define EDIS_OFFS	BCHP_MC_EDIS_1_0_REG_START - \
			BCHP_MC_EDIS_0_0_REG_START

#define MEMC(n) 	BCHP_MC_GLB_##n##_VERS
#define DDR_PHY(n)	BCHP_PHY_CONTROL_REGS_##n##_REVISION
/* EDIS_${index_reg_block}_${index_instance}_REG_START */
#define MEMC_EDIS(n)	BCHP_MC_EDIS_0_##n##_REG_START

#define SHMOO_PARAMS(n) { \
	.memc_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC(n), \
	.phy_reg_base	= BCHP_PHYSICAL_OFFSET + DDR_PHY(n), \
	.shim_reg_base	= 0, \
	.edis_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC_EDIS(n), \
}

static const struct memsys_params __maybe_unused shmoo_params[] = {
	SHMOO_PARAMS(0),
#ifdef BCHP_PHY_CONTROL_REGS_1_REG_START
	SHMOO_PARAMS(1),
#endif
#ifdef BCHP_PHY_CONTROL_REGS_2_REG_START
	SHMOO_PARAMS(2),
#endif
};


#elif defined(BCHP_DDR34_PHY_CONTROL_REGS_A_0_REG_START)

#include <bchp_ddr34_phy_common_regs_0.h>
#include <bchp_ddr34_phy_control_regs_a_0.h>
#include <bchp_memc_edis_0_0.h>
#include <bchp_memc_gen_0.h>
#include <bchp_shimphy_addr_cntl_0.h>

#define MEMC(n) 	BCHP_MEMC_GEN_##n##_CORE_REV_ID
#define DDR_PHY(n)	BCHP_DDR34_PHY_COMMON_REGS_##n##_PRIMARY_REVISION
#define SHIM_PHY(n)	BCHP_SHIMPHY_ADDR_CNTL_##n##_CONFIG
#define MEMC_EDIS(n)	BCHP_MEMC_EDIS_##n##_0_REV_ID

#define SHMOO_PARAMS(n) { \
	.memc_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC(n), \
	.phy_reg_base	= BCHP_PHYSICAL_OFFSET + DDR_PHY(n), \
	.shim_reg_base	= BCHP_PHYSICAL_OFFSET + SHIM_PHY(n), \
	.edis_reg_base	= BCHP_PHYSICAL_OFFSET + MEMC_EDIS(n), \
}

#define EDIS_NPHY	0x2 /* FIXME: should snoop from RDB on config */
#define EDIS_OFFS	BCHP_MEMC_EDIS_0_1_REG_START - \
			BCHP_MEMC_EDIS_0_0_REG_START

static const struct memsys_params __maybe_unused shmoo_params[] = {
	SHMOO_PARAMS(0),
};
/* ------------------------------------- */


#else /* TBD: Other DDR controller types. */

#define EDIS_NPHY	0
#define EDIS_OFFS	0
static const struct memsys_params __maybe_unused shmoo_params[] = {
	{0, 0, 0, 0},
};

#endif /* !BCHP_MEMC_EDIS_0_0_REG_START */


#endif /* __FSBL_SHMOO_H__ */

