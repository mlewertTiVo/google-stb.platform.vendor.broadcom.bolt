/***************************************************************************
 *     (c)2002-2009 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: splash_vdc_rulgen.h $
 * $brcm_Revision: 14 $
 * $brcm_Date: 7/19/13 1:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/splash_vdc_rulgen.h $
 * 
 * 14   7/19/13 1:54p pntruong
 * SW7429-701: Added option to scale up splash bmp to fullscreen and
 * support for CLUT bmp file as well.
 *
 * 13   4/4/13 4:37p syang
 * SW7435-676: add back runtime option for display fmt overriding
 *
 * 12   4/2/13 6:22p syang
 * SW7435-676: fix seg fault in the case SPLASH_NUM_SURFACE is 1
 *
 * 11   3/29/13 11:40a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 *
 * 10   10/31/12 6:54p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 *
 * 8   4/18/12 2:47p jessem
 * SW7425-2828: Corrected method on obtaining heap. Added run-time options
 * to set the display format and to disable the 2nd display. Backed-out
 * of creating a separate surface for the the 2nd display.
 *
 * 7   4/9/12 11:25a jessem
 * SW7425-2828: Removed hardcoded RDC scratch registers and use
 * BVDC_Test_Source_GetGfdScratchRegisters() instead. Also, updated the
 * implementation of the splash surface for the 2nd display.
 *
 * 6   8/8/11 11:22p nickh
 * SW7425-878: Pass in both MEMC0 and MEMC1
 *
 * 5   4/8/09 4:29p shyam
 * PR52386 : Add support for DTV platforms
 *
 * 3   4/8/09 12:44p shyam
 * PR52386 : Port splash to nexus Base Build system
 *
 * 2   12/24/08 5:03p katrep
 * PR50711: Add splash support for 7405/7335/7325 and settop chips
 *
 * 1   9/29/08 11:34a erickson
 * PR46184: added splash app
 *
 * 4   10/8/07 7:28p shyam
 * PR 30741 : Add support for No HDMI
 *
 * 2   6/27/07 6:21p shyam
 * PR 30741 : Magnum compliant RDC hooks
 *
 * 1   5/14/07 6:52p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 *
 ***************************************************************************/

#ifndef SPLASH_VDC_GENERATE_H__
#define SPLASH_VDC_GENERATE_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "bstd.h"

#include "bfmt.h"
#include "bchp.h"
#include "brdc.h"
#include "bvdc.h"
#include "bi2c.h"

#include "bsplash_board.h"
#include "splash_cfg.h"
#include "splash_bmp.h"
#include "platformconfig.h"

#if SPLASH_SUPORT_HDM
#include "bhdm.h"
#endif

#include "btmr.h"
#include "brdc_dbg.h"

/* this include must be put before the condtional includes */
#include "bchp_common.h"

#if BCHP_IT_0_REG_START
#include "bchp_it_0.h"
#endif
#if BCHP_IT_1_REG_START
#include "bchp_it_1.h"
#endif
#if BCHP_IT_2_REG_START
#include "bchp_it_2.h"
#endif

#ifdef BCHP_PRIM_IT_REG_START
#include "bchp_prim_it.h"
#endif
#ifdef BCHP_SEC_IT_REG_START
#include "bchp_sec_it.h"
#endif
#ifdef BCHP_DVI_DTG_0_REG_START
#include "bchp_dvi_dtg_0.h"
#endif

/* for register exclution */

