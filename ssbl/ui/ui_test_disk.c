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
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "console.h"
#include "error.h"
#include "ioctl.h"
#include "devfuncs.h"
#include "ui_command.h"
#include "bolt.h"
#include "env_subr.h"

#include "fileops.h"
#include "bootblock.h"
#include "boot.h"


#if CFG_FATFS
#if (CFG_CMD_LEVEL >= 2)
static int ui_cmd_dir(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
#if (CFG_CMD_LEVEL >= 5)
static int ui_cmd_copydisk(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_bootblock(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif
#endif /* CFG_FATFS */

int ui_init_disktestcmds(void)
{
#if CFG_FATFS
#if (CFG_CMD_LEVEL >= 2)
	cmd_addcmd("dir", ui_cmd_dir, NULL,
		   "List the directory of a FAT file system",
		   "dir device-name[:path][filename]", "");
#endif

#if (CFG_CMD_LEVEL >= 5)
	cmd_addcmd("copydisk", ui_cmd_copydisk, NULL,
		   "Copy a remote disk image to a local disk device via TFTP",
		   "copydisk host:filename device-name [offset]", "");

	cmd_addcmd("show boot", ui_cmd_bootblock, NULL,
		   "Display boot block from device,",
		   "show boot device-name\n\n"
		   "This command displays the boot block on the specified device.  The\n"
		   "device-name parameter identifies a block device (disk, tape, CD-ROM)\n"
		   "to be scanned for boot blocks.  The first boot block found will be\n"
		   "displayed.", "");
#endif
#endif /* CFG_FATFS */
	return 0;
}

#if CFG_FATFS

#if (CFG_CMD_LEVEL >= 2)
static int ui_cmd_dir(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *tok;
	char *fname;
	fileio_ctx_t *fsctx;
	int res;

	tok = cmd_getarg(cmd, 0);
	if (!tok)
		return -1;

	fname = strchr(tok, ':');
	if (fname) {
		*fname = 0;  /* terminate drive name string */
		fname = fname + 1;  /* point to start of dir or file name */
		while (*fname) {  /* remove leading separators */
			if ((*fname != '\\') && (*fname != '/'))
				break;
			++fname;
		}
		if (*fname == 0)
			fname = 0;	/* no directory or file name */
	}

	env_setenv("USBMASS_INFO", "1", ENV_FLG_BUILTIN);
	res = fs_init("fat", &fsctx, tok);
	env_delenv("USBMASS_INFO");
	if (res < 0) {
		xprintf("dir: Could not init file system: %s\n",
			bolt_errortext(res));
		return res;
	}

	res = fs_dir(fsctx, fname);
	fs_uninit(fsctx);
	if (res < 0) {
		xprintf("dir: %s\n", bolt_errortext(res));
		return res;
	}

	return !res;
}
#endif

#if (CFG_CMD_LEVEL >= 5)
static int ui_cmd_bootblock(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int fh;
	char *tok;
	struct boot_block bootblock;
	int res;
	int idx;
	int sec;
	uint32_t checksum;
	uint32_t checksumd;
	uint32_t calcsum;
	uint32_t secsize;
	uint64_t secoffset;
	uint8_t *code;

	tok = cmd_getarg(cmd, 0);
	if (!tok)
		return -1;

	fh = bolt_open(tok);
	if (fh < 0) {
		xprintf("Could not open device; %d\n", fh);
		return -1;
	}
	for (sec = 0; sec < BOOT_BLOCK_MAXLOC; sec++) {
		res = bolt_readblk(fh, sec * BOOT_BLOCK_BLOCKSIZE,
				  (unsigned char *)&bootblock,
				  sizeof(bootblock));

		if (bootblock.bb_magic != BOOT_MAGIC_NUMBER)
			continue;
		xprintf("Found boot block in sector %d\n", sec);
		if (res != sizeof(bootblock)) {
			xprintf("Could not read boot block\n");
			bolt_close(fh);
			return -1;
		}

		xprintf("Boot block data:\n");
		for (idx = 59; idx < 64; idx++)
			xprintf("  %d: %016llX\n", idx, bootblock.bb_data[idx]);
		xprintf("\n");

		xprintf("Boot block version is %d\n",
			(uint32_t) ((bootblock.bb_hdrinfo & BOOT_HDR_VER_MASK)
				    >> BOOT_HDR_VER_SHIFT));
		xprintf("Boot block flags are %02X\n",
			(uint32_t) ((bootblock.bb_hdrinfo & BOOT_HDR_FLAGS_MASK)
				    >> 56));
		checksum = ((uint32_t)
			    (bootblock.bb_hdrinfo & BOOT_HDR_CHECKSUM_MASK));
		checksumd = ((uint32_t)
			     ((bootblock.bb_secsize & BOOT_DATA_CHECKSUM_MASK)
			      >> BOOT_DATA_CHECKSUM_SHIFT));
		bootblock.bb_hdrinfo &= ~BOOT_HDR_CHECKSUM_MASK;
		secsize =
		    ((uint32_t) (bootblock.bb_secsize & BOOT_SECSIZE_MASK));
		secoffset = bootblock.bb_secstart;

		xprintf("Boot code is %d bytes at %016llX\n", secsize,
			secoffset);

		CHECKSUM_BOOT_DATA(&(bootblock.bb_magic), BOOT_BLOCK_SIZE,
				   &calcsum);

		if (checksum != calcsum) {
			xprintf
			    ("Header checksum does not match Blk=%08X Calc=%08X\n",
			     checksum, calcsum);
		} else {
			xprintf("Header checksum is ok\n");
		}

		code = KMALLOC(secsize, 0);
		if (code) {
			res = bolt_readblk(fh, secoffset, code, secsize);
			if ((uint32_t)res != secsize) {
				xprintf("Could not read boot code\n");
				bolt_close(fh);
				KFREE(code);
				return -1;
			}
			CHECKSUM_BOOT_DATA(code, secsize, &calcsum);
			if (calcsum == checksumd)
				xprintf("Boot code checksum is ok\n");
			else
				xprintf
				    ("Boot code checksum is incorrect (Calc=%08X, Blk=%08X)\n",
				     calcsum, checksumd);
			KFREE(code);
		}
		break;
	}
	if (sec == BOOT_BLOCK_MAXLOC) {
		xprintf("No valid boot blocks found in the first %d sectors\n",
			BOOT_BLOCK_MAXLOC);
	}
	bolt_close(fh);

	return 0;
}

static int ui_cmd_copydisk(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *fname;
	fileio_ctx_t *fsctx;
	void *filectx;
	char *devname;
	uint8_t buffer[1024];
	int fh;
	int res;
	int total;
	int count;
	int offset;
	char *toffset;

	fname = cmd_getarg(cmd, 0);
	if (!fname)
		return ui_showusage(cmd);

	devname = cmd_getarg(cmd, 1);
	if (!devname)
		return ui_showusage(cmd);

	toffset = cmd_getarg(cmd, 2);
	if (!toffset)
		offset = 0;
	else
		offset = atoi(toffset);

	if ((bolt_getdevinfo(devname) & BOLT_DEV_MASK) != BOLT_DEV_DISK) {
		xprintf("Device %s is not a disk.\n", devname);
		return BOLT_ERR_INV_PARAM;
	}

	fh = bolt_open(devname);
	if (fh < 0)
		return ui_showerror(fh, "Could not open device %s", devname);

	res = fs_init("tftp", &fsctx, "");
	if (res < 0)
		return ui_showerror(res, "Could not init file system");

	res = fs_open(fsctx, &filectx, fname, FILE_MODE_READ);
	if (res < 0) {
		return ui_showerror(res, "Could not open %s", fname);
	} else {
		total = 0;
		count = 0;
		for (;;) {
			res = fs_read(fsctx, filectx, buffer, sizeof(buffer));
			if (res < 0)
				break;
			if (res > 0)
				bolt_writeblk(fh, total + offset * 512, buffer,
					     res);
			total += res;
			if (res != sizeof(buffer))
				break;
			count++;
			if (count == 256) {
				xprintf(".");
				count = 0;
			}
		}
		if (res < 0)
			xprintf("read error %s\n", bolt_errortext(res));
		else
			xprintf("Total bytes read: %d\n", total);
		fs_close(fsctx, filectx);
	}

	fs_uninit(fsctx);
	bolt_close(fh);
	return 0;
}
#endif

#endif /* CFG_FATFS */
