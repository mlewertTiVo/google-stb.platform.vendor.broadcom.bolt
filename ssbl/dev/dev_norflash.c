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
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "lib_physio.h"
#include "iocb.h"
#include "device.h"
#include "ioctl.h"
#include "error.h"
#include "common.h"

#include "dev_norflash.h"
#include "bchp_ebi.h"
#include "flash.h"
#include "cfi.h"

#define REG_U16(x)	(PHYSIO_MEM_U16(x))
#define REG_U8(x)	(PHYSIO_MEM_U8(x))

/*#define _NORFLASH_DEBUG_ */
static uint8_t *flash_sector_buffer;

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static void flashdrv_probe(bolt_driver_t *drv,
			   unsigned long probe_a, unsigned long probe_b,
			   void *probe_ptr);

static int flashdrv_open(bolt_devctx_t *ctx);
static int flashdrv_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int flashdrv_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int flashdrv_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int flashdrv_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int flashdrv_close(bolt_devctx_t *ctx);

static void flash_engine(flashdev_t *softc);

/*  *********************************************************************
    *  Device dispatch
    ********************************************************************* */

static const bolt_devdisp_t flashdrv_dispatch = {
	flashdrv_open,
	flashdrv_read,
	flashdrv_inpstat,
	flashdrv_write,
	flashdrv_ioctl,
	flashdrv_close,
	NULL,
	NULL
};

const bolt_driver_t norflashdrv = {
	"CFI flash",
	"flash",
	BOLT_DEV_FLASH,
	&flashdrv_dispatch,
	flashdrv_probe
};

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int flash_sector_query(flashdev_t *softc, flash_sector_t *sector);

/*  *********************************************************************
    *  FLASH_OP_BEGIN(softc)
    *
    *  Reset the pointer to the flash operations so that we can
    *  begin filling in new instructions to execute
    *
    *  Input parameters:
    *	   softc - our softc.
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

#define flash_op_begin(softc)  (softc->fd_iptr = 0)

/*  *********************************************************************
    *  FLASH_OP_ADD(softc,op,dest,src,cnt)
    *
    *  Add an instruction to the flashop table
    *
    *  Input parameters:
    *	   softc - our flash
    *	   op,dest,src,cnt - data for the opcode
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void flash_op_add(flashdev_t *softc, long base, long op, long dest,
			 long src, long cnt)
{
	flashinstr_t *fi;

	if (softc->fd_inst == NULL) {
		int ttlsect = softc->fd_ttlsect;

		ttlsect = (ttlsect * 2) + 6;
		softc->fd_inst = KMALLOC(ttlsect * sizeof(flashinstr_t), 0);
		if (!softc->fd_inst) {
			err_msg("%s(): memory allocation fail!", __func__);
			return;
		}
	}
	fi = &(softc->fd_inst[softc->fd_iptr]);
	fi->fi_op = op;
	fi->fi_base = base;
	fi->fi_dest = dest;
	fi->fi_src = src;
	fi->fi_cnt = cnt;

	softc->fd_iptr++;
}

#if defined(NOR_FLASH_8_BIT)
static void flash_cmd(unsigned long base, unsigned long dest, unsigned char cmd)
{
	REG_U8(UNCADDR(base + (dest))) = cmd;
}
#else
static void flash_cmd(unsigned long base, unsigned long dest,
		      unsigned short cmd)
{
	REG_U16(UNCADDR(base + (dest << 1))) = cmd;
}
#endif

/*  *********************************************************************
    *  FLASH_OP_EXECUTE(softc)
    *
    *  Execute the stored "flash operations"
    *
    *  Input parameters:
    *	   softc - our flash
    *
    *  Return value:
    *	   0 if ok, else # of failures (less than zero)
    ********************************************************************* */

static int flash_op_execute(flashdev_t *softc)
{
	flash_op_add(softc, softc->fd_probe.flash_phys, FEOP_RETURN, 0, 0, 0);

#ifdef _NORFLASH_DEBUG_
	if (1) {
		int idx;

		for (idx = 0; idx < softc->fd_iptr; idx++) {
			printf("%2d %08X %08X %08X %08X\n",
			       softc->fd_inst[idx].fi_op,
			       softc->fd_inst[idx].fi_base,
			       softc->fd_inst[idx].fi_dest,
			       softc->fd_inst[idx].fi_src,
			       softc->fd_inst[idx].fi_cnt);
		}
	}
#endif
	flash_engine(softc);
	return 0;
}

/*  *********************************************************************
    *  FLASH_ERASE_RANGE(softc,range)
    *
    *  Erase a range of sectors
    *
    *  Input parameters:
    *	   softc - our flash
    *	   range - range structure
    *
    *  Return value:
    *	   0 if ok
    *	   else error
    ********************************************************************* */

