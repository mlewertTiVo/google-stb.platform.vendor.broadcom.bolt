/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_script_load.c $
 * $brcm_Revision: 27 $
 * $brcm_Date: 4/12/13 2:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/splash_script_load.c $
 * 
 * 27   4/12/13 2:06p syang
 * SW7445-213: change to use cached addr for surface and RULs
 * 
 * 26   4/1/13 5:48p syang
 * SW7435-676: make it easy to support dynamic script selecttion in CFE at
 * run time
 * 
 * 25   3/30/13 12:21a syang
 * SW7435-676: rm "include <stdio.h>", it cause CFE compile problem
 * 
 * 24   3/29/13 2:39p syang
 * SW7435-676: print out surface offset msg
 * 
 * 23   3/29/13 11:41a syang
 * SW7435-676: deep refactor for memc, surface and display number
 * scalabilty; flexibility for diff configure combination; and easy
 * adding of new chips
 * 
 * 22   10/31/12 7:02p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 19   9/18/12 2:10p jessem
 * SW7425-3872: Removed BSPLASH_RDC_SCRATCH_DISP1_REG1.
 *
 * 18   9/10/12 4:40p jessem
 * SW7425-3872: Removed hMem1 use with 7425.
 *
 * 17   4/19/12 2:53p mward
 * SW7435-114:  Add support for 7435.
 *
 * 16   4/12/12 5:04p randyjew
 * SWCFE-769: Memc0 only support
 *
 * 15   4/10/12 2:15p jessem
 * SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
 * correct heap to use.
 *
 * 14   8/22/11 5:14p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 *
 * 13   6/27/11 11:34a katrep
 * SW7231-220:add support for 7231 &newer 40nm chips
 *
 * 12   2/24/11 5:09p gmohile
 * SW7408-239 : Add 7408 splash support
 *
 * 11   10/14/10 5:22p jkim
 * SWCFE-386: remove compiler warning.
 *
 * 10   11/30/09 3:10p jrubio
 * SW7335-627: fix for non-7420 chips
 *
 * 9   9/24/09 11:44p nickh
 * SW7420-351: Modify load script to add MEMC1 for 7420 support
 *
 * 8   10/18/07 2:37p shyam
 * PR 30741 : Add error check for empty reg dump
 *
 * 6   9/28/07 2:39p shyam
 * PR 30741 : Avoid brdc.h inclusion in script loader
 *
 * 5   9/27/07 7:41p shyam
 * PR 30741 : Bug with code that filters out baadfood ruls
 *
 * 4   8/9/07 3:52p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 3   7/18/07 12:39p shyam
 * PR 30741 : Add support for 480p on HDMI and NTSC on Composite
 *
 * 2   6/28/07 5:06p shyam
 * PR 30741 : syncup with cfe version
 *
 * 1   5/14/07 7:15p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 ***************************************************************************/
#include "splash_script_load.h"

#include "splash_magnum.h"

#include "bchp_rdc.h"
#include "bchp_common.h"
/* #include "lib_printf.h" */

#include "splash_script_load.h"

/* pre-generated register and RUL header files from splashgen */
#include "splash_vdc_rul.h"
#include "splash_vdc_reg.h"

BDBG_MODULE(splash_script_load);

#define BRDC_REGISTER(reg)      ((reg) + BCHP_PHYSICAL_OFFSET)


uint32_t  g_ulNumMem = BSPLASH_NUM_MEM;
uint32_t  g_ulNumSurface = BSPLASH_NUM_SURFACE;
uint32_t  g_ulNumDisplay = BSPLASH_NUM_DISPLAY;

uint32_t *g_pulReg = &g_aulReg[0];
uint32_t g_ulNumReg = (sizeof(g_aulReg)/(2*sizeof(uint32_t)));
		
/**************************
 * This function takes a set of RULs for a given slot (say all the RULS for top
 * field or all for bottom field), copies them into memory, adds a command
 * to the first N-1 RULs so that on completion the next RUL is made active,
 * and puts the first RUL in the slot ready to be executed.
 *
 * Instead of having a function do this, this work can be done ahead of time
 * as long as the user knows exactly where each RUL is located in memory */
