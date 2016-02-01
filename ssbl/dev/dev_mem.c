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

#include "bolt.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "iocb.h"
#include "device.h"
#include "error.h"


static void dev_mem_probe(bolt_driver_t *drv, unsigned long probe_a,
			  unsigned long probe_b, void *probe_ptr) {
	char descr[16];

	xsprintf(descr, "%s", drv->drv_description);
	bolt_attach(drv, NULL, NULL, descr);
}


static int mem_open(bolt_devctx_t *ctx)
{
	return BOLT_OK;
}

static int mem_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	lib_memcpy(buffer->buf_ptr,
		(uint8_t *)(unsigned)buffer->buf_offset, buffer->buf_length);
	buffer->buf_retlen = buffer->buf_length;
	return BOLT_OK;
}

static int mem_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	inpstat->inp_status = 1; /* Always ready */
	return BOLT_OK;
}

static int mem_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	return BOLT_ERR_UNSUPPORTED;
}

static int mem_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	return BOLT_ERR_UNSUPPORTED;
}

static int mem_close(bolt_devctx_t *ctx)
{
	return BOLT_OK;
}

static void mem_poll(bolt_devctx_t *ctx, int64_t ticks)
{
}

static void mem_reset(void *softc)
{
}


static const bolt_devdisp_t dev_mem_dispatch = {
	mem_open,
	mem_read,
	mem_inpstat,
	mem_write,
	mem_ioctl,
	mem_close,
	mem_poll,
	mem_reset
};

const bolt_driver_t dev_mem = {
	"Memory",
	"mem",
	BOLT_DEV_MEM,
	&dev_mem_dispatch,
	dev_mem_probe
};
