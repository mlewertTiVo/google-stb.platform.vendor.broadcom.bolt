/***************************************************************************
*     (c)2005-2009 Broadcom Corporation
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
* $brcm_Workfile: splash_main.c $
* $brcm_Revision: 21 $
* $brcm_Date: 10/4/13 4:33p $
*
* Module Description:
*   This file is meant for unit testing of RAP PI for 7401. This file
*   contains the implementation of Functions and Data Structures to test
*   different features of RAP PI.
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/splash/splashgen/splash_main.c $
* 
* 21   10/4/13 4:33p pntruong
* SW7445-467: Allowed runtime output format selection of PAL/NTSC.
*
* 20   9/5/13 6:31p syang
* SW7145-55: use the biggest heap in  the memc
*
* 19   7/19/13 1:54p pntruong
* SW7429-701: Added option to scale up splash bmp to fullscreen and
* support for CLUT bmp file as well.
*
* 18   6/14/13 3:49p syang
* SW7346-1262: change  BVDC_FOR_BOOTLOADER to BVDC_FOR_BOOTUPDATER per
* Brian's request
*
* 17   4/25/13 11:46p syang
* SW7346-1262: add option to build with reduced vdc for bootloader usage
*
* 16   4/11/13 6:22p syang
* SW7445-213: only check NEXUS_MEMORY_TYPE_APPLICATION_CACHED for heap
* picking
*
* 15   4/11/13 3:21p syang
* SW7445-213: make num of mem fully scalable
*
* 14   4/4/13 6:13p syang
* SW7435-676: fix dbg error msg
*
* 13   4/4/13 4:37p syang
* SW7435-676: add back runtime option for display fmt overriding
*
* 12   3/29/13 11:39a syang
* SW7435-676: deep refactor for memc, surface and display number
* scalabilty; flexibility for diff configure combination; and easy
* adding of new chips
*
* 11   9/10/12 4:40p jessem
* SW7425-3872: Removed hMem1 use with 7425.
*
* 10   9/4/12 4:39p jessem
* SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
* GFD1 to MEMC0.
*
* 9   4/18/12 2:47p jessem
* SW7425-2828: Corrected method on obtaining heap. Added run-time options
* to set the display format and to disable the 2nd display. Backed-out
* of creating a separate surface for the the 2nd display.
*
* 8   4/9/12 5:04p jessem
* SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
* correct heaps to use.
*
* 7   3/16/12 4:50p jessem
* SW7425-2653: Changed MEMC1 heap assignment for 7425.
*
* 6   11/11/11 11:03a jessem
* SW7425-1527: Enclosed label "error" within SPLASH_NOHDM.
*
* 5   8/8/11 11:12p nickh
* SW7425-878: Add 7425 support
*
* 4   4/9/10 3:31p rjain
* SW7550-284: fixing for SPLASH_NOHDM
*
* 3   9/21/09 5:13p nickh
* SW7420-351: Expand functions to provide heap for MEMC1 required by 7420
*
* 2   5/14/09 5:41p shyam
* PR52592 : Add support for ARGB8888 surfaces
*
* 1   4/8/09 4:16p shyam
* PR52386 : Move splash build system to nexus
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
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstd.h"
#include "bkni.h"
#include "bmem.h"
#include "bint.h"
#include "bi2c.h"

#include "nexus_platform.h"
#ifndef debug_only
#include "priv/nexus_core.h"
#endif
#include "nexus_types.h"
#include "bsplash_board.h"
#include "splash_vdc_rulgen.h"

BDBG_MODULE(splashgen);

ModeHandles g_stModeHandles;

static void print_usage(void)
{
	printf("\nUsage:\n");
	printf("\t -d <format> - Optional, for overiding HD display format.\n");
	printf("\t               Use the number in the parenthesis:\n");
	printf("\t               NTSC(0), PAL_G(1), 480p60(2), 576p50(3), 720p60(4)\n");
	printf("\t               720p50(5), 1080i60(6), 1080i50(7), 1080p60(8), 1080p50(9)\n\n");
	printf("\t               SD format will be NTSC  for 60Hz HD format\n");
	printf("\t                                 PAL_G for 50Hz HD format\n");
	printf("\n\t -f          - Scale splash bmp to fullscreen if hardware\n");
	printf("\t               feature available.\n");
	printf("\n\t -h          - prints this message\n\n");
}

int main(int argc, char *argv[])
{
	NEXUS_Error rc;
	NEXUS_PlatformSettings platformSettings;
	BREG_I2C_Handle i2cRegHandle = NULL;
	NEXUS_MemoryStatus stat;
	BMEM_Handle  hMem[SPLASH_NUM_MEM] = {NULL};
	uint32_t heap_size[SPLASH_NUM_MEM] = {0};
	uint32_t heap_idx[SPLASH_NUM_MEM] = {0xffffffff};
	uint32_t format;
	bool overrideFmt;
	bool bScaleToFullScreen = false;
	int ii, jj;

#ifdef SPLASH_SUPPORT_HDM
	BI2C_Handle i2cHandle;
	BI2C_ChannelSettings i2cChanSettings;
	BI2C_ChannelHandle i2cChanHandle;
#endif

	/* Process run-time command options, ok to have none */
	overrideFmt = false;
	for (ii = 1; ii < argc; ii++)
	{
		if (!strcmp(argv[ii], "-d"))
		{
			format = strtol(argv[++ii], NULL, 0);
			overrideFmt = true;
		}
		else if ((!strcmp(argv[ii], "-f")) || (!strcmp(argv[ii], "--fullscreen")))
		{
			bScaleToFullScreen = true;
		}
		else
		{
			print_usage();
			return -1;
		}
	}

	BKNI_Memset((void*)&g_stModeHandles, 0x0, sizeof(ModeHandles));

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openI2c = false;
	platformSettings.openFrontend = false;
	platformSettings.openFpga=false;
	platformSettings.openOutputs = false;
	rc = NEXUS_Platform_Init(&platformSettings);
	BDBG_ASSERT(!rc);

	for (ii=0; ii<NEXUS_MAX_HEAPS; ii++)
	{
		if (g_pCoreHandles->nexusHeap[ii] == NULL)
			continue;

		rc = NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[ii], &stat);
		if (!rc)
		{
			bool bGotAll = true;

			BDBG_MSG(("mem %d: idx %d, type 0x%x", ii, stat.memcIndex, stat.memoryType));
			if (stat.memcIndex<SPLASH_NUM_MEM &&
				stat.memoryType & NEXUS_MEMORY_TYPE_APPLICATION_CACHED &&
				stat.size > heap_size[stat.memcIndex])
			{
				heap_idx[stat.memcIndex] = ii;
				heap_size[stat.memcIndex] = stat.size;
				hMem[stat.memcIndex] = NEXUS_Heap_GetMemHandle(g_pCoreHandles->nexusHeap[ii]);
			}

			for (jj=0; jj<SPLASH_NUM_MEM; jj++)
			{
				if (NULL == hMem[jj])
				{
					bGotAll = false; break;
				}
			}
			if (bGotAll) break;
		}
	}
	for(ii=0; ii<SPLASH_NUM_MEM; ii++)
	{
		BMEM_HeapInfo  info;

		if (NULL == hMem[ii])
		{
			BDBG_ERR(("Failed to get heap %d.", ii));
			return 1;
		}
		BMEM_Heap_GetInfo(hMem[ii], &info);
		BDBG_MSG(("mem %d: heap %d, virtual(0x%8.8x), offet(0x%8.8x), size(0x%8.8x)\n",
			ii, heap_idx[ii], (uint32_t) info.pvAddress, info.ulOffset, info.zSize));
	}

	/* Copy config from macro to internal ModeHandles struct
	 * note: if we add surfaces or display more than 2, here is the
	 * only place to port */
	/* PORT POINT: if we have more than 2 surfaces or displays */
	g_stModeHandles.hRulMem = hMem[SPLASH_RUL_MEM];
	g_stModeHandles.iRulMemIdx = SPLASH_RUL_MEM;
	g_stModeHandles.bScaleToFullScreen = bScaleToFullScreen;

	g_stModeHandles.surf[0].ePxlFmt = SPLASH_SURF0_PXL_FMT;
	g_stModeHandles.surf[0].hMem = hMem[SPLASH_SURF0_MEM];
	g_stModeHandles.surf[0].iMemIdx = SPLASH_SURF0_MEM;
	strcpy(&g_stModeHandles.surf[0].bmpFile[0], SPLASH_SURF0_BMP);

