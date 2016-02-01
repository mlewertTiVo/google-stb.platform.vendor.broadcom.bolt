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
#include "devfuncs.h"
#include "ioctl.h"

#include "error.h"
#include "env_subr.h"
#include "nvram_subr.h"
#include "bolt.h"

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

static int nvram_handle = -1;
static nvram_info_t nvram_info;
static char *nvram_devname = NULL;

/*  *********************************************************************
    *  nvram_getinfo(info)
    *
    *  Obtain information about the NVRAM device from the device
    *  driver.  A flash device might only dedicate a single sector
    *  to the environment, so we need to ask the driver first.
    *
    *  Input parameters:
    *      info - nvram info
    *
    *  Return value:
    *      0 if ok
    *      <0 = error code
    ********************************************************************* */

static int nvram_getinfo(nvram_info_t *info)
{
	if (nvram_handle == -1)
		return -1;

	bolt_ioctl(nvram_handle, IOCTL_NVRAM_UNLOCK, NULL, 0, NULL, 0);

	if (bolt_ioctl(nvram_handle, IOCTL_NVRAM_GETINFO,
		      info, sizeof(*info), NULL, 0) != 0) {
		return -1;
	}

	return 0;

}

/*  *********************************************************************
    *  nvram_open()
    *
    *  Open the default NVRAM device and get the information from the
    *  device driver.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int nvram_open(void)
{
	if (nvram_handle != -1)
		nvram_close();

	if (nvram_devname == NULL)
		return BOLT_ERR_DEVNOTFOUND;

	nvram_handle = bolt_open(nvram_devname);

	if (nvram_handle < 0)
		return BOLT_ERR_DEVNOTFOUND;

	if (nvram_getinfo(&nvram_info) < 0) {
		nvram_close();
		return BOLT_ERR_IOERR;
	}

	return 0;
}

/*  *********************************************************************
    *  nvram_close()
    *
    *  Close the NVRAM device
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      0
    ********************************************************************* */

int nvram_close(void)
{
	if (nvram_handle != -1) {
		bolt_close(nvram_handle);
		nvram_handle = -1;
	}

	return 0;
}

/*  *********************************************************************
    *  nvram_getsize()
    *
    *  Return the total size of the NVRAM device.  Note that
    *  this is the total size that is used for the NVRAM functions,
    *  not the size of the underlying media.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      size.  <0 if error
    ********************************************************************* */

int nvram_getsize(void)
{
	return ((nvram_handle < 0) ? 0 : 2048); /* was: ACTUAL_NVRAM_SIZE */
}

/*  *********************************************************************
    *  nvram_read(buffer,offset,length)
    *
    *  Read data from the NVRAM device
    *
    *  Input parameters:
    *      buffer - destination buffer
    *      offset - offset of data to read
    *      length - number of bytes to read
    *
    *  Return value:
    *      number of bytes read, or <0 if error occured
    ********************************************************************* */
int nvram_read(unsigned char *buffer, int offset, int length)
{
	if (nvram_handle == -1)
		return -1;

	return bolt_readblk(nvram_handle,
			   (bolt_offset_t) (offset + nvram_info.nvram_offset),
			   buffer, length);
}

/*  *********************************************************************
    *  nvram_write(buffer,offset,length)
    *
    *  Write data to the NVRAM device
    *
    *  Input parameters:
    *      buffer - source buffer
    *      offset - offset of data to write
    *      length - number of bytes to write
    *
    *  Return value:
    *      number of bytes written, or -1 if error occured
    ********************************************************************* */
int nvram_write(unsigned char *buffer, int offset, int length)
{
	if (nvram_handle == -1)
		return -1;

	return bolt_writeblk(nvram_handle,
			    (bolt_offset_t) (offset + nvram_info.nvram_offset),
			    buffer, length);
}

/*  *********************************************************************
    *  nvram_erase()
    *
    *  Erase the NVRAM device.  Not all devices need to be erased,
    *  but flash memory does.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int nvram_erase(void)
{
	if (nvram_handle < 0)
		return -1;

	if (nvram_info.nvram_eraseflg == FALSE)
		return 0;

	if (bolt_ioctl(nvram_handle, IOCTL_NVRAM_ERASE,
		      &nvram_info, sizeof(nvram_info), NULL, 0) != 0)
		return -1;

	return 0;
}

/*  *********************************************************************
    *  bolt_set_envdevice(name)
    *
    *  Set the environment NVRAM device name
    *
    *  Input parameters:
    *      name - name of device to use for NVRAM
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int bolt_set_envdevice(char *name)
{
	int res;

	if (nvram_devname) {
		printf("nvram_devname not NULL\n");
		return -1;
	}

	nvram_devname = strdup(name);
	res = nvram_open();
	if (res != 0) {
		xprintf("!! Could not open NVRAM device %s\n", nvram_devname);
		return res;
	}
	nvram_close();
	res = env_load();
	return res;
}


void bolt_set_envdevname(char *name)
{
	if (nvram_devname) {
		printf("nvram_devname not NULL\n");
		KFREE(nvram_devname);
	}
	nvram_devname = strdup(name);
}

void bolt_free_envdevname(void)
{
	if (nvram_devname) {
		KFREE(nvram_devname);
		nvram_devname = NULL;
	}
}