static int flash_erase_range(flashdev_t *softc, flash_range_t *range)
{
	flash_sector_t sector;
	int res;

	if (range->range_base + range->range_length > softc->flash.size)
		return BOLT_ERR_INV_PARAM;

	res = 0;

	sector.flash_sector_idx = 0;

	for (;;) {
		res = flash_sector_query(softc, &sector);
		if (res != 0)
			break;
		if (sector.flash_sector_status == FLASH_SECTOR_INVALID)
			break;

		if ((sector.flash_sector_offset >= range->range_base) &&
		    (sector.flash_sector_offset <
		     (range->range_base + range->range_length - 1))) {

			flash_op_begin(softc);
			flash_op_add(softc, softc->fd_probe.flash_phys,
				     softc->fd_erasefunc,
				     sector.flash_sector_offset, 0, 0);
			res = flash_op_execute(softc);

			if (res != 0)
				break;
		}
		sector.flash_sector_idx++;
	}

	return res;
}

/*  *********************************************************************
    *  FLASH_ERASE_ALL(softc)
    *
    *  Erase the entire flash device, except the NVRAM area,
    *  sector-by-sector.
    *
    *  Input parameters:
    *	   softc - our flash
    *
    *  Return value:
    *	   0 if ok
    *	   else error code
    ********************************************************************* */

static int flash_erase_all(flashdev_t *softc)
{
	flash_range_t range;

	range.range_base = 0;
	range.range_length = softc->flash.size;

	return flash_erase_range(softc, &range);
}

/*  *********************************************************************
    *  flash_range_intersection(sector,inrange,outrange)
    *
    *  Compute the intersection between a flash range and a
    *  sector.
    *
    *  Input parameters:
    *	   sector - sector to examine
    *	   range - range we are checking
    *	   outrange - where to put resulting intersection range
    *
    *  Return value:
    *	   1 - range is an entire sector
    *	   0 - range is a partial sector
    *	   -1 - range has no intersection
    ********************************************************************* */

static int flash_range_intersection(flash_sector_t *sector,
				    flash_range_t *inrange,
				    flash_range_t *outrange)
{
	int start, end;

	/*
	 * compute the start and end pointers
	 */

	start = (int)(max(sector->flash_sector_offset, inrange->range_base));

	end =
		(int)(min
		      ((sector->flash_sector_offset +
			sector->flash_sector_size),
		       (inrange->range_base + inrange->range_length)));

	/*
	 * if the end is in the right place wrt the start,
	 * there is an intersection.
	 */

	if (end > start) {
		outrange->range_base = (unsigned int)start;
		outrange->range_length = (unsigned int)(end - start);

		if ((sector->flash_sector_offset == outrange->range_base) &&
		    (sector->flash_sector_size == outrange->range_length)) {
			return 1;	/* instersection: entire sector */
		} else {
			return 0;	/* intersection: partial sector */
		}
	} else {
		outrange->range_base = (unsigned int)start;
		outrange->range_length = 0;
		return -1;	/* intersection: none */
	}
}

/*  *********************************************************************
    *  FLASH_SECTOR_QUERY(softc,sector)
    *
    *  Query the sector information about a particular sector.  You can
    *  call this iteratively to find out about all of the sectors.
    *
    *  Input parameters:
    *	   softc - our flash info
    *	   sector - structure to receive sector information
    *
    *  Return value:
    *	   0 if ok
    *	   else error code
    ********************************************************************* */

static int flash_sector_query(flashdev_t *softc, flash_sector_t *sector)
{
	int idx;
	int nblks;
	int blksiz;
	unsigned int offset;
	int whichchip;
	int secidx;
	int curblk;

	if (softc->flash.type != FLASH_TYPE_NOR)
		return BOLT_ERR_UNSUPPORTED;

	if (softc->fd_probe.flash_nsectors == 0)
		return BOLT_ERR_UNSUPPORTED;

	/* Figure out which chip */
	whichchip = sector->flash_sector_idx / softc->fd_ttlsect;
	if (whichchip >= 1) {
		sector->flash_sector_status = FLASH_SECTOR_INVALID;
		return 0;
	}

	/* Within that chip, get sector info */
	offset = softc->flash.size * whichchip;
	secidx = sector->flash_sector_idx % softc->fd_ttlsect;
	curblk = 0;

	for (idx = 0; idx < softc->fd_probe.flash_nsectors; idx++) {
		nblks = FLASH_SECTOR_NBLKS(softc->fd_probe.flash_sectors[idx]);
		blksiz = FLASH_SECTOR_SIZE(softc->fd_probe.flash_sectors[idx]);
		if (secidx < curblk + nblks) {
			sector->flash_sector_status = FLASH_SECTOR_OK;
			sector->flash_sector_offset =
				offset + (secidx - curblk) * blksiz;
			sector->flash_sector_size = blksiz;
			break;
		}

		offset += (nblks) * blksiz;
		curblk += nblks;
	}

	if (idx == softc->fd_probe.flash_nsectors)
		sector->flash_sector_status = FLASH_SECTOR_INVALID;

	return 0;
}

