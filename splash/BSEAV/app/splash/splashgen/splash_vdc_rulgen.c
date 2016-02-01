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
 * $brcm_Workfile: splash_vdc_rulgen.c $
 * $brcm_Revision: 63 $
 * $brcm_Date: 6/14/13 3:49p $
 *
 * Module Description: Application to generate RULs for splash screen.
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/splash_vdc_rulgen.c $
 * 
 * 63   6/14/13 3:49p syang
 * SW7346-1262: change  BVDC_FOR_BOOTLOADER to BVDC_FOR_BOOTUPDATER per
 * Brian's request
 * 
 * 62   4/25/13 11:46p syang
 * SW7346-1262: add option to build with reduced vdc for bootloader usage
 * 
 * 61   4/4/13 6:12p syang
 * SW7435-676: fix dbg error msg
 * 
 * 60   4/4/13 4:37p syang
 * SW7435-676: add back runtime option for display fmt overriding
 * 
 * 59   3/29/13 11:40a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 * 
 * 58   1/8/13 6:06p randyjew
 * SW7563-41: Add support for 7563
 * 
 * 57   10/31/12 6:53p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 56   10/10/12 3:47p chengs
 * SW7584-51: Add splash screen support for 7584.
 * 
 * 55   10/5/12 6:37p bselva
 * SW7360-38: Add splash support
 * 
 * 54   9/28/12 6:23p mward
 * SW7435-114:  Don't write all surface information for 2nd surface, only
 * the address.
 * 
 * 53   9/28/12 4:27p mward
 * SW7435-114:  RTS for 7435 in 1u4t mode requires feeding SD and HD
 * displays from separate MEMCs.
 * 
 * 52   9/24/12 2:34p katrep
 * SW7445-1: updated for 7445
 * 
 * 51   9/18/12 2:12p jessem
 * SW7425-3872: Removed BSPLASH_RDC_SCRATCH_DISP1_REG1.
 *
 * 50   9/10/12 4:56p jessem
 * SW7425-3872: Removed #include "brdc_private.h"
 *
 * 49   7/23/12 5:05p mward
 * SW7125-1306:  Correct typo.
 *
 * 48   4/26/12 4:28p katrep
 * SW7429-1:spalsh support
 *
 * 47   4/26/12 4:24p katrep
 * SW7429-1:add spplash support
 *
 * 46   4/19/12 1:45p mward
 * SW7435-114:  Add support for 7435.
 *
 * 45   4/19/12 1:10p jessem
 * SW7425-2828: Reduced console clutter.
 *
 * 44   4/18/12 2:47p jessem
 * SW7425-2828: Corrected method on obtaining heap. Added run-time options
 * to set the display format and to disable the 2nd display. Backed-out
 * of creating a separate surface for the the 2nd display.
 *
 * 43   4/12/12 4:18p randyjew
 * SWCFE-769: Fix include error for 7346.
 *
 * 42   4/9/12 11:25a jessem
 * SW7425-2828: Removed hardcoded RDC scratch registers and use
 * BVDC_Test_Source_GetGfdScratchRegisters() instead. Also, updated the
 * implementation of the splash surface for the 2nd display.
 *
 * 41   3/22/12 3:50p katrep
 * SW7231-737:use primary diplay size of secomdary display is disabled
 *
 * 40   3/15/12 3:43p jessem
 * SW7425-1407: Apply 7425 B0 fixes to 7425 Bx.
 *
 * 39   2/3/12 3:20p katrep
 * SW7429-1:build issue
 *
 * 38   12/8/11 5:33p xhuang
 * SW7552-169: merge for 7552 B0
 *
 * SW7552-169/1   12/7/11 2:02p jianweiz
 * SW7552-169: Add support for 7552B0
 *
 * 37   11/11/11 10:56a jessem
 * SW7425-1527: Excluded BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15 and
 * BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0. Added register block names in
 * register dump to aid in debugging.
 *
 * 36   10/31/11 7:10p randyjew
 * SW7418-13: Add Splash support for 7344B0
 *
 * 35   10/5/11 1:01p jessem
 * SW7425-1407: Added support ofr 7425B0.
 *
 * 34   10/3/11 11:28a katrep
 * SW7231-220:add 7131 B0 support
 *
 * 33   9/27/11 7:20p bselva
 * SW7358-147: Added the changes for 7358 platform
 *
 * 33   9/27/11 7:19p bselva
 * SW7358-147: Added the changes for 7358 platform
 *
 * 32   9/23/11 3:37p katrep
 * SW7429-1:7429 support
 *
 * 31   8/23/11 10:03a jrubio
 * SW7340-277: add 7340 support
 *
 * 30   8/16/11 5:20p jessem
 * SW7425-1122: Merge SW7552-48 changes to main line.
 *
 * 29   8/8/11 11:18p nickh
 * SW7425-878: Add 7425 splash support
 *
 * 28   7/12/11 1:16p xhuang
 * SW7552-48: Add splash screen support for 7552
 *
 * 27   6/29/11 7:27p randyjew
 * SW7418-13: Add 7346 support
 *
 * 26   6/29/11 4:17p randyjew
 * SW7418-13: Add support for 7344/7418
 *
 * 25   6/27/11 11:34a katrep
 * SW7231-220:add support for 7231 &newer 40nm chips
 *
 * 24   3/8/11 9:10a jrubio
 * SW7325-843: add PAL spalsh
 *
 * 23   2/24/11 5:09p gmohile
 * SW7408-239 : Add 7408 splash support
 *
 * 22   8/11/10 6:05p mward
 * SW7125-569:  Add splash support for 97125.
 *
 * 21   4/30/10 5:02p randyjew
 * SW7468-6: Add 7208/7468 Splash Screen Support
 *
 * 20   4/9/10 3:30p rjain
 * SW7550-284: adding supoprt for 7550. Also adding code to dump reg names
 * (from David Kaufman)
 *
 * 19   9/21/09 5:17p nickh
 * SW7420-351: Pass in appropriate heaps for 7420 and modify VEC register
 * naming for 7420
 *
 * 18   5/14/09 5:41p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 17   4/10/09 1:55p shyam
 * PR52386 : Fix as per new RDC API
 *
 * 16   4/8/09 4:28p shyam
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
 * 15   10/17/07 6:49p shyam
 * PR 30741 : Add error resilience if reg interface is not instrumented
 *
 * 14   10/9/07 12:31p mward
 * PR34212: Port splash to 97118[RNG] platform.
 *
 * 13   10/8/07 6:33p shyam
 * PR 30741 : Simplify the bitmap filename
 *
 * 12   9/11/07 5:18p shyam
 * PR 28750 : Add error returns to intercepts
 *
 * 11   9/7/07 1:49p shyam
 * PR 30741 : RDC hooks as function pointers and added board configration
 * for default primary/secondary displays
 *
 * 10   8/21/07 12:27p shyam
 * PR 34013 : A bug in the fix for this Sec vec list in prim trigger issue
 *
 * 9   8/13/07 5:30p shyam
 * PR 34013 : Fixed Sec Vec rul in the primary list bug
 *
 * 8   8/9/07 3:11p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 7   8/1/07 3:53p shyam
 * PR 28750 : Added hooks into RDC for capturing RULs
 *
 * 6   7/31/07 4:02p shyam
 * PR 33658 : Port splash to 97403 platform
 *
 * 5   7/20/07 4:22p shyam
 * PR 30741 : Fix issue with HDMI+480i support
 *
 * 4   7/18/07 12:35p shyam
 * PR 30741 : Add support for 480p on HDMI and NTSC on composite
 *
 * 3   6/28/07 5:03p shyam
 * PR 30741 : Syncup with cfe portability
 *
 * 2   6/27/07 6:20p shyam
 * PR 30741 : 1. Add BMP file support
 * 2. magnum compliant RDC hooks
 *
 * 1   5/14/07 6:51p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 * Hydra_Software_Devel/2   2/6/06 7:33a dkaufman
 * PR3481: Added surface update to verify graphics config; fixed to handle
 * bTrackExecution flag on RULs
 *
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 *
 ***************************************************************************/

#include "splash_vdc_rulgen.h"

BDBG_MODULE("splash_vdc_generate");

static BERR_Code APP_BRDC_Slot_SetList_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_List_Handle hList,
    BRDC_Trigger     eTrigger
);
static BERR_Code APP_BRDC_Slot_ExecuteOnTrigger_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_Trigger     eRDCTrigger,
    bool             bRecurring
);
static BERR_Code APP_BRDC_Slot_Execute_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_Trigger     eTrigger
);