static void InitializeSlot
(
    BREG_Handle       hRegister,
    BMEM_Handle       hHeap,
    int               iSlotIndex,
    uint32_t          ulTriggerSelect,
    uint32_t         *pListCount,
    uint32_t         *pulList,
    int               iListCount
)
{
    int              i;
    int              iNumEntries, iPrvNumEntries = 0, iCount;
    uint32_t         ulAddrOffset;
    uint32_t         ulValue;
    uint32_t        *pulCurr;
    uint32_t         ulSlotOffset = 4 * iSlotIndex * sizeof(uint32_t);
    int              iRUL;
	uint32_t        *apulAlloced[30];
	uint32_t        *apulCached[30];
    BERR_Code        rc;

    BDBG_MSG(("InitializeSlot : iSlotIndex:%d ulTriggerSelect:%d pListCount:%p pulList:%p iListCount:%d \n",
			  iSlotIndex,   ulTriggerSelect,(void *)pListCount,(void *)pulList, iListCount));
    /************************
     * We have to place the dumped RULs into allocated memory. Instead a user could
     * have pre-determined memory locations for each of the RULs instead of directly
     * allocating them in an application. In that case, this entire loop is pre-defined.
     */
    iRUL = 0;
    for(i=0; i<iListCount; i++)
    {
        /* how many entries for this RUL? */
        iNumEntries = pListCount[i+1] - pListCount[i];

        /* RUL passed inspection */
        /* allocate aligned memory (add 6 more elements for extra entries -- see below) */
        apulAlloced[iRUL] = (uint32_t *) BMEM_AllocAligned(hHeap,
            sizeof(uint32_t) * (iNumEntries + 6), 5, 0);
		BMEM_Heap_ConvertAddressToCached(hHeap, (void *)apulAlloced[iRUL], (void **)&apulCached[iRUL]);

        /* copy list into memory */
        BKNI_Memcpy(apulCached[iRUL], pulList + pListCount[i],
            sizeof(uint32_t) * iNumEntries);

		/* there is previous RUL in list? */
        if(iRUL > 0)
        {
            /* modify previous RUL (end of RUL) to install this new RUL when finished. This
             * can also be done ahead of time if the memory address is fixed. */

            /* get pointer to the current end of the list */
            pulCurr = apulCached[iRUL-1] + iPrvNumEntries;

            BDBG_MSG(("********* pulCurr = %p, apulAlloced[%d] = %p i=%d, iPrvNumEntries=%d\n", pulCurr, iRUL-1, apulAlloced[iRUL-1],i, iPrvNumEntries));

            /* write command to install new RUL address into slot after the
               previous list is complete  */
            rc = BMEM_ConvertAddressToOffset(hHeap,
                (void*)apulAlloced[iRUL], &ulAddrOffset);
            if(rc)
                BDBG_ERR(("Error Converting %d", rc));
            *pulCurr++ = BCHP_FIELD_ENUM(RDC_RUL, opcode, REG_WRITE_IMM) ;
            *pulCurr++ = BRDC_REGISTER(BCHP_RDC_desc_0_addr) + ulSlotOffset;
            *pulCurr++ =
                BCHP_FIELD_DATA(RDC_desc_0_addr, addr, ulAddrOffset);

            /* are we going to add more entries for this list (not the last list)? */
            iCount = (i != iListCount - 1) ? (iNumEntries+6) : iNumEntries ;

            /* write new count into slot */
            *pulCurr++ = BCHP_FIELD_ENUM(RDC_RUL, opcode, REG_WRITE_IMM) ;
            *pulCurr++ = BRDC_REGISTER(BCHP_RDC_desc_0_config) + ulSlotOffset;
            *pulCurr++ =
                BCHP_FIELD_DATA(RDC_desc_0_config, count,          iCount - 1) |
                BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTriggerSelect) |
				#if defined(BCHP_RDC_desc_0_config_reserved0_SHIFT)
                BCHP_FIELD_DATA(RDC_desc_0_config, reserved0,      0)               |
				#elif defined(BCHP_RDC_desc_0_config_segmented_SHIFT)
				BCHP_FIELD_DATA(RDC_desc_0_config, segmented,      0)               |
				#endif
                BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         1)               |
                BCHP_FIELD_DATA(RDC_desc_0_config, enable,         1)               |
                BCHP_FIELD_DATA(RDC_desc_0_config, done,           1);
			
			/* flush previous RUL: we just appended cmd to it to link the current RUL start addr */
			BMEM_Heap_FlushCache(hHeap, apulCached[iRUL-1], sizeof(uint32_t)*(iPrvNumEntries+6));
        }

        iRUL++;
        iPrvNumEntries = iNumEntries ;
    }
	
	/* flush the last RUL */
	BMEM_Heap_FlushCache(hHeap, apulCached[iRUL-1], sizeof(uint32_t)*(iPrvNumEntries+6));

    /*****************************
     * Setup the initial RDC slot with the first list.
     * Again if the memory was fixed ahead of time then
     * the calculations below would be fixed and unchanging.
     */

    /* configure initial slot */
    iNumEntries = pListCount[1] - pListCount[0];
    if(iListCount > 1)
    {
        /* we have 6 more entries on this list */
        iNumEntries += 6;
    }

    ulValue =
        BCHP_FIELD_DATA(RDC_desc_0_config, count,          iNumEntries - 1) |
        BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTriggerSelect) |
		#if defined(BCHP_RDC_desc_0_config_reserved0_SHIFT)
		BCHP_FIELD_DATA(RDC_desc_0_config, reserved0,      0)               |
		#elif defined(BCHP_RDC_desc_0_config_segmented_SHIFT)
		BCHP_FIELD_DATA(RDC_desc_0_config, segmented,      0)               |
		#endif
        BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         1)               |
        BCHP_FIELD_DATA(RDC_desc_0_config, enable,         1)               |
        BCHP_FIELD_DATA(RDC_desc_0_config, done,           1);
    BREG_Write32(hRegister, BCHP_RDC_desc_0_config + ulSlotOffset,
        ulValue);

    /* put first entry into slot */
    rc = BMEM_ConvertAddressToOffset(hHeap,
        (void*)apulAlloced[0], &ulAddrOffset);
    if(rc)
        BDBG_ERR(("Error Converting %d", rc));

    BREG_Write32(hRegister, BCHP_RDC_desc_0_addr + ulSlotOffset,
        ulAddrOffset);
}

