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

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "timer.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "error.h"
#include "console.h"
#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"
#include "env_subr.h"
#include "common.h"

#define STALL_TIMEOUT			500
#define STALL_RETRIES			10
#define DISK_OPEN_TIMEOUT		100
#define DISK_OPEN_RETRIES		50

/*  *********************************************************************
    *  USB Mass-Storage class Constants
    ********************************************************************* */

#define USBMASS_CBI_PROTOCOL	0
#define USBMASS_CBI_NOCOMPLETE_PROTOCOL 1
#define USBMASS_BULKONLY_PROTOCOL 0x50

#define USBMASS_SUBCLASS_RBC	0x01
#define USBMASS_SUBCLASS_SFF8020 0x02
#define USBMASS_SUBCLASS_QIC157	0x03
#define USBMASS_SUBCLASS_UFI	0x04
#define USBMASS_SUBCLASS_SFF8070 0x05
#define USBMASS_SUBCLASS_SCSI	0x06

#define USBMASS_CSW_PASS	0x00
#define USBMASS_CSW_FAILED	0x01
#define USBMASS_CSW_PHASEERR	0x02

#define USBMASS_CBW_SIGNATURE	0x43425355
#define USBMASS_CSW_SIGNATURE	0x53425355

/*  *********************************************************************
    *  USB Mass-Storage class Structures
    ********************************************************************* */

typedef struct usbmass_cbw_s {
	uint8_t dCBWSignature0, dCBWSignature1, dCBWSignature2, dCBWSignature3;
	uint8_t dCBWTag0, dCBWTag1, dCBWTag2, dCBWTag3;
	uint8_t dCBWDataTransferLength0, dCBWDataTransferLength1,
	    dCBWDataTransferLength2, dCBWDataTransferLength3;
	uint8_t bmCBWFlags;
	uint8_t bCBWLUN;
	uint8_t bCBWCBLength;
	uint8_t CBWCB[16];
} usbmass_cbw_t;

typedef struct usbmass_csw_s {
	uint8_t dCSWSignature0, dCSWSignature1, dCSWSignature2, dCSWSignature3;
	uint8_t dCSWTag0, dCSWTag1, dCSWTag2, dCSWTag3;
	uint8_t dCSWDataResidue0, dCSWDataResidue1, dCSWDataResidue2,
	    dCSWDataResidue3;
	uint8_t bCSWStatus;
} usbmass_csw_t;