#if (SPLASH_NUM_SURFACE>1)
	g_stModeHandles.surf[1].ePxlFmt = SPLASH_SURF1_PXL_FMT;
	g_stModeHandles.surf[1].hMem = hMem[SPLASH_SURF1_MEM];
	g_stModeHandles.surf[1].iMemIdx = SPLASH_SURF1_MEM;
	strcpy(&g_stModeHandles.surf[1].bmpFile[0], SPLASH_SURF1_BMP);
#endif

	g_stModeHandles.disp[0].eDispFmt = SPLASH_DISP0_FMT;
	g_stModeHandles.disp[0].pSurf = &g_stModeHandles.surf[SPLASH_DISP0_SUR];
	g_stModeHandles.disp[0].iSurfIdx = SPLASH_DISP0_SUR;
	g_stModeHandles.disp[0].bGfdHasVertScale = false;
#ifdef BCHP_GFD_0_VERT_FIR_SRC_STEP
	g_stModeHandles.disp[0].bGfdHasVertScale = true;
#endif

#if (SPLASH_NUM_DISPLAY>1)
	g_stModeHandles.disp[1].eDispFmt = SPLASH_DISP1_FMT;
	g_stModeHandles.disp[1].pSurf = &g_stModeHandles.surf[SPLASH_DISP1_SUR];
	g_stModeHandles.disp[1].iSurfIdx = SPLASH_DISP1_SUR;
	g_stModeHandles.disp[1].bGfdHasVertScale = false;
