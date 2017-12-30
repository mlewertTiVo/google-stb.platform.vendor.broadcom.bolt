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

#include "bchp_sun_top_ctrl.h"

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_queue.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "device.h"
#include "error.h"
#include "timer.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define BOLT_MAX_DEVINST 64	/* max # of instances of devices */
static const unsigned int POLL_SLEEP_TIME = 20; /* 20 milliseconds */

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

/*
 * Our device list.
 */

queue_t bolt_devices = { &bolt_devices, &bolt_devices };

/*  *********************************************************************
    *  bolt_finddev(name)
    *
    *  Locate a device in the device list by its name and return
    *  a pointer to the device structure.
    *
    *  Input parameters:
    *      name - name of device, e.g., "uart0"
    *
    *  Return value:
    *      bolt_device_t pointer or NULL
    ********************************************************************* */

bolt_device_t *bolt_finddev(char *name)
{
	queue_t *qb;
	bolt_device_t *dev;

	for (qb = bolt_devices.q_next; qb != &bolt_devices; qb = qb->q_next) {
		dev = (bolt_device_t *) qb;
		if (strcmp(dev->dev_fullname, name) == 0)
			return dev;
	}

	return NULL;
}

/*  *********************************************************************
    *  bolt_device_reset()
    *
    *  Call all the "reset" methods in the devices on the device
    *  list.  Note that the methods get called even when the
    *  devices are closed!
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void bolt_device_reset(void)
{
	queue_t *qb;
	bolt_device_t *dev;

	for (qb = bolt_devices.q_next; qb != &bolt_devices; qb = qb->q_next) {
		dev = (bolt_device_t *) qb;

		if (dev->dev_dispatch->dev_reset)
			(*(dev->dev_dispatch->dev_reset)) (dev->dev_softc);
	}
}

/*  *********************************************************************
    *  __bolt_attach_idx(drv,idx,softc,bootinfo,description)
    *
    *  Add a device to the device list at a specific index.  This
    *  is mainly used for devices like SCSI disks or CD-ROMs so
    *  we can use an index that matches the target ID or LUN.
    *
    *  Input parameters:
    *      drv - driver structure (from the device driver module)
    *      idx - requested index (e.g., uartn where 'n' is the idx)
    *      softc - Unique information maintained for this device
    *      bootinfo - suffix for long form of the device name.  For
    *                 example, scsi0.3.1  might mean SCSI controller
    *                 0, device ID 3, LUN 1.  The bootinfo would be
    *                 "3.1"
    *      description - something nice to say for the devices command
    *      res - a pass-by-reference return parameter
    *              0 if device has already been added at this index
    *             <0 for other problems
    *              1 if we were successful.
    *
    *  Return value:
    *      NULL on error
    *      Otherwise, the name of the resulting device
    ********************************************************************* */

static const char *__bolt_attach_idx(bolt_driver_t *drv, int idx, void *softc,
		   const char *bootinfo, const char *description, int *res)
{
	char name[64];
	bolt_device_t *dev;

	xsprintf(name, "%s%d", drv->drv_bootname, idx);

	if (bootinfo) {
		strcat(name, ".");
		strcat(name, bootinfo);
	}

	if (bolt_finddev(name) != NULL) {
		*res = 0;
		return NULL;
	}

	dev = (bolt_device_t *) KMALLOC(sizeof(bolt_device_t), 0);
	if (!dev) {
		*res = -1;
		return NULL;
	}

	memset(dev, 0, sizeof(bolt_device_t));

	dev->dev_fullname = strdup(name);
	dev->dev_softc = softc;
	dev->dev_class = drv->drv_class;
	dev->dev_dispatch = drv->drv_dispatch;
	dev->dev_description = description ? strdup(description) : NULL;
	dev->dev_opencount = 0;

	q_enqueue(&bolt_devices, (queue_t *) dev);

	*res = 1; /* success */
	return dev->dev_fullname;
}


int bolt_detach(char *dev_fullname)
{
	bolt_device_t *dev;

	dev = bolt_finddev(dev_fullname);
	if (!dev)
		return BOLT_ERR_DEVNOTFOUND;

	q_dequeue((queue_t *)dev);

	if (dev->dev_fullname)
		KFREE(dev->dev_fullname);

	if (dev->dev_description)
		KFREE(dev->dev_description);

	KFREE(dev);
	return 0;
}