#define GETCBWFIELD(s, f)	((uint32_t)((s)->f##0) | \
				((uint32_t)((s)->f##1) << 8) | \
				((uint32_t)((s)->f##2) << 16) | \
				((uint32_t)((s)->f##3) << 24))

#define PUTCBWFIELD(s, f, v) do { \
					(s)->f##0 = (v & 0xFF); \
					(s)->f##1 = ((v)>>8 & 0xFF); \
					(s)->f##2 = ((v)>>16 & 0xFF); \
					(s)->f##3 = ((v)>>24 & 0xFF); \
				} while (0)

int usbmass_request_sense(usbdev_t *dev);

/*  *********************************************************************
    *  Linkage to BOLT
    ********************************************************************* */

/*
 * Softc for the BOLT side of the disk driver.
 */
#define MAX_SECTORSIZE 2048
typedef struct usbdisk_s {
	uint32_t usbdisk_sectorsize;
	uint32_t usbdisk_ttlsect;
	uint32_t usbdisk_devtype;
	uint32_t usbdisk_capacity;
	char vend_id[9];
	char prod_id[17];
	int usbdisk_unit;
} usbdisk_t;

/*
 * This table points at the currently configured USB disk
 * devices.  This lets us leave the BOLT half of the driver lying
 * around while the USB devices come and go.  We use the unit number
 * from the original BOLT attach to index this table, and devices
 * that are not present are "not ready."
 */

#define USBDISK_MAXUNITS	4
static usbdev_t *usbdisk_units[USBDISK_MAXUNITS];

/*
 * BOLT device driver routine forwards
 */

static void usbdisk_probe(bolt_driver_t *drv,
			  unsigned long probe_a, unsigned long probe_b,
			  void *probe_ptr);

static int usbdisk_open(bolt_devctx_t *ctx);
static int usbdisk_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usbdisk_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int usbdisk_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usbdisk_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int usbdisk_close(bolt_devctx_t *ctx);

/*
 * BOLT device driver descriptor
 */

static const bolt_devdisp_t usbdisk_dispatch = {
	usbdisk_open,
	usbdisk_read,
	usbdisk_inpstat,
	usbdisk_write,
	usbdisk_ioctl,
	usbdisk_close,
	NULL,
	NULL
};

const bolt_driver_t usb_disk = {
	"USB Disk",
	"usbdisk",
	BOLT_DEV_DISK,
	&usbdisk_dispatch,
	usbdisk_probe
};

/*  *********************************************************************
    *  Forward Definitions
    ********************************************************************* */

static int usbmass_attach(usbdev_t *dev, usb_driver_t *drv);
static int usbmass_detach(usbdev_t *dev);

/*  *********************************************************************
    *  Structures
    ********************************************************************* */

typedef struct usbmass_softc_s {
	int umass_inpipe;
	int umass_outpipe;
	int umass_devtype;
	uint32_t umass_curtag;
	int umass_unit;
} usbmass_softc_t;

usb_driver_t usbmass_driver = {
	"Mass-Storage Device",
	usbmass_attach,
	usbmass_detach,
	NULL
};

usbdev_t *usbmass_dev = NULL;	/* XX hack for testing only */

/*  *********************************************************************
    *  usbmass_mass_storage_reset(dev,ifc)
    *
    *  Do a bulk-only mass-storage reset.
    *
    *  Input parameters:
    *	dev - device to reset
    *      ifc - interface number to reset (bInterfaceNum)
    *
    *  Return value:
    *	status
    ********************************************************************* */

#define usbmass_mass_storage_reset(dev, ifc) \
	usb_simple_request(dev, 0x21, 0xFF, ifc, 0)

#if 0
/*  *********************************************************************
    *  usbmass_get_max_lun(dev,lunp)
    *
    *  Get maximum LUN from device
    *
    *  Input parameters:
    *	dev - device to reset
    *      lunp - pointer to int to receive max lun
    *
    *  Return value:
    *	status
    ********************************************************************* */

static int usbmass_get_max_lun(usbdev_t *dev, int *lunp)
{
	uint8_t buf = 0;
	int res;

	res = usb_std_request(dev, 0xA1, 0xFE, 0, 0, &buf, 1);

	if (res < 0)
		return res;

	if (lunp)
		*lunp = (int)buf;
	return 0;
}

#endif

/*  *********************************************************************
    *  usbmass_stall_recovery(dev)
    *
    *  Do whatever it takes to unstick a stalled mass-storage device.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usbmass_stall_recovery(usbdev_t *dev)
{
	usbmass_softc_t *softc;
	uint8_t *cbwcsw;
	usbmass_csw_t *csw;
	usbreq_t *ur;

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	csw = (usbmass_csw_t *) cbwcsw;
	softc = (usbmass_softc_t *) dev->ud_private;

	usb_clear_stall(dev, softc->umass_inpipe);

	/*
	 * Get the Status of the stalled CBW
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	(void) usb_sync_request(ur);
	usb_free_request(ur);
	KFREE(cbwcsw);
}

/*  *********************************************************************
    *  usbmass_request_sense(dev)
    *
    *  Reads device sense status.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	status
    ********************************************************************* */

#define RS_ALLOC_LEN	18
int usbmass_request_sense(usbdev_t *dev)
{
	uint8_t *cbwcsw;
	uint8_t *sector;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res;

	softc = (usbmass_softc_t *) dev->ud_private;

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	sector = KMALLOC(64, DMA_BUF_ALIGN);

	memset(sector, 0, 64);

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, RS_ALLOC_LEN);
	cbw->bmCBWFlags = 0x80;	/* IN */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 12;
	cbw->CBWCB[0] = 0x3;	/* REQUEST SENSE */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = 0;
	cbw->CBWCB[3] = 0;
	cbw->CBWCB[4] = RS_ALLOC_LEN;	/* allocation length */
	cbw->CBWCB[5] = 0;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = 0;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the data
	 */

	memset(sector, 0, RS_ALLOC_LEN);
	ur = usb_make_request(dev, softc->umass_inpipe, sector,
			      RS_ALLOC_LEN, UR_FLAG_IN | UR_FLAG_SHORTOK);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);

done:
	KFREE(cbwcsw);
	KFREE(sector);

	return res;

}

/*  *********************************************************************
    *  usbmass_read_sector(dev,sectornum,seccnt,ssize,buffer)
    *
    *  Reads a sector from the device.
    *
    *  Input parameters:
    *	dev - usb device
    *	sectornum - sector number to read
    *	seccnt - count of sectors to read
    *	buffer - place to put sector we read
    *
    *  Return value:
    *	status
    ********************************************************************* */


int usbmass_read_sector(usbdev_t *dev, uint32_t sectornum, uint32_t seccnt,
			uint32_t ssize, uint8_t *buffer)
{
	uint8_t *cbwcsw;
	uint8_t *sector;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res, rlen, retries = STALL_RETRIES;

	softc = (usbmass_softc_t *) dev->ud_private;
	rlen = ssize * seccnt;	/* total byte count */

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	sector = KMALLOC(rlen, ((DMA_BUF_ALIGN > 512) ? DMA_BUF_ALIGN : 512));

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

retry_on_stall:

	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, rlen);
	cbw->bmCBWFlags = 0x80;	/* IN */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 10;
	cbw->CBWCB[0] = 0x28;	/* READ */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = (sectornum >> 24) & 0xFF; /* LUN 0 & MSB's of sector */
	cbw->CBWCB[3] = (sectornum >> 16) & 0xFF;
	cbw->CBWCB[4] = (sectornum >> 8) & 0xFF;
	cbw->CBWCB[5] = (sectornum >> 0) & 0xFF;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = seccnt;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the data
	 */

	ur = usb_make_request(dev, softc->umass_inpipe, sector,
			      rlen, UR_FLAG_IN | UR_FLAG_SHORTOK);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;
	else if (res == 4) {
		usbmass_stall_recovery(dev);
		if (retries-- == 0) {
			xprintf("usbmass: Drive stalling on Read\n");
			res = -1;
			goto done;
		}

		/* SK...Some device controllers need time.  */
		usb_delay_ms(NULL, STALL_TIMEOUT);

		/* So, delay and retry stalled transaction */
		goto retry_on_stall;
	}
	memcpy(buffer, sector, rlen);

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

#if 0
	printf("CSW: Signature=%08X  Tag=%08X  Residue=%08X  Status=%02X\n",
	       GETCBWFIELD(csw, dCSWSignature),
	       GETCBWFIELD(csw, dCSWTag),
	       GETCBWFIELD(csw, dCSWDataResidue), csw->bCSWStatus);
#endif
	res = (csw->bCSWStatus == USBMASS_CSW_PASS) ? 0 : -1;

done:
	KFREE(cbwcsw);
	KFREE(sector);

	return res;
}

/*  *********************************************************************
    *  usbmass_write_sector(dev,sectornum,seccnt,ssize, buffer)
    *
    *  Writes a sector to the device
    *
    *  Input parameters:
    *	dev - usb device
    *	sectornum - sector number to write
    *	seccnt - count of sectors to write
    *	buffer - place to get sector to write
    *
    *  Return value:
    *	status
    ********************************************************************* */

static int usbmass_write_sector(usbdev_t *dev, uint32_t sectornum,
				uint32_t seccnt, uint32_t ssize,
				uint8_t *buffer)
{
	uint8_t *cbwcsw;
	uint8_t *sector;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res, wlen;

	softc = (usbmass_softc_t *) dev->ud_private;
	wlen = ssize * seccnt;	/* total byte count */

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	sector = KMALLOC(wlen, ((DMA_BUF_ALIGN > 512) ? DMA_BUF_ALIGN : 512));
	memcpy(sector, buffer, wlen);

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, wlen);
	cbw->bmCBWFlags = 0x00;	/* OUT */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 10;
	cbw->CBWCB[0] = 0x2A;	/* WRITE */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = (sectornum >> 24) & 0xFF; /* LUN 0 & MSB's of sector */
	cbw->CBWCB[3] = (sectornum >> 16) & 0xFF;
	cbw->CBWCB[4] = (sectornum >> 8) & 0xFF;
	cbw->CBWCB[5] = (sectornum >> 0) & 0xFF;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = seccnt;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Send the data
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, sector,
			      wlen, UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

#if 0
	printf("CSW: Signature=%08X  Tag=%08X  Residue=%08X  Status=%02X\n",
	       GETCBWFIELD(csw, dCSWSignature),
	       GETCBWFIELD(csw, dCSWTag),
	       GETCBWFIELD(csw, dCSWDataResidue), csw->bCSWStatus);
#endif
	res = (csw->bCSWStatus == USBMASS_CSW_PASS) ? 0 : -1;

done:
	KFREE(cbwcsw);
	KFREE(sector);

	return res;
}

/*  *********************************************************************
    *  usbmass_read_capacity(dev,size)
    *
    *  Reads device capacity.
    *
    *  Input parameters:
    *	dev - usb device
    *	size - place to put the capacity we read
    *
    *  Return value:
    *	status
    ********************************************************************* */

#define RC_XFER_LEN		8

int usbmass_read_capacity(usbdev_t *dev, uint32_t *dsize, uint32_t *ssize)
{
	uint8_t *cbwcsw;
	uint8_t *sector;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res, retries = STALL_RETRIES;

	softc = (usbmass_softc_t *) dev->ud_private;

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	sector = KMALLOC(64, DMA_BUF_ALIGN);

	memset(sector, 0, 64);

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

retry_on_stall:
	*ssize = 0;
	*dsize = 0;

	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, RC_XFER_LEN);
	cbw->bmCBWFlags = 0x80;	/* IN */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 10;
	cbw->CBWCB[0] = 0x25;	/* READ CAPACITY */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = 0;
	cbw->CBWCB[3] = 0;
	cbw->CBWCB[4] = 0;
	cbw->CBWCB[5] = 0;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = 0;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the data
	 */

	ur = usb_make_request(dev, softc->umass_inpipe, sector,
			      RC_XFER_LEN, UR_FLAG_IN | UR_FLAG_SHORTOK);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;
	else if (res == 4) {
		usbmass_stall_recovery(dev);
		if (retries-- == 0) {
			xprintf("usbmass: Drive stalling on Read Capacity\n");
			res = -1;
			goto done;
		}

		/* SK...Some device controllers need time.  */
		usb_delay_ms(NULL, STALL_TIMEOUT);

		/* So, delay and retry stalled transaction */
		goto retry_on_stall;
	}

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	*dsize = ((((uint32_t) sector[0]) << 24) |
		  (((uint32_t) sector[1]) << 16) |
		  (((uint32_t) sector[2]) << 8) |
		  (((uint32_t) sector[3]) << 0)) + 1;
	*ssize = (((uint32_t) sector[4]) << 24) |
	    (((uint32_t) sector[5]) << 16) |
	    (((uint32_t) sector[6]) << 8) | (((uint32_t) sector[7]) << 0);

done:
	KFREE(cbwcsw);
	KFREE(sector);

	return res;

}

