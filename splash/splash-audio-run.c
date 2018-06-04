/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "error.h"
#include "timer.h"
#include "common.h"
#include "board.h"
#include "board_init.h"
#include "bsp_config.h"


#include "splash-api.h"
#include "splash_file.h"
#include "splash_script_load.h"

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_REG_START
#include "bchp_aud_fmm_iop_out_dac_ctrl_0.h"
#endif
#include "bchp_aud_misc.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_aud_fmm_src_ctrl0.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_aud_fmm_iop_out_mai_0.h"
#include "bchp_aud_fmm_iop_out_spdif_0.h"
#include "bchp_clkgen.h"
#include "bchp_common.h"
#include "bchp_hdmi.h"
#include "bchp_hdmi_rm.h"
#include "bchp_hdmi_tx_phy.h"

#ifdef BCHP_HIFIDAC_CTRL_0_REG_START
#include "bchp_hifidac_ctrl_0.h"
#endif
#ifdef BCHP_HIFIDAC_RM_0_REG_START
#include "bchp_hifidac_rm_0.h"
#endif
#include "bchp_memc_gen_0.h"
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
#include "bchp_aud_fmm_iop_out_i2s_stereo_0.h"
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_REG_START
#include "bchp_aud_fmm_iop_out_i2s_stereo_1.h"
#endif
#ifdef BCHP_VCXO_0_RM_REG_START
#include "bchp_vcxo_0_rm.h"
#endif

/* Ring buffer addresses are 64-bit registers */
#ifdef BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR_reserved0_SHIFT
#define AUD_RINGBUF_IS_64BIT
#endif
#ifdef AUD_RINGBUF_IS_64BIT
#define BDEV_AUD_WRITE BDEV_WR64
#else
#define BDEV_AUD_WRITE BDEV_WR
#endif

int splash_audio_script_run(unsigned int size, unsigned int address,
				uint32_t repeatcount, SplashData *splashData)
{
	uint32_t reg = 0, i, data, data0, data1;
	unsigned int addr_increment, count;
#ifdef AUD_RINGBUF_IS_64BIT
	uint64_t addr;
#else
	uint32_t addr;
#endif
#ifdef BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB0_BASE_START_MASK
	uint32_t memtop, memlow, shift;
	int rc;
	struct ddr_info *ddr;
#endif
	struct board_type *b;

#ifdef BCHP_CLKGEN_ONOFF_ANA_PLL4_1P8V_TS28HPM_6MX_2MR_FC_X_E_PLLAUDIO0_INST_SEL
	BDEV_WR(
	BCHP_CLKGEN_ONOFF_ANA_PLL4_1P8V_TS28HPM_6MX_2MR_FC_X_E_PLLAUDIO0_INST_SEL,
	reg);
#endif

#ifdef BCHP_CLKGEN_ONOFF_ANA_PLL4_1P8V_TS28HPM_6MX_2MR_FC_X_E_PLLAUDIO0_INST_SEL
	BDEV_WR(
	BCHP_CLKGEN_ONOFF_ANA_PLL4_RFMOD_1P8V_TS28HPM_6MX_2MR_NP_X_E_PLLAUDIO1_INST_SEL,
	reg);
#endif
#ifdef BCHP_MEMC_GEN_0_MSA_MODE_CLOCK_GATE_MASK
	reg = BDEV_RD(BCHP_MEMC_GEN_0_MSA_MODE);
	reg &= (~BCHP_MEMC_GEN_0_MSA_MODE_CLOCK_GATE_MASK);
	BDEV_WR(BCHP_MEMC_GEN_0_MSA_MODE, reg);
#endif

#ifdef BCHP_HIFIDAC_CTRL_0_REG_START
	reg = 0;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_INIT, reg);
	reg = 0x29FC8;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_CONFIG, reg);
	reg = 0x02000698;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_MUTECTRL, reg);
	reg = 0;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_MUTECTRL_DACONLY, reg);
	reg = 0;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_TEST, reg);

	reg = BDEV_RD(BCHP_HIFIDAC_CTRL_0_RANGE);
	reg &= (~BCHP_HIFIDAC_CTRL_0_RANGE_ASRCOUT_MASK);
	reg |= (2 << BCHP_HIFIDAC_CTRL_0_RANGE_ASRCOUT_SHIFT);
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_RANGE, reg);

	reg = BDEV_RD(BCHP_HIFIDAC_CTRL_0_PEAK_CONFIG);
	reg |= BCHP_HIFIDAC_CTRL_0_PEAK_CONFIG_PEAK_ENABLE_MASK;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_PEAK_CONFIG, reg);

	reg = 0xce10;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_PEAK_GAIN, reg);
	reg = 0x1c1a1;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_PEAK_A1, reg);
	reg = 0x330bb;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_PEAK_A2, reg);
	reg = 0x1ffff;
	BDEV_WR(BCHP_HIFIDAC_CTRL_0_SCALE, reg);