#if BCHP_IRQ0_REG_START
#include "bchp_irq0.h"
#endif
#if BCHP_TIMER_REG_START
#include "bchp_timer.h"
#endif
#if BCHP_SCIRQ0_REG_START
#include "bchp_scirq0.h"
#endif
#if BCHP_EBI_REG_START
#include "bchp_ebi.h"
#endif
#if BCHP_BSCA_REG_START
#include "bchp_bsca.h"
#endif
#if BCHP_BSCD_REG_START
#include "bchp_bscd.h"
#endif
#if BCHP_GIO_REG_START
#include "bchp_gio.h"
#endif
#if BCHP_RDC_REG_START
#include "bchp_rdc.h"
#endif
#if BCHP_AVS_PVT_MNTR_CONFIG_REG_START
#include "bchp_avs_pvt_mntr_config.h"
#endif
#if BCHP_AVS_ASB_REGISTERS_REG_START
#include "bchp_avs_asb_registers.h"
#endif
#if BCHP_AVS_RO_REGISTERS_0_REG_START
#include "bchp_avs_ro_registers_0.h"
#endif
#if BCHP_AVS_RO_REGISTERS_1_REG_START
#include "bchp_avs_ro_registers_1.h"
#endif
#if BCHP_AVS_ROSC_THRESHOLD_1_REG_START
#include "bchp_avs_rosc_threshold_1.h"
#endif

#if BCHP_MEMC_GEN_0_REG_START
#include "bchp_memc_gen_0.h"
#endif
#if BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_REG_START
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#endif
#if BCHP_MEMC_GEN_1_REG_START
#include "bchp_memc_gen_1.h"
#endif
#if BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_REG_START
#include "bchp_memc_ddr23_shim_addr_cntl_1.h"
#endif
#if BCHP_VCXO_2_RM_REG_START
#include "bchp_vcxo_2_rm.h"
#endif
#if BCHP_VCXO_0_RM_REG_START
#include "bchp_vcxo_0_rm.h"
#endif
#if BCHP_CLKGEN_INTR2_REG_START
#include "bchp_clkgen_intr2.h"
#endif
#if BCHP_CLKGEN_GR_REG_START
#include "bchp_clkgen_gr.h"
#endif
#if BCHP_MFD_0_REG_START
#include "bchp_mfd_0.h"
#endif
#if BCHP_VFD_4_REG_START
#include "bchp_vfd_4.h"
#endif
#if BCHP_SCL_0_REG_START
#include "bchp_scl_0.h"
#endif
#if BCHP_FMISC_REG_START
#include "bchp_fmisc.h"
#endif
#if BCHP_BVNM_INTR2_1_REG_START
#include "bchp_bvnm_intr2_1.h"
#endif
#if BCHP_DVP_HT_REG_START
#include "bchp_dvp_ht.h"
#endif
#if BCHP_DVP_HR_TMR_REG_START
#include "bchp_dvp_hr_tmr.h"
#endif
#if BCHP_AON_CTRL_REG_START
#include "bchp_aon_ctrl.h"
#endif
#if BCHP_AON_HDMI_RX_REG_START
#include "bchp_aon_hdmi_rx.h"
#endif
#if BCHP_AVD_INTR2_1_REG_START
#include "bchp_avd_intr2_1.h"
#endif
#if BCHP_AVD_RGR_1_REG_START
#include "bchp_avd_rgr_1.h"
#endif
#if BCHP_DECODE_RBNODE_REGS_0_REG_START
#include "bchp_decode_rbnode_regs_0.h"
#endif
#if BCHP_DECODE_IP_SHIM_1_REG_START
#include "bchp_decode_ip_shim_1.h"
#endif
#if BCHP_AVD_CACHE_0_REG_START
#include "bchp_avd_cache_0.h"
#endif
#if BCHP_AVD_CACHE_1_REG_START
#include "bchp_avd_cache_1.h"
#endif
#if BCHP_V3D_CTL_REG_START
#include "bchp_v3d_ctl.h"
#endif
#if BCHP_V3D_DBG_REG_START
#include "bchp_v3d_dbg.h"
#endif
#if BCHP_AVS_HW_MNTR_REG_START
#include "bchp_avs_hw_mntr.h"
#endif
#if BCHP_AVS_ROSC_THRESHOLD_2_REG_START
#include "bchp_avs_rosc_threshold_2.h"
#endif
#if BCHP_IRB_REG_START
#include "bchp_irb.h"
#endif
#if BCHP_UPG_UART_DMA_REG_START
#include "bchp_upg_uart_dma.h"
#endif
#if BCHP_WKTMR_REG_START
#include "bchp_wktmr.h"
#endif
#if BCHP_LDK_REG_START
#include "bchp_ldk.h"
#endif
#if BCHP_SUN_GISB_ARB_REG_START
#include "bchp_sun_gisb_arb.h"
#endif
#if BCHP_SM_FAST_REG_START
#include "bchp_sm_fast.h"
#endif
#if BCHP_UHFR_REG_START
#include "bchp_uhfr.h"
#endif
#if BCHP_UHFR_GR_BRIDGE_REG_START
#include "bchp_uhfr_gr_bridge.h"
#endif
#if BCHP_RAAGA_DSP_MISC_REG_START
#include "bchp_raaga_dsp_misc.h"
#endif
#if BCHP_RAAGA_DSP_MEM_SUBSYSTEM_REG_START
#include "bchp_raaga_dsp_mem_subsystem.h"
#endif

