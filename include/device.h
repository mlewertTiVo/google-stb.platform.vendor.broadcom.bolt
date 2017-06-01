/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <iocb.h>
#include <lib_queue.h>

#include <stdint.h>

typedef struct bolt_devdisp_s bolt_devdisp_t;

/*
 * The Device structure defines a particular instance of a device.  
 * They are generated as a result of calling the bolt_attach call.
 */

typedef struct bolt_device_s {
	queue_t dev_next;
	char *dev_fullname;
	void *dev_softc;
	int dev_class;
	const bolt_devdisp_t *dev_dispatch;
	int dev_opencount;
	char *dev_description;
} bolt_device_t;

/*
 * This is what gets returned from the OPEN call
 */
typedef struct bolt_devctx_s {
	bolt_device_t *dev_dev;
	void *dev_softc;
	void *dev_openinfo;
} bolt_devctx_t;

/*
 * This defines a given device class.  Even though there are
 * three identical MACs, there is only one of these.
 */

struct bolt_devdisp_s {
	int (*dev_open) (bolt_devctx_t * ctx);
	int (*dev_read) (bolt_devctx_t * ctx, iocb_buffer_t * buffer);
	int (*dev_inpstat) (bolt_devctx_t * ctx, iocb_inpstat_t * inpstat);
	int (*dev_write) (bolt_devctx_t * ctx, iocb_buffer_t * buffer);
	int (*dev_ioctl) (bolt_devctx_t * ctx, iocb_buffer_t * buffer);
	int (*dev_close) (bolt_devctx_t * ctx);
	void (*dev_poll) (bolt_devctx_t * ctx, int64_t ticks);
	void (*dev_reset) (void *softc);	/* called when device is closed, so no devctx_t */
};

typedef struct bolt_driver_s {
	char *drv_description;	/* Description of device for SHOW commands */
	char *drv_bootname;	/* Device's name prefix for open() */
	int drv_class;
	const bolt_devdisp_t *drv_dispatch;
	void (*drv_probe) (struct bolt_driver_s * drv,
			   unsigned long probe_a, unsigned long probe_b,
			   void *probe_ptr);
} bolt_driver_t;

char *bolt_device_name(bolt_devctx_t * ctx);

const char *bolt_attach(bolt_driver_t * devname, void *softc,
		const char *bootinfo, const char *description);

const char *bolt_attach_idx(bolt_driver_t * drv, int idx, void *softc,
		const char *bootinfo, const char *description);

int bolt_detach(char *dev_fullname);

bolt_device_t *bolt_finddev(char *name);
void bolt_attach_init(void);
#define bolt_add_device(devdescr,a,b,ptr) (devdescr)->drv_probe(devdescr,a,b,ptr)
void bolt_device_reset(void);

extern queue_t bolt_devices;

#endif /* __DEVICE_H__ */