/*  *********************************************************************
    *  FLASH_SET_CMDSET(softc,cmdset,bus16,dev16)
    *
    *  Set the command-set that we'll honor for this flash.
    *
    *  Input parameters:
    *	   softc - our flash
    *	   cmdset - CFI_CMDSET_xxx
    *      bus16 - true if bus is 16 bits wide
    *      dev16 - true if device supports 16-bit operation
    *
    *  So: bus16 && dev16 -> 16-bit commands
    *      !bus16 && dev16 -> 8-bit commands to 16-bit flash with BYTE#
    *      !bus16 && !dev16 -> 8-bit commands
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void flash_set_cmdset(flashdev_t *softc, int cmdset, int bus16,
			     int dev16)
{
	switch (cmdset) {
	case CFI_CMDSET_INTEL_ECS:
	case CFI_CMDSET_INTEL_STD:
		if (bus16) {
			softc->fd_erasefunc = FEOP_INTEL_ERASE16;
			softc->fd_pgmfunc = FEOP_INTEL_PGM16;
			softc->fd_readfunc = FEOP_READ16;
		} else {
			softc->fd_erasefunc = FEOP_INTEL_ERASE8;
			softc->fd_pgmfunc = FEOP_INTEL_PGM8;
			softc->fd_readfunc = FEOP_READ8;
		}
		break;
	case CFI_CMDSET_AMD_STD:
	case CFI_CMDSET_AMD_ECS:
		if (!bus16 && !dev16) {	/* 8-bit bus, 8-bit flash */
			softc->fd_erasefunc = FEOP_AMD_ERASE8;
			softc->fd_pgmfunc = FEOP_AMD_PGM8;
			softc->fd_readfunc = FEOP_READ8;
		} else if (bus16 && dev16) {	/* 16-bit bus, 16-bit flash */
			softc->fd_erasefunc = FEOP_AMD_ERASE16;
			softc->fd_pgmfunc = FEOP_AMD_PGM16;
			softc->fd_readfunc = FEOP_READ16;
		} else {	/* 8-bit bus, 16-bit flash w/BYTE# */
			softc->fd_erasefunc = FEOP_AMD_ERASE16B;
			softc->fd_pgmfunc = FEOP_AMD_PGM16B;
			softc->fd_readfunc = FEOP_READ8;
		}
		break;
	default:
		/* we don't understand the command set - treat it like ROM */
		softc->fd_erasefunc = FEOP_RETURN;
		softc->fd_pgmfunc = FEOP_RETURN;
		softc->fd_readfunc = bus16 ? FEOP_READ16 : FEOP_READ8;
		break;
	}
}

/*  *********************************************************************
    *  FLASH_CFI_PROBE(softc)
    *
    *  Try to do a CFI query on this device.  If we find the m
    *  magic signature, extract some useful information from the
    *  query structure.
    *
    *  Input parameters:
    *	   softc - out flash
    *
    *  Return value:
    *	   0 if successful, <0 if error
    ********************************************************************* */
static int flash_cfi_probe(flashdev_t *softc)
{
	uint8_t cfidata[MAX_CFIDATA];
	struct cfi_ident *cfi;
	unsigned int cmdset;
	unsigned int devif;
	int bus16 = 0;
	int dev16 = 0;
	int idx;
	int regcnt;
	int nblks;
	int blksiz;

	if (softc->fd_probe.flash_flags & FLASH_FLG_BUS16)
		bus16 = 1;

	/*
	 * Do a CFI query (16-bit)
	 */

	idx = FEOP_CFIQUERY8;
	if (softc->fd_probe.flash_flags & FLASH_FLG_DEV16) {
		idx = (softc->fd_probe.flash_flags & FLASH_FLG_BUS16) ?
		      FEOP_CFIQUERY16 : FEOP_CFIQUERY16B;
	}

	flash_op_begin(softc);
	flash_op_add(softc, softc->fd_probe.flash_phys,
		     idx, (long)cfidata, 0, MAX_CFIDATA);
	flash_op_execute(softc);

	cfi = (void *)cfidata + CFI_OFFSET;

	/*
	 * Look for signature.
	 */

	if (strncmp(cfi->qry, "QRY", 3)) {
		/*
		 * No CFI, bail.  First, set the command set to an invalid
		 * value so that we'll use default routines to read but not do
		 * programming
		 */
		flash_set_cmdset(softc, -1, bus16, dev16);
		return BOLT_ERR_UNSUPPORTED;
	}

	/*
	 * Gather info from flash
	 */
	cmdset = le16_to_cpu(cfi->mfr_id);
	devif = le16_to_cpu(cfi->interface_desc);
	softc->flash.size = 1 << cfi->dev_size;

	softc->fd_probe.flash_phys = BDEV_RD(BCHP_EBI_CS_BASE_0) &
				     BCHP_EBI_CS_BASE_0_base_addr_MASK;

#if CFG_ARCH_ARM
	/* Just get the offset from EBI_CS_BASE_0 */
#else /* MIPS */
	/* if flash size is less than 4MB, then set fd_prob.flash_phys to
	 * 0x1fc0_0000
	 */
	if (softc->flash.size < 0x400000)
		softc->fd_probe.flash_phys = 0x1fc00000;
	else {
		/* set flash physical starting address based on size */
		softc->fd_probe.flash_phys = 0x20000000 - softc->flash.size;
	}
#endif /* !CFG_ARCH_ARM */

	/*
	 * It's a 16-bit device if it is either always 16 bits or can be.
	 * we'll use "bus16" to decide if the BYTE# pin was strapped
	 */

	if ((devif == CFI_DEVIF_X16) || (devif == CFI_DEVIF_X8X16))
		dev16 = 1;

	regcnt = cfi->num_erase_regions;

	softc->fd_probe.flash_nsectors = regcnt;

	for (idx = 0; idx < regcnt; idx++) {
		struct cfi_erase_region *region = &cfi->erase_regions[idx];

		nblks = (int)le16_to_cpu(region->num_sectors) + 1;
		blksiz = (int)le16_to_cpu(region->density) * 256;

		softc->fd_probe.flash_sectors[idx] =
			FLASH_SECTOR_RANGE(nblks, blksiz);
	}

	/*
	 * Set the command set we're going to use.
	 */

	flash_set_cmdset(softc, cmdset, bus16, dev16);

	return 0;

}