#endif

#ifdef BCHP_HIFIDAC_RM_0_REG_START
	reg = 0x200;
	BDEV_WR(BCHP_HIFIDAC_RM_0_RATE_RATIO, reg);
	reg = 0x6502;
	BDEV_WR(BCHP_HIFIDAC_RM_0_SAMPLE_INC, reg);
	reg = 0xe9045;
	BDEV_WR(BCHP_HIFIDAC_RM_0_PHASE_INC, reg);
	reg = 0x240;
	BDEV_WR(BCHP_HIFIDAC_RM_0_CONTROL, reg);
#endif

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_REG_START
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL_audioh_cfg_dem_tone_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL, reg);

	reg = 0;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_CTL_A_L, reg);

	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_CTL_A_R, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1);
	reg |= (3 << BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_0_STB_DACbuf_ibctl_SHIFT);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_0_STB_DACbuf_ibctl_MASK);

	reg = 0x40ffffff;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_SEED_S0_B_L, reg);
	reg = 0x40ffffff;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_SEED_S0_B_R, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL_HS_enable_MASK);
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL, reg);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL_HS_enable_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_MISC_CTRL, reg);

	reg = 0xf4;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_CTL_B_L, reg);
	reg = 0xf4;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_HS_DSM_DITHER_CTL_B_R, reg);

	/* Power up the DAC */
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1_STB_reset_flipflops_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1, reg);

	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1_STB_reset_flipflops_MASK);
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_1, reg);
#endif

#ifdef BCHP_AUD_MISC_CTRL_STB_power_rail_OK_MASK
	reg = BDEV_RD(BCHP_AUD_MISC_CTRL);
	reg |= BCHP_AUD_MISC_CTRL_STB_power_rail_OK_MASK;
	BDEV_WR(BCHP_AUD_MISC_CTRL, reg);
#endif

	bolt_msleep(1);

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_2
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_2);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_2_STB_pu_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_2, reg);
#endif

#ifdef	BCHP_AUD_MISC_CTRL_STB_ready4sample_MASK
	do {
		bolt_msleep(1);

		reg = BDEV_RD(BCHP_AUD_MISC_CTRL);
	} while (!(BCHP_AUD_MISC_CTRL |
		BCHP_AUD_MISC_CTRL_STB_ready4sample_MASK));
#endif
	reg = 0x18A0;
	BDEV_WR(BCHP_VCXO_0_RM_CONTROL, reg);

	reg = BDEV_RD(BCHP_VCXO_0_RM_RATE_RATIO);
	reg &= (~BCHP_VCXO_0_RM_RATE_RATIO_DENOMINATOR_MASK);
	reg |= (0x20 << BCHP_VCXO_0_RM_RATE_RATIO_DENOMINATOR_SHIFT);
	BDEV_WR(BCHP_VCXO_0_RM_RATE_RATIO, reg);

	reg = BDEV_RD(BCHP_VCXO_0_RM_SAMPLE_INC);
	reg &= (~BCHP_VCXO_0_RM_SAMPLE_INC_NUMERATOR_MASK);
	reg |= (0x1d << BCHP_VCXO_0_RM_SAMPLE_INC_NUMERATOR_SHIFT);
	BDEV_WR(BCHP_VCXO_0_RM_SAMPLE_INC, reg);

	reg = BDEV_RD(BCHP_VCXO_0_RM_SAMPLE_INC);
	reg &= (~BCHP_VCXO_0_RM_SAMPLE_INC_SAMPLE_INC_MASK);
	reg |= (0x3 << BCHP_VCXO_0_RM_SAMPLE_INC_SAMPLE_INC_SHIFT);
	BDEV_WR(BCHP_VCXO_0_RM_SAMPLE_INC, reg);

	reg = 0x83126;
	BDEV_WR(BCHP_VCXO_0_RM_PHASE_INC, reg);