/* RUL storage */
#define MAX_NUM_TRIGGERS        12
#define MAX_RUL_COUNT           500
#define MAX_RUL_ENTRIES         2000

BRDC_List_Handle g_aRulList[MAX_RUL_COUNT];
BRDC_List_Handle g_aHandleList[MAX_RUL_COUNT];
int g_iListCount = 0;
bool g_bExecuteList[MAX_NUM_TRIGGERS];
bool g_bFirstBottom[MAX_NUM_TRIGGERS];

/* first lists to execute for primary t/b and secondary t/b */
int g_aTopLists[MAX_NUM_TRIGGERS] = { -1 };

int g_aListOrder[MAX_NUM_TRIGGERS][MAX_RUL_COUNT] = { { -1 } };
int g_aListOrderCount[MAX_NUM_TRIGGERS] = { 0 };

/* Mapping structure for the slots/triggers and the lists */
struct TriggerMap
{
    int TriggerHwNum; /* The hardware position of the trigger */
    int SlotNum;      /* The slot correposponding to the Slot */
} g_asTriggerMap[MAX_NUM_TRIGGERS];

extern ModeHandles g_stModeHandles;

/* register storage */
#define MAX_REG_ENTRIES 10000

uint32_t g_RegDataCount = 0;

struct {
    uint32_t    reg;
    uint32_t    val;
} g_RegData[MAX_REG_ENTRIES];

static void InitializeStorage(
    BRDC_Handle  hRdc
    )
{
    BERR_Code  eErr;
    int        i;

    /* allocate all temporary lists */
    for(i=0; i<MAX_RUL_COUNT; ++i)
    {
        /* allocate RUL */
        TestError( BRDC_List_Create(hRdc, MAX_RUL_ENTRIES,
            g_aRulList + i),
            "ERROR: BRDC_List_Create" );

        /* clear source handle */
        g_aHandleList[i] = NULL;
    }

done:
    return;
}

static void UnInitializeStorage(void)
{
    BERR_Code  eErr;
    int        i;

    /* allocate all temporary lists */
    for(i=0; i<MAX_RUL_COUNT; ++i)
    {
        /* allocate RUL */
        TestError( BRDC_List_Destroy(g_aRulList[i]),
            "ERROR: BRDC_List_Destroy" );
    }

done:
    return;
}

/* searches a list and looks for a setting for a specific register. When found it will
   mask the existing value and then OR in the data value. This is mainly used to change
    register values that will not run in a script based system. */
static void ReplaceRegisterList(
    BRDC_List_Handle  hList,
    uint32_t          ulReg,
    uint32_t          ulMask,
    uint32_t          ulData
    )
{
    BRDC_DBG_ListEntry  eEntry;
    uint32_t            aulArgs[4];
    bool                bCheckCommand = false;
    uint32_t            ulCurrRegister = 0;
    uint32_t           *pulAddress;


    /* get address to list */
    pulAddress = BRDC_List_GetStartAddress_isr(hList);

    /* prepare to traverse this list */
    if (BRDC_DBG_SetList(hList) != BERR_SUCCESS)
    {
        /* error */
        BDBG_ERR(("ERROR parsing list %d", __LINE__));
        goto done;
    }

    /* get first entry in list */
    if(BRDC_DBG_GetListEntry(hList, &eEntry, aulArgs))
    {
        /* error */
        BDBG_ERR(("ERROR parsing list %d", __LINE__));
        goto done;
    }

    /* traverse until finished */
    while(eEntry != BRDC_DBG_ListEntry_eEnd)
    {
        /*printf("1--ReplaceRegisterList\n");*/
        /* command entry? */
        if(eEntry == BRDC_DBG_ListEntry_eCommand)
        {
            /*printf("2--ReplaceRegisterList\n");*/
            /* is this a write to register command? */
            bCheckCommand = false;
            switch(aulArgs[0])
            {
                case BRDC_OP_VAR_TO_REG_OPCODE:
                case BRDC_OP_REG_TO_REG_OPCODE:
                case BRDC_OP_REGS_TO_REGS_OPCODE:
                case BRDC_OP_REG_TO_REGS_OPCODE:
                case BRDC_OP_REGS_TO_REG_OPCODE:
                    /* unhandled */
                    BDBG_MSG(("Unhandled register write command!!!!"));
                    break;

                case BRDC_OP_IMMS_TO_REG_OPCODE:
                case BRDC_OP_IMMS_TO_REGS_OPCODE:
                    bCheckCommand = true;
                    break;

                case BRDC_OP_IMM_TO_REG_OPCODE:
                    bCheckCommand = true;
                    break;

                default:
                    break;
            }

        /* check this command contents? */
        } else if(bCheckCommand)
        {
            /* register? */
            if(eEntry == BRDC_DBG_ListEntry_eRegister)
            {
                /* store register */
                ulCurrRegister = aulArgs[0];

            } else if(eEntry == BRDC_DBG_ListEntry_eData)
            {
                /* data -- do we have the right register? */
                if(ulCurrRegister == ulReg)
                {
                    /* update register with new value */
                    *pulAddress &= ulMask;
                    *pulAddress |= ulData;
                    BDBG_MSG(("Replacing RUL register %08x old: %08x, new %08x",
                        ulReg, aulArgs[0], *pulAddress));

                /* not a match */
                } else
                {
                    /* assume that the next data goes with the next register */
                    ulCurrRegister += 4;
                }
            }
        }

        /* get next entry in list */
        pulAddress++;
        if(BRDC_DBG_GetListEntry(hList, &eEntry, aulArgs))
        {
            /* error */
            BDBG_ERR(("ERROR parsing list %d", __LINE__));
            goto done;
        }
    }

done:
    return;
}

#define Stringize(a)        #a

static void WriteSplashInfo(FILE *fp, ModeHandles *pMode)
{
    uint32_t        surfWidth;
    uint32_t        surfHeight;
    void*           surfAddress;
    uint32_t        surfPitch;
    BPXL_Format     surfFormat;
    BFMT_VideoInfo  videoInfo;
	SplashDisplay   *pDisp;
	SplashSurface   *pSurf;
	int  ii;
	
	
    fprintf(fp,"/***************************************************************************\n");
    fprintf(fp," File : splash_vdc_rul.h\n");
    fprintf(fp," Date  : %s\n", __DATE__ " " __TIME__);
    fprintf(fp," Description : RUL list for splash BVN configuration\n");
    fprintf(fp," ***************************************************************************/\n\n");
    fprintf(fp,"#include \"splash_magnum.h\"\n\n\n");

    fprintf(fp,"#define BSPLASH_NUM_MEM\t\t\t %d\n\n", SPLASH_NUM_MEM);

	fprintf(fp,"int g_iRulMemIdx = %d;\t/* index to ahMem for vdc/rul usage */\n\n", pMode->iRulMemIdx);
	
	fprintf(fp,"#define BSPLASH_NUM_SURFACE\t\t %d\n", SPLASH_NUM_SURFACE);
	fprintf(fp,"SplashSurfaceInfo  g_SplashSurfaceInfo[BSPLASH_NUM_SURFACE] =\n{\n");
	for (ii=0; ii<SPLASH_NUM_SURFACE; ii++)
	{
		pSurf = &pMode->surf[ii];
		BSUR_Surface_GetDimensions(pSurf->hSurface, &surfWidth, &surfHeight);
		BSUR_Surface_GetAddress(pSurf->hSurface, &surfAddress, &surfPitch);
		BSUR_Surface_GetFormat(pSurf->hSurface, &surfFormat);
		fprintf(fp,"\t{   /* sur %d: */\n", ii);
		fprintf(fp,"\t\t%d,\t\t\t\t\t\t\t/* ihMemIdx */\n", pSurf->iMemIdx);
		fprintf(fp,"\t\t(BPXL_Format)0x%8.8x,\t/* %s */\n", surfFormat, BPXL_ConvertFmtToStr(surfFormat));		
		fprintf(fp,"\t\t%d,\t\t\t\t\t\t/* width */\n", surfWidth);
		fprintf(fp,"\t\t%d,\t\t\t\t\t\t/* height */\n", surfHeight);
		fprintf(fp,"\t\t%d,\t\t\t\t\t\t/* pitch */\n", surfPitch);
		fprintf(fp,"\t\t\"%s\"\t\t\t\t/* bmp file in splashgen */\n", &pSurf->bmpFile[0]);
		if ((SPLASH_NUM_SURFACE-1) == ii)
			fprintf(fp,"\t}\n");
		else
			fprintf(fp,"\t},\n");
	}
	fprintf(fp,"};\n\n");
	
	fprintf(fp,"#define BSPLASH_NUM_DISPLAY\t\t %d\n", SPLASH_NUM_DISPLAY);
	fprintf(fp,"SplashDisplayInfo  g_SplashDisplayInfo[BSPLASH_NUM_DISPLAY] =\n{\n");
	for (ii=0; ii<SPLASH_NUM_DISPLAY; ii++)
	{
		pDisp = &pMode->disp[ii];
		BFMT_GetVideoFormatInfo(pDisp->eDispFmt, &videoInfo);
		fprintf(fp,"\t{   /* disp %d: */\n", ii);
		fprintf(fp,"\t\t%d,\t\t\t\t\t/* iSurIdx */\n", pDisp->iSurfIdx);
		fprintf(fp,"\t\t(BFMT_VideoFmt)%d,\t/* %s */\n", pDisp->eDispFmt, videoInfo.pchFormatStr);
		fprintf(fp,"\t\t0x%8.8x,\t\t\t/* ulRdcScratchReg0 */\n", pDisp->ulGfdScratchReg0);
		fprintf(fp,"\t\t0x%8.8x \t\t\t/* ulRdcScratchReg1 */\n", pDisp->ulGfdScratchReg1);
		if ((SPLASH_NUM_DISPLAY-1) == ii)
			fprintf(fp,"\t}\n");
		else
			fprintf(fp,"\t},\n");
	}
	fprintf(fp,"};\n\n\n");
}