/*  *********************************************************************
    *  bolt_attach_idx(drv,idx,softc,bootinfo,description)
    *
    *  Add a device to the device list at a specific index.  This
    *  is mainly used for devices like SCSI disks or CD-ROMs so
    *  we can use an index that matches the target ID or LUN.
    *
    *  Input parameters:
    *      drv - driver structure (from the device driver module)
    *      idx - requested index (e.g., uartn where 'n' is the idx)
    *      softc - Unique information maintained for this device
    *      bootinfo - suffix for long form of the device name.  For
    *                 example, scsi0.3.1  might mean SCSI controller
    *                 0, device ID 3, LUN 1.  The bootinfo would be
    *                "3.1"
    *      description - something nice to say for the devices command
    *
    *  Return value:
    *      NULL on error
    *      Otherwise, the name of the resulting device
    ********************************************************************* */

const char *bolt_attach_idx(bolt_driver_t *drv, int idx, void *softc,
		   const char *bootinfo, const char *description)
{
	const char *name;
	int res;

	name = __bolt_attach_idx(drv, idx, softc, bootinfo, description, &res);
	if (res > 0)
		return name;
	return NULL;
}

/*  *********************************************************************
    *  bolt_attach(drv,softc,bootinfo,description
    *
    *  Add a device to the system.  This is a callback from the
    *  probe routine, and is used to actually add devices to BOLT's
    *  device list.
    *
    *  Input parameters:
    *      drv - driver structure (from the device driver module)
    *      softc - Unique information maintained for this device
    *      bootinfo - suffix for long form of the device name.  For
    *                 example, scsi0.3.1  might mean SCSI controller
    *                 0, device ID 3, LUN 1.  The bootinfo would be
    *                 "3.1"
    *      description - something nice to say for the devices command
    *
    *  Return value:
    *      NULL on error
    *      Otherwise, the name of the resulting device
    ********************************************************************* */

const char *bolt_attach(bolt_driver_t *drv, void *softc,
		const char *bootinfo, const char *description)
{
	int idx;
	int res;
	const char *name;

	/*
	 * Try device indicies 0..BOLT_MAX_DEVINST to assign a unique
	 * device name for this device.  This is a really braindead way to
	 * do this, but how many devices are we expecting anyway?
	 */

	for (idx = 0; idx < BOLT_MAX_DEVINST; idx++) {

		name = __bolt_attach_idx(drv, idx, softc, bootinfo,
				description, &res);
		if (res < 0)
			return NULL;	/* out of memory or other badness */
		if (res > 0)
			return name;	/* success! */
		/* otherwise, try again, slot is taken */
	}
	return NULL;
}

/*  *********************************************************************
    *  bolt_attach_init()
    *
    *  Initialize this module.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
void bolt_attach_init(void)
{
	q_init(&(bolt_devices));
}

/*  *********************************************************************
    *  bolt_device_name(ctx)
    *
    *  Given a device context, return a device name
    *
    *  Input parameters:
    *      ctx - context
    *
    *  Return value:
    *      name
    ********************************************************************* */

char *bolt_device_name(bolt_devctx_t *ctx)
{
	return ctx->dev_dev->dev_fullname;
}

/*  *********************************************************************
    *  bolt_waitdev(waittime, devdesc)
    *
    *  This routine is called to wait for a particular device to get
    *  initialized.
    *
    *  Input parameters:
    *       waittime - in millisecs the max wait time for device to initialize.
    *       devdesc - device description of the device.(Ex: USB-Ethernet)
    *
    *  Return value:
    *      pointer to structure bolt_device_t if device initialized successfully
    *      NULL otherwise.
    ********************************************************************* */

bolt_device_t *bolt_waitdev(int waittime, char *devdesc)
{
	queue_t *qb;
	bolt_device_t *dev;
	int idx;
	int cnt;

	cnt = waittime / POLL_SLEEP_TIME;
	for (idx = 0; idx < cnt; ++idx) {
		for (qb = bolt_devices.q_next; qb != &bolt_devices;
			qb = qb->q_next) {
			dev = (bolt_device_t *) qb;
			if (lib_strstr(dev->dev_description, devdesc))
				return dev;
		}
		bolt_msleep(POLL_SLEEP_TIME);
	}

	return NULL;
}