/*  *********************************************************************
    *  flashdrv_probe(drv,probe_a,probe_b,probe_ptr)
    *
    *  Device probe routine.  Attach the flash device to
    *  BOLT's device table.
    *
    *  Input parameters:
    *	   drv - driver descriptor
    *	   probe_a - physical address of flash
    *	   probe_b - size of flash (bytes)
    *	   probe_ptr - unused
    *
    *  Return value:
    *	   nothing
    ********************************************************************* */

static void flashdrv_probe(bolt_driver_t *drv,
			   unsigned long probe_a, unsigned long probe_b,
			   void *probe_ptr)
{
	flashdev_t *softc;
	norflash_probe_t *probe;
	int idx;
	static int flashidx;

	/*
	 * Now, on with the probing.
	 */

	probe = (norflash_probe_t *) probe_ptr;

	/*
	 * Allocate sector buffer. never freed
	 * Sector buffer is used when programming the partial block
	 * by reading the sector and merging it with new data.
	 * At most, there could be two partial sector, so alocate two times
	 * sector size.
	 */
	if (flash_sector_buffer == NULL)
		flash_sector_buffer = KMALLOC(MAX_FLASH_SECTOR_SIZE * 2, 4);

	/*
	 * probe_a is the flash base address
	 * probe_b is the size of the flash
	 * probe_ptr is unused.
	 */

	if (!probe) {
		printf("Invalid flash structure\n");
		return;
	}

	softc = (flashdev_t *) KMALLOC(sizeof(flashdev_t), 0);
	if (!softc)
		return;
	memset(softc, 0, sizeof(flashdev_t));

	/*
	 * Passed probe structure, do fancy stuff
	 */
	memcpy(&softc->fd_probe, probe, sizeof(*probe));

	probe = &softc->fd_probe;

	softc->flash.cs = flashidx; /* Only support CS 0? */
	softc->flash.type = FLASH_TYPE_NOR;
	softc->flash.writesize = 1;

	/* Store driver data */
	softc->flash.priv = softc;

	softc->fd_sectorbuffer = flash_sector_buffer;
	if (flash_cfi_probe(softc) < 0) {
		err_msg("NOR: flash not detected");
		KFREE(softc);
		return;
	}

	/* Determine maximum sector (block) size */
	softc->flash.blocksize = 0;
	for (idx = 0; idx < probe->flash_nsectors; idx++) {
		unsigned int bsize =
			FLASH_SECTOR_SIZE(probe->flash_sectors[idx]);
		if (bsize > softc->flash.blocksize)
			softc->flash.blocksize = bsize;
	}

	/* Register this flash with the generic flash system */
	flash_register_device(drv, &softc->flash);

	printf("CS0: NOR, %dMB, %ukB blocks\n",
	       (int)(softc->flash.size / (1024 * 1024)),
	       softc->flash.blocksize / 1024);

	flashidx++;

	/* Count total sectors on the device */

	softc->fd_ttlsect = 0;
	for (idx = 0; idx < probe->flash_nsectors; idx++)
		softc->fd_ttlsect +=
			FLASH_SECTOR_NBLKS(probe->flash_sectors[idx]);
}