static void DumpLists(
    char *pchFileName,
    ModeHandles* pMode
    )
{
    FILE *fp;
    int i;
    uint32_t ulNumEntries;
    uint32_t *pulStartAddress;
    int iListIndex;
    int iIndex;
    int iTrigger;
    uint32_t ulCount;
    uint32_t totalListCount = 0;

    fp = fopen(pchFileName, "w");
    if(!fp)
    {
        BDBG_ERR(("ERROR: cannot open file '%s' %d", pchFileName, errno));
        goto done;
    }

    BDBG_MSG(("DUMPING LISTS to %s", pchFileName));

    /* splash surface and format info */

    WriteSplashInfo(fp, pMode);

    /* dump lists in order of execution for a given trigger */
    for(iTrigger=0; iTrigger<MAX_NUM_TRIGGERS; iTrigger++)
    {
        totalListCount += g_aListOrderCount[iTrigger];
        if(g_aListOrderCount[iTrigger] == 0)
        {
            BDBG_MSG(("Skipping empty trigger %d", iTrigger, g_aListOrderCount[iTrigger]));
            continue;
        }
        BDBG_MSG(("Writing trigger %d, cntr", iTrigger, g_aListOrderCount[iTrigger]));

        /* count header */
        fprintf(fp, "\nuint32_t g_aListCount%d[] = \n{\n",
            iTrigger);

        ulCount = 0;

        /* if(iTrigger == 6)
            printf("totalListCount %d g_aListOrderCount[%d]%d\n",totalListCount,iTrigger,g_aListOrderCount[iTrigger]); */

		for(iIndex=0; iIndex < g_aListOrderCount[iTrigger]; iIndex++)
        {
            /* index of list */
            iListIndex = g_aListOrder[iTrigger][iIndex];

#ifdef BCHP_IT_0_VEC_TRIGGER_0
            /* replace list entries so resulting RULs will work without _isr assistance */
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_0_VEC_TRIGGER_0,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0,ENABLE,1));

            /* todo for progressive formats this triiger is not required */
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_0_VEC_TRIGGER_1,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_1,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1,ENABLE,1));
#elif defined BCHP_PRIM_IT_VEC_TRIGGER_0
            /* replace list entries so resulting RULs will work without _isr assistance */
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_PRIM_IT_VEC_TRIGGER_0,
                ~BCHP_MASK(PRIM_IT_VEC_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_0,ENABLE,1));
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_PRIM_IT_VEC_TRIGGER_1,
                ~BCHP_MASK(PRIM_IT_VEC_TRIGGER_1,ENABLE),
                BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_1,ENABLE,1));
#else
#error "Port required for splashgen, talk to VDC team"
#endif
 
#ifdef BCHP_IT_1_VEC_TRIGGER_0
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_1_VEC_TRIGGER_0,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0,ENABLE,1));
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_1_VEC_TRIGGER_1,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_1,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1,ENABLE,1));
#elif defined BCHP_SEC_IT_VEC_TRIGGER_0
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_SEC_IT_VEC_TRIGGER_0,
                ~BCHP_MASK(SEC_IT_VEC_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(SEC_IT_VEC_TRIGGER_0,ENABLE,1));
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_SEC_IT_VEC_TRIGGER_1,
                ~BCHP_MASK(SEC_IT_VEC_TRIGGER_1,ENABLE),
                BCHP_FIELD_DATA(SEC_IT_VEC_TRIGGER_1,ENABLE,1));
#endif

#ifdef BCHP_IT_2_VEC_TRIGGER_0
			/* To check with VDC team about this */
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_2_VEC_TRIGGER_0,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0,ENABLE,1));
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_IT_2_VEC_TRIGGER_1,
                ~BCHP_MASK(IT_0_VEC_TRIGGER_1,ENABLE),
                BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1,ENABLE,1));
#endif

#ifdef BCHP_DVI_DTG_0_DTG_TRIGGER_0
			/* To check with VDC team about this */
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_DVI_DTG_0_DTG_TRIGGER_0,
                ~BCHP_MASK(DVI_DTG_0_DTG_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0,ENABLE,1));
            ReplaceRegisterList(g_aRulList[iListIndex],BCHP_DVI_DTG_0_DTG_TRIGGER_1,
                ~BCHP_MASK(DVI_DTG_0_DTG_TRIGGER_0,ENABLE),
                BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0,ENABLE,1));
#endif
			
            /* get entries for list */
            BRDC_List_GetNumEntries_isr(g_aRulList[iListIndex], &ulNumEntries);

            /* print num entries */
            fprintf(fp, "\t%d,\n", ulCount);

            /* increment count */
            ulCount += ulNumEntries;
        }

        /* count end */
        fprintf(fp, "\t%d,\n};\n", ulCount);

        /* trigger header */
        fprintf(fp, "\nuint32_t g_aList%d[] = \n{\n",
            iTrigger);

        for(iIndex=0; iIndex < g_aListOrderCount[iTrigger]; iIndex++)
        {
            /* index of list */
            iListIndex = g_aListOrder[iTrigger][iIndex];

            /* get entries for list */
            BRDC_List_GetNumEntries_isr(g_aRulList[iListIndex], &ulNumEntries);
            pulStartAddress = BRDC_List_GetStartAddress_isr(g_aRulList[iListIndex]);

            /* header */
            fprintf(fp, "\n\t/* LIST: %d (%d entries) */\n",
                iListIndex, (int)ulNumEntries);

            /* body */
            for(i=0; i<(int)ulNumEntries; i++)
            {
                fprintf(fp, "\t0x%08x,\n", *(pulStartAddress++));
            }
        }

        /* trigger close */
        fprintf(fp, "};\n");
    }

    if(totalListCount==0)
    {
        BDBG_ERR(("***************************************************************************"));
        BDBG_ERR(("Error ... Error ... Error ! " "\n" "RUL Dump is empty !!! "));
		BDBG_ERR(("Looks like your chip uses a new RDC trigger"));
		BDBG_ERR(("   add them into isTrigger() and GetArrayIndex()"));
        BDBG_ERR(("or you have not chosen the right version of RDC"));
        BDBG_ERR(("   please pickup the instrumented version from LATEST RDC commonutils"));
        BDBG_ERR(("   or reuqest VDC team to enable splash on RDC for your chip"));
        BDBG_ERR(("***************************************************************************"));
        BDBG_ERR((""));
        fclose(fp);
        return;
    }

    /* Generate the trigger map */
    fprintf(fp,
        "\n\nstruct stTriggerMap\n"
        "{\n"
        "\tint TriggerHwNum;             /* The hardware position of the trigger */\n"
        "\tint SlotNum;                  /* The RDMA slot correposponding to the trigger */\n"
        "\tint ListCountMaxIndex;       /* The Max Index of the ListCount Array */\n"
        "\tuint32_t *aListCountArray; /* The list count array pointer */\n"
        "\tuint32_t *aListArray;      /* The list array pointer */\n"
        "\t} g_aTriggerMap[] = \n"
        "{\n"
    );
    for(iTrigger=0; iTrigger<MAX_NUM_TRIGGERS; iTrigger++)
    {
        char listcount[50],list[50];
        int lclTriggerHwNum, lclSlotnum;
        if(g_aListOrderCount[iTrigger])
        {
            sprintf(listcount,"g_aListCount%d",iTrigger);
            sprintf(list,"g_aList%d",iTrigger);
            lclTriggerHwNum = g_asTriggerMap[iTrigger].TriggerHwNum;
            lclSlotnum = g_asTriggerMap[iTrigger].SlotNum;
        }
        else
        {
            sprintf(listcount,"NULL");
            sprintf(list,"NULL");
            lclTriggerHwNum = -1;
            lclSlotnum = -1;
        }

        fprintf(fp," { %d, %d, %d, %s, %s },\n",
            lclTriggerHwNum,
            lclSlotnum,
            g_aListOrderCount[iTrigger],
            listcount,
            list);
    }
    fprintf(fp, "\n};\n\n\n");
    fclose(fp);

