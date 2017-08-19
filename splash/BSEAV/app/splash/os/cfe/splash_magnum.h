/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_magnum.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 12/15/11 11:31a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/os/cfe/splash_magnum.h $
 * 
 * 7   12/15/11 11:31a syang
 * SW7572-623: add  BPXL_eA8_Y8_Cb8_Cr8 dst pxl format
 * 
 * 6   4/6/11 6:10p ltokuda
 * SWCFE-386: remove compiler warning.
 * 
 * 5   10/14/10 5:20p jkim
 * SWCFE-386: remove compiler warning.
 * 
 * 4   5/19/10 1:46p jkim
 * SWCFE-312: modify not to use definitions from bpxl.h
 * 
 * 4   5/19/10 1:43p jkim
 * SWCFE-312: modify not to use definitions from bpxl.h
 *
 * 2   8/9/07 3:51p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 1   7/24/07 6:47p shyam
 * PR 30741 : Add magnum portability layer for CFE environment
 *
 ***************************************************************************/

#ifndef __SPLASH_MAGNUM_H__
#define __SPLASH_MAGNUM_H__

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_malloc.h"
#include "lib_physio.h"
#include "bchp_common.h"
#include "arch.h"

/* This header file is a extremely simplified version of the magnum stack */
/* Handles for example are either not required at all and are simplfied to nothing */
typedef void * BREG_Handle ;
typedef void * BMEM_Handle ;

#define BREG_Write32(RegHandle, reg, data) (*((volatile uint32_t *)((uintptr_t)REG_ADDR(reg)))=data)
#define BREG_Read32(RegHandle, reg) (*((volatile uint32_t *)((uintptr_t)REG_ADDR(reg))))

#define BREG_Write64(RegHandle, reg, data) \
	(*((volatile uint64_t *)((uintptr_t)REG_ADDR(reg)))=data)
#define BREG_Read64(RegHandle, reg) \
	(*((volatile uint64_t *)((uintptr_t)REG_ADDR(reg))))

/* Private pixel format types */
#define BPXL_P_ALPHA           0x01000000
#define BPXL_P_COLOR           0x02000000
#define BPXL_P_RGB             0x04000000
#define BPXL_P_YCbCr           0x08000000
#define BPXL_P_PALETTE         0x10000000
#define BPXL_P_SPECIAL         0x20000000
#define BPXL_P_WINDOW          0x40000000
#define BPXL_P_TYPE_MASK       0x7F000000

/* Private pixel component information */
#define BPXL_P_COMP_LOC_SHIFT  16

/* Private pixel component locations */
#define BPXL_P_LOC_A(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 3 * 2))
#define BPXL_P_LOC_R(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 2 * 2))
#define BPXL_P_LOC_G(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 1 * 2))
#define BPXL_P_LOC_B(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))
#define BPXL_P_LOC_Y(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 2 * 2))
#define BPXL_P_LOC_Cb(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 1 * 2))
#define BPXL_P_LOC_Cr(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))
#define BPXL_P_LOC_ARGB        (BPXL_P_LOC_A(3)  | BPXL_P_LOC_R(2)  | BPXL_P_LOC_G(1)  | BPXL_P_LOC_B(0))
#define BPXL_P_LOC_AYCbCr      (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(2)  | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(0))
#define BPXL_P_YCbCr           0x08000000

typedef enum BPXL_Format
{
	/* RGB */
	BPXL_eA8_R8_G8_B8      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x8888,

	BPXL_eR5_G6_B5         = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x0565,
	BPXL_eA8_Y8_Cb8_Cr8    = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_AYCbCr | 0x8888,

	BPXL_INVALID           = 0
} BPXL_Format;