/*  *********************************************************************
    *  usbmass_test_unit_ready(dev)
    *
    *  Reads device capacity.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	status - on error, negative; otherwise, unit ready (1) or not (0)
    ********************************************************************* */

int usbmass_test_unit_ready(usbdev_t *dev)
{
	uint8_t *cbwcsw;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res;

	softc = (usbmass_softc_t *) dev->ud_private;

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, 0);
	cbw->bmCBWFlags = 0x80;	/* IN */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 6;
	cbw->CBWCB[0] = 0x00;	/* TEST UNIT READY */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = 0;
	cbw->CBWCB[3] = 0;
	cbw->CBWCB[4] = 0;
	cbw->CBWCB[5] = 0;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = 0;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	res = (csw->bCSWStatus == 0);

done:
	KFREE(cbwcsw);

	return res;
}

/*  *********************************************************************
    *  usbmass_inquiry(dev)
    *
    *  Reads device capacity.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	status - Unit ready (1) or not (0)
    ********************************************************************* */

#define INQ_ALLOC_LEN	36

int usbmass_inquiry(usbdev_t *dev, char *vend_id, char *prod_id)
{
	uint8_t *cbwcsw;
	uint8_t *sector;
	usbmass_cbw_t *cbw;
	usbmass_csw_t *csw;
	usbreq_t *ur;
	usbmass_softc_t *softc;
	int res, retries = STALL_RETRIES;

	softc = (usbmass_softc_t *) dev->ud_private;

	cbwcsw = KMALLOC(64, DMA_BUF_ALIGN);
	sector = KMALLOC(64, DMA_BUF_ALIGN);

	memset(sector, 0, 64);

	cbw = (usbmass_cbw_t *) cbwcsw;
	csw = (usbmass_csw_t *) cbwcsw;

retry_on_stall:
	/*
	 * Fill in the fields of the CBW
	 */

	PUTCBWFIELD(cbw, dCBWSignature, USBMASS_CBW_SIGNATURE);
	PUTCBWFIELD(cbw, dCBWTag, softc->umass_curtag);
	PUTCBWFIELD(cbw, dCBWDataTransferLength, INQ_ALLOC_LEN);
	cbw->bmCBWFlags = 0x80;	/* IN */
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = 6;
	cbw->CBWCB[0] = 0x12;	/* INQUIRY */
	cbw->CBWCB[1] = 0;
	cbw->CBWCB[2] = 0;
	cbw->CBWCB[3] = 0;
	cbw->CBWCB[4] = INQ_ALLOC_LEN;	/* allocation length */
	cbw->CBWCB[5] = 0;
	cbw->CBWCB[6] = 0;
	cbw->CBWCB[7] = 0;
	cbw->CBWCB[8] = 0;
	cbw->CBWCB[9] = 0;

	softc->umass_curtag++;

	/*
	 * Send the CBW
	 */

	ur = usb_make_request(dev, softc->umass_outpipe, (uint8_t *) cbw,
			      sizeof(usbmass_cbw_t), UR_FLAG_OUT);
	res = usb_sync_request(ur);
	usb_free_request(ur);

	/*
	 * Get the data
	 */

	ur = usb_make_request(dev, softc->umass_inpipe, sector,
			      INQ_ALLOC_LEN, UR_FLAG_IN | UR_FLAG_SHORTOK);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;
	else if (res == 4) {
		usbmass_stall_recovery(dev);
		if (retries-- == 0) {
			xprintf("usbmass: Drive stalling on Inquiry\n");
			res = -1;
			goto done;
		}

		/* SK...Some device controllers need time.  */
		usb_delay_ms(NULL, STALL_TIMEOUT);

		/* So, delay and retry stalled transaction */
		goto retry_on_stall;
	}

	/*
	 * Get the Status
	 */

	memset(csw, 0, sizeof(usbmass_csw_t));
	ur = usb_make_request(dev, softc->umass_inpipe, (uint8_t *) csw,
			      sizeof(usbmass_csw_t), UR_FLAG_IN);
	res = usb_sync_request(ur);
	usb_free_request(ur);
	if (res < 0)
		goto done;

	res = (csw->bCSWStatus == 0);
	if (res) {
		memcpy(vend_id, &sector[8], 8);
		memcpy(prod_id, &sector[16], 16);
	}

done:
	KFREE(cbwcsw);
	KFREE(sector);

	return res;
}