/*  *********************************************************************
    *  flashdrv_open(ctx)
    *
    *  Called when the flash device is opened.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0 if ok else error code
    ********************************************************************* */

static int flashdrv_open(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	flashdev_t *softc = flashpart_get_drvdata(part);
	int ttlsect = softc->fd_ttlsect;

	/*
	 * Calculate number of flashop instructions we'll need at most.
	 * This will be two for each sector plus two more for the first
	 * and last sectors, plus two extra
	 */

	ttlsect = (ttlsect * 2) + 6;

	/*
	 * Allocate memory for instructions.
	 */

	softc->fd_inst = KMALLOC(ttlsect * sizeof(flashinstr_t), 0);
	if (!softc->fd_inst)
		return BOLT_ERR_NOMEM;

	return 0;
}

/*  *********************************************************************
    *  flashdrv_read(ctx,buffer)
    *
    *  Read data from the flash device.    The flash device is
    *  considered to be like a disk (you need to specify the offset).
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - buffer descriptor
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */

static int flashdrv_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	flashdev_t *softc = flashpart_get_drvdata(part);
	unsigned int blen;
	unsigned int offset;

	blen = buffer->buf_length;
	offset = (long)buffer->buf_offset;

	if ((offset + blen) > part->size)
		blen = part->size - offset;

	offset += part->offset;

	if (blen > 0) {
		flash_op_begin(softc);
		flash_op_add(softc, softc->fd_probe.flash_phys,
			     softc->fd_readfunc, (long)buffer->buf_ptr, offset,
			     blen);
		flash_op_execute(softc);
	}

	buffer->buf_retlen = blen;

	return 0;
}

/*  *********************************************************************
    *  flashdrv_inpstat(ctx,inpstat)
    *
    *  Return "input status".  For flash devices, we always return true.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   inpstat - input status structure
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */

static int flashdrv_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	inpstat->inp_status = 1;
	return 0;
}

/*  *********************************************************************
    *  flashdrv_write(ctx,buffer)
    *
    *  Write data to the flash device.    The flash device is
    *  considered to be like a disk (you need to specify the offset).
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - buffer descriptor
    *
    *  Return value:
    *	   0 if ok, else error code
    ********************************************************************* */

static int flashdrv_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	flashdev_t *softc = flashpart_get_drvdata(part);
	unsigned int blen;
	int res;
	unsigned int offset;
	int whichchip;
	long chipbase;
	long sector_buffer_index = 0;
	flash_range_t outrange;
	flash_range_t inrange;
	flash_sector_t sector;

	blen = buffer->buf_length;
	offset = (long)buffer->buf_offset;

	/* Compute range within physical flash */

	if ((offset + blen) > part->size)
		blen = part->size - offset;

	offset += part->offset;

	/* Handle case of writing nothing */

	if (blen == 0) {
		buffer->buf_retlen = blen;
		return (buffer->buf_length == blen) ? 0 : BOLT_ERR_IOERR;
	}

	/* now, offset/blen forms the range we want to write to. */

	inrange.range_base = offset;
	inrange.range_length = blen;

	sector.flash_sector_idx = 0;

	flash_op_begin(softc);

	for (;;) {
		res = flash_sector_query(softc, &sector);
		if (res != 0)
			break;
		if (sector.flash_sector_status == FLASH_SECTOR_INVALID)
			break;

		whichchip = sector.flash_sector_idx / softc->fd_ttlsect;
		chipbase = softc->fd_probe.flash_phys +
			   (long)(whichchip * softc->flash.size);

		res = flash_range_intersection(&sector, &inrange, &outrange);

		switch (res) {
		case 1:	/* Erase/program entire sector */
			flash_op_add(softc, chipbase,
				     softc->fd_erasefunc,
				     sector.flash_sector_offset, 0, 0);
			break;

		case 0:	/* Erase/reprogram partial sector */
			/* Save old sector */
			flash_op_add(softc, chipbase,
				     softc->fd_readfunc,
				     (long)(&
					    (softc->fd_sectorbuffer
					     [sector_buffer_index])),
				     sector.flash_sector_offset,
				     sector.flash_sector_size);
			/* Copy in new stuff */
			flash_op_add(softc, chipbase, FEOP_MEMCPY, ((long)
					(&
					 (softc->fd_sectorbuffer
					  [sector_buffer_index])))
				     + (outrange.range_base -
					sector.flash_sector_offset),
				     ((long)buffer->buf_ptr) +
				     (outrange.range_base - inrange.range_base),
				     outrange.range_length);
			/* Erase sector */
			flash_op_add(softc, chipbase,
				     softc->fd_erasefunc,
				     sector.flash_sector_offset, 0, 0);
			/* update the index so that another sector buffer is
			 * used if there is another partial block programming.
			 */
			if (sector_buffer_index > MAX_FLASH_SECTOR_SIZE) {
				err_msg("more than two partial blocks (%ld)",
					sector_buffer_index);
				return BOLT_ERR_IOERR;
			} else
				sector_buffer_index += MAX_FLASH_SECTOR_SIZE;
			break;

		case -1:	/* No intersection */
			break;
		}

		sector.flash_sector_idx++;

	}

	sector.flash_sector_idx = 0;

	for (;;) {
		res = flash_sector_query(softc, &sector);
		if (res != 0)
			break;
		if (sector.flash_sector_status == FLASH_SECTOR_INVALID)
			break;

		whichchip = sector.flash_sector_idx / softc->fd_ttlsect;
		chipbase = softc->fd_probe.flash_phys +
			   (long)(whichchip * softc->flash.size);

		res = flash_range_intersection(&sector, &inrange, &outrange);

		switch (res) {
		case 1:
			flash_op_add(softc, chipbase,
				     softc->fd_pgmfunc,
				     outrange.range_base,
				     ((long)buffer->buf_ptr) +
				     (outrange.range_base - inrange.range_base),
				     outrange.range_length);
			break;

		case 0:
			/* Program sector */
			flash_op_add(softc, chipbase,
				     softc->fd_pgmfunc,
				     sector.flash_sector_offset,
				     (long)(softc->fd_sectorbuffer),
				     sector.flash_sector_size);
			break;

		case -1:	/* No intersection */
			break;
		}

		sector.flash_sector_idx++;

	}

	res = flash_op_execute(softc);

	buffer->buf_retlen = blen;

	return (res == 0) ? 0 : BOLT_ERR_IOERR;
}

