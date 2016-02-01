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
 * $brcm_Workfile: splash_setup_vdc.c $
 * $brcm_Revision: 42 $
 * $brcm_Date: 10/4/13 11:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/splash_setup_vdc.c $
 * 
 * 42   10/4/13 11:33a pntruong
 * SW7445-467: Improved debug message output.
 *
 * 41   7/19/13 1:53p pntruong
 * SW7429-701: Added option to scale up splash bmp to fullscreen and
 * support for CLUT bmp file as well.
 *
 * 40   6/20/13 3:49p syang
 * SW7346-1262: set VDC default display fmt as 480p
 *
 * 39   6/14/13 3:49p syang
 * SW7346-1262: change  BVDC_FOR_BOOTLOADER to BVDC_FOR_BOOTUPDATER per
 * Brian's request
 *
 * 38   4/25/13 11:46p syang
 * SW7346-1262: add option to build with reduced vdc for bootloader usage
 *
 * 37   4/12/13 12:45p syang
 * SW7445-213:  change to use cached addr for surface
 *
 * 36   4/11/13 3:25p syang
 * SW7445-213: take care unused surface
 *
 * 35   4/4/13 5:53p syang
 * SW7563-41: fix HDMI display with DVI master TG
 *
 * 34   3/29/13 11:39a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 *
 * 33   10/31/12 6:53p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 *
 * 32   10/5/12 6:32p bselva
 * SW7360-38: Add splash support
 *
 * 31   9/28/12 4:26p mward
 * SW7435-114:  RTS for 7435 in 1u4t mode requires feeding SD and HD
 * displays from separate MEMCs.
 *
 * 30   9/4/12 4:39p jessem
 * SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
 * GFD1 to MEMC0.
 *
 * 29   7/23/12 5:05p mward
 * SW7125-1306:  Fix run on chips without GFD vertical upscale.
 *
 * 28   4/19/12 1:46p mward
 * SW7435-114:  Add support for 7435.
 *
 * 27   4/19/12 1:10p jessem
 * SW7425-2828: Reduced console clutter.
 *
 * 26   4/18/12 3:23p jessem
 * SW7425-2828: Removed extraneous variable.
 *
 * 25   4/18/12 2:47p jessem
 * SW7425-2828: Corrected method on obtaining heap. Added run-time options
 * to set the display format and to disable the 2nd display. Backed-out
 * of creating a separate surface for the the 2nd display.
 *
 * 24   4/9/12 4:43p jessem
 * SW7425-2828: Corrected GFD handle used with 2nd display when obtaining
 * RDC scratch regs.
 *
 * 23   4/9/12 11:25a jessem
 * SW7425-2828: Removed hardcoded RDC scratch registers and use
 * BVDC_Test_Source_GetGfdScratchRegisters() instead. Also, updated the
 * implementation of the splash surface for the 2nd display.
 *
 * 22   12/20/11 4:50p randyjew
 * SW7208-176: Change Hdmi Output Format default to RGB from YCrCb.
 *
 * 21   8/16/11 5:20p jessem
 * SW7425-1122: Merge SW7552-48 changes to main line.
 *
 * 20   8/8/11 11:16p nickh
 * SW7425-878: Add 7425 splash support
 *
 * 19   2/24/11 5:09p gmohile
 * SW7408-239 : Add 7408 splash support
 *
 * 18   4/30/10 4:51p randyjew
 * SW7468-6: Add SPLASH_NUM_XXX_OUTPUTS flags instead of
 * NEXUS_NUM_XXX_OUTPUTS.
 *
 * 17   4/16/10 6:10p nitinb
 * SW7550-284: Do not configure composite 1 and S-Video outputs for 7550
 * splash
 *
 * 16   6/26/09 12:45p shyam
 * PR54436 : Add 1080i 50 Hz support
 *
 * 15   5/15/09 5:06p shyam
 * PR54436 : Add 720p50Hz and other HD format support
 *
 * 14   5/15/09 3:32p shyam
 * PR54436 : Add HD support
 *
 * 13   5/14/09 5:39p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 12   4/8/09 4:28p shyam
 * PR52386 : Add support for DTV platforms
 *
 * 4   4/8/09 12:44p shyam
 * PR52386 : Port splash to nexus Base Build system
 *
 * 3   12/24/08 5:03p katrep
 * PR50711: Add splash support for 7405/7335/7325 and settop chips
 *
 * 2   10/13/08 10:04a erickson
 * PR46184: merge
 *
 * PR46184/1   10/10/08 3:49p chengs
 * PR46184: BSUR_Surface_Create() add a new parameter.
 *
 * 1   9/29/08 11:34a erickson
 * PR46184: added splash app
 *
 * 11   11/2/07 12:59p shyam
 * PR 34874 : Add support for Composite 1
 *
 * 10   11/2/07 12:54p shyam
 * PR 34874 : Add support for Composite 1
 *
 * 9   10/19/07 4:09p shyam
 * PR 30741 : Add support for 576p+PAL
 *
 * 8   10/8/07 7:27p shyam
 * PR 30741 : Add support for No HDMI
 *
 * 7   9/7/07 4:43p shyam
 * PR 30741 : Cleaned up LEGACY_VDC code and assert on VDC errors
 *
 * 6   8/9/07 6:18p shyam
 * PR 33658 : Add PAL support
 *
 * 5   8/9/07 3:10p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 4   7/31/07 4:01p shyam
 * PR 33658 : Port splash to 97403 platform
 *
 * 3   7/18/07 12:35p shyam
 * PR 30741 : Add support for 480p on HDMI and NTSC on composite
 *
 * 2   6/29/07 2:09p shyam
 * PR 30741 : Added back HDMI
 *
 * 1   5/14/07 6:50p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */

