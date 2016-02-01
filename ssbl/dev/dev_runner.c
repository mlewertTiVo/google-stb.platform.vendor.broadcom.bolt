/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#if CFG_RUNNER

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_hexdump.h"

#include "iocb.h"
#include "ioctl.h"
#include "device.h"
#include "devfuncs.h"
#include "mii.h"
#include "error.h"
#include "common.h"

#include "bsp_config.h"
#include "timer.h"
#include "macutils.h"
#include "ui_command.h"
#include "env_subr.h"
#include "dev_bcmethsw.h"
#include "dev_sf2.h"
#include "board.h"
#include "net_ebuf.h"
#include "net_mdio.h"

#include "libfdt_env.h"
#include "fdt.h"
#include "devtree.h"

#define PFX	"RUNNER: "

static int runner_ether_open(bolt_devctx_t *ctx)
{
	return -1;
}

static int runner_ether_close(bolt_devctx_t *ctx)
{
	return -1;
}

static void runner_ether_init(bolt_driver_t *drv, int instance)
{
	/* Initialize the switch MDIO bus for early access */
	bcm_sf2_mdio_init();
}

static void runner_ether_probe(bolt_driver_t *drv, unsigned long probe_a,
			       unsigned long probe_b, void *probe_ptr)
{
	if (!env_getenv("ETH_OFF"))
		runner_ether_init(drv, (int)probe_ptr);
}

int runner_ether_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	return -1;
}

static int runner_ether_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	return -1;
}

static int runner_ether_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	return -1;
}

static int runner_ether_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	return -1;
}

static const bolt_devdisp_t runner_ether_dispatch = {
	.dev_open = runner_ether_open,
	.dev_read = runner_ether_read,
	.dev_inpstat = runner_ether_inpstat,
	.dev_write = runner_ether_write,
	.dev_ioctl = runner_ether_ioctl,
	.dev_close = runner_ether_close,
};

const bolt_driver_t runnerdrv = {
	"RUNNER Internal Ethernet",
	"eth",
	BOLT_DEV_NETWORK,
	&runner_ether_dispatch,
	runner_ether_probe
};

#endif /* CFG_RUNNER */
