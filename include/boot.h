/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BOOT_H__
#define __BOOT_H__

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#if defined(CFG_ARCH_MIPS)
#define BOOT_START_ADDRESS	0x80010000	/* VA of boot area */
#define BOOT_AREA_SIZE		(256*1024)	/* 256K */
#else
#define BOOT_START_ADDRESS	0x8000
#define BOOT_AREA_SIZE		(32*1024*1024)	/* 32MB */
	/* TODO: Verify 32MB. It is quite big comparing 256K in MIPS.
	 *       32MB was a magic number in ssbl/ui/ui_loadcmd.c, 0x2000000. */
#endif

#endif /* __BOOT_H__ */

