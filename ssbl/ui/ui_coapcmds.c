/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#if CFG_COAP

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "brcm/coap_api.h"
#include "ui_command.h"

#if CFG_COAP
#include "brcm/coap_api.h"


static int ui_cmd_coap_listen(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_coap_term(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

int ui_init_coapcmds(void)
{
	cmd_addcmd("coap listen",
			ui_cmd_coap_listen,
			NULL,
			"CoAP listener (server).",
			"coap listen",
			"");

	cmd_addcmd("coap term",
		ui_cmd_coap_term,
		NULL,
		"CoAP listener (server).",
		"coap term",
		"");

	return 0;
}

static int ui_cmd_coap_listen(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int ret;

	ret = coap_server_init();
	if (0 == ret) {
		coap_server_thread(NULL);
		return 0;
	} else
		return -1;
}

static int ui_cmd_coap_term(ui_cmdline_t *cmd, int argc, char *argv[])
{
	return coap_server_term();
}

#endif /* CFG_COAP */