done:
    return;
}

#define ENT(_start_, _end_, _desc_) { _start_, _end_, _desc_ }

static struct
{
    uint32_t        start;
    uint32_t        end;
    char*  desc;
}
RegisterNames[]=
{
	/* new ENT add should use BCHP_*_REG_START and BCHP_*_REG_END in bchp_common.h,
	 * so you don't need to add any new include for this new ENT */
#if BCHP_FMISC_REG_START
	ENT(BCHP_FMISC_REG_START, BCHP_FMISC_REG_END, "FMISC"),
#endif
#if BCHP_MMISC_REG_START
	ENT(BCHP_MMISC_REG_START, BCHP_MMISC_REG_END, "MMISC"),
#endif
#if BCHP_BMISC_REG_START
	ENT(BCHP_BMISC_REG_START, BCHP_BMISC_REG_END, "BMISC"),
#endif
#if BCHP_DMISC_REG_START
	ENT(BCHP_DMISC_REG_START, BCHP_DMISC_REG_END, "DMISC"),
#endif
#if BCHP_BVNF_INTR2_0_REG_START
	ENT(BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_0_REG_END, "BVNF_INTR2_0"),
#endif
#if BCHP_BVNF_INTR2_1_REG_START
	ENT(BCHP_BVNF_INTR2_1_REG_START, BCHP_BVNF_INTR2_1_REG_END, "BVNF_INTR2_1"),
#endif
#if BCHP_BVNF_INTR2_2_REG_START
	ENT(BCHP_BVNF_INTR2_2_REG_START, BCHP_BVNF_INTR2_2_REG_END, "BVNF_INTR2_2"),
#endif
#if BCHP_BVNF_INTR2_3_REG_START
	ENT(BCHP_BVNF_INTR2_3_REG_START, BCHP_BVNF_INTR2_3_REG_END, "BVNF_INTR2_3"),
#endif
#if BCHP_BVNF_INTR2_4_REG_START
	ENT(BCHP_BVNF_INTR2_4_REG_START, BCHP_BVNF_INTR2_4_REG_END, "BVNF_INTR2_4"),
#endif
#if BCHP_BVNF_INTR2_5_REG_START
	ENT(BCHP_BVNF_INTR2_5_REG_START, BCHP_BVNF_INTR2_5_REG_END, "BVNF_INTR2_5"),
#endif
#if BCHP_BVNF_INTR2_6_REG_START
	ENT(BCHP_BVNF_INTR2_6_REG_START, BCHP_BVNF_INTR2_6_REG_END, "BVNF_INTR2_6"),
#endif
#if BCHP_BVNF_INTR2_7_REG_START
	ENT(BCHP_BVNF_INTR2_7_REG_START, BCHP_BVNF_INTR2_7_REG_END, "BVNF_INTR2_7"),
#endif
#if BCHP_BVNM_INTR2_0_REG_START
	ENT(BCHP_BVNM_INTR2_0_REG_START, BCHP_BVNM_INTR2_0_REG_END, "BVNM_INTR2_0"),
#endif
#if BCHP_BVNM_INTR2_1_REG_START
	ENT(BCHP_BVNM_INTR2_1_REG_START, BCHP_BVNM_INTR2_1_REG_END, "BVNM_INTR2_1"),
#endif
#if BCHP_BVNB_INTR2_REG_START
	ENT(BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END, "BVNB_INTR2"),
#endif
#if BCHP_CLKGEN_INTR2_REG_START
	ENT(BCHP_CLKGEN_INTR2_REG_START, BCHP_CLKGEN_INTR2_REG_END, "CLKGEN_INTR2"),
#endif
#if BCHP_VCXO_0_RM_REG_START
	ENT(BCHP_VCXO_0_RM_REG_START, BCHP_VCXO_0_RM_REG_END, "VCXO_0_RM"),
#endif
#if BCHP_VCXO_1_RM_REG_START
	ENT(BCHP_VCXO_1_RM_REG_START, BCHP_VCXO_1_RM_REG_END, "VCXO_1_RM"),
#endif
#if BCHP_VCXO_2_RM_REG_START
	ENT(BCHP_VCXO_2_RM_REG_START, BCHP_VCXO_2_RM_REG_END, "VCXO_2_RM"),
#endif
#if (BCHP_CLKGEN_REG_END && BCHP_CLKGEN_REG_START)
	ENT(BCHP_CLKGEN_REG_START, BCHP_CLKGEN_REG_END, "CLKGEN"),
#endif
#if (BCHP_CLKGEN_GR_REG_END && BCHP_CLKGEN_GR_REG_START)
	ENT(BCHP_CLKGEN_GR_REG_START, BCHP_CLKGEN_GR_REG_END, "CLKGEN_GR"),
#endif
#if (BCHP_GFD_0_REG_START)
	ENT(BCHP_GFD_0_REG_START, BCHP_GFD_0_REG_END, "GFD_0"),
#endif
#if (BCHP_GFD_1_REG_START)
	ENT(BCHP_GFD_1_REG_START, BCHP_GFD_1_REG_END, "GFD_1"),
#endif
#if (BCHP_GFD_2_REG_START)
	ENT(BCHP_GFD_2_REG_START, BCHP_GFD_2_REG_END, "GFD_2"),
#endif
#if (BCHP_GFD_3_REG_START)
	ENT(BCHP_GFD_3_REG_START, BCHP_GFD_3_REG_END, "GFD_3"),
#endif
#if (BCHP_GFD_4_REG_START)
	ENT(BCHP_GFD_4_REG_START, BCHP_GFD_4_REG_END, "GFD_4"),
#endif
#if (BCHP_GFD_5_REG_START)
	ENT(BCHP_GFD_5_REG_START, BCHP_GFD_5_REG_END, "GFD_5"),
#endif
#if (BCHP_GFD_6_REG_START)
	ENT(BCHP_GFD_6_REG_START, BCHP_GFD_6_REG_END, "GFD_6"),
#endif
#if (BCHP_CMP_0_REG_START)
	ENT(BCHP_CMP_0_REG_START, BCHP_CMP_0_REG_END, "CMP_0"),
#endif
#if (BCHP_CMP_1_REG_START)
	ENT(BCHP_CMP_1_REG_START, BCHP_CMP_1_REG_END, "CMP_1"),
#endif
#if (BCHP_CMP_2_REG_START)
	ENT(BCHP_CMP_2_REG_START, BCHP_CMP_2_REG_END, "CMP_2"),
#endif
#if (BCHP_CMP_3_REG_START)
	ENT(BCHP_CMP_3_REG_START, BCHP_CMP_3_REG_END, "CMP_3"),
#endif
#if (BCHP_CMP_4_REG_START)
	ENT(BCHP_CMP_4_REG_START, BCHP_CMP_4_REG_END, "CMP_4"),
#endif
#if (BCHP_CMP_5_REG_START)
	ENT(BCHP_CMP_5_REG_START, BCHP_CMP_5_REG_END, "CMP_5"),
#endif
#if (BCHP_CMP_6_REG_START)
	ENT(BCHP_CMP_6_REG_START, BCHP_CMP_6_REG_END, "CMP_6"),
#endif
#if BCHP_RAAGA_DSP_MISC_1_REG_START
	ENT(BCHP_RAAGA_DSP_MISC_1_REG_START, BCHP_RAAGA_DSP_MISC_1_REG_END, "RAAGA_DSP_MISC_1"),
#endif
#if BCHP_AIO_INTH_REG_START
	ENT(BCHP_AIO_INTH_REG_START, BCHP_AIO_INTH_REG_END, "AIO_INTH"),
#endif
#if BCHP_SID_L2_REG_START
	ENT(BCHP_SID_L2_REG_START, BCHP_SID_L2_REG_END, "SID_L2"),
#endif
#if BCHP_SCPU_HOST_INTR2_REG_START
	ENT(BCHP_SCPU_HOST_INTR2_REG_START, BCHP_SCPU_HOST_INTR2_REG_END, "SCPU_HOST_INTR2"),
#endif
#if BCHP_RAAGA_DSP_INTH_1_REG_START
	ENT(BCHP_RAAGA_DSP_INTH_1_REG_START, BCHP_RAAGA_DSP_INTH_1_REG_END, "RAAGA_DSP_INTH_1"),
#endif
#if BCHP_RAAGA_DSP_FW_INTH_1_REG_START
	ENT(BCHP_RAAGA_DSP_FW_INTH_1_REG_START, BCHP_RAAGA_DSP_FW_INTH_1_REG_END, "RAAGA_DSP_FW_INTH_1"),
#endif
#if (BCHP_SUN_TOP_CTRL_SW_RESET && BCHP_SUN_TOP_CTRL_SW_RESET)
	ENT(BCHP_SUN_TOP_CTRL_SW_RESET, BCHP_SUN_TOP_CTRL_SW_RESET, "SUN_TOP_CTRL_SW_RESET"),
#endif
#if (BCHP_SUN_TOP_CTRL_SW_INIT_0_SET && BCHP_SUN_TOP_CTRL_SW_INIT_0_SET)
	ENT(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, "SUN_TOP_CTRL_SW_RESET"),
#endif
#if (BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR && BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR)
	ENT(BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, "SUN_TOP_CTRL_SW_INIT_0_CLEAR"),	
#endif
#if (BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12 && BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0)
	ENT(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12, "SUN_TOP_CTRL_PIN_MUX"),
#endif
#if BCHP_SUN_TOP_CTRL_REG_START
	ENT(BCHP_SUN_TOP_CTRL_REG_START, BCHP_SUN_TOP_CTRL_REG_END, "SUN_TOP_CTRL ..."),
#endif
#if BCHP_AON_PIN_CTRL_REG_START
	ENT(BCHP_AON_PIN_CTRL_REG_START, BCHP_AON_PIN_CTRL_REG_END, "AON_PIN_MUX"),
#endif
#if (BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL && BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL)
	ENT(BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL,BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL, "M2MC_SW_INIT"),
#endif
#if BCHP_M2MC_REG_START
	ENT(BCHP_M2MC_REG_START, BCHP_M2MC_REG_END, "M2MC"),
#endif


#if (BCHP_DVP_DGEN_0_DVO_0_TEST_DATA_GEN_CFG_14 && BCHP_MISC_MISC_REVISION_ID)
	ENT(BCHP_MISC_MISC_REVISION_ID, BCHP_DVP_DGEN_0_DVO_0_TEST_DATA_GEN_CFG_14, "VEC"),
#endif
#if (BCHP_DVP_TVG_0_TVG_3D_CFG_3 && BCHP_MISC_MISC_REVISION_ID)
	ENT(BCHP_MISC_MISC_REVISION_ID, BCHP_DVP_TVG_0_TVG_3D_CFG_3, "VEC..."),
#endif
#if (BCHP_MISC_DAC_CRC_STATUS && BCHP_MISC_MISC_REVISION_ID)
	ENT(BCHP_MISC_MISC_REVISION_ID, BCHP_MISC_DAC_CRC_STATUS, "VEC"),
#endif
#if (BCHP_MEMC_GEN_0_ZQCS_PERIOD && BCHP_MEMC_GEN_0_CORE_REV_ID)
	ENT(BCHP_MEMC_GEN_0_CORE_REV_ID, BCHP_MEMC_GEN_0_ZQCS_PERIOD, "MEMC0"),
#endif
#if (BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_AON_STORAGE_IN_PHY_3 && BCHP_MEMC_GEN_0_CORE_REV_ID)
	ENT(BCHP_MEMC_GEN_0_CORE_REV_ID, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_AON_STORAGE_IN_PHY_3, "MEMC0..."),
#endif
#if (BCHP_MEMC_GEN_1_ZQCS_PERIOD && BCHP_MEMC_GEN_1_CORE_REV_ID)
	ENT(BCHP_MEMC_GEN_1_CORE_REV_ID, BCHP_MEMC_GEN_1_ZQCS_PERIOD, "MEMC1"),
#endif
#if (BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_AON_STORAGE_IN_PHY_3 && BCHP_MEMC_GEN_1_CORE_REV_ID)
	ENT(BCHP_MEMC_GEN_1_CORE_REV_ID, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_AON_STORAGE_IN_PHY_3, "MEMC1"),
#endif
	ENT(BCHP_RDC_REG_START, BCHP_RDC_REG_START, "RDC"),
	ENT(BCHP_VNET_F_REG_START, BCHP_VNET_F_REG_START, "VNET_F"),
	ENT(BCHP_VNET_B_REG_START, BCHP_VNET_B_REG_START, "VNET_B"),
#if (BCHP_MEMC_GEN_2_ZQCS_PERIOD && BCHP_MEMC_GEN_2_CORE_REV_ID)
	ENT(BCHP_MEMC_GEN_2_CORE_REV_ID, BCHP_MEMC_GEN_2_ZQCS_PERIOD, "MEMC2"),
#endif
#if (BCHP_VFD_4_SCRATCH_REGISTER_1 && BCHP_MFD_0_REVISION_ID)
	ENT(BCHP_MFD_0_REVISION_ID, BCHP_VFD_4_SCRATCH_REGISTER_1, "MFD/VFD..."),
#endif
#if (BCHP_DVP_HR_TMR_WDCTRL && BCHP_DVP_HT_CORE_REV)
	ENT(BCHP_DVP_HT_CORE_REV, BCHP_DVP_HR_TMR_WDCTRL, "DVP HT and DVP HR..."),
#endif
#if (BCHP_AON_CTRL_ANA_XTAL_EXT_CML_CONTROL && BCHP_AON_CTRL_RESET_CTRL)
	ENT(BCHP_AON_CTRL_RESET_CTRL, BCHP_AON_CTRL_ANA_XTAL_EXT_CML_CONTROL, "AON..."),
#endif
#if (BCHP_AON_HDMI_RX_I2C_PASS_THROUGH_CONFIG2 && BCHP_AON_CTRL_RESET_CTRL)
	ENT(BCHP_AON_CTRL_RESET_CTRL, BCHP_AON_HDMI_RX_I2C_PASS_THROUGH_CONFIG2, "AON..."),
#endif
#if (BCHP_AON_HDMI_RX_I2C_PASS_THROUGH_CONFIG1 && BCHP_AON_CTRL_RESET_CTRL)
	ENT(BCHP_AON_CTRL_RESET_CTRL, BCHP_AON_HDMI_RX_I2C_PASS_THROUGH_CONFIG1, "AON..."),
#endif
#if (BCHP_HDMI_RAM_PACKET_RSVD_1 && BCHP_HDMI_BKSV0)
	ENT(BCHP_HDMI_BKSV0, BCHP_HDMI_RAM_PACKET_RSVD_1, "HDMI TX Control"),
#endif
#if (BCHP_SUN_GISB_ARB_ERR_CAP_MASTER && BCHP_SUN_GISB_ARB_REVISION)
	ENT(BCHP_SUN_GISB_ARB_REVISION, BCHP_SUN_GISB_ARB_ERR_CAP_MASTER, "SUN_GISB_ARB_..."),	
#endif
#if (BCHP_SM_FAST_DAA_ROM_TM && BCHP_SUN_GISB_ARB_REVISION)
	ENT(BCHP_SUN_GISB_ARB_REVISION, BCHP_SM_FAST_DAA_ROM_TM, "SYS_CTRL..."),
#endif
#if (BCHP_SUN_L2_PCI_MASK_CLEAR && BCHP_SUN_L2_CPU_STATUS)
	ENT(BCHP_SUN_L2_CPU_STATUS, BCHP_SUN_L2_PCI_MASK_CLEAR, "SUN_L2_..."),
#endif
#if (BCHP_VICH_2_RESERVED_END && BCHP_HEVD_OL_CPU_REGS_0_HST2CPU_MBX)
	ENT(BCHP_HEVD_OL_CPU_REGS_0_HST2CPU_MBX, BCHP_VICH_2_RESERVED_END, "AVD..."),
#endif
#if (BCHP_V3D_DBG_ERRSTAT && BCHP_V3D_CTL_IDENT0)
	ENT(BCHP_V3D_CTL_IDENT0, BCHP_V3D_DBG_ERRSTAT, "V3D..."),
#endif
#if (BCHP_AVS_ROSC_THRESHOLD_2_INTERRUPT_STATUS_THRESHOLD2_FAULTY_SENSOR && BCHP_AVS_HW_MNTR_SW_CONTROLS)
	ENT(BCHP_AVS_HW_MNTR_SW_CONTROLS, BCHP_AVS_ROSC_THRESHOLD_2_INTERRUPT_STATUS_THRESHOLD2_FAULTY_SENSOR, "AVS..."),
#endif
#if (BCHP_UPG_UART_DMA_RX_REQ_SEL && BCHP_IRB_BLAST_NUMSEQ)
	ENT(BCHP_IRB_BLAST_NUMSEQ, BCHP_UPG_UART_DMA_RX_REQ_SEL, "UPG..."),
#endif
#if (BCHP_WKTMR_PRESCALER_VAL && BCHP_LDK_KEYROW32)
	ENT(BCHP_LDK_KEYROW32, BCHP_WKTMR_PRESCALER_VAL, "AON UPG..."),
#endif
#if (BCHP_RAAGA_DSP_MEM_SUBSYSTEM_MEMSUB_ERROR_CLEAR  && BCHP_RAAGA_DSP_MISC_REVISION)
	ENT(BCHP_RAAGA_DSP_MISC_REVISION, BCHP_RAAGA_DSP_MEM_SUBSYSTEM_MEMSUB_ERROR_CLEAR ,"RAAGA..."),
#endif
#if (BCHP_RAAGA_DSP_MEM_SUBSYSTEM_1_MEMSUB_ERROR_CLEAR  && BCHP_RAAGA_DSP_MISC_1_REVISION)
	ENT(BCHP_RAAGA_DSP_MISC_1_REVISION, BCHP_RAAGA_DSP_MEM_SUBSYSTEM_1_MEMSUB_ERROR_CLEAR ,"RAAGA_1..."),
#endif
#if (BCHP_VICE2_ARCSS_MISC_0_MISC_CTRL && BCHP_VICE2_CME_0_0_FW_CONTROL)
	ENT(BCHP_VICE2_CME_0_0_FW_CONTROL, BCHP_VICE2_ARCSS_MISC_0_MISC_CTRL, "VICE2_0..."),
#endif
#if (BCHP_VICE2_ARCSS_MISC_1_MISC_CTRL && BCHP_VICE2_CME_0_1_FW_CONTROL)
	ENT(BCHP_VICE2_CME_0_1_FW_CONTROL, BCHP_VICE2_ARCSS_MISC_1_MISC_CTRL, "VICE2_1..."),
#endif
#if (BCHP_AVD_CACHE_0_REG_PCACHE_END && BCHP_DECODE_RBNODE_REGS_0_RBNODE_REGS_END)
	ENT(BCHP_DECODE_RBNODE_REGS_0_RBNODE_REGS_END, BCHP_AVD_CACHE_0_REG_PCACHE_END, "AVD..."),
#endif
#if (BCHP_AVD_CACHE_1_REG_PCACHE_END && BCHP_DECODE_IP_SHIM_1_STC0_REG)
	ENT(BCHP_DECODE_IP_SHIM_1_STC0_REG, BCHP_AVD_CACHE_1_REG_PCACHE_END, "AVD SHIM and CACHE..."),
#endif
#if (BCHP_AVD_RGR_1_SW_INIT_1 && BCHP_AVD_INTR2_1_CPU_STATUS)
	ENT(BCHP_AVD_INTR2_1_CPU_STATUS, BCHP_AVD_RGR_1_SW_INIT_1, "AVD INTR2..."),
#endif
#if (BCHP_UHFR_GR_BRIDGE_SW_INIT_1 && BCHP_UHFR_RST)
	ENT(BCHP_UHFR_RST, BCHP_UHFR_GR_BRIDGE_SW_INIT_1, "UHFR..."),
#endif
#if (BCHP_VICE2_ARCSS_MISC_MISC_CTRL && BCHP_VICE2_CME_0_FW_CONTROL)
	ENT(BCHP_VICE2_CME_0_FW_CONTROL, BCHP_VICE2_ARCSS_MISC_MISC_CTRL, "VICE2..."),
#endif
#if (BCHP_HDMI_RAM_PACKET_RSVD_1 && BCHP_HDMI_CORE_REV)
	ENT(BCHP_HDMI_CORE_REV, BCHP_HDMI_RAM_PACKET_RSVD_1, "HDMI..."),
#endif
#if (BCHP_CLK_SCRATCH && BCHP_CLK_REVISION)
	ENT(BCHP_CLK_REVISION, BCHP_CLK_SCRATCH, "BCHP_CLK_..."),
#endif
	
	{0, 0, "??"}
};