#ifdef BCHP_VCXO_0_RM_INTEGRATOR_HI
	reg = 0x0;
	BDEV_WR(BCHP_VCXO_0_RM_INTEGRATOR_HI, reg);
	reg = 0x0;
	BDEV_WR(BCHP_VCXO_0_RM_INTEGRATOR_LO, reg);
#endif
	reg = 0x1;
	BDEV_WR(BCHP_VCXO_0_RM_CONTROL, reg);
	reg = 0x2000064;
	BDEV_WR(BCHP_VCXO_0_RM_FORMAT, reg);
	reg = 0x10000000;
	BDEV_WR(BCHP_VCXO_0_RM_OFFSET, reg);
	reg = 0x8A0;
	BDEV_WR(BCHP_VCXO_0_RM_CONTROL, reg);

#ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_AUDIO0_OSCREF_CMOS_CLOCK_MASK
	reg = BDEV_RD(BCHP_CLKGEN_INTERNAL_MUX_SELECT);
	reg &= (~BCHP_CLKGEN_INTERNAL_MUX_SELECT_AUDIO0_OSCREF_CMOS_CLOCK_MASK);
	BDEV_WR(BCHP_CLKGEN_INTERNAL_MUX_SELECT, reg);
#endif

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG);
	reg |= (BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG_CLOCK_ENABLE_MASK);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG_DATA_ENABLE_MASK);
	reg |= (0x1 << BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG_DATA_ENABLE_SHIFT);
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, reg);

	/* Initializing Audio Data Path 0 Module */
	for (addr = BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE;
	addr < BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE;
	addr += 16) {
		BDEV_WR(addr, 0x800000);
		BDEV_WR(addr + 4, 0);
		BDEV_WR(addr + 8, 0);
		BDEV_WR(addr + 12, 0x800000);
	}

	for (addr = BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE;
	addr < BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi_ARRAY_BASE;
	addr += 16) {
		BDEV_WR(addr, 0x100000);
		BDEV_WR(addr + 4, 0x100000);
		BDEV_WR(addr + 8, 0x100000);
		BDEV_WR(addr + 12, 0x100000);
	}

	for (addr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi_ARRAY_BASE;
	addr < BCHP_AUD_FMM_DP_CTRL0_SOFT_COEFCi_ARRAY_BASE;
	addr += 4)
		BDEV_WR(addr, 0x800000);


	/* Initialize FCI ID map */
	for (i = 0, addr = BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE;
	i < 36; i++) {

		if (i < 12)
			reg = 0x80 + i;
		else if	(i < 24)
			reg = 0xc0 + i - 12;
		else
			reg = 0x100 + (i - 24) * 2 + 1;
		BDEV_WR(addr + (i * 4), reg);
	}

	for (i = 0, addr = BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE;
	i < 12; i++)
		BDEV_WR(addr + (4 * i),
		i<<BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_MIXER_GROUP_BEGIN_SHIFT);

	/* Initializing SRC0 */
	for (i = 0, addr = BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_BASE;
	i < 12; i++)
		BDEV_WR(addr + (4 * i), (i << 16) + i);

	for (i = 0, addr = BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE;
	i < 12; i++)
		BDEV_WR(addr + (4 * i), 0);

	/* Initializing BF */
	b = board_thisboard();
	if (!b)
		return 1;

#ifdef BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB0_BASE_START_MASK
	reg = BDEV_RD(BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG);

	/* Set the end byte address in the base region for SCB1 and SCB2 to 0x0 */
	/* Set the start byte address in the base region for SCB1 and SCB2 to 0xF */
#ifdef BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB1_BASE_START_MASK
	reg |= BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB1_BASE_START_MASK;
	reg &= ~BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB1_BASE_END_MASK;
#endif
#ifdef BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB2_BASE_START_MASK
	reg |= BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB2_BASE_START_MASK;
	reg &= ~BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB2_BASE_END_MASK;
#endif

	/* Update the start and end byte addresses for the SCBs  */
	for (i = 0; i < b->nddr; i++) {
		rc = splash_glue_getmem(i, &memtop, &memlow);
		if (rc)
			continue;
		ddr = board_find_ddr(b, i);
		if (!ddr)
			continue;
		/* Use the base address from the ddr config command to update the
		  * start address for the SCBs
		  */
		shift = BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB0_BASE_START_SHIFT \
			+ (8 * i);
		reg &= ~(0xf << shift);
		reg |= ((ddr->base_mb >> 8) << shift);

		shift = BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG_SCB0_BASE_END_SHIFT \
			+ (8 * i);
		reg &= ~(0xf << shift);
		reg |= ((memtop >> 28) << shift);
	}
	BDEV_WR(BCHP_AUD_FMM_BF_CTRL_MISC_CONFIG, reg);