/* common utilities */
#include "brdc.h"
#include "bfmt.h"

/* porting interfaces */
#include "bvdc.h"
#include "bvdc_test.h"

#if SPLASH_SUPPORT_HDM
#include "bhdm_edid.h"
#endif

#include "splash_bmp.h"
#include "splash_file.h"
#include "bsplash_board.h"
#include "splash_vdc_rulgen.h"
#include "nexus_platform_features.h"

BDBG_MODULE(splash_setup_vdc);

#ifdef SPLASH_SUPPORT_HDM
/* prototypes */
static BERR_Code ActivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay);
static void   DeactivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay);
#ifndef SPLASH_HDMI_OUTPUTFORMAT_YCrCb
#define SPLASH_HDMI_OUTPUTFORMAT_YCrCb 0  /*RGB default*/
#endif

#else
#undef SPLASH_MASTERTG_DVI
#endif

#define IS_HD(fmt) (\
	(fmt == BFMT_VideoFmt_e480p)               || \
	(fmt == BFMT_VideoFmt_e576p_50Hz)          || \
	(fmt == BFMT_VideoFmt_e720p)               || \
	(fmt == BFMT_VideoFmt_e720p_24Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_25Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_30Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_50Hz)          || \
	(fmt == BFMT_VideoFmt_e1080i)              || \
	(fmt == BFMT_VideoFmt_e1080i_50Hz)         || \
	(fmt == BFMT_VideoFmt_e1250i_50Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p)              || \
	(fmt == BFMT_VideoFmt_e1080p_24Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_25Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_30Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_50Hz)         || \
	(fmt == BFMT_VideoFmt_e720p_60Hz_3DOU_AS)  || \
	(fmt == BFMT_VideoFmt_e720p_50Hz_3DOU_AS)  || \
	(fmt == BFMT_VideoFmt_e1080p_24Hz_3DOU_AS) || \
	(fmt == BFMT_VideoFmt_e1080p_30Hz_3DOU_AS) || \
	(fmt == BFMT_VideoFmt_eCUSTOM_1366x768p)   || \
	(fmt == BFMT_VideoFmt_eDVI_1600x1200p_60Hz)|| \
	(fmt == BFMT_VideoFmt_eCustom2)               \
)

/***************************************************************************
 * This function sets up the display with graphics and does ONE
 * ApplyChanges call. After this point, all register writes and RUL
 * updates should be intercepted and dumped.
 */
