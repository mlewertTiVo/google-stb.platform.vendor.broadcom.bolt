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
#if CFG_VENDOR_EXTENSIONS || CFG_CUSTOM_CODE
#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "xiocb.h"
#include "vendor_xiocb.h"
#include "vendor_iocb.h"

#include "device.h"
#include "console.h"
#include "timer.h"
#include "devfuncs.h"
#include "env_subr.h"
#include "ui_command.h"
#include "bolt.h"
#include "board.h"
#include "ioctl.h"
#include "initdata.h"
#include "fsbl-common.h"
#include "ssbl-common.h"
#include "devtree.h"
#include "bchp_sun_top_ctrl.h"
#include "common.h"
#endif /* CFG_VENDOR_EXTENSIONS || CFG_CUSTOM_CODE */


#if CFG_CUSTOM_CODE
void custom_early(void)
{
	xprintf("*** CFG_CUSTOM_CODE in %s ***\n", __func__);
}

void custom_init(void)
{
	xprintf("*** CFG_CUSTOM_CODE in %s ***\n", __func__);
}

void custom_main(void)
{
	xprintf("*** CFG_CUSTOM_CODE in %s ***\n", __func__);
}
#endif /* CFG_CUSTOM_CODE */





#if CFG_VENDOR_EXTENSIONS
/*
BOLT>example truck -opta car bike -optb -num=42 skate wheel
Option A selected
Option B selected
Number is 42

 --- args (5) ----
ARGV[0] = truck
ARGV[1] = car
ARGV[2] = bike
ARGV[3] = skate
ARGV[4] = wheel
*** command status = 0

*/
static int ui_cmd_example(ui_cmdline_t *cmd, int argc, char *argv[])
{
	const char *s;
	int num = 0;

	if (cmd_sw_isset(cmd, "-opta"))
		xprintf("Option A selected\n");

	if (cmd_sw_isset(cmd, "-optb"))
		xprintf("Option B selected\n");

	if (cmd_sw_value(cmd, "-num", &s)) {
		num = atoi(s);
		xprintf("Number is %d\n", num);
	}

	xprintf("\n --- args (%d) ----\n",argc );

	for(num=0; num < argc; num++)
		xprintf("ARGV[%d] = %s\n", num, argv[num]);

	return 0;
}

int ui_init_vendorcmds(void)
{
	xprintf("*** CFG_VENDOR_EXTENSIONS in %s ***\n",__FUNCTION__);	
	
	cmd_addcmd("example", ui_cmd_example, NULL,
		"An example command",
		"vendor_cmd -opta -optb -num=N",
		"-opta;Option A|"
		"-optb;Option B|"
		"-num=*;A number");

	return 0;
}


bolt_int_t bolt_vendor_doxreq(bolt_vendor_xiocb_t *xiocb)
{
	xprintf("*** CFG_VENDOR_EXTENSIONS in %s ***\n",__FUNCTION__);
	return BOLT_ERR_INV_COMMAND;
}

#endif /* CFG_VENDOR_EXTENSIONS */
