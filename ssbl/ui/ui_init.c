/***************************************************************************
 *	 Copyright (c) 2012-2015, Broadcom Corporation
 *	 All Rights Reserved
 *	 Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "ui_init.h"


/*  *********************************************************************
	*  ui_init()
	*
	*  Call all the other UI initialization modules.  Each of these
	*  modules usually calls back to the UI dispatcher to add command
	*  tables.
	*
	* Input parameters:
	*	nothing
	*
	* Return value:
	*	nothing
	********************************************************************* */
void ui_init(void)
{
	ui_init_cmddisp();

#if (CFG_CMD_LEVEL >= 1)
	ui_init_envcmds();
#endif

#if (CFG_CMD_LEVEL >= 2)
	ui_init_devcmds();
#endif

#if CFG_NETWORK
	ui_init_netcmds();
#endif

	ui_init_loadcmds();

#if (CFG_CMD_LEVEL >= 3)
	ui_init_memcmds();
	ui_init_examcmds();
#endif

	ui_init_flashcmds();

#if CFG_VENDOR_EXTENSIONS
	ui_init_vendorcmds();
#endif

#if (CFG_CMD_LEVEL >= 1)
	ui_init_maccmds();
#endif

	ui_init_disktestcmds();
	ui_init_misccmds();
	ui_init_devtree();
#if (CFG_CMD_LEVEL >= 3)
	ui_init_memtestcmds();
#endif
#if (CFG_CMD_LEVEL >= 5)
	ui_init_ethertestcmds();
#endif
#if CFG_EMMC_FLASH
	ui_init_rpmb_cmds();
#endif
	ui_init_shacmds();
#if CFG_TRUSTZONE
	ui_init_tz();
#endif
}
