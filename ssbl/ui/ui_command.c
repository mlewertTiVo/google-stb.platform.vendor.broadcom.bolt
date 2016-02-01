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

#include <stdarg.h>

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "error.h"
#include "env_subr.h"
#include "bolt.h"

#include "ui_command.h"

struct lexstate {
	const char *rawline;
	const char *lineptr;
	const char *saveptr;
	int curpos;
	int expand;
};

struct cmdtab {
	struct cmdtab *sibling;
	struct cmdtab *child;
	char *cmdword;
	int (*func)(ui_cmdline_t *, int argc, char *argv[]);
	void *ref;
	char *help;
	char *usage;
	char *switches;
};

struct cmdtab *cmd_root;

#define myisalpha(x) (((x) >= 'A') && ((x) <= 'Z') && \
	((x) >= 'a') && ((x) <= 'z'))
#define myisdigit(x) (((x) >= '0') && ((x) <= '9'))
char *varchars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_?";

static int getvarname(const char **ptr, char *vname, int vmax)
{
	const char *p;
	char *v;

	p = *ptr;
	v = vname;

	vmax--;

	/*
	 * Check for the case of a var in braces:  ${foo}
	 */

	if (*p == '{') {
		p++;
		while (*p && (*p != '}'))
			*v++ = *p++;
		if (*p == '}')
			p++;
	}

	/*
	 * Otherwise, stop at a break character
	 */

	else {
		while (*p) {
			if (strchr(varchars, *p)) {
				*v++ = *p++;
				vmax--;
				if (vmax == 0)
					break;
				continue;
			}
			break;
		}
	}

	*v = 0;
	*ptr = p;

	return v != vname;	/* nonzero if we got something */
}

static char *lex_getvarvalue(char *varname)
{
	char *val;

	val = env_getenv(varname);
	if (!val)
		return "";
	return val;
}

static void lex_init(struct lexstate *lex, const char *line, int expand)
{
	lex->rawline = line;
	lex->lineptr = line;
	lex->saveptr = NULL;
	lex->expand = expand;
	lex->curpos = 0;
}

static int lex_gettoken(struct lexstate *lex, char *token, int tmax)
{
	const char *p;
	char *t;
	int inquote_d = 0;
	int inquote_s = 0;
	int inbracket = 0;
	int inexpansion;
	static char *brkchars = " ;{}";

	t = token;

	/*
	 * See if we were currently expanding
	 */

	inexpansion = (lex->saveptr != NULL);
	p = lex->lineptr;

	/*
	 * Skip leading whitespace
	 */

	while (*p) {
		if (*p == ' ') {
			p++;
			continue;
		}
		break;
	}

	/*
	 * Update current position
	 */

	if (inexpansion)
		lex->curpos = lex->lineptr - lex->rawline;

	/*
	 * If after whitespace we're at EOL, exit.
	 */

	lex->lineptr = p;

	if (!*p && !inexpansion)
		return 0;

	/*
	 * Leave one byte in output buffer for null
	 */

	tmax--;

	/*
	 * Start eating characters
	 */

	for (;;) {

		/* token ends on buffer overflow */

		if (tmax == 0)
			break;

		/*
		 * If at end of string, see if we were expanding.  If so,
		 * go back to what we were doing before.
		 */

		if (!*p && inexpansion) {
			p = lex->saveptr;
			inexpansion = 0;
			lex->saveptr = NULL;
		}

		/*
		 * If at end of string, it's for real this time.
		 */

		if (!*p)
			break;

		/*
		 * Semicolon?  Stop now.
		 */

		if (t == token) {
			if (strchr(brkchars, *p)) {
				*t++ = *p++;
				break;
			}
		}

		/*
		 * Check for escape
		 */

		if ((*p == '\\') && !inquote_s) {
			if (strchr("\\\"$", *(p + 1))) {
				p++;
				*t++ = *p;
				tmax--;
				if (*p)
					p++;
				continue;
			}
		}

		/*
		 * Check for macro expansions
		 */

		if (lex->expand && !inexpansion && (*p == '$') && !inquote_s) {
			char vname[100];

			p++;
			if (getvarname(&p, vname, sizeof(vname))) {
				lex->saveptr = p;
				inexpansion = 1;
				p = lex_getvarvalue(vname);
			}
			continue;
		}

		/*
		 * Switch on/off quote mode if we see a quote or bracket
		 */

		if ((*p == '[') && !inquote_d && !inquote_s) {
			p++;
			inbracket = 1;
			continue;
		}

		if (inbracket && (*p == ']')) {
			p++;
			inbracket = 0;
			continue;
		}

		if (*p == '\'') {
			/* but only if not in double quotes */
			if (inquote_d == 0) {
				p++;
				inquote_s = !inquote_s;
				continue;
			}
		}

		if ((*p == '"') && !inbracket) {
			/* but only if not in single quotes */
			if (inquote_s == 0) {
				p++;
				inquote_d = !inquote_d;
				continue;
			}
		}

		/*
		 * Stop at a space or semicolon
		 */

		if (!inbracket && !inquote_s && !inquote_d &&
		    (strchr(brkchars, *p))) {
			break;
		}

		/*
		 * Copy a character
		 */

		*t++ = *p++;
		tmax--;
	}

	/*
	 * Update state and return
	 */

	lex->lineptr = p;
	*t = 0;
	return (t != token);
}

