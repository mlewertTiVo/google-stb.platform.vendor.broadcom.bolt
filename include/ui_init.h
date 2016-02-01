/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __UI_INIT_H__
#define __UI_INIT_H__

/* dependencies */
int ui_init_cmddisp(void);
int ui_init_envcmds(void);
int ui_init_devcmds(void);
int ui_init_netcmds(void);
int ui_init_loadcmds(void);
int ui_init_memcmds(void);
int ui_init_examcmds(void);
int ui_init_flashcmds(void);
int ui_init_vendorcmds(void);
int ui_init_maccmds(void);
int ui_init_disktestcmds(void);
int ui_init_misccmds(void);
int ui_init_devtree(void);
int ui_init_memtestcmds(void);
int ui_init_ethertestcmds(void);
int ui_init_rpmb_cmds(void);
int ui_init_shacmds(void);

/* master init function */
void ui_init(void);

#endif /* __UI_INIT_H__ */