BERR_Code  splash_vdc_setup(
	BCHP_Handle         hChp,
	BREG_Handle         hReg,
	BINT_Handle         hInt,
	BTMR_Handle         hTmr,
	ModeHandles        *pState
	)
{
	BERR_Code               eErr;
	BVDC_Handle             hVdc;
#ifdef SPLASH_SUPPORT_HDM
	BHDM_Handle             hHdm;
	BHDM_Settings           HDMSettings;
#endif
	BVDC_Compositor_Handle  hCompositor;
	BVDC_Display_Handle     hDisplay;
	BFMT_VideoInfo          stVideoInfo;
	BSUR_Surface_Handle     hSurface;
	BVDC_Source_Handle      hGfxSource;
	BVDC_Window_Handle      hGfxWindow;
	BAVC_Gfx_Picture        pic;
	uint32_t         surfWidth;
	uint32_t         surfHeight;
	BPXL_Format      surfPxlFmt;
	void*            splashAddress;
	uint32_t         splashPitch;
	char  *bmpFileName = NULL;
	uint8_t  *bmpBuf = NULL;
	BMP_HEADER_INFO  myBmpInfo;
	int  x,y;
	uint32_t  svideoDspIdx;
	uint32_t  composite0DspIdx;
	uint32_t  composite1DspIdx;
	uint32_t  componentDspIdx;
	uint32_t  hdmDspIdx;
	uint32_t  winHeight;
	uint32_t ulNewOffset;
	BVDC_Settings  stDefSettings;
	int  ii;
	BMEM_HeapInfo info;

	/* setup surfaces */
	for (ii=0; ii<SPLASH_NUM_SURFACE; ii++)
	{
		if ((bmpFileName == NULL) || strcmp(bmpFileName, &pState->surf[ii].bmpFile[0]))
		{
			if(bmpBuf)
				BKNI_Free(bmpBuf);

			bmpFileName = &pState->surf[ii].bmpFile[0];
			bmpBuf = splash_open_bmp(bmpFileName);
			if(bmpBuf)
				splash_bmp_getinfo(bmpBuf, &myBmpInfo);
			else
				BDBG_ERR(("Missing file %s. could use BSEAV/app/splash/splashgen/splash.bmp", bmpFileName));
		}

		/* scale up fullscreen AND gfd has scaler.  For vertical only scale
		 * if HW is available */
		if(pState->bScaleToFullScreen)
		{
			surfWidth  = myBmpInfo.info.width;
			if(pState->disp[ii].bGfdHasVertScale)
			{
				surfHeight = myBmpInfo.info.height;
			}
			else
			{
				surfHeight = pState->surf[ii].ulHeight;
			}
		}
		else
		{
			surfWidth  = pState->surf[ii].ulWidth;
			surfHeight = pState->surf[ii].ulHeight;
		}

		surfPxlFmt = pState->surf[ii].ePxlFmt;
		if (0==surfWidth)
			continue;
		TestError(BSUR_Surface_Create(
			pState->surf[ii].hMem,
			surfWidth, surfHeight,
			0, NULL, surfPxlFmt, NULL, 0, NULL,
			&hSurface ),
			"ERROR:BSUR_Surface_Create" );
		pState->surf[ii].hSurface = hSurface;

		/* since we use nexus_platform, it does not do mmap for uncached address, so we
		 * have to convert surface address to cached address before writing to it */
		BSUR_Surface_GetAddress(hSurface, &splashAddress, &splashPitch);
		BMEM_ConvertAddressToOffset(pState->surf[ii].hMem, splashAddress, &ulNewOffset);
		BMEM_Heap_ConvertAddressToCached(pState->surf[ii].hMem, splashAddress, &splashAddress);
		splash_set_surf_params(surfPxlFmt, splashPitch, surfWidth, surfHeight) ;

		BMEM_Heap_GetInfo(pState->surf[ii].hMem, &info);
		BDBG_MSG(("***********surface[%d]*************", ii));
		BDBG_MSG(("surface %d created, virtualAddr 0x%8.8x, offset 0x%8.8x", ii, splashAddress, ulNewOffset));
		BDBG_MSG(("from mem %d, start addr:  virtual(0x%8.8x), offset(0x%8.8x), size %d",
			pState->surf[ii].iMemIdx, (uint32_t) info.pvAddress, info.ulOffset, info.zSize));

		/* splash_fillbuffer(splashAddress, 0xF8, 0xE0, 0) ; */
		splash_fillbuffer(splashAddress, 0x00, 0x00, 0x80);

		if(bmpBuf)
		{
			BDBG_MSG(("splash.bmp: Width = %d Height = %d", myBmpInfo.info.width, myBmpInfo.info.height));
			BDBG_MSG(("rendered into surface %d",ii));

			if(pState->bScaleToFullScreen)
			{
				x = 0;
				if(pState->disp[ii].bGfdHasVertScale)
				{
					y = 0;
				}
				else
				{
					y = (surfHeight-myBmpInfo.info.height)/2;
				}
			}
			else
			{
				x = (surfWidth-myBmpInfo.info.width)/2;
				y = (surfHeight-myBmpInfo.info.height)/2;
			}
			splash_render_bmp_into_surface(x, y, bmpBuf, splashAddress) ;
		}

		/* flush cached addr */
		BMEM_Heap_FlushCache(pState->surf[ii].hMem, splashAddress, splashPitch * surfHeight);
	}

	if(bmpBuf)
	{
		BKNI_Free(bmpBuf);
	}

	/* open VDC
	 * note: VDC default display format is 1080i, we must override it here
	 * if we are compiling with B_PI_FOR_BOOTUPDATER and 1080i is not supported in bootloader
	 */
	BVDC_GetDefaultSettings(&stDefSettings);
	stDefSettings.eVideoFormat = BFMT_VideoFmt_e480p;
	eErr = BVDC_Open(&pState->hVdc,
		hChp, hReg, pState->hRulMem, hInt, pState->hRdc, hTmr, &stDefSettings);
    hVdc = pState->hVdc;

#ifdef SPLASH_SUPPORT_HDM
    eErr = BHDM_GetDefaultSettings(&HDMSettings);

	/* TODO:  get rifd of chip specific code */
#if (BCHP_CHIP != 7360)
	HDMSettings.hTMR = hTmr;
#endif
    eErr = BHDM_Open(&hHdm, hChp, hReg, hInt, pState->hRegI2c, &HDMSettings);
#endif

	/* which disp should drive which output interface? */
	svideoDspIdx = 0;
	composite0DspIdx = 0;
	composite1DspIdx = 0;
	componentDspIdx = 0;
	hdmDspIdx = 0;
	for(ii=0; ii<SPLASH_NUM_DISPLAY; ii++)
	{
		if (IS_HD(pState->disp[ii].eDispFmt))
		{
			componentDspIdx = ii;
			hdmDspIdx = ii;
		}
		else
		{
			svideoDspIdx = ii;
			composite0DspIdx = ii;
			composite1DspIdx = ii;
		}
	}

	/* setup display */
	for(ii=0; ii<SPLASH_NUM_DISPLAY; ii++)
	{
		int32_t iTop, iLeft;
		uint32_t ulWidth, ulHeight;

		BDBG_MSG(("***********display[%d]*************", ii));

		/* Create a compositor handle from our hVdc handle */
		TestError( BVDC_Compositor_Create(
			hVdc, &hCompositor, BVDC_CompositorId_eCompositor0 + ii, NULL),
			"ERROR: BVDC_Compositor_Create" );
		pState->disp[ii].hCompositor = hCompositor;

		/* Create display handle */
#ifdef SPLASH_MASTERTG_DVI
		if (hdmDspIdx == (uint32_t)ii)
		{
			BVDC_Display_Settings  cfg_display;

			BVDC_Display_GetDefaultSettings(BVDC_DisplayId_eDisplay0, &cfg_display);
			cfg_display.eMasterTg = BVDC_DisplayTg_eDviDtg;
			eErr = BVDC_Display_Create(hCompositor,&hDisplay,
				BVDC_DisplayId_eDisplay0, &cfg_display);
		}
		else
		{
			eErr = BVDC_Display_Create(hCompositor, &hDisplay, BVDC_DisplayId_eAuto, NULL);
		}
#else
		eErr = BVDC_Display_Create(hCompositor, &hDisplay, BVDC_DisplayId_eAuto, NULL);
#endif
		if (eErr != BERR_SUCCESS)
		{
			BDBG_ERR(("BVDC_Display_Create failed"));
			goto done;
		}
		pState->disp[ii].hDisplay = hDisplay;

		/* Set display format */
		TestError( BVDC_Display_SetVideoFormat(hDisplay, pState->disp[ii].eDispFmt),
			"ERROR: BVDC_Display_SetVideoFormat" );

		/* Set the background color to blue */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor, 0x00, 0x00, 0x80 ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );

		/* set DAC configurations for specific display format
		 * Dac setup is specified in bsplash_board.h */
#if SPLASH_NUM_COMPONENT_OUTPUTS && !defined(SPLASH_MASTERTG_DVI)
		if (componentDspIdx == (uint32_t)ii)
		{
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_PR, BVDC_DacOutput_ePr),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_Y, BVDC_DacOutput_eY),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_PB, BVDC_DacOutput_ePb),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			BDBG_MSG(("Set dac for component with display %d", ii));
		}