/*  *********************************************************************
    *  usbmass_attach(dev,drv)
    *
    *  This routine is called when the bus scan stuff finds a mass-storage
    *  device.  We finish up the initialization by configuring the
    *  device and allocating our softc here.
    *
    *  Input parameters:
    *	dev - usb device, in the "addressed" state.
    *	drv - the driver table entry that matched
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbmass_attach(usbdev_t *dev, usb_driver_t *drv)
{
	usb_config_descr_t *cfgdscr = dev->ud_cfgdescr;
	usb_endpoint_descr_t *epdscr;
	usb_endpoint_descr_t *indscr = NULL;
	usb_endpoint_descr_t *outdscr = NULL;
	usb_interface_descr_t *ifdscr;
	usbmass_softc_t *softc;
	int idx;

	dev->ud_drv = drv;

	softc = KMALLOC(sizeof(usbmass_softc_t), DMA_BUF_ALIGN);
	memset(softc, 0, sizeof(usbmass_softc_t));
	dev->ud_private = softc;

	ifdscr = usb_find_cfg_descr(dev, USB_INTERFACE_DESCRIPTOR_TYPE, 0);

	if (ifdscr == NULL)
		return -1;

	if (((ifdscr->bInterfaceSubClass != USBMASS_SUBCLASS_SCSI) &&
	     (ifdscr->bInterfaceSubClass != USBMASS_SUBCLASS_SFF8070)) ||
	    (ifdscr->bInterfaceProtocol != USBMASS_BULKONLY_PROTOCOL)) {
		console_log
		    ("USBMASS: Do not understand devices with SubClass 0x%02X, Protocol 0x%02X",
		     ifdscr->bInterfaceSubClass, ifdscr->bInterfaceProtocol);
		return -1;
	}

	for (idx = 0; idx < 2; idx++) {
		epdscr =
		    usb_find_cfg_descr(dev, USB_ENDPOINT_DESCRIPTOR_TYPE, idx);

		if (USB_ENDPOINT_DIR_OUT(epdscr->bEndpointAddress))
			outdscr = epdscr;
		else
			indscr = epdscr;
	}

	if (!indscr || !outdscr) {
		/*
		 * Could not get descriptors, something is very wrong.
		 * Leave device addressed but not configured.
		 */
		return -1;
	}

	/*
	 * Choose the standard configuration.
	 */

	usb_set_configuration(dev, cfgdscr->bConfigurationValue);

	/*
	 * Open the pipes.
	 */

	softc->umass_inpipe = usb_open_pipe(dev, indscr);
	softc->umass_outpipe = usb_open_pipe(dev, outdscr);
	softc->umass_curtag = 0x12345678;

	/*
	 * Save pointer in global unit table so we can
	 * match BOLT devices up with USB ones
	 */

	softc->umass_unit = -1;
	for (idx = 0; idx < USBDISK_MAXUNITS; idx++) {
		if (usbdisk_units[idx] == NULL) {
			softc->umass_unit = idx;
			usbdisk_units[idx] = dev;
			break;
		}
	}

	if (idx == USBDISK_MAXUNITS) {
		console_log("USBMASS: Cannot support more than %d drives", idx);
		return -1;
	}

	usbdisk_probe((bolt_driver_t *) &usb_disk, idx, 0, 0);

	console_log("USBMASS: Unit %d connected", softc->umass_unit);

	usbmass_dev = dev;

	return 0;
}

