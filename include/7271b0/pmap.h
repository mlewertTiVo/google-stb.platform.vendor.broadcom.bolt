/***************************************************************************
 *     Copyright (c) 2012-2017, Broadcom
 *     All Rights Reserved
 *     Confidential Property of Broadcom
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/***************************************************************************
*
* This file is generated by generate_pmap_file.pl for supporting platforms.
*
* This file contains a list of PMAPs and the corresponding register settings
*
***************************************************************************/

#ifndef BCHP_PMAP_PRIV_H__
#define BCHP_PMAP_PRIV_H__

#include <bchp_clkgen.h>

#define AVS_DOC_VER 7

enum pmaps {
	PMap_e0 = 0,
	PMap_e1 = 1,
	PMap_e2 = 2,
	PMap_eMax,
};

struct pmapParameters {
	enum pmaps pmapId;
	unsigned int num_domains;
	char *desc;
};

#ifndef AVS_ONCE
#define AVS_ONCE
static const struct pmapParameters pmapTable[] = {
	{ PMap_e0, 2, "PMap0"},
	{ PMap_e1, 1, "PMap1"},
	{ PMap_e2, 2, "PMap2"},
};

static const int PMAP_MAX = sizeof(pmapTable)/sizeof(pmapTable[0]);

struct pmapReg {
	uint32_t reg;
	uint32_t mask;
	uint8_t shift;
};

static const struct pmapReg pmapMuxes[] = {
	{ BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_SID_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_SID_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_CPU_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_CPU_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_CORE_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_HVDP0_TOP_INST_CLOCK_SELECT_HVDP0_CORE_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_STB_V3D_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_V3D_TOP_INST_CLOCK_SELECT_V3D_CORE_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_V3D_TOP_INST_CLOCK_SELECT_V3D_CORE_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_STB_VEC_AIO_GFX_TOP_INST_M2MC0,
	BCHP_CLKGEN_STB_VEC_AIO_GFX_TOP_INST_M2MC0_CLOCK_SELECT_M2MC0_MASK,
	BCHP_CLKGEN_STB_VEC_AIO_GFX_TOP_INST_M2MC0_CLOCK_SELECT_M2MC0_SHIFT
	},
	{ BCHP_CLKGEN_STB_RAAGA_DSP_TOP_0_INST_RAAGA0,
	BCHP_CLKGEN_STB_RAAGA_DSP_TOP_0_INST_RAAGA0_DSP_CLOCK_SELECT_RAAGA0_MASK,
	BCHP_CLKGEN_STB_RAAGA_DSP_TOP_0_INST_RAAGA0_DSP_CLOCK_SELECT_RAAGA0_SHIFT
	},
	{ BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT_HVDS0_CORE_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT_HVDS0_CORE_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT,
	BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT_HVDS0_CPU_CLOCK_SELECT_MASK,
	BCHP_CLKGEN_STB_HVDS0_TOP_INST_CLOCK_SELECT_HVDS0_CPU_CLOCK_SELECT_SHIFT
	},
	{ BCHP_CLKGEN_ITU656_0_MUX_SELECT,
	BCHP_CLKGEN_ITU656_0_MUX_SELECT_VEC_ITU656_0_CLOCK_MASK,
	BCHP_CLKGEN_ITU656_0_MUX_SELECT_VEC_ITU656_0_CLOCK_SHIFT
	},
	{ BCHP_CLKGEN_SMARTCARD_MUX_SELECT,
	BCHP_CLKGEN_SMARTCARD_MUX_SELECT_SC0_CLOCK_MASK,
	BCHP_CLKGEN_SMARTCARD_MUX_SELECT_SC0_CLOCK_SHIFT
	},
	{ BCHP_CLKGEN_SMARTCARD_MUX_SELECT,
	BCHP_CLKGEN_SMARTCARD_MUX_SELECT_SC1_CLOCK_MASK,
	BCHP_CLKGEN_SMARTCARD_MUX_SELECT_SC1_CLOCK_SHIFT
	},
};

#define PMAP_MAX_MUXES (sizeof(pmapMuxes)/sizeof(pmapMuxes[0]))

static const uint8_t pmapMuxValues[][PMAP_MAX_MUXES] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
};

static const struct pmapReg pmapDividers[] = {
	{ BCHP_CLKGEN_PLL_RAAGA_PLL_DIV,
	BCHP_CLKGEN_PLL_RAAGA_PLL_DIV_PDIV_MASK,
	BCHP_CLKGEN_PLL_RAAGA_PLL_DIV_PDIV_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_DIV,
	BCHP_CLKGEN_PLL_AVX_PLL_DIV_PDIV_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_DIV_PDIV_SHIFT
	},
	{ BCHP_CLKGEN_PLL_RAAGA_PLL_CHANNEL_CTRL_CH_0,
	BCHP_CLKGEN_PLL_RAAGA_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_MASK,
	BCHP_CLKGEN_PLL_RAAGA_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_0,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_1,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_2,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_3,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_4,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_SHIFT
	},
	{ BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_2,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_MASK,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_SHIFT
	},
	{ BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_3,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_MASK,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_SHIFT
	},
	{ BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_4,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_MASK,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_SHIFT
	},
	{ BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_5,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_5_MDIV_CH5_MASK,
	BCHP_CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_5_MDIV_CH5_SHIFT
	},
};

#define PMAP_MAX_DIVIDERS \
	(sizeof(pmapDividers)/sizeof(pmapDividers[0]))

static const uint8_t pmapDividerValues[][PMAP_MAX_DIVIDERS] = {
	{  2,  3,  5, 11,  6,  8,  7,  7,  0,  0,  0,  0,},
	{  2,  3,  5, 11,  6,  8,  7,  7,  0,  0,  0,  0,},
	{  2,  3,  5, 11,  6,  7,  7,  7,  0,  0,  0,  0,},
};

static const struct pmapReg pmapMultipliers[] = {
	{ BCHP_CLKGEN_PLL_RAAGA_PLL_DIV,
	BCHP_CLKGEN_PLL_RAAGA_PLL_DIV_NDIV_INT_MASK,
	BCHP_CLKGEN_PLL_RAAGA_PLL_DIV_NDIV_INT_SHIFT
	},
	{ BCHP_CLKGEN_PLL_AVX_PLL_DIV,
	BCHP_CLKGEN_PLL_AVX_PLL_DIV_NDIV_INT_MASK,
	BCHP_CLKGEN_PLL_AVX_PLL_DIV_NDIV_INT_SHIFT
	},
};

#define PMAP_MAX_MULTIPLIERS \
	(sizeof(pmapMultipliers)/sizeof(pmapMultipliers[0]))

static const uint8_t pmapMultiplierValues[][PMAP_MAX_MULTIPLIERS] = {
	{ 130, 200,},
	{ 130, 200,},
	{ 130, 200,},
};

#endif
#endif
