/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "devfuncs.h"
#include "ioctl.h"
#include "timer.h"
#include "error.h"

#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"

#include "fileops.h"
#include "byteorder.h"

#include "loader.h"

#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"

#include "initdata.h"
#include "board.h"
#include "board_init.h"
#include "bsp_config.h"
#include "gisb.h"

#if defined(DVFS_SUPPORT)
#include "pmap.h"
#include "avs_dvfs.h"
#endif

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

static int ui_cmd_flash(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_boards(ui_cmdline_t *cmd, int argc, char *argv[]);
#if !defined(SECURE_BOOT)
static int ui_cmd_setboard(ui_cmdline_t *cmd, int argc, char *argv[]);
#if defined(DVFS_SUPPORT)
static int ui_cmd_pmap(ui_cmdline_t *cmd, int argc, char *argv[]);
static unsigned int dvfs_show_board_config(struct board_type *b, struct fsbl_info *inf);
#endif
#endif
static int ui_cmd_rts(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_gisb(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_erase(ui_cmdline_t *cmd, int argc, char *argv[]);


/*  *********************************************************************
    *  ui_init_flashcmds()
    *
    *  Initialize the flash commands, add them to the table.
    *
    *  Input parameters:
    *	   nothing
    *
    *  Return value:
    *	   0 if ok, else error
    ********************************************************************* */

int ui_init_flashcmds(void)
{
	cmd_addcmd("flash",
		   ui_cmd_flash,
		   NULL,
		   "Update a flash memory device",
		   "flash [options] filename [flashdevice]\n\n"
		   "Copies data from a source file name or device to a flash memory device.\n"
		   "The source device can be a disk file (FAT filesystem), a remote file\n"
		   "(TFTP) or a flash device.  The destination device may be a flash or eeprom.\n",
		   "-noerase;Don't erase flash before writing|"
		   "-forceerase;Force erase flash before writing|"
		   "-blockerase;erase a sector in the flash (deprecated)|"
		   "-forcewrite;If file larger than flash device or default maximum,\n"
		   "truncate and force write|"
		   "-offset=*;Begin programming at this offset in the flash device|"
		   "-size=*;Size of source device when programming from flash to flash.\n"
		   "Otherwise, for disk and network files, this is the maximum\n"
		   "size of file, which when less than 1024 is interpreted in MB.|"
		   "-mem=*;if mem device then specify from where to read|"
		   "-memsize=*;if mem device then specify amount to read|"
		   "-header;deprecated option; do not use|"
		   "-noheader;deprecated option; do not use|"
		   "-revend;Binary file being downloaded is halfword swapped prior to being flashed|"
		   "-revend1;Binary file being downloaded is full byte swapped prior to being flashed");

	cmd_addcmd("boards", ui_cmd_boards, NULL, "show supported boards", "",
		"");

#if !defined(SECURE_BOOT)
	cmd_addcmd("setboard", ui_cmd_setboard, NULL, "set board number",
			"setboard [boardnum] (requires reboot)",
			"-park;enable overtemp checking|"
			"-nopark;disable overtemp checking|"
			"-avs;enable AVS|"
			"-noavs;disable AVS|"
			"-bavs;board defaults for AVS|"
			"-force;always update flash|"
			"-forget;forget saved board selection"
);
#if defined(DVFS_SUPPORT)
	cmd_addcmd("pmap", ui_cmd_pmap, NULL, "Select or clear a PMap",
			"pmap [-set=N] where N is the number shown when listing PMap",
			"-set=*;program and save to flash PMap|"
			"-clear;unprogram and unset flash saved PMap (will then use board defaults)"
);
#endif
#endif
	cmd_addcmd("rts", ui_cmd_rts, NULL, "select an RTS or clear selection",
			"rts [-set=N]",
			"-set=*;selects a specified RTS set|"
			"-clear;clears selectoin and returns to board default");

	cmd_addcmd("gisb", ui_cmd_gisb, NULL, "show or set GISB bus timeout",
			"gisb [-set=*]",
			"-set=*;program new GISB timeout"
);

	cmd_addcmd("erase", ui_cmd_erase, NULL,
		   "erase flash device or partition",
		   "erase [options] device|device.partition",
		   "-offset=*;Begin erasing at this offset in the flash device|"
		   "-size=*;Size of the region to erase (0, for rest of the flash)");

	return 0;
}

/*  *********************************************************************
    *  set_size_addr(ptr,size,la)
    *
    *  Set the load address and size if specified.
    *
    *  Input parameters:
    *      ptr - load address
    *      size - size of file to load
    *      la - load args
    *
    *  Return value:
    *      0 if ok
    *      else error occured
    ********************************************************************* */

static int set_size_addr(uint8_t **ptr, int size, bolt_loadargs_t *la)
{
	if (size > 0) {
		if (size < 1024)  /* size specified in MB */
			size *= 1024 * 1024;
		if (size <= FLASH_STAGING_BUFFER_SIZE) { /* SWBOLT-97 */
			la->la_address =
			    (intptr_t) FLASH_STAGING_BUFFER;
			la->la_maxsize = size;
			*ptr = (uint8_t *) FLASH_STAGING_BUFFER;
			xprintf("(size=%d) ", size);
		} else {
			xprintf
			    ("Specified size (%d) is invalid for this BOLT.\n",
			     size);
			return ui_showerror(BOLT_ERR_INV_PARAM, "Failed");
		}
	} else {
		la->la_address = (intptr_t) *ptr;
		la->la_maxsize = FLASH_STAGING_BUFFER_SIZE;
	}

	return 0;
}

/*
 * Handle the deprecated 'flash -blockerase=*' option
 */
static int ui_cmd_flash_blockerase(ui_cmdline_t *cmd, int argc, char *argv[])
{
	const char *x;
	int fh, res;
	char *flashdev;
	unsigned long long offset = 0;

	warn_msg("Warning: 'flash -blockerase' is deprecated; use 'erase' command instead");

	/*
	 * The syntax will be something like
	 * flash -blockerase -offset=20000 flash0.avail0
	 */

	flashdev = cmd_getarg(cmd, 0);
	if (!flashdev)
		return ui_showerror(BOLT_ERR_INV_PARAM, "No flash provided");

	res = bolt_getdevinfo(flashdev);
	if (res < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, flashdev);

	if (cmd_sw_value(cmd, "-offset", &x))
		offset = atoq(x);

	fh = bolt_open(flashdev);
	res = bolt_ioctl(fh, IOCTL_FLASH_ERASE_BLOCK, 0, 0, 0, offset);
	bolt_close(fh);
	if (res != 0) {
		printf("ioctl error\n");
		return BOLT_ERR_IOERR;
	}

	return 0;
}

/*  *********************************************************************
    *  ui_cmd_flash(cmd,argc,argv)
    *
    *  The 'flash' command lives here.  Program the boot flash,
    *  or if a device name is specified, program the alternate
    *  flash device.
    *
    *  Input parameters:
    *      cmd - command table entry
    *      argc,argv - parameters
    *
    *  Return value:
    *      0 if ok
    *      else error
    ********************************************************************* */

static int ui_cmd_flash(ui_cmdline_t *cmd, int argc, char *argv[])
{
	uint8_t *ptr;
	int fh;
	int res;
	char *fname;
	char *flashdev;
	bolt_loadargs_t la;
	unsigned int amtcopy;
	int devtype;
	int srcdevtype;
	int sfd;
	unsigned int copysize;
	struct flash_info flashinfo;
	unsigned long long offset = 0;
	int noerase = 0;
	int revend = 0, revend1 = 0;
	const char *x;
	int size = 0;
	int forceerase = 0;
	int forcewrite = 0;
	uint32_t mem_address, mem_size;

	memset(&la, 0, sizeof(la));

	/*
	 * Get the address of the staging buffer.  We can't
	 * allocate the space from the heap to store the
	 * new flash image, because the heap may not be big
	 * enough.  So, grab some unallocated memory
	 * at the 1MB line (we could also calculate
	 * something, but this will do for now).
	 * We assume the flash will be somewhere between
	 * 1KB (yeah, right) and 4MB.
	 */

#if CFG_RUNFROMKSEG0
	ptr = (uint8_t *) CACADDR(FLASH_STAGING_BUFFER);
#else
	ptr = (uint8_t *) UNCADDR(FLASH_STAGING_BUFFER);
#endif

	/*
	 * Parse command line parameters
	 */

	fname = cmd_getarg(cmd, 0);

	if (!fname)
		return ui_showusage(cmd);

	if (cmd_sw_isset(cmd, "-blockerase"))
		return ui_cmd_flash_blockerase(cmd, argc, argv);

	forcewrite = cmd_sw_isset(cmd, "-forcewrite");

	flashdev = cmd_getarg(cmd, 1);
	if (!flashdev)
		return ui_showerror(BOLT_ERR_INV_PARAM, "No flash provided");

	/*
	 * Make sure it's a flash device.
	 */
	res = bolt_getdevinfo(flashdev);
	if (res < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, flashdev);

	devtype = res & BOLT_DEV_MASK;

	if ((res != BOLT_DEV_FLASH) && (res != BOLT_DEV_NVRAM)) {
		xprintf("Device '%s' is not a flash or eeprom device.\n",
			flashdev);
		return BOLT_ERR_INV_PARAM;
	}

	if (cmd_sw_isset(cmd, "-header")) {
		xprintf("Invalid option; '-header' is deprecated\n");
		return BOLT_ERR_INV_PARAM;
	} else if (cmd_sw_isset(cmd, "-noheader")) {
		xprintf("Warning: deprecated option '-noheader' will be removed soon\n");
	}

	/*
	 * Check for some obscure options here.
	 */

	noerase = cmd_sw_isset(cmd, "-noerase");

	/*
	 * Force ease flash the partition before writing.
	 */
	forceerase = cmd_sw_isset(cmd, "-forceerase");

	/*
	 * Check for endian difference between BOLT and binary
	 * image being downloaded
	 */

	revend = cmd_sw_isset(cmd, "-revend");
	revend1 = cmd_sw_isset(cmd, "-revend1");

	if (cmd_sw_value(cmd, "-offset", &x))
		offset = atoq(x);

	if (cmd_sw_value(cmd, "-size", &x))
		size = atoi(x);

	/*
	 * Read the new flash image from the source device
	 */

	xprintf("Reading %s: ", fname);

	/*
	 * On fail we get the default action due to a -ve error code, else we
	 * get a device (BOLT_DEV_*) to play with.
	 */
	srcdevtype = bolt_getdevinfo(fname);
	if (srcdevtype > 0)
		srcdevtype &= BOLT_DEV_MASK;

	switch (srcdevtype) {
	case BOLT_DEV_FLASH:
		sfd = bolt_open(fname);
		if (sfd < 0) {
			return ui_showerror(sfd,
					"Could not open source device");
		}
		memset(ptr, 0xFF, FLASH_STAGING_BUFFER_SIZE);

		/*
		 * If the flash device can be used for NVRAM, then the max size
		 * of or flash is the offset of the flash info.  Otherwise it
		 * is the full staging buffer size.
		 * XXX: if it's larger, we lose.
		 */

		if (bolt_ioctl(sfd, IOCTL_FLASH_GETINFO, &flashinfo,
					sizeof(flashinfo), NULL, 0) != 0) {
			flashinfo.flash_size = FLASH_STAGING_BUFFER_SIZE;
		}

		if (size > 0)
			xprintf("(size=0x%X) ", size);
		else
			size = flashinfo.flash_size;

		/* Make sure we don't overrun the staging buffer */

		if (size > FLASH_STAGING_BUFFER_SIZE)
			size = FLASH_STAGING_BUFFER_SIZE;

		/* Read the flash device here. */

		res = bolt_read(sfd, ptr, size);

		bolt_close(sfd);
		if (res < 0) {
			return ui_showerror(res, "Could not read from flash");
		}
		/*
		 * Search for non-0xFF byte at the end.  This will work because
		 * flashes get erased to all FF's, we pre-fill our buffer to
		 * FF's,
		 */
		while (res > 0) {
			if (ptr[res - 1] != 0xFF)
				break;
			res--;
		}
		break;

	case BOLT_DEV_SERIAL:
		la.la_filesys = "raw";
		la.la_filename = NULL;
		la.la_device = fname;
		la.la_address = (intptr_t) ptr;
		la.la_options = NULL;
		la.la_maxsize = FLASH_STAGING_BUFFER_SIZE;
		la.la_flags = LOADFLG_SPECADDR;

		res = bolt_load_program("srec", &la);

		if (res < 0) {
			ui_showerror(res, "Failed.");
			return res;
		}
		break;

	case BOLT_DEV_MEM:
		if (cmd_sw_value(cmd, "-mem", &x)) {
			mem_address = atoi(x);
		} else {
			ui_showerror(BOLT_ERR_INV_PARAM, "Failed. -mem?");
			return BOLT_ERR_INV_PARAM;
		}

		if (cmd_sw_value(cmd, "-memsize", &x)) {
			mem_size = atoi(x);
		} else {
			ui_showerror(BOLT_ERR_INV_PARAM, "Failed. -memsize?");
			return BOLT_ERR_INV_PARAM;
		}

		res = mem_size;
		memcpy(ptr, (uint8_t *)mem_address, mem_size);
		break;

	case BOLT_DEV_DISK:
		{
			char *pdisk;

			pdisk = strdup(fname);
			if (!pdisk)
				return -1;

			fname = strchr(pdisk, ':');
			if (!fname) {
				KFREE(pdisk);
				return -1;
			}
			*fname++ = '\0';

			la.la_filesys = "fat";
			la.la_filename = fname;
			la.la_device = pdisk;
			res = set_size_addr(&ptr, size, &la);
			if (res < 0)
				return res;
			la.la_options = NULL;
			la.la_flags = LOADFLG_SPECADDR;

			res = bolt_load_program("raw", &la);

			if (res < 0) {
				ui_showerror(res, "Failed.");
				return res;
			}
			break;
		}

	default:
		la.la_filesys = "tftp";
		la.la_filename = fname;
		la.la_device = NULL;
		res = set_size_addr(&ptr, size, &la);
		if (res < 0)
			return res;
		la.la_options = NULL;
		la.la_flags = LOADFLG_SPECADDR;

		res = bolt_load_program("raw", &la);

		if (res < 0) {
			if (strstr(fname, "usb"))
				/* change error for USB devices */
				res = BOLT_ERR_DEVNOTFOUND;
			ui_showerror(res, "Failed");
			return res;
		}
		break;
	}

	xprintf("Done. %d bytes read\n", res);
	if (res == FLASH_STAGING_BUFFER_SIZE) {
		if (!forcewrite) {
			xprintf
				("\nWARNING: Downloaded file size equals the default maximum of %dM.\n"
				 "         If file is larger than this, please use the -size= option to\n"
				 "         specify a larger size or the -forcewrite= option to override.\n\n",
				 (FLASH_STAGING_BUFFER_SIZE >> 20));
			return BOLT_ERR_IOERR;
		}
	}

	copysize = res;

	if (copysize == 0)
		return 0;	/* 0 bytes, don't flash */

	/*
	 * Open the destination flash device.
	 */

	fh = bolt_open(flashdev);
	if (fh < 0) {
		xprintf("Could not open device '%s'\n", flashdev);
		return BOLT_ERR_DEVNOTFOUND;
	}

	if (bolt_ioctl(fh, IOCTL_FLASH_GETPARTINFO, &flashinfo,
				sizeof(flashinfo), NULL, 0) == 0) {
		if ((copysize + offset) > flashinfo.flash_size) {
			if (forcewrite) {
				/*
				 * Truncate write if source size is
				 * greater than flash size
				 */
				copysize = flashinfo.flash_size;
				xprintf("Writing truncated file... (size=%#x)\n",
					copysize);
			} else {
				xprintf("ERROR: File larger than flash device...(size=%#llx dev=%#llx)\n",
					 (copysize + offset),
					 flashinfo.flash_size);
				bolt_close(fh);
				return BOLT_ERR_IOERR;
			}
		}
	}

	/*
	 * Erase the flash, if the device requires it.  Our new flash
	 * driver does the copy/merge/erase for us.
	 */
	if (forceerase) {
		flash_sector_t sector;

		/*
		 * force erase will guarantee all the empty
		 * sectors are clean, i.e., filled with 0xFF
		 */
		sector.flash_sector_idx = 0;
		res = bolt_ioctl(fh, IOCTL_FLASH_GETSECTORS, &sector,
				sizeof(sector), NULL, 0);
		if (res != 0) {
			printf("ioctl error\n");
			bolt_close(fh);
			return BOLT_ERR_IOERR;
		}
		if (sector.flash_sector_status ==
				FLASH_SECTOR_INVALID) {
			printf("invalid sector\n");
			bolt_close(fh);
			return BOLT_ERR_IOERR;
		}

		printf("offset: %08llX   size: %08llX\n", offset,
				flashinfo.flash_size);
		if (devtype == BOLT_DEV_FLASH) {
			if (IS_ALIGNED(flashinfo.flash_base,
						sector.flash_sector_size) &&
					IS_ALIGNED(flashinfo.flash_size,
						sector.flash_sector_size)) {
				flash_range_t range;

				xprintf("Force erasing...\n");
				range.range_base = offset;
				range.range_length =
					flashinfo.flash_size;
				xprintf("Erasing flash...");
				if (bolt_ioctl
						(fh, IOCTL_FLASH_ERASE_RANGE,
						 &range, sizeof(range),
						 NULL, 0) != 0) {
					printf("Failed to erase the flash\n");
					bolt_close(fh);
					return BOLT_ERR_IOERR;
				}
			} else {
				xprintf("Flash erase failed: flash partition(base %llx + size %llx)\n"
					"not aligned to erase sector size %x\n",
					flashinfo.flash_base,
					flashinfo.flash_size,
					sector.flash_sector_size);
			}

		}
	} else {
		if (!noerase) {
			if ((devtype == BOLT_DEV_FLASH)
					&& !(flashinfo.flags &
						FLASH_FLAG_NOERASE)) {
				flash_range_t range;

				range.range_base = offset;
				range.range_length = copysize;
				xprintf("Erasing flash...");
				if (bolt_ioctl(fh, IOCTL_FLASH_ERASE_RANGE,
							&range, sizeof(range),
							NULL, 0) != 0) {
					printf("Failed to erase the flash\n");
					bolt_close(fh);
					return BOLT_ERR_IOERR;
				}
				xprintf("done\n");
			}
		}
		if (revend) {
			uint16_t *cptr = (uint16_t *)ptr;
			unsigned int i;

			for (i = 0; i < copysize; i += 2, cptr++)
				*cptr = swap16(*cptr);
		}
		if (revend1) {
			uint32_t *cptr = (uint32_t *)ptr;
			unsigned int i;

			for (i = 0; i < copysize; i += 4, cptr++)
				*cptr = swap32(*cptr);
		}
	}
	/*
	 * Program the flash
	 */

	xprintf("Programming...");

	amtcopy = bolt_writeblk(fh, offset, ptr, copysize);

	if (copysize == amtcopy) {
		xprintf("done. %d bytes written\n", amtcopy);
		res = 0;
	} else {
		ui_showerror(amtcopy, "Failed.");
		res = BOLT_ERR_IOERR;
	}

	/*
	 * done!
	 */
	bolt_close(fh);

	return res;
}


static int ui_cmd_boards(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct fsbl_info *inf = board_info();
	struct board_type *b;
	const char *c, *s;
	unsigned int i;
	uint32_t hardflags;

	if (!inf)
		goto out;

	b = inf->board_types;
	if (!b)
		goto out;

	for (i = 0; i < FSBLINFO_N_BOARDS(inf->n_boards); i++) {
		c = b[i].name;
		xprintf("%c)\t%s %s %s%s\n", board_idx_to_char(i),
			(AVS_ENABLE(b[i].avs) ? "AVS  " : "noavs"),
			c,
			(i == inf->board_idx) ? "*" : " ",
			(i == inf->saved_board.board_idx) ? "S" : " ");
	}

	xprintf(" FSBL info version %d, %d boards.\n",
		FSBLINFO_VERSION(inf->n_boards),
		FSBLINFO_N_BOARDS(inf->n_boards));

	hardflags = (inf->saved_board.hardflags	>>
			FSBL_HARDFLAG_AVS_SHIFT) &
			FSBL_HARDFLAG_AVS_MASK;

	switch (hardflags) {
	case FSBL_HARDFLAG_AVS_BOARD:
		s = "is using board defaults";
		break;
	case FSBL_HARDFLAG_AVS_ON:
		s = "has been overridden ON";
		break;
	case FSBL_HARDFLAG_AVS_OFF:
		s = "has been overridden OFF";
		break;
	default:
		s = "defaults unknown!";
		break;
	}
	xprintf(" AVS %s, load/run status: %x\n", s, inf->avs_err);

#if defined(DVFS_SUPPORT) && !defined(SECURE_BOOT)
	if(dvfs_show_board_config(b, inf))
		return BOLT_ERR;
#endif

	if (CFG_MONITOR_OVERTEMP)
		xprintf(" Overtemp park check %sabled\n",
			(inf->saved_board.hardflags &
			FSBL_HARDFLAG_OTPARK_MASK) ? "dis" : "en");
 out:
	return BOLT_OK;
}


#if !defined(SECURE_BOOT)
static int ui_cmd_setboard(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *s;
	unsigned int i;
	int rc, force = 0, forget = 0;
	struct fsbl_info *inf = board_info();
	uint32_t hf, old;

	s = cmd_getarg(cmd, 0);

	if (!inf)
		return ui_showusage(cmd);

	if (cmd_sw_isset(cmd, "-forget"))
		forget = 1;

	if (s) {
		if (strlen(s) > 1)
			return BOLT_ERR_INV_PARAM;

		i = board_char_to_idx(s[0]);
		if (i >= FSBLINFO_N_BOARDS(inf->n_boards))
			return BOLT_ERR_INV_PARAM;

		if (forget) {
			err_msg("Cannot mix board selection with -forget flag");
			return BOLT_ERR_INV_PARAM;
		}

		if (inf->board_idx != i)
			force = 1;

		inf->board_idx = i;
	} else {
		if (forget)
			inf->board_idx = CFG_BOARDDEFAULT - 1;
	}

	old = inf->saved_board.hardflags;

	if (cmd_sw_isset(cmd, "-avs"))
		hf = FSBL_HARDFLAG_AVS_ON;
	else if (cmd_sw_isset(cmd, "-noavs"))
		hf = FSBL_HARDFLAG_AVS_OFF;
	else if (cmd_sw_isset(cmd, "-bavs"))
		hf = FSBL_HARDFLAG_AVS_BOARD;
	else
		hf = (old >> FSBL_HARDFLAG_AVS_SHIFT) & FSBL_HARDFLAG_AVS_MASK;

	hf = (old & ~(FSBL_HARDFLAG_AVS_MASK << FSBL_HARDFLAG_AVS_SHIFT)) |
		(hf << FSBL_HARDFLAG_AVS_SHIFT);

	if (cmd_sw_isset(cmd, "-park"))
		hf &= ~FSBL_HARDFLAG_OTPARK_MASK;

	if (cmd_sw_isset(cmd, "-nopark"))
		hf |= FSBL_HARDFLAG_OTPARK_MASK;

	if (hf != old) {
		inf->saved_board.hardflags = hf;
		force = 1;
	}

	if (cmd_sw_isset(cmd, "-force"))
		force = 1;

	rc = board_check(force);
	if (!rc)
		inf->saved_board.board_idx = inf->board_idx;

	return rc;
}

#if defined(DVFS_SUPPORT)
static unsigned int dvfs_show_board_config(struct board_type *b, struct fsbl_info *inf)
{
	char *ds;
	unsigned int pmap_id;
	const struct dvfs_params *dvfs = board_dvfs();

	if (!dvfs)
		return BOLT_ERR_INV_PARAM;

	b = board_thisboard();
	xprintf(" AVS domains: %d\n", AVS_DOMAINS(b->avs));

	switch (dvfs->mode) {
	case avs_mode_e:
		ds = "avs";
		break;
	case dfs_mode_e:
		ds = "dfs";
		break;
	case dvfs_mode_e:
		ds = "dvfs";
		break;
	default:
		ds = "?";
		break;
	}

	pmap_id = FSBL_HARDFLAG_PMAP_ID(inf->saved_board.hardflags);
	if (pmap_id == FSBL_HARDFLAG_PMAP_BOARD) {
		/* respect the board default PMap configuration */
		pmap_id = dvfs->pmap;
	}
	xprintf(" DVFS mode: %s (%d), ", ds, dvfs->mode);
	xprintf(" pmap: %d, ", pmap_id);
	xprintf(" pstate: %d\n", dvfs->pstate);
	return BOLT_OK;
}

static int ui_cmd_pmap(ui_cmdline_t *cmd, int argc, char *argv[])
{
	bool set_pmap = false, clear_pmap = false;
	int pmap_id_old, pmap_id_new, index;
	const char *s;
	struct fsbl_info *inf = board_info();
	uint32_t hf_new, hf_old;
	struct board_type *b;
	unsigned int num_domains_board, num_domains_pmap;

	if (!inf)
		return ui_showusage(cmd);

	b = get_board_type(inf); /* no need to sanity check */
	num_domains_board = AVS_DOMAINS(b->avs);

	if (cmd_sw_value(cmd, "-set", &s))
		set_pmap = true;

	if (cmd_sw_isset(cmd, "-clear"))
		clear_pmap = true;

	if (set_pmap && clear_pmap) {
		xprintf("cannot set and clear PMap at the same time!\n");
		return BOLT_ERR;
	}

	if (!set_pmap && !clear_pmap) {
		xprintf("Either set or clear PMap option needs to be sppecified.\n");
		return BOLT_ERR_INV_PARAM;
	}

	pmap_id_old = board_pmap();
	if (is_pmap_valid(pmap_id_old))
		xprintf("Stored PMap ID %d is invalid.\n", pmap_id_old);

	if (set_pmap) {
		pmap_id_new = atoi(s);
		if (!is_pmap_valid(pmap_id_new)) {
			index = board_pmap_index(pmap_id_new);
			num_domains_pmap = pmapTable[index].num_domains;
			if (num_domains_pmap != 0 &&
					num_domains_board != num_domains_pmap) {
				err_msg("PMap ID %d cannot be for this board.",
						pmap_id_new);
				return BOLT_ERR;
			}
		} else {
			return BOLT_ERR;
		}
	} else if (clear_pmap) {
		pmap_id_new = -1;
	} else { /* nobody expects the unexpected! */
		return BOLT_ERR;
	};

	hf_old = inf->saved_board.hardflags;
	hf_new = hf_old;
	hf_new &= ~(FSBL_HARDFLAG_PMAP_MASK << FSBL_HARDFLAG_PMAP_SHIFT);
	hf_new |= pmap_id_new << FSBL_HARDFLAG_PMAP_SHIFT;
	if (hf_new != hf_old) {
		inf->saved_board.hardflags = hf_new;
		board_check(1);
	}

	return BOLT_OK;
}
#endif
#endif

static int ui_cmd_rts(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;
	bool do_set = false, do_clear;
	int id = -1;
	const char *s;

	if (cmd_sw_value(cmd, "-set", &s))
		do_set = true;

	do_clear = cmd_sw_isset(cmd, "-clear");

	if (do_set) {
		if (do_clear) {
			xprintf("cannot set and clear RTS at the same time!\n");
			return BOLT_ERR_INV_COMMAND;
		} else {
			id = atoi(s);
			if (id < 1) {
				err_msg("cannot set %d, must be greater than "
					"0 (zero)", id);
				return BOLT_ERR_INV_PARAM;
			}
/* This is not a recommended way of selecting a BOX mode. Instead,
 * please try the B_REFSW_BOXMODE environment variable in Linux.
 * For example:
 *     export B_REFSW_BOXMODE=%d
 */
			warn_msg("This is not a recommended way of selecting "
				"a BOX mode. Instead,\n"
				"please try the B_REFSW_BOXMODE environment "
				"variable in Linux.\n"
				"For example:");
			warn_msg("    export B_REFSW_BOXMODE=%d", id);
		}
	} else {
		if (do_clear) {
			id = -1; /* -1 means "clear the current one" */
		} else {
			/* not set and not clear, display current box mode */
			xprintf("BOX MODE: %d\n",
				board_init_rts_current_boxmode());
			return BOLT_OK;
		}
	}

	rc = board_init_rts_update(id);
	if (rc < 0)
		return rc;

	return BOLT_OK;
}


static int ui_cmd_gisb(ui_cmdline_t *cmd, int argc, char *argv[])
{
	const char *s;
	uint32_t value;

	if (cmd_sw_value(cmd, "-set", &s)) {
		value = atoi(s);
		bolt_gisb_write(value);
	}

	xprintf("GISB timeout: %u\n", bolt_gisb_read());
	return BOLT_OK;
}


/* SWBOLT-88 */
static int ui_cmd_erase(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *flashdev, *err = NULL;
	int fh, rc = 0;
	flash_sector_t sec;
	flash_range_t fr;
	struct flash_info fi;
	const char *x;

	flashdev = cmd_getarg(cmd, 0);
	if (!flashdev)
		return ui_showerror(BOLT_ERR_INV_PARAM, "missing device name");

	/* Make sure it's a flash device.
	*/
	rc = bolt_getdevinfo(flashdev);
	if (rc < 0)
		return ui_showerror(BOLT_ERR_DEVNOTFOUND, flashdev);

	rc &= BOLT_DEV_MASK;

	if ((rc != BOLT_DEV_FLASH) && (rc != BOLT_DEV_NVRAM)) {
		xprintf("Device '%s' is not a flash or eeprom device.\n",
			flashdev);
		return BOLT_ERR_INV_PARAM;
	}

	if (cmd_sw_value(cmd, "-offset", &x))
		fr.range_base = xtoq(x);
	else
		fr.range_base   = 0;

	if (cmd_sw_value(cmd, "-size", &x))
		fr.range_length = xtoq(x);
	else
		fr.range_length = 0;

	fh = bolt_open(flashdev);
	if (fh < 0)
		return ui_showerror(fh, flashdev);

	/* Do the erase	*/
	rc = bolt_ioctl(fh, IOCTL_FLASH_GETPARTINFO, &fi, sizeof(fi), NULL, 0);
	if (rc) {
		err = "IOCTL_FLASH_GETPARTINFO";
		goto out;
	}
#if (CFG_CMD_LEVEL >= 5)
	printf("PARTINFO: base %llx, size %llx\n",
			fi.flash_base, fi.flash_size);
#endif
	sec.flash_sector_idx = 0;

	rc = bolt_ioctl(fh, IOCTL_FLASH_GETSECTORS, &sec, sizeof(sec), NULL, 0);
	if (rc) {
		err = "IOCTL_FLASH_ERASE_RANGE";
		goto out;
	}

	if (sec.flash_sector_status == FLASH_SECTOR_INVALID) {
		rc = BOLT_ERR_IOERR;
		err = "sector is invalid";
		goto out;
	}

	if (!IS_ALIGNED(fi.flash_base, sec.flash_sector_size) ||
	    !IS_ALIGNED(fi.flash_size, sec.flash_sector_size)) {
#if (CFG_CMD_LEVEL >= 5)
		xprintf("Alignment problem: partition (base %llx + size %llx)\n"
			"not aligned to erase sector size %x\n",
			fi.flash_base, fi.flash_size,
			sec.flash_sector_size);
#endif
		rc = BOLT_ERR_IOERR;
		err = "bad alignment";
		goto out;
	}

	if (fr.range_length == 0 || fr.range_length > fi.flash_size)
		fr.range_length = fi.flash_size - fr.range_base;

	if (!IS_ALIGNED(fr.range_base, sec.flash_sector_size) ||
	    !IS_ALIGNED(fr.range_length, sec.flash_sector_size)) {
		rc = BOLT_ERR_IOERR;
		err = "range alignment";
		goto out;
	}

	xprintf("Erasing %llx to %llx...", fr.range_base,
		fr.range_base + fr.range_length);

	rc = bolt_ioctl(fh, IOCTL_FLASH_ERASE_RANGE, &fr, sizeof(fr), NULL, 0);
	if (rc) {
		err = "IOCTL_FLASH_ERASE_RANGE";
		goto out;
	}
	xprintf("done\n");

out:
	bolt_close(fh);

	return (rc) ? ui_showerror(rc, err) : 0;
}