#endif
#if SPLASH_NUM_SVIDEO_OUTPUTS
		if ((svideoDspIdx == (uint32_t)ii) && !IS_HD(pState->disp[ii].eDispFmt))
		{
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_SVIDEO_CHROMA, BVDC_DacOutput_eSVideo_Chroma),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_SVIDEO_LUMA, BVDC_DacOutput_eSVideo_Luma),
					   "ERROR: BVDC_Display_SetDacConfiguration" );
			BDBG_MSG(("Set dac for svideo with display %d", ii));
		}
#endif
#if SPLASH_NUM_COMPOSITE_OUTPUTS
		if ((composite0DspIdx == (uint32_t)ii) && !IS_HD(pState->disp[ii].eDispFmt))
		{
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_COMPOSITE_0, BVDC_DacOutput_eComposite),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			BDBG_MSG(("Set dac for coposite 0 with display %d", ii));
		}
#endif
#if SPLASH_NUM_COMPOSITE_OUTPUTS > 1
		if ((composite1DspIdx == (uint32_t)ii) && !IS_HD(pState->disp[ii].eDispFmt))
		{
			TestError( BVDC_Display_SetDacConfiguration( hDisplay,
				BRCM_DAC_COMPOSITE_1, BVDC_DacOutput_eComposite),
					   "ERROR: BVDC_Display_SetDacConfiguration" );
			BDBG_MSG(("Set dac for coposite 1 with display %d", ii));
		}
