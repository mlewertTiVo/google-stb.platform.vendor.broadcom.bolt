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
#include "devfuncs.h"
#include "timer.h"
#include "ioctl.h"

#include "error.h"

#include "ui_command.h"
#include "ui_init.h"

static int ui_cmd_flashtest(ui_cmdline_t *cmd, int argc, char *argv[]);
#if 0
/* FIXME: remove unused functions? */
static int ui_cmd_readnvram(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_erasenvram(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

int ui_init_flashtestcmds(void)
{
	cmd_addcmd("show flash",
		   ui_cmd_flashtest,
		   NULL,
		   "Display information about a flash device.",
		   "show flash [-sectors]",
		   "-sectors;Display sector information");

#if 0
	cmd_addcmd("nvram read",
		   ui_cmd_readnvram,
		   NULL, "read the NVRAM", "test nvram devname offset", "");

	cmd_addcmd("nvram erase",
		   ui_cmd_erasenvram,
		   NULL, "erase the NVRAM", "erasenvram devname", "-pattern");
#endif

	return 0;
}

static char *flashtypes[] = {
	"Unknown", "SRAM", "ROM", "Flash"
};

static int ui_cmd_flashtest(ui_cmdline_t *cmd, int argc, char *argv[])
{
	struct flash_info info;
	int fd;
	int res = 0;
	int idx;
	flash_sector_t sector;
	nvram_info_t nvraminfo;
	char *devname;
	int showsectors;

	devname = cmd_getarg(cmd, 0);
	if (!devname)
		return ui_showusage(cmd);

	showsectors = cmd_sw_isset(cmd, "-sectors");

	fd = bolt_open(devname);
	if (fd < 0) {
		ui_showerror(fd, "Could not open flash device %s", devname);
		return fd;
	}

	res = bolt_ioctl(fd, IOCTL_FLASH_GETINFO, &info,
		      sizeof(info), NULL, 0);
	if (res == 0) {
		printf
		    ("FLASH: Base %016llX size %08llX type %02X(%s) flags %08X\n",
		     info.flash_base, info.flash_size, info.type,
		     flashtypes[info.type], info.flags);
	} else {
		printf("FLASH: Could not determine flash information\n");
	}

	res = bolt_ioctl(fd, IOCTL_NVRAM_GETINFO, &nvraminfo,
		      sizeof(nvraminfo), NULL, 0);
	if (res == 0) {
		printf("NVRAM: Offset %08lX Size %08lX EraseFlg %d\n",
		       nvraminfo.nvram_offset, nvraminfo.nvram_size,
		       nvraminfo.nvram_eraseflg);
	} else {
		printf("NVRAM: Not supported by this flash\n");
	}

	if (showsectors && info.type != FLASH_TYPE_UNKNOWN) {
		printf("Flash sector information:\n");

		idx = 0;
		for (;;) {
			sector.flash_sector_idx = idx;
			res = bolt_ioctl(fd, IOCTL_FLASH_GETSECTORS,
				      &sector, sizeof(sector), NULL, 0);
			if (res != 0) {
				printf("ioctl error\n");
				break;
			}
			if (sector.flash_sector_status == FLASH_SECTOR_INVALID)
				break;
			printf("  Sector %d offset %08X size %d\n",
			       sector.flash_sector_idx,
			       sector.flash_sector_offset,
			       sector.flash_sector_size);
			idx++;
		}
	}

	bolt_close(fd);
	return 0;

}

#if 0
static int ui_cmd_readnvram(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *dev;
	char *tok;
	int fd;
	int offset = 0;
	int res;
	uint8_t buf[512];
	int idx;

	dev = cmd_getarg(cmd, 0);
	if (!dev)
		return ui_showusage(cmd);

	tok = cmd_getarg(cmd, 1);
	if (tok)
		offset = xtoi(tok);
	else
		offset = 0;

	fd = bolt_open(dev);
	if (fd < 0) {
		ui_showerror(fd, "could not open NVRAM");
		return fd;
	}

	res = bolt_readblk(fd, offset, buf, 512);
	printf("Offset %d Result %d\n", offset, res);
	for (idx = 0; idx < 512; idx++) {
		if ((idx % 16) == 0)
			printf("\n");
		printf("%02X ", buf[idx]);
	}
	printf("\n");

	bolt_close(fd);
	return 0;

}

static int ui_cmd_erasenvram(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *dev;
	int fd;
	uint8_t buffer[2048];
	int res;
	char *tok;
	int offset;
	int length;
	uint8_t data;

	dev = cmd_getarg(cmd, 0);
	if (!dev)
		return ui_showusage(cmd);

	offset = 0;
	tok = cmd_getarg(cmd, 1);
	if (tok)
		offset = xtoi(tok);
	length = 512;

	tok = cmd_getarg(cmd, 2);
	if (tok)
		length = xtoi(tok);
	if (length > 2048)
		length = 2048;

	data = 0xFF;
	tok = cmd_getarg(cmd, 3);
	if (tok)
		data = xtoi(tok);

	fd = bolt_open(dev);
	if (fd < 0) {
		ui_showerror(fd, "could not open NVRAM");
		return fd;
	}

	if (cmd_sw_isset(cmd, "-pattern")) {
		memset(buffer, 0, sizeof(buffer));
		for (res = 0; res < 2048; res++)
			buffer[res] = res & 0xFF;
	} else {
		memset(buffer, data, sizeof(buffer));
	}

	printf("Fill offset %04X length %04X\n", offset, length);

	res = bolt_writeblk(fd, offset, buffer, length);

	printf("write returned %d\n", res);

	bolt_close(fd);
	return 0;

}
#endif
