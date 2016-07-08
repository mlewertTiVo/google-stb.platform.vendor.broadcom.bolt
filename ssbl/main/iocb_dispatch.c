/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_queue.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "error.h"
#include "device.h"
#include "timer.h"
#include "fileops.h"
#include "env_subr.h"
#include "bolt.h"
#include "console.h"
#include "bsp_config.h"
#include "initdata.h"
#include "reboot.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define HV	1		/* handle valid */

#ifndef CFG_BOARD_ID
#define CFG_BOARD_ID 0
#endif

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

bolt_devctx_t *bolt_handle_table[BOLT_MAX_HANDLE];

extern void clear_all_d_cache(void);
extern void invalidate_all_i_cache(void);

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

static int bolt_cmd_fw_getinfo(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_restart(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_boot(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_cpuctl(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_gettime(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_memenum(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_fw_flushcache(bolt_devctx_t *ctx, bolt_iocb_t *iocb);

static int bolt_cmd_dev_gethandle(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_enum(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_open(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_inpstat(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_read(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_write(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_ioctl(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_close(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_dev_getinfo(bolt_devctx_t *ctx, bolt_iocb_t *iocb);

static int bolt_cmd_env_enum(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_env_get(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_env_set(bolt_devctx_t *ctx, bolt_iocb_t *iocb);
static int bolt_cmd_env_del(bolt_devctx_t *ctx, bolt_iocb_t *iocb);

/*  *********************************************************************
    *  Dispatch table
    ********************************************************************* */

struct bolt_cmd_dispatch_s {
	unsigned int plistsize;
	int flags;
	int (*func) (bolt_devctx_t *ctx, bolt_iocb_t *iocb);
};

static const struct bolt_cmd_dispatch_s
	bolt_cmd_dispatch_table[BOLT_CMD_MAX] = {
	{sizeof(iocb_fwinfo_t), 0, bolt_cmd_fw_getinfo}
	,			/* 0 : BOLT_CMD_FW_GETINFO */
	{sizeof(iocb_exitstat_t), 0, bolt_cmd_fw_restart}
	,			/* 1 : BOLT_CMD_FW_RESTART */
	{sizeof(iocb_buffer_t), 0, bolt_cmd_fw_boot}
	,			/* 2 : BOLT_CMD_FW_BOOT */
	{sizeof(iocb_cpuctl_t), 0, bolt_cmd_fw_cpuctl}
	,			/* 3 : BOLT_CMD_FW_CPUCTL */
	{sizeof(iocb_time_t), 0, bolt_cmd_fw_gettime}
	,			/* 4 : BOLT_CMD_FW_GETTIME */
	{sizeof(iocb_meminfo_t), 0, bolt_cmd_fw_memenum}
	,			/* 5 : BOLT_CMD_FW_MEMENUM */
	{0, 0, bolt_cmd_fw_flushcache}
	,			/* 6 : BOLT_CMD_FW_FLUSHCACHE */
	{-1, 0, NULL}
	,			/* 7 : */
	{-1, 0, NULL}
	,			/* 8 : */
	{sizeof(iocb_buffer_t), 0, bolt_cmd_dev_gethandle}
	,			/* 9 : BOLT_CMD_DEV_GETHANDLE */
	{sizeof(iocb_envbuf_t), 0, bolt_cmd_dev_enum}
	,			/* 10 : BOLT_CMD_DEV_ENUM */
	{sizeof(iocb_buffer_t), 0, bolt_cmd_dev_open}
	,			/* 11 : BOLT_CMD_DEV_OPEN */
	{sizeof(iocb_inpstat_t), HV, bolt_cmd_dev_inpstat}
	,			/* 12 : BOLT_CMD_DEV_INPSTAT */
	{sizeof(iocb_buffer_t), HV, bolt_cmd_dev_read}
	,			/* 13 : BOLT_CMD_DEV_READ */
	{sizeof(iocb_buffer_t), HV, bolt_cmd_dev_write}
	,			/* 14 : BOLT_CMD_DEV_WRITE */
	{sizeof(iocb_buffer_t), HV, bolt_cmd_dev_ioctl}
	,			/* 15 : BOLT_CMD_DEV_IOCTL */
	{0, HV, bolt_cmd_dev_close}
	,			/* 16 : BOLT_CMD_DEV_CLOSE */
	{sizeof(iocb_buffer_t), 0, bolt_cmd_dev_getinfo}
	,			/* 17 : BOLT_CMD_DEV_GETINFO */
	{-1, 0, NULL}
	,			/* 18 : */
	{-1, 0, NULL}
	,			/* 19 : */
	{sizeof(iocb_envbuf_t), 0, bolt_cmd_env_enum}
	,			/* 20 : BOLT_CMD_ENV_ENUM */
	{-1, 0, NULL}
	,			/* 21 : */
	{sizeof(iocb_envbuf_t), 0, bolt_cmd_env_get}
	,			/* 22 : BOLT_CMD_ENV_GET */
	{sizeof(iocb_envbuf_t), 0, bolt_cmd_env_set}
	,			/* 23 : BOLT_CMD_ENV_SET */
	{sizeof(iocb_envbuf_t), 0, bolt_cmd_env_del}
	,			/* 24 : BOLT_CMD_ENV_DEL */
	{-1, 0, NULL}
	,			/* 25 : */
	{-1, 0, NULL}
	,			/* 26 : */
	{-1, 0, NULL}
	,			/* 27 : */
	{-1, 0, NULL}
	,			/* 28 : */
	{-1, 0, NULL}
	,			/* 29 : */
	{-1, 0, NULL}
	,			/* 30 : */
	{-1, 0, NULL}		/* 31 : */
};

/*  *********************************************************************
    *  IOCB dispatch routines
    ********************************************************************* */

void bolt_device_poll(void *x)
{
	int idx;
	bolt_devctx_t **ctx = bolt_handle_table;

	for (idx = 0; idx < BOLT_MAX_HANDLE; idx++, ctx++) {
		if ((*ctx) && ((*ctx)->dev_dev->dev_dispatch->dev_poll)) {
			(*ctx)->dev_dev->dev_dispatch->dev_poll(*ctx,
								bolt_ticks);
		}
	}
}

int bolt_iocb_dispatch(bolt_iocb_t *iocb)
{
	const struct bolt_cmd_dispatch_s *disp;
	int res;
	bolt_devctx_t *ctx;

	/*
	 * Check for commands codes out of range
	 */

	if (iocb->iocb_fcode >= BOLT_CMD_MAX) {
		iocb->iocb_status = BOLT_ERR_INV_COMMAND;
		return iocb->iocb_status;
	}

	/*
	 * Check for command codes in range but invalid
	 */

	disp = &bolt_cmd_dispatch_table[iocb->iocb_fcode];

	/*
	 * Check for invalid parameter list size
	 */

	if (disp->plistsize != iocb->iocb_psize) {
		iocb->iocb_status = BOLT_ERR_INV_PARAM;
		return iocb->iocb_status;
	}

	/*
	 * Determine handle
	 */

	ctx = NULL;
	if (disp->flags & HV) {
		if ((iocb->iocb_handle >= BOLT_MAX_HANDLE) ||
		    (iocb->iocb_handle < 0) ||
		    (bolt_handle_table[iocb->iocb_handle] == NULL)) {
			iocb->iocb_status = BOLT_ERR_INV_PARAM;
			return iocb->iocb_status;
		}
		ctx = bolt_handle_table[iocb->iocb_handle];
	}

	/*
	 * Dispatch to handler routine
	 */
	res = (*disp->func) (ctx, iocb);
	iocb->iocb_status = res;
	return res;
}

static int bolt_newhandle(void)
{
	int idx;

	for (idx = 0; idx < BOLT_MAX_HANDLE; idx++) {
		if (bolt_handle_table[idx] == NULL)
			break;
	}

	if (idx == BOLT_MAX_HANDLE)
		return -1;

	return idx;
}

/*  *********************************************************************
    *  Implementation routines for each IOCB function
    ********************************************************************* */

static int bolt_cmd_fw_getinfo(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	iocb_fwinfo_t *info = &iocb->plist.iocb_fwinfo;

	info->fwi_version = BOLT_VERSION;
	info->fwi_totalmem = ((bolt_int64_t) mem_totalsize) << 20;
	info->fwi_flags =
#ifdef __long64
	    BOLT_FWI_64BIT |
#else
	    BOLT_FWI_32BIT |
#endif
#if CFG_EMBEDDED_PIC
	    BOLT_FWI_RELOC |
#endif
#if (!CFG_RUNFROMKSEG0)
	    BOLT_FWI_UNCACHED |
#endif
	    0;

	info->fwi_boardid = CFG_BOARD_ID;
	info->fwi_bootarea_pa = (bolt_int64_t)BOOT_START_ADDRESS;
	info->fwi_bootarea_va = BOOT_START_ADDRESS;
	info->fwi_bootarea_size = (bolt_int64_t)BOOT_AREA_SIZE;
	info->fwi_reserved1 = 0;
	info->fwi_reserved2 = 0;
	info->fwi_reserved3 = 0;

	return BOLT_OK;
}

static int bolt_cmd_fw_restart(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	if (iocb->iocb_flags & BOLT_FLG_WARMSTART) {
		/* bolt_warmstart not supported anymore
		 with new reset functions.
		 arm bolt_warmstart(iocb->plist.iocb_exitstat.status); */
		return BOLT_ERR;
	} else {
		bolt_master_reboot();
	}

	/* should not get here */

	return BOLT_OK;
}

static int bolt_cmd_fw_boot(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	return BOLT_ERR_INV_COMMAND;	/* not implemented yet */
}

static int bolt_cmd_fw_cpuctl(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	return BOLT_ERR_INV_COMMAND;
}

static int bolt_cmd_fw_gettime(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	POLL();

	iocb->plist.iocb_time.ticks = bolt_ticks;

	return BOLT_OK;
}

static int bolt_cmd_fw_memenum(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	return BOLT_ERR_INV_COMMAND;
}

static int bolt_cmd_fw_flushcache(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	if (iocb->iocb_flags == 0) {
		/* flush L1 D-cache and invalidate L1 I-cache */
		CACHE_INVAL_ALL();
	} else if (iocb->iocb_flags == BOLT_CACHE_FLUSH_D) {
		/* flush all data cache lines */
		CACHE_FLUSH_ALL();
	} else if (iocb->iocb_flags == BOLT_CACHE_INVAL_I) {
		CACHE_INVAL_INS();
	} else
		return BOLT_ERR;

	return BOLT_OK;
}

static int bolt_cmd_dev_enum(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int i, status = BOLT_ERR_DEVNOTFOUND;
	queue_t *qb;
	bolt_device_t *dev = 0;

	for (i = 0, qb = bolt_devices.q_next; qb != &bolt_devices;
	     qb = qb->q_next, ++i) {
		if (i == iocb->plist.iocb_envbuf.enum_idx) {
			dev = (bolt_device_t *) qb;
			xstrncpy((char *)iocb->plist.iocb_envbuf.name_ptr,
				 dev->dev_fullname,
				 iocb->plist.iocb_envbuf.name_length);
			xstrncpy((char *)iocb->plist.iocb_envbuf.val_ptr,
				 dev->dev_description,
				 iocb->plist.iocb_envbuf.val_length);
			status = 0;
			break;
		}
	}

	return status;
}

static int bolt_cmd_dev_gethandle(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int i;
	char *devname;
	bolt_device_t *dev;

	devname = (char *)iocb->plist.iocb_buffer.buf_ptr;

	dev = bolt_finddev(devname);
	if (!dev)
		return BOLT_ERR_DEVNOTFOUND;

	for (i = 0; i < BOLT_MAX_HANDLE; i++) {
		ctx = bolt_handle_table[i];
		if (!ctx)
			continue;

		if (ctx->dev_dev &&
				(strcmp(devname,
					ctx->dev_dev->dev_fullname) == 0))
			break;
	}

	if (i >= BOLT_MAX_HANDLE)
		i = BOLT_ERR_NOHANDLES;
	else
		iocb->iocb_handle = i;

	return i;
}

static int bolt_cmd_dev_open(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int h;
	bolt_device_t *dev;
	char devname[64];
	int res;

	/*
	 * Get device name
	 */

	xstrncpy(devname, (char *)iocb->plist.iocb_buffer.buf_ptr,
		 sizeof(devname));

	/*
	 * Find device in device table
	 */
	dev = bolt_finddev(devname);
	if (!dev)
		return BOLT_ERR_DEVNOTFOUND;
	/*
	 * Fail if someone else already has the device open
	 */

	if (dev->dev_opencount > 0)
		return BOLT_ERR_DEVOPEN;

	/*
	 * Generate a new handle
	 */

	h = bolt_newhandle();
	if (h < 0)
		return BOLT_ERR_NOMEM;

	/*
	 * Allocate a context
	 */

	ctx = (bolt_devctx_t *) KMALLOC(sizeof(bolt_devctx_t), 0);
	if (ctx == NULL)
		return BOLT_ERR_NOMEM;

	/*
	 * Fill in the context
	 */

	ctx->dev_dev = dev;
	ctx->dev_softc = dev->dev_softc;
	ctx->dev_openinfo = NULL;

	/*
	 * Call driver's open func
	 */

	res = dev->dev_dispatch->dev_open(ctx);

	if (res != 0) {
		KFREE(ctx);
		return res;
	}

	/*
	 * Increment refcnt and save handle
	 */

	dev->dev_opencount++;
	bolt_handle_table[h] = ctx;
	iocb->iocb_handle = h;

	/*
	 * Success!
	 */

	return BOLT_OK;
}

static int bolt_cmd_dev_inpstat(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int status;

	status =
	    ctx->dev_dev->dev_dispatch->dev_inpstat(ctx,
						    &(iocb->
						      plist.iocb_inpstat));

	return status;
}

static int bolt_cmd_dev_read(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int status;

	status =
	    ctx->dev_dev->dev_dispatch->dev_read(ctx,
						 &(iocb->plist.iocb_buffer));

	return status;
}

static int bolt_cmd_dev_write(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int status;

	status =
	    ctx->dev_dev->dev_dispatch->dev_write(ctx,
						  &(iocb->plist.iocb_buffer));

	return status;
}

static int bolt_cmd_dev_ioctl(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int status;

	status =
	    ctx->dev_dev->dev_dispatch->dev_ioctl(ctx,
						  &(iocb->plist.iocb_buffer));

	return status;
}

static int bolt_cmd_dev_close(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	/*
	 * Call device close function
	 */

	ctx->dev_dev->dev_dispatch->dev_close(ctx);

	/*
	 * Decrement refcnt
	 */

	ctx->dev_dev->dev_opencount--;

	/*
	 * Wipe out handle
	 */

	bolt_handle_table[iocb->iocb_handle] = NULL;

	/*
	 * Release device context
	 */

	KFREE(ctx);

	return BOLT_OK;
}

static int bolt_cmd_dev_getinfo(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	bolt_device_t *dev;
	char devname[64];
	char *x;

	/*
	 * Get device name
	 */

	xstrncpy(devname, (char *)iocb->plist.iocb_buffer.buf_ptr,
		 sizeof(devname));

	/*
	 * Find device in device table
	 */
	x = strchr(devname, ':');
	if (x)
		*x = '\0';

	dev = bolt_finddev(devname);
	if (!dev)
		return BOLT_ERR_DEVNOTFOUND;

	/*
	 * Return device class
	 */

	iocb->plist.iocb_buffer.buf_devflags = dev->dev_class;

	return BOLT_OK;
}

static int bolt_cmd_env_enum(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int vallen, namelen, res;

	namelen = iocb->plist.iocb_envbuf.name_length;
	vallen = iocb->plist.iocb_envbuf.val_length;

	res = env_enum(iocb->plist.iocb_envbuf.enum_idx,
		       (char *)iocb->plist.iocb_envbuf.name_ptr,
		       &namelen,
		       (char *)iocb->plist.iocb_envbuf.val_ptr, &vallen);

	if (res < 0)
		return BOLT_ERR_ENVNOTFOUND;

	return BOLT_OK;
}

static int bolt_cmd_env_get(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	char *env;

	env = env_getenv((char *)iocb->plist.iocb_envbuf.name_ptr);

	if (env == NULL)
		return BOLT_ERR_ENVNOTFOUND;

	xstrncpy((char *)iocb->plist.iocb_envbuf.val_ptr,
		 env, iocb->plist.iocb_envbuf.val_length);

	return BOLT_OK;
}

static int bolt_cmd_env_set(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int res;
	int flg;

	flg = (iocb->iocb_flags & BOLT_FLG_ENV_PERMANENT) ?
	    ENV_FLG_NORMAL : ENV_FLG_BUILTIN;

	res = env_setenv((char *)iocb->plist.iocb_envbuf.name_ptr,
			 (char *)iocb->plist.iocb_envbuf.val_ptr, flg);

	if (res == 0) {
		if (iocb->iocb_flags & BOLT_FLG_ENV_PERMANENT)
			res = env_save();
	}

	if (res < 0)
		return res;

	return BOLT_OK;
}

static int bolt_cmd_env_del(bolt_devctx_t *ctx, bolt_iocb_t *iocb)
{
	int res;

	res = env_delenv((char *)iocb->plist.iocb_envbuf.name_ptr);

	return res;
}