/*  *********************************************************************
    *  flashdrv_ioctl(ctx,buffer)
    *
    *  Handle special IOCTL functions for the flash.  Flash devices
    *  support NVRAM information, sector and chip erase, and a
    *  special IOCTL for updating the running copy of BOLT.
    *
    *  Input parameters:
    *	   ctx - device context
    *	   buffer - descriptor for IOCTL parameters
    *
    *  Return value:
    *	   0 if ok else error
    ********************************************************************* */
static int flashdrv_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	struct flash_partition *part = ctx->dev_softc;
	flashdev_t *softc = flashpart_get_drvdata(part);
	nvram_info_t *nvinfo;
	struct flash_info *info;
	flash_range_t range;
	unsigned int offset;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_NVRAM_GETINFO:
		/*
		 * We only support NVRAM on flashes that have been partitioned
		 * into at least two partitions.  Every partition supports
		 * being an NVRAM in that case, but we'll only attach one
		 * of them to the environment subsystem.
		 */
		if (softc->flash.nparts <= 1)
			return BOLT_ERR_UNSUPPORTED;
		nvinfo = (nvram_info_t *) buffer->buf_ptr;
		if (buffer->buf_length != sizeof(nvram_info_t))
			return BOLT_ERR_INV_PARAM;

		nvinfo->nvram_offset = 0;
		nvinfo->nvram_offs_part = part->offset;
		nvinfo->nvram_size = part->size;
		nvinfo->nvram_eraseflg = 1;
		buffer->buf_retlen = sizeof(nvram_info_t);
		return 0;
		break;

	case IOCTL_FLASH_ERASE_SECTOR:
	case IOCTL_FLASH_ERASE_BLOCK:
		offset = (int)buffer->buf_offset;
		offset += part->offset;
		if (offset >= softc->flash.size)
			return -1;

		flash_op_begin(softc);
		flash_op_add(softc,
			     softc->fd_probe.flash_phys,
			     softc->fd_erasefunc, offset, 0, 0);
		flash_op_execute(softc);
		return 0;

	case IOCTL_FLASH_ERASE_ALL:
		offset = (int)buffer->buf_offset;
		if (offset != 0)
			return -1;
		flash_erase_all(softc);
		return 0;

	case IOCTL_FLASH_GETINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = softc->fd_probe.flash_phys;
		info->flash_size = softc->flash.size;
		info->type = softc->flash.type;
		info->flags = FLASH_FLAG_NOERASE;
		return 0;

	case IOCTL_FLASH_GETPARTINFO:
		info = (struct flash_info *)buffer->buf_ptr;
		info->flash_base = part->offset;
		info->flash_size = part->size;
		info->flags = FLASH_FLAG_NOERASE;
		return 0;
	case IOCTL_FLASH_GETSECTORS:
		return flash_sector_query(softc,
					  (flash_sector_t *) buffer->buf_ptr);

	case IOCTL_FLASH_ERASE_RANGE:
		memcpy(&range, buffer->buf_ptr, sizeof(flash_range_t));
		range.range_base += part->offset;
		if (range.range_length > part->size)
			range.range_length = part->size;
		return flash_erase_range(softc, &range);

	default:
		return -1;
	}

	return -1;
}

/*  *********************************************************************
    *  flashdrv_close(ctx)
    *
    *  Close the flash device.
    *
    *  Input parameters:
    *	   ctx - device context
    *
    *  Return value:
    *	   0
    ********************************************************************* */