int cmd_sw_value(ui_cmdline_t *cmd, char *swname, const char **swvalue)
{
	int idx;

	for (idx = 0; idx < cmd->swc; idx++) {
		if (strcmp(swname, cmd->swv[idx].swname) == 0) {
			*swvalue = cmd->swv[idx].swvalue;
			return 1;
		}
	}

	return 0;
}

int cmd_sw_posn(ui_cmdline_t *cmd, char *swname)
{
	int idx;

	for (idx = 0; idx < cmd->swc; idx++) {
		if (strcmp(swname, cmd->swv[idx].swname) == 0)
			return cmd->swv[idx].swidx;
	}

	return -1;
}

char *cmd_sw_name(ui_cmdline_t *cmd, int swidx)
{
	if ((swidx < 0) || (swidx >= cmd->swc))
		return NULL;

	return cmd->swv[swidx].swname;
}

int cmd_sw_isset(ui_cmdline_t *cmd, char *swname)
{
	int idx;

	for (idx = 0; idx < cmd->swc; idx++) {
		if (strcmp(swname, cmd->swv[idx].swname) == 0)
			return 1;
	}

	return 0;
}

char *cmd_getarg(ui_cmdline_t *cmd, int argnum)
{
	argnum += cmd->argidx;
	if ((argnum < 0) || (argnum >= cmd->argc))
		return NULL;
	return cmd->argv[argnum];
}

void cmd_free(ui_cmdline_t *cmd)
{
	int idx;

	for (idx = 0; idx < cmd->argc; idx++)
		KFREE(cmd->argv[idx]);

	for (idx = 0; idx < cmd->swc; idx++)
		KFREE(cmd->swv[idx].swname);

	cmd->argc = 0;
	cmd->swc = 0;
}

int cmd_sw_validate(ui_cmdline_t *cmd, char *validstr)
{
	char *vdup;
	char *vptr;
	char *vnext;
	char atype;
	char *x;
	int idx;
	int valid;

	if (cmd->swc == 0)
		return -1;

	vdup = strdup(validstr);

	for (idx = 0; idx < cmd->swc; idx++) {
		vptr = vdup;

		vnext = vptr;
		valid = 0;

		while (vnext) {

			/*
			 * Eat the next switch description from the valid string
			 */
			x = strchr(vptr, '|');
			if (x) {
				*x = '\0';
				vnext = x + 1;
			} else {
				vnext = NULL;
			}

			/*
			 * Get the expected arg type, if any
			 */
			x = strchr(vptr, '=');
			if (x) {
				atype = *(x + 1);
				*x = 0;
			} else {
				x = strchr(vptr, ';');
				if (x)
					*x = 0;
				atype = 0;
			}

			/*
			 * See if this matches what the user typed
			 * XXX for now, ignore the arg type processing but
			 * complain if an arg is missing.
			 */

			if (strcmp(vptr, cmd->swv[idx].swname) == 0) {
				/* Value not needed and not supplied */
				if ((atype == 0)
				    && (cmd->swv[idx].swvalue == NULL)) {
					valid = 1;
				}
				/* value needed and supplied */
				if ((atype != 0)
				    && (cmd->swv[idx].swvalue != NULL)) {
					valid = 1;
				}
				strcpy(vdup, validstr);
				break;
			}

			/*
			 * Otherwise, next!
			 */

			strcpy(vdup, validstr);
			vptr = vnext;
		}

		/*
		 * If not valid, return index of bad switch
		 */

		if (valid == 0) {
			KFREE(vdup);
			return idx;
		}

	}

	/*
	 * Return -1 if everything went well.  A little strange,
	 * but it's easier this way.
	 */

	KFREE(vdup);
	return -1;
}