static uint32_t IdentifyRegister(uint32_t reg, unsigned char* name, uint32_t namelen)
{
    uint32_t result = 9999;
    uint32_t i;
    uint32_t listlen = sizeof(RegisterNames)/sizeof(*RegisterNames);

    for (i = 0; i < listlen; i++)
    {
        if (RegisterNames[i].start <= reg && reg <= RegisterNames[i].end)
        {
            result = i;
            break;
        }
    }

    /* Point to the '??' */
    if (result == 9999)
        result = listlen - 1;

    strncpy((char *)name, RegisterNames[result].desc, namelen);

    return result;
}

static void DumpRegs(char *pchFileName)
{
    uint32_t    i;
    FILE*       fp;
    uint32_t    lastRegisterNameIndex = 9999;
    uint32_t    registerNameIndex;
    unsigned char registerName[100];

    if(g_RegDataCount == 0)
    {
        BDBG_ERR(("\n" "***************************************************************************"));
        BDBG_ERR(("Error ... Error ... Error ! " "\n" "Register Dump is empty !!! " ));
        BDBG_ERR(("Looks like you have not chosen the instrumented version of breg_mem.c"));
        BDBG_ERR(("Please pickup the instrumented version from the SPLASH_Devel branch"));
        BDBG_ERR(("***************************************************************************"));
        BDBG_ERR((""));
        return;
    }
    BDBG_MSG(("Dumping register settings: %d entries to %s", g_RegDataCount, pchFileName));

    /* open file for register dump */
    fp = fopen(pchFileName, "w");

    if (fp != NULL)
    {
        /* register header */
        fprintf(fp,"/***************************************************************************\n");
        fprintf(fp," File : splash_vdc_reg.h\n");
        fprintf(fp," Date  : %s\n", __DATE__ " " __TIME__);
        fprintf(fp," Description : Register list for splash BVN configuration\n");
        fprintf(fp," ***************************************************************************/\n\n");
        fprintf(fp,  "#include \"splash_magnum.h\"\n\n");
        fprintf(fp,  "uint32_t g_aulReg[] = \n{\n");

        for (i = 0; i < g_RegDataCount; i++)
        {
            registerNameIndex = IdentifyRegister(g_RegData[i].reg, registerName, sizeof(registerName));

            if (lastRegisterNameIndex == 9999 || lastRegisterNameIndex != registerNameIndex)
            {
                fprintf(fp, "\n");
                fprintf(fp, "\t0x%08x,   0x%08x, /* %s */\n", g_RegData[i].reg, g_RegData[i].val, registerName);
            }
            else
            {
                fprintf(fp, "\t0x%08x,   0x%08x,\n", g_RegData[i].reg, g_RegData[i].val);
            }

            lastRegisterNameIndex = registerNameIndex;
        }

        /* close */
        /* tail */
        fprintf(fp, "};\n\n\n");
        fclose(fp);
    }
    else
    {
        BDBG_ERR(("FAILED TO OPEN %s FOR WRITING", pchFileName));
    }
}


