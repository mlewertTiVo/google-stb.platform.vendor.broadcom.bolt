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

#ifndef __UI_COMMAND_H__
#define __UI_COMMAND_H__

#include "lib_queue.h"

typedef struct ui_cmdsw_s {
	int swidx;
	char *swname;
	char *swvalue;
} ui_cmdsw_t;

#define MAX_TOKENS	64
#define MAX_SWITCHES	16
#define MAX_TOKEN_SIZE  1000
typedef struct ui_cmdline_s {
	int argc;
	char *argv[MAX_TOKENS];
	int swc;
	ui_cmdsw_t swv[MAX_SWITCHES];
	int (*func) (struct ui_cmdline_s *, int argc, char *argv[]);
	int argidx;
	char *ref;
	char *usage;
	char *switches;
} ui_cmdline_t;

typedef struct ui_command_s {
	queue_t list;
	int term;
	char *ptr;
} ui_command_t;

#define CMD_TERM_EOL	0
#define CMD_TERM_SEMI	1
#define CMD_TERM_AND	2
#define CMD_TERM_OR	3

int cmd_sw_value(ui_cmdline_t * cmd, char *swname, const char **swvalue);
int cmd_sw_posn(ui_cmdline_t * cmd, char *swname);
char *cmd_sw_name(ui_cmdline_t * cmd, int swidx);
int cmd_sw_isset(ui_cmdline_t * cmd, char *swname);
char *cmd_getarg(ui_cmdline_t * cmd, int argnum);
void cmd_free(ui_cmdline_t * cmd);
int cmd_sw_validate(ui_cmdline_t * cmd, char *validstr);
void cmd_parse(ui_cmdline_t * cmd, const char *line);
int cmd_addcmd(char *command,
	       int (*func) (ui_cmdline_t *, int argc, char *argv[]),
	       void *ref, char *help, char *usage, char *switches);
int cmd_lookup(const char *text, ui_cmdline_t * cmd);
void cmd_init(void);
int cmd_getcommand(ui_cmdline_t * cmd);
void cmd_showusage(ui_cmdline_t * cmd);

#define CMD_ERR_INVALID	-1
#define CMD_ERR_AMBIGUOUS -2
#define CMD_ERR_BLANK -3

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

const char *ui_errstring(int errcode);
int ui_showerror(int errcode, char *tmplt, ...);
int ui_showusage(ui_cmdline_t * cmd);
int ui_docommand(const char *buffer);
void ui_restart(int);
int ui_init_cmddisp(void);
ui_command_t *cmd_readcommand(const char **pptr);

#endif /* __UI_COMMAND_H__ */