#endif

		/* to determine size of display */
		TestError( BFMT_GetVideoFormatInfo(pState->disp[ii].eDispFmt, &stVideoInfo),
			"ERROR:BFMT_GetVideoFormatInfo" );

		/* create a graphics source handle */
		TestError( BVDC_Source_Create( hVdc, &hGfxSource, BAVC_SourceId_eGfx0 + ii, NULL),
			"ERROR: BVDC_Source_Create" );
		pState->disp[ii].hGfxSource = hGfxSource;

		/* obtain RDC scratch registers */
		BVDC_Test_Source_GetGfdScratchRegisters(pState->disp[ii].hGfxSource,
			&pState->disp[ii].ulGfdScratchReg0, &pState->disp[ii].ulGfdScratchReg1);

		/* specify the source surface */
		BKNI_Memset(&pic, 0, sizeof(pic));
		pic.hSurface = pState->disp[ii].pSurf->hSurface;
		TestError( BVDC_Source_SetSurface( hGfxSource, &pic ),
			"ERROR: BVDC_Source_SetSurface" );

		BDBG_MSG(("uses surface %d", pState->disp[ii].iSurfIdx));

		/* create a window handle */
		TestError( BVDC_Window_Create( hCompositor,
			&hGfxWindow, BVDC_WindowId_eAuto, hGfxSource, NULL ),
			"ERROR:BVDC_Window_Create" );
		pState->disp[ii].hGfxWindow = hGfxWindow;

		/* set destination height not bigger than src height */
		winHeight = (stVideoInfo.ulHeight <= pState->disp[ii].pSurf->ulHeight)?
			stVideoInfo.ulHeight : pState->disp[ii].pSurf->ulHeight;

		if(pState->bScaleToFullScreen)
		{
			iLeft = 0;
			ulWidth  = stVideoInfo.ulWidth;

			/* set destination size to match display */
			if(pState->disp[ii].bGfdHasVertScale)
			{
				iTop     = 0;
				ulHeight = stVideoInfo.ulHeight;
			}
			else
			{
				iTop  = (stVideoInfo.ulHeight - winHeight)/2;
				ulHeight = winHeight;
			}
		}
		else
		{
			iLeft    = 0;
			iTop     = (stVideoInfo.ulHeight - winHeight)/2;
			ulWidth  = stVideoInfo.ulWidth;
			ulHeight = winHeight;
		}

		BDBG_MSG(("output rect(%4d, %4d, %4d, %4d)", iTop, iTop, ulWidth, ulHeight));

		TestError( BVDC_Window_SetDstRect(hGfxWindow, iTop, iTop, ulWidth, ulHeight),
			"ERROR:BVDC_Window_SetDstRect");
		TestError( BVDC_Window_SetScalerOutput(hGfxWindow, iTop, iTop, ulWidth, ulHeight),
			"ERROR:BVDC_Window_SetScalerOutput");

		/* set order to front */
		TestError( BVDC_Window_SetZOrder( hGfxWindow, 1),
			"ERROR:BVDC_Window_SetZOrder" );

		/* enable visibility */
		TestError( BVDC_Window_SetVisibility( hGfxWindow, true),
			"ERROR:BVDC_Window_SetVisibility" );
	}

    /***************************
     * Apply Changes
     */
    TestError( BVDC_ApplyChanges(hVdc), "ERROR:BVDC_ApplyChanges" );