static int flashdrv_close(bolt_devctx_t *ctx)
{
	struct flash_partition *part = ctx->dev_softc;
	flashdev_t *softc = flashpart_get_drvdata(part);

	if (softc->fd_inst)
		KFREE(softc->fd_inst);

	softc->fd_inst = NULL;

	/* XXX Invalidate the cache ?!?! */

	return 0;
}

static void flash_engine(flashdev_t *softc)
{
	int idx;
	unsigned int i, j;
	unsigned long reg_src, reg_base, reg_dest, reg_cnt, temp;

	for (idx = 0; idx < softc->fd_iptr; idx++) {
		reg_base = softc->fd_inst[idx].fi_base,
		reg_dest = softc->fd_inst[idx].fi_dest;
		reg_src = softc->fd_inst[idx].fi_src;
		reg_cnt = softc->fd_inst[idx].fi_cnt;

		switch (softc->fd_inst[idx].fi_op) {
		case FEOP_RETURN:
			break;
		case FEOP_READ16:
			reg_src = UNCADDR(reg_src + reg_base);
			for (i = 0; i < reg_cnt; i += 1) {
				REG_U8(reg_dest) =
					REG_U8(reg_src);
				reg_src += 1;
				reg_dest += 1;
			}
			break;
		case FEOP_MEMCPY:
			for (i = 0; i < reg_cnt; i += 1) {
				REG_U8(reg_dest) = REG_U8(reg_src);
				reg_src += 1;
				reg_dest += 1;
			}
			break;
		case FEOP_CFIQUERY16:
			reg_src = reg_src + reg_base;
			flash_cmd(reg_base, FLASH_CFI_QUERY_ADDR,
				  FLASH_CFI_QUERY_MODE);
#if defined(NOR_FLASH_8_BIT)
			for (i = 0; i < reg_cnt; i++) {
				REG_U8(reg_dest) =
					REG_U8(UNCADDR(reg_src));
				reg_dest += 1;
				reg_src += 1;
#else
			for (i = 0; i < reg_cnt; i += 2) {
				REG_U8(reg_dest) = le16_to_cpu(
					(le16)REG_U16(UNCADDR(reg_src)));
				reg_dest += 1;
				reg_src += 2;
#endif
			}
			flash_cmd(reg_base, 0, 0xf0);	/* reset */
			break;

		case FEOP_AMD_ERASE16:
			reg_dest = reg_dest + reg_base;
			flash_cmd(reg_dest, AMD_FLASH_MAGIC_ADDR_1,
				  AMD_FLASH_MAGIC_1);
			flash_cmd(reg_dest, AMD_FLASH_MAGIC_ADDR_2,
				  AMD_FLASH_MAGIC_2);

			/* send the erase command (cycle 3) */
			flash_cmd(reg_dest, AMD_FLASH_MAGIC_ADDR_1,
				  AMD_FLASH_ERASE_3);

			flash_cmd(reg_dest, AMD_FLASH_MAGIC_ADDR_1,
				  AMD_FLASH_MAGIC_1);
			flash_cmd(reg_dest, AMD_FLASH_MAGIC_ADDR_2,
				  AMD_FLASH_MAGIC_2);

			/* Send the "erase sector" qualifier (cycle 6) */
			flash_cmd(reg_dest, 0, AMD_FLASH_ERASE_SEC_6);
#if defined(NOR_FLASH_8_BIT)
			while ((REG_U8(UNCADDR(reg_dest))) != 0xFF)
				;
#else
			while ((REG_U16(UNCADDR(reg_dest))) != 0xFFFF)
				;
#endif
			break;
		case FEOP_AMD_PGM16:
			reg_dest = reg_dest + reg_base;
			temp = reg_dest;
#if defined(NO_WRITE_BUFFER)
#if defined(NOR_FLASH_8_BIT)
			for (i = 0; i < reg_cnt; i++) {
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_1,
					  AMD_FLASH_MAGIC_1);
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_2,
					  AMD_FLASH_MAGIC_2);
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_1,
					  AMD_FLASH_PROGRAM);

				REG_U8(UNCADDR(reg_dest)) =
					   REG_U8(reg_src);
				while ((REG_U8(UNCADDR(reg_dest)) & 0x80) !=
				       (REG_U8(reg_src) & 0x80))
					;
				reg_dest += 1;
				reg_src += 1;
#else
			for (i = 0; i < reg_cnt; i += 2) {
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_1,
					  AMD_FLASH_MAGIC_1);
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_2,
					  AMD_FLASH_MAGIC_2);
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_1,
					  AMD_FLASH_PROGRAM);

				REG_U16(UNCADDR(reg_dest)) =
					REG_U16(reg_src);
				while ((REG_U16(UNCADDR(reg_dest)) & 0x80) !=
				       (REG_U16(reg_src) & 0x80))
					;
				reg_dest += 2;
				reg_src += 2;