void cmd_parse(ui_cmdline_t *cmd, const char *line)
{
	char *cldup;
	int res, cnt;
	char *tokens[MAX_TOKENS + MAX_SWITCHES];
	char *x;
	int tmax;
	int idx;
	struct lexstate lex;

	/*
	 * Start with nothing.
	 */

	cmd->argc = 0;
	cmd->swc = 0;

	tmax = MAX_TOKEN_SIZE;
	cldup = KMALLOC(tmax, 0);

	/*
	 * Chop up the command line into tokens.
	 */

	cnt = 0;

	lex_init(&lex, line, 1);

	while ((res = lex_gettoken(&lex, cldup, tmax))) {
		tokens[cnt] = strdup(cldup);
		cnt++;
		if (cnt == (MAX_TOKENS + MAX_SWITCHES))
			break;
	}

	/*
	 * Make a pass over the accumulated tokens and
	 * weed out the ones that look like switches
	 */

	for (idx = 0; idx < cnt; idx++) {
		if (tokens[idx][0] != '-') {
			/*
			 * It's a token (non-switch).  Just remember it.
			 * Free tokens that don't fit in our array.
			 */
			if (cmd->argc < MAX_TOKENS) {
				cmd->argv[cmd->argc] = tokens[idx];
				cmd->argc++;
			} else {
				KFREE(tokens[idx]);
			}
		} else {
			/*
			 * It's a switch.  Remember it, and try to
			 * figure out which part is the switch and which
			 * is the value.  Free switches that don't fit
			 * in the array.
			 */
			if (cmd->swc < MAX_SWITCHES) {
				cmd->swv[cmd->swc].swname = tokens[idx];
				x = strchr(cmd->swv[cmd->swc].swname, '=');
				if (x) {
					*x++ = '\0';
					cmd->swv[cmd->swc].swvalue = x;
				} else {
					cmd->swv[cmd->swc].swvalue = NULL;
				}
				/*
				 * swidx is the index of the argument that this
				 * switch precedes.  So, if you have
				 * "foo -d bar", swidx for "-d" would be 1.
				 */
				cmd->swv[cmd->swc].swidx = cmd->argc;
				cmd->swc++;
			} else {
				KFREE(tokens[idx]);
			}
		}
	}

	KFREE(cldup);
}

static struct cmdtab *cmd_findword(struct cmdtab *list, char *cmdword)
{
	while (list) {
		if (strcmp(cmdword, list->cmdword) == 0)
			return list;
		list = list->sibling;
	}

	return NULL;
}

int cmd_addcmd(char *command,
	       int (*func)(ui_cmdline_t *, int argc, char *argv[]),
	       void *ref, char *help, char *usage, char *switches)
{
	char token[1024];
	struct lexstate lex;
	struct cmdtab **list = &cmd_root;
	struct cmdtab *cmd = NULL;

	lex_init(&lex, command, 1);

	while (lex_gettoken(&lex, token, sizeof(token))) {
		cmd = cmd_findword(*list, token);
		if (!cmd) {
			cmd = KMALLOC(sizeof(struct cmdtab) + strlen(token) + 1,
					0);
			memset(cmd, 0, sizeof(struct cmdtab));
			cmd->cmdword = (char *)(cmd + 1);
			strcpy(cmd->cmdword, token);
			cmd->sibling = *list;
			*list = cmd;
		}
		list = &(cmd->child);
	}

	if (!cmd)
		return -1;

	cmd->func = func;
	cmd->usage = usage;
	cmd->ref = ref;
	cmd->help = help;
	cmd->switches = switches;

	return 0;
}

static void _dumpindented(char *str, int amt)
{
	int idx;
	char *dupstr;
	char *end;
	char *ptr;

	dupstr = strdup(str);

	ptr = dupstr;

	while (*ptr) {
		for (idx = 0; idx < amt; idx++)
			printf(" ");

		end = strchr(ptr, '\n');

		if (end)
			*end++ = '\0';
		else
			end = ptr + strlen(ptr);

		printf("%s\n", ptr);
		ptr = end;
	}

	KFREE(dupstr);
}

/*
 *  Formats a string to print as a paragraph.  If the string runs past max_cols,
 *  a line break and indent are inserted.  For now, the function assumes that
 *  there was already an indent when it was called.
 */