#ifdef SPLASH_SUPPORT_HDM
    /***************************
     * Add HDMI
     */
	pState->disp[hdmDspIdx].hHdm = hHdm;
    eErr = ActivateHdmi(hVdc, hHdm, pState->disp[hdmDspIdx].hDisplay);
    if( eErr != BERR_SUCCESS)
        BDBG_ERR(("Error ActivateHDMI, HDMI is not connected, or TV is off?\n"));

    /* apply changes */
    TestError( BVDC_ApplyChanges(hVdc),
        "ERROR:BVDC_ApplyChanges" );
#endif

done:
    /* return status */
    BDBG_ASSERT(!eErr) ;
    return eErr;
}

#ifndef BVDC_FOR_BOOTUPDATER
BERR_Code  close_mode(
	ModeHandles            *pState
	)
{
	BERR_Code  eErr = BERR_SUCCESS;
	int  ii;

	for(ii=0; ii<SPLASH_NUM_DISPLAY; ii++)
	{
#ifdef SPLASH_SUPPORT_HDM
		if (pState->disp[ii].hHdm)
		{
			DeactivateHdmi(pState->hVdc, pState->disp[ii].hHdm, pState->disp[ii].hDisplay);
		}
#endif
		TestError( BVDC_Window_Destroy(pState->disp[ii].hGfxWindow),
            "BVDC_Window_Destroy");

		TestError( BVDC_ApplyChanges(pState->hVdc),
            "BVDC_ApplyChanges");

		TestError( BVDC_Source_Destroy(pState->disp[ii].hGfxSource),
            "BVDC_Source_Destroy");

		TestError( BVDC_Display_Destroy(pState->disp[ii].hDisplay),
            "BVDC_Display_Destroy");

		TestError( BVDC_Compositor_Destroy(pState->disp[ii].hCompositor),
            "BVDC_Compositor_Destroy");

#ifdef SPLASH_SUPPORT_HDM
		if (pState->disp[ii].hHdm)
		{
			BHDM_Close(pState->disp[ii].hHdm);
		}
#endif

		TestError( BVDC_ApplyChanges(pState->hVdc),
			"BVDC_ApplyChanges");
	}

	for (ii=0; ii<SPLASH_NUM_SURFACE; ii++)
	{
		BSUR_Surface_Destroy(pState->surf[ii].hSurface) ;
	}

    TestError( BVDC_Close(pState->hVdc),
            "BVDC_Close");

done:
    return eErr;
}
#endif /* #ifndef BVDC_FOR_BOOTUPDATER */