#ifdef BCHP_GFD_1_VERT_FIR_SRC_STEP
	g_stModeHandles.disp[1].bGfdHasVertScale = true;
#endif
#endif

	/* override 1st and 2nd display formats by run-time command option, if there is */
	if (overrideFmt)
	{
		const BFMT_VideoInfo *pFmtInfo;

		switch (format)
		{
		case 0:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_eNTSC;
			break;
		case 1:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_ePAL_G;
			break;
		case 2:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e480p;
			break;
		case 3:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e576p_50Hz;
			break;
		case 4:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e720p;
			break;
		case 5:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e720p_50Hz;
			break;
		case 6:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e1080i;
			break;
		case 7:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e1080i_50Hz;
			break;
		case 8:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e1080p;
			break;
		case 9:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_e1080p_50Hz;
			break;
		default:
			g_stModeHandles.disp[0].eDispFmt = BFMT_VideoFmt_eNTSC;
			break;
		}

		/* if HD is 60hz then SD path will be NTSC
		 * if HD is 50hz then SD path will be PAL_G */
		pFmtInfo = BFMT_GetVideoFormatInfoPtr(g_stModeHandles.disp[0].eDispFmt);
		g_stModeHandles.disp[1].eDispFmt = (
				(pFmtInfo->ulVertFreq == (25 * BFMT_FREQ_FACTOR)) ||
				(pFmtInfo->ulVertFreq == (50 * BFMT_FREQ_FACTOR)))
			? BFMT_VideoFmt_ePAL_G : BFMT_VideoFmt_eNTSC;
	}

#ifdef SPLASH_SUPPORT_HDM
	rc = BI2C_Open( &i2cHandle, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, NULL);
	if ( rc!=NEXUS_SUCCESS )
	{
		BDBG_ERR(("Failed to open i2c"));
		goto error;
	}
	BI2C_GetChannelDefaultSettings(i2cHandle, B_I2C_CHANNEL_HDMI, &i2cChanSettings);

	i2cChanSettings.clkRate = BI2C_Clk_eClk100Khz;

	rc = BI2C_OpenChannel(i2cHandle, &i2cChanHandle, B_I2C_CHANNEL_HDMI, &i2cChanSettings);
	if ( rc!=NEXUS_SUCCESS )
	{
		BDBG_ERR(("Failed to open i2c channel "));
		goto error;
	}

	rc = BI2C_CreateI2cRegHandle(i2cChanHandle, &i2cRegHandle);
	if ( rc!=NEXUS_SUCCESS )
	{
	BDBG_ERR(("Failed to open reg handle"));goto error;
	}
#endif

#ifdef debug_only
    splash_generate_script(
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL);
#else
    splash_generate_script(
                  g_pCoreHandles->chp,
                  g_pCoreHandles->reg,
                  g_pCoreHandles->bint,
                  i2cRegHandle);
#endif

    error:
	#ifndef BVDC_FOR_BOOTUPDATER
#ifdef SPLASH_SUPPORT_HDM
	BI2C_CloseI2cRegHandle(i2cRegHandle) ;
	BI2C_CloseChannel(i2cChanHandle) ;
	BI2C_Close(i2cHandle) ;
#endif
	NEXUS_Platform_Uninit();
#endif /* #ifndef BVDC_FOR_BOOTUPDATER */
	return 0;
}

/* End of File */