static void format_print(int indent, int max_cols, char *str)
{
	int i;
	int done = 0;
	char buf[128];
	char *p0, *p1;

	if ((int)sizeof(buf) < max_cols)
		return;

	*str = toupper(*str);

	/* Undo the string's own hopeful formatting */
	for (p0 = str; *p0; p0++)
		if (*p0 == '\n' || *p0 == '\r' || *p0 == '\t')
			*p0 = ' ';
	p0 = str;
	while (*p0 && !done) {
		p1 = p0;
		while (*p1 && *p1 != ' ' && *p1 != '\t')
			p1++;
		while (*p1 && *p1 == ' ')
			p1++;
		if (indent + (p1 - str) >= max_cols) {
			if (p0 == str) {
				/* We're forced to print past the margin */
				strncpy(buf, p0, p1-p0);
				buf[p1-p0] = 0;
				str = p0 = p1;
			} else {
				strncpy(buf, str, p0 - str);
				buf[p0-str] = 0;
				str = p0;
			}
			printf("%s\n", buf);
			for (i = 0; i < indent; i++)
				printf(" ");
		} else if (!*p1) {
			printf("%s\n", str);
			done = 1;
		} else {
			p0 = p1;
		}
	}
}


static void _dumpswitches(char *str)
{
	int len, i;
	char *switches;
	char *end;
	char *ptr;
	char *semi;
	const int indent = 24;
	const int max_cols = 74;

	switches = strdup(str);

	ptr = switches;

	while (*ptr) {
		end = strchr(ptr, '|');
		if (end)
			*end++ = '\0';
		else
			end = ptr + strlen(ptr);

		semi = strchr(ptr, ';');
		if (semi) {
			*semi++ = '\0';
			while (*semi && *semi == ' ')
				semi++;
		}
		len = strlen(ptr);
		printf("     %s ", ptr);
		for (i = len + 6; i < indent - 1; i++)
			printf(".");
		printf(" ");

		if (semi && *semi)
			format_print(indent, max_cols, semi);
		else
			printf("(no information)\n");
		ptr = end;
	}
	KFREE(switches);
}


static void _dumpcmds(struct cmdtab *cmd, int level, char **words, int verbose)
{
	const int indent = 20;
	const int max_cols = 74;
	int idx;
	int len;

	while (cmd) {
		len = 0;
		words[level] = cmd->cmdword;
		if (cmd->func) {
			for (idx = 0; idx < level; idx++) {
				printf("%s ", words[idx]);
				len += strlen(words[idx]) + 1;
			}
			printf("%s", cmd->cmdword);
			len += strlen(cmd->cmdword) + 1;
			printf(" ");
			for (idx = len; idx < indent-1; idx++)
				printf(".");
			printf(" ");
			format_print(indent, max_cols, cmd->help);
			if (verbose) {
				printf("\n");
				_dumpindented(cmd->usage, 5);
				printf("\n");
				_dumpswitches(cmd->switches);
				printf("\n");
			}
		}
		_dumpcmds(cmd->child, level + 1, words, verbose);
		cmd = cmd->sibling;
	}
}

static void dumpcmds(int verbose)
{
	char *words[20];

	/* Sort the command list and then dump the commands */
	struct cmdtab *new_root = 0, *c0, *c1;
	struct cmdtab *next = cmd_root;
	struct cmdtab **prev = 0;

	while ((c0 = next)) {
		next = c0->sibling;
		c1 = new_root;
		prev = &new_root;

		while (1) {
			if (!c1 || strcmp(c0->cmdword, c1->cmdword) < 0) {
				/* insert c0 */
				*prev = c0;
				c0->sibling = c1;
				break;
			} else {
				prev = &c1->sibling;
				c1 = c1->sibling;
			}
		}
	}
	cmd_root = new_root;
	_dumpcmds(cmd_root, 0, words, verbose);
}

static void _showpossible(ui_cmdline_t *cline, struct cmdtab *cmd)
{
	int i;

	if (cline->argidx == 0) {
		printf("Available commands: ");
	} else {
		printf("Available \"");
		for (i = 0; i < cline->argidx; i++)
			printf("%s%s", (i == 0) ? "" : " ", cline->argv[i]);
		printf("\" commands: ");
	}

	while (cmd) {
		printf("%s", cmd->cmdword);
		if (cmd->sibling)
			printf(", ");
		cmd = cmd->sibling;
	}

	printf("\n");
}

static int cmd_help(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct cmdtab **tab;
	struct cmdtab *cword;
	int idx;

	if (argc == 0) {
		printf("Available commands:\n\n");
		dumpcmds(0);
		printf("\n");
		printf("For more information about a command, enter 'help command-name'\n");
	} else {
		idx = 0;
		tab = &cmd_root;
		cword = NULL;

		for (;;) {
			cword = cmd_findword(*tab, argv[idx]);
			if (!cword)
				break;
			if (cword->func != NULL)
				break;
			idx++;
			tab = &(cword->child);
			if (idx >= argc)
				break;
		}

		if (cword == NULL) {
			printf("No help available for '%s'.\n\n", argv[idx]);
			printf("Type 'help' for a list of commands.\n");
			return -1;
		}

		if (!cword->func && (idx >= argc)) {
			printf("No help available for '%s'.\n\n",
			       cword->cmdword);
			printf("Type 'help' for a list of commands.\n");
			return -1;
		}

		printf("\n  SUMMARY\n\n");
		_dumpindented(cword->help, 5);
		printf("\n  USAGE\n\n");
		_dumpindented(cword->usage, 5);
		if (cword->switches && cword->switches[0]) {
			printf("\n  OPTIONS\n\n");
			_dumpswitches(cword->switches);
		}
		printf("\n");
	}

	return 0;
}