#ifdef SPLASH_SUPPORT_HDM
static BERR_Code ActivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay)
{
    BERR_Code  eErr = BERR_SUCCESS;
    BFMT_VideoFmt           vidfmt;
    BHDM_Settings           hdmiSettings;
    const BFMT_VideoInfo*   vidinfo;
    bool                    hasHdmiSupport;

    BHDM_EDID_RxVendorSpecificDB    vsdb;

    /* Get Current Display format */
    TestError(BVDC_Display_GetVideoFormat(hDisplay, &vidfmt),
        "BVDC_Display_GetVideoFormat") ;

    /* Get video info */
    vidinfo = BFMT_GetVideoFormatInfoPtr(vidfmt);

    /* Get Current Settings */
    TestError(BHDM_GetHdmiSettings(hHDM, &hdmiSettings),
        "BHDM_GetHdmiSettings");

    /* Set the video format */
    hdmiSettings.eInputVideoFmt = vidfmt;

    if (hdmiSettings.eOutputPort == BHDM_OutputPort_eHDMI)
    {
        BHDM_EDID_IsRxDeviceHdmi(hHDM, &vsdb, &hasHdmiSupport);

        if (SPLASH_HDMI_OUTPUTFORMAT_YCrCb /*hasHdmiSupport*/)
        {
            hdmiSettings.eOutputFormat = BHDM_OutputFormat_eHDMIMode;
            hdmiSettings.eAspectRatio = vidinfo->eAspectRatio;

            /* Audio settings (for later):
			   set hdmi audio()
			   BAUD_GetClockSamplingRate(GetBAUD(), ??, &sampleRate);
			   hdmiSettings.eAudioSamplingRate = sampleRate;
			   hdmiSettings.AudioBits = BAVC_AudioBits_e16;
            */
        }
        else
        {
            /* Configure for DVI mode */
            hdmiSettings.eOutputFormat = BHDM_OutputFormat_eDVIMode;
        }
    }

    TestError(BHDM_GetColorimetry(hdmiSettings.eOutputFormat, vidfmt, &hdmiSettings.eColorimetry),
        "BHDM_GetColorimetry");

    TestError(BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, hdmiSettings.eColorimetry),
        "BVDC_Display_SetHdmiConfiguration");

    TestError(BVDC_ApplyChanges(hVDC),
        "BVDC_ApplyChanges");

#ifdef SPLASH_MASTERTG_DVI
	BHDM_SetHdmiDataTransferMode(hHDM, true);
#endif

    TestError(BHDM_EnableDisplay(hHDM, &hdmiSettings),
        "BHDM_EnableDisplay");

	#ifndef BVDC_FOR_BOOTUPDATER
    TestError(BVDC_Display_InstallCallback(hDisplay,
                    (BVDC_CallbackFunc_isr)BHDM_AudioVideoRateChangeCB_isr,
                    hHDM,
                    BHDM_Callback_Type_eVideoChange),
        "BVDC_Display_InstallCallback");
	#endif /* #ifndef BVDC_FOR_BOOTUPDATER */

	#if 0 /* Example comment out shyam */
    TestError(BVDC_ApplyChanges(hVDC),
        "BVDC_ApplyChanges");
	#endif

done:
    return eErr;
}

/* Activate HDMI to match the current video format */
#ifndef BVDC_FOR_BOOTUPDATER
static void DeactivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay)
{
    /*
        uninstall audio callback (later)
    */
    BVDC_Display_InstallCallback(hDisplay, NULL, NULL, 0) ;
    (void) BVDC_ApplyChanges(hVDC);
    (void) BHDM_DisableDisplay(hHDM);
}
#endif /* #ifndef BVDC_FOR_BOOTUPDATER */
#endif

/* End of File */