/*  *********************************************************************
    *  usbmass_detach(dev)
    *
    *  This routine is called when the bus scanner notices that
    *  this device has been removed from the system.  We should
    *  do any cleanup that is required.  The pending requests
    *  will be cancelled automagically.
    *
    *  Input parameters:
    *	dev - usb device
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbmass_detach(usbdev_t *dev)
{
	usbmass_softc_t *softc = (usbmass_softc_t *)dev->ud_private;
	char fullname[16];

	xsprintf(fullname, "%s%d", usb_disk.drv_bootname, softc->umass_unit);

	if ((softc->umass_unit < 0) ||
		(softc->umass_unit >= USBDISK_MAXUNITS)) {
		console_log("USBMASS: unexpected device! USB unit [%s] %d ",
			fullname, softc->umass_unit);
		return BOLT_ERR_DEVNOTFOUND;
	}

	console_log("USBMASS: USB unit [%s] %d disconnected",
		fullname, softc->umass_unit);

	usbdisk_units[softc->umass_unit] = NULL;
	KFREE(softc);
	dev->ud_private = NULL;

	return bolt_detach(fullname);
}

/*  *********************************************************************
    *  usbdisk_sectorshift(size)
    *
    *  Given a sector size, return log2(size).  We cheat; this is
    *  only needed for 2048 and 512-byte sectors.
    *  Explicitly using shifts and masks in sector number calculations
    *  helps on 32-bit-only platforms, since we probably won't need
    *  a helper library.
    *
    *  Input parameters:
    *	size - sector size
    *
    *  Return value:
    *	# of bits to shift
    ********************************************************************* */