void splash_bvn_init(BREG_Handle hRegister, SplashBufInfo *pSplashBufInfo)
{
    int  ii;
    int  iTriggerIndex;
	int  iSurIdx;
	
    if( 0 == g_ulNumReg )
    {
        BDBG_ERR(("\n\n\n" "***************************************************************************"
                  "\n" "Error ... Error ... Error ! " "\n" "Register Dump is empty !!! "
                  "\n" "Looks like you have not chosen the instrumented version of REG basemodule"
                  "\n" "Please rebuild/rerun the splash script generator with instrumented BREG "
                  "\n" "module"
                  "\n" "***************************************************************************"
                  "\n\n\n")) ;
        return ;
    }
    /* Dumped registers */
    for(ii=0; ii<(int)(g_ulNumReg); ++ii)
    {
        /* Write dumped register value */
        BREG_Write32(hRegister, *(g_pulReg + 2*ii), *(g_pulReg + 2*ii+1));
    }

	/* write our surface addr into RDC scratch registers,
	 * note that the RUL has been built to pick them up for gfx display */
	for (ii=0; ii<BSPLASH_NUM_DISPLAY; ii++)
	{
		iSurIdx = g_SplashDisplayInfo[ii].iSurIdx;
		BREG_Write32(hRegister, g_SplashDisplayInfo[ii].ulRdcScratchReg0,
			pSplashBufInfo->aulSurfaceBufOffset[iSurIdx]);
		if (g_SplashDisplayInfo[ii].ulRdcScratchReg1)
			BREG_Write32(hRegister, g_SplashDisplayInfo[ii].ulRdcScratchReg1,
				pSplashBufInfo->aulSurfaceBufOffset[iSurIdx]);
		BDBG_MSG(("** set surface offset 0x%8.8x for display %d\n",pSplashBufInfo->aulSurfaceBufOffset[iSurIdx], ii));
	}
		
    BDBG_MSG(("***** Number of triggers = %d \n", sizeof(g_aTriggerMap)/sizeof(struct stTriggerMap) )) ;
    for( iTriggerIndex = 0; iTriggerIndex < (int)(sizeof(g_aTriggerMap)/sizeof(struct stTriggerMap)); iTriggerIndex++)
    {
        if(g_aTriggerMap[iTriggerIndex].TriggerHwNum != -1)
        {
            BDBG_MSG(("Slot number %d, Trigger number %d aList%d[]\n",
                g_aTriggerMap[iTriggerIndex].SlotNum, g_aTriggerMap[iTriggerIndex].TriggerHwNum, iTriggerIndex));
            /* Initialize slot for RUL list n */
            InitializeSlot(
                hRegister,
                pSplashBufInfo->hRulMem,
                g_aTriggerMap[iTriggerIndex].SlotNum,
                g_aTriggerMap[iTriggerIndex].TriggerHwNum,
                g_aTriggerMap[iTriggerIndex].aListCountArray,
                g_aTriggerMap[iTriggerIndex].aListArray,
                g_aTriggerMap[iTriggerIndex].ListCountMaxIndex);
        }
    }

    for( iTriggerIndex = 0; iTriggerIndex < (int)(sizeof(g_aTriggerMap)/sizeof(struct stTriggerMap)); iTriggerIndex++)
    {
        if(g_aTriggerMap[iTriggerIndex].TriggerHwNum != -1)
        {
            /* Force Top slot to start engine for each display */
            if(0 == (g_aTriggerMap[iTriggerIndex].SlotNum%2))
            {
                uint32_t ulSlotOffset = 4 * g_aTriggerMap[iTriggerIndex].SlotNum * sizeof(uint32_t);

                BDBG_MSG(("Writing immediate to %08x", BCHP_RDC_desc_0_immediate+ulSlotOffset));
                BREG_Write32(hRegister, BCHP_RDC_desc_0_immediate+ulSlotOffset, 1);
            }
        }
    }
}

/* End of File */