#endif

	for (addr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
		addr <=  BCHP_AUD_FMM_BF_CTRL_REG_END;
		addr += sizeof(addr))
		BDEV_AUD_WRITE(addr, 0);

	data = address;
	addr_increment = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR -
		BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
	/* Populate all the source and destination channel ring buffers.
	 * The count is derived by subtracting final destination channel ring
	 * buffer address and the first source channel ring buffer address.
	 * These may not be contiguous in the future chips.
	 */
	count = 2 * (BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_END + 1 +
			BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_END + 1);
	for (i = 0, addr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
		i < count; addr += addr_increment, i++) {
		data0 = data + i * 0x800;
		data1 = data0 + size - 1;
		BDEV_AUD_WRITE(addr, data0);
		BDEV_AUD_WRITE(addr + sizeof(addr), data0);
		BDEV_AUD_WRITE(addr + sizeof(addr) * 2, data0);
		BDEV_AUD_WRITE(addr + sizeof(addr) * 3, data1);
	}

	if (repeatcount != 0) {
		reg = BDEV_RD(BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR);
		BDEV_AUD_WRITE(BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR,
			reg - 1);
	}

	for (i = 0, addr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE;
						i < 15; addr += 4, i++)
		BDEV_WR(addr, 0x420);

	for (i = 0, addr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_GRPi_ARRAY_BASE;
	i < 15; addr += 4, i++)
		BDEV_WR(addr, i);

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0_FCI_ID_MASK);
	reg |= (0x100 << BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0_FCI_ID_SHIFT);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0_GROUP_ID_MASK);
	reg |= (0x100 << BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0_GROUP_ID_SHIFT);
	reg |= BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, reg);

	for (i = 0, addr = BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_ARRAY_BASE;
	i < 5; addr += 0x20, i++){
		reg = BDEV_RD(addr);
		reg &= (~BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_MIXER_INPUT_ENA_MASK);
		reg |= (0x1 << BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_MIXER_INPUT_ENA_SHIFT);
		reg &= (~BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_MIXER_INPUT_PB_NUMBER_MASK);
		BDEV_WR(addr, reg);
	}

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_ARRAY_BASE);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_FCI_ID_MASK);
	reg |= 0x102;
	reg |= BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_GROUP_ID_MASK;
	reg |= BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_ARRAY_BASE, reg);

	reg = 0x2000000;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CHANSTAT_0, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL);
	reg |= BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL_DITHER_ENA_MASK;
	reg |= BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL_INSERT_ON_UFLOW_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL);
	reg |= BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL_INSERT_ON_UFLOW_MASK;
	reg |= BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL_DITHER_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, reg);

#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_FCI_ID_MASK);
	reg |= 0x104;
	reg &= (~BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_MASK);
	reg |= (0x2 << BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_SHIFT);
	reg |= BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0, reg);
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_REG_START
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_STREAM_CFG_0);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_FCI_ID_MASK);
	reg |= 0x106;
	reg &= (~BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_MASK);
	reg |= (0x3 << BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0_GROUP_ID_SHIFT);
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_STREAM_CFG_0, reg);
#endif

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_FCI_ID_MASK);
	reg |= 0x108;
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_GROUP_ID_MASK);
	reg |= (0x4 << BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_GROUP_ID_SHIFT);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0, reg);
#endif

	for (i = 0, addr = BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE;
	i < 5; i++){
		reg = BDEV_RD(addr + (4 * i));
		reg |= BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_MIXER_OUTPUT0_ENA_MASK;
		BDEV_WR(addr + (4 * i), reg);
	}

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0);
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_FCI_ID_MASK);
	reg |= 0x108;
	reg &= (~BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_GROUP_ID_MASK);
	reg |= (0x4 << BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_GROUP_ID_SHIFT);
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0, reg);
#endif

	reg = BDEV_RD(BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_ARRAY_BASE);
	reg |= BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_MIXER_INPUT_ENA_MASK;
	reg &= (~BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_MIXER_INPUT_PB_NUMBER_MASK);
	BDEV_WR(BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_ARRAY_BASE, reg);

	if (repeatcount == 0)
		reg = 0x80000420;
	else
		reg = 0x80000020;

	BDEV_WR(BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE+4);
	reg |= BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_MIXER_OUTPUT0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE+4, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_BASE);
	reg |= BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ENABLE_MASK;
	BDEV_WR(BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_BASE, reg);