typedef enum
{
    BFMT_VideoFmt_eNTSC = 0,                   /* 480i, NTSC-M for North America */
    BFMT_VideoFmt_eNTSC_J,                     /* 480i (Japan) */
    BFMT_VideoFmt_eNTSC_443,                   /* NTSC-443 */
    BFMT_VideoFmt_ePAL_B,                      /* Australia */
    BFMT_VideoFmt_ePAL_B1,                     /* Hungary */
    BFMT_VideoFmt_ePAL_D,                      /* China */
    BFMT_VideoFmt_ePAL_D1,                     /* Poland */
    BFMT_VideoFmt_ePAL_G,                      /* Europe */
    BFMT_VideoFmt_ePAL_H,                      /* Europe */
    BFMT_VideoFmt_ePAL_K,                      /* Europe */
    BFMT_VideoFmt_ePAL_I,                      /* U.K. */
    BFMT_VideoFmt_ePAL_M,                      /* 525-lines (Brazil) */
    BFMT_VideoFmt_ePAL_N,                      /* Jamaica, Uruguay */
    BFMT_VideoFmt_ePAL_NC,                     /* N combination (Argentina) */
    BFMT_VideoFmt_ePAL_60,                     /* 60Hz PAL */
    BFMT_VideoFmt_eSECAM_L,                    /* France */
    BFMT_VideoFmt_eSECAM_B,                    /* Middle East */
    BFMT_VideoFmt_eSECAM_G,                    /* Middle East */
    BFMT_VideoFmt_eSECAM_D,                    /* Eastern Europe */
    BFMT_VideoFmt_eSECAM_K,                    /* Eastern Europe */
    BFMT_VideoFmt_eSECAM_H,                    /* Line SECAM */
    BFMT_VideoFmt_e1080i,                      /* HD 1080i */
    BFMT_VideoFmt_e1080p,                      /* HD 1080p 60/59.94Hz, SMPTE 274M-1998 */
    BFMT_VideoFmt_e720p,                       /* HD 720p */
    BFMT_VideoFmt_e720p_60Hz_3DOU_AS,          /* HD 3D 720p */
    BFMT_VideoFmt_e720p_50Hz_3DOU_AS,          /* HD 3D 720p50 */
    BFMT_VideoFmt_e720p_30Hz_3DOU_AS,          /* HD 3D 720p30 */
    BFMT_VideoFmt_e720p_24Hz_3DOU_AS,          /* HD 3D 720p24 */
    BFMT_VideoFmt_e480p,                       /* HD 480p */
    BFMT_VideoFmt_e1080i_50Hz,                 /* HD 1080i 50Hz, 1125 sample per line, SMPTE 274M */
    BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,         /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
    BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,         /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
    BFMT_VideoFmt_e1080p_60Hz_3DOU_AS,         /* HD 1080p 60Hz, 2200 sample per line  */
    BFMT_VideoFmt_e1080p_60Hz_3DLR,            /* HD 1080p 60Hz, 4400 sample per line  */
    BFMT_VideoFmt_e1080p_24Hz,                 /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
    BFMT_VideoFmt_e1080p_25Hz,                 /* HD 1080p 25Hz, 2640 sample per line, SMPTE 274M-1998 */
    BFMT_VideoFmt_e1080p_30Hz,                 /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
    BFMT_VideoFmt_e1080p_50Hz,                 /* HD 1080p 50Hz. */
    BFMT_VideoFmt_e1080p_100Hz,                /* HD 1080p 100Hz. */
    BFMT_VideoFmt_e1080p_120Hz,                /* HD 1080p 120Hz. */
    BFMT_VideoFmt_e1250i_50Hz,                 /* HD 1250i 50Hz, another 1080i_50hz standard SMPTE 295M */
    BFMT_VideoFmt_e720p_24Hz,                  /* HD 720p 23.976/24Hz, 750 line, SMPTE 296M */
    BFMT_VideoFmt_e720p_25Hz,                  /* HD 720p 25Hz, 750 line, SMPTE 296M */
    BFMT_VideoFmt_e720p_30Hz,                  /* HD 720p 30Hz, 750 line, SMPTE 296M */
    BFMT_VideoFmt_e720p_50Hz,                  /* HD 720p 50Hz (Australia) */
    BFMT_VideoFmt_e576p_50Hz,                  /* HD 576p 50Hz (Australia) */
    BFMT_VideoFmt_e240p_60Hz,                  /* NTSC 240p */
    BFMT_VideoFmt_e288p_50Hz,                  /* PAL 288p */
    BFMT_VideoFmt_e1440x480p_60Hz,             /* CEA861B */
    BFMT_VideoFmt_e1440x576p_50Hz,             /* CEA861B */
    BFMT_VideoFmt_e3840x2160p_24Hz,            /* 3840x2160 24Hz */
    BFMT_VideoFmt_e3840x2160p_25Hz,            /* 3840x2160 25Hz */
    BFMT_VideoFmt_e3840x2160p_30Hz,            /* 3840x2160 30Hz */
    BFMT_VideoFmt_e3840x2160p_50Hz,            /* 3840x2160 50Hz */
    BFMT_VideoFmt_e3840x2160p_60Hz,            /* 3840x2160 60Hz */
    BFMT_VideoFmt_e4096x2160p_24Hz,            /* 4096x2160 24Hz */
    BFMT_VideoFmt_e4096x2160p_25Hz,            /* 4096x2160 25Hz */
    BFMT_VideoFmt_e4096x2160p_30Hz,            /* 4096x2160 30Hz */
    BFMT_VideoFmt_e4096x2160p_50Hz,            /* 4096x2160 50Hz */
    BFMT_VideoFmt_e4096x2160p_60Hz,            /* 4096x2160 60Hz */
#ifdef BFMT_LEGACY_3DTV_SUPPORT
    BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz,    /* 3548 LVDS output for legacy 3DTV support */
    BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz,    /* 3548 LVDS output for legacy 3DTV support */
#endif
    BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz,      /* 240p 60Hz 7411 custom format. */
    BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz,      /* 288p 50Hz 7411 custom format. */
    BFMT_VideoFmt_eCUSTOM_1366x768p,           /* Custom 1366x768 mode */
    BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz,      /* Custom 1366x768 50Hz mode */
    BFMT_VideoFmt_eDVI_640x480p,               /* DVI Safe mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x480p_CVT,           /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p,               /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1024x768p,              /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x768p,              /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x768p_Red,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p_50Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p,              /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p_Red,          /* DVI VESA mode for computer monitors */

    /* Added for HDMI/HDDVI input support!  VEC does not support these timing format!
     * Convention: BFMT_VideoFmt_eDVI_{av_width}x{av_height}{i/p}_{RefreshRateInHz}.
     * Eventually VEC can output all these timing formats when we get the microcodes
     * for it.  Currently there are no microcode for these yet. */
    BFMT_VideoFmt_eDVI_640x350p_60Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x350p_70Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x350p_72Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x350p_75Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x350p_85Hz,          /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_640x400p_60Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x400p_70Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x400p_72Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x400p_75Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x400p_85Hz,          /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_640x480p_66Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x480p_70Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x480p_72Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x480p_75Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_640x480p_85Hz,          /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_720x400p_60Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_720x400p_70Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_720x400p_72Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_720x400p_75Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_720x400p_85Hz,          /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_800x600p_56Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p_59Hz_Red,      /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p_70Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p_72Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p_75Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_800x600p_85Hz,          /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_1024x768p_66Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1024x768p_70Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1024x768p_72Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1024x768p_75Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1024x768p_85Hz,         /* DVI VESA mode for computer monitors */

    BFMT_VideoFmt_eDVI_1280x720p_70Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p_72Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p_75Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x720p_85Hz,         /* DVI VESA mode for computer monitors */

    /* New DVI or PC vdec input support */
    BFMT_VideoFmt_eDVI_1024x768i_87Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1152x864p_75Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x768p_75Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x768p_85Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x800p_60Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x960p_60Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x960p_85Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x1024p_60Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x1024p_69Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x1024p_75Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1280x1024p_85Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_832x624p_75Hz,          /*   MAC-16 */
    BFMT_VideoFmt_eDVI_1360x768p_60Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1366x768p_60Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1400x1050p_60Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red,    /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1400x1050p_75Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1600x1200p_60Hz,        /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red,    /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_848x480p_60Hz,          /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1064x600p_60Hz,         /* DVI VESA mode for computer monitors */
    BFMT_VideoFmt_eDVI_1440x900p_60Hz,         /* DVI VESA mode for computer monitors */

    /* SW7435-276: New format enums for 482/483 */
    BFMT_VideoFmt_e720x482_NTSC,               /* 720x482i NSTC-M for North America */
    BFMT_VideoFmt_e720x482_NTSC_J,             /* 720x482i Japan */
    BFMT_VideoFmt_e720x483p,                   /* 720x483p */

    /* statics: custom formats */
    BFMT_VideoFmt_eCustom0,         /* 59.94/60 Hz */
    BFMT_VideoFmt_eCustom1,         /* 50 Hz */

    /* dynamics: custom format */
    BFMT_VideoFmt_eCustom2,         /* defined at run time by app */

    /* Must be last */
    BFMT_VideoFmt_eMaxCount         /* Counter. Do not use! */

} BFMT_VideoFmt;