#if BCHP_VICE2_CME_0_REG_START
#include "bchp_vice2_cme_0.h"
#endif
#if BCHP_VICE2_ARCSS_MISC_REG_START
#include "bchp_vice2_arcss_misc.h"
#endif
#if BCHP_RAAGA_DSP_MEM_SUBSYSTEM_1_REG_START
#include "bchp_raaga_dsp_mem_subsystem_1.h"
#endif
#if BCHP_VICE2_CME_0_0_REG_START
#include "bchp_vice2_cme_0_0.h"
#endif
#if BCHP_VICE2_CME_0_1_REG_START
#include "bchp_vice2_cme_0_1.h"
#endif
#if BCHP_VICE2_ARCSS_MISC_0_REG_START
#include "bchp_vice2_arcss_misc_0.h"
#endif
#if BCHP_VICE2_ARCSS_MISC_1_REG_START
#include "bchp_vice2_arcss_misc_1.h"
#endif
#if BCHP_MEMC_GEN_2_REG_START
#include "bchp_memc_gen_2.h"
#endif
#if BCHP_SHVD_INTR2_0_REG_START
#include "bchp_shvd_intr2_0.h"
#endif
#if BCHP_SHVD_INTR2_1_REG_START
#include "bchp_shvd_intr2_1.h"
#endif
#if BCHP_SHVD_INTR2_2_REG_START
#include "bchp_shvd_intr2_2.h"
#endif
#if BCHP_DVI_DTG_REG_START
#include "bchp_dvi_dtg.h"
#endif