#ifdef BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0
	reg = BDEV_RD(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0);
	reg |= BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_STREAM_CFG_0, reg);
#endif

	reg = BDEV_RD(BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE+(4*4));
	reg |= BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_MIXER_OUTPUT0_ENA_MASK;
	BDEV_WR(BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE+(4*4), reg);

	reg = BDEV_RD(BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_BASE);
	reg |= BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ENABLE_MASK;
	BDEV_WR(BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_BASE, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE);
	reg |= BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_SOURCEFIFO_ENABLE_MASK;
	BDEV_WR(BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE, reg);

	reg = BDEV_RD(BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE);
	reg |= BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_PLAY_RUN_MASK;
	BDEV_WR(BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE, reg);

	reg = BDEV_RD(BCHP_HDMI_SCHEDULER_CONTROL);
	reg |= BCHP_HDMI_SCHEDULER_CONTROL_HDMI_MODE_REQUEST_MASK;
	BDEV_WR(BCHP_HDMI_SCHEDULER_CONTROL, reg);

	reg = BDEV_RD(BCHP_HDMI_CRP_CFG);
	reg &= ~BCHP_HDMI_CRP_CFG_N_VALUE_MASK;
	reg &= ~BCHP_HDMI_CRP_CFG_INC_N_COUNT_BY_4_ON_HBR_MODE_MASK;
	if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)28)
		reg |= 0x1800; /* 6144 */
	else if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)21)
		reg |= 0x2D80; /* 11648 */
	else
		return -1;
	BDEV_WR(BCHP_HDMI_CRP_CFG, reg);

	reg = BDEV_RD(BCHP_HDMI_CTS_0);
	reg &= ~BCHP_HDMI_CTS_0_CTS_0_MASK;
	if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)28)
		reg |= 0x6978; /* 27000 */
	else if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)21)
		reg |= 0x22551; /* 140625 */
	BDEV_WR(BCHP_HDMI_CTS_0, reg);

	reg = BDEV_RD(BCHP_HDMI_CTS_1);
	reg &= ~BCHP_HDMI_CTS_1_CTS_1_MASK;
	if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)28)
		reg |= 0x6978; /* 27000 */
	else if (splashData->pDispInfo->eDspFmt == (BFMT_VideoFmt)21)
		reg |= 0x22551; /* 140625 */
	BDEV_WR(BCHP_HDMI_CTS_1, reg);

	reg = 0xd; /* 13. Reserved bits must be written 0. */
	BDEV_WR(BCHP_HDMI_RM_RATE_RATIO, reg);

	reg = BDEV_RD(BCHP_HDMI_RM_SAMPLE_INC);
	reg &= ~BCHP_HDMI_RM_SAMPLE_INC_NUMERATOR_MASK;
	reg |= (0x7 << BCHP_HDMI_RM_SAMPLE_INC_NUMERATOR_SHIFT);
	reg &= ~BCHP_HDMI_RM_SAMPLE_INC_SAMPLE_INC_MASK;
	reg |= BCHP_HDMI_RM_SAMPLE_INC_SAMPLE_INC_DEFAULT;
	BDEV_WR(BCHP_HDMI_RM_SAMPLE_INC, reg);

	/* WHAT IS THIS VALUE??? */
	reg = 0x8200000;
	BDEV_WR(BCHP_HDMI_RM_OFFSET, reg);

	reg = BDEV_RD(BCHP_HDMI_TX_PHY_CLK_DIV);
	reg &= ~BCHP_HDMI_TX_PHY_CLK_DIV_VCO_MASK;
	reg |= (0xd << BCHP_HDMI_TX_PHY_CLK_DIV_VCO_SHIFT); /* 13 */
	reg &= ~BCHP_HDMI_TX_PHY_CLK_DIV_RM_MASK;
	reg |= (0x64 << BCHP_HDMI_TX_PHY_CLK_DIV_RM_SHIFT); /* 100 */
	BDEV_WR(BCHP_HDMI_TX_PHY_CLK_DIV, reg);

	return 0;
}