#define true 1
#define false 0

typedef unsigned int BERR_Code ;

/***************************************************************************
Summary:
	Extracts the bitfield value of a register using RDB name.
**************************************************************************/
#define BCHP_GET_FIELD_DATA(Memory,Register,Field) \
	((((Memory) & BCHP_MASK(Register,Field)) >> \
	BCHP_SHIFT(Register,Field)))

/***************************************************************************
Summary:
	Push a value into a register bitfield using RDB name.
**************************************************************************/

#define BCHP_FIELD_DATA(Register,Field,Data) \
	((Data) << BCHP_SHIFT(Register,Field))

/***************************************************************************
Summary:
	Push a  value name into a register bitfield using RDB name.
**************************************************************************/
#define BCHP_FIELD_ENUM(Register,Field,Name) \
	BCHP_FIELD_DATA(Register,Field, BCHP_##Register##_##Field##_##Name)

/*************************************************************************
Summary:
	Provide a mask for a specific register field.
**************************************************************************/
#define BCHP_MASK(Register,Field) \
	BCHP_##Register##_##Field##_MASK

/*************************************************************************
Summary:
	Provide a shift for a specific register field.
**************************************************************************/
#define BCHP_SHIFT(Register,Field) \
	BCHP_##Register##_##Field##_SHIFT


/* KNI related calls */
#define BKNI_Memcpy(d,s,c)	memcpy(d,s,c)
/* #define BMEM_AllocAligned(Heap, Size, Alignment, Boundry)	BCM_K0_TO_K1( (uint32_t)KMALLOC(Size, (1<<Alignment))) */
void *BMEM_AllocAligned
(
	BMEM_Handle       pheap,       /* Heap to allocate from */
	size_t            ulSize,      /* size in bytes of block to allocate */
	unsigned int      ucAlignBits, /* alignment for the block */
	unsigned int      Boundary     /* boundry restricting allocated value */
);

BERR_Code BMEM_ConvertAddressToOffset(BMEM_Handle heap, void* addr, uint32_t* offset) ;
uint32_t AlignAddress(
		uint32_t	ui32Address,	/* [in] size in bytes of block to allocate */
		unsigned int uiAlignBits	/* [in] alignment for the block */
		);

#define BDBG_MODULE(x)

/* #define SPLASH_DEBUG */

#define BDBG_ERR(x) xprintf x
#ifdef SPLASH_DEBUG
#define BDBG_MSG(x) xprintf x
#else
#define BDBG_MSG(x)
#endif

#include "splash-glue.h"

#endif /* __SPLASH_MAGNUM_H__ */

/* End of File */