/* for regsiter name comment ? */
#if BCHP_VCXO_CTL_MISC_REG_START
#include "bchp_vcxo_ctl_misc.h"
#endif
#if BCHP_PLL_VCXO_RM_REG_START
#include "bchp_pll_vcxo_rm.h"
#endif
#if BCHP_MISC_REG_START
#include "bchp_misc.h"
#endif
#if BCHP_CLK_REG_START
#include "bchp_clk.h"
#endif
#if BCHP_CLKGEN_REG_START
#include "bchp_clkgen.h"
#endif
#if BCHP_SUN_L2_REG_START
#include "bchp_sun_l2.h"
#endif
#if BCHP_DMISC_REG_START
#include "bchp_dmisc.h"
#endif
#if BCHP_GFD_3_REG_START
#include "bchp_gfd_3.h"
#endif
#if BCHP_GFD_2_REG_START
#include "bchp_gfd_2.h"
#endif
#if BCHP_GFD_1_REG_START
#include "bchp_gfd_1.h"
#endif
#if BCHP_GFD_0_REG_START
#include "bchp_gfd_0.h"
#endif
#if BCHP_CMP_3_REG_START
#include "bchp_cmp_3.h"
#endif
#if BCHP_CMP_2_REG_START
#include "bchp_cmp_2.h"
#endif
#if BCHP_CMP_1_REG_START
#include "bchp_cmp_1.h"
#endif
#if BCHP_CMP_0_REG_START
#include "bchp_cmp_0.h"
#endif
#if BCHP_M2MC_REG_START
#include "bchp_m2mc.h"
#endif
#if BCHP_AON_PIN_CTRL_REG_START
#include "bchp_aon_pin_ctrl.h"
#endif
#if BCHP_DVP_DGEN_0_REG_START
#include "bchp_dvp_dgen_0.h"
#endif
#if BCHP_DVP_TVG_0_REG_START
#include "bchp_dvp_tvg_0.h"
#endif
#if BCHP_GFD_6_REG_START
#include "bchp_gfd_6.h"
#endif
#if BCHP_GFD_5_REG_START
#include "bchp_gfd_5.h"
#endif
#if BCHP_GFD_4_REG_START
#include "bchp_gfd_4.h"
#endif
#if BCHP_HEVD_OL_CPU_REGS_0_REG_START
#include "bchp_hevd_ol_cpu_regs_0.h"
#endif
#if BCHP_VICH_2_REG_START
#include "bchp_vich_2.h"
#endif
#if BCHP_MMISC_REG_START
#include "bchp_mmisc.h"
#endif
#if BCHP_BMISC_REG_START
#include "bchp_bmisc.h"
#endif
#if BCHP_SUN_TOP_CTRL_REG_START
#include "bchp_sun_top_ctrl.h"
#endif
#if BCHP_BVNB_INTR2_REG_START
#include "bchp_bvnb_intr2.h"
#endif
#if BCHP_HDMI_REG_START
#include "bchp_hdmi.h"
#endif
#if BCHP_HDMI_RAM_REG_START
#include "bchp_hdmi_ram.h"
#endif


#define TestError(e, str)   {\
    eErr = e;\
    if (eErr != BERR_SUCCESS)\
    {\
    BDBG_ERR(( str". %s: %d", __FILE__, __LINE__ ));\
        goto done;\
    }\
}

typedef struct
{
	BSUR_Surface_Handle    hSurface;
	BMEM_Handle            hMem;
	int                    iMemIdx;
	BPXL_Format            ePxlFmt;
	uint32_t               ulWidth;
	uint32_t               ulHeight;
	char                   bmpFile[256];
} SplashSurface;

typedef struct
{
	BFMT_VideoFmt          eDispFmt;

	BVDC_Source_Handle     hGfxSource;
	BVDC_Window_Handle     hGfxWindow;
	BVDC_Compositor_Handle hCompositor;
	BVDC_Display_Handle    hDisplay;

	BHDM_Handle            hHdm;

	SplashSurface         *pSurf;
	int                    iSurfIdx;

	uint32_t               ulGfdScratchReg0;
	uint32_t               ulGfdScratchReg1;

	bool                   bGfdHasVertScale;

} SplashDisplay;

typedef struct
{
	BRDC_Handle            hRdc;
	BVDC_Handle            hVdc;
	BI2C_Handle            hI2c;
	BI2C_ChannelHandle     hI2cChn ;
	BREG_I2C_Handle        hRegI2c;

	int                    iRulMemIdx;
	BMEM_Handle            hRulMem;
	bool                   bScaleToFullScreen;

	SplashDisplay          disp[SPLASH_NUM_DISPLAY];
	SplashSurface          surf[SPLASH_NUM_SURFACE];

} ModeHandles;


int splash_generate_script(BCHP_Handle hChp, BREG_Handle hReg, BINT_Handle hInt, BREG_I2C_Handle hRegI2c);

BERR_Code  close_mode
	( ModeHandles        *pState );

void APP_BREG_Write32
    ( BREG_Handle  hReg,
      uint32_t     ulReg,
      uint32_t     ulValue );

#endif

/* End of file */