#define usbdisk_sectorshift(size) (((size) == 2048) ? 11 : 9)

/*  *********************************************************************
    *  usbdisk_probe(drv,probe_a,probe_b,probe_ptr)
    *
    *  Our probe routine.  Attach an empty USB disk device to the firmware.
    *
    *  Input parameters:
    *	drv - driver structure
    *	probe_a - not used
    *	probe_b - not used
    *	probe_ptr - not used
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void usbdisk_probe(bolt_driver_t *drv,
			  unsigned long probe_a, unsigned long probe_b,
			  void *probe_ptr)
{
	usbdisk_t *softc;
	char descr[128];

	softc = (usbdisk_t *) KMALLOC(sizeof(usbdisk_t), DMA_BUF_ALIGN);

	memset(softc, 0, sizeof(usbdisk_t));

	softc->usbdisk_sectorsize = 512;
	softc->usbdisk_devtype = BLOCK_DEVTYPE_DISK;
	softc->usbdisk_ttlsect = 0;	/* not calculated yet */
	softc->usbdisk_unit = (int)probe_a;

	xsprintf(descr, "USB Disk unit %d", (int)probe_a);

	bolt_attach(drv, softc, NULL, descr);
}

/*  *********************************************************************
    *  usbdisk_open(ctx)
    *
    *  Process the BOLT OPEN call for this device.  For IDE disks,
    *  the device is reset and identified, and the geometry is
    *  determined.
    *
    *  Input parameters:
    *	ctx - device context
    *
    *  Return value:
    *	0 if ok, else error code
    ********************************************************************* */