static int GetArrayIndex(BRDC_Trigger eTrigger)
{
    switch(eTrigger)
    {
    case BRDC_Trigger_eVec0Trig0:
        return 0;
    case BRDC_Trigger_eVec0Trig1:
        return 1;
    case BRDC_Trigger_eVec1Trig0:
        return 2;
    case BRDC_Trigger_eVec1Trig1:
        return 3;

    case BRDC_Trigger_eVdec0Trig0:
        return 4;
    case BRDC_Trigger_eVdec0Trig1:
        return 5;

    case BRDC_Trigger_eDtgTrig0:
        return 6;
    case BRDC_Trigger_eDtgTrig1:
        return 7;

    case BRDC_Trigger_eCmp_0Trig0:
        return 8;
    case BRDC_Trigger_eCmp_0Trig1:
        return 9;
    case BRDC_Trigger_eCmp_1Trig0:
        return 10;
    case BRDC_Trigger_eCmp_1Trig1:
        return 11;

    default:
        /* printf("Don't understand this trigger %d returning -1\n", eTrigger); */
        return -1;
    }
}


static bool isTrigger(BRDC_Trigger eTrigger)
{
    switch(eTrigger)
        {
        case BRDC_Trigger_eVec0Trig1:
        case BRDC_Trigger_eVec1Trig1:
        case BRDC_Trigger_eVdec0Trig1:
        case BRDC_Trigger_eDtgTrig1:
        case BRDC_Trigger_eCmp_0Trig1:
        case BRDC_Trigger_eCmp_1Trig1:
            return(true);
			
        case BRDC_Trigger_eVec0Trig0:
        case BRDC_Trigger_eVec1Trig0:
        case BRDC_Trigger_eVdec0Trig0:
        case BRDC_Trigger_eDtgTrig0:
		case BRDC_Trigger_eCmp_0Trig0:
		case BRDC_Trigger_eCmp_1Trig0:
        default:
            return(false);
        }
}