void cmd_init(void)
{
	cmd_root = NULL;

	cmd_addcmd("help",
		   cmd_help,
		   NULL,
		   "Obtain help for BOLT commands",
		   "help [command]\n\n"
		   "Without any parameters, the 'help' command will display a summary\n"
		   "of available commands.  For more details on a command, type 'help'\n"
		   "and the command name.", "");
}

int cmd_lookup(const char *text, ui_cmdline_t *cmd)
{
	struct cmdtab **tab;
	struct cmdtab *cword;
	int idx;

	/*
	 * Reset the command line
	 */

	memset(cmd, 0, sizeof(ui_cmdline_t));

	/*
	 * Break it up into tokens
	 */

	cmd_parse(cmd, text);

	if (cmd->argc == 0)
		return CMD_ERR_BLANK;

	/*
	 * Start walking the tree looking for a function
	 * to execute.
	 */

	idx = 0;
	tab = &cmd_root;
	cword = NULL;

	for (;;) {
		cword = cmd_findword(*tab, cmd->argv[idx]);
		if (!cword)
			break;
		if (cword->func != NULL)
			break;
		idx++;
		tab = &(cword->child);
		if (idx >= cmd->argc)
			break;
	}

	cmd->argidx = idx;

	if (cword == NULL) {
		printf("Invalid command: \"%s\"\n", text);
		_showpossible(cmd, *tab);
		printf("\n");
		return CMD_ERR_INVALID;
	}

	if (!cword->func && (idx >= cmd->argc)) {
		printf("Incomplete command: \"%s\"\n", text);
		_showpossible(cmd, *tab);
		printf("\n");
		return CMD_ERR_AMBIGUOUS;
	}

	cmd->argidx++;
	cmd->ref = cword->ref;
	cmd->usage = cword->usage;
	cmd->switches = cword->switches;
	cmd->func = cword->func;

	return 0;
}

void cmd_showusage(ui_cmdline_t *cmd)
{
	printf("\n");
	_dumpindented(cmd->usage, 5);
	printf("\n");
	if (cmd->switches[0]) {
		_dumpswitches(cmd->switches);
		printf("\n");
	}
}

ui_command_t *cmd_readcommand(const char **pptr)
{
	const char *ptr = *pptr;
	int insquote = FALSE;
	int indquote = FALSE;
	ui_command_t *cmd;
	int term = CMD_TERM_EOL;
	int termlen = 0;

	while (*ptr && ((*ptr == ' ') || (*ptr == '\t')))
		ptr++;
	*pptr = ptr;

	if (!*ptr)
		return NULL;

	while (*ptr) {

		if (!insquote && !indquote) {
			if ((*ptr == ';') || (*ptr == '\n')) {
				term = CMD_TERM_SEMI;
				termlen = 1;
				break;
			}
			if ((*ptr == '&') && (*(ptr + 1) == '&')) {
				term = CMD_TERM_AND;
				termlen = 2;
				break;
			}
			if ((*ptr == '|') && (*(ptr + 1) == '|')) {
				term = CMD_TERM_OR;
				termlen = 2;
				break;
			}
		}

		if (*ptr == '\'') {
			insquote = !insquote;
			ptr++;
			continue;
		}

		if (!insquote) {
			if (*ptr == '"') {
				indquote = !indquote;
				ptr++;
				continue;
			}
		}

		if (!insquote && (*ptr == '\\')) {
			ptr++;	/* skip the backslash */
			if (*ptr)
				ptr++;	/* skip escaped char */
			continue;
		}

		ptr++;
	}

	ptr += termlen;

	cmd = (ui_command_t *)
	    KMALLOC((sizeof(ui_cmdline_t) + (ptr - *pptr) - termlen + 1), 0);
	memcpy((char *)(cmd + 1), *pptr, ptr - *pptr - termlen);
	cmd->ptr = (char *)(cmd + 1);
	cmd->term = term;
	cmd->ptr[ptr - *pptr - termlen] = 0;

	*pptr = ptr;

	return cmd;
}
