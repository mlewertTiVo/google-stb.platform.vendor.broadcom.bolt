/***************************************************************************
 *     Copyright (c) 2005-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsplash_board.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/4/13 4:02p $
 *
 * Module Description: Application to generate RULs for splash screen.
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97445/bsplash_board.h $
 * 
 * 6   10/4/13 4:02p pntruong
 * SW7445-467: Added support for cx and fixed build error for bx.
 *
 * 5   4/11/13 6:18p syang
 * SW7445-213: rul now uses memc2
 *
 * 4   4/11/13 3:26p syang
 * SW7445-213: add mem/sur/disp cnfigure
 *
 * 3   12/29/12 1:39p katrep
 * SW7445-1:baseonly built
 *
 * 2   9/24/12 2:34p katrep
 * SW7445-1: updated for 7445
 *
 * 1   8/31/12 2:50p katrep
 * SW7445-1:add spalsh
 *
 * 1   4/19/12 1:35p mward
 * SW7435-114:  Add support for 97435 platform(s).
 *
 * 6   4/18/12 2:55p jessem
 * SW7425-2828: Removed display formats.
 *
 * 4   3/27/12 12:15p jessem
 * SW7425-2653: Updated RDC scratch regsiters.
 *
 * 3   11/11/11 10:43a jessem
 * SW7425-1527: Excluded BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15 and
 * BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0. Added register block names in
 * register dump to aid in debugging.
 *
 * 2   8/22/11 5:11p jessem
 * SW7425-878: Updated RDC registers used by splash.
 *
 * 1   8/8/11 11:07p nickh
 * SW7425-878: Add 7425
 *
 * 1   6/27/11 11:34a katrep
 * SW7231-220:add support for 7231 &newer 40nm chips
 *
 ***************************************************************************/
#ifndef BSPLASH_BOARD_H__
#define BSPLASH_BOARD_H__

/* For register exclusions */
#include "bchp_common.h"
#include "bchp_irq0.h"
#include "bchp_scirq0.h"
#include "bchp_ebi.h"
#include "bchp_gio.h"
#include "bchp_rdc.h"

/* These are the registers that need to be excluded from the register dump either
   because they interrupt the CPU or disturb settings done elsewhere like the CFE
   If you do not want to program certain registers - add them to this macro.
*/
#define BSPLASH_ADDRESS_IN_RANGE(addr, rangeLow, rangeHigh) ((addr>=rangeLow) && (addr <=rangeHigh))

#define BSPLASH_REGDUMP_EXCLUDE(addr)   ( \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_5_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_BUS_IF_REG_START, BCHP_XPT_XPU_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_0_REG_START, BCHP_SHVD_INTR2_0_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_1_REG_START, BCHP_SHVD_INTR2_1_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_2_REG_START, BCHP_SHVD_INTR2_2_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RAAGA_DSP_INTH_REG_START, BCHP_RAAGA_DSP_INTH_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_M2MC_L2_REG_START, BCHP_M2MC_L2_REG_END)  || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_M2MC1_L2_REG_START, BCHP_M2MC1_L2_REG_END)  || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HDMI_TX_INTR2_REG_START, BCHP_HDMI_TX_INTR2_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VIDEO_ENC_INTR2_REG_START, BCHP_VIDEO_ENC_INTR2_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_ARC_0_REG_START, BCHP_MEMC_ARC_0_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_ARC_1_REG_START, BCHP_MEMC_ARC_1_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_ARC_2_REG_START, BCHP_MEMC_ARC_2_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_EBI_CS_BASE_0, BCHP_EBI_ECR) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_REG_START, BCHP_TIMER_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GIO_ODEN_LO, BCHP_GIO_STAT_EXT) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IE0, BCHP_TIMER_TIMER_IE0) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RAAGA_DSP_SEC0_REG_START, BCHP_RAAGA_DSP_MEM_SUBSYSTEM_1_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_XPT_SECURITY_NS_REG_END)   || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_L2_1_0_REG_START, BCHP_MEMC_L2_1_0_REG_END) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0) || \
	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVS_CPU_PROG_MEM_REG_START, BCHP_AVS_PMB_REGISTERS_REG_END)\
)

/* number of mem heap handles  */
#define SPLASH_NUM_MEM        3

/* heap for RUL bufs */
#define SPLASH_RUL_MEM        2   /* idx to BMEM_Handle array */

/* number of surface */
#define SPLASH_NUM_SURFACE    2

/* surface 0 info */
#define SPLASH_SURF0_MEM      2   /* idx to BMEM_Handle array */
#define SPLASH_SURF0_PXL_FMT  BPXL_eR5_G6_B5
#define SPLASH_SURF0_BMP      "splash.bmp"

/* surface 1 info */
#define SPLASH_SURF1_MEM      2   /* idx to BMEM_Handle array */
#define SPLASH_SURF1_PXL_FMT  BPXL_eR5_G6_B5
#define SPLASH_SURF1_BMP      "splash.bmp"

/* number of display */
#define SPLASH_NUM_DISPLAY    2

#ifdef  CFG_SPLASH_PAL
/* display 0 info */
#define SPLASH_DISP0_FMT      BFMT_VideoFmt_e576p_50Hz
#define SPLASH_DISP0_SUR      0   /* idx to splash surface buffer array */

/* display 1 info */
#define SPLASH_DISP1_FMT      BFMT_VideoFmt_ePAL_I
#define SPLASH_DISP1_SUR      1   /* idx to splash surface buffer array */

#else
/* display 0 info */
#define SPLASH_DISP0_FMT      BFMT_VideoFmt_e480p
#define SPLASH_DISP0_SUR      0   /* idx to splash surface buffer array */

/* display 1 info */
#define SPLASH_DISP1_FMT      BFMT_VideoFmt_eNTSC
#define SPLASH_DISP1_SUR      1   /* idx to splash surface buffer array */
#endif

#define SPLASH_NUM_SVIDEO_OUTPUTS      0
#define SPLASH_NUM_COMPOSITE_OUTPUTS   1
#define SPLASH_NUM_COMPONENT_OUTPUTS   1

#define BRCM_DAC_SVIDEO_LUMA           BVDC_Dac_5
#define BRCM_DAC_SVIDEO_CHROMA         BVDC_Dac_6
#define BRCM_DAC_COMPOSITE_0           BVDC_Dac_3

#define BRCM_DAC_Y                     BVDC_Dac_0
#define BRCM_DAC_PR                    BVDC_Dac_2
#define BRCM_DAC_PB                    BVDC_Dac_1

#define SPLASH_SURFACE_PXL_TYPE BPXL_eR5_G6_B5 /* BPXL_eA8_R8_G8_B8 */

#define B_I2C_CHANNEL_HDMI    3

#endif /* BSPLASH_BOARD_H__ */

/* End of File */