static int usbdisk_open(bolt_devctx_t *ctx)
{
	usbdisk_t *softc = ctx->dev_softc;
	usbdev_t *dev = usbdisk_units[softc->usbdisk_unit];
	uint32_t dsize, ssize;
	int res, retries = DISK_OPEN_RETRIES;

	if (!dev)
		return BOLT_ERR_NOTREADY;

	res = usbmass_inquiry(dev, softc->vend_id, softc->prod_id);
	if (res < 0)
		return res;

	/* SK...Some USB disks need startup time...otherwise they stall forever!
	So far (2/15/09), winning combination seems to be loop of RequestSense
	and TesUnitReady! */
	while (retries-- > 0) {
		res = usbmass_request_sense(dev);
		if (res < 0)
			return res;
		res = usbmass_test_unit_ready(dev);
		if (res > 0) /* success */
			break;
		if (res < 0) /* error */
			return res;
		usb_delay_ms(NULL, DISK_OPEN_TIMEOUT);
	}

	if (retries > 0) {
		res = usbmass_read_capacity(dev, &dsize, &ssize);
		if (res < 0)
			return res;
		if ((ssize != 512) && (ssize != 2048)) {
			xprintf("*** ERROR: Cannot handle drive's");
			xprintf(" logical block size of %d\n", ssize);
			return BOLT_ERR_IOERR;
		}
		softc->usbdisk_capacity = dsize;	/* logical blocks */
		softc->usbdisk_sectorsize = ssize;	/* logical block size */

		/* in MB...denominator expression due to integer math */
		dsize = dsize / ((1000 * 1000) / ssize);
		if (env_getenv("USBMASS_INFO"))
			xprintf("*** %s %s  (%d MB, lbs=%d) ***\n",
				softc->vend_id, softc->prod_id, dsize, ssize);
	} else {
		xprintf("usbmass: Drive open timed out\n");
		return BOLT_ERR_NOTREADY;
	}

	softc->usbdisk_ttlsect = dsize;

	return 0;
}

/*  *********************************************************************
    *  usbdisk_read(ctx,buffer)
    *
    *  Process a BOLT READ command for the IDE device.  This is
    *  more complex than it looks, since BOLT offsets are byte offsets
    *  and we may need to read partial sectors.
    *
    *  Input parameters:
    *	ctx - device context
    *	buffer - buffer descriptor
    *
    *  Return value:
    *	number of bytes read, or <0 if an error occured
    ********************************************************************* */

static int usbdisk_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usbdisk_t *softc = ctx->dev_softc;
	usbdev_t *dev = usbdisk_units[softc->usbdisk_unit];
	unsigned char *bptr;
	unsigned int blen;
	int res = 0;
	unsigned int amtcopy;
	uint64_t lba;
	uint64_t offset;
	unsigned char sector[MAX_SECTORSIZE];
	int sectorshift;

	if (!dev)
		return BOLT_ERR_NOTREADY;

	sectorshift = usbdisk_sectorshift(softc->usbdisk_sectorsize);

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;
	offset = buffer->buf_offset;

	if (!IS_ALIGNED(offset, softc->usbdisk_sectorsize)) {
		lba = (offset >> sectorshift);
		res = usbmass_read_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		amtcopy =
		    softc->usbdisk_sectorsize -
		    (offset & (softc->usbdisk_sectorsize - 1));
		if (amtcopy > blen)
			amtcopy = blen;
		memcpy(bptr, &sector[offset & (softc->usbdisk_sectorsize - 1)],
		       amtcopy);
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen >= softc->usbdisk_sectorsize) {
		int seccnt;

		lba = (offset >> sectorshift);
		seccnt = (blen >> sectorshift);

		res = usbmass_read_sector(dev, lba, seccnt,
			softc->usbdisk_sectorsize, bptr);
		if (res < 0)
			goto out;

		amtcopy = seccnt << sectorshift;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen) {
		lba = (offset >> sectorshift);
		res = usbmass_read_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		amtcopy = blen;
		memcpy(bptr, sector, amtcopy);
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

out:
	buffer->buf_retlen = bptr - buffer->buf_ptr;

	return res;
}