#endif
			}
#else
			/* write buffer support */
			for (i = 0; i < reg_cnt; i += 32) {
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_1,
					  AMD_FLASH_MAGIC_1);
				flash_cmd(temp, AMD_FLASH_MAGIC_ADDR_2,
					  AMD_FLASH_MAGIC_2);
				/* specify write buffer programming */
				flash_cmd(temp, 0, 0x25);
#if defined(NOR_FLASH_8_BIT)
				/* number of bytes to be written, minus 1 */
				flash_cmd(temp, 0, 0x1f);
				for (j = 0; j < 32; j++) {
					REG_U8(UNCADDR(reg_dest)) =
						REG_U8(reg_src);
					reg_dest += 1;
					reg_src += 1;
				}
				flash_cmd(temp, 0, 0x29);
				while ((REG_U8(UNCADDR(reg_dest - 1)) & 0x80)
				       != (REG_U8(reg_src - 1) & 0x80)) {
					if ((REG_U8(UNCADDR(reg_dest - 1))
					     & 0x20) == 0x20)
						continue;
				}
#else
				/* number of bytes to be written, minus 1 */
				flash_cmd(temp, 0, 0xf);
				for (j = 0; j < 16; j++) {
					REG_U16(UNCADDR(reg_dest)) =
						   REG_U16(reg_src);
					reg_dest += 2;
					reg_src += 2;
				}
				/* number of bytes to be written, minus 1 */
				flash_cmd(temp, 0, 0x29);
				while ((REG_U16(UNCADDR(reg_dest - 2)) & 0x80)
				       != (REG_U16(reg_src - 2) & 0x80)) {
					if ((REG_U16(UNCADDR(reg_dest - 2))
					     & 0x20) == 0x20)
						continue;
				}
#endif
			}
#endif
			break;
		case FEOP_INTEL_ERASE16:
			reg_dest = reg_dest + reg_base;
			flash_cmd(reg_dest, 0, 0x90);
			/* Block locked, so unlock before erasing */
			if (REG_U16(UNCADDR(reg_dest + 4)) == 0x0001) {
				flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
				/* clear status */
				flash_cmd(reg_dest, 0, 0x50);
				/* Unlock flash */
				flash_cmd(reg_dest, 0, 0x60);
			} else {
				flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
				/* clear status */
				flash_cmd(reg_dest, 0, 0x50);
			}
			flash_cmd(reg_dest, 0, INTEL_FLASH_ERASE_CONFIRM);

			flash_cmd(reg_dest, 0, INTEL_FLASH_ERASE_BLOCK);
			flash_cmd(reg_dest, 0, INTEL_FLASH_ERASE_CONFIRM);

			flash_cmd(reg_dest, 0, 0x70);
			while ((REG_U16(UNCADDR(reg_dest)) & 0x80) != 0x80)
				;
			flash_cmd(reg_dest, 0, 0x50);	/* clear status */
			flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
			break;
		case FEOP_INTEL_PGM16:
#if defined(NO_WRITE_BUFFER)
			reg_dest = reg_dest + reg_base;
			for (i = 0; i < reg_cnt; i += 2) {
				flash_cmd(reg_dest, 0, INTEL_FLASH_PROGRAM);
				REG_U16(UNCADDR(reg_dest)) =
					   REG_U16(reg_src);
				while ((REG_U16(UNCADDR(reg_dest)) & 0x80)
					!= 0x80)
					;
				flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
				reg_dest += 2;
				reg_src += 2;
			}
			reg_dest -= 2;
			flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
#else
			reg_dest = reg_dest + reg_base;
			for (i = 0; i < reg_cnt; i += 32) {
				/* buffer write */
				flash_cmd(reg_dest, 0, 0xe8);
				/* read status reg */
				flash_cmd(reg_dest, 0, 0x70);
				while ((REG_U16(UNCADDR(reg_dest)) & 0x80)
					!= 0x80)
					;
				/* number of words to be written */
				flash_cmd(reg_dest, 0, 0x1f);
				for (j = 0; j < 16; j++) {
					REG_U16(UNCADDR(reg_dest)) =
						REG_U16(reg_src);
					reg_dest += 2;
					reg_src += 2;
				}
				reg_dest -= 32;
				/* confirm write */
				flash_cmd(reg_dest, 0, 0xd0);
				/* read status reg */
				flash_cmd(reg_dest, 0, 0x70);
				while ((REG_U16(UNCADDR(reg_dest)) & 0x80)
					!= 0x80)
					;
				/* clear status */
				flash_cmd(reg_dest, 0, 0x50);
				flash_cmd(reg_dest, 0, INTEL_FLASH_READ_MODE);
				reg_dest += 32;
			}
#endif
			break;
		}
	}
}