#define REG_PATH "splash_vdc_reg.h"
#define RUL_PATH "splash_vdc_rul.h"

extern BERR_Code  splash_vdc_setup
	( BCHP_Handle         hChp,
      BREG_Handle         hReg,
      BINT_Handle         hInt,
      BTMR_Handle         hTmr,
      ModeHandles        *pState );

int splash_generate_script
   ( BCHP_Handle hChp,
     BREG_Handle hReg,
     BINT_Handle hInt,
     BREG_I2C_Handle hRegI2c )
{
    int              iErr;
    BERR_Code        eErr;
    BFMT_VideoInfo   stVideoInfo;
    BTMR_Handle      hTmr;
    BTMR_DefaultSettings stTimerSettings;
    BRDC_Settings stRDCSettings;
    int ii;

	/* error reporting */
    BDBG_SetLevel(BDBG_eErr);
    BDBG_SetModuleLevel("BRDC_DBG", BDBG_eErr);
    BDBG_SetModuleLevel("splash_setup_vdc", BDBG_eMsg);
	
    /* Init globals */
    for(ii=0;ii<MAX_NUM_TRIGGERS;ii++)
        g_bFirstBottom[ii] = true;
	
	for (ii=0; ii<SPLASH_NUM_DISPLAY; ii++)
	{
		SplashSurface *pSurf = g_stModeHandles.disp[ii].pSurf;

		if (pSurf->ulWidth == 0)
		{
			TestError( BFMT_GetVideoFormatInfo(g_stModeHandles.disp[ii].eDispFmt, &stVideoInfo),
				"ERROR:BFMT_GetVideoFormatInfo" );
			pSurf->ulWidth = stVideoInfo.ulWidth;
			pSurf->ulHeight = stVideoInfo.ulHeight;
		}
	}
	
    BTMR_GetDefaultSettings(&stTimerSettings);
    iErr = BTMR_Open(&hTmr, hChp, hReg, hInt, &stTimerSettings);
    if ( iErr )
    {
        BDBG_ERR(("Bad TMR return!"));
        return iErr;
    }

    /* Configure anything platform-specific */
    TestError( ConfigPlatform(hReg), "ERROR: ConfigPlatform");

    for (ii=0; ii<MAX_NUM_TRIGGERS; ii++)
    {
        g_asTriggerMap[ii].SlotNum = -1;
        g_asTriggerMap[ii].TriggerHwNum = -1;
    }

    /* Init I2C Reg handle */
    g_stModeHandles.hRegI2c = hRegI2c;

    /* open Register DMA */
    stRDCSettings.pfnSlot_SetList_Intercept_isr = APP_BRDC_Slot_SetList_isr;
    stRDCSettings.pfnSlot_Execute_Intercept_isr = APP_BRDC_Slot_Execute_isr;
    stRDCSettings.pfnSlot_ExecuteOnTrigger_Intercept_isr =
        APP_BRDC_Slot_ExecuteOnTrigger_isr;
    TestError( BRDC_Open(&(g_stModeHandles.hRdc), hChp, hReg, g_stModeHandles.hRulMem, &stRDCSettings),
        "ERROR: BRDC_Open" );

    /* initialize static storage */
    InitializeStorage(g_stModeHandles.hRdc);

    /* setup VDC mode */
    BDBG_MSG(("Initializing display and surface"));
    TestError( splash_vdc_setup(
                hChp,
                hReg,
                hInt,
                hTmr,
                &g_stModeHandles),
            "ERROR: splash_vdc_setup" );

    /* ensure that all lists have been created */
    BDBG_WRN(("System is settling down. Please wait..."));
	for(ii=0;ii<5;ii++)
	{
		BKNI_Sleep(1000);
	}

    /* dump out all rul lists */
    DumpLists(RUL_PATH, &g_stModeHandles);

	/* dump out all reg writes */
    DumpRegs(REG_PATH);

    printf("Press any key to continue .....\n");
    getchar();
    /* close VDC */
	#ifndef BVDC_FOR_BOOTUPDATER
    TestError( close_mode(&g_stModeHandles),  "ERROR: close_mode" );
    UnInitializeStorage();
    BRDC_Close(g_stModeHandles.hRdc);
    BTMR_Close(hTmr);
	#endif /* #ifndef BVDC_FOR_BOOTUPDATER */
done:
    return 0;
}

/* used in BREG_Write32 when macro BREG_CAPTURE is defined */
void APP_BREG_Write32
(
    BREG_Handle  hReg,
    uint32_t     ulReg,
    uint32_t     ulValue
)
{
    BSTD_UNUSED(hReg);

    /* do we want to dump out the register? */
    /* Ignore all writes for interrupts */
    /* Ignore all writes for XPT */
    /* Ignore all writes to register DMA */	
#ifdef BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_ext_sys_sw_init_SHIFT 
    if(!BSPLASH_REGDUMP_EXCLUDE(ulReg) &&
       !(ulReg == BCHP_SUN_TOP_CTRL_SW_INIT_0_SET && ulValue == BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_ext_sys_sw_init_MASK) &&
       !(ulReg ==  BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR && ulValue == BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_ext_sys_sw_init_MASK))
#elif defined(BCHP_SUN_TOP_CTRL_SW_RESET)
    if(!BSPLASH_REGDUMP_EXCLUDE(ulReg) &&
       !(ulReg == BCHP_SUN_TOP_CTRL_SW_RESET && ulValue == BCHP_SUN_TOP_CTRL_SW_RESET_ext_sys_reset_1shot_MASK))
#else
#error "Port required for splashgen"
#endif
    {
        /*printf("reg count %d x%08x,   0x%08x \n", g_RegDataCount, ulReg, ulValue);*/
        /* dump register */
        if (g_RegDataCount < MAX_REG_ENTRIES)
        {
            g_RegData[g_RegDataCount].reg = ulReg;
            g_RegData[g_RegDataCount].val = ulValue;
            g_RegDataCount++;
        }

    }
}