/*  *********************************************************************
    *  usbdisk_inpstat(ctx,inpstat)
    *
    *  Test input status for the IDE disk.  Disks are always ready
    *  to read.
    *
    *  Input parameters:
    *	ctx - device context
    *	inpstat - input status structure
    *
    *  Return value:
    *	0
    ********************************************************************* */

static int usbdisk_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	/* usbdisk_t *softc = ctx->dev_softc; */

	inpstat->inp_status = 1;
	return 0;
}

/*  *********************************************************************
    *  usbdisk_write(ctx,buffer)
    *
    *  Process a BOLT WRITE command for the IDE device.  If the write
    *  involves partial sectors, the affected sectors are read first
    *  and the changes are merged in.
    *
    *  Input parameters:
    *	ctx - device context
    *	buffer - buffer descriptor
    *
    *  Return value:
    *	number of bytes write, or <0 if an error occured
    ********************************************************************* */

static int usbdisk_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usbdisk_t *softc = ctx->dev_softc;
	usbdev_t *dev = usbdisk_units[softc->usbdisk_unit];
	unsigned char *bptr;
	unsigned int blen;
	int res = 0;
	unsigned int amtcopy;
	uint64_t offset;
	uint64_t lba;
	unsigned char sector[MAX_SECTORSIZE];
	int sectorshift;
	int seccnt;

	if (!dev)
		return BOLT_ERR_NOTREADY;

	sectorshift = usbdisk_sectorshift(softc->usbdisk_sectorsize);

	bptr = buffer->buf_ptr;
	blen = buffer->buf_length;
	offset = buffer->buf_offset;

	if (!IS_ALIGNED(offset, softc->usbdisk_sectorsize)) {
		lba = (offset >> sectorshift);
		res = usbmass_read_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		amtcopy =
		    softc->usbdisk_sectorsize -
		    (offset & (softc->usbdisk_sectorsize - 1));
		if (amtcopy > blen)
			amtcopy = blen;
		memcpy(&sector[offset & (softc->usbdisk_sectorsize - 1)], bptr,
		       amtcopy);
		res = usbmass_write_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen >= softc->usbdisk_sectorsize) {
		lba = (offset >> sectorshift);
		seccnt = (blen >> sectorshift);

		res = usbmass_write_sector(dev, lba, seccnt,
			softc->usbdisk_sectorsize, bptr);
		if (res < 0)
			goto out;

		amtcopy = seccnt << sectorshift;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

	if (blen) {
		lba = (offset >> sectorshift);
		res = usbmass_read_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		amtcopy = blen;
		memcpy(sector, bptr, amtcopy);
		res = usbmass_write_sector(dev, lba, 1,
			softc->usbdisk_sectorsize, sector);
		if (res < 0)
			goto out;
		bptr += amtcopy;
		offset += amtcopy;
		blen -= amtcopy;
	}

out:
	buffer->buf_retlen = bptr - buffer->buf_ptr;

	return res;
}

/*  *********************************************************************
    *  usbdisk_ioctl(ctx,buffer)
    *
    *  Process device I/O control requests for the IDE device.
    *
    *  Input parameters:
    *	ctx - device context
    *	buffer - buffer descriptor
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int usbdisk_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	usbdisk_t *softc = ctx->dev_softc;
	unsigned int *info = (unsigned int *)buffer->buf_ptr;
	unsigned long long *linfo = (unsigned long long *)buffer->buf_ptr;
	blockdev_info_t *devinfo;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_BLOCK_GETBLOCKSIZE:
		*info = softc->usbdisk_sectorsize;
		break;
	case IOCTL_BLOCK_GETTOTALBLOCKS:
		*linfo = softc->usbdisk_ttlsect;
		break;
	case IOCTL_BLOCK_GETDEVTYPE:
		devinfo = (blockdev_info_t *) buffer->buf_ptr;
		devinfo->blkdev_totalblocks = softc->usbdisk_ttlsect;
		devinfo->blkdev_blocksize = softc->usbdisk_sectorsize;
		devinfo->blkdev_devtype = softc->usbdisk_devtype;
		break;
	default:
		return -1;
	}

	return 0;
}

/*  *********************************************************************
    *  usbdisk_close(ctx)
    *
    *  Close the I/O device.
    *
    *  Input parameters:
    *	ctx - device context
    *
    *  Return value:
    *	0 if ok, else error code
    ********************************************************************* */

static int usbdisk_close(bolt_devctx_t *ctx)
{
	/* usbdisk_t *softc = ctx->dev_softc; */

	return 0;
}