/* called in BRDC_P_Slot_SetList_isr and BRDC_P_Slot_SetListDual_isr */
static BERR_Code APP_BRDC_Slot_SetList_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_List_Handle hList,
    BRDC_Trigger     eTrigger
)
{
    int i, j;
    bool bMatch = false;
    uint32_t ulNumEntries;
    uint32_t *pulStartAddress;
    uint32_t ulNumEntriesStored;
    uint32_t *pulStartAddressStored;
    BRDC_List_Handle  hNewList;
    int iTriggerIndex;

    BSTD_UNUSED(hSlot);

    /* get information on new list */
    BRDC_List_GetNumEntries_isr(hList, &ulNumEntries);
    pulStartAddress = BRDC_List_GetStartAddress_isr(hList);

    /* search for handle in existing list */
    for(i=g_iListCount-1; i>=0; --i)
    {
        /* match? */
        if(g_aHandleList[i] == hList)
        {
            /* match found */
            break;
        }
    }

    /* found? */
    if(i>=0)
    {
        /* get information on stored list */
        BRDC_List_GetNumEntries_isr(g_aRulList[i], &ulNumEntriesStored);
        pulStartAddressStored = BRDC_List_GetStartAddress_isr(g_aRulList[i]);

        /* number of elements the same? */
        if(ulNumEntriesStored == ulNumEntries)
        {
            /* compare elements */
            bMatch = true;
            for(j=0; j<(int)ulNumEntries; j++)
            {
                /* element different? */
                if(*(pulStartAddress + j) != *(pulStartAddressStored + j))
                {
                    if(true) /* !BRDC_IsScratchReg(g_stModeHandles.hRdc, *(pulStartAddress+j-1)) ) */
                    {
                        BDBG_MSG(("SlotList: Cleared %d [0x%8.8x] (element different: #%d of %d)", i, (uint32_t)hList, j+1, ulNumEntries));
                        bMatch = false;
                        break;
                    }
                    #if 0
                    else
                    {
                        printf("************ Mismatch in Scratch pad area - ignoring (%d)!!\n", j);
                    }
                    #endif
                }
            }
        }
        else
        {
            BDBG_MSG(("SlotList: Cleared %d [0x%8.8x] (sizes different: %d vs. %d)", i, (uint32_t)hList, ulNumEntriesStored, ulNumEntries));
        }

        /* match? */
        if(bMatch)
        {
            /* no need to update list */
            /* printf("SetList_isr: OLD LIST\n"); */
            goto done;

        /* not a match */
        } else
        {
            /* clear out the handle since this is not active any longer */
            /* printf("Clearing...\n"); */
            g_aHandleList[i] = NULL;
        }
    }

    /* new list -- no more storage? */
    /* printf("SetList_isr: NEW LIST... (%x, %d)\n", (uint32_t)hList, ulNumEntries);*/
    if((g_iListCount == MAX_RUL_COUNT) ||
        (ulNumEntries > MAX_RUL_ENTRIES))
    {
        /* cannot store */
        BDBG_ERR(("ERROR: %d %d -No pre-allocation available for list.",g_iListCount,ulNumEntries));
    }

    /* copy into new list */
    g_aHandleList[g_iListCount] = hList;
    hNewList = g_aRulList[g_iListCount];
    BRDC_List_SetNumEntries_isr(hNewList, ulNumEntries);
    pulStartAddressStored = BRDC_List_GetStartAddress_isr(hNewList);
    for(j=0; j<(int)ulNumEntries; j++)
    {
        /* copy */
        *(pulStartAddressStored + j) = *(pulStartAddress + j);
    }

    /* store as top list for that trigger */
    iTriggerIndex = GetArrayIndex(eTrigger);

    if(-1 == iTriggerIndex) return BERR_SUCCESS;

    g_aTopLists[iTriggerIndex] = g_iListCount;

    /* store list? (only after a force execute) */
    if(isTrigger(eTrigger) || g_bExecuteList[iTriggerIndex])
    {
        /* first bottom field? */
        if(g_bFirstBottom[iTriggerIndex] && isTrigger(eTrigger))
        {
            /* ignore first bottom field */
            g_bFirstBottom[iTriggerIndex] = false;

        /* not the first bottom */
        } else
        {
            /* add to list */
            g_aListOrder[iTriggerIndex][g_aListOrderCount[iTriggerIndex]++] =
                g_iListCount;
            BDBG_MSG(("SlotList: Added   %d [0x%8.8x] TriggerIndex=%d", g_iListCount, (uint32_t)hList, iTriggerIndex));
        }
    }

    /* Update Trigger Map */
    {
        BRDC_SlotId slotId;
        const BRDC_TrigInfo *trigInfo = BRDC_Trigger_GetInfo(g_stModeHandles.hRdc, eTrigger);
        BRDC_Slot_GetId(hSlot, &slotId);
        BDBG_MSG(("Slot number %d, Trigger enum %d Trigger HW num %d TriggerIndex %d", slotId, eTrigger, trigInfo->ulTrigVal, iTriggerIndex));
        g_asTriggerMap[iTriggerIndex].SlotNum = slotId;
        g_asTriggerMap[iTriggerIndex].TriggerHwNum = trigInfo->ulTrigVal;
    }

    /* added new list element */
    g_iListCount++;

done:
    return BERR_SUCCESS;
}

/* called in BRDC_Slot_ExecuteOnTrigger_isr */
static BERR_Code APP_BRDC_Slot_ExecuteOnTrigger_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_Trigger     eRDCTrigger,
    bool             bRecurring
)
{
    BRDC_List_Handle hList;
    uint32_t ulNumEntries;

    BSTD_UNUSED(bRecurring);

    /* get list that was executed without trigger
       (only one should be this way) */
    BRDC_Slot_GetList_isr(hSlot, &hList);
    BRDC_List_GetNumEntries_isr(hList, &ulNumEntries);
    BDBG_MSG(("ExecuteOnTrigger_isr(%d): %08x %d", eRDCTrigger, (uint32_t)hList, ulNumEntries));

    switch(eRDCTrigger)
    {
    case BRDC_Trigger_eCap0Trig0:
        BDBG_MSG(("BRDC_Trigger_eCap0Trig0"));
        break;
    case BRDC_Trigger_eCap0Trig1:
        BDBG_MSG(("BRDC_Trigger_eCap0Trig1"));
        break;
    case BRDC_Trigger_eVec0Trig0:
        BDBG_MSG(("BRDC_Trigger_eVec0Trig0"));
        break;
    case BRDC_Trigger_eVec0Trig1:
        BDBG_MSG(("BRDC_Trigger_eVec0Trig1"));
        break;
		
    case BRDC_Trigger_eVec1Trig0:
        BDBG_MSG(("BRDC_Trigger_eVec1Trig0"));
        break;
    case BRDC_Trigger_eVec1Trig1:
        BDBG_MSG(("BRDC_Trigger_eVec1Trig1"));
        break;

    case BRDC_Trigger_eCmp_0Trig0:
        BDBG_MSG(("BRDC_Trigger_eCmp_0Trig0"));
        break;
    case BRDC_Trigger_eCmp_0Trig1:
        BDBG_MSG(("BRDC_Trigger_eCmp_0Trig1"));
        break;
    case BRDC_Trigger_eCmp_1Trig0:
        BDBG_MSG(("BRDC_Trigger_eCmp_1Trig0"));
        break;
    case BRDC_Trigger_eCmp_1Trig1:
        BDBG_MSG(("BRDC_Trigger_eCmp_1Trig1"));
        break;

    default:
        BDBG_MSG(("(%d)Trigger unknown" , eRDCTrigger));
        break;
    }

    return BERR_SUCCESS;
}
/* called in BRDC_Slot_Execute_isr */
static BERR_Code APP_BRDC_Slot_Execute_isr
(
    BRDC_Slot_Handle hSlot,
    BRDC_Trigger     eTrigger
)
{
    BRDC_List_Handle hList;
    uint32_t ulNumEntries;
    int iTriggerIndex;

    /* printf("Execute_isr\n"); */

    /* get list that was executed without trigger
       (only one should be this way) */
    BRDC_Slot_GetList_isr(hSlot, &hList);
    BRDC_List_GetNumEntries_isr(hList, &ulNumEntries);
    BDBG_MSG(("Execute_isr: triger(%d): %08x %d", eTrigger, (uint32_t)hList, ulNumEntries));
	
    /* which trigger fired? */
    iTriggerIndex = GetArrayIndex(eTrigger);
    if(-1 == iTriggerIndex) return BERR_SUCCESS;

    /* add list that was executed to start of list */
    g_aListOrder[iTriggerIndex][0] = g_aTopLists[iTriggerIndex];
    g_aListOrderCount[iTriggerIndex]++;

    /* a list has been executed. At this point, keep all new lists */
    g_bExecuteList[iTriggerIndex] = true;

    return BERR_SUCCESS;
}


/* End of file */

